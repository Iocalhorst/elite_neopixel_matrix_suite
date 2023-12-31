//todo @ente implement hsv2rgb rgb2hsv

#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"

#pragma once

#define MAX_FW_PARTICLES 300
#define MAX_FW_ROCKETS 10
#define MAX_FW_HALOS 10
#define NUM_ROCKET_COLORS 9

static int bangs_count=0;
sfRGBA elite_fireworks_colors[NUM_ROCKET_COLORS]={
  vivid_yellow,
  vivid_blue,
  vivid_red,
  vivid_orange,
  vivid_magenta,
  vivid_blue,
  vivid_green,
  vivid_purple,
  vivid_cyan
};
const float gravity_discount=0.15f;
const float friction_discount=0.6f;
const float f_gravity=45.0f;
const float low_gravity=f_gravity*gravity_discount;
const float f_friction=5.0f;
const float low_friction=f_friction*friction_discount;
const float fy_base=29.0f;
const float fx_base=4.0f;
const float fvx_base=0.0f;
const float fvy_base=-50.0f;
const float t_base_intervall=0.4f;

typedef struct{
  float fx;
  float fy;
  float fvx;
  float fvy;
  float fuse;
  float radius;
  float fuse_;
  sfRGBA color;
}elite_fireworks_halo_t;

typedef struct{
    int fx_type;
    float fuse;
    float fuse_;
    float fuse_fx;
    bool lock_col;
    float fx;
    float fy;
    float fvx;
    float fvy;
    sfRGBA base_col;
    sfRGBA col;
}elite_fireworks_particle_t;

#define FX_NONE 69
#define FX_FLICKER 71

#define SZ_S 42
#define SZ_M 43
#define SZ_B 44

typedef struct{
    int param_size;
    int param_fx;
    int param_col_idx;
  }rocket_params_t;
typedef struct{
    rocket_params_t params;
    float fuse;
    float fx;
    float fy;
    float fvx;
    float fvy;
}elite_fireworks_rocket_t;



typedef struct {
  char* app_name;
  float timer;
  elite_fireworks_particle_t particles[MAX_FW_PARTICLES];
  elite_fireworks_rocket_t rockets[MAX_FW_ROCKETS];
  elite_fireworks_halo_t halos[MAX_FW_HALOS];
  int fx_lock;
}elite_fireworks_t;


elite_fireworks_t* elite_fireworks_construct(elite_pixel_game_t* ente){

  elite_fireworks_t *self=e_mall0c(__FUNCTION__,sizeof(elite_fireworks_t));

  self->app_name="elite_fireworks";

  for (int i=0;i<MAX_FW_ROCKETS;i++) {self->rockets[i].fuse=-1.0f;};
  for (int i=0;i<MAX_FW_PARTICLES;i++) {self->particles[i].fuse=0.0f;};
  for (int i=0;i<MAX_FW_HALOS;i++) {self->halos[i].fuse=0.0f;};
  self->timer=t_base_intervall;
  self->fx_lock=5;

  return self;
};

static void elite_fireworks_rocket_launch(elite_fireworks_t* self,rocket_params_t rp){

      elite_fireworks_rocket_t *r=NULL;

      for (int i=0;i<MAX_FW_ROCKETS;i++) {
        if (self->rockets[i].fuse<=0.0f) {
          r=&self->rockets[i];
          break;
        };
      };
      if (r==NULL) {ELOG("ERROR : [%s] OUT_OF_ROCKETS\n",__FUNCTION__);return;};
      float modulator=(0.01f*(esp_random()%20));
      r->fuse=0.85f-modulator;
      r->fx=fx_base;
      r->fy=fy_base;
      r->fvx=(fvx_base-(0.175f*fvy_base))+(fvy_base*(0.35f*(0.01f*(esp_random()%100))));
      r->fvy=fvy_base*(0.8f+modulator);
      r->params=rp;
      ELOG("DEBUG : [%s] weeeeeeeeeeeeeeeeeeeeeeee\n",__FUNCTION__);
      return;
};

