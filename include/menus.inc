new Float:g_bLastUsed[MAX_PLAYERS];
new bool:g_bLEGITMenu[MAX_PLAYERS];

public bot_menu(id)
{
	if(get_gametime() - g_bLastUsed[id] < 20.0 && !(get_user_flags(id) & ADMIN_BAN)){
		client_print_color(id, print_team_red, "^4[BOT] ^1Trebuie sa astepti ^4[%.2f] ^1secunde pentru a folosit comanda din nou!", 20.0-(get_gametime() - g_bLastUsed[id]));
		return PLUGIN_HANDLED;
	}

	new menu = menu_create( "\rBot Menu:", "bot_handle" );
	new text[256];	
	menu_additem( menu, "\wStart/Restart Bot", "1");
	
	menu_additem( menu, "\wPause/Unpause Bot", "2", ADMIN_BAN);
	menu_additem( menu, "\wStop Bot", "3", ADMIN_BAN);
	format(text, charsmax(text), "\wReplay Speed: %.2fx", g_BotData[frametime_def] / g_BotData[frametime]);
	menu_additem( menu, text, "4", ADMIN_BAN);
	menu_additem( menu, "\wFast forward", "5", ADMIN_BAN);	
	menu_additem( menu, "\wFast backward", "6", ADMIN_BAN);
	menu_additem( menu, "\wBot Sources", "7");
	menu_additem( menu, "", "");
	menu_additem( menu, "Exit", "0" )
	
	menu_setprop(menu, MPROP_PERPAGE, 0);
	menu_setprop( menu, MPROP_EXIT, MEXIT_ALL );
	menu_display( id, menu, 0 );
	
	return PLUGIN_HANDLED;
}

public bot_handle(id, menu, item)
{
	if( item == MENU_EXIT ) 
	{ 
		menu_destroy( menu ); 
		return PLUGIN_HANDLED; 
	} 
	switch( item )
	{
		case 0:
		{		
			g_BotData[current_frame] = 0;
			g_BotData[paused] = false;
		}
		case 1:
		{		
			g_BotData[paused] = !g_BotData[paused];
		}
		case 2:
		{		
			g_BotData[current_frame] = 0;
			g_BotData[paused] = true;
		}
		case 3:
		{		
			g_BotData[frametime] *= 2.0;
			if(g_BotData[frametime] > g_BotData[frametime_def] * 4.0)
				g_BotData[frametime] = g_BotData[frametime_def];
		}
		case 4:
		{
			g_BotData[current_frame] += 157;
			if(g_BotData[current_frame] > g_BotData[current_source_size])
			{
				g_BotData[current_frame] = 0;
			}
		}
		case 5:
		{
			g_BotData[current_frame] -= 157;
			
			if(g_BotData[current_frame] < 0)
			{
				g_BotData[current_frame] = 0;
			}
		}
		case 6:
		{
			select_category_menu( id );
			return PLUGIN_CONTINUE; 
		}
		case 7:
		{
			menu_destroy( menu ); 
			return PLUGIN_HANDLED; 
		}
		case 8:
		{
			menu_destroy( menu ); 
			return PLUGIN_HANDLED; 
		}
		case 9:
		{
			menu_destroy( menu ); 
			return PLUGIN_HANDLED; 
		}
	}
	g_bLastUsed[id] = get_gametime();
	bot_menu(id)
	
	return PLUGIN_HANDLED; 
}


public select_category_menu(id){
	new menu = menu_create( "\rCategory Menu:", "records_category_handle" );
	
	menu_additem( menu, "GRAVITY", "");
	menu_additem( menu, "\yLEGIT", "");

	menu_setprop( menu, MPROP_EXIT, MEXIT_ALL );
	menu_display( id, menu, 0 )
}

public records_category_handle(id, menu, item){
	if( item == MENU_EXIT ) 
	{ 
		menu_destroy( menu );
		bot_menu( id );
		return PLUGIN_HANDLED; 
	}

	switch(item){
		case 0:
		{
			g_bLEGITMenu[id] = false;
			records_menu(id);
		}
		case 1:
		{
			g_bLEGITMenu[id] = true;
			records_menu(id);
		}
	}

	return PLUGIN_HANDLED; 

}


public records_menu(id){
	new szMenuTitle[64];
	format(szMenuTitle, charsmax(szMenuTitle), "\y%s \rRecords", g_bLEGITMenu[id]?"LEGIT":"GRAVITY");
	new menu = menu_create( szMenuTitle, "records_handle" );
	new text[64];
	new szNum[8];
	new sourceCategory[128];
	new sourcePath[128];

	for(new i=0; i < g_iSourcesCount; i++){
		format(sourceCategory, charsmax(sourceCategory), g_BotSources[i][source_category]);
		format(sourcePath, charsmax(sourcePath), g_BotSources[i][source_path]);
		if (g_bLEGITMenu[id] && containi(sourceCategory, "LEGIT") != -1 || !g_bLEGITMenu[id] && containi(sourceCategory, "LEGIT") == -1) {
			
			// Add the record to the menu
			formatex(text, 63, "\r%s \w- %s [\y%s\w]", g_BotSources[i][source_category], g_BotSources[i][source_name], g_BotSources[i][source_time]);
			format(szNum, charsmax(szNum), "%d", i);
			menu_additem( menu, text, szNum);
		}
	}

	menu_setprop( menu, MPROP_EXIT, MEXIT_ALL );

	menu_display( id, menu, 0 )
}

public records_handle(id, menu, item){
	if( item == MENU_EXIT ) 
	{ 
		menu_destroy( menu ); 
		select_category_menu(id);
		return PLUGIN_HANDLED; 
	} 

	new szNum[8], access;
	menu_item_getinfo(menu, item, access, szNum, charsmax(szNum))
	
	g_BotData[current_source] = str_to_num(szNum);
	g_BotData[current_frame] = 0;

	g_BotData[current_source_size] = ArraySize(g_BotSources[g_BotData[current_source]][source_array]) - 1;

	g_BotData[frametime_def] = 1.0 / ((g_BotData[current_source_size] - g_iFrmsBeforeStart) / str_to_time(g_BotSources[g_BotData[current_source]][source_time]));
	g_BotData[frametime] = g_BotData[frametime_def];

	for(new i = 1;i<33;i++)
		update_sourcename(i);

	return PLUGIN_HANDLED; 
}