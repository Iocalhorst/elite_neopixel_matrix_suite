//header only elite_pixel_game_ente bare minimum template example application
#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"

#pragma once

typedef struct {
  char* app_name;

  /*
  declare assets here like so

  void* some_user_asset;
  void* another_user_asset;
  int x,y,z;
  bool some_flag,another_flag;
  bool game_over_flag;

  */

}template_pixel_app_t;


bool template_pixel_app_on_user_update_entered_log=false;
bool template_pixel_app_on_user_update_leaving_log=false;
bool template_pixel_app_on_user_update_pre_particle_shower_update_log=false;


template_pixel_app_t* template_pixel_app_construct(){
  elog("INFO : [template_construct] entering template_construct()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  elog("INFO : [template_app_construct] allocating self(template_t)\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  template_pixel_app_t *self=malloc(sizeof(template_pixel_app_t));
  if (self!=NULL) {
    elog("INFO : [template_construct] successfully allocated self(template_t)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else {
    elog("ERROR : [template_construct] failed to allocated self(template_t); returning NULL from constructor\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    return NULL;
  };
  self->app_name="template_pixel_app";//unused
  elog("INFO : [template_pixel_app_construct] successfully constructed self(template_t); returning self from constructor\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);


  return self;
};

bool template_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
//debug tracing in
    if (template_on_user_update_entered_log==false) {
        template_on_user_update_entered_log=true;
        elog("INFO : [template_pixel_app_update] entered template_pixel_app_update() - this notification will only occur once\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        };
//body
  template_app_t *self=(template_app_t*)params;

  elite_pixel_game_set_target_layer(ente,0);
  sfRGB c={0.0f,0.0f,0.0f};
  elite_pixel_game_fill_flayer(ente,c);


//debug tracing out
  if (template_on_user_update_leaving_log==false) {
      template_on_user_update_leaving_log=true;
      elog("INFO : [template_pixel_app_on_user_update] leaving template_pixel_app_on_user_update() - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

  return true;
};



bool template_pixel_app_on_user_destroy(void* params){

    elog("INFO : [template_pixel_app_on_user_update] entering template_pixel_app_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    template_t *self=(template_t*)params;
    elog("INFO : [template_pixel_app_on_user_update] deallocating self(template_pixel_app)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    if (self!=NULL) {
        free(self);
        elog("INFO : [template_pixel_app_on_user_update] successfully deallocated self(template_pixel_app)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else {
        elog("ERROR : [template_pixel_app_on_user_update] failed to deallocated self(template_pixel_app); returning false from template_pixel_app_on_user_destroy()\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        return false;
    };
    elog("INFO : [template_pixel_app_on_user_update] returning true from template_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  return true;

};



void template_pixel_app_start_task(){

    elog("INFO : [main_start_pixel_game_task] entered main_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elog("INFO : [main_start_pixel_game_task] creating &pixel_game_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elite_pixel_game_config_t pixel_game_config={
        .app_name="template_pixel_app",
        .screen_width=10,
        .screen_height=30,
        .on_user_construct=(void*)&template_pixel_app_construct,
        .on_user_update=&template_pixel_app_on_user_update,
        .on_user_destroy=&template_pixel_app_on_user_destroy
    };
    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&pixel_game_config, 5, NULL);
    elog("INFO : [main_start_pixelapp_task] leaving main_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

};
