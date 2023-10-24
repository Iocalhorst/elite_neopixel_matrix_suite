//header only elite_pixel_game_ente bare minimum template example application
#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"

#pragma once


#include "elite_spriteshow_assets.h"

//since on-demand loading of resources from flash or external locations is noticably time-consuming(seconds maybe even),
//instead of fetching resources "just-in-time" - at the end of the cyle (after fading out the last resources)
//we want to pre-fetch "early" - specifically during the hold-time of the current resource
//this avoids lags/delays when swapping the content and fading back in again when using alpha-blending for transitioning
//and works in favor of (possible) spatial transitioning effects
#define STATE_ERROR -1
#define STATE_INITIAL_HOLD 0 //we use this state to offset the individual container timings a little
#define STATE_FADE_IN 1
#define STATE_HOLD 2
#define STATE_FETCH_NEXT 3
#define STATE_FADE_OUT 4
#define STATE_SWAP_SPRITE 5

typedef struct{
  int container_state;
  int container_current_resource_index;
  float state_timer;
  float initial_hold_time;
  float hold_time,fade_in_time,fade_out_time;
}sprite_container_t;

typedef struct {
  char* app_name;
  uint16_t num_resources;
  char *resource_locations[NUM_URLS];
  uint8_t num_sprite_containers;
  sprite_container_t *p_sprite_containers;
  elite_sprite_t *test_sprite;



}spriteshow_t;


bool spriteshow_on_user_update_entered_log=false;
bool spriteshow_on_user_update_leaving_log=false;
bool spriteshow_on_user_update_pre_particle_shower_update_log=false;


bool sprite_container_update(sprite_container_t *self,float fElapsedTime){
  switch (self->container_state) {
    case : STATE_INITIAL_HOLD {
        self->state_timer-=fElapsedTime;
        if (self->state_timer<=0.0f) {
          self->state_timer=self->fade_in_time;
          self->container_state=STATE_FADE_IN;
        };
        break;
      };
    case : STATE_FADE_IN {
        self->state_timer-=fElapsedTime;
        if (self->state_timer<=0.0f) {
          self->state_timer=self->fade_hold_time;
          self->container_state=STATE_HOLD;
        break;
      };
    case : STATE_HOLD {
        self->state_timer-=fElapsedTime;
        if (self->state_timer<=0.0f) {
          self->state_timer=self->fade_out_time;
          self->container_state=STATE_FADE_OUT;
          //
          //We do not (asynchronously) retrieve self->p_next_sprite in here!
          //
          //reason :      retrieval might cause blocking calls(sockets,flash memory reads etc) when resource aquisition is performed
          //              and cause pending/"fading" containers to hang/lag
          //workaround :  we do assing p_next_sprite synchronously in the on_user_update for all containers at once,
          //              ensuring that all containers are on hold at this point
          //note :        proper time span tolerances must be taken into account
          //              this workaround can be removed if non-blocking resources retrieval is implemented
          //todo :        implement non blocking resource aquisition
        };
        break;
      };
    case : STATE_FADE_OUT {
        self->state_timer-=fElapsedTime;
        if (self->state_timer<=0.0f) {
            self->state_timer=self->fade_in_time;
            self->container_state=STATE_FADE_IN;
            self->p_current_sprite=self->p_next_sprite;
        };
        break;
      };
    case : STATE_ERROR {
        break;
    };
    default : {
        self->container_state=STATE_ERROR;
        //unreachable;
        return false;
        break;
      };
  };

    if (self->state_timer<=0.0f) {self->container_state=STATE_FADE_IN;};
  }

  return true;
};

bool sprite_container_draw(sprite_container_t *self,elite_pixel_game_t *ente){
  return true;
};

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
  self->num_resources=NUM_URLS;
  for (size_t r=0;r<NUM_URLS;r++) self->resource_locations[r]=uris[r];

  self->num_sprite_containers=3;
  self->p_sprite_containers=(sprite_container_t*)malloc(sizeof(sprite_container_t)*self->num_sprite_containers);
  if (self->p_sprite_containers==NULL) {
      //todo error message+handling
      return NULL;
    };
  for (int i=0;i<self->num_sprite_containers;i++){
      self->p_sprite_containers[i].container_state=STATE_INITIAL_HOLD;
      self->p_sprite_containers[i].initial_hold_time=0.5f*i;
      self->p_sprite_containers[i].fade_in_time=1.0f;
      self->p_sprite_containers[i].hold_time=10.0f;
      self->p_sprite_containers[i].fade_out_time=1.0f;
    };


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

  for (int i=0;i<self->num_sprite_containers;i++) sprite_container_update(&self->p_sprite_containers[i],fElapsedTime);
  bool all_containers_on_hold=true;
  for (int i=0;i<self->num_sprite_containers;i++) {
    if (self->p_sprite_containers[i].container_state!=STATE_HOLD) {
      all_containers_on_hold=false;
      break;
    };
  };
  if (all_containers_on_hold) {
    //fetch_next()
    //advance()
      //for (int i=0;i<self->num_sprite_containers;i++) sprite_container_fetch_next(self->p_sprite_container[i],);
  };
  for (int i=0;i<self->num_sprite_containers;i++) sprite_container_draw(&self->p_sprite_containers[i],ente);

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
