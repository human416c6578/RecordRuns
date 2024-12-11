
stock makebot(name[64])
{
	remove_quotes(name);
	trim(name)
	new bot = engfunc( EngFunc_CreateFakeClient, name );
	if ( !bot )
	{
		server_print( "Couldn't create a bot, server full?" );
		return 0;
	}
	
	engfunc( EngFunc_FreeEntPrivateData, bot );
	bot_settings( bot );

	static szRejectReason[128];
	dllfunc( DLLFunc_ClientConnect, bot, name, "127.0.0.1", szRejectReason );
	if ( !is_user_connected( bot ) )
	{
		server_print( "Connection rejected: %s", szRejectReason );
		return 0;
	}

	dllfunc( DLLFunc_ClientPutInServer, bot );
	set_pev( bot, pev_spawnflags, pev( bot, pev_spawnflags ) | FL_FAKECLIENT );
	set_pev( bot, pev_flags, pev( bot, pev_flags ) | FL_FAKECLIENT );

	engclient_cmd( bot , "jointeam" , "6" )
	cs_set_user_team(bot, CS_TEAM_CT);
	ExecuteHamB( Ham_CS_RoundRespawn, bot );
	fm_give_item(bot, "weapon_knife" );
	fm_give_item(bot, "weapon_usp" );
	fm_set_user_godmode( bot, 1 );

	return bot;
}
//SETS BOTS SETTINGS
stock bot_settings( id )
{
	set_user_info( id, "model", "gordon" );
	set_user_info( id, "rate", "100000" );
	set_user_info( id, "cl_updaterate", "102" );
	set_user_info( id, "cl_lw", "1" );
	set_user_info( id, "cl_lc", "1" );
	set_user_info( id, "tracker", "0" );
	set_user_info( id, "cl_dlmax", "512" );
	set_user_info( id, "lefthand", "0" );
	set_user_info( id, "friends", "0" );
	set_user_info( id, "dm", "0" );
	set_user_info( id, "ah", "1" );

	set_user_info( id, "*bot", "1" );
	set_user_info( id, "_cl_autowepswitch", "1" );
	set_user_info( id, "_vgui_menu", "0" );		 
	set_user_info( id, "_vgui_menus", "0" );		
}


stock ExplodeString( p_szOutput[][], p_nMax, p_nSize, p_szInput[], p_szDelimiter )
{
	new nIdx	= 0, l = strlen( p_szInput );
	new nLen	= (1 + copyc( p_szOutput[nIdx], p_nSize, p_szInput, p_szDelimiter ) );
	while ( (nLen < l) && (++nIdx < p_nMax) )
		nLen += (1 + copyc( p_szOutput[nIdx], p_nSize, p_szInput[nLen], p_szDelimiter ) );
	return(nIdx);
}


stock animate_legs(id,  keys, bool:onground)
{
	#define InMove (keys & IN_FORWARD || keys & IN_LEFT || keys & IN_RIGHT || keys & IN_BACK)
	if (onground)
	{
		if ( keys & IN_DUCK && InMove )
		{
			set_pev(id, pev_gaitsequence, 5 );
		}else if ( keys & IN_DUCK )
		{
			set_pev(id, pev_gaitsequence, 2 );
		}else  {
			set_pev(id, pev_gaitsequence, 4 );
		}
		if ( keys & IN_JUMP )
		{
			set_pev(id, pev_gaitsequence, 6 );
		}else  {
			set_pev(id, pev_gaitsequence, 4 );
		}
	}else  {
		set_pev(id, pev_gaitsequence, 6 );
		
		if ( keys & IN_DUCK )
		{
			set_pev(id, pev_gaitsequence, 5 );
		}
	}
}

