#include "elite_pixel_game_ente.h"
#include "elite_particle.h"
#include "elite.h"
#include "main.h"


#pragma once

typedef struct {
  char* app_name;
  particle_shower_t* my_particle_shower;
}rain_app_t;



rain_app_t* rain_app_construct(){
  elog("INFO : [rain_app_construct] entering rain_app_construct()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  elog("INFO : [rain_app_construct] allocating self(rain_app_t)\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  rain_app_t *self=malloc(sizeof(rain_app_t));
  if (self!=NULL) {
    elog("INFO : [rain_app_construct] successfully allocated self(rain_app_t)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else {
    elog("ERROR : [rain_app_construct] failed to allocated self(rain_app_t); returning NULL from constructor\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    return NULL;
  };
  self->app_name="rain app";//unused
  elog("INFO : [rain_app_construct] successfully constructed self(rain_app_t); returning self from constructor\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  particle_shower_config_t my_particle_shower_config={.num_particles=50};
  self->my_particle_shower=particle_shower_construct(my_particle_shower_config);


  return self;
};

//self->particle_shower=particle_shower_construct();
bool on_user_update_entered_log=false;
bool on_user_update_leaving_log=false;
bool on_user_update_pre_particle_shower_update_log=false;
bool rain_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
  if (on_user_update_entered_log==false) {
    on_user_update_entered_log=true;
    elog("INFO : [rain_on_user_update] entered rain_on_user_update() - this notification will only occur once\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };

  rain_app_t *self=(rain_app_t*)params;



  elite_pixel_game_set_target_layer(ente,0);
  sfRGB c={0.0f,0.0f,0.0f};
  elite_pixel_game_fill_flayer(ente,c);

    if (on_user_update_pre_particle_shower_update_log==false) {
      on_user_update_pre_particle_shower_update_log=true;
      elog("INFO : [rain_on_user_update] calling particle_shower_update(fElapsedTime) - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };
  particle_shower_update(self->my_particle_shower,fElapsedTime);

  particle_shower_draw(ente,self->my_particle_shower);
  if (on_user_update_leaving_log==false) {
      on_user_update_leaving_log=true;
      elog("INFO : [rain_on_user_update] leaving rain_on_user_update() - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

  return true;
};

bool rain_on_user_destroy(void* params){
  elog("INFO : [rain_on_user_update] entering rain_app_destroy()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  rain_app_t *self=(rain_app_t*)params;
  elog("INFO : [rain_on_user_update] deallocating self(rain_app)\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  if (self!=NULL) {
    free(self);
    elog("INFO : [rain_on_user_update] successfully deallocated self(rain_app)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else {
    elog("ERROR : [rain_on_user_update] failed to deallocated self(rain_app); returning false from on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    return false;
  };
  elog("INFO : [rain_on_user_update] returning true from rain_on_user_destroy()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  return true;
};
