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



typedef struct {
  uint8_t r,g,b;
}sRGB;


typedef struct {
  float fr,fg,fb;
}sfRGB;


typedef struct {
  float fr,fg,fb,fa;
}sfRGBA;


void elite_display_update(sRGB *p_frame_buf);

const sRGB pink={
  .r=68,
  .g=32,
  .b=28
};
const sRGB dark_green={
  .r=2,
  .g=8,
  .b=2
};
const sRGB dark_pink={
  .r=24,
  .g=10,
  .b=8
};
const sRGB white={
  .r=64,
  .g=60,
  .b=56
};
const sRGB black={
  .r=0,
  .g=0,
  .b=0
};
const sRGB yellow={
  .r=64,
  .g=60,
  .b=4
};
const sRGB dark_yellow={
  .r=24,
  .g=22,
  .b=2
};
const sRGB blue={
  .r=22,
  .g=18,
  .b=56
};
const sRGB dark_blue={
  .r=4,
  .g=6,
  .b=16
};


typedef struct {
  uint16_t width,height;
  bool load_immediatly;
  char url[ESP_VFS_PATH_MAX];
}elite_sprite_config_t;

typedef struct {
  uint16_t width,height;
  char url[ESP_VFS_PATH_MAX];
  bool load_immediatly;
  bool load_failed;
  sRGB *_p_bitmap;
}elite_sprite_t;


//todo : int elite_err_t elite_sprite_load_bla();
bool elite_sprite_load(elite_sprite_t *self){
  if (self==NULL) return false;
  if (self->load_failed) return false;
  char log_str[256]={0};
  char file_path[128]={0};
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



  vTaskDelay(log_delay/portTICK_PERIOD_MS);
  elog("DEBUG : [elite_sprite_load] leaving elite_sprite_load, true\n");

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
    sfRGB *pixels;
}layer_fRGB;


struct s_elite_pixel_game {
    bool init_ok;
    bool user_construct_done;
    bool user_construct_failed;
    elite_pixel_game_config_t config;
    uint16_t screen_width,screen_height;//actual "usable" witdh/height since referencing the config is verbooten
    sRGB *p_frame_buf;
    size_t p_frame_buf_size;
    layer_fRGB *p_layer[4];
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
    //default config is just to render layer 0 to the p_frame_buf without any fancy pants like alpha blending and stuff.
    //p_frame_buf is where your sRGB (uint8_t,uint8_t,uint8_t) rgb data bytes will sit at
    //i think id like to have a union for that. whatever;
    bool (*render)(elite_pixel_game_t*);

};


