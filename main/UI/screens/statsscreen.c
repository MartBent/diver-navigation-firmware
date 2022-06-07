StatsScreen* createStatsScreen() {
  StatsScreen* stats_screen = (StatsScreen*) malloc(sizeof(StatsScreen));
  stats_screen->root = lv_obj_create(NULL, NULL);
  stats_screen->label = lv_label_create(stats_screen->root, NULL);
  lv_label_set_text(stats_screen->label, "Stats screen");
  return stats_screen;
}

void handleStatsScreenButton(uint8_t button_num) {
    
}