#include "elite_pixel_game_ente.h"
#include "elite_particle.h"
#include "elite.h"
//#include "elite_tetris_agent.h"
#include "main.h"


#pragma once
#define MAX_BULLETS 20
#define NUM_FUSED_PARTICLES 420
#define NUM_SPACE_BLOCKS 6
#define WAVE_SPEED 10
#define GUN_FUSE_100TH 20
#define BULLET_SPEED 35
#define FORMATION_TIGHTNESS 35


//I put DIV_DEFS_BEGIN and DIV_DEFS_END for validity check simplification.the whole scripting concept is totally bonkers. so what?

#define CMD_DEFS_BEGIN          10
#define CMD_SPAWN_MULTI         11
#define CMD_WAIT_N_DELTA_T      12
#define CMD_SET_PARAM_SHAPE     13
#define CMD_SET_PARAM_SIZE      14
#define CMD_SET_PARAM_HEALTH    15
#define CMD_SET_PARAM_PATH_ID   16
#define CMD_WAIT_KILL           17
#define CMD_INSERT_COIN         18
#define CMD_SET_PARAM_DELTA_T   19
#define CMD_GOTO                20
#define CMD_HALT                21
#define CMD_DEFS_END            22

#define PATH_DEFS_BEGIN         40
#define PATH_SIN_L              41
#define PATH_SIN_R              42
#define PATH_LIN_L              43
#define PATH_LIN_R              44
#define PATH_LIN_C              45
#define PATH_LIN_S              46
#define PATH_DEFS_END           47

#define SHAPE_DEFS_BEGIN        60
#define SHAPE_L                 61
#define SHAPE_J                 62
#define SHAPE_I                 63
#define SHAPE_O                 64
#define SHAPE_Z                 65
#define SHAPE_S                 66
#define SHAPE_T                 67
#define SHAPE_DEFS_END          68

#define SIZE_DEFS_BEGIN         80
#define SIZE_S                  81
#define SIZE_M                  82
#define SIZE_L                  83
#define SIZE_XL                 84
#define SIZE_XXL                85
#define SIZE_DEFS_END           86

typedef struct{
  int cmd;
  int arg;
}bogo_script_action_t;

bogo_script_action_t level0[]={
  {CMD_SET_PARAM_DELTA_T,3},
  {CMD_SET_PARAM_SIZE,SIZE_S},
  {CMD_SET_PARAM_HEALTH,1},
  {CMD_SET_PARAM_SHAPE,SHAPE_L},
  {CMD_SET_PARAM_PATH_ID,PATH_SIN_L},
  {CMD_SPAWN_MULTI,5},
  {CMD_WAIT_N_DELTA_T,3},
  {CMD_SET_PARAM_SHAPE,SHAPE_Z},
  {CMD_SET_PARAM_PATH_ID,PATH_SIN_R},
  {CMD_SPAWN_MULTI,5},
  {CMD_WAIT_N_DELTA_T,3},
  {CMD_SET_PARAM_SHAPE,SHAPE_I},
  {CMD_SET_PARAM_PATH_ID,PATH_SIN_L},
  {CMD_SPAWN_MULTI,5},
  {CMD_WAIT_N_DELTA_T,3},
  {CMD_SET_PARAM_SHAPE,SHAPE_Z},
  {CMD_SET_PARAM_PATH_ID,PATH_SIN_R},
  {CMD_SPAWN_MULTI,5},
  {CMD_WAIT_N_DELTA_T,3},
  {CMD_SET_PARAM_SHAPE,SHAPE_S},
  {CMD_SET_PARAM_PATH_ID,PATH_SIN_L},
  {CMD_SPAWN_MULTI,5},
  {CMD_WAIT_N_DELTA_T,3},
  {CMD_SET_PARAM_SHAPE,SHAPE_T},
  {CMD_SET_PARAM_PATH_ID,PATH_SIN_R},
  {CMD_SPAWN_MULTI,5},
  {CMD_WAIT_N_DELTA_T,3},
  {CMD_SET_PARAM_SHAPE,SHAPE_O},
  {CMD_SET_PARAM_PATH_ID,PATH_SIN_L},
  {CMD_SPAWN_MULTI,5},
  {CMD_WAIT_N_DELTA_T,3},
  {CMD_HALT,1}
};

typedef struct s_tetr1s_reloaded tetr1s_reloaded_t;

typedef struct{
  float delta_t;
  int shape_id;
  int health;
  int size_id;
  int path_id;
}spawn_params_t;

typedef struct{
  float timer;
  int index_current_action;
  spawn_params_t params;
  bogo_script_action_t *p_script;
  bogo_script_action_t current_cmd;
  tetr1s_reloaded_t *p_game;
}bogo_script_processor_t;

bogo_script_processor_t* bogo_script_processor_construct(tetr1s_reloaded_t* game,bogo_script_action_t* p_script){
  bogo_script_processor_t *self=malloc(sizeof(bogo_script_processor_t));
  self->p_game=game;
  self->p_script=p_script;
  self->timer=0.0f;
  self->params.delta_t=0.25f;
  self->params.shape_id=SHAPE_L;
  self->params.size_id=SIZE_S;
  self->params.health=0;
  self->params.path_id=PATH_SIN_L;
  self->current_cmd.cmd=0;
  self->current_cmd.arg=0;
  self->index_current_action=-1;//Todo: get rid of indexing. do shifting or ptr++ or smth
  return self;
};
void tetr1s_reloaded_spawn(tetr1s_reloaded_t* p_game,spawn_params_t params);

