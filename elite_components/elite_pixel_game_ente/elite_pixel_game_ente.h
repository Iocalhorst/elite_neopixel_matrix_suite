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
static float ente_fps=42.0f;
const float deg2rad=0.0174533f;
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
    float fps_update_timer;
    float fps_update_intervall;
    float log_internal_update_intervall;
    float log_internal_update_timer;
    int log_internal_update_count;
    //in case you wanted to customize the renderer i.e if user wants to render multiple layers and/or arbitrarly ordered overwrite this function pointer
    //default config is just to render layer 0 to the p_framebuf without any fancy pants like alpha blending and stuff.
    //p_framebuf is where your sRGB (uint8_t,uint8_t,uint8_t) rgb data bytes will sit at
    //i think id like to have a union for that. whatever;
    bool (*render)(elite_pixel_game_t*);

};

//forward function declarations
elite_pixel_game_t* elite_pixel_game_construct(elite_pixel_game_config_t config);
bool elite_pixel_game_set_target_layer(elite_pixel_game_t *self,int tl);
bool elite_pixel_game_render_to_framebuf(elite_pixel_game_t *self);
//bool elite_pixel_game_fill_layer(elite_pixel_game_t *self,sRGB fill_col);
bool elite_pixel_game_fill_flayerRGBA(elite_pixel_game_t *self,sfRGBA fill_fcol);

void elite_pixel_game_draw_filled_circle(elite_pixel_game_t *self, float fx,float fy,float fr,sfRGBA col);
void elite_pixel_game_draw_shaded_circle(elite_pixel_game_t *self, float fx,float fy,float fr,sfRGBA col);
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

  ELOG("INFO : [elite_pixel_game_construct] entering elite_pixel_game_construct(&config)\n");


  ELOG("INFO : [elite_pixel_game_construct] config={app_name=\"%s\", screen_width=%i, screen_height=%i num_layers=%i}\n",config.app_name,config.screen_width,config.screen_height,config.num_layers);



  elite_pixel_game_t* self=(elite_pixel_game_t*)e_mall0c(__FUNCTION__,sizeof(elite_pixel_game_t));
  if (self!=NULL) {
      ELOG("INFO : [elite_pixel_game_construct] successfully allocated pixelapp memory\n");

    }else{
      ELOG("ERROR : [elite_pixel_game_construct] failed to allocate pixelapp memory\n");

      return NULL;
    };
  self->config=config;
  self->screen_height=0;//we dont know yet if the user config parameters will hold up
  self->screen_width=0;
  self->target_layer=-1;
  ELOG("INFO : [elite_pixel_game_construct] configuration set\n");


  self->p_framebuf_size=0;//uninitialized variables suck.
  self->p_framebuf=(FRAMEBUF_PIXFORMAT*)e_mall0c(__FUNCTION__,self->config.screen_width*self->config.screen_height*sizeof(FRAMEBUF_PIXFORMAT));

  if (self->p_framebuf!=NULL) {
    ELOG("INFO : [elite_pixel_game_construct] p_framebuf allocated\n");
    self->p_framebuf_size=self->config.screen_width*self->config.screen_height*sizeof(FRAMEBUF_PIXFORMAT);


  }else{
    ELOG("ERROR : [elite_pixel_game_construct] failed to allocate p_framebuf\n");

  }

  for (int i=0;i<self->config.screen_width*self->config.screen_height;i++) {
    FRAMEBUF_PIXFORMAT c={0.0f,0.0f,0.0f};self->p_framebuf[i]=c;
  };
  ELOG("INFO : [elite_pixel_game_construct] allocating p_layer[0]\n");

  if (self->config.num_layers<=0) {self->config.num_layers=1;ELOG("ERROR : [%s] config.num_layers<=0 - default to 1\n",__FUNCTION__);};
  if (self->config.num_layers>4) {self->config.num_layers=4;ELOG("ERROR : [%s] config.num_layers>4 - limiting to 4\n",__FUNCTION__);};


  self->num_layers=0;
  ELOG("DEBUG : [%s] self->num_layers==%i\n",__FUNCTION__,self->num_layers);

  //allocate layer ptr_array
