#pragma once
void main_start_pixel_game_task();
void main_reboot();
static bool main_kill_pixel_game=false;
static bool main_theres_a_pixel_game_running=false;
float fElapsedTime_global=0.0f;
float accumulated_fElapsedTime_global=0.0f;
int64_t lastTimeStampInMicroSeconds=0;
static int ota_timer=15000;
static bool ota_has_stopped=false;