void bogo_script_processor_update(bogo_script_processor_t* self,float fElapsedTime){
  if (self->timer>=0.0f) {
      self->timer-=fElapsedTime;
      return;
  };
  //todo: return if current_commant==WAIT_KILL

  if ((self->current_cmd.cmd==0)&&(self->current_cmd.arg==0)) {
    self->index_current_action+=1;
    self->current_cmd.cmd=self->p_script[self->index_current_action].cmd;
    self->current_cmd.arg=self->p_script[self->index_current_action].arg;
    //fetch_next();
  };

  switch (self->current_cmd.cmd) {
    case CMD_SET_PARAM_SHAPE :{
        ELOG("DEBUG : [%s] SET PARAM SHAPE\n",__FUNCTION__);
        self->params.shape_id=self->current_cmd.arg;
        self->current_cmd.cmd=0;
        self->current_cmd.arg=0;
        break;
    };
    case CMD_SET_PARAM_DELTA_T :{
        ELOG("DEBUG : [%s] SET PARAM DELTA_T\n",__FUNCTION__);
        self->params.delta_t=0.1f*self->current_cmd.arg;
        self->current_cmd.cmd=0;
        self->current_cmd.arg=0;
        break;
    };
    case CMD_SET_PARAM_PATH_ID :{
        ELOG("DEBUG : [%s] SET PARAM PATH_ID\n",__FUNCTION__);
        self->params.path_id=self->current_cmd.arg;
        self->current_cmd.cmd=0;
        self->current_cmd.arg=0;
        break;
    };
    case CMD_SET_PARAM_HEALTH :{
        ELOG("DEBUG : [%s] SET PARAM HEALTH\n",__FUNCTION__);
        self->params.health=self->current_cmd.arg;
        self->current_cmd.cmd=0;
        self->current_cmd.arg=0;
        break;
    };
    case CMD_WAIT_N_DELTA_T :{
        ELOG("DEBUG : [%s] CMD WAIT_N_DELTA_T\n",__FUNCTION__);
        self->timer=self->params.delta_t*self->current_cmd.arg;
        self->current_cmd.cmd=0;
        self->current_cmd.arg=0;
        break;
    };
    case CMD_SET_PARAM_SIZE :{
        ELOG("DEBUG : [%s] CMD CMD_SET_PARAM_SIZE\n",__FUNCTION__);
        self->params.size_id=self->current_cmd.arg;
        self->current_cmd.cmd=0;
        self->current_cmd.arg=0;
        break;
    };
    case CMD_SPAWN_MULTI : {
        ELOG("DEBUG : [%s] CMD SPAWN_MULTI\n",__FUNCTION__);
        self->timer=self->params.delta_t;
        tetr1s_reloaded_spawn(self->p_game,self->params);
        self->current_cmd.arg-=1;
        if (self->current_cmd.arg==0) {self->current_cmd.cmd=0;};
        break;
    };
    case CMD_GOTO : {
        ELOG("DEBUG : [%s] CMD GOTO\n",__FUNCTION__);
        self->index_current_action=self->current_cmd.arg-1;
        self->current_cmd.cmd=0;
        self->current_cmd.arg=0;
        break;
    };
    case CMD_HALT : {
        if (self->current_cmd.arg>=0) {
            self->current_cmd.arg=-1;
            ELOG("DEBUG : [%s] bogo_script processor CMD HALT\n",__FUNCTION__);
        };
        break;
    };
    default : {
        ELOG("ERROR : [%s] invalid bogo_script enty\n",__FUNCTION__);
        self->current_cmd.cmd=CMD_HALT;
        self->current_cmd.arg=1;
    };
  };

};

typedef elite_particle_t bullet_t;

sfRGBA tetr1s_colors[10]={black2,red2,green2,blue2,yellow2,cyan2,magenta2,orange2,purple2,white2};
sfRGBA bullet_col={192.0f,182.0f,152.0f,192.0f};

static float phase_offset=0.4146f;
static float drift=2.5f;
static float scale=5.2f;

//drift=2.500000 phase_offset=0.414603 scale=5.200000
//static const char *valid_chars="ojltisJojltizSoltizsTojlizsLjltizsZojltisOjltizsI";

static float init_fuse=3.5f;

typedef struct{
  float fx;
  float fy;
  float fvx;
  float fvy;
  float fuse;
  sfRGBA col;
}fused_particle_t;


typedef struct {
  float fx;
  float fy;
  float fvx;
  float fvy;
}spaceship_t;


typedef struct {
   char body_str[MAX_SPACE_BLOCK_BODY_STR_LEN];
   int x;
   int y;
   uint8_t width;
   uint8_t height;
   uint8_t color_index;
   int path_id;
   int size_id;
   int shape_id;
   int health;

   float t_value;
 }space_block_t;

struct s_tetr1s_reloaded{
  char* app_name;
  elite_particle_shower_t* my_particle_shower;
  spaceship_t ship;
  elite_particle_t bullets[MAX_BULLETS];
  float gun_fuse;
  float gun_intervall;
  int gun_bullets_count;
  bogo_script_processor_t* p_bs_processor;
  space_block_t space_blocks[NUM_SPACE_BLOCKS];
  fused_particle_t fused_particles[NUM_FUSED_PARTICLES];
  int wave_char;
};

void tetr1s_reloaded_space_block_create(space_block_t *p_sb,int shape_id,int size_id,int path_id);
void tetr1s_reloaded_spawn(tetr1s_reloaded_t* game,spawn_params_t params){
  for (int i=0;i<NUM_SPACE_BLOCKS;i++) {
    if (game->space_blocks[i].health==0) {
      tetr1s_reloaded_space_block_create(&game->space_blocks[i],params.shape_id,params.size_id,params.path_id);
      game->space_blocks[i].t_value=-0.1f;
      game->space_blocks[i].health=params.health;
      return;
    };
  };
  ELOG("ERROR : [%s] NUM SPACEBLOCKS exceeded - skipped spawn\n",__FUNCTION__);
};

bool tetr1s_reloaded_on_user_update_entered_log=false;
bool tetr1s_reloaded_on_user_update_leaving_log=false;
bool tetr1s_reloaded_on_user_update_pre_particle_shower_update_log=false;

int apprx(float f){
  return (int)f;
  int i=(int)(f*100.0f);
  int i2=i%100;
  if (i2<-50) i-=100;
  if (i2>50) i+=100;
  i/=100;
  return i;
}

bool is_visible(int x,int y){
  if (x>=0&&x<10&&y>=0&&y<30) return true;
  return false;
}
void draw_path0(elite_pixel_game_t *ente){
float t=0.0f;
  while (t<3.0f) {
    float offset_x=2.0f;
    float fx=offset_x+(sinf(t*3.14159f-phase_offset)*scale);
    float fy=t*10.0f;
    //int x=(int)fx+t*drift;
    int x=apprx(fx+t*drift);

    x=(9-x);
    int y=apprx(fy);
    sfRGBA c={42.0f,69.0f,123.45f,11.22f};
    t+=0.01f;
    if (is_visible(x,y)) elite_pixel_game_fputpixelRGBA(ente,x,y,c);
  };
};

void draw_path1(elite_pixel_game_t *ente){
float t=0.0f;
  while (t<3.0f) {
    float offset_x=2.0f;
    float fx=offset_x+(sinf(t*3.14159f-phase_offset)*scale);
    float fy=t*10.0f;
    int x=apprx(fx+t*drift);
    int y=apprx(fy);
    sfRGBA c={123.0f,69.0f,42.42f,11.22f};
    t+=0.01f;
    if (is_visible(x,y)) elite_pixel_game_fputpixelRGBA(ente,x,y,c);
  };
};


void update_position(space_block_t* sb){

    switch (sb->path_id) {

        case PATH_LIN_C :

        case PATH_LIN_S :

        case PATH_LIN_R :

        case PATH_LIN_L : {

                float fy=sb->t_value*3.5f;
                if (fy>=11.0f) fy=11.0f;
                float fx=5.0f;
                sb->y=(int)fy;
                sb->x=(int)fx;
                return;
            };
        case PATH_SIN_L : {

                float offset_x=2.0f;
                float fx=offset_x+(sinf(sb->t_value*3.14159f-phase_offset)*scale);
                float fy=sb->t_value*10.0f;
                int x=(int)fx+sb->t_value*drift;
                int y=(int)fy;
                sb->x=x;
                sb->y=y;
                return;
        }
        case PATH_SIN_R : {

                float offset_x=2.0f;
                float fx=offset_x+(sinf(sb->t_value*3.14159f-phase_offset)*scale);
                float fy=sb->t_value*10.0f;
                int x=(int)fx+sb->t_value*drift;
                x=(9-x);
                int y=(int)fy;
                sb->x=x;
                sb->y=y;
                return;
        }
        default : {
                ELOG("ERROR : [%s] UNREACHABLE invalid path id\n",__FUNCTION__);
        };
    };
};



