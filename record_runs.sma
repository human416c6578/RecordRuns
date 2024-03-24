#include <amxmodx>
#include <hamsandwich>
#include <fakemeta_util>
#include <fun>
#include <cstrike>
#include <engine>
#include <json>
#include <easy_http>
#include <get_user_fps>
#include <strafe_stats>
#include <globals>
#include <stocks>
#include <storage>
#include <menus>
#include <think>

#define PLUGIN "Record Runs"
#define VERSION "1.0"
#define AUTHOR "MrShark45"


public plugin_init() 
{
	register_plugin(PLUGIN, VERSION, AUTHOR);
	register_clcmd( "say /bot", "bot_menu");

	g_cRepeatReplay = register_cvar("bot_repeat", "2");

	register_forward(FM_UpdateClientData, "fw_updateclientdata", 1)
	register_forward(FM_PlayerPreThink, "fw_preThink", 1);

	RegisterHam(Ham_Spawn, "player", "player_spawned");

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

	new arrsize = ArraySize(g_aPlayerFrames[id]);
	if(arrsize > 20)
	{
		new Array:temp = ArrayClone(g_aPlayerFrames[id]);
		new temp_frame[eFrameData];

		ArrayClear(g_aPlayerFrames[id]);
		for(new i=arrsize - 20;i<arrsize;i++)
		{
			ArrayGetArray(temp, i, temp_frame);
			ArrayPushArray(g_aPlayerFrames[id], temp_frame);
		}
		
		ArrayDestroy(temp);
	}
}

public save_record_native(numParams) {
	new id = get_param(1);
	new demo_time[32];
	new demo_name[32];
	new demo_path[128];
	
	get_string(2, demo_time, charsmax(demo_time));
	get_string(3, demo_name, charsmax(demo_name));
	get_string(4, demo_path, charsmax(demo_path));

	save_record(id, demo_time, demo_name, demo_path);
}

public load_record_native(numParams) {
	new path[128];
	
	get_string(1, path, charsmax(path));

	load_record(path);
}

public plugin_cfg()
{

	g_tRecords = TrieCreate();

	new temp[eBotData];
	g_BotData = temp;

	g_BotData[frametime] = g_fFrameTime;

	for ( new i = 1; i < 33; i++ )
		g_aPlayerFrames[i] = ArrayCreate( eFrameData );

	for ( new i = 0;  i < MAX_SOURCES; i++)
	{
		g_BotSources[i][source_array] = _:ArrayCreate( eFrameData );
	}

	set_task(3.0, "create_bot");
}

public plugin_end(){
	for ( new i = 0; i < 33; i++ )
	{
		ArrayDestroy( g_aPlayerFrames[i] );
		
	}	
	for ( new i = 0;  i < MAX_SOURCES; i++)
	{
		ArrayDestroy( g_BotSources[i][source_array] );
	}

	TrieDestroy(g_tRecords);
}

public client_putinserver(id) {
	update_sourcename(id);
}

public player_spawned(id)
{
	ArrayClear(g_aPlayerFrames[id]);
}

public create_bot()
{
	g_iBot = makebot("Record Bot");
}

public fw_preThink(id){
	new Float:now = get_gametime();
	static Float:last_input[33];
	static Float:last[33];
	
	if(GetButton(id, IN_ATTACK) || GetButton(id, IN_JUMP) || GetButton(id, IN_FORWARD) || GetButton(id, IN_BACK) || GetButton(id, IN_MOVELEFT) || GetButton(id, IN_MOVERIGHT)){
		last_input[id] = get_gametime();
	}
	
	if(!is_user_alive(id) || now - last_input[id] > 10.0) return;
	
	if((now - last[id]) >= g_fFrameTime ){
		record_player(id);
		last[id] = get_gametime();
	}

}

public entities_think( ent )
{	
	static classname[64]
	pev(ent, pev_classname, classname, 63);


	if(equal(classname, "bot_playback")){
		botThink( g_iBot );		
		set_pev( ent, pev_nextthink, get_gametime() + g_BotData[frametime] );
	}
	if(equal(classname, "bot_smooth")){
		botSmooth( g_iBot );		
		
		set_pev( ent, pev_nextthink, get_gametime() + g_BotData[frametime] / 5.0 );
	}
}

public reset_record(id)
{
	new arrsize = ArraySize(g_aPlayerFrames[id]);

	if(arrsize > 20)
	{
		new Array:temp = ArrayClone(g_aPlayerFrames[id]);
		new temp_frame[eFrameData];

		ArrayClear(g_aPlayerFrames[id]);
		for(new i=arrsize - 20;i<arrsize;i++)
		{
			ArrayGetArray(temp, i, temp_frame);
			ArrayPushArray(g_aPlayerFrames[id], temp_frame);
		}
		
		ArrayDestroy(temp);
	}

	
}

public record_player( id )
{
	static temp_data[eFrameData];
	
	temp_data[frame_buttons] = pev(id, pev_button);
	
	pev(id, pev_origin, 	temp_data[frame_origin]);
	pev(id, pev_v_angle, 	temp_data[frame_angles]);
	pev(id, pev_velocity,  	temp_data[frame_velocity]);
	temp_data[frame_gravity] = get_user_gravity(id);
	temp_data[frame_fps] = get_user_fps(id);
	temp_data[frame_strafes] = get_user_strafes(id);
	temp_data[frame_sync] = get_user_sync(id);
	
	ArrayPushArray(g_aPlayerFrames[id], temp_data);
}


public check_spectators(id){
	g_BotData[spectators] = get_spectators(g_iBot);
}