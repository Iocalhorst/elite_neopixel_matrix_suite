
#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"

#pragma once

typedef struct{
    float fx;
    float fy;
    float fvy;
    float fvx;
    float fspeed;
    sfRGBA core_color;
    sfRGBA light_color;
    float fr;
    int illumination_map[10][30];

}light_source_t;

typedef struct {
  int x;
  int y;
  sfRGBA color;
}block_t;


typedef struct{

}map_t;
typedef struct {
  char* app_name;
  light_source_t light_source;
  int map[10][30];
}line_of_sight_demo_t;



bool line_of_sight_demo_on_user_update_entered_log=false;
bool line_of_sight_demo_on_user_update_leaving_log=false;
bool line_of_sight_demo_on_user_update_pre_particle_shower_update_log=false;

//void light_source_cast_ray(light_source_t* ls,int x,int y){
void light_source_cast_ray(light_source_t *ls,int x,int y){

    int illumination_value=1;
    int x1=(int)ls->fx;
    int y1=(int)ls->fy;
    int x2=x;
    int y2=y;
    int dx = x2 - x1;
    int dy = y2 - y1;
  //  sfRGBA c=c_;
    int vx=1;
    int vy=1;
    // If both of the differences are 0 there will be a division by 0 below.
    if (dx == 0 && dy == 0) {

        if ((x1>=0)&&(y1>=0)&&(x1<10)&&(y1<30)) {
          ls->illumination_map[x][y]=illumination_value;
            //elite_pixel_game_fputpixelRGBA(ente,x1,y1,c);
          //  c.fr*=0.7f;
//            c.fg*=0.7f;
            //c.fb*=0.7f;
            //c.fa*=0.7f;

        }
        return;
    }

    if ((dx*dx) > (dy*dy)) {

        if (x1 > x2) vx*=-1;
        int x=x1;
        do {
            x+=vx;
            int y = dy*(x - x1)/dx + y1;
            if ((x>=0)&&(y>=0)&&(x<10)&&(y<30)) {
                if (ls->illumination_map[x][y]<0) {return;illumination_value=+1;};
                ls->illumination_map[x][y]=illumination_value;

            }

        }while (x!=x2);

    } else {
        if (y1 > y2) vy*=-1;
        int y=y1;
        do  {
            y+=vy;
            int x = dx*(y - y1)/dy + x1;
            if ((x>=0)&&(y>=0)&&(x<10)&&(y<30)) {
              if (ls->illumination_map[x][y]<0) {return;illumination_value=+1;};
              ls->illumination_map[x][y]=illumination_value;

            };

        }while (y!=y2);

    };
};

void line_of_sight_demo_cast_illumination_map(line_of_sight_demo_t* self,light_source_t* ls){

    for (int y=0;y<30;y++){
        for (int x=0;x<10;x++){
            ls->illumination_map[x][y]=0;
            ls->illumination_map[x][y]-=self->map[x][y];
        };
    };


    for (int cursor_x=0;cursor_x<10;cursor_x++) {light_source_cast_ray(ls,cursor_x,-1);};
    for (int cursor_y=0;cursor_y<30;cursor_y++) {light_source_cast_ray(ls,10,cursor_y);};
    for (int cursor_x=0;cursor_x<10;cursor_x++) {light_source_cast_ray(ls,cursor_x,30);};
    for (int cursor_y=0;cursor_y<30;cursor_y++) {light_source_cast_ray(ls,-1,cursor_y);};



  /*light_source_cast_ray(ls,0,0);
  light_source_cast_ray(ls,0,0);
  light_source_cast_ray(ls,9,0);
  light_source_cast_ray(ls,0,29);
  light_source_cast_ray(ls,9,29);*/
};