sfRGBA col_debug={42.0f,21.0f,8.0f,32.0f};
sfRGBA col_debug2={8.0f,42.0f,21.0f,32.0f};
sfRGBA col_none={0.0f,0.0f,0.0f,0.0f};

void tetr1s_reloaded_space_blocks_draw(tetr1s_reloaded_t* self,elite_pixel_game_t* ente){

    for (int i=0;i<NUM_SPACE_BLOCKS;i++) {
        if (self->space_blocks[i].health==0) continue;
        int x=self->space_blocks[i].x;
        int y=self->space_blocks[i].y;

        x=x-(self->space_blocks[i].width/2);
        y=y-(self->space_blocks[i].height/2);
        int idx=-1;
        for (int j=0;j<self->space_blocks[i].height;j++) {
            for (int k=0;k<self->space_blocks[i].width;k++) {
                idx+=1;
                int xx=x+k;
                int yy=y+j;
                sfRGBA cc=tetr1s_colors[self->space_blocks[i].color_index];
                if (is_visible(xx,yy)) {
                    if (self->space_blocks[i].body_str[idx]=='.') {
                        cc=col_none;
                    };
                    if (self->space_blocks[i].body_str[idx]=='X') {
                          cc.fr/=2;
                          cc.fg/=2;
                          cc.fb/=2;
                    };
                elite_pixel_game_fputpixelRGBA(ente,xx,yy,cc);
                };
            };
        };
    };
};


//this is so fucking bad.... sweep and prune please. and calc the idx to test agains. wtf
bool tetr1s_reloaded_space_blocks_test_bullet_collision(space_block_t* s,bullet_t* b,elite_pixel_game_t* ente){
    int x=s->x;
    int y=s->y;

    x=x-(s->width/2);
    y=y-(s->height/2);
    int idx=-1;
    for (int j=0;j<s->height;j++) {
        for (int k=0;k<s->width;k++) {
            idx+=1;
            int xx=x+k;
            int yy=y+j;
            if (s->body_str[idx]=='.') {
              //elite_pixel_game_fputpixelRGBA(ente,xx,yy,col_debug);
              continue;
            };
            int bx=(int)(b->fx);
            int by=(int)(b->fy);
          //  elite_pixel_game_fputpixelRGBA(ente,bx,by,col_debug2);
            if (is_visible(xx,yy)&&is_visible(bx,by)&&(bx==xx)&&(by==yy)) return true;
        };
    };
  return false;
};


bool tetr1s_reloaded_bullet_is_gone(bullet_t *p_b){
  if (p_b->fx<0.0f||p_b->fx>=10.0f||p_b->fy<0.0f||p_b->fy>=30.0f) return true;
  return false;
};

void tetr1s_reloaded_bullets_clear(tetr1s_reloaded_t *self){
  for (int i=0;i<MAX_BULLETS;i++) {
    self->bullets[i].fx=-69.0f;
    self->bullets[i].fy=-69.0f;
  };
};

void tetr1s_reloaded_bullets_draw(tetr1s_reloaded_t* self,elite_pixel_game_t *ente){
  for (int i=0;i<MAX_BULLETS;i++){
    if (tetr1s_reloaded_bullet_is_gone(&self->bullets[i])) continue;
    int x=self->bullets[i].fx;
    int y=self->bullets[i].fy;
    sfRGBA c=self->bullets[i].col;
    elite_pixel_game_fputpixelRGBA(ente,x,y,c);
  };
};



void tetr1s_reloaded_bullets_update(tetr1s_reloaded_t* self,float fElapsedTime){
  for (int i=0;i<MAX_BULLETS;i++){
    if (tetr1s_reloaded_bullet_is_gone(&self->bullets[i])) continue;
    self->bullets[i].fx+=self->bullets[i].fvx*fElapsedTime;
    self->bullets[i].fy+=self->bullets[i].fvy*fElapsedTime;
  };
  return;
};



/*

void tetr1s_reloaded_space_blocks_generate_wave(tetr1s_reloaded_t*self,char c_type,int sub_type,int path_type,float fdistance){

    //the path_id of slot 0 may not be in line with the other slots if it was used for final boss(path_id=69). copy/paste ftw.
    self->space_blocks[0].path_id=self->space_blocks[1].path_id;

  //  for (int i=0;i<NUM_SPACE_BLOCKS;i++) {
  //    self->space_blocks_t_values[i]=-0.2f-(fdistance*i);
  //    self->space_blocks[i].health=0;
  //  };
    self->gun_fuse+=0.5f;
    for (int i=0;i<NUM_SPACE_BLOCKS;i++) {
      tetr1s_reloaded_space_block_create(&self->space_blocks[i],c_type);
      self->space_blocks_t_values[i]=-0.1f-(fdistance*i);
      if (c_type>='a') {//lower case means "generic enemy". "man ascii" if your wondering
          self->space_blocks[i].path_id+=1;
          self->space_blocks[i].path_id%=2;
          self->space_blocks[i].health=1;
      }else{//uppercase means its time for a final boss
          self->space_blocks_t_values[i]=-4.0f-(fdistance*i);
          self->space_blocks[i].health=23;
          self->space_blocks[i].path_id=69;
          self->gun_fuse=5.0f;

          break;//theres only a single final boss needed. so we stop right here.
    };
  };
};
*/

void tetr1s_reloaded_space_blocks_update_wave(tetr1s_reloaded_t*self,float fElapsedTime){
  for (int i=0;i<NUM_SPACE_BLOCKS;i++) {
    if (self->space_blocks[i].health==0) continue;
    self->space_blocks[i].t_value+=(fElapsedTime*0.1f*WAVE_SPEED);

  //  ELOG("DEBUG : [%s] updating block %i\n",__FUNCTION__,i);
    update_position(&self->space_blocks[i]);
    if (self->space_blocks[i].y>=32.0f) {self->space_blocks[i].health=0;};
    //if (self->space_blocks_t_values[i]>3.0f) {self->space_blocks[i].health=0;};


  };
};

void tetr1s_reloaded_bullets_add(tetr1s_reloaded_t* self,bullet_t b){
  for (int i=0;i<MAX_BULLETS;i++){
    if (tetr1s_reloaded_bullet_is_gone(&self->bullets[i])) {
        self->bullets[i].fx=b.fx;
        self->bullets[i].fy=b.fy;
        self->bullets[i].fvx=b.fvx;
        self->bullets[i].fvy=b.fvy;
        self->bullets[i].col=b.col;
        return;
    };
  };
  ELOG("DEBUG : [%s] %s\b",__FUNCTION__,"out of bullets");
  return;
};

