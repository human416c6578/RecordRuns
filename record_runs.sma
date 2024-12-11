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

	set_task(0.5, "check_spectators", TASK_SPECTATORS, _, _, "b");

	register_event("ResetHUD", "reset_model", "b");

}

public plugin_precache()
{
	precache_model("models/llg/v_butcher.mdl");

	precache_generic("models/player/robot/robot.mdl");
	precache_generic("models/player/robot/robotT.mdl");
}

public reset_model(id, level, cid)
{
	if(id != g_iBot || !is_user_connected(g_iBot)) return PLUGIN_CONTINUE;
	
	cs_set_user_model(id, "robot");

	return PLUGIN_CONTINUE;
}

public plugin_natives()
{
	register_library("record_runs");

	register_native("open_bot_menu", "open_bot_menu_native");
	register_native("reset_record", "reset_record_native");
	register_native("save_record", "save_record_native");
	register_native("load_record", "load_record_native");
}

public open_bot_menu_native(numParams){
	new id = get_param(1);
	bot_menu(id);
}

public reset_record_native(numParams) {
	new id = get_param(1);

	StartRecord(id);
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

	if(equal(classname, "bot_playback")){
		new Float:nextframe = botThink( g_iBot ) * float(g_BotData[slow]);
		set_pev( ent, pev_nextthink, get_gametime() + nextframe );
	}
}


public check_spectators(id){
	g_BotData[spectators] = get_spectators(g_iBot);
}