line_of_sight_demo_t* line_of_sight_demo_construct(elite_pixel_game_t* ente){
  line_of_sight_demo_t *self=e_mall0c(__FUNCTION__,sizeof(line_of_sight_demo_t));
  if (self!=NULL) {
    ELOG("INFO : [line_of_sight_demo_construct] successfully allocated self(line_of_sight_demo_t)\n");

  }else {
    ELOG("ERROR : [line_of_sight_demo_construct] failed to allocated self(line_of_sight_demo_t); returning NULL from constructor\n");

    return NULL;
  };
  self->app_name="line_of_sight_demo";//unused
  ELOG("INFO : [line_of_sight_demo_construct] successfully constructed self(line_of_sight_demo_t); returning self from constructor\n");

  sfRGBA c={255.0f,172.0f,142.0f,255.0f};
  float f_bla=0.55f;
  //sfRGBA c2={172.0f*f_bla,112.0f*f_bla,42.0f*f_bla,172.0f};
//  sfRGBA c2={96.0f*f_bla,68.0f*f_bla,192.0f,172.0f};
  sfRGBA c2={64.0f*f_bla,192.0f,68.0f*f_bla,152.0f};
  self->light_source.core_color=c;
  self->light_source.light_color=c2;
  self->light_source.fx=0.0f;
  self->light_source.fy=0.0f;
  self->light_source.fspeed=+6.25f;
  self->light_source.fvx=self->light_source.fspeed;
  self->light_source.fvy=0.0f;
  self->light_source.fr=15.0f;
uint8_t map_[30][10]={{0,0,0,0,0,0,0,0,0,0},//0
                      {0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0},
                      {0,0,1,1,1,1,1,1,0,0},//4
                      {0,0,1,1,1,1,1,1,0,0},
                      {0,0,0,0,0,0,1,1,0,0},
                      {0,0,0,0,0,0,1,1,0,0},
                      {0,0,0,0,0,0,1,1,0,0},//9
                      {0,0,0,0,0,0,1,1,0,0},
                      {0,0,1,1,0,0,0,0,0,0},
                      {0,0,1,1,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0},//14
                      {0,0,0,0,1,1,0,0,0,0},
                      {0,0,0,0,1,1,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,1,1,0,0},//19
                      {0,0,0,0,0,0,1,1,0,0},
                      {0,0,1,1,0,0,0,0,0,0},
                      {0,0,1,1,0,0,0,0,0,0},
                      {0,0,1,1,0,0,0,0,0,0},
                      {0,0,1,1,0,0,0,0,0,0},//24
                      {0,0,1,1,1,1,1,1,0,0},
                      {0,0,1,1,1,1,1,1,0,0},
                      {0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0}};//29
  for (int y=0;y<30;y++){
      for (int x=0;x<10;x++){
          self->map[x][y]=map_[y][x];
      };
  };

  return self;
};

bool line_of_sight_demo_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
//debug tracing in
    if (line_of_sight_demo_on_user_update_entered_log==false) {
        line_of_sight_demo_on_user_update_entered_log=true;
        ELOG("INFO : [line_of_sight_demo_update] entered line_of_sight_demo_update() - this notification will only occur once\n");

        };
//body

    line_of_sight_demo_t *self=(line_of_sight_demo_t*)params;

    float fdx=self->light_source.fvx*fElapsedTime;
    float fdy=self->light_source.fvy*fElapsedTime;
    bool bounce=false;

    if (self->light_source.fx+fdx>(float)ente->screen_width-1.0f){
        self->light_source.fvx=0.0f;
        self->light_source.fvy=self->light_source.fspeed;
        bounce=true;
    };

    if (self->light_source.fy+fdy>(float)ente->screen_height-1.0f){
        self->light_source.fvx=-1.0f*self->light_source.fspeed;
        self->light_source.fvy=0.0f;
        bounce=true;
    };

    if (self->light_source.fx+fdx<0.0f){
        self->light_source.fvx=0.0f;
        self->light_source.fvy=-1.0f*self->light_source.fspeed;
        bounce=true;
    };

    if (self->light_source.fy+fdy<0.0f){
        self->light_source.fvx=self->light_source.fspeed;
        self->light_source.fvy=0.0f;
        bounce=true;
    };

    if (bounce==false) {self->light_source.fx+=fdx;self->light_source.fy+=fdy;};

    elite_pixel_game_set_target_layer(ente,0);
    sfRGBA c={0.0f,0.0f,0.0f,255.0f};
    elite_pixel_game_fill_flayerRGBA(ente,c);

    line_of_sight_demo_cast_illumination_map(self,&self->light_source);
    elite_pixel_game_draw_filled_circle(ente,self->light_source.fx,self->light_source.fy,1.0f,self->light_source.core_color);


    for (int y=0;y<30;y++){
        for (int x=0;x<10;x++){
            float dx=(float)x-self->light_source.fx;
            float dy=(float)y-self->light_source.fy;
            float distance=sqrt(dx*dx+dy*dy);
            sfRGBA c_light;
            c_light.fr=self->light_source.light_color.fr;
            c_light.fg=self->light_source.light_color.fg;
            c_light.fb=self->light_source.light_color.fb;
            c_light.fa=self->light_source.light_color.fa;
            sfRGBA c_shadow;
            c_shadow.fr=c_light.fr*0.25f;
            c_shadow.fg=c_light.fg*0.25f;
            c_shadow.fb=c_light.fb*0.25f;
            c_shadow.fa=c_light.fa*0.45f;

            float rel_dist=distance/self->light_source.fr;
            c_shadow.fa*=(1.0f-rel_dist);
            if (c_shadow.fa<=1.0f) c_shadow.fa=1.0f;
            if (c_shadow.fa>=255.0f) c_shadow.fa=255.0f;
            elite_pixel_game_fputpixelRGBA(ente,x,y,c_shadow);

            if (rel_dist<1.0f&&rel_dist>0.0f) {
                //float fmod2=(1.0f-rel_dist)*(1.0f-rel_dist);
                float fmod=(1.0f-rel_dist);
                if (fmod<=0.0f) fmod=0.0f;
                if (fmod>1.0f) fmod=1.0f;
                c_light.fr*=((0.5f+0.5f)*fmod);
                c_light.fg*=((0.5f+0.5f)*fmod);
                c_light.fb*=((0.5f+0.5f)*fmod);
                c_light.fa*=(fmod*fmod);
                //c_shadow.fa*=fmod_a;
                if (self->light_source.illumination_map[x][y]>0){
                    elite_pixel_game_fputpixelRGBA(ente,x,y,c_light);
                };

                /*sfRGBA c_local;
                c_local.fr=8.0f;
                c_local.fg=16.0f;
                c_local.fg=64.0f;*/
            };
        };
    };
    for (int y=0;y<30;y++){
        for (int x=0;x<10;x++){
            if (self->map[x][y]>0) {
                sfRGBA c={192.0f,96.0f,24.0f,255.0};
                elite_pixel_game_fputpixelRGBA(ente,x,y,c);
            }
        }
    }
    if (line_of_sight_demo_on_user_update_leaving_log==false) {
        line_of_sight_demo_on_user_update_leaving_log=true;
        ELOG("INFO : [line_of_sight_demo_on_user_update] leaving line_of_sight_demo_on_user_update() - this notification will only occur once\n");

    };

    return true;
};