static void elite_fireworks_particles_draw(elite_fireworks_t* self,elite_pixel_game_t* ente){


      for (int i=0;i<MAX_FW_PARTICLES;i++) {
          elite_fireworks_particle_t* p=&self->particles[i];
          if (p->fuse<=0.0f) continue;
          int x=(int)p->fx;
          int y=(int)p->fy;
          if ((x<0)||(x>=10)||(y<0)||(y>=30)) continue;
          sfRGBA c=p->col;
          elite_pixel_game_set_target_layer(ente,0);
          elite_pixel_game_fputpixelRGBA(ente,x,y,c);
          elite_pixel_game_set_target_layer(ente,1);
          sfRGBA c2=c;
          c2.fa*=0.35f;
          elite_pixel_game_fputpixelRGBA(ente,x,y,c2);


      };
};


static void elite_fireworks_particles_update(elite_fireworks_t* self,float fElapsedTime){

    for (int i=0;i<MAX_FW_PARTICLES;i++) {
        elite_fireworks_particle_t* p=&self->particles[i];
        if (p->fuse<=0.0f) continue;
        p->fuse-=fElapsedTime;
        p->fx+=p->fvx*fElapsedTime;
        p->fy+=p->fvy*fElapsedTime;
        p->fvy+=low_gravity*fElapsedTime;
        p->fvx*=(1.0f-fElapsedTime*low_friction);
        p->fvy*=(1.0f-fElapsedTime*low_friction);
        p->col=p->base_col;
        float t=p->fuse/p->fuse_;
        if (t<0.0f) t=0.0f;
        if (t>1.0f) t=1.0f;
        p->col.fa*=t;
        if (p->fx_type==FX_FLICKER&&t<0.3f) {
            int magic_toggler=(int)(50.0f*t);
            magic_toggler%=2;
            switch (magic_toggler)   {
              case 0 : {
                  p->col=vivid_white;
                  p->lock_col=false;
                  break;
              };
              case 1 : {
                  sfRGBA c={0.0f,0.0f,0.0f,0.0f};
                  p->col=c;
                  p->lock_col=false;
                  break;
              };
              case 2 : {
                  sfRGBA c={0.0f,0.0f,0.0f,0.0f};
                  p->col=c;
                  p->lock_col=false;
                  break;
              };
              case 3 : {
                  if (p->lock_col==false) {
                      p->col=elite_fireworks_colors[esp_random()%NUM_ROCKET_COLORS];
                      p->lock_col=true;
                  };
              break;
              };
              default : p->col=p->base_col;
              };

        };
    };
};



static void elite_fireworks_halos_update(elite_fireworks_t* self,float fElapsedTime){

      for (int i=0;i<MAX_FW_HALOS;i++) {
        elite_fireworks_halo_t* h=&self->halos[i];
        if (h->fuse<=0.0f) continue;
        h->fuse-=fElapsedTime;
        h->fvy+=low_gravity*fElapsedTime;
        h->fvx*=(1.0f-fElapsedTime*low_friction);
        h->fvy*=(1.0f-fElapsedTime*low_friction);
        h->fx+=h->fvx*fElapsedTime;
        h->fy+=h->fvy*fElapsedTime;
      };
};



static void elite_fireworks_halos_add(elite_fireworks_t* self,elite_fireworks_halo_t new_halo){
      ELOG("DEBUG : [%s] enter\n",__FUNCTION__);
      for (int i=0;i<MAX_FW_HALOS;i++) {
        elite_fireworks_halo_t* h=&self->halos[i];
        if (h==NULL) {ELOG("ERROR: [%s] nullptr halo wtf\n",__FUNCTION__);return;};
        if (h->fuse>0.0f) {continue;};
        ELOG("DEBUG : [%s] using i=%i, p=%p\n",__FUNCTION__,i,(void*)h);
        h->fuse=new_halo.fuse;
        h->fuse_=new_halo.fuse;
        h->color=new_halo.color;
        h->fx=new_halo.fx;
        h->fvx=new_halo.fvx;
        h->radius=new_halo.radius;
        h->fy=new_halo.fy;
        h->fvy=new_halo.fvy;
        ELOG("DEBUG : [%s] halo added x=%i, y=%i\n",__FUNCTION__,(int)h->fx,(int)h->fy);
        return;
      };
};