void tetr1s_reloaded_fused_particles_update(tetr1s_reloaded_t* self,float fElapsedTime){
  for (int i=0;i<NUM_FUSED_PARTICLES;i++) {
    if (self->fused_particles[i].fuse>0.0f) {
      self->fused_particles[i].fuse-=fElapsedTime;
      self->fused_particles[i].fx+=self->fused_particles[i].fvx*fElapsedTime;
      self->fused_particles[i].fy+=self->fused_particles[i].fvy*fElapsedTime;
      if (self->fused_particles[i].fx<0.0f||self->fused_particles[i].fx>=10.0f||self->fused_particles[i].fy<0.0f||self->fused_particles[i].fy>=30.0f) {
        self->fused_particles[i].fuse=0.0f;
      };

    }
  };
};

bool tetr1s_reloaded_all_spaceblock_gone(tetr1s_reloaded_t* self){
  for (int i=0;i<NUM_SPACE_BLOCKS;i++) {
    if (self->space_blocks[i].health>0) return false;
  };
  return true;
};

void tetr1s_reloaded_fused_particles_draw(tetr1s_reloaded_t* self,elite_pixel_game_t *ente){
  for (int i=0;i<NUM_FUSED_PARTICLES;i++) {
    if (self->fused_particles[i].fuse>0.0f) {
      int x=(int)self->fused_particles[i].fx;
      int y=(int)self->fused_particles[i].fy;
      sfRGBA c=self->fused_particles[i].col;
      elite_pixel_game_fputpixelRGBA(ente,x,y,c);
    };
  };
};

void tetr1s_reloaded_fused_particles_add(tetr1s_reloaded_t* self,fused_particle_t *p){
  for (int i=0;i<NUM_FUSED_PARTICLES;i++) {
    if (self->fused_particles[i].fuse<=0.0f) {
      self->fused_particles[i].fx=p->fx;
      self->fused_particles[i].fy=p->fy;
      self->fused_particles[i].fvx=p->fvx;
      self->fused_particles[i].fvy=p->fvy;
      self->fused_particles[i].fuse=p->fuse;
      self->fused_particles[i].col=p->col;
      return;
    };
  };
  ELOG("DEBUG : [%s] out of fused_particles\n",__FUNCTION__);
};

void tetr1s_reloaded_put_explosion(tetr1s_reloaded_t*self,float fx,float fy){
  sfRGBA c_boom={224.0f,192.0f,14.0f,172.0f};

  float v_boom=15.0f;
  float t_boom=0.25f;
  fused_particle_t p1={.fx=fx,.fy=fy,.fvx=-1.0f*v_boom,.fvy=+1.0f*v_boom,.col=c_boom,.fuse=t_boom};
  fused_particle_t p2={.fx=fx,.fy=fy,.fvx=-1.0f*v_boom,.fvy=+0.0f*v_boom,.col=c_boom,.fuse=t_boom};
  fused_particle_t p3={.fx=fx,.fy=fy,.fvx=-1.0f*v_boom,.fvy=-1.0f*v_boom,.col=c_boom,.fuse=t_boom};
  fused_particle_t p4={.fx=fx,.fy=fy,.fvx=+1.0f*v_boom,.fvy=+1.0f*v_boom,.col=c_boom,.fuse=t_boom};
  fused_particle_t p5={.fx=fx,.fy=fy,.fvx=+1.0f*v_boom,.fvy=+0.0f*v_boom,.col=c_boom,.fuse=t_boom};
  fused_particle_t p6={.fx=fx,.fy=fy,.fvx=+1.0f*v_boom,.fvy=-1.0f*v_boom,.col=c_boom,.fuse=t_boom};
  fused_particle_t p7={.fx=fx,.fy=fy,.fvx=+0.0f*v_boom,.fvy=+1.0f*v_boom,.col=c_boom,.fuse=t_boom};
  fused_particle_t p8={.fx=fx,.fy=fy,.fvx=+0.0f*v_boom,.fvy=-1.0f*v_boom,.col=c_boom,.fuse=t_boom};
  tetr1s_reloaded_fused_particles_add(self,&p1);
  tetr1s_reloaded_fused_particles_add(self,&p2);
  tetr1s_reloaded_fused_particles_add(self,&p3);
  tetr1s_reloaded_fused_particles_add(self,&p4);
  tetr1s_reloaded_fused_particles_add(self,&p5);
  tetr1s_reloaded_fused_particles_add(self,&p6);
  tetr1s_reloaded_fused_particles_add(self,&p7);
  tetr1s_reloaded_fused_particles_add(self,&p8);
}