stock set_user_fake_name(const id, const name[])
{	
	message_begin(MSG_ONE, SVC_UPDATEUSERINFO, _, id)	
	write_byte(g_iBot - 1)
	write_long(get_user_userid(g_iBot))
	write_char('\')
	write_char('n')
	write_char('a')
	write_char('m')
	write_char('e')
	write_char('\')
	write_string(name)
	for(new i; i < 16; i++) write_byte(0)
	message_end()
}

stock update_sourcename( id )
{
	if(!is_user_connected(id)) return;
	if(ArraySize(g_Replays) == 0) return;
	new sz_name[128]
	new replay[eHeader];
	ArrayGetArray(g_Replays, g_BotData[current_source], replay);
	format(sz_name, charsmax(sz_name), "%s %s %s", replay[hInfo], replay[hName], replay[hTime]);
	set_user_fake_name(id, sz_name);	
}

stock Float:str_to_time(time[]){
	new Float:s;
	new exploded[2][32];
	new exploded2[2][32];
	ExplodeString( exploded, 2, 31, time, ':' );
	ExplodeString( exploded2, 2, 31, exploded[1], '.' );
	//0:56.925s
	
	
	s += str_to_float(exploded[0]) * 60;
	s += str_to_float(exploded2[0]);
	s += str_to_float(exploded2[1]) / 1000.0;
	return s;
}


stock SetUserAgl(id,Float:agl[3]){
	entity_set_vector(id,EV_VEC_angles,agl)
	entity_set_int(id,EV_INT_fixangle,1)
}

stock Float:slerp_angles(Float:start_angle, Float:end_angle, Float:t)
{
	static Float:angle, Float:delta;

	static Float:PI = 3.14159;

	// Convert angles to radians
	start_angle = start_angle * PI / 180.0;
	end_angle = end_angle * PI / 180.0;

	// Find the shortest path between the two angles
	delta = end_angle - start_angle;
	if (delta > PI) {
		delta -= 2.0 * PI;
	} else if (delta < -PI) {
		delta += 2.0 * PI;
	}

	// Interpolate between the two angles
	angle = start_angle + t * delta;

	// Convert angle back to degrees
	angle = angle * 180.0 / PI;

	// Wrap angle to the range [-180, 180]
	if (angle > 180.0) {
		angle -= 360.0;
	} else if (angle < -180.0) {
		angle += 360.0;
	}

	return angle;
}

stock Float:slerp(Float:a, Float:b, Float:dt){
	new Float:dist = get_dist(a + 180.0, b + 180.0);

	// +/- | a   b
	//  -  | 10  330 | abs( a - b ) > 180 and a < b
	//  +  | 330 10  | abs( a - b ) > 180 and a > b
	//  +  | 260 280 | abs( a - b ) < 180 and a < b
	//  -  | 280 260 | abs( a - b ) < 180 and a > b
	
	if(floatabs(a-b) > 180){
		if(a<b)
			// -
			return fmod((a + 180.0 - (dist * dt)), 360.0) - 180.0;
		else
			// +
			return fmod((a + 180.0 + (dist * dt)), 360.0) - 180.0;
	}
	else{
		if(a<b)
			// +
			return fmod((a + 180.0 + (dist * dt)), 360.0) - 180.0;
		else
			// -
			return fmod((a + 180.0 - (dist * dt)), 360.0) - 180.0;
	}
}

stock Float:get_dist(Float:a, Float:b){
	new Float:raw_diff = floatabs(a - b);
	return 180.0 - floatabs(raw_diff - 180.0)
}

stock Float:fmod(Float:a, Float:b){
	if(a > b)
		return a - b
	if(a < 0)
		return b - floatabs(a) 
	return a
}


stock get_spectators(id)
{
	new players[MAX_PLAYERS], iNum;
	get_players(players, iNum, "bch");

	for(new i;i<iNum;i++)
	{
		if(pev(players[i], pev_iuser2) == id)
			return 1;
	}

	return 0;
}

stock bool:GetButton(id, button){
	return bool:(entity_get_int(id,EV_INT_button) & button);
}

stock get_formated_time(iTime, szTime[], size){
	formatex(szTime, size, "%d:%02d.%03ds", iTime/60000, (iTime/1000)%60, iTime%1000);
}