static void elite_fireworks_halos_draw(elite_fireworks_t* self,elite_pixel_game_t* ente){

      for (int i=0;i<MAX_FW_HALOS;i++) {
        elite_fireworks_halo_t* h=&self->halos[i];
        if (h->fuse<=0.0f) continue;
        int x=(int)h->fx;
        int y=(int)h->fy;
        float r1=0.25f*h->radius;
        float r2=0.75f*h->radius;
        float modulator=(h->fuse/h->fuse_);
        r2*=(1.0f-modulator);
        float r=r1+r2;

        sfRGBA c=h->color;
       //sfRGBA c={255.0f,224.0f,212.0f,255.0f};
        //elite_pixel_game_draw_filled_circle(ente,x,y,r,c);
        elite_pixel_game_draw_shaded_circle(ente,x,y,r,c);
      };
};


static bool elite_fireworks_particles_add(elite_fireworks_t* self,elite_fireworks_particle_t new_particle){
      elite_fireworks_particle_t* p=NULL;
      for (int i=0;i<MAX_FW_PARTICLES;i++) {
          if (self->particles[i].fuse<=0.0f) {
              p=&self->particles[i];
              break;
            };
      };
      if (p==NULL) return false;
      //*p=new_particle;
      p->fx=new_particle.fx;
      p->fx_type=new_particle.fx_type;
      p->fy=new_particle.fy;
      p->fvx=new_particle.fvx;
      p->fvy=new_particle.fvy;
      p->fuse=new_particle.fuse;
      p->fuse_=new_particle.fuse;
      p->base_col=new_particle.col;
      const float spread=0.3f;
      p->base_col.fr*=(1.0f-spread);
      p->base_col.fr*=1.0f+(spread*(esp_random()%100/100.0f));
      p->base_col.fg*=(1.0f-spread);
      p->base_col.fg*=1.0f+(spread*(esp_random()%100/100.0f));
      p->base_col.fb*=(1.0f-spread);
      p->base_col.fb*=1.0f+(spread*(esp_random()%100/100.0f));
      p->base_col.fa*=(1.0f-spread);
      p->base_col.fa*=1.0f+(spread*(esp_random()%100/100.0f));
      p->lock_col=false;
      if (p->base_col.fr>=255.0f){p->base_col.fr=255.0f;};
      if (p->base_col.fg>=255.0f){p->base_col.fg=255.0f;};
      if (p->base_col.fb>=255.0f){p->base_col.fb=255.0f;};
      if (p->base_col.fa>=255.0f){p->base_col.fa=255.0f;};
      if (p->base_col.fr<=0.0f){p->base_col.fr=0.0f;};
      if (p->base_col.fg<=0.0f){p->base_col.fg=0.0f;};
      if (p->base_col.fb<=0.0f){p->base_col.fb=0.0f;};
      if (p->base_col.fa<=0.0f){p->base_col.fa=0.0f;};




      return true;
};