void tetr1s_reloaded_block_explode(tetr1s_reloaded_t*self,space_block_t* sb){
//  sfRGBA c_boom={224.0f,192.0f,14.0f,172.0f};


  int x=sb->x;
  int y=sb->y;
//  x=x-(sb->width/2);
  //y=y-(sb->height/2);
  float fx=(float)x;
  float fy=(float)y;
  float fvx;
  float fvy;
  for (int i=0;i<36;i++) {
    float angle_deg=(float)(i*10);
    float angle_rad=0.0174533f*angle_deg;
    fvx=cos(angle_rad);
    fvy=sin(angle_rad);
    fvx*=15.0f;
    fvy*=15.0f;
    sfRGBA c_boom={224.0f,192.0f,14.0f,172.0f};
    float t_boom=1.0f;
    fused_particle_t p1={.fx=fx,.fy=fy,.fvx=fvx,.fvy=fvy,.col=c_boom,.fuse=t_boom};
  //  fused_particle_t p2={.fx=fx,.fy=fy,.fvx=1.25f*fvx,.fvy=1.25f*fvy,.col=c_boom,.fuse=t_boom};
    fused_particle_t p3={.fx=fx,.fy=fy,.fvx=1.5f*fvx,.fvy=1.5f*fvy,.col=c_boom,.fuse=t_boom};
    tetr1s_reloaded_fused_particles_add(self,&p1);
  //  tetr1s_reloaded_fused_particles_add(self,&p2);
    tetr1s_reloaded_fused_particles_add(self,&p3);

  };

  /*int idx=-1;
  for (int j=0;j<sb->height;j++) {
      for (int k=0;k<sb->width;k++) {
                  idx+=1;
                  int xx=x+k;
                  int yy=y+j;
                  if (is_visible(xx,yy)&&sb->body_str[idx]!='.') {
                    float fx=(float)xx;
                    float fy=(float)yy;
                    float v_boom;
                    float t_boom;
                    t_boom=0.3f+(0.01f*((float)(esp_random()%35)));
                    v_boom=15.0f+((float)(esp_random()%15));
                    fused_particle_t p1={.fx=fx,.fy=fy,.fvx=-1.0f*v_boom,.fvy=+1.0f*v_boom,.col=c_boom,.fuse=t_boom};
                    v_boom=15.0f+((float)(esp_random()%15));
                    t_boom=0.3f+(0.01f*((float)(esp_random()%35)));
                    fused_particle_t p2={.fx=fx,.fy=fy,.fvx=-1.0f*v_boom,.fvy=+0.0f*v_boom,.col=c_boom,.fuse=t_boom};
                    v_boom=15.0f+((float)(esp_random()%15));
                    t_boom=0.3f+(0.01f*((float)(esp_random()%35)));
                    fused_particle_t p3={.fx=fx,.fy=fy,.fvx=-1.0f*v_boom,.fvy=-1.0f*v_boom,.col=c_boom,.fuse=t_boom};
                    v_boom=15.0f+((float)(esp_random()%15));
                    t_boom=0.3f+(0.01f*((float)(esp_random()%35)));
                    fused_particle_t p4={.fx=fx,.fy=fy,.fvx=+1.0f*v_boom,.fvy=+1.0f*v_boom,.col=c_boom,.fuse=t_boom};
                    v_boom=15.0f+((float)(esp_random()%15));
                    t_boom=0.3f+(0.01f*((float)(esp_random()%35)));
                    fused_particle_t p5={.fx=fx,.fy=fy,.fvx=+1.0f*v_boom,.fvy=+0.0f*v_boom,.col=c_boom,.fuse=t_boom};
                    v_boom=15.0f+((float)(esp_random()%15));
                    t_boom=0.3f+(0.01f*((float)(esp_random()%35)));
                    fused_particle_t p6={.fx=fx,.fy=fy,.fvx=+1.0f*v_boom,.fvy=-1.0f*v_boom,.col=c_boom,.fuse=t_boom};
                    v_boom=15.0f+((float)(esp_random()%15));
                    t_boom=0.3f+(0.01f*((float)(esp_random()%35)));
                    fused_particle_t p7={.fx=fx,.fy=fy,.fvx=+0.0f*v_boom,.fvy=+1.0f*v_boom,.col=c_boom,.fuse=t_boom};
                    v_boom=15.0f+((float)(esp_random()%15));
                    t_boom=0.3f+(0.01f*((float)(esp_random()%35)));
                    fused_particle_t p8={.fx=fx,.fy=fy,.fvx=+0.0f*v_boom,.fvy=-1.0f*v_boom,.col=c_boom,.fuse=t_boom};
                    if ((esp_random()%4)==1)tetr1s_reloaded_fused_particles_add(self,&p1);
                    if ((esp_random()%4)==1)tetr1s_reloaded_fused_particles_add(self,&p2);
                    if ((esp_random()%4)==1)tetr1s_reloaded_fused_particles_add(self,&p3);
                    if ((esp_random()%4)==1)tetr1s_reloaded_fused_particles_add(self,&p4);
                    if ((esp_random()%4)==1)tetr1s_reloaded_fused_particles_add(self,&p5);
                    if ((esp_random()%4)==1)tetr1s_reloaded_fused_particles_add(self,&p6);
                    if ((esp_random()%4)==1)tetr1s_reloaded_fused_particles_add(self,&p7);
                    if ((esp_random()%4)==1)tetr1s_reloaded_fused_particles_add(self,&p8);


                  };
      };
  };*/
  sb->health=0;
};




tetr1s_reloaded_t* tetr1s_reloaded_construct(elite_pixel_game_t* ente){
  ELOG("INFO : [tetr1s_reloaded_construct] entering tetr1s_reloaded_construct()\n");


  ELOG("INFO : [tetr1s_reloaded_construct] allocating self(tetr1s_reloaded_t)\n");

  tetr1s_reloaded_t *self=e_mall0c(__FUNCTION__,sizeof(tetr1s_reloaded_t));
  if (self!=NULL) {
    ELOG("INFO : [tetr1s_reloaded_construct] successfully allocated self(tetr1s_reloaded_t)\n");

  }else {
    ELOG("ERROR : [tetr1s_reloaded_construct] failed to allocated self(tetr1s_reloaded_t); returning NULL from constructor\n");

    return NULL;
  };
  self->app_name="tetr1s_reloaded";//unused
  self->ship.fx=5.0f;
  self->ship.fy=27.0f;
  self->ship.fvx=6.0f;
  self->ship.fvy=0.0f;
  self->gun_intervall=0.01f*GUN_FUSE_100TH;
  self->gun_fuse=1.0f;
  self->gun_bullets_count=0;
  self->wave_char=0;
  for (int i=0;i<NUM_FUSED_PARTICLES;i++){
    self->fused_particles[i].fuse=0.0f;
    sfRGBA c={192.0f,192.0f,42.0f,192.0f};
    self->fused_particles[i].col=c;
  };


  self->p_bs_processor=bogo_script_processor_construct(self,&level0[0]);
  //for (int i=0;i<NUM_SPACE_BLOCKS;i++) {
  //  self->space_blocks[i].path_id=0;
  //}

//  tetr1s_reloaded_space_blocks_generate_wave(self,valid_chars[self->wave_char],0.01f*FORMATION_TIGHTNESS);
  for (int i=0;i<NUM_SPACE_BLOCKS;i++) {
    self->space_blocks[i].health=0;
    self->space_blocks[i].t_value=0;
    //self->space_blocks[i].x=-69;
    //self->space_blocks[i].y=-69;
    //get_position(self->space_blocks_t_values[i],&self->space_blocks[i].x,&self->space_blocks[i].y,self->space_blocks[i].path_id);
  };

  tetr1s_reloaded_bullets_clear(self);

  ELOG("INFO : [tetr1s_reloaded_construct] successfully constructed self(tetr1s_reloaded_t); returning self from constructor\n");


  elite_particle_shower_config_t my_particle_shower_config={.num_particles=50};
  self->my_particle_shower=elite_particle_shower_construct(my_particle_shower_config);


  return self;
};

void tetr1s_reloaded_update_ship(tetr1s_reloaded_t* self,float fElapsedTime){
  float df=self->ship.fvx*fElapsedTime;
  float fposx=self->ship.fx+df;
  if (fposx>1.0f&&fposx<9.0f) {
    self->ship.fx=fposx;
  }else {self->ship.fvx*=-1.0f;};//just in case we
  if (self->ship.fx>9.0f) {self->ship.fx=9.0f;};
  if (self->ship.fx<1.0f) {self->ship.fx=1.0f;};

}

void tetr1s_reloaded_draw_ship(tetr1s_reloaded_t* self, elite_pixel_game_t* ente){
    sfRGBA c={172.0f,164.0f,152.0f,255.0f};
    int x=(int)self->ship.fx;
    int y=(int)self->ship.fy;
    elite_pixel_game_fputpixelRGBA(ente,x,y,c);
    elite_pixel_game_fputpixelRGBA(ente,x,y-1,c);
    elite_pixel_game_fputpixelRGBA(ente,x-1,y,c);
    elite_pixel_game_fputpixelRGBA(ente,x+1,y,c);
    elite_pixel_game_fputpixelRGBA(ente,x-1,y+1,c);
    elite_pixel_game_fputpixelRGBA(ente,x+1,y+1,c);
}

