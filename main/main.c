#define TAG "EliteProject"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
//#include "freertos/timers.h"
#include "esp_timer.h"
#include "led_strip.h"

#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "lwip/opt.h"

#include "monster.h"
#include "mr_display.h"


#include "defines.h"
#include "elite.h"
#include "elite_pixel_game_ente.h"
#include "elite_particle.h"
#include "elite_rain.h"
#include "tcp_server.h"
#include "elite_shell.h"
#include "main.h"
//static const char *TAG = "EliteProject Main";


static void main_eye_task1(void* args){
    int random_delay=minimum_eyes_delay+esp_random()%maximum_eyes_add_delay;
      while (true) {
        vTaskDelay(random_delay / portTICK_PERIOD_MS);
        //elog("blink\n");
        monsters[0].eyes_direction=monsters_get_new_eye_direction(monsters[0].eyes_direction);

        random_delay=minimum_eyes_delay+esp_random()%maximum_eyes_add_delay;
      };
};

static void main_eye_task2(void* args){
    int random_delay=minimum_eyes_delay+esp_random()%maximum_eyes_add_delay;
      while (true) {
        vTaskDelay(random_delay / portTICK_PERIOD_MS);
        //elog("blink\n");
        monsters[1].eyes_direction=monsters_get_new_eye_direction(monsters[1].eyes_direction);
        random_delay=minimum_eyes_delay+esp_random()%maximum_eyes_add_delay;
      };
};

static void main_eye_task3(void* args){
    int random_delay=minimum_eyes_delay+esp_random()%maximum_eyes_add_delay;
      while (true) {
        vTaskDelay(random_delay / portTICK_PERIOD_MS);
        //elog("blink\n");
        monsters[2].eyes_direction=monsters_get_new_eye_direction(monsters[2].eyes_direction);
        random_delay=minimum_eyes_delay+esp_random()%maximum_eyes_add_delay;
      };
};



void main_draw_background(){
  for (int yy=0;yy<30;yy++){
    for (int xx=0;xx<10;xx++){
        mr_display_put_pixel(xx,yy,dark_green);
    };
  };

};
void main_clear_display(){
  for (int yy=0;yy<30;yy++){
    for (int xx=0;xx<10;xx++){
        mr_display_put_pixel(xx,yy,black);
    };
  };

};

void main_draw_monsters(){
  for (int i=0;i<NUM_MONSTERS;i++) {
    int offset_y=i*10;
    for (int y=0;y<10;y++){
      for (int x=0;x<10;x++){
        int ci=monster_bitmap[x+y*10];
        if (ci!=0) {
          sRGB c={monsters[i].cols[ci].r,monsters[i].cols[ci].g,monsters[i].cols[ci].b};
          mr_display_put_pixel(x,y+offset_y,c);
        };
      };
    };
    if (monsters[i].eyes_direction==UP_LEFT) {
        mr_display_put_pixel(3,offset_y+3,black);
        mr_display_put_pixel(6,offset_y+3,black);
    };
    if (monsters[i].eyes_direction==UP_CENTER) {
        mr_display_put_pixel(4,offset_y+3,black);
        mr_display_put_pixel(6,offset_y+3,black);
    };
    if (monsters[i].eyes_direction==UP_RIGHT) {
        mr_display_put_pixel(4,offset_y+3,black);
        mr_display_put_pixel(7,offset_y+3,black);
    };
    if (monsters[i].eyes_direction==DOWN_LEFT) {
        mr_display_put_pixel(3,offset_y+4,black);
        mr_display_put_pixel(6,offset_y+4,black);
    };
    if (monsters[i].eyes_direction==DOWN_CENTER) {
        mr_display_put_pixel(4,offset_y+4,black);
        mr_display_put_pixel(6,offset_y+4,black);
    };
    if (monsters[i].eyes_direction==DOWN_RIGHT) {
        mr_display_put_pixel(4,offset_y+4,black);
        mr_display_put_pixel(7,offset_y+4,black);
    };

  };
};

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