static void elite_fireworks_rocket_explode(elite_fireworks_t* self,elite_fireworks_rocket_t* r){

      const float f_speed_min=6.0f;
      //ignore param_size;
      //ignore param_fx;
      //sfRGBA c={255.0f,255.0f,255.0f,255.0f};
      bangs_count+=1;
      ELOG("DEBUG : [%s] bang! %i \n",__FUNCTION__,bangs_count);

      //lets go for 36 particles in a circle - without randomization of velociti
      float w=1.0f;
      sfRGBA halo_col={255.0f,255.0f,255.0f,255.0f};
      sfRGBA r_col=elite_fireworks_colors[r->params.param_col_idx];
      halo_col.fr+=(r_col.fr*w);
      halo_col.fg+=(r_col.fg*w);
      halo_col.fb+=(r_col.fb*w);
      halo_col.fa+=(r_col.fa*w);

      //in before worst clampling code ever
      while ((halo_col.fr>255.0f)||(halo_col.fg>255.0f)||(halo_col.fb>255.0f)||(halo_col.fa>255.0f)){
        halo_col.fr*=0.9f;
        halo_col.fg*=0.9f;
        halo_col.fb*=0.9f;
        halo_col.fa*=0.9f;
      }

      if (r==NULL) {ELOG("ERROR : [%s] nullptr \n",__FUNCTION__); return;};
      elite_fireworks_halo_t h={
        .fx=r->fx,
        .fy=r->fy,
        .fvx=r->fvx,
        .fvy=r->fvy,
        .fuse=0.2f,
        .radius=3.4f,
        .color=halo_col
      };
      if (r->params.param_size==SZ_M) {
        h.radius*=1.6f;
      };
      elite_fireworks_halos_add(self,h);

      int density=22;
      if (r->params.param_size==SZ_M) {density=44;};
      for (int i=0;i<density;i++) {
        int random_deg=esp_random()%360;
        float alpha=deg2rad*random_deg;
        //todo try adding the rockets inertia to the particles :-)
        float modulator=0.012f*(esp_random()%100);
        float p_fuse=1.0f;
        float f_speed=f_speed_min;


        if (r->params.param_size==SZ_S) {
            p_fuse=1.25f-(0.25f*modulator);
            f_speed*=(0.9f+0.9f*modulator);
        }

        if (r->params.param_size==SZ_M) {
          p_fuse=2.75f-(0.75f*modulator);
          if (i%3) {f_speed*=(1.0f+1.1f*modulator);
          }else {f_speed*=(1.8f+1.8f*modulator);};
        };


        elite_fireworks_particle_t new_particle={
          .fx=r->fx,
          .fy=r->fy,
          .fvx=cosf(alpha)*f_speed+r->fvx,
          .fvy=sinf(alpha)*f_speed+r->fvy,
          .col=elite_fireworks_colors[r->params.param_col_idx],
          .base_col=elite_fireworks_colors[r->params.param_col_idx],
          .fuse=p_fuse,
          .fuse_=p_fuse,
          .fx_type=r->params.param_fx
        };

        if (elite_fireworks_particles_add(self,new_particle)==false) {
          ELOG("DEBUG : [%s] out of particles :-( )\n",__FUNCTION__);
          return;
        };
      };

};
static void elite_fireworks_rockets_update(elite_fireworks_t* self,float fElapsedTime){
  for (int i=0;i<MAX_FW_ROCKETS;i++) {
    elite_fireworks_rocket_t *r=&self->rockets[i];
    if (r->fuse<=0.0f) continue;
    r->fuse-=fElapsedTime;
    r->fvy+=f_gravity*fElapsedTime;
    r->fvx*=(1.0f-fElapsedTime);
    r->fx+=(r->fvx*fElapsedTime);
    r->fy+=(r->fvy*fElapsedTime);
    //ELOG("DEBUG : [%s] bla\n",__FUNCTION__);


    if (r->fuse<=0.0f) {
      if (r->params.param_size==SZ_M) self->timer+=1.5f*t_base_intervall;
      elite_fireworks_rocket_explode(self,r);};
  };
};

static void elite_fireworks_rockets_draw(elite_fireworks_t* self,elite_pixel_game_t* ente){

  sfRGBA c={255.0f,192.0f,69.0f,255.0f};
  for (int i=0;i<MAX_FW_ROCKETS;i++){
    elite_fireworks_rocket_t* r=&self->rockets[i];
    if (r->fuse<=0.0f) continue;
    int x=r->fx;
    int y=r->fy;
    elite_pixel_game_set_target_layer(ente,0);
    elite_pixel_game_fputpixelRGBA(ente,x,y,c);
    elite_pixel_game_set_target_layer(ente,1);
    sfRGBA c2=c;
    c2.fa*=0.4f;
    elite_pixel_game_fputpixelRGBA(ente,x,y,c2);
  };
};