bool tetr1s_reloaded_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
  if (tetr1s_reloaded_on_user_update_entered_log==false) {
    tetr1s_reloaded_on_user_update_entered_log=true;
    ELOG("INFO : [tetr1s_reloaded_on_user_update] entered tetr1s_reloaded_on_user_update() - this notification will only occur once\n");

  };

  tetr1s_reloaded_t *self=(tetr1s_reloaded_t*)params;



    elite_pixel_game_set_target_layer(ente,0);
    sfRGBA c={0.0f,0.0f,0.0f,255.0f};
    elite_pixel_game_fill_flayerRGBA(ente,c);

    bogo_script_processor_update(self->p_bs_processor,fElapsedTime);
//update all positions
  tetr1s_reloaded_update_ship(self,fElapsedTime);

  self->gun_fuse-=fElapsedTime;

  if (self->gun_fuse<=0.0f) {
    self->gun_fuse=self->gun_intervall;

    //self->gun_bullets_count+=1;
    bullet_t b={
      .fx=self->ship.fx,
      .fy=self->ship.fy-1.5f,
      .fvx=0.0f,
      .fvy=-1.0f*BULLET_SPEED,
      .col=bullet_col
    };
    tetr1s_reloaded_bullets_add(self,b);
    if (esp_random()%2==1) {self->gun_fuse+=self->gun_intervall;};
  };

  //if (self->gun_bullets_count==2) {self->gun_fuse=self->gun_intervall*2.5f;self->gun_bullets_count+=1;};
  //if (self->gun_bullets_count==6) {self->gun_fuse=self->gun_intervall*4.0f;self->gun_bullets_count=0;};
  tetr1s_reloaded_space_blocks_update_wave(self,fElapsedTime);


  tetr1s_reloaded_bullets_update(self,fElapsedTime);
  tetr1s_reloaded_fused_particles_update(self,fElapsedTime);

  //collision detection. its a bit messy. but :
  //i wanted to leave the bulk loop in here for now
  //to have collision handling(and explosion managment) in the user_update scope





        for (int j=0;j<NUM_SPACE_BLOCKS;j++) {
            for (int i=0;i<MAX_BULLETS;i++) {
                if (tetr1s_reloaded_bullet_is_gone(&self->bullets[i])||self->space_blocks[j].health==0) continue;

                if (tetr1s_reloaded_space_blocks_test_bullet_collision(&self->space_blocks[j],&self->bullets[i],ente)) {
                    self->space_blocks[j].health-=1;
                    tetr1s_reloaded_put_explosion(self,self->bullets[i].fx,self->bullets[i].fy);//boom

                    self->bullets[i].fx=-69.0f;//displace the bullets....
                    self->bullets[i].fy=-69.0f;
                };
            };

        };
/*

    if (self->space_blocks[0].path_id==69&&self->space_blocks[0].health==1){
            tetr1s_reloaded_block_explode(self,&self->space_blocks[0]);
            self->gun_fuse=3.5f;
    };
*/
  /*  if ((tetr1s_reloaded_all_spaceblock_gone(self)==true)&&(init_fuse<=0.0f)) {
          char last_wave_char=valid_chars[self->wave_char];

            self->wave_char+=1;
            self->wave_char%=strlen(valid_chars);
            tetr1s_reloaded_space_blocks_generate_wave(self,valid_chars[self->wave_char],0.01f*FORMATION_TIGHTNESS);
            if (self->space_blocks[0].block_char<'a') {
            };
            if (last_wave_char<='a') {
              for (int ii=0;ii<NUM_SPACE_BLOCKS;ii++) self->space_blocks_t_values[ii]-=5.0f;
            }
        };*/
    if (init_fuse>0.0f) init_fuse-=fElapsedTime;

  //if (self->space_blocks[0].block_char>='a') {

  //}else {
    //if (self->space_blocks[0].y<7.00f){
//      self->gun_fuse=0.5f;
  //  };
  //};

  if (tetr1s_reloaded_on_user_update_pre_particle_shower_update_log==false) {
      tetr1s_reloaded_on_user_update_pre_particle_shower_update_log=true;
      ELOG("INFO : [tetr1s_reloaded_on_user_update] calling particle_shower_update(fElapsedTime) - this notification will only occur once\n");
  };
  elite_particle_shower_update(self->my_particle_shower,fElapsedTime);

  elite_particle_shower_draw(ente,self->my_particle_shower);
  //draw_path0(ente);
  //draw_path1(ente);
  tetr1s_reloaded_draw_ship(self,ente);
  tetr1s_reloaded_fused_particles_draw(self,ente);
  tetr1s_reloaded_space_blocks_draw(self,ente);

  tetr1s_reloaded_bullets_draw(self,ente);


  if (tetr1s_reloaded_on_user_update_leaving_log==false) {
      tetr1s_reloaded_on_user_update_leaving_log=true;
      ELOG("INFO : [tetr1s_reloaded_on_user_update] leaving tetr1s_reloaded_on_user_update() - this notification will only occur once\n");

    };


  return true;
};

//todo : proper cleanup
bool tetr1s_reloaded_on_user_destroy(void* params){


    ELOG("INFO : [tetr1s_reloaded_on_user_update] entering tetr1s_reloaded_destroy()\n");

    tetr1s_reloaded_t *self=(tetr1s_reloaded_t*)params;
    ELOG("INFO : [tetr1s_reloaded_on_user_update] deallocating self(tetr1s_reloaded)\n");

    if (self!=NULL) {

        elite_particle_shower_destruct(self->my_particle_shower);
        free(self);
    ELOG("INFO : [tetr1s_reloaded_on_user_update] successfully deallocated self(tetr1s_reloaded)\n");

  }else {
    ELOG("ERROR : [tetr1s_reloaded_on_user_update] failed to deallocated self(tetr1s_reloaded); returning false from on_user_destroy()\n");

    return false;
  };
  ELOG("INFO : [tetr1s_reloaded_on_user_update] returning true from tetr1s_reloaded_on_user_destroy()\n");

  return true;
};

elite_pixel_game_config_t tetr1s_reloaded_config={
    .app_name="tetr1s_reloaded_pixel_app",
    .screen_width=10,
    .screen_height=30,
    .on_user_construct=(void*)&tetr1s_reloaded_construct,
    .on_user_update=&tetr1s_reloaded_on_user_update,
    .on_user_destroy=&tetr1s_reloaded_on_user_destroy
};

void tetr1s_reloaded_start_task(){

    ELOG("INFO : [main_start_pixel_game_task] entered main_start_pixelapp_task\n");

    ELOG("INFO : [main_start_pixel_game_task] creating &pixel_game_task\n");
    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 8192,&tetr1s_reloaded_config, 5, NULL);

    ELOG("INFO : [main_start_pixelapp_task] leaving tetris_reloaded_start_task\n");


};

