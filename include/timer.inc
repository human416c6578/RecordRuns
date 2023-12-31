forward timer_player_started(id);
forward timer_player_finished(id);
forward timer_player_record(id);
forward timer_player_category_changed(id);
forward timer_player_rank_changed(id, rank);
forward timer_db_loaded();

native get_bool_timer(id);
native toggle_timer(id);

native open_top_menu(id);
native open_cat_menu(id);

native set_user_finished(id, value);
native get_user_finished(id);

native get_user_best(id);

native get_user_category(id, name[], len);

/*
    // get_categories_enabled(data[], len)
    writes into data all categories enabled separated by a comma(,)
*/
native get_categories_enabled(data[], len);

// returns category id
// sets name as category name
native get_category_name(id, name[], len);