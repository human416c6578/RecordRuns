new currFrame[eFrame];
new nextFrame[eFrame];
new Float:deltaTime = 0.0;

public Float:botThink( id )
{
	if(g_BotData[paused]) return 0.1;
	if(ArraySize(g_Replays) == 0) return 0.1;
	if(!is_user_alive(id)) return 0.1;
	if(!g_BotData[spectators]) return 0.1;

	g_BotData[current_frame]++;

	if ( g_BotData[current_frame] < GetReplaySize() )
	{
		GetFrame(g_BotData[current_frame], currFrame);
		if(g_BotData[current_frame] + 1 < GetReplaySize() ){
			GetFrame(g_BotData[current_frame] + 1, nextFrame);
		}
		deltaTime = 0.0;
		currFrame[fAngles][0] /= -3.0;
		nextFrame[fAngles][0] /= -3.0;

		if(currFrame[fGravity] != g_BotData[gravity]){
			if(currFrame[fGravity] == 1.0){
				engclient_cmd(id, "weapon_usp");
			}
			else{
				engclient_cmd(id, "weapon_knife")
				set_pev(id, pev_viewmodel2, "models/llg/v_butcher.mdl");
			}
		}
		
		animate_legs(id, currFrame[fKeys], bool:currFrame[fGrounded]);
		entity_set_int(id, EV_INT_button, currFrame[fKeys]);
		set_user_fps(id, currFrame[fFps]);
		
		if(currFrame[fSync] > 0)
			display_stats(id, currFrame[fStrafes], currFrame[fSync]);

		g_BotData[gravity] = Float:currFrame[fGravity];
	}
	else  
	{
		if(g_BotData[current_counter] < 2)
		{
			g_BotData[current_frame] = 0;
			g_BotData[current_counter]++;

			return 0.1;
		}

		NextReplay();

		g_BotData[current_frame] = 0;
		g_BotData[current_counter] = 0;
		g_BotData[current_source] = GetCurrentReplay();

		for(new i = 1;i<33;i++)
			update_sourcename(i);
	}

	if(currFrame[fTimestamp] == 0)
		return 0.033;
	return float(currFrame[fTimestamp]) / 1000.0;
}


public botSmooth(id){
	if(g_BotData[paused]) return HAM_IGNORED;
	if(ArraySize(g_Replays) == 0) return HAM_IGNORED;
	if(!is_user_alive(id)) return HAM_IGNORED;
	if(!g_BotData[spectators]) return HAM_IGNORED;
	if(deltaTime + 0.2 >= 1.0) return HAM_IGNORED;
	
	if(g_BotData[current_frame] + 1 < GetReplaySize()){
		new Float:angles[3];
		new Float:origin[3];
		new Float:oldorigin[3];
		new Float:neworigin[3];
		new Float:velocity[3];

		oldorigin[0] = currFrame[fOrigin][0];
		oldorigin[1] = currFrame[fOrigin][1];
		oldorigin[2] = currFrame[fOrigin][2];

		neworigin[0] = nextFrame[fOrigin][0];
		neworigin[1] = nextFrame[fOrigin][1];
		neworigin[2] = nextFrame[fOrigin][2];

		angles[0] = slerp_angles(currFrame[fAngles][0], nextFrame[fAngles][0], deltaTime);
		angles[1] = slerp_angles(currFrame[fAngles][1], nextFrame[fAngles][1], deltaTime);
		angles[2] = 0.0;

		origin[0] = lerp(oldorigin[0], neworigin[0], deltaTime);
		origin[1] = lerp(oldorigin[1], neworigin[1], deltaTime);
		origin[2] = lerp(oldorigin[2], neworigin[2], deltaTime);
		
		velocity[0] = float(currFrame[fSpeed]);
		velocity[1] = 0.0;
		velocity[2] = 0.0;

		SetUserAgl(id, angles);

		set_pev(id, pev_movetype, MOVETYPE_NONE);
		set_pev(id, pev_solid, SOLID_NOT);
		set_pev(id, pev_velocity, velocity);

		entity_set_origin(id, origin);

		deltaTime += 0.2;
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
	
	if(g_iBot == ent)
	{	
		new buttons = entity_get_int(id, EV_INT_button);
		
		if(buttons & IN_DUCK)		
			set_cd(cd_handle, CD_ViewOfs, Float:{0.0, 0.0, 12.0});
		else
			set_cd(cd_handle, CD_ViewOfs, Float:{0.0, 0.0, 17.0});
	}
	return FMRES_IGNORED;
}