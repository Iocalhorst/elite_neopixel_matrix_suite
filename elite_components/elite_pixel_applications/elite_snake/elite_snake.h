#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"

#pragma once

#define DIR_UP 0
#define DIR_LEFT 1
#define DIR_DOWN 2
#define DIR_RIGHT 3
#define WALL 255
#define HEAD 1
#define EMPTY 0
#define FOOD -1




typedef struct {
  char* app_name;
  int *playfield;
  int height,width;
  int snake_len;
  int snake_head_x,snake_head_y;
  int snake_dir;
  bool snake_fail;
  float tick_intervall;
  float fuse;
  int food_x;
  int food_y;
}elite_snake_t;


bool elite_snake_on_user_update_entered_log=false;
bool elite_snake_on_user_update_leaving_log=false;
bool elite_snake_on_user_update_pre_particle_shower_update_log=false;


sfRGBA col_empty={0.0f,0.0f,0.0f,255.0f};
sfRGBA col_head={111.0f,212.0f,32.0f,255.0f};
sfRGBA col_tail={24.0f,129.0f,16.0f,255.0f};
sfRGBA col_food={222.0f,72.0f,32.0f,255.0f};
sfRGBA col_wall={172.0f,152.0f,112.0f,255.0f};

elite_snake_t* elite_snake_construct(elite_pixel_game_t* ente);
bool elite_snake_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime);
void elite_snake_reset_game(elite_snake_t* self);
void elite_snake_advance(elite_snake_t *self);
void elite_snake_draw_playfield(elite_snake_t *self,elite_pixel_game_t *ente);


void elite_snake_eat(elite_snake_t* self){
  int idx;
  self->snake_len+=1;
  while(true) {
      self->food_x=esp_random()%self->width;
      self->food_y=esp_random()%self->height;
      if (self->food_x>1&&self->food_x<8&&self->food_y>2&&self->food_y<27) {
        idx=self->width*self->food_y+self->food_x;
        if (self->playfield[idx]==EMPTY) {
          self->playfield[idx]=FOOD;
          break;
        }
      }
  };


};

void elite_snake_advance(elite_snake_t *self){

    int idx;
    for (int y=1;y<self->height-1;y++) {
        for (int x=1;x<self->width-1;x++) {
              idx=self->width*y+x;
              if (self->playfield[idx]>0) {
                  self->playfield[idx]+=1;
                  if (self->playfield[idx]>self->snake_len) {
                      self->playfield[idx]=EMPTY;
                  };
              };

        };
    };
    switch (self->snake_dir) {
      case DIR_UP : {self->snake_head_y-=1;break;};
      case DIR_DOWN : {self->snake_head_y+=1;break;};
      case DIR_LEFT : {self->snake_head_x-=1;break;};
      case DIR_RIGHT : {self->snake_head_x+=1;break;};
      default : {elog("DEBUG : [elite_snake_advance] UNREACHABLE");};
    };

    idx=self->width*self->snake_head_y+self->snake_head_x;
    if (self->playfield[idx]>EMPTY) {
      self->snake_fail=true;
    };
    if (self->playfield[idx]<EMPTY) {
      elite_snake_eat(self);
    };

    self->playfield[idx]=HEAD;

};