void main_simple_ota_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA task");

    esp_http_client_config_t config = {
        .url = "http://192.168.0.249/eliteProject_1_0_1.bin",
        .event_handler = _http_event_handler,
        .keep_alive_enable = true

    };
    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    char log_str[128]={0};
    sprintf(log_str,"INFO : [main_simple_ota_task] Attempting to download update from %s\n",config.url);
    elog(log_str);
    ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
        elog("INFO : [main_simple_ota_task] OTA Firmware updated succeeded, rebooting...\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
        elog("ERROR : [main_simple_ota_task] Firmware upgrade failed\n");
    }

while(ota_timer>0){
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ota_timer-=1000;
  };
  elog("INFO : [main_simple_ota_task] stopping main_simple_ota_task \n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
ota_has_stopped=true;
vTaskDelete(NULL);

};


void main_update_fElapsedTime_global(){
  int64_t now=esp_timer_get_time();
  fElapsedTime_global=(float)(now-lastTimeStampInMicroSeconds);
  fElapsedTime_global*=0.000001f;
  accumulated_fElapsedTime_global+=fElapsedTime_global;
  lastTimeStampInMicroSeconds=now;
};


void main_timer_log_task(void *pvParameter){
  while (true) {
    char time_str[256]={0};
    sprintf(time_str,"INFO : [main_timer_log_task] time=%f\n",accumulated_fElapsedTime_global);
    elog(time_str);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  };
};

