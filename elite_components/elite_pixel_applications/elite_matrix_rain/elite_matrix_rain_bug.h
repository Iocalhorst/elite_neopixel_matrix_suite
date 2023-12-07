#pragma once
#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "elite_glyphs.h"
#include "main.h"

#define NUESCHTS ((void*)0))
#define NUM_ROWS 10
#define NUM_COLS 3

typedef struct {
  char* app_name;
  char* m_chars;
  float* m_floats;
  float fx;
  float fy;
}elite_matrix_rain_t;

int elite_index_of(int x,int y){
  if (x>=0&&x<NUM_COLS&&y>=0&&y<NUM_ROWS) {
    int r=(y*NUM_COLS)+x;
    return r;
  };
  ELOG("ERROR : index_of outOfBounds\n");
  vTaskDelay(1000/portTICK_PERIOD_MS);
  return -1;
}

elite_matrix_rain_t* elite_matrix_rain_construct(elite_pixel_game_t* ente){

  ELOG("DEBUG : ctor enter elite_matrix_rain\n");
  vTaskDelay(1000/portTICK_PERIOD_MS);

  elite_matrix_rain_t *self=(elite_matrix_rain_t*)e_mall0c(__FUNCTION__,sizeof(elite_matrix_rain_t));
  if (self==NULL){
    ELOG("ERROR : [elite_matrix_rain_construct] e_mall0c fail\n");
    return NULL;
  };

  float* p_fx=(float*)e_mall0c(__FUNCTION__,(sizeof(float)*(NUM_ROWS*NUM_COLS)));
  self->m_floats=p_fx;


  if (p_fx==NULL){ELOG("ERROR : [ctor] p42_fx?null\n");};

  //  ELOG("DEBUG : [ctor] p42_fx==%p\n",p42_fx);

//  if (!(p_fx==NULL)){};
  //   (void)0;
  //}else{
    //(void)0;
  //};


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

  char* p_chars=(char*)e_mall0c(__FUNCTION__,(sizeof(char)*NUM_ROWS*NUM_COLS));
  if (p_chars==NULL) {ELOG("ERROR : e_mall0c fail\n");return NULL;};
      self->m_chars=p_chars;


  self->fx=(float)(esp_random()%NUM_COLS);
  //self->fx=2.0f;
  self->fy=0.0f;

  for (int yy=0;yy<NUM_ROWS;yy++) {
      for (int xx=0;xx<NUM_COLS;xx++) {
          int idx=elite_index_of(xx,yy);
          if (idx>=0) {
              self->m_chars[idx]=16+yy;
              self->m_floats[idx]=1.0f;
          }else{
            ELOG("DEBUG : assign fail");
          };
      };
  };
  self->app_name="elite_matrix_rain";
  ELOG("DEBUG : ctor exit elite_matrix_rain\n");
  vTaskDelay(1000/portTICK_PERIOD_MS);
    return self;
};

void elite_matrix_draw_char(elite_pixel_game_t* ente,int _x,int _y,char c,sfRGBA col) {
    int idx=0;
    for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++) {
            idx+=1;
            int arr_idx=0;
                arr_idx+=((int)c);
            if (elite_glyphs[arr_idx][idx-1]!=0) {
                int x=_x+j;
                int y=_y+i;
                if (x>=0&&x<10&&y>=0&&y<30) {
                    elite_pixel_game_fputpixelRGBA(ente,x,y,col);
                };
            };
        };
    };
};

static sfRGBA col_matrix={16.0f,192.0f,8.0f,192.0f};
static sfRGBA col_cursor={32.0f,255.0f,42.0f,255.0f};
bool elite_matrix_rain_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
  elite_matrix_rain_t *self=(elite_matrix_rain_t*)params;
  elite_pixel_game_set_target_layer(ente,0);
  sfRGBA c={0.0f,8.0f,0.0f,255.0f};
  elite_pixel_game_fill_flayerRGBA(ente,c);



  self->fy+=0.65*(fElapsedTime*20.0f);
  if (self->fy>=(30.0f)) self->fy=0.0f;

  //int x_debug=(int)self->fx;
  int y_debug=(int)self->fy;
  sfRGBA c_debug={172.0f,16.0f,16.0f,255.0f};
  elite_pixel_game_fputpixelRGBA(ente,9,y_debug,c_debug);
  int shader_y=(int)(self->fy/3);
  int shader_x=(int)(self->fx/NUM_COLS);
  shader_x=2;


  for (int i=0;i<(NUM_ROWS*NUM_COLS);i++) {
    float newf=self->m_floats[i];
    newf-=fElapsedTime*0.5f;
    if (newf<0.05f) {newf=0.00f;};
    //if (newf>0.5f) {newf=0.5f;};
    self->m_floats[i]=newf;
  };

  self->m_floats[elite_index_of(shader_x,shader_y)]=1.0f;


  for (int i_row=0;i_row<NUM_ROWS;i_row++){
    for (int i_col=0;i_col<NUM_COLS;i_col++) {
        int idx=elite_index_of(i_col,i_row);
        char char_local=self->m_chars[idx];
        sfRGBA col_local=col_matrix;
        sfRGBA col_cursor2={128.0f,255.0f,42.0f,255.0f};
        float m_local=self->m_floats[idx];
        col_local.fa*=m_local;
        col_local.fr*=m_local;
        col_local.fg*=m_local;
        col_local.fb*=m_local;
  /*      if (m_local==1.0f) {
          col_local.fr=col_cursor2.fr;
          col_local.fg=col_cursor2.fg;
          col_local.fb=col_cursor2.fb;
          col_local.fa=col_cursor2.fa;
        };*/
        //col_local=col_matrix;col_local.fa*=self->m_floats[idx];
        //if (idx==elite_index_of(shader_x,shader_y)) {col_local=col_cursor;}
        //else {col_local=col_matrix;};
      //  if ((i_row==shader_y)&&(i_col==shader_x)) {col_local=col_cursor;};
        int canvas_x=i_col*3;
        int canvas_y=i_row*3;
        elite_matrix_draw_char(ente,canvas_x,canvas_y,char_local,col_local);
    }

  }

//  int idx=elite_index_of(4,4);
  //sfRGBA col={12.0f,152.0f,0.0f,192.0f};
  //char c0=self->m_chars[0]-(int)' ';
  //char c1=self->m_chars[1]-(int)' ';
  //char c2=self->m_chars[2]-(int)' ';
  //char c3=self->m_chars[3]-(int)' ';
  //char c4=self->m_chars[4]-(int)' ';
  //char c5=self->m_chars[5]-(int)' ';
  //char c6=self->m_chars[6]-(int)' ';
  //char c7=self->m_chars[7]-(int)' ';
  //char c8=self->m_chars[8]-(int)' ';
  //char c9=self->m_chars[9]-(int)' ';
  /*
  elite_matrix_draw_char(ente,1,2,c0,col);
  elite_matrix_draw_char(ente,1,7,c1,col);
  elite_matrix_draw_char(ente,1,12,c2,col);
  elite_matrix_draw_char(ente,1,17,c3,col);
  elite_matrix_draw_char(ente,1,22,c4,col);
  elite_matrix_draw_char(ente,5,2,c5,col);
  elite_matrix_draw_char(ente,5,7,c6,col);
  elite_matrix_draw_char(ente,5,12,c7,col);
  elite_matrix_draw_char(ente,5,17,c8,col);
  elite_matrix_draw_char(ente,5,22,c9,col);
*/

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
