#pragma reqlib record_runs

native open_bot_menu(id);
native reset_record(id);
native save_record(id, demo_path[128], demo_time, demo_info[32]);
native load_record(path[128]);