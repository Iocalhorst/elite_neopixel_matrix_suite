//elite pixel game ente header file
//todoooooo: decription
//todo : implement  setlayer() for additional "user" layer[1,2,3] and stuff
//todo : come up with a rendering scheme/funtion interface that serves the purpose. wat?
//todo : implement dynamic array. for sprites and stuff;
#pragma once
bool elite_theres_a_pixel_game_running=false;
bool elite_kill_pixel_game=false;
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "elite.h"
#include "elite_display.h"
#include "freertos/stream_buffer.h"
#include "elite_pixel_formats.h"

//void elite_display_update(FRAMEBUF_PIXFORMAT *p_framebuf);

const sRGB pink={
  .r=68*2,
  .g=32*2,
  .b=28*2
};
const sRGB dark_green={
  .r=2*2,
  .g=8*2,
  .b=2*2
};
const sRGB dark_pink={
  .r=24*2,
  .g=10*2,
  .b=8*2
};
const sRGB white={
  .r=64*2,
  .g=60*2,
  .b=56*2
};
const sRGB black={
  .r=0*2,
  .g=0*2,
  .b=0*2
};
const sRGB yellow={
  .r=64*2,
  .g=60*2,
  .b=4*2
};
const sRGB dark_yellow={
  .r=24*2,
  .g=22*2,
  .b=2*2
};
const sRGB blue={
  .r=22*2,
  .g=18*2,
  .b=56*2
};
const sRGB dark_blue={
  .r=4*2,
  .g=6*2,
  .b=16*2
};

const sfRGBA red1={
  .fr=255.0f,
  .fg=0.0f,
  .fb=0.0f,
  .fa=255
};

const sfRGBA green1={
  .fr=0.0f,
  .fg=255.0f,
  .fb=0.0f,
  .fa=255.0f
};

const sfRGBA blue1={
  .fr=0.0f,
  .fg=0.0f,
  .fb=255.0f,
  .fa=255
};

const sfRGBA cyan1={
  .fr=0.0f,
  .fg=255.0f,
  .fb=255.0f,
  .fa=255.0f};
const sfRGBA magenta1={255.0f,0.0f,255.0f,255.0f};
const sfRGBA yellow1={255.0f,204.0f,0.0f,255.0f};
const sfRGBA white1={255.0f,255.0f,255.0f,255.0f};
const sfRGBA orange1={255.0f,102.0f,0.0f,255.0f};
const sfRGBA purple1={96.0f,0.0f,192.0f,255.0f};
const sfRGBA black1={0.0f,0.0f,0.0f,255.0f};

const sfRGBA red2={192.0f,16.0f,16.0f,255.0f};
const sfRGBA green2={16.0f,192.0f,12.0f,255.0f};
const sfRGBA blue2={38.0f,28.0f,212.0f,255.0f};//pretty blue
const sfRGBA cyan2={8.0f,162.0f,172.0f,255.0f};
const sfRGBA magenta2={172.0f,18.0f,152.0f,255.0f};
const sfRGBA yellow2={172.0f,152.0f,8.0f,255.0f};
const sfRGBA white2={172.0f,164.0f,158.0f,255.0f};
const sfRGBA orange2={212.0f,96.0f,8.0f,255.0f};
const sfRGBA purple2={116.0f,8.0f,212.0f,255.0f};
const sfRGBA black2={0.0f,0.0f,0.0f,255.0f};



typedef struct {
  uint16_t width,height;
  bool load_immediatly;
  char url[128];
}elite_sprite_config_t;

typedef struct {
  uint16_t width,height;
  char url[128];
  bool load_immediatly;
  bool load_failed;
  sRGB *_p_bitmap;
}elite_sprite_t;



typedef struct s_elite_pixel_game elite_pixel_game_t;

typedef struct{
  char* app_name;
  uint16_t screen_width;
  uint16_t screen_height;
  uint16_t num_layers;
  bool (*on_user_update)(void*,elite_pixel_game_t*,float);
  void* (*on_user_construct)(elite_pixel_game_t*);
  bool (*on_user_destroy)(void*);
}elite_pixel_game_config_t;


