//header only elite_pixel_game_ente tetris example application
#pragma once
#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"
#include "string.h"
#include "elite_tetris_agent.h"
#include "elite_tetris_block.h"
/*
typedef struct {
   char *body_str;
   int x;
   int y;
   uint8_t width;
   uint8_t height;
   uint8_t color_index;
 }block_t;
*/


typedef struct {
  char* app_name;
  float startup_delay;//=3.0f;
   // float tickIntervall=0.0000025f;//speedrun;
  float tick_intervall;
  //  AsyncUDP udp;
  sfRGBA color_map[10];
  uint8_t num_color_maps;
  sfRGBA color_maps[2][10];
  uint8_t color_map_index;//=1;
  uint8_t playfield[10][30];
  elite_tetris_block_t *p_current_block;
  uint8_t last_dice_roll;
  bool game_over;//=false;
  elite_particle_shower_t *particle_shower;
  elite_tetris_agent_t* p_agent;
  char last_block_char;//='_';
  uint16_t lines_cleared_count;//=0;
  uint16_t sum_lines_cleared;//=0;
  uint16_t games_count;//=0;
  float next_tick;//=1.0f;


}elite_tetris_t;


elite_tetris_t* elite_elite_tetris_construct(elite_pixel_game_t* ente);
//bool elite_tetris_rotate_block_l(elite_tetris_block_t* p_block);
//bool elite_tetris_rotate_block_r(elite_tetris_block_t* p_block);
bool elite_tetris_move_block(elite_tetris_t *self,const char key);
void elite_tetris_handle_key(elite_tetris_t *self,const char key);
void elite_tetris_reset_block(elite_tetris_block_t* p_block);
void elite_tetris_draw_playfield(elite_tetris_t *self,elite_pixel_game_t* ente);
void elite_tetris_draw_block(elite_tetris_t *self,elite_pixel_game_t* ente,elite_tetris_block_t* p_block);
bool elite_tetris_check_for_collision(elite_tetris_t *self,elite_tetris_block_t *block);
void elite_tetris_consolidate_block(elite_tetris_t *self,elite_tetris_block_t *p_block);
void elite_tetris_remove_and_consolidate_lines(elite_tetris_t *self);
//bool elite_tetris_handle_command_string(elite_tetris_t *self,const char* cmd);
void elite_tetris_advance(elite_tetris_t *self);
void elite_tetris_reset_game(elite_tetris_t *self);
bool elite_tetris_on_user_update(void* vp_self,elite_pixel_game_t* ente,float fElapsedTime);



void print_playfield_checksum(elite_tetris_t*self){
  char log_str[128]={0};
  (void)log_str;
  int playfield_checksum=0;
  for (int i=0;i<10;i++){
      for (int j=0;j<30;j++){
          playfield_checksum+=self->playfield[i][j];
      };
  };
  //sprintf(log_str,"DEBUG : [elite_tetris_reset_game_state] checksum==%i\n",playfield_checksum);
  //elog(log_str);
  //vTaskDelay(log_delay/portTICK_PERIOD_MS);
};


