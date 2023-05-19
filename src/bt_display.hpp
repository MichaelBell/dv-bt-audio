void bt_display_init();
void bt_display_set_samples(int16_t * samples, uint16_t num_samples);
void bt_display_set_track_title(char* title);
void bt_display_set_track_artist(char* artist);
void bt_display_set_track_album(char* album);
void bt_display_set_cover_art(const uint8_t * cover_data, uint32_t cover_len);