elite_snake_t* elite_snake_construct(elite_pixel_game_t* ente){
  elog("INFO : [elite_snake_construct] entering elite_snake_construct()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  elog("INFO : [elite_snake_construct] allocating self(elite_snake_t)\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elite_snake_t *self=(elite_snake_t*)malloc(sizeof(elite_snake_t));
  if (self!=NULL) {
    elog("INFO : [elite_snake_construct] successfully allocated self(elite_snake_t)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else {
    elog("ERROR : [elite_snake_construct] failed to allocated self(elite_snake_t); returning NULL from constructor\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    return NULL;
  };
  self->app_name="elite_snake";
  self->tick_intervall=0.033f;
  self->height=30;
  self->width=10;
  self->playfield=(int*)malloc(sizeof(int)*self->height*self->width);

  elog("INFO : [elite_snake_construct] successfully constructed self(elite_snake_t); returning self from constructor\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);


  return self;
};

void elite_snake_turn(elite_snake_t* self){
  self->snake_dir+=1;
  self->snake_dir%=4;
};

void elite_snake_reset_game(elite_snake_t* self){
  elog("DEBUG : [elite_snake_reset_game] entered elite_snake_reset_game()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  self->fuse=2.0f;
  int field_t;
  int idx=0;
  for (int y=0;y<self->height;y++) {
    for (int x=0;x<self->width;x++) {
      if (x*y==0||x==self->width-1||y==self->height-1) {field_t=WALL;}
      else field_t=EMPTY;
      idx=self->width*y+x;
      self->playfield[idx]=field_t;
    };
  };
  self->food_x=3;
  self->food_y=3;
  self->snake_head_x=7;
  self->snake_head_y=26;
  self->playfield[self->width*self->snake_head_y+self->snake_head_x]=HEAD;
  self->playfield[self->width*self->food_y+self->food_x]=FOOD;
  self->snake_dir=DIR_UP;
  self->snake_fail=false;
  self->snake_len=3;
  for (int i=0;i<self->snake_len;i++) elite_snake_advance(self);
  elog("DEBUG : [elite_snake_reset_game] leaving elite_snake_reset_game()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);


}

void elite_snake_draw_playfield(elite_snake_t* self,elite_pixel_game_t*ente){
    sfRGBA col;
    for (int y=0;y<self->height;y++) {
        for (int x=0;x<self->width;x++) {
            col=col_empty;
            int idx=self->width*y+x;
            int field_t=self->playfield[idx];
            col=col_tail;
            if (field_t==HEAD) col=col_head;
            if (field_t==WALL) col=col_wall;
            if (field_t==EMPTY) col=col_empty;
            if (field_t==FOOD) col=col_food;
            elite_pixel_game_fputpixelRGBA(ente,x,y,col);
        }
    };

};

void elite_snake_option(elite_snake_t *self){
  bool turn=false;

  int x=self->snake_head_x;
  int y=self->snake_head_y;
  if (self->snake_dir==DIR_UP) y-=1;
  if (self->snake_dir==DIR_DOWN) y+=1;
  if (self->snake_dir==DIR_LEFT) x-=1;
  if (self->snake_dir==DIR_RIGHT) x+=1;
  int idx=self->width*y+x;

  if ((self->snake_dir==DIR_UP||self->snake_dir==DIR_DOWN)&&self->food_y==self->snake_head_y) {turn=true;};
  if ((self->snake_dir==DIR_LEFT||self->snake_dir==DIR_RIGHT)&&self->food_x==self->snake_head_x) {turn=true;};
  if (self->playfield[idx]>0) {turn=true;};

  if (turn==true) {elite_snake_turn(self);};
};

bool elite_snake_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
//debug tracing in
  if (elite_snake_on_user_update_entered_log==false) {
      elite_snake_on_user_update_entered_log=true;
      elog("INFO : [elite_snake_update] entered elite_snake_update() - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };
//body
  elite_snake_t *self=(elite_snake_t*)params;
  elite_pixel_game_set_target_layer(ente,0);
  sfRGBA c={0.0f,0.0f,0.0f,255.0f};
  elite_pixel_game_fill_flayerRGBA(ente,c);

  if (self->snake_fail) {elite_snake_reset_game(self);};



  self->fuse-=fElapsedTime;
  if (self->fuse<=0.0f) {
      self->fuse=self->tick_intervall;
      elite_snake_option(self);
      elite_snake_advance(self);
  };

  elite_snake_draw_playfield(self,ente);


//debug tracing out
  if (elite_snake_on_user_update_leaving_log==false) {
      elite_snake_on_user_update_leaving_log=true;
      elog("INFO : [elite_snake_on_user_update] leaving elite_snake_on_user_update() - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

  return true;
};



bool elite_snake_on_user_destroy(void* params){

    elog("INFO : [elite_snake_on_user_update] entering elite_snake_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elite_snake_t *self=(elite_snake_t*)params;
    elog("INFO : [elite_snake_on_user_update] deallocating self(elite_snake)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    if (self!=NULL) {
        if (self->playfield) free(self->playfield);
        free(self);
        elog("INFO : [elite_snake_on_user_update] successfully deallocated self(elite_snake)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else {
        elog("ERROR : [elite_snake_on_user_update] failed to deallocated self(elite_snake); returning false from elite_snake_on_user_destroy()\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        return false;
    };
    elog("INFO : [elite_snake_on_user_update] returning true from template_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  return true;

};



void elite_snake_start_task(){

    elog("INFO : [elite_snake_start_pixel_game_task] entered elite_snake_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elog("INFO : [elite_snake_start_pixel_game_task] creating &pixel_game_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elite_pixel_game_config_t pixel_game_config={
        .app_name="elite_snake",
        .screen_width=10,
        .screen_height=30,
        .on_user_construct=(void*)&elite_snake_construct,
        .on_user_update=&elite_snake_on_user_update,
        .on_user_destroy=&elite_snake_on_user_destroy
    };
    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&pixel_game_config, 5, NULL);
    elog("INFO : [elite_snake_start_pixelapp_task] leaving elite_snake_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

};