void tetr1s_reloaded_space_block_create(space_block_t *p_sb,int shape_id,int size_id,int path_id){

       //ELOG("DEBUG : [%s] enter\n",__FUNCTION__);

    //   char* tetromino_chars="ojltizsOJLTIZS";
    //   bool is_valid_char=false;
      // for (int i=0;i<strlen(tetromino_chars);i++) {
        //  if (c_sb==tetromino_chars[i]) {is_valid_char=true;break;};
       //};
       //if (is_valid_char!=true) {
//         ELOG("ERROR : [tetr1s_reloaded_space_block_create] invalid arg c_sb\n");
  //       elite_panic(__FUNCTION__,"invalid arg c_sb",__LINE__);
    //   };
      // ELITE_CHECK(__FUNCTION__);

       if (p_sb==NULL) {
          ELOG("ERROR : [tetr1s_reloaded_space_block_create] invalid arg p_sb==NULL\n");
          elite_panic(__FUNCTION__,"nullptr p_sb",__LINE__);
       };
       ELITE_CHECK(__FUNCTION__);

       for (int j=0;j<MAX_SPACE_BLOCK_BODY_STR_LEN;j++) {p_sb->body_str[j]='.';};
       p_sb->body_str[MAX_SPACE_BLOCK_BODY_STR_LEN]=0;
       p_sb->width=0;
       p_sb->height=0;
       p_sb->color_index=0;
       p_sb->path_id=PATH_SIN_R;

       if ((shape_id<=SHAPE_DEFS_BEGIN)||(shape_id>=SHAPE_DEFS_END)) {
          ELOG("ERROR : [%s] invalid SHAPE_ID\n",__FUNCTION__);
          return;
       };
       if ((size_id<=SIZE_DEFS_BEGIN)||(size_id>=SIZE_DEFS_END)) {
          ELOG("ERROR : [%s] invalid SIZE_ID\n",__FUNCTION__);
          return;
       };
       if ((path_id<=PATH_DEFS_BEGIN)||(path_id>=PATH_DEFS_END)) {
          ELOG("ERROR : [%s] invalid PATH_ID\n",__FUNCTION__);
          return;
       };
       p_sb->path_id=path_id;
       p_sb->shape_id=shape_id;
       p_sb->size_id=size_id;


   switch (shape_id) {

       case SHAPE_O : {
              if (size_id==SIZE_S) {
              p_sb->width=2;
              p_sb->height=2;
              p_sb->color_index=1;
              char tmp[4]="11"
                          "11";
              for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
              break;
              };
              if (size_id==SIZE_M) {
                p_sb->width=3;
                p_sb->height=3;
                p_sb->color_index=1;
                char tmp[9]="111"
                            "1X1"
                            "111";
                for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;

              };
              if (size_id==SIZE_L) {
                p_sb->width=4;
                p_sb->height=4;
                p_sb->color_index=1;
                char tmp[16]="1111"
                             "1XX1"
                             "1XX1"
                             "1111";
                for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
              break;
              };
              if (size_id==SIZE_XL) {
                p_sb->width=5;
                p_sb->height=5;
                p_sb->color_index=1;
                  char tmp[25]="11111"
                               "1X1X1"
                               "11111"
                               "1X1X1"
                               "11111";
                for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
              break;
              };
              if (size_id==SIZE_XXL) {
                  p_sb->width=6;
                  p_sb->height=6;
                  p_sb->color_index=1;
                  char tmp[36]="XXX111"
                               "X1X1X1"
                               "XXX111"
                               "111XXX"
                               "1X1X1X"
                               "111XXX";
                  for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;

              break;
              };
          break;
          };
            /*  if (sub_type==5) {
                  p_sb->width=7;
                  p_sb->height=7;
                  p_sb->color_index=1;
                  char tmp[49]="1111111"
                               "1XX1XX1"
                               "1XX1XX1"
                               "1111111"
                               "1XX1XX1"
                               "1XX1XX1"
                               "1111111";
              };*/


               /*
                case 'O' : {
                         p_sb->width=8;
                         p_sb->height=8;
                         p_sb->color_index=1;
                         char tmp[64]="11111111"
                                      "1XX11XX1"
                                      "1XX11XX1"
                                      "11111111"
                                      "11111111"
                                      "1XX11XX1"
                                      "1XX11XX1"
                                      "11111111";

              };*/

        case SHAPE_I : {

              if (size_id==SIZE_S) {
                  p_sb->width=4;
                  p_sb->height=4;
                  p_sb->color_index=2;
                  char tmp[16]="..2."
                               "..2."
                               "..2."
                               "..2.";
                  for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                  break;
              };

              if (size_id==SIZE_M) {
                  p_sb->width=4;
                  p_sb->height=8;
                  p_sb->color_index=2;
                  char tmp[32]=".22."
                               ".22."
                               ".22."
                               ".22."
                               ".22."
                               ".22."
                               ".22."
                               ".22.";
                  for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                  break;
              };

              if (size_id==SIZE_L) {
                  p_sb->width=3;
                  p_sb->height=12;
                  p_sb->color_index=2;
                  char tmp[36]="222"
                               "2X2"
                               "222"
                               "222"
                               "2X2"
                               "222"
                               "222"
                               "2X2"
                               "222"
                               "222"
                               "2X2"
                               "222";
                  for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                  break;
              };

             if (size_id==SIZE_XL) {
                      p_sb->width=4;
                      p_sb->height=16;
                      p_sb->color_index=2;
                      char tmp[64]="2222"
                                   "2XX2"
                                   "2XX2"
                                   "2222"
                                   "2222"
                                   "2XX2"
                                   "2XX2"
                                   "2222"
                                   "2222"
                                   "2XX2"
                                   "2XX2"
                                   "2222"
                                   "2222"
                                   "2XX2"
                                   "2XX2"
                                   "2222";
                      for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                      break;
            };

            if (size_id==SIZE_XXL) {
                    p_sb->width=6;
                    p_sb->height=21;
                    p_sb->color_index=2;
                    char tmp[126]="222222"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "222222"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "222222"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "222222"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "2XXXX2"
                                  "222222";
                  for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                  break;
            };
        break;
        };

      case SHAPE_J : {

          if (size_id==SIZE_S) {
                    p_sb->width=3;
                    p_sb->height=3;
                    p_sb->color_index=3;
                    char tmp[9]=".3."
                                ".3."
                                "33.";
                    for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                    break;
          };
          if (size_id==SIZE_M) {
                    p_sb->width=4;
                    p_sb->height=6;
                    p_sb->color_index=3;
                    char tmp[24]="..33"
                                 "..33"
                                 "..33"
                                 "..33"
                                 "3333"
                                 "3333";
                    for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                    break;
          };
          if (size_id==SIZE_L) {
                    p_sb->width=6;
                    p_sb->height=9;
                    p_sb->color_index=3;
                    char tmp[54]="...333"
                                 "...3X3"
                                 "...333"
                                 "...333"
                                 "...3X3"
                                 "...333"
                                 "333333"
                                 "3X33X3"
                                 "333333";
                    for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                    break;
          };

          if ((size_id==SIZE_XL)||(size_id==SIZE_XXL)) {
                     p_sb->width=8;
                     p_sb->height=12;
                     p_sb->color_index=3;
                     char tmp[96]="....3333"
                                  "....3XX3"
                                  "....3XX3"
                                  "....3333"
                                  "....3333"
                                  "....3XX3"
                                  "....3XX3"
                                  "....3333"
                                  "33333333"
                                  "3XX33XX3"
                                  "3XX33XX3"
                                  "33333333";
                      for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                      break;
          };
      break;
      };

      case SHAPE_L :{

          if (size_id==SIZE_S) {
            ELOG("DEBUG : [%s] SHAPE L %i - SIZE S %i \n",__FUNCTION__,shape_id,size_id);
              p_sb->width=3;
              p_sb->height=3;
              p_sb->color_index=4;
              char tmp[9]=".4."
                          ".4."
                          ".44";
              for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
              break;
          };

          if (size_id==SIZE_M) {
               p_sb->width=4;
               p_sb->height=6;
               p_sb->color_index=4;
               char tmp[32]="44.."
                            "44.."
                            "44.."
                            "44.."
                            "4444"
                            "4444";
              for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
              break;
          };

          if (size_id==SIZE_L) {
                p_sb->width=6;
                p_sb->height=9;
                p_sb->color_index=4;
                char tmp[54]="444..."
                             "4X4..."
                             "4X4..."
                             "4X4..."
                             "4X4..."
                             "4X4..."
                             "4X4444"
                             "4XXXX4"
                             "444444";
                for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                break;
          };

          if ((size_id==SIZE_XL)||(size_id==SIZE_XXL)) {
               p_sb->width=8;
               p_sb->height=12;
               p_sb->color_index=4;
               char tmp[96]="4444...."
                            "4XX4...."
                            "4XX4...."
                            "4444...."
                            "4444...."
                            "4XX4...."
                            "4XX4...."
                            "4444...."
                            "44444444"
                            "4XX44XX4"
                            "4XX44XX4"
                            "44444444";
               for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
               break;
           };
       break;
     };

       case SHAPE_T : {
           if (size_id==SIZE_S) {
                  p_sb->width=3;
                  p_sb->height=3;
                  p_sb->color_index=5;
                  char tmp[9]="..."
                              "555"
                              ".5." ;
                  for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                  break;
            };

           if (size_id==SIZE_M) {
                 p_sb->width=6;
                 p_sb->height=4;
                 p_sb->color_index=5;
                 char tmp[24]="55XX55"
                              "55XX55"
                              "..55.."
                              "..55..";
                for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                break;
          };


          if (size_id==SIZE_L) {
                  p_sb->width=7;
                  p_sb->height=6;
                  p_sb->color_index=5;
                  char tmp[42]="5555555"
                               "5X5X5X5"
                               "5555555"
                               "..5X5.."
                               "..5X5.."
                               "..555..";
                  for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                  break;

          };
          if ((size_id==SIZE_XL)||(size_id==SIZE_XXL)) {
              p_sb->width=8;
              p_sb->height=12;
              p_sb->color_index=5;
              char tmp[96]="....5555"
                           "....5XX5"
                           "....5XX5"
                           "....5555"
                           "55555555"
                           "5XX55XX5"
                           "5XX55XX5"
                           "55555555"
                           "....5555"
                           "....5XX5"
                           "....5XX5"
                           "....5555"
                          ;
              for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
              break;
          };
          break;
        };

       case SHAPE_S : {
         if (size_id==SIZE_S) {
           ELOG("DEBUG : [%s] SHAPE S %i - SIZE S %i \n",__FUNCTION__,shape_id,size_id);
                p_sb->width=3;
                p_sb->height=3;
                p_sb->color_index=6;
                char tmp[9]="..6"
                            ".66"
                            ".6.";
                for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                break;
         };
         if (size_id==SIZE_M){
                p_sb->width=4;
                p_sb->height=6;
                p_sb->color_index=6;
                char tmp[36]="XX.."
                             "XX.."
                             "66XX"
                             "66XX"
                             "..66"
                             "..66";
                for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                break;
          };
          if (size_id==SIZE_L){
                p_sb->width=6;
                p_sb->height=9;
                p_sb->color_index=6;
                char tmp[54]="666..."
                             "6X6..."
                             "666..."
                             "666666"
                             "6X66X6"
                             "666666"
                             "...666"
                             "...6X6"
                             "...666";
              for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];}
               break;
          };
          if ((size_id==SIZE_XL)||(size_id==SIZE_XXL)) {
              p_sb->width=8;
              p_sb->height=12;
              p_sb->color_index=6;
              char tmp[96]="6666...." //666
                           "6XX6...." //666
                           "6XX6...." //666
                           "6666...." //666666
                           "66666666" //666666
                           "6XX66XX6" //666666
                           "6XX66XX6" //   666
                           "66666666" //   666
                           "....6666" //   666
                           "....6XX6"
                           "....6XX6"
                           "....6666";
              for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
              break;
              };
          break;
          };


       case SHAPE_Z : {
            if (size_id==SIZE_S) {
              ELOG("DEBUG : [%s] SHAPE Z %i - SIZE S %i \n",__FUNCTION__,shape_id,size_id);
                p_sb->width=3;
                p_sb->height=3;
                p_sb->color_index=7;
                char tmp[9]="..7"
                            ".77"
                            ".7.";
                for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                break;
            };

            if (size_id==SIZE_M) {
                p_sb->width=4;
                p_sb->height=6;
                p_sb->color_index=6;
                char tmp[24]="..X7"
                             "..7X"
                             "X7X7"
                             "7X7X"
                             "X7.."
                             "7X..";
                for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
                break;
            };

            if (size_id==SIZE_L) {
                p_sb->width=6;
                p_sb->height=9;
                p_sb->color_index=6;
                char tmp[54]="...777"
                             "...7X7"
                             "...777"
                             "777XXX"
                             "7X7X7X"
                             "777XXX"
                             "XXX..."
                             "X7X..."
                             "XXX...";
               for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;
               break;
             };

            if((size_id==SIZE_XL)||(size_id==SIZE_XXL)) {
                p_sb->width=8;
                p_sb->height=12;
                p_sb->color_index=7;
                char tmp[96]="....7777"
                             "....7XX7"
                             "....7XX7"
                             "....7777"
                             "77777777"
                             "7XX77XX7"
                             "7XX77XX7"
                             "77777777"
                             "7777...."
                             "7XX7...."
                             "7XX7...."
                             "7777....";
               for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];};return;return;
               break;
            };//if
        break;
      };//case
      default : {ELOG("ERROR : [%s] UNREACHABLE\n",__FUNCTION__);return;};
    };//switch
};
//for (int i=0;i<(p_sb->width*p_sb->height);i++){p_sb->body_str[i]=tmp[i];
