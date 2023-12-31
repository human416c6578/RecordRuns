
#define MAX_SOURCES 42

#define TASK_SPECTATORS 5734

new Array:g_aPlayerFrames[33];

new g_iBot;

new const Float:g_fFrameTime = 0.033334;

new const g_iFrmsBeforeStart = 20;

enum eSourceData
{
	source_name[32],
	source_time[32],
	source_category[33],
	source_path[128],
	Array:source_array
}

new g_BotSources[MAX_SOURCES][eSourceData]

enum eFrameData
{
	Float:frame_origin[3],
	Float:frame_angles[3],
	Float:frame_velocity[3],
	frame_buttons,
	Float:frame_gravity,
	frame_fps,
	frame_sync,
	frame_strafes
}

enum eBotData
{
	current_source,
	current_source_size,
	current_frame,
	current_frame_data[eFrameData],
	next_frame_data[eFrameData],
	bool:paused,
	current_counter,
	Float:gravity,
	Float:deltatime,
	Float:frametime_def,
	Float:frametime,
	spectators
}

new Trie:g_tRecords;

new g_iSourcesCount;

new g_BotData[eBotData];

new g_cRepeatReplay;