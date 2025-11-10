#define TASK_SPECTATORS 5734

new g_iBot;

new Array:g_Replays;

enum eBotData
{
	current_frame,
	current_source,
	bool:paused,
	current_counter,
	Float:gravity,
	slow,
	spectators
}

new g_BotData[eBotData];

new g_fwdOpenSourcesMenu;