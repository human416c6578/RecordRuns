#include <amxmodx>
#include <easy_http>
#include <timer>
#include <record_runs>

#define PLUGIN "Record Runs"
#define VERSION "1.0"
#define AUTHOR "MrShark45"

#define MAX_CATEGORIES 40

new const rec_foldername[10] = "recording";

new g_cStorageType;
new g_cStorageUrl;

new g_szStorageUrl[128];

new g_szDirectory[128];
new g_szMapName[64];

public plugin_init() 
{
	// 0 - local | 1 - webserver
	g_cStorageType = register_cvar("bot_storage_type", "1");
	g_cStorageUrl = register_cvar("bot_storage_url", "http://cs-gfx.eu/uploads/recording");

}

public plugin_cfg()
{	
	get_pcvar_string(g_cStorageUrl, g_szStorageUrl, charsmax(g_szStorageUrl));

	get_localinfo( "amxx_datadir", g_szDirectory, charsmax( g_szDirectory ) );
	format(g_szDirectory, charsmax(g_szDirectory), "%s/%s", g_szDirectory, rec_foldername)
	get_mapname(g_szMapName, 63);
	strtolower(g_szMapName);

	if(!dir_exists(g_szDirectory))
	{
		mkdir(g_szDirectory);
	}		
	format(g_szDirectory, charsmax(g_szDirectory), "%s/%s", g_szDirectory, g_szMapName)

	if(!dir_exists(g_szDirectory))
	{
		mkdir(g_szDirectory)
	}
}

public timer_db_loaded()
{
	set_task(3.0, "load_records");
}

public timer_player_record(id) {
	new szCategory[32], path[128];
	new iTime = get_user_best(id);

	get_user_category(id, szCategory, charsmax(szCategory));
	format(szCategory, charsmax(szCategory), "[%s]", szCategory);

	format(path, charsmax(path), "%s/%s.rec", g_szDirectory, szCategory);
	save_record(id, path, iTime, szCategory);

	// write what replays the external script should grab
	new szReplaysFilepath[128], string[128];

	get_localinfo("amxx_datadir", szReplaysFilepath, charsmax(szReplaysFilepath));
	format(szReplaysFilepath, charsmax(szReplaysFilepath), "%s/replays.txt", szReplaysFilepath);
	format(string, charsmax(string), "^"%s^" ^"%s^"^n", g_szMapName, szCategory);

	new file = fopen(szReplaysFilepath, "at");
	if (file) {
		fputs(file, string);
		fclose(file);
	}
}

public timer_player_started(id){
	start_record(id);
}

public timer_player_finished(id) {
	stop_record(id);
}

public load_records(){
	if(get_pcvar_num(g_cStorageType) == 0) 
		load_records_local();
	else
		load_records_webserver();
}

public load_records_local()
{
	new data[1024];
	new categories[MAX_CATEGORIES][32];
	new path[128];
	get_categories_enabled(data, charsmax(data));
	
	new categories_count = ExplodeString( categories, MAX_CATEGORIES, 31, data, ',' );
	for(new i=0;i<=categories_count;i++)
	{
		format(path, charsmax(path), "%s/[%s].rec", g_szDirectory, categories[i]);
		if(file_exists(path))
			load_record(path);
	}
}

public load_records_webserver()
{
	new data[1024];
	new categories[MAX_CATEGORIES][32];
	get_categories_enabled(data, charsmax(data));
	
	new categories_count = ExplodeString( categories, 32, 31, data, ',' );
	for(new i=0;i<=categories_count;i++)
	{
		replace_string(categories[i], charsmax(categories[]), " ", "%20");
		format(categories[i], charsmax(categories[]), "[%s].rec", categories[i]);
		get_file(categories[i]);
	}
}

public get_file(filename[])
{
	new url[128];
	format(url, charsmax(url), "%s/%s/%s", g_szStorageUrl, g_szMapName, filename);
	ezhttp_get(url, "http_get_file_complete");
}

public http_get_file_complete(EzHttpRequest:request_id)
{
    // Check if there's an error in the response
    if (ezhttp_get_error_code(request_id) != EZH_OK)
    {
        new error[64];
        ezhttp_get_error_message(request_id, error, charsmax(error));
        server_print("Response error: %s", error);
        return;
    }

    // Check for HTTP status code 404
    if (ezhttp_get_http_code(request_id) == 404)
    {
        server_print("Error: File not found (404)");
        return;
    }

    new url[256];
    ezhttp_get_url(request_id, url, charsmax(url));

    if(!dir_exists("addons/amxmodx/data/temp"))
        mkdir("addons/amxmodx/data/temp");

    new filepath[128];
    format(filepath, charsmax(filepath), "addons/amxmodx/request_%d.json", request_id);
    ezhttp_save_data_to_file(request_id, filepath);

    load_record(filepath);

    delete_file(filepath);
}

stock ExplodeString( p_szOutput[][], p_nMax, p_nSize, p_szInput[], p_szDelimiter )
{
	new nIdx	= 0, l = strlen( p_szInput );
	new nLen	= (1 + copyc( p_szOutput[nIdx], p_nSize, p_szInput, p_szDelimiter ) );
	while ( (nLen < l) && (++nIdx < p_nMax) )
		nLen += (1 + copyc( p_szOutput[nIdx], p_nSize, p_szInput[nLen], p_szDelimiter ) );
	return(nIdx);
}

stock get_formated_time(iTime, szTime[], size){
	formatex(szTime, size, "%d:%02d.%03ds", iTime/60000, (iTime/1000)%60, iTime%1000);
}

/*
OLD WAY OF GETTING THE RECORDS FROM THE WEB SERVER
Using a php script to retrieve the valid demos from the map folder

public load_records(){
	new url[128];
	format(url, charsmax(url), "https://cs-gfx.eu/get_recordings.php?map=%s", g_szMapName);
	ezhttp_get(url, "http_get_files_complete")
}

public http_get_files_complete(EzHttpRequest:request_id)
{
	if (ezhttp_get_error_code(request_id) != EZH_OK)
	{
		new error[64]
		ezhttp_get_error_message(request_id, error, charsmax(error))
		server_print("Response error: %s", error);
		return
	}

	new data[1024];
	ezhttp_get_data(request_id, data, charsmax(data))

	new JSON:object = json_parse(data);

	new JSON:value;
	new filename[64];
	new count = json_object_get_count(object);

	new categories[32][32];
	get_categories_enabled(data, charsmax(data));
	
	new categories_count = ExplodeString( categories, 32, 31, data, ',' );
	for(new i=0;i<=categories_count;i++)
	{
		replace_string(categories[i], charsmax(categories[]), " ", "%20");
		format(categories[i], charsmax(categories[]), "[%s]", categories[i]);
	}
	for(new i=0;i<count;i++)
	{
		value = json_object_get_value_at(object, i);
		json_get_string(value, filename, charsmax(filename));
		
		new bool:download = false;

		for(new j=0;j<=categories_count;j++)
		{	
			if(containi(filename, categories[j]) != -1)
			{	
				download = true;
				break;
			}
		}

		json_free(value);
		if(download)
			get_file(filename);
	}
	json_free(object);
}

*/