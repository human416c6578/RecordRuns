#include <amxmodx>
#include <hamsandwich>
#include <fakemeta_util>
#include <fun>
#include <cstrike>
#include <engine>
#include <replays>
#include <get_user_fps>
#include <strafe_stats>
#include <globals>
#include <stocks>
#include <menus>
#include <think>

#define PLUGIN "Record Runs"
#define VERSION "1.0"
#define AUTHOR "MrShark45"

public plugin_init() 
{
	register_plugin(PLUGIN, VERSION, AUTHOR);
	register_clcmd( "say /bot", "bot_menu");

	register_forward(FM_UpdateClientData, "fw_updateclientdata", 1)

	new ent = fm_create_entity( "info_target" );

	set_pev( ent, pev_classname, "entities_think" );
	set_pev( ent, pev_nextthink, get_gametime() + 1.0 );
	RegisterHam( Ham_Think, "info_target", "entities_think", 1 );
	
	ent = fm_create_entity("info_target");
	set_pev(ent, pev_classname, "bot_playback");
	set_pev(ent, pev_nextthink, get_gametime() + 1.0);

	ent = fm_create_entity("info_target");
	set_pev(ent, pev_classname, "bot_smooth");
	set_pev(ent, pev_nextthink, get_gametime() + 1.0);

	set_task(0.5, "check_spectators", TASK_SPECTATORS, _, _, "b");

	g_fwdOpenSourcesMenu = CreateMultiForward("record_runs_open_menu", ET_IGNORE, FP_CELL, FP_CELL);
}

public plugin_precache()
{
	precache_model("models/llg/v_butcher.mdl");
}

public plugin_natives()
{
	register_library("record_runs");

	register_native("open_bot_menu", "open_bot_menu_native");
	register_native("start_record", "start_record_native");
	register_native("stop_record", "stop_record_native");
	register_native("save_record", "save_record_native");
	register_native("load_record", "load_record_native");
	register_native("set_current_replay", "set_current_replay_native");
}

public set_current_replay_native(numParams)
{
	new id = get_param(1);
	g_BotData[current_source] = id;
	g_BotData[current_frame] = 0;
	SetCurrentReplay(id);

	for(new i = 1;i<33;i++)
		update_sourcename(i);
}

public open_bot_menu_native(numParams){
	new id = get_param(1);
	bot_menu(id);
}

public start_record_native(numParams) {
	new id = get_param(1);

	StartRecord(id);
}

public stop_record_native(numParams) {
	new id = get_param(1);

	StopRecord(id);
}

public save_record_native(numParams) {
	new id = get_param(1);
	new demo_time;
	new demo_name[32];
	new demo_authid[24];
	new demo_path[128];
	new demo_map[32];
	new demo_info[32];
	
	
	get_string(2, demo_path, charsmax(demo_path));
	demo_time = get_param(3);
	get_string(4, demo_info, charsmax(demo_info));

	get_mapname(demo_map, charsmax(demo_map));
	get_user_name(id, demo_name, charsmax(demo_name));
	get_user_authid(id, demo_authid, charsmax(demo_authid));

	SaveReplay(demo_path, id, demo_map, demo_authid, demo_info, demo_time);
	
	new header[eHeader];
	for(new i=0;i<ArraySize(g_Replays);i++) {
		ArrayGetArray(g_Replays, i, header);
		if(equali(header[hInfo], demo_info))
		{
			ArrayDeleteItem(g_Replays, i);
			DeleteReplay(i);
		}
	}
	LoadReplay(0, demo_path, header);
	ArrayPushArray(g_Replays, header);
	
}

public load_record_native(numParams) {
	new path[128];
	
	get_string(1, path, charsmax(path));

	new header[eHeader];
	LoadReplay(0, path, header);
	ArrayPushArray(g_Replays, header);
}

public plugin_cfg()
{
	g_Replays = ArrayCreate(eHeader);

	new temp[eBotData];
	g_BotData = temp;

	g_BotData[slow] = 1;

	set_task(3.0, "create_bot");
}

public plugin_end()
{
	ArrayDestroy(g_Replays);
}

public client_putinserver(id) {
	update_sourcename(id);
}

public create_bot()
{
	g_iBot = makebot("Record Bot");
}

public entities_think( ent )
{	
	static classname[64]
	pev(ent, pev_classname, classname, 63);
	static Float:fFrameTime;

	if(equal(classname, "bot_playback")){
		fFrameTime = botThink( g_iBot ) * float(g_BotData[slow]);
		
		set_pev( ent, pev_nextthink, get_gametime() + fFrameTime );
	}
	else if(equal(classname, "bot_smooth")){
		botSmooth( g_iBot );
		set_pev( ent, pev_nextthink, get_gametime() + fFrameTime / 5.0 );
	}
}


public check_spectators(id){
	g_BotData[spectators] = get_spectators(g_iBot);
}