public botThink( id )
{
	if(g_BotData[paused]) return HAM_IGNORED;
	if(!g_iSourcesCount) return HAM_IGNORED;
	if(!is_user_alive(id)) return HAM_IGNORED;
	if(!g_BotData[spectators]) return HAM_IGNORED;

	g_BotData[current_frame]++;

	if ( g_BotData[current_frame] < g_BotData[current_source_size]  )
	{
		ArrayGetArray(g_BotSources[g_BotData[current_source]][source_array], g_BotData[current_frame], g_BotData[current_frame_data]);
		if(g_BotData[current_frame]+1 < g_BotData[current_source_size] ){
			ArrayGetArray(g_BotSources[g_BotData[current_source]][source_array], g_BotData[current_frame]+1, g_BotData[next_frame_data]);
		}
		g_BotData[deltatime] = 0.0;
		g_BotData[current_frame_data][frame_angles][0] /= -3.0;
		g_BotData[next_frame_data][frame_angles][0] /= -3.0;

		if(g_BotData[current_frame_data][frame_gravity] != g_BotData[gravity]){
			if(g_BotData[current_frame_data][frame_gravity] == 1.0){
				engclient_cmd(id, "weapon_usp");
			}
			else{
				engclient_cmd(id, "weapon_knife")
				set_pev(id, pev_viewmodel2, "models/llg/v_butcher.mdl");
			}
		}
		
		new bool:onground = g_BotData[current_frame_data][frame_velocity][2] == 0.0 ? true : false
		animate_legs(id, g_BotData[current_frame_data][frame_buttons], onground);
		entity_set_int(id, EV_INT_button, g_BotData[current_frame_data][frame_buttons]);
		set_user_fps(id, g_BotData[current_frame_data][frame_fps]);
		
		if(g_BotData[current_frame_data][frame_sync])
			display_stats(id, g_BotData[current_frame_data][frame_strafes], g_BotData[current_frame_data][frame_sync]);

		g_BotData[gravity] = g_BotData[current_frame_data][frame_gravity];
	}
	else  
	{
		if(g_BotData[current_counter] < get_pcvar_num(g_cRepeatReplay))
		{
			g_BotData[current_frame] = 0;
			g_BotData[current_counter]++;

			return HAM_IGNORED;
		}

		if(g_BotData[current_source] + 1 >= g_iSourcesCount)
			g_BotData[current_source] = 0;
		else
			g_BotData[current_source]++;

		g_BotData[current_source_size] = ArraySize(g_BotSources[g_BotData[current_source]][source_array]) - 1;

		g_BotData[current_frame] = 0;
		g_BotData[current_counter] = 0;

		g_BotData[frametime_def] = 1.0 / ((g_BotData[current_source_size] - g_iFrmsBeforeStart )/ str_to_time(g_BotSources[g_BotData[current_source]][source_time]));

		g_BotData[frametime] = g_BotData[frametime_def];

		for(new i = 1;i<33;i++)
			update_sourcename(i);
	}

	return HAM_IGNORED;
}


public botSmooth(id){
	if(g_BotData[paused]) return HAM_IGNORED;
	if(!g_iSourcesCount) return HAM_IGNORED;
	if(!is_user_alive(id)) return HAM_IGNORED;
	if(!g_BotData[spectators]) return HAM_IGNORED;
	if(g_BotData[deltatime] + 0.2 >= 1.0) return HAM_IGNORED;
	
	if(g_BotData[current_frame]+1 < g_BotData[current_source_size]){

		new Float:angles[3];
		new Float:origin[3];
		new Float:oldorigin[3];
		new Float:neworigin[3];

		oldorigin[0] = g_BotData[current_frame_data][frame_origin][0];
		oldorigin[1] = g_BotData[current_frame_data][frame_origin][1];
		oldorigin[2] = g_BotData[current_frame_data][frame_origin][2];

		neworigin[0] = g_BotData[next_frame_data][frame_origin][0];
		neworigin[1] = g_BotData[next_frame_data][frame_origin][1];
		neworigin[2] = g_BotData[next_frame_data][frame_origin][2];

		angles[0] = lerp(g_BotData[current_frame_data][frame_angles][0], g_BotData[next_frame_data][frame_angles][0], g_BotData[deltatime]);
		angles[1] = slerp_angles(g_BotData[current_frame_data][frame_angles][1], g_BotData[next_frame_data][frame_angles][1], g_BotData[deltatime]);
		angles[2] = lerp(g_BotData[current_frame_data][frame_angles][2], g_BotData[next_frame_data][frame_angles][2], g_BotData[deltatime]);

		origin[0] = lerp(oldorigin[0], neworigin[0], g_BotData[deltatime]);
		origin[1] = lerp(oldorigin[1], neworigin[1], g_BotData[deltatime]);
		origin[2] = lerp(oldorigin[2], neworigin[2], g_BotData[deltatime]);

		SetUserAgl(id, angles);

		set_pev( id, pev_movetype, MOVETYPE_NONE);
		set_pev(id, pev_solid, SOLID_NOT)
		set_pev( id, pev_velocity, g_BotData[current_frame_data][frame_velocity] );

		entity_set_origin(id, origin);

		g_BotData[deltatime]+= 0.2;
	}

	return HAM_IGNORED;
}

public fw_updateclientdata(id, sendweapons, cd_handle )
{	
	if(id == g_iBot)
	return FMRES_IGNORED;
	
	new ent = pev(id, pev_iuser2);
	if(!ent)
	return FMRES_IGNORED;
	
	if((g_iBot == ent && g_iSourcesCount) && (g_BotData[current_source_size] > g_BotData[current_frame]))
	{
		if(g_BotData[current_frame_data][frame_buttons] & IN_DUCK)		
			set_cd(cd_handle, CD_ViewOfs, Float:{0.0, 0.0, 12.0});
		else
			set_cd(cd_handle, CD_ViewOfs, Float:{0.0, 0.0, 17.0});
	}
	return FMRES_IGNORED;
}