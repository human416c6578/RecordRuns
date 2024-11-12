stock load_record(filename[128], id = -1)
{
	new file = fopen(filename, "rb");
	if (!file) {
		server_print("Failed to open file for reading.");
		return -1;
	}

	new player_name[32], demo_time[32], demo_category[33];

	// Read header
	fread_blocks(file, player_name, 32, BLOCK_CHAR);
	fread_blocks(file, demo_time, 32, BLOCK_CHAR);
	fread_blocks(file, demo_category, 32, BLOCK_CHAR);


	// If id is not provided, assign a new one
	if (id == -1) {
		id = g_iSourcesCount;
		g_iSourcesCount++;
	}

	// Save header data to source
	g_BotSources[id][source_name] = player_name;
	g_BotSources[id][source_time] = demo_time;
	g_BotSources[id][source_category] = demo_category;
	g_BotSources[id][source_path] = filename;

	remove_quotes(g_BotSources[id][source_name]);
	remove_quotes(g_BotSources[id][source_time]);
	remove_quotes(g_BotSources[id][source_category]);
	trim(g_BotSources[id][source_name]);
	trim(g_BotSources[id][source_time]);

	ArrayClear(g_BotSources[id][source_array]);

	// Frame data format
	new temp_frame[eFrameData];
	new scaled_origin[3];
	new scaled_angles[2];
	new Float:speed;
	new Float:velocity[3];
	new packed_byte, fps;

	// Read frame data
	while (!feof(file)) {
		// Read origin values (3 x short), scaled by 5
		fread(file, scaled_origin[0], BLOCK_SHORT);
		fread(file, scaled_origin[1], BLOCK_SHORT);
		fread(file, scaled_origin[2], BLOCK_SHORT);

		temp_frame[frame_origin][0] = _:scaled_origin[0] / 5.0;
		temp_frame[frame_origin][1] = _:scaled_origin[1] / 5.0;
		temp_frame[frame_origin][2] = _:scaled_origin[2] / 5.0;

		// Read angles (2 x short), scaled by 91
		fread(file, scaled_angles[0], BLOCK_SHORT);
		fread(file, scaled_angles[1], BLOCK_SHORT);

		temp_frame[frame_angles][0] = _:scaled_angles[0] / 182.0;
		temp_frame[frame_angles][1] = _:scaled_angles[1] / 182.0;
		temp_frame[frame_angles][2] = 0.0;

		// Read speed (short)
		fread(file, speed, BLOCK_SHORT);

		velocity[0] = floatsqroot(speed);
		velocity[1] = velocity[0];
		velocity[2] = 0.0;
		temp_frame[frame_velocity] = velocity;

		// Read buttons (short)
		fread(file, temp_frame[frame_buttons], BLOCK_SHORT);

		// Read packed byte for gravity, strafes, and sync
		fread(file, packed_byte, BLOCK_CHAR);
		temp_frame[frame_gravity] = (packed_byte & 0x80) ? 1.0 : 0.5;
		temp_frame[frame_strafes] = packed_byte & 0x7F;

		fread(file, temp_frame[frame_sync], BLOCK_CHAR);

		// Read FPS (short)
		fread(file, fps, BLOCK_SHORT);
		temp_frame[frame_fps] = fps;

		// Push frame data to array
		ArrayPushArray(g_BotSources[id][source_array], temp_frame);
	}

	fclose(file);

	// Finalize loading process
	TrieSetCell(g_tRecords, g_BotSources[id][source_category], id);
	g_BotData[current_source] = id;
	g_BotData[current_source_size] = ArraySize(g_BotSources[id][source_array]);
	g_BotData[current_frame] = 0;
	g_BotData[paused] = false;
	g_BotData[frametime_def] = 1.0 / ((g_BotData[current_source_size] - g_iFrmsBeforeStart) / str_to_time(g_BotSources[g_BotData[current_source]][source_time]));
	g_BotData[frametime] = g_BotData[frametime_def];

	for (new i = 1; i < 33; i++) {
		update_sourcename(i);
	}
	
	return id;
}


