#pragma once

//header only elite_pixel_game_ente bare minimum template example application
//display shows screen filled with dark_green on invocation
#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"


typedef struct {
    float fx;
    float fy;
    float fvx;
    float fvy;
    bool is_locked;
    float fuse;
}ball_t;

typedef struct {
  char* app_name;

  ball_t ball;

}elite_pinball_t;


bool elite_pinball_on_user_update_entered_log=false;
bool elite_pinball_on_user_update_leaving_log=false;
bool elite_pinball_on_user_update_pre_particle_shower_update_log=false;
void elite_pinball_reset_ball(ball_t* p_ball){
  p_ball->fx=8.5f;
  p_ball->fy=25.5f;
  p_ball->is_locked=true;
  p_ball->fuse=3.0f;

};

elite_pinball_t* elite_pinball_construct(elite_pixel_game_t* ente){


  elite_pinball_t *self=malloc(sizeof(elite_pinball_t));
  if (self==NULL) {
    ELOG("ERROR : [elite_pinball_construct] failed to allocated self(elite_pinball_t); returning NULL from constructor\n");

    return NULL;
  };
  self->app_name="elite_pinball";//unused

  elite_pinball_reset_ball(&self->ball);


  return self;
};

void elite_pinball_draw_playfield(elite_pinball_t* self,elite_pixel_game_t* ente){
  sfRGBA flipper_col={172.0f,152.0f,136.0f,255.0f};
  sfRGBA bumper_col={152.0f,32.0f,72.0f,255.0f};
  elite_pixel_game_draw_filled_circle(ente, 2.5f,6.5f,0.75f,bumper_col);
  elite_pixel_game_draw_filled_circle(ente, 6.5f,6.5f,0.75f,bumper_col);
  elite_pixel_game_draw_filled_circle(ente, 4.5f,10.5f,0.75f,bumper_col);


  elite_pixel_game_fputpixelRGBA(ente,1.0f,0.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,2.0f,0.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,0.0f,1.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,0.0f,2.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,1.0f,1.0f,flipper_col);
//  elite_pixel_game_fputpixelRGBA(ente,0.0f,3.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,3.0f,0.0f,flipper_col);

  elite_pixel_game_fputpixelRGBA(ente,8.0f,0.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,7.0f,0.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,9.0f,1.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,9.0f,2.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,8.0f,1.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,6.0f,0.0f,flipper_col);
  //elite_pixel_game_fputpixelRGBA(ente,9.0f,3.0f,flipper_col);
elite_pixel_game_fputpixelRGBA(ente,5.0f,0.0f,flipper_col);
elite_pixel_game_fputpixelRGBA(ente,4.0f,0.0f,flipper_col);

  elite_pixel_game_fputpixelRGBA(ente,1.0f,26.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,2.0f,27.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,3.0f,27.0f,flipper_col);

  elite_pixel_game_fputpixelRGBA(ente,8.0f,26.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,7.0f,27.0f,flipper_col);
  elite_pixel_game_fputpixelRGBA(ente,6.0f,27.0f,flipper_col);

};

bool elite_pinball_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
//debug tracing in
    if (elite_pinball_on_user_update_entered_log==false) {
        elite_pinball_on_user_update_entered_log=true;
        ELOG("INFO : [elite_pinball_update] entered elite_pinball_update() - this notification will only occur once\n");

        };
//body
  elite_pinball_t *self=(elite_pinball_t*)params;
  (void)self;
  elite_pixel_game_set_target_layer(ente,0);
  sfRGBA c={0.0f,0.0f,24.0f,255.0f};
  elite_pixel_game_fill_flayerRGBA(ente,c);

  elite_pinball_draw_playfield(self,ente);
//debug tracing out
  if (elite_pinball_on_user_update_leaving_log==false) {
      elite_pinball_on_user_update_leaving_log=true;
      ELOG("INFO : [elite_pinball_on_user_update] leaving elite_pinball_on_user_update() - this notification will only occur once\n");

    };

  return true;
};



bool elite_pinball_on_user_destroy(void* params){

    ELOG("INFO : [elite_pinball_on_user_update] entering elite_pinball_on_user_destroy()\n");

    elite_pinball_t *self=(elite_pinball_t*)params;
    ELOG("INFO : [elite_pinball_on_user_update] deallocating self(elite_pinball)\n");


    if (self!=NULL) {
        free(self);
        ELOG("INFO : [elite_pinball_on_user_update] successfully deallocated self(elite_pinball)\n");

    }else {
        ELOG("ERROR : [elite_pinball_on_user_update] failed to deallocated self(elite_pinball); returning false from elite_pinball_on_user_destroy()\n");

        return false;
    };
    ELOG("INFO : [elite_pinball_on_user_update] returning true from template_on_user_destroy()\n");

  return true;

};



void elite_pinball_start_task(){

    ELOG("INFO : [elite_pinball_start_pixel_game_task] entered elite_pinball_start_pixelapp_task\n");

    ELOG("INFO : [elite_pinball_start_pixel_game_task] creating &pixel_game_task\n");

    elite_pixel_game_config_t pixel_game_config={
        .app_name="elite_pinball",
        .screen_width=10,
        .screen_height=30,
        .on_user_construct=(void*)&elite_pinball_construct,
        .on_user_update=&elite_pinball_on_user_update,
        .on_user_destroy=&elite_pinball_on_user_destroy
    };
    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&pixel_game_config, 5, NULL);
    ELOG("INFO : [elite_pinball_start_pixelapp_task] leaving elite_pinball_start_pixelapp_task\n");


};
