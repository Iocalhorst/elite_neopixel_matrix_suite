//simple particle physics utility header only library
//particleshower class with for simple rain like background generation
//user asset template for elite_pixel_game_ente applications
//configurable via particle_shower_construct(config);
//batch particle[] physics via particle_shower_update(fElapsedTime);
//render_draw() via pixel_game_ente as parameter.
//see elite_pixel_game_ente.h for api details
#include "elite.h"
#include "elite_pixel_game_ente.h"
#pragma once

typedef struct{
  float fx;
  float fy;
  float fvx;
  float fvy;
  sfRGB col;
}particle_t;


typedef struct{
  size_t num_particles;
}particle_shower_config_t;

typedef struct{
  particle_t *particles;
  size_t num_particles;
}particle_shower_t;

 particle_shower_t* particle_shower_construct(particle_shower_config_t self_config){
  particle_shower_t *self=(particle_shower_t*)malloc(sizeof(particle_shower_t));
  self->num_particles=self_config.num_particles;
  self->particles=(particle_t*)malloc(self->num_particles*sizeof(particle_t));
  for (int i=0;i<self->num_particles;i++){
    self->particles[i].fx=(float)(esp_random()%10);
    self->particles[i].fy=-30.0f-(float)(esp_random()%30);
    self->particles[i].fvx=0.0f;
    float fvy=10.0f+(float)(esp_random()%5);
    float frandr=10.0f+(float)(esp_random()%10);
    float frandb=10.0f+(float)(esp_random()%14);

    self->particles[i].fvy=fvy;
    //sfRGB c={10.0f+fvy,5.0f+fvy*0.5f,15.0f+fvy*1.5f};

    //sfRGB c={10.0f+1.0f*fvy+frandr,5.0f+fvy*0.5f,15.0f+fvy+frandb};
    sfRGB c={5.0f+0.5f*fvy+frandr,5.0f+fvy*0.25f,10.0f+fvy+frandb};
    self->particles[i].col=c;
  };
  return self;
};

bool particle_shower_update(particle_shower_t *self, float fElapsedTime){
  for (int i=0;i<self->num_particles;i++){
    self->particles[i].fx+=self->particles[i].fvx*fElapsedTime;
    self->particles[i].fy+=self->particles[i].fvy*fElapsedTime;
    if (self->particles[i].fy>=30.0f) {
      float fvy=10.0f+(float)(esp_random()%5);
      float frandr=10.0f+(float)(esp_random()%10);
      float frandb=10.0f+(float)(esp_random()%14);


      self->particles[i].fvy=fvy;
      //sfRGB c={10.0f+1.5f*fvy,5.0f+fvy*0.25f,15.0f+fvy*2.0f};
          sfRGB c={5.0f+0.5f*fvy+frandr,5.0f+fvy*0.25f,10.0f+fvy+frandb};
      self->particles[i].col=c;
      self->particles[i].fy-=30.0f;
    };
  };
  return true;
};

bool particle_shower_draw(elite_pixel_game_t* ente,particle_shower_t *self){
  for (int i=0;i<self->num_particles;i++){
    //elite_pixel_game_fputpixel(elite_pixel_game_t* self,int16_t x,int16_t y,sfRGB col)
    int16_t x=(int16_t)self->particles[i].fx;
    int16_t y=(int16_t)self->particles[i].fy;
    if (y>=0&&y<30&&x>=0&&x<10) {elite_pixel_game_fputpixel(ente,x,y,self->particles[i].col);};
  };
  return true;
};

bool particle_shower_destruct(particle_shower_t *self){
  free(self->particles);
  free(self);
  self=NULL;
  return true;
};