elite_tetris_t* elite_tetris_construct(elite_pixel_game_t* ente){
    char log_str[128]={0};
    (void)log_str;
    elog("INFO : [elite_tetris_construct] entering elite_tetris_construct()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    elog("INFO : [elite_tetris_construct] allocating self(elite_tetris_t)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    elite_tetris_t *self=malloc(sizeof(elite_tetris_t));

    self->app_name="elite_tetris";
    self->next_tick=0.1f;
    self->startup_delay=1.5f;
    self->tick_intervall=0.00175f;
    self->color_maps[0][0]=black1;
    self->color_maps[0][1]=red1;
    self->color_maps[0][2]=green1;
    self->color_maps[0][3]=blue1;
    self->color_maps[0][4]=yellow1;
    self->color_maps[0][5]=cyan1;
    self->color_maps[0][6]=magenta1;
    self->color_maps[0][7]=orange1;
    self->color_maps[0][8]=purple1;
    self->color_maps[0][9]=white1;
    self->color_maps[1][0]=black2;
    self->color_maps[1][1]=red2;
    self->color_maps[1][2]=green2;
    self->color_maps[1][3]=blue2;
    self->color_maps[1][4]=yellow2;
    self->color_maps[1][5]=cyan2;
    self->color_maps[1][6]=magenta2;
    self->color_maps[1][7]=orange2;
    self->color_maps[1][8]=purple2;
    self->color_maps[1][9]=white2;
    self->lines_cleared_count=0;//=0;
    self->sum_lines_cleared=0;//=0;
    self->games_count=0;//=0;
    self->num_color_maps=2;
    elog("INFO : [elite_tetris_construct] assigned color_maps\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    elog("INFO : [elite_tetris_construct] initialized playfield\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    elite_particle_shower_config_t particle_shower_config={.num_particles=50};
    self->particle_shower=elite_particle_shower_construct(particle_shower_config);
    elog("INFO : [elite_tetris_construct] constructed particle shower\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    for (int y=0;y<30;y++) {
        for (int x=0;x<10;x++) {
            self->playfield[x][y]=0;
        };
    };

    self->game_over=false;
    self->p_agent=elite_tetris_agent_construct();
    elog("INFO : [elite_tetris_construct] constructed agent\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    elite_tetris_agent_set_keystroke_intervall(self->p_agent,self->tick_intervall);


    //for (int i=0;i<10;i++) {
        //self->color_maps[i]=self->color_maps[self->color_map_index][i];
    //};

    //elog("INFO : [elite_tetris_construct] even more color stuff\n");
    //vTaskDelay(log_delay / portTICK_PERIOD_MS);

    //sprintf(log_str,"DEBUG : [elite_tetris_contruct] self->p_current_block=%p \n",(void*)self->p_current_block);
    //elog(log_str);
    //vTaskDelay(log_delay/portTICK_PERIOD_MS);

    self->p_current_block=(elite_tetris_block_t*)malloc(sizeof(elite_tetris_block_t));
    elog("INFO : [elite_tetris_construct] allocating self->current_block\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    sprintf(log_str,"DEBUG : [elite_tetris_contruct] self->p_current_block=%p\n",(void*)self->p_current_block);
    elog(log_str);
    vTaskDelay(log_delay/portTICK_PERIOD_MS);

    sprintf(log_str,"DEBUG : [elite_tetris_contruct] self->p_current_block->body_str=%p\n",(void*)self->p_current_block->body_str);
    elog(log_str);
    vTaskDelay(log_delay/portTICK_PERIOD_MS);

    elog("INFO : [elite_tetris_construct] allocating self->current_block->body_str\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->p_current_block->body_str=(char*)malloc(sizeof(char)*16);

    sprintf(log_str,"DEBUG : [elite_tetris_contruct] self->p_current_block->body_str=%p\n",(void*)self->p_current_block->body_str);
    elog(log_str);
    vTaskDelay(log_delay/portTICK_PERIOD_MS);

    for (int i=0;i<16;i++) self->p_current_block->body_str[i]='_';

    sprintf(log_str,"DEBUG : [elite_tetris_contruct] self->p_current_block->body_str==%s\n",self->p_current_block->body_str);
    elog(log_str);
    vTaskDelay(log_delay/portTICK_PERIOD_MS);

    self->p_current_block->width=4;
    self->p_current_block->height=4;
    self->p_current_block->x=4;
    self->p_current_block->y=0;
    self->p_current_block->block_char=' ';
    self->p_current_block->color_index=0;


    self->lines_cleared_count=0;

  //  uint8_t *p_playfield=&self->playfield[0][0];

  if (self!=NULL) {
    elog("INFO : [elite_tetris_construct] successfully allocated self(elite_tetris_t)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }else {
    elog("ERROR : [elite_tetris_construct] failed to allocated self(elite_tetris_t); returning NULL from constructor\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    return NULL;
  };
  self->app_name="elite_tetris";//unused
  elog("INFO : [elite_tetris_construct] successfully constructed self(elite_tetris_t); returning self from constructor\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  elite_tetris_reset_game(self);
  elog("INFO : [elite_tetris_construct] feeding playfield into agent\n");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

//  print_playfield_checksum(self);
  elite_tetris_agent_read_game_state(self->p_agent,self->playfield,self->p_current_block);
  //elog("INFO : [elite_tetris_construct] calling elite_tetris_agent_run_simulation()\n");
//  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  elite_tetris_agent_run_simulation(self->p_agent);

  return self;
};

/*bool elite_tetris_handle_command_string(elite_tetris_t* self,const char* cmd_str){

    if (strcmp(cmd_str,"swap\n\0")==0) {
        self->color_map_index+=1;
        self->color_map_index%=self->num_color_maps;
        for (int i=0;i<10;i++) {
            self->color_map[i]=self->color_maps[self->color_map_index][i];
        };
        return true;
    };

};
*/


void elite_tetris_reset_block(elite_tetris_block_t* p_block){
  //  char log_str[128]={0};
//cEliteSystem::log("resetting block");
    //elog("DEBUG : [elite_tetris_reset_block] entered\n");
    //vTaskDelay(log_delay / portTICK_PERIOD_MS);

    char* tetromino_chars="OJLTIZS";

    //elog("DEBUG : [elite_tetris_reset_block] initialized tetromino_chars\n");
    //vTaskDelay(log_delay / portTICK_PERIOD_MS);

    //elog("DEBUG : [elite_tetris_reset_block] rolling dice\n");
    //vTaskDelay(log_delay / portTICK_PERIOD_MS);

    int dice=esp_random()%strlen(tetromino_chars);
    char block_char=tetromino_chars[dice];
    //elog("DEBUG : [elite_tetris_reset_block] rolled dice\n");
    //vTaskDelay(log_delay / portTICK_PERIOD_MS);

    //we roll again if we hit the same block_char as before
    if (p_block->block_char==block_char) {
        dice=esp_random()%strlen(tetromino_chars);
        block_char=tetromino_chars[dice];
    };


    //sprintf(log_str,"DEBUG : [elite_tetris_reset_block] picking tetromino_chars[%i]\n",dice);
    //elog(log_str);
    //sprintf(log_str,"DEBUG : [elite_tetris_reset_block] picking tetromino_chars[%i] == '%c' \n",dice,tetromino_chars[dice]);
    //elog(log_str);
    //vTaskDelay(log_delay / portTICK_PERIOD_MS);
    p_block->block_char=block_char;

    //elog("DEBUG : [elite_tetris_reset_block] before switch case\n");
    //vTaskDelay(log_delay / portTICK_PERIOD_MS);

  switch (p_block->block_char) {

    case 'O' : {
        p_block->width=4;
        p_block->height=4;
        p_block->color_index=1;
        char tmp[16]="____"
                     "_11_"
                     "_11_"
                     "____";
        for (int i=0;i<p_block->width*p_block->height;i++){p_block->body_str[i]=tmp[i];};
        break;
    };

    case 'I' : {
        p_block->width=4;
        p_block->height=4;
        p_block->color_index=2;
        char tmp[16]="__2_"
                     "__2_"
                     "__2_"
                     "__2_";
        for (int i=0;i<p_block->width*p_block->height;i++){p_block->body_str[i]=tmp[i];};
        break;
    };

    case 'J' : {
        p_block->width=3;
        p_block->height=3;
        p_block->color_index=3;
        char tmp[9]="_3_"
                    "_3_"
                    "33_";
        for (int i=0;i<p_block->width*p_block->height;i++){p_block->body_str[i]=tmp[i];};
        break;
    };


    case 'L' :{
        p_block->width=3;
        p_block->height=3;
        p_block->color_index=4;
        char tmp[9]="_4_"
                    "_4_"
                    "_44";
        for (int i=0;i<p_block->width*p_block->height;i++){p_block->body_str[i]=tmp[i];};
        break;

    };

    case 'T' : {
        p_block->width=3;
        p_block->height=3;
        p_block->color_index=5;
        char tmp[9]="___"
                    "555"
                    "_5_";
        for (int i=0;i<p_block->width*p_block->height;i++){p_block->body_str[i]=tmp[i];};
        break;

    };

    case 'S' : {
      p_block->width=3;
      p_block->height=3;
      p_block->color_index=6;
      char tmp[9]="_66"
                  "66_"
                  "___";

      for (int i=0;i<p_block->width*p_block->height;i++){p_block->body_str[i]=tmp[i];};
      break;
    };

    case 'Z' : {
      p_block->width=3;
      p_block->height=3;
      p_block->color_index=7;
      char tmp[9]="77_"
                  "_77"
                  "___";
      for (int i=0;i<p_block->width*p_block->height;i++){p_block->body_str[i]=tmp[i];};
      break;
    };

    default : {
      p_block->width=4;
      p_block->height=4;
      p_block->color_index=0;
      char tmp[16]="____"
                   "____"
                   "____"
                   "____";
      for (int i=0;i<p_block->width*p_block->height;i++){p_block->body_str[i]=tmp[i];};
      break;
    };
  };//switch


  p_block->x=4;
  p_block->y=0;
};


/*
  elog("DEBUG : [elite_tetris_reset_block] before check_for_collision case\n");
  vTaskDelay(log_delay/portTICK_PERIOD_MS);
  if (elite_tetris_check_for_collision(self,self->current_block)==true) {

      elite_tetris_consolidate_block(self);
      self->game_over=true;


  elog("DEBUG : [elite_tetris_reset_block] after check_for_collision case\n");
  vTaskDelay(log_delay/portTICK_PERIOD_MS);

  char result_str[128]={0};
  sprintf(result_str,"INFO [elite_tetris_resself_et_block] Game over! Lines cleared : %i",self->lines_cleared_count);
  elog(result_str);
  vTaskDelay(log_delay/portTICK_PERIOD_MS);

  self->sum_lines_cleared+=self->lines_cleared_count;
  float avg=(float)self->sum_lines_cleared/(float)self->games_count;
  sprintf(result_str,"INFO : [elite_tetris_reset_block] Total Lines cleared : %i in %i games played",self->sum_lines_cleared,self->games_count);
  elog(result_str);
  vTaskDelay(log_delay/portTICK_PERIOD_MS);
  sprintf(result_str,"INFO : [elite_tetris_reset_block] Average Lines cleared per game : %f",avg);
  elog(result_str);
  vTaskDelay(log_delay/portTICK_PERIOD_MS);
  self->next_tick=1.0f;
  self->lines_cleared_count=0;
*/

void elite_tetris_draw_playfield(elite_tetris_t *self,elite_pixel_game_t* ente){
    for (int y=0;y<30;y++) {
        for (int x=0;x<10;x++) {
            sfRGBA color;
            if (self->playfield[x][y]>0) {
                color=self->color_maps[1][self->playfield[x][y]];
                elite_pixel_game_fputpixelRGBA(ente,x,y,color);
            };
        };
    };
};


void elite_tetris_draw_block(elite_tetris_t* self,elite_pixel_game_t*ente,elite_tetris_block_t *p_block){
  char log_str[127]={0};
  (void)log_str;
  //sprintf(log_str,"DEBUG : [elite_tetris_draw_block] p_block==%p\n",(void*)p_block);
  //elog(log_str);
  //vTaskDelay(log_delay/portTICK_PERIOD_MS);

    //cEliteSystem::log("drawing block");
    sfRGBA color=self->color_maps[1][p_block->color_index];
    for (int y=0;y<p_block->height;y++) {
        for (int x=0;x<p_block->width;x++) {
            char c=p_block->body_str[p_block->width*y+x];
            //char c='1';
            if (c!='_') elite_pixel_game_fputpixelRGBA(ente,x+p_block->x,y+p_block->y,color);
        };
    };
};

bool elite_tetris_check_for_collision_loop_enter_once_log=false;
bool elite_tetris_check_for_collision_loop_leave_once_log=false;
bool elite_tetris_check_for_collision(elite_tetris_t *self,elite_tetris_block_t *p_block){
  char log_str[128]={0};
  (void)log_str;
  //elog("DEBUG : [elite_tetris_check_for_collision] entering\n");
  //vTaskDelay(log_delay/portTICK_PERIOD_MS);
  if (p_block==NULL) {
      elog("DEBUG : [elite_tetris_check_for_collision] assertion fail(block!=null)\n");
      vTaskDelay(log_delay/portTICK_PERIOD_MS);
  };

  if (p_block->body_str==NULL) {
    //  elog("DEBUG : [elite_tetris_check_for_collision] assertion fail(block->body_str!=null)\n");
  //    vTaskDelay(log_delay/portTICK_PERIOD_MS);
  };

    for (int i=0;i<p_block->height;i++){
        for (int j=0;j<p_block->width;j++){
            int x=j+p_block->x;
            int y=i+p_block->y;
            if (elite_tetris_check_for_collision_loop_enter_once_log==false) {
    //            elog("DEBUG : [elite_tetris_check_for_collision] before char block_char=block->body_str[block->width*i+j]; !once \n");
                //vTaskDelay(log_delay/portTICK_PERIOD_MS);
                elite_tetris_check_for_collision_loop_enter_once_log=true;
                //sprintf(log_str,"DEBUG : [elite_tetris_check_for_collision] [block->width*i+j]==%i\n",(p_block->width*i+j));
                //elog(log_str);
                //vTaskDelay(log_delay/portTICK_PERIOD_MS);

            };

            char block_char=p_block->body_str[p_block->width*i+j];

            if (elite_tetris_check_for_collision_loop_leave_once_log==false) {
            //    sprintf(log_str,"DEBUG : [elite_tetris_check_for_collision] after char block_char=block->body_str[block->width*i+j]; !once \n");
              //  elog(log_str);
                //vTaskDelay(log_delay/portTICK_PERIOD_MS);
                elite_tetris_check_for_collision_loop_leave_once_log=true;
            };
            if (block_char!='_') {
                if (x<0||x>=10||y>=30||y<0) return true;
                if (((x>=0)&&(x<10)&&(y<30)&&(y>=0))&&(self->playfield[x][y]!=0)) return true;
            };
        };
    };
    return false;
};

void elite_tetris_consolidate_block(elite_tetris_t* self,elite_tetris_block_t *p_block){
    for (int i=0;i<p_block->height;i++){
        for (int j=0;j<p_block->width;j++){
            int x=j+p_block->x;
            int y=i+p_block->y;
            if ((x>=0)&&(x<10)&&(y<30)&&(y>=0)) {
                if (p_block->body_str[p_block->width*i+j]!='_') {self->playfield[x][y]=p_block->color_index;};
            };
        };
    };
};

void elite_tetris_remove_and_consolidate_lines(elite_tetris_t *self){
    for (int y=0;y<30;y++) {
        int result=1;
        for (int x=0;x<10;x++) {
            result*=self->playfield[x][y];
        };
        if (result!=0) {
            self->lines_cleared_count+=1;
            for (int y2=y;y2>0;y2--) {
                for (int x=0;x<10;x++) {
                    self->playfield[x][y2]=self->playfield[x][y2-1];
                };
            };
        };
    };
};

void elite_tetris_advance(elite_tetris_t *self){
    //self->p_current_block->y+=1;
    //elog("DEBUG : [elite_tetris_advance ] entered\n");
    //vTaskDelay(log_delay/portTICK_PERIOD_MS);
    self->p_current_block->y+=1;
    //elog("DEBUG : [elite_tetris_advance ] calling elite_tetris_check_for_collision()\n");
    //vTaskDelay(log_delay/portTICK_PERIOD_MS);
    if (elite_tetris_check_for_collision(self,self->p_current_block)) {

        self->p_current_block->y-=1;
        elite_tetris_consolidate_block(self,self->p_current_block);
        elite_tetris_remove_and_consolidate_lines(self);
        elite_tetris_reset_block(self->p_current_block);
        //print_playfield_checksum(self);
        elite_tetris_agent_read_game_state(self->p_agent,self->playfield,self->p_current_block);
        elite_tetris_agent_run_simulation(self->p_agent);
      //  elog("DEBUG : [elite_tetris_advance ] calling elite_tetris_check_for_collision()\n");
      //  vTaskDelay(log_delay/portTICK_PERIOD_MS);

        if (elite_tetris_check_for_collision(self,self->p_current_block)) {
              char result_str[128]={0};
              elite_tetris_consolidate_block(self,self->p_current_block);
              elite_tetris_remove_and_consolidate_lines(self);
              self->game_over=true;
              sprintf(result_str,"INFO [elite_tetris_resself_et_block] Game over! Lines cleared : %i\n",self->lines_cleared_count);
              elog(result_str);
              vTaskDelay(log_delay/portTICK_PERIOD_MS);
              self->sum_lines_cleared+=self->lines_cleared_count;
              float avg=(float)self->sum_lines_cleared/(float)self->games_count;
              sprintf(result_str,"INFO : [elite_tetris_reset_block] Total Lines cleared : %i in %i games played\n",self->sum_lines_cleared,self->games_count);
              elog(result_str);
              vTaskDelay(log_delay/portTICK_PERIOD_MS);
              sprintf(result_str,"INFO : [elite_tetris_reset_block] Average Lines cleared per game : %f\n",avg);
              elog(result_str);
              vTaskDelay(log_delay/portTICK_PERIOD_MS);
              self->next_tick=1.0f;
              self->lines_cleared_count=0;
              return;
        };

    };


};

void elite_tetris_reset_game(elite_tetris_t *self){
  //  elog("INFO : [elite_tetris_construct] initializing playfield[30][10]\n");
  //  vTaskDelay(log_delay / portTICK_PERIOD_MS);

    for (int y=0;y<30;y++) {
          for (int x=0;x<10;x++) {
            self->playfield[x][y]=0;
          };
        };
    //elog("DEBUG : [elite_tetris_construct] elite_tetris_reset_block()\n");
    //vTaskDelay(log_delay / portTICK_PERIOD_MS);

    elite_tetris_reset_block(self->p_current_block);
    self->game_over=false;
    self->games_count++;
    char log_str[128]={0};
    (void)log_str;
    sprintf(log_str,"INFO : [elite_tetris_reset_playfield] Starting new game [%i]\n",self->games_count);
    elog(log_str);
    vTaskDelay(log_delay/portTICK_PERIOD_MS);
//    sprintf(log_str,"DEBUG : [elite_tetris_on_user_update] self->p_current_block==%p\n",(void*)self->p_current_block);
    //elog(log_str);
    //vTaskDelay(log_delay/portTICK_PERIOD_MS);

//    print_playfield_checksum(self);
    elite_tetris_agent_read_game_state(self->p_agent,self->playfield,self->p_current_block);
//    sprintf(log_str,"DEBUG : [elite_tetris_on_user_update] self->p_current_block==%p\n",(void*)self->p_current_block);
//    elog(log_str);
//    vTaskDelay(log_delay/portTICK_PERIOD_MS);

    elite_tetris_agent_run_simulation(self->p_agent);
  //  sprintf(log_str,"DEBUG : [elite_tetris_on_user_update] self->p_current_block==%p\n",(void*)self->p_current_block);
//    elog(log_str);
  //  vTaskDelay(log_delay/portTICK_PERIOD_MS);


  //  elog("DEBUG : [elite_tetris_construct] leaving elite_tetris_reset_game\n");
  //  vTaskDelay(log_delay / portTICK_PERIOD_MS);
};



void elite_tetris_handle_key(elite_tetris_t *self,char key){
  char log_str[128]={0};
  (void)log_str;
  //elog("DEBUG : [elite_tetris_handle_key] entering\n");
//  sprintf(log_str,"DEBUG : [elite_tetris_on_user_update] self->p_current_block==%p\n",(void*)self->p_current_block);
//  elog(log_str);
//  vTaskDelay(log_delay/portTICK_PERIOD_MS);

//  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  //String logStr="TetrisController handling [";
  //logStr+=keyStr;
  //logStr+="]";
  //cEliteSystem::log(logStr);
    if (key=='u') {
      elite_tetris_block_rotate_l(self->p_current_block);
      //elog("DEBUG : [elite_tetris_handle_key] handling key=='u'\n");
      //vTaskDelay(log_delay / portTICK_PERIOD_MS);
      //elite_tetris_block_rotate_l(self->p_current_block);
      if (elite_tetris_check_for_collision(self,self->p_current_block)) elite_tetris_block_rotate_r(self->p_current_block);
    };
    if (key=='d'){
    //    elite_tetris_advance(self);
        self->p_current_block->y+=1;
  //      elog("DEBUG : [elite_tetris_handle_key ] calling elite_tetris_check_for_collision()\n");
        //vTaskDelay(log_delay/portTICK_PERIOD_MS);
        if (elite_tetris_check_for_collision(self,self->p_current_block)){
          self->p_current_block->y-=1;
        //    elite_tetris_consolidate_block(self,self->p_current_block);
            //elite_tetris_reset_block(self->p_current_block);
//            elog("DEBUG : [elite_tetris_handle_key ] calling elite_tetris_check_for_collision()\n");
          //  vTaskDelay(log_delay/portTICK_PERIOD_MS);
            //if (elite_tetris_check_for_collision(self,self->p_current_block)) {

              //  self->game_over=true;
              //  self->next_tick=self->tick_intervall;
            };

        return;
      };
  //  };
    if (key=='l'){
        self->p_current_block->x-=1;
        //elog("DEBUG : [elite_tetris_handle_key] handling key=='l'\n");
        //vTaskDelay(log_delay / portTICK_PERIOD_MS);
        if (elite_tetris_check_for_collision(self,self->p_current_block)) self->p_current_block->x+=1;
    };

    if (key=='r'){
      //elog("DEBUG : [elite_tetris_handle_key] handling key=='r'\n");
      //vTaskDelay(log_delay / portTICK_PERIOD_MS);
        self->p_current_block->x+=1;
    //    char c=self->p_current_block->body_str[0];
    //    elog("DEBUG : [elite_tetris_handle_key ] calling elite_tetris_check_for_collision()\n");
    //    vTaskDelay(log_delay/portTICK_PERIOD_MS);
        if (elite_tetris_check_for_collision(self,self->p_current_block)) self->p_current_block->x-=1;
    };

    //elog("DEBUG : [elite_tetris_handle_key] handle_key leaving\n");
    //vTaskDelay(log_delay / portTICK_PERIOD_MS);
};

bool elite_tetris_on_user_update_entered_log=false;
bool elite_tetris_on_user_update_leaving_log=false;
bool elite_tetris_on_user_update_pre_particle_shower_update_log=false;

bool elite_tetris_on_user_update(void* vp_self,elite_pixel_game_t* ente,float fElapsedTime){
  char log_str[127]={0};
  (void)log_str;
  (void)log_str;
  //debug tracing in
      if (elite_tetris_on_user_update_entered_log==false) {
          elite_tetris_on_user_update_entered_log=true;
      //    elog("INFO : [elite_tetris_update] entered elite_tetris_update() - this notification will only occur once\n");
      //    vTaskDelay(log_delay / portTICK_PERIOD_MS);
      };
  //body

      elite_tetris_t *self=(elite_tetris_t*)vp_self;
      elite_pixel_game_set_target_layer(ente,0);
      sfRGBA c={0.0f,0.0f,0.0f,255.0f};
      elite_pixel_game_fill_flayerRGBA(ente,c);

    if (self->startup_delay>=0.0f) {self->startup_delay-=fElapsedTime;};
    if (self->startup_delay<=0.0f) {
          self->next_tick-=fElapsedTime;
          if (self->next_tick<=0.0f) {
              if (self->game_over==true) {
                elite_tetris_reset_game(self);
              }
              else {
                self->next_tick=self->tick_intervall;
                elite_tetris_advance(self);
              };
          };

        //  sprintf(log_str,"DEBUG : [elite_tetris_on_user_update] self->p_current_block==%p\n",(void*)self->p_current_block);
        //  elog(log_str);
        //  vTaskDelay(log_delay/portTICK_PERIOD_MS);

          char key=elite_tetris_agent_get_key(self->p_agent,fElapsedTime,self->p_current_block);

          elite_tetris_handle_key(self,key);

        //  elog("DEBUG : [elite_tetris_on_user_update] returned from elite_tetris_handle_key(self,key);\n");
      //    sprintf(log_str,"DEBUG : [elite_tetris_on_user_update] self->p_current_block==%p\n",(void*)self->p_current_block);
      //    elog(log_str);
    //      vTaskDelay(log_delay/portTICK_PERIOD_MS);

      //    vTaskDelay(log_delay);
    };

  //cEliteSystem::log("clearing canvas");
    //elog("DEBUG : [elite_tetris_on_user_update] left block if (self->startup_delay<=0.0f){};\n");
    //vTaskDelay(log_delay);

    //elog("DEBUG : [elite_tetris_on_user_update] elite_particle_shower_update();\n");
    //vTaskDelay(log_delay);
    elite_particle_shower_update(self->particle_shower,fElapsedTime);
    sfRGBA bg_col={0.0f,0.0f,0.0f,0.0f};
    elite_pixel_game_fill_flayerRGBA(ente,bg_col);
    //elog("DEBUG : [elite_tetris_on_user_update] elite_particle_shower_draw();\n");
    //vTaskDelay(log_delay);

    elite_particle_shower_draw(ente,self->particle_shower);
    if (self->startup_delay<=0.0f) {
    //    elog("DEBUG : [elite_tetris_on_user_update] elite_tetris_draw_playfield();\n");
  //      vTaskDelay(log_delay);
        elite_tetris_draw_playfield(self,ente);
        if (self->p_current_block){
      //  elog("DEBUG : [elite_tetris_on_user_update] elite_tetris_draw_block());\n");
//        vTaskDelay(log_delay);
        //sprintf(log_str,"DEBUG : [elite_tetris_on_user_update] self->p_current_block==%p\n",(void*)self->p_current_block);
        //elog(log_str);
        //vTaskDelay(log_delay/portTICK_PERIOD_MS);
        elite_tetris_draw_block(self,ente,self->p_current_block);
        //sprintf(log_str,"DEBUG : [elite_tetris_on_user_update] self->p_current_block==%p\n",(void*)self->p_current_block);
        //elog(log_str);
      //  vTaskDelay(log_delay/portTICK_PERIOD_MS);

      };
    };

    //debug tracing out
    if (elite_tetris_on_user_update_leaving_log==false) {
        elite_tetris_on_user_update_leaving_log=true;
    //    elog("INFO : [elite_tetris_on_user_update] leaving elite_tetris_on_user_update() - this notification will only occur once\n");
    //    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

    return true;
};


bool elite_tetris_on_user_destroy(void* params){

    elog("INFO : [elite_tetris_on_user_destroy] entering elite_tetris_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elite_tetris_t *self=(elite_tetris_t*)params;
    elog("INFO : [elite_tetris_on_user_destroy] deallocating self(elite_tetris)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    if (self!=NULL) {
        elite_particle_shower_destruct(self->particle_shower);
        free(self);
        elog("INFO : [elite_tetris_on_user_destroy] successfully deallocated self(elite_tetris)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else {
        elog("ERROR : [elite_tetris_on_user_destroy] failed to deallocated self(elite_tetris); returning false from elite_tetris_on_user_destroy()\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        return false;
    };
    elog("INFO : [elite_tetris_on_user_update] returning true from template_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  return true;
};

void elite_tetris_start_task(){

    elog("INFO : [main_start_pixel_game_task] entered main_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elog("INFO : [main_start_pixel_game_task] creating &pixel_game_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elite_pixel_game_config_t pixel_game_config={
        .app_name="elite_tetris",
        .screen_width=10,
        .screen_height=30,
        .on_user_construct=(void*)&elite_tetris_construct,
        .on_user_update=&elite_tetris_on_user_update,
        .on_user_destroy=&elite_tetris_on_user_destroy
    };
    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 8192,&pixel_game_config, 5, NULL);
    elog("INFO : [tetris_start_pixelapp_task] leaving tetris_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

};