stock save_record(id, demo_time[32], demo_name[32], demo_path[128])
{
	new name[32];
	get_user_name(id, name, charsmax(name));

	new file = fopen(demo_path, "wb");
	if (!file) {
		server_print("Failed to open file for writing.");
		return PLUGIN_CONTINUE;
	}

	// Write header
	fwrite_blocks(file, name, 32, BLOCK_CHAR);		 // 32 bytes for name
	fwrite_blocks(file, demo_time, 32, BLOCK_CHAR);	 // 32 bytes for demo time
	fwrite_blocks(file, demo_name, 32, BLOCK_CHAR);	 // 32 bytes for demo name

	new Array:array = g_aPlayerFrames[id];
	new arrsize = ArraySize(array);
	new temp_frame[eFrameData];
	new Float:temp_speed, temp_gravity, scaled_origin[3], scaled_yaw, scaled_pitch, packed_byte;

	// Iterate through each frame
	for (new i = 0; i < arrsize; i++) {
		ArrayGetArray(array, i, temp_frame);

		// Scale origin values by 5
		scaled_origin[0] = floatround(temp_frame[frame_origin][0] * 5);
		scaled_origin[1] = floatround(temp_frame[frame_origin][1] * 5);
		scaled_origin[2] = floatround(temp_frame[frame_origin][2] * 5);
		fwrite(file, scaled_origin[0], BLOCK_SHORT);
		fwrite(file, scaled_origin[1], BLOCK_SHORT);
		fwrite(file, scaled_origin[2], BLOCK_SHORT);

		// Adjust and scale angles
		new Float:angle_yaw = temp_frame[frame_angles][0];
		new Float:angle_pitch = temp_frame[frame_angles][1];

		// Adjust yaw and pitch to avoid problems caused by the slerp function`
		if (angle_yaw > 180.0) {
			angle_yaw -= 180.0;
		}
		if (angle_pitch > 180.0) {
			angle_pitch -= 360.0;
		}

		// Scale yaw with a factor of 182 and pitch with a factor of 91
		scaled_yaw = floatround(angle_yaw * 182);				// Yaw: -180 to 180 scaled to -32768 to 32767
		scaled_pitch = floatround(angle_pitch * 182);   		// Pitch: -180 to 180 scaled to -32768 to 32767

		// Write scaled yaw as unsigned short and scaled pitch as signed short
		fwrite(file, scaled_yaw, BLOCK_SHORT);
		fwrite(file, scaled_pitch, BLOCK_SHORT);

		temp_speed = floatsqroot(floatpower(temp_frame[frame_velocity][0], 2.0) + floatpower(temp_frame[frame_velocity][1], 2.0));
		fwrite(file, temp_speed, BLOCK_SHORT);

		// Write buttons as short
		fwrite(file, temp_frame[frame_buttons], BLOCK_SHORT);

		// Pack gravity and strafes into a single byte
		temp_gravity = temp_frame[frame_gravity] > 0.9 ? 1 : 0;
		packed_byte = (temp_gravity << 7) | (temp_frame[frame_strafes] & 0x7F);
		fwrite(file, packed_byte, BLOCK_CHAR);

		fwrite(file, temp_frame[frame_sync], BLOCK_CHAR);

		// Write FPS as short
		fwrite(file, temp_frame[frame_fps], BLOCK_SHORT);
	}

	fclose(file);

	// Load the record after saving
	new source_id;
	if (TrieGetCell(g_tRecords, demo_name, source_id)) {
		g_BotData[paused] = true;
		delete_source(g_BotSources[source_id]);
		load_record(demo_path, source_id);
		return PLUGIN_CONTINUE;
	}

	load_record(demo_path);
	return PLUGIN_CONTINUE;
}