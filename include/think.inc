public Float:botThink( id )
{
	if(g_BotData[paused]) return 0.1;
	if(ArraySize(g_Replays) == 0) return 0.1;
	if(!is_user_alive(id)) return 0.1;
	if(!g_BotData[spectators]) return 0.1;

	new frame[eFrame];
	GetNextFrame(frame);

	frame[fAngles][0] /= -3.0;

	// bad, I should've changed the dimension of the variable
	new Float:tAngles[3], Float:tOrigin[3];
	tAngles[0] = frame[fAngles][0];
	tAngles[1] = frame[fAngles][1];
	tAngles[2] = 0.0;

	SetUserAgl(id, tAngles);

	set_pev( id, pev_movetype, MOVETYPE_NONE);
	set_pev(id, pev_solid, SOLID_NOT)
	//set_pev( id, pev_velocity, g_BotData[current_frame_data][frame_velocity] );

	//I don't know..
	tOrigin[0] = frame[fOrigin][0];
	tOrigin[1] = frame[fOrigin][1];
	tOrigin[2] = frame[fOrigin][2];
	entity_set_origin(id, tOrigin);


	if(frame[fGravity] != g_BotData[gravity]){
		if(frame[fGravity] == 1){
			engclient_cmd(id, "weapon_usp");
		}
		else{
			engclient_cmd(id, "weapon_knife")
			set_pev(id, pev_viewmodel2, "models/llg/v_butcher.mdl");
		}
	}
	

	animate_legs(id, frame[fKeys], bool:frame[fGrounded]); // won't work currently, I have to convert the keys back to goldsrc model
	
	entity_set_int(id, EV_INT_button, frame[fKeys]);
	//set_user_fps(id, frame[fFps]);
	
	// TODO
	//if(g_BotData[current_frame_data][frame_sync])
	//	display_stats(id, g_BotData[current_frame_data][frame_strafes], g_BotData[current_frame_data][frame_sync]);

	g_BotData[gravity] = Float:frame[fGravity];

	return float(frame[fTimestamp])/1000.0;
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