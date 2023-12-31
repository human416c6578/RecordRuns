stock load_record(filename[128], id = -1)
{
	new file = fopen(filename, "r");
	new string[300];
	new player_name[32], demo_time[32], demo_category[33];
	
	fgets(file, string, charsmax(string));
	if(containi(string, "HEADER") == -1)
	{		
		fclose(file)
		
		return -1;
	}
	static ExplodedString[17][32];
	parse(string, player_name, 31, player_name, 31, demo_time, 31 , demo_category, 31);

	static temp_frame[eFrameData];
	if(id == -1){
		id = g_iSourcesCount;
		g_iSourcesCount++
	}

	g_BotSources[id][source_name] = player_name;
	g_BotSources[id][source_time] = demo_time;
	g_BotSources[id][source_category] = demo_category;
	g_BotSources[id][source_path] = filename;
	remove_quotes(g_BotSources[id][source_name])
	remove_quotes(g_BotSources[id][source_time])
	remove_quotes(g_BotSources[id][source_category])
	trim(g_BotSources[id][source_name])
	trim(g_BotSources[id][source_time])

	ArrayClear(g_BotSources[id][source_array]);

	while(fgets(file, string, charsmax(string)))
	{
		ExplodeString( ExplodedString, 16, 31, string, ' ' );
		temp_frame[frame_origin][0] = _:str_to_float(ExplodedString[1])
		temp_frame[frame_origin][1] = _:str_to_float(ExplodedString[2])
		temp_frame[frame_origin][2] = _:str_to_float(ExplodedString[3])
		temp_frame[frame_angles][0] = _:str_to_float(ExplodedString[4])
		temp_frame[frame_angles][1] = _:str_to_float(ExplodedString[5])
		temp_frame[frame_angles][2] = _:str_to_float(ExplodedString[6])
		temp_frame[frame_velocity][0] = _:str_to_float(ExplodedString[7]) 
		temp_frame[frame_velocity][1] = _:str_to_float(ExplodedString[8])
		temp_frame[frame_velocity][2] = _:str_to_float(ExplodedString[9])
		temp_frame[frame_buttons] = _:str_to_num(ExplodedString[10])
		temp_frame[frame_gravity] = _:str_to_float(ExplodedString[11])
		temp_frame[frame_fps] = _:str_to_num(ExplodedString[12])
		temp_frame[frame_strafes] = _:str_to_num(ExplodedString[13])
		temp_frame[frame_sync] = _:str_to_num(ExplodedString[14])
		ArrayPushArray(g_BotSources[id][source_array], temp_frame)
		
	}
	fclose(file)

	TrieSetCell(g_tRecords, g_BotSources[id][source_category], id);

	g_BotData[current_source] = id;
	g_BotData[current_source_size] = ArraySize(g_BotSources[id][source_array]);
	g_BotData[current_frame] = 0;
	g_BotData[paused] = false;
	g_BotData[frametime_def] = 1.0 / ((g_BotData[current_source_size] - g_iFrmsBeforeStart) / str_to_time(g_BotSources[g_BotData[current_source]][source_time]));
	g_BotData[frametime] = g_BotData[frametime_def];
	//server_print("Frames: %d | Time: %s | Time: %f(s) | FrameTime: %f", g_BotData[current_source_size], g_BotSources[g_BotData[current_source]][source_time], str_to_time(g_BotSources[g_BotData[current_source]][source_time]), g_BotData[frametime]);

	for(new i = 1;i<33;i++)
		update_sourcename(i);
	
	return id;
}

stock save_record(id, demo_time[32], demo_name[32], demo_path[128])
{
	new name[32];

	get_user_name(id, name, 31);

	new file = fopen(demo_path, "wb");

	new string[300];
	format(string, charsmax(string), "HEADER ^"%s^" ^"%s^" ^"%s^" ^n", name, demo_time, demo_name);
	fputs(file, string);
	
	new Array:array;
	array = g_aPlayerFrames[id];

	new arrsize = ArraySize(array);
	new temp_frame[eFrameData];	
	for(new i = 0; i < arrsize; i++ )
	{
		ArrayGetArray( array, i, temp_frame );		
		format(string, charsmax(string), "INFO %f %f %f %f %f %f %f %f %f %d %f %d %d %d^n", temp_frame[frame_origin][0],
		temp_frame[frame_origin][1], temp_frame[frame_origin][2], temp_frame[frame_angles][0], temp_frame[frame_angles][1],
		temp_frame[frame_angles][2], temp_frame[frame_velocity][0], temp_frame[frame_velocity][1], temp_frame[frame_velocity][2],
		temp_frame[frame_buttons], temp_frame[frame_gravity], temp_frame[frame_fps], temp_frame[frame_strafes], temp_frame[frame_sync]);
		fputs(file, string);
	}
		
	fclose(file);

	
	new source_id;

	if(TrieGetCell(g_tRecords, demo_name, source_id))
	{
		g_BotData[paused] = true;
		
		delete_source(g_BotSources[source_id]);

		load_record(demo_path, source_id);

		return PLUGIN_CONTINUE;
	}

	load_record(demo_path);

	return PLUGIN_CONTINUE;
}