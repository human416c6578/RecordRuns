#pragma reqlib record_runs

native open_bot_menu(id);
native reset_record(id);
native save_record(id, demo_time[32], demo_name[32], demo_path[128]);
native load_record(path[128]);