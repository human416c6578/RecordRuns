#include <amxmodx>
#include <timer>
#include <record_runs>

#define PLUGIN "Record Runs"
#define VERSION "1.0"
#define AUTHOR "MrShark45"

new const rec_foldername[10] = "recording";

new g_szDirectory[128];
new g_szMapName[64];

public plugin_init() 
{

}

public plugin_cfg()
{
	get_localinfo( "amxx_datadir", g_szDirectory, charsmax( g_szDirectory ) );
	format(g_szDirectory, charsmax(g_szDirectory), "%s/%s", g_szDirectory, rec_foldername)
	get_mapname(g_szMapName, 63);

	if(!dir_exists(g_szDirectory))
	{
		mkdir(g_szDirectory);
	}		
	format(g_szDirectory, charsmax(g_szDirectory), "%s/%s", g_szDirectory, g_szMapName)

	if(!dir_exists(g_szDirectory))
	{
		mkdir(g_szDirectory)
	}

	set_task(1.0, "load_records");
}

public fwPlayerFinished(id, iTime, record) {
    if(!record)
        return;

    new szTime[32], szInfo[32], path[128];

    // Get the formatted time for the player's finish time
    get_formated_time(iTime, szTime, charsmax(szTime));

    // Save the new record at the correct spot
    format(path, charsmax(path), "%s/Record.rec", g_szDirectory);
    save_record(id, szTime, szInfo, path);
}


public fwPlayerStarted(id){
	reset_record(id);
}


public load_records()
{
	new path[128], replay[128];
	
	new dp = open_dir(g_szDirectory, path, charsmax(path));
	
	if(!dp) return;
	formatex(replay, charsmax(replay), "%s/%s", g_szDirectory, path);
	load_record(replay);
 
	while(next_file(dp, path, charsmax(path)))
	{	
		formatex(replay, charsmax(replay), "%s/%s", g_szDirectory, path);
		server_print("%s", replay);
		load_record(replay);
	}
 
	close_dir(dp);
}

stock get_formated_time(iTime, szTime[], size){
	formatex(szTime, size, "%d:%02d.%03ds", iTime/60000, (iTime/1000)%60, iTime%1000);
}