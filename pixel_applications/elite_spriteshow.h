//header only elite_pixel_game_ente bare minimum template example application
#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"

#pragma once

typedef struct {
  char* app_name;
  elite_sprite_t *test_sprite;
  /*
  declare assets here like so

  void* some_user_asset;
  void* another_user_asset;
  int x,y,z;
  bool some_flag,another_flag;
  bool game_over_flag;

  */

}spriteshow_t;


bool spriteshow_on_user_update_entered_log=false;
bool spriteshow_on_user_update_leaving_log=false;
bool spriteshow_on_user_update_pre_particle_shower_update_log=false;


spriteshow_t* spriteshow_construct(elite_pixel_game_t* ente){
  elog("INFO : [spriteshow_construct] entering spriteshow_construct()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  elog("INFO : [spriteshow_construct] allocating self(spriteshow_t)\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  spriteshow_t *self=malloc(sizeof(spriteshow_t));
  if (self!=NULL) {
    elog("INFO : [spriteshow_construct] successfully allocated self(spriteshow_t)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else {
    elog("ERROR : [spriteshow_construct] failed to allocated self(spriteshow_t); returning NULL from constructor\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    return NULL;
  };
  self->app_name="spriteshow";//unused

  elite_sprite_config_t test_sprite_config={
    .width=10,
    .height=10,
    .uri="/littlefs/nes11.raw",
    .load_immediatly=true
  };
  self->test_sprite=elite_sprite_construct(test_sprite_config);
  if (self->test_sprite==NULL) {
    elog("ERROR : [spriteshow_construct] elite_sprite_construct() failed\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else{elog("INFO : [spriteshow_construct] elite_sprite_construct() success\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);};

  elog("INFO : [spriteshow_construct] successfully constructed self(spriteshow_t); returning self from constructor\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  return self;
};

bool spriteshow_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
//debug tracing in
    if (spriteshow_on_user_update_entered_log==false) {
        spriteshow_on_user_update_entered_log=true;
        elog("INFO : [spriteshow_update] entered spriteshow_update() - this notification will only occur once\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        };
//body
  spriteshow_t *self=(spriteshow_t*)params;
//  (void)self;
  elite_pixel_game_set_target_layer(ente,0);
  sfRGB c={0.0f,64.0f,0.0f};
  elite_pixel_game_fill_flayer(ente,c);

size_t i=0;

for (int y=0;y<self->test_sprite->height;y++) {
    for (int x=0;x<self->test_sprite->width;x++) {
        sRGB c=self->test_sprite->p_bitmap[i++];
        if (c.r>0||c.b>0||c.r>0) {
          sfRGB cf={c.r,c.g,c.b};
          elite_pixel_game_fputpixel(ente,x,y+10,cf);
        };
    }
};

//debug tracing out
  if (spriteshow_on_user_update_leaving_log==false) {
      spriteshow_on_user_update_leaving_log=true;
      elog("INFO : [spriteshow_on_user_update] leaving spriteshow_on_user_update() - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

  return true;
};



bool spriteshow_on_user_destroy(void* params){

    elog("INFO : [spriteshow_on_user_update] entering spriteshow_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    spriteshow_t *self=(spriteshow_t*)params;
    elog("INFO : [spriteshow_on_user_update] deallocating self(spriteshow)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    if (self!=NULL) {
        free(self);
        elog("INFO : [spriteshow_on_user_update] successfully deallocated self(spriteshow)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else {
        elog("ERROR : [spriteshow_on_user_update] failed to deallocated self(spriteshow); returning false from spriteshow_on_user_destroy()\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        return false;
    };
    elog("INFO : [spriteshow_on_user_update] returning true from template_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  return true;

};



void spriteshow_start_task(){

    elog("INFO : [main_start_pixel_game_task] entered main_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elog("INFO : [main_start_pixel_game_task] creating &pixel_game_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elite_pixel_game_config_t pixel_game_config={
        .app_name="spriteshow",
        .screen_width=10,
        .screen_height=30,
        .on_user_construct=(void*)&spriteshow_construct,
        .on_user_update=&spriteshow_on_user_update,
        .on_user_destroy=&spriteshow_on_user_destroy
    };
    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&pixel_game_config, 5, NULL);
    elog("INFO : [main_start_pixelapp_task] leaving main_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

};
