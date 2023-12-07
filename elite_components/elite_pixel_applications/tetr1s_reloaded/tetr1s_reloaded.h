#include "elite_pixel_game_ente.h"
#include "elite_particle.h"
#include "elite.h"
#include "elite_tetris_block.h"
//#include "elite_tetris_agent.h"
#include "main.h"


#pragma once
#define MAX_BULLETS 20
typedef elite_particle_t bullet_t;

sfRGBA tetr1s_colors[10]={black2,red2,green2,blue2,yellow2,cyan2,magenta2,orange2,purple2,white2};
sfRGBA bullet_col={192.0f,182.0f,152.0f,192.0f};


typedef struct {
  float fx;
  float fy;
  float fvx;
  float fvy;
}spaceship_t;

typedef struct {
  char* app_name;
  elite_particle_shower_t* my_particle_shower;
  spaceship_t ship;
  elite_particle_t bullets[MAX_BULLETS];
  float gun_fuse;
  float gun_intervall;
}tetr1s_reloaded_t;


bool tetr1s_reloaded_on_user_update_entered_log=false;
bool tetr1s_reloaded_on_user_update_leaving_log=false;
bool tetr1s_reloaded_on_user_update_pre_particle_shower_update_log=false;


bool tetr1s_reloaded_bullet_is_gone(bullet_t *p_b){
  if (p_b->fx<0.0f||p_b->fx>=10.0f||p_b->fy<0.0f||p_b->fy>=30.0f) return true;
  return false;
};

void tetr1s_reloaded_bullets_clear(tetr1s_reloaded_t *self){
  for (int i=0;i<MAX_BULLETS;i++) {
    self->bullets[i].fx=-69.0f;
    self->bullets[i].fy=-69.0f;
  };
};

void tetr1s_reloaded_bullets_draw(tetr1s_reloaded_t* self,elite_pixel_game_t *ente){
  for (int i=0;i<MAX_BULLETS;i++){
    if (tetr1s_reloaded_bullet_is_gone(&self->bullets[i])) continue;
    int x=self->bullets[i].fx;
    int y=self->bullets[i].fy;
    sfRGBA c=self->bullets[i].col;
    elite_pixel_game_fputpixelRGBA(ente,x,y,c);
  };
};

void tetr1s_reloaded_bullets_update(tetr1s_reloaded_t* self,float fElapsedTime){
  for (int i=0;i<MAX_BULLETS;i++){
    if (tetr1s_reloaded_bullet_is_gone(&self->bullets[i])) continue;
    self->bullets[i].fx+=self->bullets[i].fvx*fElapsedTime;
    self->bullets[i].fy+=self->bullets[i].fvy*fElapsedTime;
  };
  return;
};

void tetr1s_reloaded_bullets_add(tetr1s_reloaded_t* self,bullet_t b){
  for (int i=0;i<MAX_BULLETS;i++){
    if (tetr1s_reloaded_bullet_is_gone(&self->bullets[i])) {
        self->bullets[i].fx=b.fx;
        self->bullets[i].fy=b.fy;
        self->bullets[i].fvx=b.fvx;
        self->bullets[i].fvy=b.fvy;
        self->bullets[i].col=b.col;
        return;
    };
  };
  ELOG("DEBUG : [%s] %s\b",__FUNCTION__,"out of bullets");
  return;
};

tetr1s_reloaded_t* tetr1s_reloaded_construct(elite_pixel_game_t* ente){
  ELOG("INFO : [tetr1s_reloaded_construct] entering tetr1s_reloaded_construct()\n");


  ELOG("INFO : [tetr1s_reloaded_construct] allocating self(tetr1s_reloaded_t)\n");

  tetr1s_reloaded_t *self=e_mall0c(__FUNCTION__,sizeof(tetr1s_reloaded_t));
  if (self!=NULL) {
    ELOG("INFO : [tetr1s_reloaded_construct] successfully allocated self(tetr1s_reloaded_t)\n");

  }else {
    ELOG("ERROR : [tetr1s_reloaded_construct] failed to allocated self(tetr1s_reloaded_t); returning NULL from constructor\n");

    return NULL;
  };
  self->app_name="tetr1s_reloaded";//unused
  self->ship.fx=5.0f;
  self->ship.fy=27.0f;
  self->ship.fvx=6.5f;
  self->ship.fvy=0.0f;
  self->gun_intervall=0.25f;
  self->gun_fuse=self->gun_intervall;
  tetr1s_reloaded_bullets_clear(self);

  ELOG("INFO : [tetr1s_reloaded_construct] successfully constructed self(tetr1s_reloaded_t); returning self from constructor\n");


  elite_particle_shower_config_t my_particle_shower_config={.num_particles=50};
  self->my_particle_shower=elite_particle_shower_construct(my_particle_shower_config);


  return self;
};