typedef struct{
    LAYER_PIXFORMAT *pixels;
}layer_fRGBA;

struct s_elite_pixel_game {
    bool init_ok;
    bool user_construct_done;
    bool user_construct_failed;
    elite_pixel_game_config_t config;
    uint16_t screen_width,screen_height;//actual "usable" witdh/height since referencing the config is verbooten
    FRAMEBUF_PIXFORMAT *p_framebuf;
    size_t p_framebuf_size;
    layer_fRGBA *p_layer[4];
    int target_layer;
    int num_layers;
    void *user;
    //the updateFunction should take care of at least populating p_layer[0] with sfRGB data
    bool (*on_user_update)(void*,elite_pixel_game_t*,float);
    void* (*on_user_construct)(elite_pixel_game_t*);
    bool (*on_user_destroy)(void*);
    uint64_t lastTimeStampInMicroSeconds;
    float fElapsedTime;
    float accumulated_fElapsedTime;
    //in case you wanted to customize the renderer i.e if user wants to render multiple layers and/or arbitrarly ordered overwrite this function pointer
    //default config is just to render layer 0 to the p_framebuf without any fancy pants like alpha blending and stuff.
    //p_framebuf is where your sRGB (uint8_t,uint8_t,uint8_t) rgb data bytes will sit at
    //i think id like to have a union for that. whatever;
    bool (*render)(elite_pixel_game_t*);

};


