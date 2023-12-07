#pragma once
#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"

#define NUESCHTS ((void*)0))

typedef struct {
  char* app_name;
  char* m_chars;
  float* m_floats;
  float fx;
  float fy;
}elite_matrix_rain_t;
/*
int elite_index_of(int x,int y){
  if (x>=0&&x<5&&y>=0&&y<15) return (5*y+x);
  ELOG("ERROR : index_of outOfBounds\n");
  vTaskDelay(1000/portTICK_PERIOD_MS);
  return -1;
}
*/
elite_matrix_rain_t* elite_matrix_rain_construct(elite_pixel_game_t* ente){

  ELOG("DEBUG : ctor enter elite_matrix_rain\n");
  vTaskDelay(1000/portTICK_PERIOD_MS);

  elite_matrix_rain_t *self=(elite_matrix_rain_t*)e_mall0c(__FUNCTION__,sizeof(elite_matrix_rain_t));
  if (self==NULL){
    ELOG("ERROR : [elite_matrix_rain_construct] e_mall0c fail\n");
    return NULL;
  };

  float* p42_fx=(float*)e_mall0c(__FUNCTION__,(sizeof(float)*15*5));
  self->m_floats=p42_fx;


  if (p42_fx==NULL){};
    ELOG("ERROR : [ctor] p42_fx?null\n");

  //  ELOG("DEBUG : [ctor] p42_fx==%p\n",p42_fx);

  if (!(p42_fx==NULL)){};
     (void)0;
  }else{
    (void)0;
  };


  //if (p42_fx!=NUESCHTS){};
  //  (void)0;
  //}else{
  //  (void)0;
    //ELOG("ERROR : e_mall0c fail");
    //vTaskDelay(1000/portTICK_PERIOD_MS);
    //return NULL;
    //self->m_floats=p_fx;
//  };

//  if (p_fx!=NULL) {
//  }else{
    //ELOG("ERROR : e_mall0c fail");
    //vTaskDelay(1000/portTICK_PERIOD_MS);
    //return NULL;
    //self->m_floats=p_fx;
//  };
/*
  char* p_chars=(char*)e_mall0c(__FUNCTION__,15*5*sizeof(char));
  if (p_chars!=NULL) {
    self->matrix_chars=p_chars;
  } else{
    ELOG("ERROR : e_mall0c fail\n");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    return NULL;
  };
  self->fx=(float)(esp_random()%5);
  self->fx=2.0f;
  self->fy=1.0f;
*/
  /*for (int i=0;i<15;i++) {
    for (int j=0;j<5;j++) {
      int idx=elite_index_of(j,i);
      self->matrix_chars[idx]=40+(esp_random()%(127-48));
      self->matrix_fx[idx]=1.0f;
    }*/

  self->app_name="elite_matrix_rain";
  ELOG("DEBUG : ctor exit elite_matrix_rain\n");
  vTaskDelay(1000/portTICK_PERIOD_MS);
    return self;
};
/*
void elite_matrix_draw_char(elite_pixel_game_t* ente,int x,int y,char c,sfRGBA col) {
    for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++) {
            int idx=j+i*3;
            if (elite_glyphs[c%70][idx]!=0) {
                if (x>=0&&x<10&&y>=0&&y<30) {
                    elite_pixel_game_fputpixelRGBA(ente,x+j,y+i,col);
                };
            };
        };
    };
};
*/
bool elite_matrix_rain_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
  elite_matrix_rain_t *self=(elite_matrix_rain_t*)params;
  elite_pixel_game_set_target_layer(ente,0);
  /*sfRGBA c={6.0f,16.0f,8.0f,255.0f};
  elite_pixel_game_fill_flayerRGBA(ente,c);

  return true;

  self->fy+=fElapsedTime;
  if (self->fy>=15.0f) self->fy=1.0f;

  int idx=elite_index_of(4,4);*/
//  self->matrix_fx[idx]=0.5f;

  //for (int i=0;i<15;i++) {
    //for (int j=0;j<5;j++) {
//        self->matrix_fx[i][j]-=fElapsedTime;
  //      if (self->matrix_fx[i][j]<=0.0f) {self->matrix_fx[i][j]=0.0f;};
    //}
  //}
//  int ix=(int)self->fx;
//  int iy=(int)self->fy;
  //if (iy>=0&&iy<15&&ix>=0&&ix<5) {
      //self->matrix_fx[idy][idx]=0.75f;
    //  sfRGBA c={127.0f,127.0f,127.0f,172.0f};
      //elite_pixel_game_fputpixelRGBA(ente,ix*2,iy*2,c);
  //};

  /*for (int i=0;i<15;i++) {
    for (int j=0;j<5;j++) {
        char c=self->matrix_chars[i][j];
        sfRGBA col={0.0f,255.0f,0.0f,255.0f};
        col.fa*=self->matrix_fx[i][j];
        int x=j*2;
        int y=i*2;
    //    if (col.fa>=0.05f) {elite_matrix_draw_char(ente,x,y,c,col);};
    }
  }*/



  return true;
};


bool elite_matrix_rain_on_user_destroy(void* params){
    elite_matrix_rain_t *self=(elite_matrix_rain_t*)params;

    if (self!=NULL) {free(self);return false;};

    return true;

};



void elite_matrix_rain_start_task(){

    elite_pixel_game_config_t pixel_game_config={
        .app_name="elite_matrix_rain",
        .screen_width=10,
        .screen_height=30,
        .on_user_construct=(void*)&elite_matrix_rain_construct,
        .on_user_update=&elite_matrix_rain_on_user_update,
        .on_user_destroy=&elite_matrix_rain_on_user_destroy
    };
    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&pixel_game_config, 5, NULL);



};