//  self->p_layer=NULL;
  //self->p_layer=(layer_fRGBA*)e_mall0c(__FUNCTION__,sizeof(layer_fRGBA)*self->config.num_layers);
  //if (self->p_layer==NULL) {ELOG("ERROR : [%s] malloc fail\n",__FUNCTION__);return NULL;};
  for (int l=0;l<self->config.num_layers;l++){
      self->p_layer[l]=(layer_fRGBA*)e_mall0c(__FUNCTION__,sizeof(layer_fRGBA));
      if (self->p_layer[l]!=NULL) {
          ELOG("INFO : [elite_pixel_game_construct] p_layer[%i] allocated\n",l);
      }else{
          ELOG("ERROR : [elite_pixel_game_construct] failed to allocate p_layer[%i]\n",l);
          return NULL;
      };

      self->p_layer[l]->pixels=(LAYER_PIXFORMAT*)e_mall0c(__FUNCTION__,self->config.screen_width*self->config.screen_height*sizeof(LAYER_PIXFORMAT));
      if (self->p_layer[l]->pixels!=NULL) {
          ELOG("INFO : [%s] p_layer[%i].pixels allocated\n",__FUNCTION__,l);
          self->num_layers+=1;
          ELOG("DEBUG : [%s] self->num_layers==%i\n",__FUNCTION__,self->num_layers);
      }else{
          ELOG("ERROR : [%s] failed to allocate p_layer[%i].pixels\n",__FUNCTION__,l);
          return NULL;
      };
      for (int i=0;i<self->config.screen_width*self->config.screen_height;i++) {
          LAYER_PIXFORMAT c={0.0f,0.0f,0.0f,0.0f};self->p_layer[l]->pixels[i]=c;
      };
  };
  if (self->num_layers!=self->config.num_layers) {
      ELOG("ERROR : [%s] self->num_layers!=self->config.num_layers %i!=%i\n",__FUNCTION__,self->num_layers,self->config.num_layers);
      //TODO : unwind partial allocations
      return NULL;
  };

  self->screen_height=self->config.screen_height;
  self->screen_width=self->config.screen_width;
  self->target_layer=0;

  ELOG("INFO : [elite_pixel_game_construct] registering user function pointers\n");

  self->on_user_construct=config.on_user_construct;
  if (self->on_user_construct!=NULL){
      ELOG("INFO : [elite_pixel_game_construct] &on_user_construct() registered successfully\n");
  }else{
      ELOG("ERROR : [elite_pixel_game_construct] failed to register &on_user_construct()\n");
      return NULL;
      //TODO : unwind partial allocations
  };
  self->on_user_update=config.on_user_update;
  if (self->on_user_update!=NULL){
      ELOG("INFO : [elite_pixel_game_construct] &on_user_update() registered successfully\n");
  }else{
      ELOG("ERROR : [elite_pixel_game_construct] failed to register &on_user_update()\n");
      //TODO : unwind partial allocations
      return NULL;
  };
  self->on_user_destroy=config.on_user_destroy;
  if (self->on_user_destroy!=NULL){
      ELOG("INFO : [elite_pixel_game_construct] &on_user_destroy() registered successfully\n");
  }else{
      ELOG("ERROR : [elite_pixel_game_construct] failed to register &on_user_destroy()\n");
      return NULL;
  };

  //batch check and fail early+unwind,abort

  ELOG("INFO : [elite_pixel_game_construct] calling self->on_user_construct()\n");
  self->user=(void*)self->on_user_construct(self);
  if (self->user!=NULL) {
      ELOG("INFO : [elite_pixel_game_construct] on_user_construct returned instance. registered\n");
  }else{
      ELOG("ERROR : [elite_pixel_game_construct] on_user_construct returned NULL. casted to (void*) and registered anyway... see?!\n");
      //TODO : unwind,abort
      return NULL;
  };

  /*  ELOG("INFO : [elite_pixel_game_construct] registering generic renderer\n");

    self->render=&elite_pixel_game_render_to_framebuf;


    if (self->render!=NULL){
       ELOG("INFO : [elite_pixel_game_construct] generic renderer registered successfully\n");

     }
    else {
      ELOG("ERROR : [elite_pixel_game_construct] failed to register generic renderer\n");

    };
*/

    if (self->user!=NULL) {
      self->init_ok=true;
    }else {
      self->init_ok=false;
      return NULL;
    };

    self->lastTimeStampInMicroSeconds=esp_timer_get_time();
    self->accumulated_fElapsedTime=0.0f;
    self->fElapsedTime=0.0f;
    self->fps_update_intervall=1.0f/ente_fps;
    self->fps_update_timer=1.0f/ente_fps;
    self->log_internal_update_intervall=10.0f;
    self->log_internal_update_timer=10.0f;
    self->log_internal_update_count=0;
    return self;
};