//todo : int elite_err_t elite_sprite_load_bla();
bool elite_sprite_load(elite_sprite_t *self){
  if (self==NULL) return false;
  if (self->load_failed) return false;
  char log_str[256]={0};
  char file_path[138]={0};
  const char *base_path="/littlefs";
  sprintf(file_path,"%s/%s",base_path,self->url);
  FILE *f=fopen(file_path,"r");
  if (f==NULL) {
    //todo : verbose error message : invalid filename, file doesnt exist, readError
    sprintf(log_str,"ERROR : [elite_sprite_load] could not open <%s> for reading\n",file_path);
    elog(log_str);
    vTaskDelay(log_delay/portTICK_PERIOD_MS);
  return false;
}else {
  //todo : verbose error message : invalid filename, file doesnt exist, readError
  sprintf(log_str,"INFO : [elite_sprite_load] opened <%s> for reading\n",file_path);
  elog(log_str);
  vTaskDelay(log_delay/portTICK_PERIOD_MS);

};

  if (self->_p_bitmap!=NULL) {
    elog("INFO : [elite_sprite_load] overwriting data at self->p_bitmap\n");
    vTaskDelay(log_delay/portTICK_PERIOD_MS);
    }else {
      size_t _p_bitmap_size=sizeof(sRGB)*self->height*self->width;
      self->_p_bitmap=(sRGB*)malloc(_p_bitmap_size);
      if (self->_p_bitmap==NULL) {
        elog("ERROR : [elite_sprite_load] malloc fail\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        err_t fclose_ret=fclose(f);
        sprintf(log_str,"DEBUG : [elite_sprite_load] fclose() fclose_ret=%i\n",fclose_ret);
        elog(log_str);
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
      }else{
        sprintf(log_str,"DEBUG : [elite_sprite_load] malloc(%dd)==%p success\n",_p_bitmap_size,(void *)&self->_p_bitmap);
        elog(log_str);
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
      };
    };

  int num_bytes_read=fread(self->_p_bitmap,1,self->width*self->height*3,f);

  if (num_bytes_read!=self->width*self->height*3){
      free(self->_p_bitmap);
      elog("DEBUG : [elite_sprite_load] free(self->_p_bitmap)");
      fclose(f);
      elog("ERROR : [elite_sprite_load] fread() fail\n");
      vTaskDelay(log_delay/portTICK_PERIOD_MS);
      return false;
  }else {
      int fclose_ret=fclose(f);
      sprintf(log_str,"DEBUG : [elite_sprite_load] fclose() fclose_ret=%i\n",fclose_ret);
      elog(log_str);
      vTaskDelay(log_delay/portTICK_PERIOD_MS);
      sprintf(log_str,"DEBUG : [elite_sprite_load] fread()==%i bytes, which is a great success\n",num_bytes_read);
      elog(log_str);
      vTaskDelay(log_delay/portTICK_PERIOD_MS);
      uint32_t checksum=0l;
      for (int i=0;i<100;i++) {checksum+=self->_p_bitmap[i].r;checksum+=self->_p_bitmap[i].g;checksum+=self->_p_bitmap[i].b;};
      sprintf(log_str,"DEBUG : [elite_sprite_load] checksum=%lu\n",checksum);
      elog(log_str);
      vTaskDelay(log_delay/portTICK_PERIOD_MS);

  }




  elog("DEBUG : [elite_sprite_load] leaving elite_sprite_load, true\n");
  vTaskDelay(log_delay/portTICK_PERIOD_MS);
  return true;

};


elite_sprite_t *elite_sprite_construct(elite_sprite_config_t config){
    char log_str[256]={0};
    elog("INFO : [spriteshow_construct] entering elite_sprite_construct()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    elite_sprite_t *self=(elite_sprite_t*)malloc(sizeof(elite_sprite_t));
    memset(&self->url, 0, sizeof(self->url));
    strncpy(self->url,config.url,strlen(config.url));//lol

    self->height=config.height;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->height=%ul\n",self->height);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->width=config.width;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->width=%ul\n",self->width);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->load_immediatly=config.load_immediatly;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->load_immediatly=%b\n",self->load_immediatly);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->load_failed=false;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->load_failed=false\n");
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->_p_bitmap=NULL;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->_p_bitmap=nullptr\n");
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    if (self->load_immediatly==true) {
        elog("DEBUG : [elit_sprite_construct] load_immediatly==true, calling elite_sprite_load(self)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        if (elite_sprite_load(self)) {
            sprintf(log_str,"INFO : [elite_sprite_construct] loaded sprite from file <%s> loaded\n",self->url);
            elog(log_str);
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
        }else {
            sprintf(log_str,"ERROR : [elite_sprite_construct] elite_sprite_load(self) failed\n");
            elog(log_str);
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
        };
    }else {
        elog("DEBUG : [elit_sprite_construct] load_immediatly==false, skipping elite_sprite_load(self)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

    //if (self->load_immediatly) self->load_failed=elite_sprite_load(self);
    elog("DEBUG : [elite_sprite_construct] leaving elite_sprite_construct()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    return self;
};


//forward function declarations
elite_pixel_game_t* elite_pixel_game_construct(elite_pixel_game_config_t config);
bool elite_pixel_game_set_target_layer(elite_pixel_game_t *self,int tl);
bool elite_pixel_game_render_to_framebuf(elite_pixel_game_t *self);
//bool elite_pixel_game_fill_layer(elite_pixel_game_t *self,sRGB fill_col);
bool elite_pixel_game_fill_flayerRGBA(elite_pixel_game_t *self,sfRGBA fill_fcol);
//bool elite_pixel_game_putpixel(elite_pixel_game_t* self,int16_t x,int16_t y,sRGB col);
//bool elite_pixel_game_fputpixel(elite_pixel_game_t* self,int16_t x,int16_t y,sfRGB col);
bool elite_pixel_game_fputpixelRGBA(elite_pixel_game_t* self,int16_t x,int16_t y,sfRGBA col);
//bool elite_pixel_game_fputPixelRGBA_(elite_pixel_game_t* self,int16_t x,int16_t y,sfRGBA col);
bool elite_pixel_game_destruct(elite_pixel_game_t *self);

//function definitions

elite_pixel_game_t* elite_pixel_game_construct(elite_pixel_game_config_t config){
//we need to check for valid configuration
//we need to check for conflicts since elite_pixel_game_t should be kind of a Singleton ... i guess
  //  if (config==NULL) return NULL;
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elog("INFO : [elite_pixel_game_construct] entering elite_pixel_game_construct(&config)\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  char log_str[512]={0};
  sprintf(log_str,"INFO : [elite_pixel_game_construct] config={app_name=\"%s\", screen_width=%i, screen_height=%i }\n",config.app_name,config.screen_width,config.screen_height);
  elog(log_str);
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  elite_pixel_game_t* self=(elite_pixel_game_t*)malloc(sizeof(elite_pixel_game_t));
  if (self!=NULL) {
      elog("INFO : [elite_pixel_game_construct] successfully allocated pixelapp memory\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else{
      elog("ERROR : [elite_pixel_game_construct] failed to allocate pixelapp memory\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
      return NULL;
    };
  self->config=config;
  self->screen_height=0;//we dont know yet if the user config parameters will hold up
  self->screen_width=0;
  self->target_layer=-1;
  elog("INFO : [elite_pixel_game_construct] configuration set\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

    self->p_framebuf_size=0;//uninitialized variables suck.
    self->p_framebuf=(FRAMEBUF_PIXFORMAT*)malloc(self->config.screen_width*self->config.screen_height*sizeof(FRAMEBUF_PIXFORMAT));

  if (self->p_framebuf!=NULL) {
    elog("INFO : [elite_pixel_game_construct] p_framebuf allocated\n");
    self->p_framebuf_size=self->config.screen_width*self->config.screen_height*sizeof(FRAMEBUF_PIXFORMAT);

    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else{
    elog("ERROR : [elite_pixel_game_construct] failed to allocate p_framebuf\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }

  elog("INFO : [elite_pixel_game_construct] allocating p_layer[0]\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  self->p_layer[0]=(layer_fRGBA*)malloc(sizeof(layer_fRGBA));
  if (self->p_layer[0]!=NULL) {
    elog("INFO : [elite_pixel_game_construct] p_layer[0] allocated\n");
    self->target_layer=0;
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else{
    elog("ERROR : [elite_pixel_game_construct] failed to allocate p_layer[0]\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
};

    self->p_layer[0]->pixels=(LAYER_PIXFORMAT*)malloc(self->config.screen_width*self->config.screen_height*sizeof(LAYER_PIXFORMAT));
    if (self->p_layer[0]->pixels!=NULL) {
      elog("INFO : [elite_pixel_game_construct] p_layer[0].pixels allocated\n");
      self->num_layers=1;
      self->screen_height=self->config.screen_height;
      self->screen_width=self->config.screen_width;
      self->target_layer=0;
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else{
      elog("ERROR : [elite_pixel_game_construct] failed to allocate p_layer[0].pixels\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

    /*self->p_layer[1]->pixels=nullptr;
    self->p_layer[2]->pixels=nullptr;
    self->p_layer[3]->pixels=nullptr;*/
    elog("INFO : [elite_pixel_game_construct] registering user function pointers\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    self->on_user_construct=config.on_user_construct;
    if (self->on_user_construct!=NULL){
      elog("INFO : [elite_pixel_game_construct] &on_user_construct() registered successfully\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else{
      elog("ERROR : [elite_pixel_game_construct] failed to register &on_user_construct()\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };
    self->on_user_update=config.on_user_update;
    if (self->on_user_update!=NULL){
      elog("INFO : [elite_pixel_game_construct] &on_user_update() registered successfully\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else{
      elog("ERROR : [elite_pixel_game_construct] failed to register &on_user_update()\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };
    self->on_user_destroy=config.on_user_destroy;
    if (self->on_user_destroy!=NULL){
      elog("INFO : [elite_pixel_game_construct] &on_user_destroy() registered successfully\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else{
      elog("ERROR : [elite_pixel_game_construct] failed to register &on_user_destroy()\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };
    if (self->p_layer[0]!=NULL) {
      elog("INFO : [elite_pixel_game_construct] p_layer[0] allocated successfully\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else{
      elog("ERROR : [elite_pixel_game_construct] p_layer[0] allocation failed\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }
    elog("INFO : [elite_pixel_game_construct] calling self->on_user_construct()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->user=(void*)self->on_user_construct(self);
    if (self->user!=NULL) {
      elog("INFO : [elite_pixel_game_construct] on_user_construct returned instance. registered\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else{
      elog("ERROR : [elite_pixel_game_construct] on_user_construct returned NULL. casted to (void*) and registered anyway... see?!\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };
    elog("INFO : [elite_pixel_game_construct] registering generic renderer\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->render=&elite_pixel_game_render_to_framebuf;
    if (self->render!=NULL){
       elog("INFO : [elite_pixel_game_construct] generic renderer registered successfully\n");
       vTaskDelay(log_delay / portTICK_PERIOD_MS);
     }
    else {
      elog("ERROR : [elite_pixel_game_construct] failed to register generic renderer\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };
    if (self->user!=NULL) {
      self->init_ok=true;
    }else {
      self->init_ok=false;
      return NULL;
    };

    self->lastTimeStampInMicroSeconds=esp_timer_get_time();
    self->accumulated_fElapsedTime=0.0f;
    self->fElapsedTime=0.0f;

    return self;
};


bool elite_pixel_game_destruct(elite_pixel_game_t *self){
  elog("INFO : [elite_pixel_game_destruct] entering elite_pixel_game_destruct()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  if (self==NULL) {
    elog("ERROR : [elite_pixel_game_destruct] self=NULL, returning false;\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    return false;
  }

  elog("INFO : [elite_pixel_game_destruct] deallocating self->p_framebuf\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  if (self->p_framebuf==NULL) {
  elog("ERROR : [elite_pixel_game_destruct] self->p_framebuf==NULL, returning false;\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  return false;
  }else{free(self->p_framebuf);
  elog("INFO : [elite_pixel_game_destruct] successfully deallocated self->p_framebuf\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };

  elog("INFO : [elite_pixel_game_destruct] deallocating self->p_layer[0].pixels\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  if (self->p_layer[0]->pixels==NULL) {
    elog("ERROR : [elite_pixel_game_destruct] self->layer[0]->pixels==NULL, \n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  //  return false;
  }else {
    free(self->p_layer[0]->pixels);
    elog("INFO : [elite_pixel_game_destruct] successfully deallocated self->layer[0]->pixels\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };
  elog("INFO : [elite_pixel_game_destruct] deallocating self->p_layer[0]\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  if (self->p_layer[0]==NULL) {
    elog("ERROR : [elite_pixel_game_destruct] self->p_layer[0]==NULL \n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  //  return false;
  }else {
    free(self->p_layer[0]);
    elog("INFO : [elite_pixel_game_destruct] successfully deallocated self->p_layer[0]\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };
    elog("INFO : [elite_pixel_game_destruct] calling on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    if (self->on_user_destroy(self->user)==false){
      elog("ERROR : [elite_pixel_game_destruct] user_destroy() returned false\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);

    }else {
      elog("INFO : [elite_pixel_game_destruct] user_destroy() returned true\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);

    }
    return true;
};

bool elite_pixel_game_set_target_layer(elite_pixel_game_t *self,int tl){
  if (tl<0) {return false;};
  if (tl>self->num_layers) {return false;};
  self->target_layer=tl;
  return true;
}


bool elite_pixel_game_fputpixelRGBA_entered_log=false;
bool elite_pixel_game_fputpixelRGBA_leaving_log=false;
bool elite_pixel_game_fputpixelRGBA_dont_hurt_the_canvas_log=false;

static uint32_t canvas_hurt_count=0;


bool elite_pixel_game_fputpixelRGBA(elite_pixel_game_t *self,int16_t x,int16_t y,sfRGBA fSrcColor) {
  //tracing pre
      if (elite_pixel_game_fputpixelRGBA_entered_log==false) {
          elite_pixel_game_fputpixelRGBA_entered_log=true;
          elog("INFO : [elite_pixel_game_fputpixel] entered elite_pixel_game_fputpixel() - this notification will only occur once\n");
          vTaskDelay(log_delay / portTICK_PERIOD_MS);
        };

    if ((x<0)||(y<0)||(x>=self->screen_width)||(y>=self->screen_height)) {  //dont hurt the canvas
          canvas_hurt_count+=1;
          if (elite_pixel_game_fputpixelRGBA_dont_hurt_the_canvas_log==false) {
              elite_pixel_game_fputpixelRGBA_dont_hurt_the_canvas_log=true;
              elog("ERROR : [elite_pixel_game_fputpixelRGBA] You've hurt the canvas. You're not supposed to hurt the canvas! - this notification will only occur once\n");
              char hurt_str2[256]={0};
              sprintf(hurt_str2,"details : \n config.screen_width=%i\n config.screen_height=%i\n x=%i y=%i\n",self->config.screen_width,self->config.screen_height,x,y);
              elog(hurt_str2);
              vTaskDelay(log_delay / portTICK_PERIOD_MS);
          };
      return false;
    }; //cause you're not supposed to hurt the canvas

    uint16_t pixelIndex=x+(y*(self->screen_width));
  //apply alpha blending
    sfRGBA  cB=self->p_layer[self->target_layer]->pixels[pixelIndex];
  //cB.clamp(0.0f,255.0f);
    cB.fa/=255.0f;
    cB.fr/=255.0f;
    cB.fg/=255.0f;
    cB.fb/=255.0f;

    sfRGBA cA=fSrcColor;
  //cA.clamp(0.0f,255.0f);
    cA.fa/=255.0f;
    cA.fr/=255.0f;
    cA.fg/=255.0f;
    cA.fb/=255.0f;
    sfRGBA cC;
  //if ((cA.fa!=0.0f)&&(cB.fa!=0.0f)) {
    cC.fa=cA.fa+((1.0f-cA.fa)*cB.fa);
  //catch divByZero cases
    if (cC.fa!=0.0f) {
        cC.fr=((1.0f/cC.fa)*(cA.fa*cA.fr+((1-cA.fa)*(cB.fa*cB.fr))));
        cC.fg=((1.0f/cC.fa)*(cA.fa*cA.fg+((1-cA.fa)*(cB.fa*cB.fg))));
        cC.fb=((1.0f/cC.fa)*(cA.fa*cA.fb+((1-cA.fa)*(cB.fa*cB.fb))));
    }else {
        if (cA.fa==0.0f) {
            cC=cB;
        }else {
            if (cB.fa==0.0f) {
                cC=cA;
            };
        };
    };
    cC.fa*=255.0f;
    cC.fr*=255.0f;
    cC.fg*=255.0f;
    cC.fb*=255.0f;
    //cC.clamp(0.0f,255.0f);
    //if (self->target_layer==FX_LAYER) cC.fa*=0.175f;
    self->p_layer[self->target_layer]->pixels[pixelIndex]=cC;


    //tracing post
    if (elite_pixel_game_fputpixelRGBA_leaving_log==false) {
        elite_pixel_game_fputpixelRGBA_leaving_log=true;
        elog("INFO : [elite_pixel_game_fputpixelRGBA] leaving elite_pixel_game_fputpixelRGBA() - this notification will only occur once\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

    return true;
};

/*bool elite_pixel_game_putpixel(elite_pixel_game_t* self,int16_t x,int16_t y,sRGB col){
    sfRGB fcol;
    fcol.fr=(float)col.r;
    fcol.fg=(float)col.g;
    fcol.fb=(float)col.b;
    elite_pixel_game_fputpixel(self,x,y,fcol);
    return true;
};
*/
//bool elite_pixel_game_fill_layer_entered_log=false;
//bool elite_pixel_game_fill_layer_leaving_log=false;

/*bool elite_pixel_game_fill_layer(elite_pixel_game_t *self,sRGBA fill_col){

//tracing pre
if (elite_pixel_game_fill_layer_entered_log==false) {
      elite_pixel_game_fill_layer_entered_log=true;
      elog("INFO : [elite_pixel_game_fill_layer] entered elite_pixel_game_fill_layer - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

//body
  for (int y=0;y<self->config.screen_height;y++){
    for (int x=0;x<self->config.screen_width;x++){
          elite_pixel_game_putpixelRGBA(self,x,y,fill_col);
    };
  };

//tracing post
if (elite_pixel_game_fill_flayerRGBA_leaving_log==false) {
      elite_pixel_game_fill_flayerRGBA_leaving_log=true;
      elog("INFO : [elite_pixel_game_fill_layerRGBA] leaving elite_pixel_game_fill_flayerRGBA - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

  return true;
}
*/

bool elite_pixel_game_fill_flayerRGBA_entered_log=false;
bool elite_pixel_game_fill_flayerRGBA_leaving_log=false;

bool elite_pixel_game_fill_flayerRGBA(elite_pixel_game_t *self,sfRGBA fill_fcol){

//tracing pre
if (elite_pixel_game_fill_flayerRGBA_entered_log==false) {
    elite_pixel_game_fill_flayerRGBA_entered_log=true;
    elog("INFO : [elite_pixel_game_fill_flayer] entered elite_pixel_game_fill_flayer - this notification will only occur once\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };


//body
  for (int y=0;y<self->config.screen_height;y++){
    for (int x=0;x<self->config.screen_width;x++){
          elite_pixel_game_fputpixelRGBA(self,x,y,fill_fcol);
    };
  };
//tracing post
if (elite_pixel_game_fill_flayerRGBA_leaving_log==false) {
      elite_pixel_game_fill_flayerRGBA_leaving_log=true;
      elog("INFO : [elite_pixel_game_fill_flayerRGBA] leaving elite_pixel_game_fill_flayerRGBA - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

  return true;
}



elite_pixel_game_config_t elite_pixel_game_default_config={
  .app_name="default_name",
  .screen_width=10,
  .screen_height=30,
  .num_layers=1
  //.updatefunctionPtr=&elite_pixel_game_update

};



bool elite_pixel_game_coreupdate(elite_pixel_game_t *self,float fElapsedTime){
  (void)fElapsedTime;
  return false;
}

bool on_user_update_pre_log=false;
bool on_user_update_post_log=false;

//no unfiltered logging via network here cause it would spam the socket
bool elite_pixel_game_update(elite_pixel_game_t *self,float fElapsedTime){
  if (self->init_ok!=true) return false;
  bool on_user_update_result=false;

for (int i=0;i<300;i++) {FRAMEBUF_PIXFORMAT c={0.0f,0.0f,0.0f};self->p_framebuf[i]=c;};

for (int i=0;i<300;i++) {LAYER_PIXFORMAT c={0.0f,0.0f,0.0f,255.0f};self->p_layer[0]->pixels[i]=c;};



if (on_user_update_pre_log==false) {
  on_user_update_pre_log=true;
  elog("INFO : [elite_pixel_game_update] calling self->on_user_update() - this notification will only occur once\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
};


on_user_update_result=self->on_user_update(self->user,self,fElapsedTime);
if (on_user_update_result==true){
    if (on_user_update_post_log==false){
        elog("INFO : [elite_pixel_game_update] self->on_user_update() returned true - this notification will only occur once\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        on_user_update_post_log=true;
    };
  }else{
    if (on_user_update_post_log==false){
    elog("ERROR : [elite_pixel_game_update] self->on_user_update() returned false - this notification will only occur once\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    on_user_update_post_log=true;
    };
  };

//to short circuit or not to short circuit - thats a question
bool result=(on_user_update_result&elite_pixel_game_render_to_framebuf(self));
if (result&&mr_displays_global_inputstream_handle!=NULL)
  xStreamBufferSend(mr_displays_global_inputstream_handle,self->p_framebuf,self->p_framebuf_size,portMAX_DELAY);

return result;
}

bool elite_pixel_game_render_to_framebuf(elite_pixel_game_t *self){
  if (self->init_ok!=true) return false;
  size_t pixel_index=0;
  for (size_t y=0;y<self->config.screen_height;y++){
      for (size_t x=0;x<self->config.screen_width;x++){
          FRAMEBUF_PIXFORMAT pixel_color={0};
          LAYER_PIXFORMAT fpixel_color={0};
          fpixel_color=self->p_layer[0]->pixels[pixel_index];
          pixel_color.fr=fpixel_color.fr;
          pixel_color.fg=fpixel_color.fg;
          pixel_color.fb=fpixel_color.fb;
          self->p_framebuf[pixel_index]=pixel_color;
          pixel_index+=1;
      };
    };
    return true;
}



void elite_pixel_game_update_fElapsedTime(elite_pixel_game_t *self){
  int64_t now=esp_timer_get_time();
  self->fElapsedTime=(float)(now-self->lastTimeStampInMicroSeconds);
  self->fElapsedTime*=0.000001f;
  self->accumulated_fElapsedTime+=self->fElapsedTime;
  self->lastTimeStampInMicroSeconds=now;
};


void elite_pixel_game_task(void* params){
  elite_theres_a_pixel_game_running=true;
  elite_kill_pixel_game=false;
  elite_pixel_game_config_t *p_pixel_game_config=(elite_pixel_game_config_t*)params;

  elite_pixel_game_config_t pixel_game_config={
    .app_name=p_pixel_game_config->app_name,
    .screen_width=p_pixel_game_config->screen_width,
    .screen_height=p_pixel_game_config->screen_height,
    .on_user_construct=p_pixel_game_config->on_user_construct,
    .on_user_update=p_pixel_game_config->on_user_update,
    .on_user_destroy=p_pixel_game_config->on_user_destroy
  };

  //p_pixel_game_config_t pixel_game_config=(pixel_game_config_t)params;
  //to not fuck up the update window at startup
  //while(ota_has_stopped==false){
//    vTaskDelay(500 / portTICK_PERIOD_MS);
//  };

  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elog("INFO : [elite_pixel_game_task] started\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  //int32_t pixelapp_runtime_limit_ms=60000;
  int32_t pixelapp_runtime_ms=0;


  elog("INFO : [elite_pixel_game_task] calling elite_pixel_game_construct(pixel_app_config\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  elite_pixel_game_t *p_pixel_game=elite_pixel_game_construct(pixel_game_config);

  elog("INFO : [elite_pixel_game_task] finished constructing pixelapp\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);




  while (true){

    elite_pixel_game_update_fElapsedTime(p_pixel_game);

    elite_pixel_game_update(p_pixel_game,p_pixel_game->fElapsedTime);
    //TODO factor out display output function calls to some more appropriate place ...

    vTaskDelay(20/ portTICK_PERIOD_MS);
    pixelapp_runtime_ms+=20;
    if (elite_kill_pixel_game==true) break;
  };

  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elog("INFO : [elite_pixel_game_task] calling elite_pixel_game_destruct()\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  if (elite_pixel_game_destruct(p_pixel_game)==false) {
      elog("ERROR : [elite_pixel_game_task] elite_pixel_game_destruct(p_pixel_app) returned false\n");
    }else{
      elog("INFO : [elite_pixel_game_task] elite_pixel_game_destruct(p_pixel_app) returned true\n");
  };
  vTaskDelay( log_delay/ portTICK_PERIOD_MS);
  elog("INFO : [elite_pixel_game_task] killing elite_pixel_game_task\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elite_theres_a_pixel_game_running=false;
  elite_kill_pixel_game=false;
  //exit_condition=true;
  vTaskDelete(NULL);

};


//bool elite_pixel_game_render_to_framebuf();