//filter types :
//           f_scalar : c.red = c.red+255.0f*x[0.0f..1.0f]; //dunno what u want this for but ....
//           f_linear : c.red = c.red*(1.0f-x[0.0f..1.0f]);
//           f_exp    : c.red = pow(c.red_normalized,1.0f-x[]

//set_target_layer(1);
static void elite_pixel_game_layer_apply_filter(elite_pixel_game_t* ente,float fElapsedTime){

      float f_tmp=1.0f-fElapsedTime*30.5f;
      //f_tmp*=f_tmp;

      for (int y=0;y<ente->config.screen_height;y++) {
          for (int x=0;x<ente->config.screen_width;x++) {
                int idx=0;
                idx=ente->config.screen_width*y+x;
                sfRGBA c=ente->p_layer[1]->pixels[idx];
                c.fa/=255.0f;
                c.fa*=f_tmp;
                c.fa*=255.0f;
                if (c.fa<=3.0f) {c.fa=0.0f;};
                if (c.fa>=255.0f) {c.fa=255.0f;};
                ente->p_layer[1]->pixels[idx]=c;
          }
      }
};

static bool elite_fireworks_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
  elite_fireworks_t *self=(elite_fireworks_t*)params;

  elite_pixel_game_set_target_layer(ente,0);
  sfRGBA c={0.0f,0.0f,0.0f,255.0f};
  elite_pixel_game_fill_flayerRGBA(ente,c);

  elite_pixel_game_set_target_layer(ente,1);
  elite_pixel_game_layer_apply_filter(ente,fElapsedTime);

  elite_pixel_game_set_target_layer(ente,0);
  for (int i=0;i<10;i++) {
    for (int j=0;j<30;j++) {
      int idx=i+j*10;
      sfRGBA c=ente->p_layer[1]->pixels[idx];
      elite_pixel_game_fputpixelRGBA(ente,i,j,c);
    };
  };

  self->timer-=fElapsedTime;
  if (self->timer<=0.0f) {

      self->timer=t_base_intervall;//todo: randomizes release gun_intervall
      self->timer+=t_base_intervall*(0.01f*(esp_random()%60));//todo: randomizes release gun_intervall

      rocket_params_t rp={
        .param_col_idx=esp_random()%NUM_ROCKET_COLORS,
        .param_fx=FX_NONE,
        .param_size=SZ_S
      };
      if (self->fx_lock>0) {
          self->fx_lock-=1;
      }else{
          if ((esp_random()%5)==1){
              rp.param_size=SZ_M;
              rp.param_fx=FX_FLICKER;
              self->fx_lock=5;
          };
      };
      elite_fireworks_rocket_launch(self,rp);
  };

  elite_fireworks_rockets_update(self,fElapsedTime);
  elite_fireworks_particles_update(self,fElapsedTime);
  elite_fireworks_halos_update(self,fElapsedTime);
  elite_fireworks_particles_draw(self,ente);
  elite_fireworks_halos_draw(self,ente);
  elite_fireworks_rockets_draw(self,ente);

//sfRGBA c_test={127.f,42.0f,255.0f,255.0f};
//elite_pixel_game_draw_shaded_circle(ente,5,22,3.5f,c_test);
//elite_pixel_game_draw_filled_circle(ente,5,8,3.5f,c_test);

  return true;
};


static bool elite_fireworks_on_user_destroy(void* params){
    elite_fireworks_t *self=(elite_fireworks_t*)params;

    if (self!=NULL) {free(self);return false;};

    return true;

};


elite_pixel_game_config_t elite_fireworks_config={
    .app_name="elite_fireworks",
    .screen_width=10,
    .screen_height=30,
    .num_layers=3,
    .on_user_construct=(void*)&elite_fireworks_construct,
    .on_user_update=&elite_fireworks_on_user_update,
    .on_user_destroy=&elite_fireworks_on_user_destroy
};

void elite_fireworks_start_task(){


    xTaskCreate(&elite_pixel_game_task, "elite_fireworks_task", 8192,&elite_fireworks_config, 5, NULL);



};