void main_pixel_game_task(void* params){
  main_theres_a_pixel_game_running=true;
  main_kill_pixel_game=false;
  elite_pixel_game_config_t *p_pixel_game_config=(elite_pixel_game_config_t*)params;

  elite_pixel_game_config_t pixel_game_config={
    .app_name=p_pixel_game_config->app_name,
    .screen_width=p_pixel_game_config->screen_width,
    .screen_height=p_pixel_game_config->screen_height,
    .on_user_construct=p_pixel_game_config->on_user_construct,
    .on_user_update=p_pixel_game_config->on_user_update,
    .on_user_destroy=p_pixel_game_config->on_user_destroy
  };

  //p_pixel_game_config_t pixel_game_config=(pixel_game_config_t)params;
  //to not fuck up the update window at startup
  while(ota_has_stopped==false){
    vTaskDelay(500 / portTICK_PERIOD_MS);
  };

  vTaskDelay(500 / portTICK_PERIOD_MS);
  elog("INFO : [main_pixel_game_task] started\n");
  //int32_t pixelapp_runtime_limit_ms=60000;
  int32_t pixelapp_runtime_ms=0;


  elog("INFO : [main_pixel_game_task] calling elite_pixel_game_construct(pixel_app_config\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  elite_pixel_game_t *p_pixel_game=elite_pixel_game_construct(pixel_game_config);

  elog("INFO : [main_pixel_game_task] finished constructing pixelapp\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  lastTimeStampInMicroSeconds=esp_timer_get_time();


  while (true){

    main_update_fElapsedTime_global();
    elite_pixel_game_update(p_pixel_game,fElapsedTime_global);
    for (size_t i=0;i<height*width;i++) {pixels[i]=p_pixel_game->p_frame_buf[i];}
    mr_display_update_leds();
    vTaskDelay(20/ portTICK_PERIOD_MS);
    pixelapp_runtime_ms+=20;
    if (main_kill_pixel_game==true) break;
  };

  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elog("INFO : [rain_app_task] calling elite_pixel_game_destruct()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  if (elite_pixel_game_destruct(p_pixel_game)==false) {
      elog("ERROR : [rain_app_task] elite_pixel_game_destruct(p_pixel_app) returned false\n");
    }else{
      elog("INFO : [rain_app_task] elite_pixel_game_destruct(p_pixel_app) returned true\n");
  };
  vTaskDelay( log_delay/ portTICK_PERIOD_MS);
  elog("INFO : [rain_app_task] killing rain_app_task\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  main_theres_a_pixel_game_running=false;
  main_kill_pixel_game=false;
  //exit_condition=true;
  vTaskDelete(NULL);

};

void main_start_ota_task(){
  elog("INFO : [main_start_ota_task] entered main_start_ota_task\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  elog("INFO : [main_start_ota_task] creating &simple_ota task\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  xTaskCreate(&main_simple_ota_task, "main_simple_ota_task", 8192, NULL, 5, NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main_start_ota_task] leaving main_start_ota_task\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

};


void main_start_eyes_tasks(){
  elog("INFO : [main_start_eyes_task] entered main_start_eyes_tasks\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elog("INFO : [main_start_eyes_task] starting eyes_task1\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  xTaskCreate(&main_eye_task1, "eye_task1", 4096, NULL, 5, NULL);
  elog("INFO : [main_start_eyes_task] starting eyes_task2\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  xTaskCreate(&main_eye_task2, "eye_task2", 4096, NULL, 5, NULL);
  elog("INFO : [main_start_eyes_task] starting eyes_task3\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  xTaskCreate(&main_eye_task3, "eye_task3", 4096, NULL, 5, NULL);
  elog("INFO : [main_start_eyes_task] starting timer_log_task\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  xTaskCreate(&main_timer_log_task, "timer_log_task", 4096, NULL, 5, NULL);
  elog("INFO : [main_start_eyes_task] leaving main_start_eyes_tasks\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
};


void main_reboot(){

  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  ESP_LOGI(TAG, "rebooting in 10\n");
  elog("INFO : [main] rebooting in 10\n");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main] rebooting in 9\n");
  ESP_LOGI(TAG, "9");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main] rebooting in 8\n");
  ESP_LOGI(TAG, "8");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main] rebooting in 7\n");
  ESP_LOGI(TAG, "7");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main] rebooting in 6\n");
  ESP_LOGI(TAG, "6");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main] rebooting in 5\n");
  ESP_LOGI(TAG, "5");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main] rebooting in 4\n");
  ESP_LOGI(TAG, "4");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main] rebooting in 3\n");
  ESP_LOGI(TAG, "3");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main] rebooting in 2\n");
  ESP_LOGI(TAG, "2");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  elog("INFO : [main] rebooting in 1\n");
  ESP_LOGI(TAG, "1");
  vTaskDelay(500 / portTICK_PERIOD_MS);
  elog("INFO : [main] Don't forget to like, share and subscribe!\n");
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ESP_LOGI(TAG, "rebooting in 0");
  vTaskDelay(500 / portTICK_PERIOD_MS);
  //just for good measure
  free(global_log_buffer);

        esp_restart();
};



void main_start_pixel_game_task(){
  elog("INFO : [main_start_pixel_game_task] entered main_start_pixelapp_task\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elog("INFO : [main_start_pixel_game_task] creating &pixel_game_task\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elite_pixel_game_config_t pixel_game_config={
    .app_name="rain_pixel_app",
    .screen_width=10,
    .screen_height=30,
    .on_user_construct=(void*)&rain_app_construct,
    .on_user_update=&rain_on_user_update,
    .on_user_destroy=&rain_on_user_destroy
  };

  //elog("starting pixel_app task\n");
  //xTaskCreate(&pixel_app_task, "pixel_app_task", 4096, NULL, 5, NULL);
  xTaskCreate(&main_pixel_game_task, "main_pixel_game_task", 4096,&pixel_game_config, 5, NULL);
  elog("INFO : [main_start_pixelapp_task] leaving main_start_pixelapp_task\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

};



void app_main(void){

    elite_init_nvs();
    elite_init_wifi();
    esp_err_t err= esp_timer_early_init();
    lastTimeStampInMicroSeconds=esp_timer_get_time();

    ESP_ERROR_CHECK(err);
    ESP_LOGI(TAG, "Waiting 3 seconds for reasons");
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    led_strip = configure_led_spi();

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    elite_logger_task_params_t elite_logger_task_params={
      .log_socket=sock,
    };


    xTaskCreate(elite_logger_task, "elite_logger_task", 4096, &elite_logger_task_params, 5, &p_elite_logger_task_handle);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    elog("\n------------hello again-------------\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    elog("INFO : Hier koennte ihre werbung stehen\n");
    main_start_ota_task();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    main_start_eyes_tasks();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreate(tcp_server_task, "tcp_server", 4096*2, (void*)AF_INET, 5, NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    bool main_exit_condition=false;
    elog("INFO : [main] starting main monster loop\n");


    while (main_exit_condition!=true) {              //xTaskNotify(p_elite_logger_task_handle,TEST_LOG_CMD,3);//3==eSetValueWithOverwrite

        if (!main_theres_a_pixel_game_running) {
              main_clear_display();
              main_draw_background();
              main_draw_monsters();
              vTaskDelay(10 / portTICK_PERIOD_MS);
              mr_display_update_leds();
        };
    };

   main_reboot();

}