void tetr1s_reloaded_update_ship(tetr1s_reloaded_t* self,float fElapsedTime){
  float df=self->ship.fvx*fElapsedTime;
  float fposx=self->ship.fx+df;
  if (fposx>1.0f&&fposx<9.0f) {
    self->ship.fx=fposx;
  }else {self->ship.fvx*=-1.0f;};//just in case we
  if (self->ship.fx>9.0f) {self->ship.fx=9.0f;};
  if (self->ship.fx<1.0f) {self->ship.fx=1.0f;};

}

void tetr1s_reloaded_draw_ship(tetr1s_reloaded_t* self, elite_pixel_game_t* ente){
    sfRGBA c={172.0f,164.0f,152.0f,255.0f};
    int x=(int)self->ship.fx;
    int y=(int)self->ship.fy;
    elite_pixel_game_fputpixelRGBA(ente,x,y,c);
    elite_pixel_game_fputpixelRGBA(ente,x,y-1,c);
    elite_pixel_game_fputpixelRGBA(ente,x-1,y,c);
    elite_pixel_game_fputpixelRGBA(ente,x+1,y,c);
    elite_pixel_game_fputpixelRGBA(ente,x-1,y+1,c);
    elite_pixel_game_fputpixelRGBA(ente,x+1,y+1,c);
}

bool tetr1s_reloaded_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
  if (tetr1s_reloaded_on_user_update_entered_log==false) {
    tetr1s_reloaded_on_user_update_entered_log=true;
    ELOG("INFO : [tetr1s_reloaded_on_user_update] entered tetr1s_reloaded_on_user_update() - this notification will only occur once\n");

  };

  tetr1s_reloaded_t *self=(tetr1s_reloaded_t*)params;


  tetr1s_reloaded_update_ship(self,fElapsedTime);
  tetr1s_reloaded_bullets_update(self,fElapsedTime);

  self->gun_fuse-=fElapsedTime;
  if (self->gun_fuse<=0.0f) {
    self->gun_fuse=self->gun_intervall;
    bullet_t b={
      .fx=self->ship.fx,
      .fy=self->ship.fy-1.5f,
      .fvx=0.0f,
      .fvy=-20.0f,
      .col=bullet_col
    };
    tetr1s_reloaded_bullets_add(self,b);
  };

  elite_pixel_game_set_target_layer(ente,0);
  sfRGBA c={0.0f,0.0f,0.0f,255.0f};
  elite_pixel_game_fill_flayerRGBA(ente,c);

    if (tetr1s_reloaded_on_user_update_pre_particle_shower_update_log==false) {
      tetr1s_reloaded_on_user_update_pre_particle_shower_update_log=true;
      ELOG("INFO : [tetr1s_reloaded_on_user_update] calling particle_shower_update(fElapsedTime) - this notification will only occur once\n");

    };
  elite_particle_shower_update(self->my_particle_shower,fElapsedTime);

  elite_particle_shower_draw(ente,self->my_particle_shower);
  tetr1s_reloaded_draw_ship(self,ente);
  tetr1s_reloaded_bullets_draw(self,ente);

  if (tetr1s_reloaded_on_user_update_leaving_log==false) {
      tetr1s_reloaded_on_user_update_leaving_log=true;
      ELOG("INFO : [tetr1s_reloaded_on_user_update] leaving tetr1s_reloaded_on_user_update() - this notification will only occur once\n");

    };


  return true;
};

//todo : proper cleanup
bool tetr1s_reloaded_on_user_destroy(void* params){


    ELOG("INFO : [tetr1s_reloaded_on_user_update] entering tetr1s_reloaded_destroy()\n");

    tetr1s_reloaded_t *self=(tetr1s_reloaded_t*)params;
    ELOG("INFO : [tetr1s_reloaded_on_user_update] deallocating self(tetr1s_reloaded)\n");

    if (self!=NULL) {

        elite_particle_shower_destruct(self->my_particle_shower);
        free(self);
    ELOG("INFO : [tetr1s_reloaded_on_user_update] successfully deallocated self(tetr1s_reloaded)\n");

  }else {
    ELOG("ERROR : [tetr1s_reloaded_on_user_update] failed to deallocated self(tetr1s_reloaded); returning false from on_user_destroy()\n");

    return false;
  };
  ELOG("INFO : [tetr1s_reloaded_on_user_update] returning true from tetr1s_reloaded_on_user_destroy()\n");

  return true;
};

elite_pixel_game_config_t tetr1s_reloaded_config={
    .app_name="tetr1s_reloaded_pixel_app",
    .screen_width=10,
    .screen_height=30,
    .on_user_construct=(void*)&tetr1s_reloaded_construct,
    .on_user_update=&tetr1s_reloaded_on_user_update,
    .on_user_destroy=&tetr1s_reloaded_on_user_destroy
};

void tetr1s_reloaded_start_task(){

    ELOG("INFO : [main_start_pixel_game_task] entered main_start_pixelapp_task\n");

    ELOG("INFO : [main_start_pixel_game_task] creating &pixel_game_task\n");
    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&tetr1s_reloaded_config, 5, NULL);

    ELOG("INFO : [main_start_pixelapp_task] leaving tetris_reloaded_start_task\n");


};
