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

	set_task(3.0, "load_records");
}

public fwPlayerFinished(id, iTime, record) {
	if(!record)
		return;

	new szTime[32], szCategory[32], path[128];

	get_formated_time(iTime, szTime, charsmax(szTime));

	format(path, charsmax(path), "%s/%s.rec", g_szDirectory, "Record");
	save_record(id, szTime, szCategory, path);
}


public fwPlayerStarted(id){
	reset_record(id);
}


public load_records()
{
	new path[128];

	new dp = open_dir(g_szDirectory, path, charsmax(path));
	
	if(!dp) return;
 
	while(next_file(dp, path, charsmax(path)))
	{	
		load_record(path);
	}
 
	close_dir(dp);
}

stock get_formated_time(iTime, szTime[], size){
	formatex(szTime, size, "%d:%02d.%03ds", iTime/60000, (iTime/1000)%60, iTime%1000);
}