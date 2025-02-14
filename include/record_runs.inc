#pragma reqlib record_runs

native open_bot_menu(id);
native start_record(id);
native stop_record(id);
native save_record(id, demo_path[128], demo_time, demo_info[32]);
native load_record(path[128]);
native set_current_replay(id);

forward record_runs_open_menu(id, Array:array);