/*  int _x=(int)self->light_source.fx;
  int _y=(int)self->light_source.fy;
  draw_line(ente,_x,_y,0,0,green2);
  draw_line(ente,_x,_y,9,0,blue2);
  draw_line(ente,_x,_y,0,_y,cyan2);
  draw_line(ente,_x,_y,9,_y,orange2);
  draw_line(ente,_x,_y,_x,0,yellow2);
  draw_line(ente,_x,_y,_x,29,purple2);
  draw_line(ente,_x,_y,0,29,red2);
  draw_line(ente,_x,_y,9,29,magenta2);*/
/*

  for (int y=0;y<30;y++){
      for (int x=0;x<10;x++){
          if (self->map[x][y]>0) {
              sfRGBA c={42.0f,64.0f,255.0f,255.0};
              elite_pixel_game_fputpixelRGBA(ente,x,y,c);
          }
      }
  }
*/
  //line_of_sight_demo_cast_illumination_map(self,&self->light_source);

/*
  for (int y=0;y<30;y++){
    for (int x=0;x<10;x++){
            if (self->light_source.illumination_map[x][y]>0) {
                sfRGBA c={64.0f,16.0f,16.0f,255.0};
                elite_pixel_game_fputpixelRGBA(ente,x,y,c);
            }
    }
  }
*/
//debug tracing out




bool line_of_sight_demo_on_user_destroy(void* params){

    ELOG("INFO : [line_of_sight_demo_on_user_update] entering line_of_sight_demo_on_user_destroy()\n");

    line_of_sight_demo_t *self=(line_of_sight_demo_t*)params;
    ELOG("INFO : [line_of_sight_demo_on_user_update] deallocating self(line_of_sight_demo)\n");


    if (self!=NULL) {
        free(self);
        ELOG("INFO : [line_of_sight_demo_on_user_update] successfully deallocated self(line_of_sight_demo)\n");

    }else {
        ELOG("ERROR : [line_of_sight_demo_on_user_update] failed to deallocated self(line_of_sight_demo); returning false from line_of_sight_demo_on_user_destroy()\n");

        return false;
    };
    ELOG("INFO : [line_of_sight_demo_on_user_update] returning true from template_on_user_destroy()\n");

  return true;

};


elite_pixel_game_config_t line_of_sight_demo_config={
    .app_name="line_of_sight_demo",
    .screen_width=10,
    .screen_height=30,
    .on_user_construct=(void*)&line_of_sight_demo_construct,
    .on_user_update=&line_of_sight_demo_on_user_update,
    .on_user_destroy=&line_of_sight_demo_on_user_destroy
};

void line_of_sight_demo_start_task(){

    ELOG("INFO : [line_of_sight_demo_start_pixel_game_task] entered line_of_sight_demo_start_pixelapp_task\n");

    ELOG("INFO : [line_of_sight_demo_start_pixel_game_task] creating &pixel_game_task\n");


    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&line_of_sight_demo_config, 5, NULL);
    ELOG("INFO : [line_of_sight_demo_start_pixelapp_task] leaving line_of_sight_demo_start_pixelapp_task\n");


};
