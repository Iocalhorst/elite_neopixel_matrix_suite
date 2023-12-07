//simple header only elite_pixel_game_ente application implementation
#include "elite_pixel_game_ente.h"
#include "elite_particle.h"
#include "elite.h"
#include "elite_tetris_agent.h"
#include "main.h"


#pragma once

typedef struct {
  char* app_name;
  elite_particle_shower_t* my_particle_shower;
}rain_app_t;


bool on_user_update_entered_log=false;
bool on_user_update_leaving_log=false;
bool on_user_update_pre_particle_shower_update_log=false;

rain_app_t* rain_app_construct(elite_pixel_game_t* ente){
  ELOG("INFO : [rain_app_construct] entering rain_app_construct()\n");


  ELOG("INFO : [rain_app_construct] allocating self(rain_app_t)\n");

  rain_app_t *self=e_mall0c(__FUNCTION__,sizeof(rain_app_t));
  if (self!=NULL) {
    ELOG("INFO : [rain_app_construct] successfully allocated self(rain_app_t)\n");

  }else {
    ELOG("ERROR : [rain_app_construct] failed to allocated self(rain_app_t); returning NULL from constructor\n");

    return NULL;
  };
  self->app_name="rain app";//unused
  ELOG("INFO : [rain_app_construct] successfully constructed self(rain_app_t); returning self from constructor\n");


  elite_particle_shower_config_t my_particle_shower_config={.num_particles=50};
  self->my_particle_shower=elite_particle_shower_construct(my_particle_shower_config);


  return self;
};

bool rain_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
  if (on_user_update_entered_log==false) {
    on_user_update_entered_log=true;
    ELOG("INFO : [rain_on_user_update] entered rain_on_user_update() - this notification will only occur once\n");

  };

  rain_app_t *self=(rain_app_t*)params;



  elite_pixel_game_set_target_layer(ente,0);
  sfRGBA c={0.0f,0.0f,0.0f,255.0f};
  elite_pixel_game_fill_flayerRGBA(ente,c);

    if (on_user_update_pre_particle_shower_update_log==false) {
      on_user_update_pre_particle_shower_update_log=true;
      ELOG("INFO : [rain_on_user_update] calling particle_shower_update(fElapsedTime) - this notification will only occur once\n");

    };
  elite_particle_shower_update(self->my_particle_shower,fElapsedTime);

  elite_particle_shower_draw(ente,self->my_particle_shower);
  if (on_user_update_leaving_log==false) {
      on_user_update_leaving_log=true;
      ELOG("INFO : [rain_on_user_update] leaving rain_on_user_update() - this notification will only occur once\n");

    };

  return true;
};

//todo : proper cleanup
bool rain_on_user_destroy(void* params){


    ELOG("INFO : [rain_on_user_update] entering rain_app_destroy()\n");

    rain_app_t *self=(rain_app_t*)params;
    ELOG("INFO : [rain_on_user_update] deallocating self(rain_app)\n");

    if (self!=NULL) {

        elite_particle_shower_destruct(self->my_particle_shower);
        free(self);
    ELOG("INFO : [rain_on_user_update] successfully deallocated self(rain_app)\n");

  }else {
    ELOG("ERROR : [rain_on_user_update] failed to deallocated self(rain_app); returning false from on_user_destroy()\n");

    return false;
  };
  ELOG("INFO : [rain_on_user_update] returning true from rain_on_user_destroy()\n");

  return true;
};


elite_pixel_game_config_t elite_rain_config={
    .app_name="rain_pixel_app",
    .screen_width=10,
    .screen_height=30,
    .on_user_construct=(void*)&rain_app_construct,
    .on_user_update=&rain_on_user_update,
    .on_user_destroy=&rain_on_user_destroy
};

void rain_start_task(){

    ELOG("INFO : [main_start_pixel_game_task] entered main_start_pixelapp_task\n");

    ELOG("INFO : [main_start_pixel_game_task] creating &pixel_game_task\n");

    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&elite_rain_config, 5, NULL);
    ELOG("INFO : [main_start_pixelapp_task] leaving main_start_pixelapp_task\n");


};
