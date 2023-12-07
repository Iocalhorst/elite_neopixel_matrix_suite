//header only elite_pixel_game_ente bare minimum template example application
//display shows screen filled with dark_green on invocation
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


template_pixel_app_t* template_pixel_app_construct(elite_pixel_game_t* ente){
  ELOG("INFO : [template_pixel_app_construct] entering template_construct()\n");

  template_pixel_app_t *self=e_mall0c(__FUNCTION__,sizeof(template_pixel_app_t));
  if (self!=NULL) {
    ELOG("INFO : [template_pixel_app_construct] successfully allocated self(template_pixel_app_t)\n");

  }else {
    ELOG("ERROR : [template_pixel_app_construct] failed to allocated self(template_pixel_app_t); returning NULL from constructor\n");

    return NULL;
  };
  self->app_name="template_pixel_app";//unused
  ELOG("INFO : [template_pixel_app_construct] successfully constructed self(template_pixel_app_t); returning self from constructor\n");



  return self;
};

bool template_pixel_app_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
//debug tracing in
    if (template_pixel_app_on_user_update_entered_log==false) {
        template_pixel_app_on_user_update_entered_log=true;
        ELOG("INFO : [template_pixel_app_update] entered template_pixel_app_update() - this notification will only occur once\n");

        };
//body
  template_pixel_app_t *self=(template_pixel_app_t*)params;
  (void)self;
  elite_pixel_game_set_target_layer(ente,0);
  sfRGBA c={0.0f,64.0f,0.0f,255.0f};
  elite_pixel_game_fill_flayerRGBA(ente,c);


//debug tracing out
  if (template_pixel_app_on_user_update_leaving_log==false) {
      template_pixel_app_on_user_update_leaving_log=true;
      ELOG("INFO : [template_pixel_app_on_user_update] leaving template_pixel_app_on_user_update() - this notification will only occur once\n");

    };

  return true;
};



bool template_pixel_app_on_user_destroy(void* params){

    ELOG("INFO : [template_pixel_app_on_user_update] entering template_pixel_app_on_user_destroy()\n");

    template_pixel_app_t *self=(template_pixel_app_t*)params;
    ELOG("INFO : [template_pixel_app_on_user_update] deallocating self(template_pixel_app)\n");


    if (self!=NULL) {
        free(self);
        ELOG("INFO : [template_pixel_app_on_user_update] successfully deallocated self(template_pixel_app)\n");

    }else {
        ELOG("ERROR : [template_pixel_app_on_user_update] failed to deallocated self(template_pixel_app); returning false from template_pixel_app_on_user_destroy()\n");

        return false;
    };
    ELOG("INFO : [template_pixel_app_on_user_update] returning true from template_on_user_destroy()\n");

  return true;

};

elite_pixel_game_config_t template_pixel_app_config={
    .app_name="template_pixel_app",
    .screen_width=10,
    .screen_height=30,
    .on_user_construct=(void*)&template_pixel_app_construct,
    .on_user_update=&template_pixel_app_on_user_update,
    .on_user_destroy=&template_pixel_app_on_user_destroy
};

void template_pixel_app_start_task(){

    ELOG("INFO : [template_pixel_app_start_pixel_game_task] entered template_pixel_app_start_pixelapp_task\n");

    ELOG("INFO : [template_pixel_app_start_pixel_game_task] creating &pixel_game_task\n");


    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&template_pixel_app_config, 5, NULL);
    ELOG("INFO : [template_pixel_app_start_pixelapp_task] leaving template_pixel_app_start_pixelapp_task\n");


};
