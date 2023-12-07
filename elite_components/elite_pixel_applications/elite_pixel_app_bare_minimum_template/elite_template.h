#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"

#pragma once
#define APP_NAME elite_template

typedef struct {
  char* app_name;
}APP_NAME_t;



APP_NAME_t* APP_NAME_construct(elite_pixel_game_t* ente){


  APP_NAME_t *self=e_mall0c(__FUNCTION__,sizeof(APP_NAME_t));
  if (self==NULL){
    ELOG("ERROR : [APP_NAME_construct] e_mall0c fail\n");
    return NULL;
  };

  self->app_name="APP_NAME";
    return self;
};

bool APP_NAME_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
  APP_NAME_t *self=(APP_NAME_t*)params;
  elite_pixel_game_set_target_layer(ente,0);
  sfRGBA c={0.0f,64.0f,0.0f,255.0f};
  elite_pixel_game_fill_flayerRGBA(ente,c);
  return true;
};


bool APP_NAME_on_user_destroy(void* params){
    APP_NAME_t *self=(APP_NAME_t*)params;

    if (self!=NULL) {free(self);return false;};

    return true;

};


elite_pixel_game_config_t template_config={
    .app_name="APP_NAME",
    .screen_width=10,
    .screen_height=30,
    .on_user_construct=(void*)&APP_NAME_construct,
    .on_user_update=&APP_NAME_on_user_update,
    .on_user_destroy=&APP_NAME_on_user_destroy
};

void APP_NAME_start_task(){


    xTaskCreate(&elite_pixel_game_task, "APP_NAME_task", 4096,&template_config, 5, NULL);



};