bool elite_pixel_game_destruct(elite_pixel_game_t *self){
  ELOG("INFO : [elite_pixel_game_destruct] entering elite_pixel_game_destruct()\n");


  if (self==NULL) {
    ELOG("ERROR : [elite_pixel_game_destruct] self=NULL, returning false;\n");

    return false;
  }

  ELOG("INFO : [elite_pixel_game_destruct] deallocating self->p_framebuf\n");

  if (self->p_framebuf==NULL) {
  ELOG("ERROR : [elite_pixel_game_destruct] self->p_framebuf==NULL, returning false;\n");

  return false;
  }else{free(self->p_framebuf);
  ELOG("INFO : [elite_pixel_game_destruct] successfully deallocated self->p_framebuf\n");

  };

  ELOG("INFO : [elite_pixel_game_destruct] deallocating self->p_layer[0].pixels\n");

  if (self->p_layer[0]->pixels==NULL) {
    ELOG("ERROR : [elite_pixel_game_destruct] self->layer[0]->pixels==NULL, \n");

  //  return false;
  }else {
    free(self->p_layer[0]->pixels);
    ELOG("INFO : [elite_pixel_game_destruct] successfully deallocated self->layer[0]->pixels\n");

  };
  ELOG("INFO : [elite_pixel_game_destruct] deallocating self->p_layer[0]\n");

  if (self->p_layer[0]==NULL) {
    ELOG("ERROR : [elite_pixel_game_destruct] self->p_layer[0]==NULL \n");

  //  return false;
  }else {
    free(self->p_layer[0]);
    ELOG("INFO : [elite_pixel_game_destruct] successfully deallocated self->p_layer[0]\n");

  };
    ELOG("INFO : [elite_pixel_game_destruct] calling on_user_destroy()\n");

    if (self->on_user_destroy(self->user)==false){
      ELOG("ERROR : [elite_pixel_game_destruct] user_destroy() returned false\n");


    }else {
      ELOG("INFO : [elite_pixel_game_destruct] user_destroy() returned true\n");


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


void elite_pixel_game_draw_line(elite_pixel_game_t*self,int x1,int y1,int x2,int y2,sfRGBA c_){
    int dx = x2 - x1;
    int dy = y2 - y1;
    sfRGBA c=c_;
    int vx=1;
    int vy=1;
    // If both of the differences are 0 there will be a division by 0 below.
    if (dx == 0 && dy == 0) {

        if ((x1>=0)&&(y1>=0)&&(x1<10)&&(y1<30)) {
            elite_pixel_game_fputpixelRGBA(self,x1,y1,c);
            c.fr*=0.7f;
            c.fg*=0.7f;
            c.fb*=0.7f;
            c.fa*=0.7f;

        }
        return;
    }

    if ((dx*dx) > (dy*dy)) {

        if (x1 > x2) vx*=-1;
        int x=x1;
        while (x!=x2) {
            int y = dy*(x - x1)/dx + y1;
            if ((x>=0)&&(y>=0)&&(x<10)&&(y<30)) {
                elite_pixel_game_fputpixelRGBA(self,x,y,c);
                c.fr*=0.7f;
                c.fg*=0.7f;
                c.fb*=0.7f;
                c.fa*=0.7f;
            }
            x+=vx;
        };

    } else {
        if (y1 > y2) vy*=-1;
        int y=y1;
        while (y!=y2) {
            int x = dx*(y - y1)/dy + x1;
            if ((x>=0)&&(y>=0)&&(x<10)&&(y<30)) {
                elite_pixel_game_fputpixelRGBA(self,x,y,c);
                c.fr*=0.7f;
                c.fg*=0.7f;
                c.fb*=0.7f;
                c.fa*=0.7f;
            };
            y+=vy;
        };

    };
};


bool elite_pixel_game_fputpixelRGBA(elite_pixel_game_t *self,int16_t x,int16_t y,sfRGBA fSrcColor) {
  //tracing pre
      if (elite_pixel_game_fputpixelRGBA_entered_log==false) {
          elite_pixel_game_fputpixelRGBA_entered_log=true;
          ELOG("INFO : [elite_pixel_game_fputpixel] entered elite_pixel_game_fputpixel() - this notification will only occur once\n");

        };

    if ((x<0)||(y<0)||(x>=self->screen_width)||(y>=self->screen_height)) {  //dont hurt the canvas
          canvas_hurt_count+=1;
          if (elite_pixel_game_fputpixelRGBA_dont_hurt_the_canvas_log==false) {
              elite_pixel_game_fputpixelRGBA_dont_hurt_the_canvas_log=true;
              ELOG("ERROR : [elite_pixel_game_fputpixelRGBA] You've hurt the canvas. You're not supposed to hurt the canvas! - this notification will only occur once\n");
              char hurt_str2[256]={0};
              sprintf(hurt_str2,"details : \n config.screen_width=%i\n config.screen_height=%i\n x=%i y=%i\n",self->config.screen_width,self->config.screen_height,x,y);
              ELOG(hurt_str2);

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
        cC.fr=((1.0f/cC.fa)*(cA.fa*cA.fr+((1.0f-cA.fa)*(cB.fa*cB.fr))));
        cC.fg=((1.0f/cC.fa)*(cA.fa*cA.fg+((1.0f-cA.fa)*(cB.fa*cB.fg))));
        cC.fb=((1.0f/cC.fa)*(cA.fa*cA.fb+((1.0f-cA.fa)*(cB.fa*cB.fb))));
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
        ELOG("INFO : [elite_pixel_game_fputpixelRGBA] leaving elite_pixel_game_fputpixelRGBA() - this notification will only occur once\n");

    };

    return true;
};

bool elite_pixel_game_fill_flayerRGBA_entered_log=false;
bool elite_pixel_game_fill_flayerRGBA_leaving_log=false;

bool elite_pixel_game_fill_flayerRGBA(elite_pixel_game_t *self,sfRGBA fill_fcol){

//tracing pre
if (elite_pixel_game_fill_flayerRGBA_entered_log==false) {
    elite_pixel_game_fill_flayerRGBA_entered_log=true;
    ELOG("INFO : [elite_pixel_game_fill_flayer] entered elite_pixel_game_fill_flayer - this notification will only occur once\n");

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
      ELOG("INFO : [elite_pixel_game_fill_flayerRGBA] leaving elite_pixel_game_fill_flayerRGBA - this notification will only occur once\n");

    };

  return true;
}


void elite_pixel_game_draw_filled_circle(elite_pixel_game_t *self, float fx,float fy,float fr,sfRGBA col){
    int x1=(int)(fx-fr-1.0f);
    int x2=(int)(fx+fr+1.0f);
    int y1=(int)(fy-fr-1.0f);
    int y2=(int)(fy+fr+1.0f);

    for (int y=y1;y<y2;y++) {
        for (int x=x1;x<x2;x++) {
            if (x<0&&x>self->screen_width&&y<0&&y>self->screen_height) continue;
            float fdx=fabs(fx-(float)x);
            float fdy=fabs(fy-(float)y);
            float fd=sqrt(fdx*fdx+fdy*fdy);
            if (fd<=fr){
                elite_pixel_game_fputpixelRGBA(self,x,y,col);
            };
        };
    };
};
void elite_pixel_game_draw_shaded_circle(elite_pixel_game_t *self, float fx,float fy,float fr,sfRGBA col){
    int x1=(int)(fx-fr-1.0f);
    int x2=(int)(fx+fr+1.0f);
    int y1=(int)(fy-fr-1.0f);
    int y2=(int)(fy+fr+1.0f);

    for (int y=y1;y<y2;y++) {
        for (int x=x1;x<x2;x++) {
            if (x<0&&x>self->screen_width&&y<0&&y>self->screen_height) continue;
            float fdx=fabs(fx-(float)x);
            float fdy=fabs(fy-(float)y);
            float fd=sqrt(fdx*fdx+fdy*fdy);
            if (fd<=fr){
                float d_normal=1.0f-(fd/fr);
                d_normal*=d_normal;
                sfRGBA c=col;
                c.fa*=d_normal;
                elite_pixel_game_fputpixelRGBA(self,x,y,c);
            };
        };
    };
};


  //        float fa=(1.0f-((d/r)*(d/r)))*255.0f;
   //fa*=0.5;
   //fa+=127;
    //      if (fa>255.0f) fa=255.0f;
   //if (fa<=0.0f) fa=0.0f;

   //if (shade) {c.a=(uint8_t)fa;};
   //if (cEliteTools::apprx(d)<=cEliteTools::apprx(r)) {this->putPixelRGBA(j,i,c);};



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


/*
for (int i=0;i<300;i++) {FRAMEBUF_PIXFORMAT c={0.0f,0.0f,0.0f};self->p_framebuf[i]=c;};

for (int i=0;i<300;i++) {LAYER_PIXFORMAT c={0.0f,0.0f,0.0f,255.0f};self->p_layer[0]->pixels[i]=c;};
*/


if (on_user_update_pre_log==false) {
  on_user_update_pre_log=true;
  ELOG("INFO : [elite_pixel_game_update] calling self->on_user_update() - this notification will only occur once\n");

};



on_user_update_result=self->on_user_update(self->user,self,fElapsedTime);

/*
if (on_user_update_result==true){

    if (on_user_update_post_log==false){
        ELOG("INFO : [elite_pixel_game_update] self->on_user_update() returned true - this notification will only occur once\n");

        on_user_update_post_log=true;
    };
  }else{
    if (on_user_update_post_log==false){
    ELOG("ERROR : [elite_pixel_game_update] self->on_user_update() returned false - this notification will only occur once\n");

    on_user_update_post_log=true;
    };
  };
  */
self->log_internal_update_timer-=self->fElapsedTime;
self->log_internal_update_count+=1;
if (self->log_internal_update_timer<=0.0f) {
  //float f_updates_per_intervall=self->log_internal_update_count/self->log_internal_update_intervall;
  float f_updates_per_second=0.1f*self->log_internal_update_count;
//  ELOG("DEBUG : [%s] on_user_update() happend %i / %f intervall\n",__FUNCTION__,self->log_internal_update_count,self->log_internal_update_intervall);
  ELOG("DEBUG : [%s] on_user_update() happend %f /s\n",__FUNCTION__,f_updates_per_second);
  self->log_internal_update_timer=self->log_internal_update_intervall;
  self->log_internal_update_count=0;
}
//to short circuit or not to short circuit - thats a question
bool result=(on_user_update_result&elite_pixel_game_render_to_framebuf(self));
self->fps_update_timer-=self->fElapsedTime;
if (result&&mr_displays_global_inputstream_handle!=NULL&&self->fps_update_timer<=0.0f) {
  self->fps_update_intervall=1.0f/ente_fps;
  self->fps_update_timer=self->fps_update_intervall;
  xStreamBufferSend(mr_displays_global_inputstream_handle,self->p_framebuf,self->p_framebuf_size,portMAX_DELAY);
};

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


static void elite_pixel_game_task(void* params){
  if (params==NULL) {ELOG("FATAL ERROR : %s","config ptr==NULL (did the data behind it run out of scope?)");};
  vTaskDelay(2000/portTICK_PERIOD_MS);
  assert(params);
  elite_theres_a_pixel_game_running=true;
  elite_kill_pixel_game=false;
  elite_pixel_game_config_t *p_pixel_game_config=(elite_pixel_game_config_t*)params;

  elite_pixel_game_config_t pixel_game_config={
    .app_name=p_pixel_game_config->app_name,
    .screen_width=p_pixel_game_config->screen_width,
    .screen_height=p_pixel_game_config->screen_height,
    .num_layers=p_pixel_game_config->num_layers,
    .on_user_construct=p_pixel_game_config->on_user_construct,
    .on_user_update=p_pixel_game_config->on_user_update,
    .on_user_destroy=p_pixel_game_config->on_user_destroy
  };

  //p_pixel_game_config_t pixel_game_config=(pixel_game_config_t)params;
  //to not fuck up the update window at startup
  //while(ota_has_stopped==false){
//    vTaskDelay(500 / portTICK_PERIOD_MS);
//  };


  ELOG("INFO : [elite_pixel_game_task] started\n");

  //int32_t pixelapp_runtime_limit_ms=60000;
  //int32_t pixelapp_runtime_ms=0;


  ELOG("INFO : [elite_pixel_game_task] calling elite_pixel_game_construct(pixel_app_config\n");


  elite_pixel_game_t *p_pixel_game=elite_pixel_game_construct(pixel_game_config);
  if (p_pixel_game==NULL) {
      ELOG("ERROR : [%s] p_pixel_game==null, constructor failed\n",__FUNCTION__);
      elite_theres_a_pixel_game_running=false;
      vTaskDelete(NULL);
  };
  ELOG("INFO : [%s] pixelapp constructed \n",__FUNCTION__);



  while (true){
    ELITE_CHECK(__FUNCTION__);
    elite_pixel_game_update_fElapsedTime(p_pixel_game);
    ELITE_CHECK(__FUNCTION__);
    elite_pixel_game_update(p_pixel_game,p_pixel_game->fElapsedTime);
    //TODO factor out display output function calls to some more appropriate place ...

    vTaskDelay(5/ portTICK_PERIOD_MS);
    //pixelapp_runtime_ms+=20;
    if (elite_kill_pixel_game==true) break;
  };


  ELOG("INFO : [elite_pixel_game_task] calling elite_pixel_game_destruct()\n");


  if (elite_pixel_game_destruct(p_pixel_game)==false) {
      ELOG("ERROR : [elite_pixel_game_task] elite_pixel_game_destruct(p_pixel_app) returned false\n");
    }else{
      ELOG("INFO : [elite_pixel_game_task] elite_pixel_game_destruct(p_pixel_app) returned true\n");
  };
  vTaskDelay( log_delay/ portTICK_PERIOD_MS);
  ELOG("INFO : [elite_pixel_game_task] killing elite_pixel_game_task\n");

  elite_theres_a_pixel_game_running=false;
  elite_kill_pixel_game=false;
  //exit_condition=true;
  vTaskDelete(NULL);

};


//bool elite_pixel_game_render_to_framebuf();