//forward function declarations
elite_pixel_game_t* elite_pixel_game_construct(elite_pixel_game_config_t config);
bool elite_pixel_game_set_target_layer(elite_pixel_game_t *self,int tl);
bool elite_pixel_game_render_to_framebuf(elite_pixel_game_t *self);
bool elite_pixel_game_fill_layer(elite_pixel_game_t *self,sRGB fill_col);
bool elite_pixel_game_fill_flayer(elite_pixel_game_t *self,sfRGB fill_fcol);
bool elite_pixel_game_putpixel(elite_pixel_game_t* self,int16_t x,int16_t y,sRGB col);
bool elite_pixel_game_fputpixel(elite_pixel_game_t* self,int16_t x,int16_t y,sfRGB col);
bool elite_pixel_game_fputpixelRGBA(elite_pixel_game_t* self,int16_t x,int16_t y,sfRGBA col);
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

    self->p_frame_buf_size=0;//uninitialized variables suck.
    self->p_frame_buf=(sRGB*)malloc(self->config.screen_width*self->config.screen_height*sizeof(sRGB));

  if (self->p_frame_buf!=NULL) {
    elog("INFO : [elite_pixel_game_construct] p_frame_buf allocated\n");
    self->p_frame_buf_size=self->config.screen_width*self->config.screen_height*sizeof(sRGB);

    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else{
    elog("ERROR : [elite_pixel_game_construct] failed to allocate p_frame_buf\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }

  elog("INFO : [elite_pixel_game_construct] allocating p_layer[0]\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  self->p_layer[0]=(layer_fRGB*)malloc(sizeof(layer_fRGB));
  if (self->p_layer[0]!=NULL) {
    elog("INFO : [elite_pixel_game_construct] p_layer[0] allocated\n");
    self->target_layer=0;
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else{
    elog("ERROR : [elite_pixel_game_construct] failed to allocate p_layer[0]\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
};

    self->p_layer[0]->pixels=(sfRGB*)malloc(self->config.screen_width*self->config.screen_height*sizeof(sfRGB));
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

  elog("INFO : [elite_pixel_game_destruct] deallocating self->p_frame_buf\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  if (self->p_frame_buf==NULL) {
  elog("ERROR : [elite_pixel_game_destruct] self->p_frame_buf==NULL, returning false;\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  return false;
  }else{free(self->p_frame_buf);
  elog("INFO : [elite_pixel_game_destruct] successfully deallocated self->p_frame_buf\n");
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

  //send sigkill to game_tasks
/*    elog("INFO : deallocating self->p_layer[1,2,3]\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  if (self->p_layer[1]->pixels!=nullptr) {
    free(self->p_layer[1]->pixels);
    elog("INFO : successfully deallocated self->p_layer[1]->pixels\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else {
    elog("INFO : self->p_layer[1]->pixels not allocated. skipping\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };
  if (self->p_layer[2]->pixels!=nullptr) {
    free(self->p_layer[2]->pixels);
    elog("INFO : successfully deallocated self->p_layer[2]->pixels\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else {
    elog("INFO : self->p_layer[2]->pixels not allocated. skipping\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };
  if (self->p_layer[3]->pixels!=nullptr) {
    free(self->p_layer[3]->pixels);
    elog("INFO : successfully deallocated self->p_layer[3]->pixels\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else {
    elog("INFO : self->p_layer[3]->pixels not allocated. skipping\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }
*/
//    elog("INFO : setting on_user_update=NULL\n");
//    vTaskDelay(log_delay / portTICK_PERIOD_MS);
//    self->on_user_update=NULL;
//    elog("INFO : setting on_user_construct=NULL");
//    vTaskDelay(log_delay / portTICK_PERIOD_MS);
//    self->on_user_construct=NULL;
    elog("INFO : [elite_pixel_game_destruct] calling on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    if (self->on_user_destroy(self->user)==false){
      elog("ERROR : [elite_pixel_game_destruct] user_destroy() returned false\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);

    }else {
      elog("INFO : [elite_pixel_game_destruct] user_destroy() returned true\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);

    }
//    elog("INFO : setting on_user_destroy=NULL");
//    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    //self->on_user_destroy=NULL;
//    free(self);
    //self=NULL;
    return true;
};

bool elite_pixel_game_set_target_layer(elite_pixel_game_t *self,int tl){
  if (tl<0) {return false;};
  if (tl>self->num_layers) {return false;};
  self->target_layer=tl;
  return true;
}


bool elite_pixel_game_fputpixel_entered_log=false;
bool elite_pixel_game_fputpixel_leaving_log=false;
bool elite_pixel_game_fputpixel_dont_hurt_the__canvas_log=false;

static uint32_t canvas_hurt_count=0;


bool elite_pixel_game_fputpixel(elite_pixel_game_t* self,int16_t x,int16_t y,sfRGB col){
//tracing pre
    if (elite_pixel_game_fputpixel_entered_log==false) {
        elite_pixel_game_fputpixel_entered_log=true;
        elog("INFO : [elite_pixel_game_fputpixel] entered elite_pixel_game_fputpixel() - this notification will only occur once\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
      };
//body
    if ((x>=0)&&(x<self->config.screen_width)&&(y>=0)&&(y<self->config.screen_height)) {
        self->p_layer[self->target_layer]->pixels[y*self->config.screen_width+x]=col;
    }else{
        canvas_hurt_count+=1;
        if (elite_pixel_game_fputpixel_dont_hurt_the__canvas_log==false) {
            elite_pixel_game_fputpixel_dont_hurt_the__canvas_log=true;
            elog("ERROR : [elite_pixel_game_fputpixel] You've hurt the canvas. You're not supposed to hurt the canvas! - this notification will only occur once\n");
            char hurt_str2[256]={0};
            sprintf(hurt_str2,"details : \n config.screen_width=%i\n config.screen_height=%i\n x=%i y=%i\n",self->config.screen_width,self->config.screen_height,x,y);
            elog(hurt_str2);
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
        };

    };
//tracing post
    if (elite_pixel_game_fputpixel_leaving_log==false) {
      elite_pixel_game_fputpixel_leaving_log=true;
      elog("INFO : [elite_pixel_game_fputpixel] leaving elite_pixel_game_fputpixel() - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };
    return true;
};

bool elite_pixel_game_fputpixelRGBA_entered_log=false;
bool elite_pixel_game_fputpixelRGBA_leaving_log=false;
bool elite_pixel_game_fputpixelRGBA_dont_hurt_the__canvas_log=false;

bool elite_pixel_game_fputpixelRGBA(elite_pixel_game_t* self,int16_t x,int16_t y,sfRGBA src_col){
//tracing pre
    if (elite_pixel_game_fputpixelRGBA_entered_log==false) {
        elite_pixel_game_fputpixelRGBA_entered_log=true;
        elog("INFO : [elite_pixel_game_fputpixelRGBA] entered elite_pixel_game_fputpixelRGBA() - this notification will only occur once\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

//body
    if ((x>=0)&&(x<self->config.screen_width)&&(y>=0)&&(y<self->config.screen_height)) {
        sfRGB fmixed_color;
        //rubbish alpha blending implementation
        fmixed_color.fr=self->p_layer[self->target_layer]->pixels[y*self->config.screen_width+x].fr;
        fmixed_color.fg=self->p_layer[self->target_layer]->pixels[y*self->config.screen_width+x].fg;
        fmixed_color.fb=self->p_layer[self->target_layer]->pixels[y*self->config.screen_width+x].fb;
        fmixed_color.fr*=(1.0f-(src_col.fa/255.0f));
        fmixed_color.fg*=(1.0f-(src_col.fa/255.0f));
        fmixed_color.fb*=(1.0f-(src_col.fa/255.0f));
        fmixed_color.fr+=src_col.fr*(src_col.fa/255.0f);
        fmixed_color.fg+=src_col.fg*(src_col.fg/255.0f);
        fmixed_color.fb+=src_col.fb*(src_col.fb/255.0f);

        self->p_layer[self->target_layer]->pixels[y*self->config.screen_width+x]=fmixed_color;

    }else{
        canvas_hurt_count+=1;
        if (elite_pixel_game_fputpixelRGBA_dont_hurt_the__canvas_log==false) {
            elite_pixel_game_fputpixelRGBA_dont_hurt_the__canvas_log=true;
            elog("ERROR : [elite_pixel_game_fputpixelRGBA] You've hurt the canvas. You're not supposed to hurt the canvas! - this notification will only occur once\n");
            char hurt_str2[256]={0};
            sprintf(hurt_str2,"details : \n config.screen_width=%i\n config.screen_height=%i\n x=%i y=%i\n",self->config.screen_width,self->config.screen_height,x,y);
            elog(hurt_str2);
            vTaskDelay(log_delay / portTICK_PERIOD_MS);

        };
    return false;
    };

//tracing post
    if (elite_pixel_game_fputpixelRGBA_leaving_log==false) {
        elite_pixel_game_fputpixelRGBA_leaving_log=true;
        elog("INFO : [elite_pixel_game_fputpixelRGBA] leaving elite_pixel_game_fputpixel() - this notification will only occur once\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

    return true;
};

bool elite_pixel_game_putpixel(elite_pixel_game_t* self,int16_t x,int16_t y,sRGB col){
    sfRGB fcol;
    fcol.fr=(float)col.r;
    fcol.fg=(float)col.g;
    fcol.fb=(float)col.b;
    elite_pixel_game_fputpixel(self,x,y,fcol);
    return true;
};

bool elite_pixel_game_fill_layer_entered_log=false;
bool elite_pixel_game_fill_layer_leaving_log=false;

bool elite_pixel_game_fill_layer(elite_pixel_game_t *self,sRGB fill_col){

//tracing pre
if (elite_pixel_game_fill_layer_entered_log==false) {
      elite_pixel_game_fill_layer_entered_log=true;
      elog("INFO : [elite_pixel_game_fill_layer] entered elite_pixel_game_fill_layer - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

//body
  for (int y=0;y<self->config.screen_height;y++){
    for (int x=0;x<self->config.screen_width;x++){
          elite_pixel_game_putpixel(self,x,y,fill_col);
    };
  };

//tracing post
if (elite_pixel_game_fill_layer_leaving_log==false) {
      elite_pixel_game_fill_layer_leaving_log=true;
      elog("INFO : [elite_pixel_game_fill_layer] leaving elite_pixel_game_fill_flayer - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

  return true;
}


bool elite_pixel_game_fill_flayer_entered_log=false;
bool elite_pixel_game_fill_flayer_leaving_log=false;

bool elite_pixel_game_fill_flayer(elite_pixel_game_t *self,sfRGB fill_fcol){

//tracing pre
if (elite_pixel_game_fill_flayer_entered_log==false) {
    elite_pixel_game_fill_flayer_entered_log=true;
    elog("INFO : [elite_pixel_game_fill_flayer] entered elite_pixel_game_fill_flayer - this notification will only occur once\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };


//body
  for (int y=0;y<self->config.screen_height;y++){
    for (int x=0;x<self->config.screen_width;x++){
          elite_pixel_game_fputpixel(self,x,y,fill_fcol);
    };
  };
//tracing post
if (elite_pixel_game_fill_flayer_leaving_log==false) {
      elite_pixel_game_fill_flayer_leaving_log=true;
      elog("INFO : [elite_pixel_game_fill_flayer] leaving elite_pixel_game_fill_flayer - this notification will only occur once\n");
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
//  memset(self->p_frame_buf,0,self->p_frame_buf_size);

//test of direct access to the p_frame_buf - dont do it like that - use the api
//for (int i=0;i<300;i++) {sRGB c={32,32,32};self->p_frame_buf[i]=c;};


//test of directly accessing the layer[0]->pixels - dont do it like that - use the api
//just a little color wash
/*
for (int i=0;i<20;i++) {sfRGB c={255.0f,0.0f,0.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=20;i<40;i++) {sfRGB c={255.0f,96.0f,0.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=40;i<60;i++) {sfRGB c={255.0f,127.0f,0.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=60;i<80;i++) {sfRGB c={255.0f,172.0f,64.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=80;i<100;i++) {sfRGB c={255.0f,192.0f,96.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=100;i<120;i++) {sfRGB c={0.0f,255.0f,0.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=120;i<140;i++) {sfRGB c={96.0f,255.0f,0.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=140;i<160;i++) {sfRGB c={128.0f,255.0f,0.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=160;i<180;i++) {sfRGB c={172.0f,255.0f,0.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=180;i<200;i++) {sfRGB c={255.0f,192.0f,96.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=200;i<220;i++) {sfRGB c={0.0f,0.0f,255.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=220;i<240;i++) {sfRGB c={32.0f,96.0f,255.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=240;i<260;i++) {sfRGB c={0.0f,128.0f,255.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=260;i<280;i++) {sfRGB c={0.0f,172.0f,255.0f};self->p_layer[0]->pixels[i]=c;};
for (int i=280;i<300;i++) {sfRGB c={32.0f,212.0f,255.0f};self->p_layer[0]->pixels[i]=c;};
*/


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

return result;
}

bool elite_pixel_game_render_to_framebuf(elite_pixel_game_t *self){
  if (self->init_ok!=true) return false;
  for (size_t y=0;y<self->config.screen_height;y++){
      for (size_t x=0;x<self->config.screen_width;x++){
          size_t pixel_index=0;
          pixel_index=self->config.screen_width*y+x;
          sRGB pixel_color={0};
          sfRGB fpixel_color={0};
          fpixel_color=self->p_layer[0]->pixels[pixel_index];
          pixel_color.r=(uint8_t)fpixel_color.fr;
          pixel_color.g=(uint8_t)fpixel_color.fg;
          pixel_color.b=(uint8_t)fpixel_color.fb;
          self->p_frame_buf[pixel_index]=pixel_color;
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

  vTaskDelay(500 / portTICK_PERIOD_MS);
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
    elite_display_update(p_pixel_game->p_frame_buf);
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
