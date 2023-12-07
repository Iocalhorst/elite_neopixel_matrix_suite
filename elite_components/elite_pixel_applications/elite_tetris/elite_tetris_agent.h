//TODO : exclude square piece from rotation (amonst other redundant permutations) in brute force simulation procedure. for reasons.
#pragma once
#include "string.h"
#include "elite_tetris_block.h"
#include "elite.h"
typedef struct {
  int tower_height;
  int hole_count;
  int column_height_sum;
  int bumpiness;
  int num_lines_completed;
  int rotation_count;
  int x;
}resultset_t;

typedef struct{

          uint8_t test_view[10][30];//temporary, volatile copy of the playfield. used for simulating hard drops,
          uint8_t backup_view[10][30];//a copy of the playfield - must not touch - use to reset/restore the testView
          elite_tetris_block_t* p_test_block;
          elite_tetris_block_t* p_backup_block;
          float f_best_result_score;
          int best_result_x;
          int best_result_rotation_count;
          float f_keystroke_intervall;//=0.5f;
          //LinkedList<sResultSet> listResults;
          float f_fuse;
}elite_tetris_agent_t;

elite_tetris_agent_t* elite_tetris_agent_construct();
char elite_tetris_agent_get_key(elite_tetris_agent_t* self,float fElapsedTime,elite_tetris_block_t* p_current_block);
void elite_tetris_agent_read_game_state(elite_tetris_agent_t* self,uint8_t p_playfield[10][30],elite_tetris_block_t* p_current_block);
void elite_tetris_agent_consolidate_test_view(elite_tetris_agent_t* self,elite_tetris_block_t* p_block);
bool elite_tetris_agent_check_for_collision(elite_tetris_agent_t* self,elite_tetris_block_t *block);
int elite_tetris_agent_remove_and_consolidate_lines(elite_tetris_agent_t* self);
bool elite_tetris_agent_has_filled_fields_above(elite_tetris_agent_t* self,int x,int y);
int elite_tetris_agent_get_hole_count(elite_tetris_agent_t* self);
void elite_tetris_agent_restore_test_view(elite_tetris_agent_t* self);
bool elite_tetris_agent_row_is_empty(elite_tetris_agent_t* self,int y);
void elite_tetris_agent_hard_drop(elite_tetris_agent_t* self,elite_tetris_block_t *p_block);
int elite_tetris_agent_get_tower_height(elite_tetris_agent_t* self);
int elite_tetris_agent_get_column_height_sum(elite_tetris_agent_t* self);
int elite_tetris_agent_get_bumpiness(elite_tetris_agent_t* self);
int elite_tetris_agent_get_column_height(elite_tetris_agent_t* self,int x);
void elite_tetris_agent_run_simulation(elite_tetris_agent_t* self);
void elite_tetris_agent_set_keystroke_intervall(elite_tetris_agent_t* self,float f);
float elite_tetris_agent_evaluate_resultset(resultset_t resultset);

elite_tetris_agent_t* elite_tetris_agent_construct(){

//  ELOG("DEBUG : [elite_tetris_agent_construct] enter\n");

  elite_tetris_agent_t* self=(elite_tetris_agent_t*)e_mall0c(__FUNCTION__,sizeof(elite_tetris_agent_t));

  self->f_fuse=0.3f;
  //self->listResults=LinkedList<sResultSet>();
  self->f_keystroke_intervall=0.25f;
  self->p_backup_block=(elite_tetris_block_t*)e_mall0c(__FUNCTION__,sizeof(elite_tetris_block_t));
  self->p_backup_block->body_str=(char*)e_mall0c(__FUNCTION__,16);
  self->p_test_block=(elite_tetris_block_t*)e_mall0c(__FUNCTION__,sizeof(elite_tetris_block_t));
  self->p_test_block->body_str=(char*)e_mall0c(__FUNCTION__,16);

//  ELOG("DEBUG : [elite_tetris_agent_construct] exit\n");
  return self;
};

char elite_tetris_agent_get_key(elite_tetris_agent_t* self,float fElapsedTime,elite_tetris_block_t* p_current_block){

//    ELOG("DEBUG : [elite_tetris_agent_read_game_state] return null\n");
    //
    self->f_fuse-=fElapsedTime;

    if (self->f_fuse>0.0f) {
//      ELOG("DEBUG : [elite_tetris_agent_read_game_state] return null\n");
      //
      return '\0';}
    else {
        self->f_fuse=self->f_keystroke_intervall;
        if (self->best_result_rotation_count>0) {
          //ELOG("DEBUG : [elite_tetris_agent_get_key_game_state] return 'u'\n");
          //
          self->best_result_rotation_count-=1;
          //ELOG("rotate l\n");
          //
          return 'u';
        };
        if (p_current_block->x>self->best_result_x) {
          //ELOG("DEBUG : [elite_tetris_agent_get_key] return 'l'\n");
          //
          return 'l';
        }
        if (p_current_block->x<self->best_result_x) {
          //ELOG("DEBUG : [elite_tetris_agent_get_key] return 'r'\n");
          //
          return 'r';
        };
      //  if (self->best_result_x==p_current_block->x&&self->best_result_rotation_count==0) {
          //ELOG("DEBUG : [elite_tetris_agent_get_key] return 'd'\n");
          //
        //  self->f_fuse=0.15f*self->f_keystroke_intervall;
        //  return 'd';
        //};
    };
    //    ELOG("ERROR : [elite_tetris_agent_get_key] UNREACHABLE\n");
    //
        return 42;
};

void elite_tetris_agent_read_game_state(elite_tetris_agent_t* self,uint8_t p_playfield[10][30],elite_tetris_block_t* p_current_block){

  //  ELOG("DEBUG : [elite_tetris_agent_read_game_state] entering\n");
//
//    char line_str[15]={0};


    int playfield_checksum=0;
    for (int i=0;i<30;i++){
        for (int j=0;j<10;j++){
            playfield_checksum+=p_playfield[j][i];
            self->backup_view[j][i]=p_playfield[j][i];
        };
    };
//    ELOG("DEBUG : [elite_tetris_agent_read_game_state] checksum==%i \n",playfield_checksum);
    //
    //

    self->p_backup_block->width=p_current_block->width;
    self->p_backup_block->height=p_current_block->height;
    self->p_backup_block->x=p_current_block->x;
    self->p_backup_block->y=p_current_block->y;

    for (int i=0;i<16;i++) self->p_backup_block->body_str[i]=p_current_block->body_str[i];

    /*ELOG("DEBUG : [elite_tetris_agent_read_game_state] \n p_backup_block->x==%i\n p_backup_block->y==%i\n",self->p_backup_block->x,self->p_backup_block->y);



    line_str[line_str_index++]='|';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='|';
    line_str[line_str_index++]='\n';line_str[line_str_index]='\0';
    ELOG(line_str);

    line_str_index=0;
    for (int i=0;i<30;i++) {
        line_str[line_str_index++]='|';
        for (int j=0;j<10;j++) {
            if (self->backup_view[j][i]>0) {
              line_str[line_str_index++]='#';
            }else{
              line_str[line_str_index++]='.';
            };
        };
        line_str[line_str_index++]='|';
        line_str[line_str_index++]='\n';
        line_str[line_str_index++]='\0';
        ELOG(line_str);

    };
    line_str_index=0;
    line_str[line_str_index++]='|';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='_';
    line_str[line_str_index++]='_';line_str[line_str_index++]='|';
    line_str[line_str_index++]='\n';line_str[line_str_index]='\0';
    ELOG(line_str);



*/
  //  ELOG("DEBUG : [elite_tetris_agent_read_game_state] leaving\n");
  //
};

void elite_tetris_agent_restore_test_view(elite_tetris_agent_t* self){

//
    for (int i=0;i<10;i++){
        for (int j=0;j<30;j++){
            self->test_view[i][j]=self->backup_view[i][j];
        };
    };
};
void elite_tetris_agent_restore_test_block(elite_tetris_agent_t* self){
    self->p_test_block->x=self->p_backup_block->x;
    self->p_test_block->y=self->p_backup_block->y;
    self->p_test_block->width=self->p_backup_block->width;
    self->p_test_block->height=self->p_backup_block->height;
    for (int i=0;i<16;i++) {
      self->p_test_block->body_str[i]=self->p_backup_block->body_str[i];
    };
};


void elite_tetris_agent_consolidate_test_view(elite_tetris_agent_t* self,elite_tetris_block_t* p_block){
    for (int i=0;i<p_block->height;i++){
        for (int j=0;j<p_block->width;j++){
            int x=j+p_block->x;
              int y=i+p_block->y;
              if ((x>=0)&&(x<10)&&(y<30)&&(y>=0)) {
                  if (p_block->body_str[p_block->width*i+j]!='_') {self->test_view[x][y]=p_block->color_index;};
              };
        };
    };
};

void elite_tetris_agent_set_keystroke_intervall(elite_tetris_agent_t* self,float t){self->f_keystroke_intervall=t;};

int elite_tetris_agent_remove_and_consolidate_lines(elite_tetris_agent_t* self){
  int num_lines_cleared=0;
  for (int y=0;y<30;y++) {
    int result=1;
    for (int x=0;x<10;x++) {
      result*=self->test_view[x][y];
    };
    if (result!=0) {
      num_lines_cleared+=1;
      for (int y2=y;y2>0;y2--) {
        for (int x=0;x<10;x++) {
          self->test_view[x][y2]=self->test_view[x][y2-1];
        };
      };
    };
  };
  return num_lines_cleared;
};



bool elite_tetris_agent_check_for_collision(elite_tetris_agent_t* self,elite_tetris_block_t *p_block){


  //ELOG("DEBUG : [elite_tetris_check_for_collision] entering\n");
  //
  if (p_block==NULL) {
    //  ELOG("DEBUG : [elite_tetris_check_for_collision] assertion fail(block!=null)\n");
    //
  };

  if (p_block->body_str==NULL) {
    //  ELOG("DEBUG : [elite_tetris_check_for_collision] assertion fail(block->body_str!=null)\n");
    //
  };

    for (int i=0;i<p_block->height;i++){
        for (int j=0;j<p_block->width;j++){
            int x=j+p_block->x;
            int y=i+p_block->y;

            char block_char=p_block->body_str[p_block->width*i+j];

            if (block_char!='_') {
                if (x<0||x>=10||y>=30||y<0) {
                  //ELOG("DEBUG : out_of_bounds collision detected i=%i, j=%i, x=%i, y=%i block_char=%i \n",i,j,x,y,block_char);
                  //
                  //
                  return true;
                };
                if (((x>=0)&&(x<10)&&(y<30)&&(y>=0))&&(self->test_view[x][y]!=0)) {
                    //ELOG("DEBUG : collision detected i=%i, j=%i, x=%i, y=%i backup_view[x,y]=%i block_char=%i \n",i,j,x,y,self->test_view[x][y],block_char);
                    //
                    //
                    return true;
                };
            };
        };
    };
    return false;
};


void elite_tetris_agent_hard_drop(elite_tetris_agent_t* self,elite_tetris_block_t *block){

    while (elite_tetris_agent_check_for_collision(self,block)==false) {
    //    ELOG("DEBUG : [elite_tetris_hard_drop] y+=1 \n");
  //
        block->y+=1;
    };
  //  ELOG("DEBUG : [elite_tetris_hard_drop] boink! \n");
  //
    block->y-=1;
  //  ELOG("DEBUG : [elite_tetris_hard_drop] consolidate_test_view()\n");
  //
    elite_tetris_agent_consolidate_test_view(self,block);

};

bool elite_tetris_agent_row_is_empty(elite_tetris_agent_t *self,int y){
  for (int x=0;x<10;x++) {
      if (self->test_view[x][y]>0) return false;
  };
  return true;
};


int elite_tetris_agent_get_tower_height(elite_tetris_agent_t* self){
    int row=0;
    while (elite_tetris_agent_row_is_empty(self,row)) {
        row++;
    };
    int height=30-row;
    return height;
};


bool elite_tetris_agent_has_filled_fields_above(elite_tetris_agent_t* self,int x,int y){
 // cEliteSystem::log("fieldsabove");


  for (int i=0;i<y;i++) {
      if (self->test_view[x][i]!=0) {
            //ELOG("DEBUG : [has_filled_fields_above] hole field at x=%i, y=%i has x=%i y=%i\n",x,y,x,i);
            //
            //
        return true;
      }
  };
 //if (this->testView[x][y-1]!=0) return true;
  return false;

};

int elite_tetris_agent_get_hole_count(elite_tetris_agent_t* self){
  int holes=0;
 // cEliteSystem::log("holecount");
  for (int y=29;y>0;y--) {
    for (int x=0;x<10;x++) {
      if ((self->test_view[x][y]==0)&&(elite_tetris_agent_has_filled_fields_above(self,x,y))){holes+=1;};
    };
  };
  return holes;
};

int elite_tetris_agent_get_column_height(elite_tetris_agent_t *self,int x){
    int column_height=0;
    for (int y=0;y<30;y++) {
        if (self->test_view[x][y]!=0) {column_height=30-y;return column_height;};
    };
    return column_height;
};

int elite_tetris_agent_get_column_height_sum(elite_tetris_agent_t *self){
    int sum=0;
    for (int x=0;x<10;x++) {
        sum+=elite_tetris_agent_get_column_height(self,x);
    };
    return sum;
};

int elite_tetris_agent_get_bumpiness(elite_tetris_agent_t *self){
    int sum=0;
    for (int x=0;x<9;x++) {
        int r1=elite_tetris_agent_get_column_height(self,x);
        int r2=elite_tetris_agent_get_column_height(self,x+1);
        int diff=0;
        if (r1>r2) diff=r2-r1;
        if (r2<r1) diff=r1-r2;
        sum+=diff;
    };
    return sum;
};




float elite_tetris_agent_evaluate_resultset(resultset_t resultset){
    float coeff_height=-0.25f;
    (void)coeff_height;
    float coeff_holes=-0.8f;//-0.8
    (void)coeff_holes;
    float coeff_bump=-0.33f;
    (void)coeff_bump;
//    int bestResultIndex=0;
      //  sResultSet currentResultSet=this->listResults.get(bestResultIndex);
    float f_score=coeff_holes*resultset.hole_count;
    f_score+=coeff_height*resultset.column_height_sum;
    f_score+=coeff_bump*resultset.bumpiness;
    return f_score;
};


void elite_tetris_agent_run_simulation(elite_tetris_agent_t *self){


    //ELOG("DEBUG [elite_tetris_agent_run_simulation] : entered");
    //
    //




    /*String s="current hole count :"+String(this->getHoleCount());
    s="current columnHeightSum :"+String(this->getColumnHeightSum());
    s="current bumpiness :"+String(this->getBumpiness());*/


    int tower_height=0;
    int hole_count=0;
    int column_height_sum=0;
    int num_lines_completed=0;
    int bumpiness=0;

    elite_tetris_agent_restore_test_view(self);
    elite_tetris_agent_restore_test_block(self);
    elite_tetris_agent_hard_drop(self,self->p_test_block);
    num_lines_completed=elite_tetris_agent_remove_and_consolidate_lines(self);
    tower_height=elite_tetris_agent_get_tower_height(self);
    column_height_sum=elite_tetris_agent_get_column_height_sum(self);
    hole_count=elite_tetris_agent_get_hole_count(self);
    bumpiness=elite_tetris_agent_get_bumpiness(self);

    resultset_t first_test_result_set={
                  .tower_height=tower_height,
                  .hole_count=hole_count,
                  .column_height_sum=column_height_sum,
                  .bumpiness=bumpiness,
                  .num_lines_completed=num_lines_completed,
                  .rotation_count=0,
                  .x=self->p_test_block->x
                };
           //step 3 : ???
    self->f_best_result_score=elite_tetris_agent_evaluate_resultset(first_test_result_set);
    self->best_result_x=self->p_backup_block->x;
    self->best_result_rotation_count=0;


  //this->listResults.clear();
    const int num_rotations=3;
    for (int rotation_count=0;rotation_count<=num_rotations;rotation_count++) {

        //step 1 analyze the vertical degree of freedom limits for each permutation/rotation
        elite_tetris_agent_restore_test_view(self);
        elite_tetris_agent_restore_test_block(self);
        if (rotation_count==0) {};
        if (rotation_count==1) {elite_tetris_block_rotate_l(self->p_test_block);};
        if (rotation_count==2) {elite_tetris_block_rotate_l(self->p_test_block);elite_tetris_block_rotate_r(self->p_test_block);};
        if (rotation_count==3) {elite_tetris_block_rotate_l(self->p_test_block);elite_tetris_block_rotate_r(self->p_test_block);elite_tetris_block_rotate_r(self->p_test_block);};
        while (elite_tetris_agent_check_for_collision(self,self->p_test_block)==false) {
            self->p_test_block->x-=1;
            //ELOG("DEBUG [elite_tetris_agent_run_simulation] : testing for limit_left %i\n",self->p_test_block->x);
            //
            //
        };
        self->p_test_block->x+=1;
        int limit_left=self->p_test_block->x;
        //ELOG("DEBUG [elite_tetris_agent_run_simulation] : limit_left==%i\n",limit_left);
        //
        //

        while (elite_tetris_agent_check_for_collision(self,self->p_test_block)==false) {
            self->p_test_block->x+=1;
            //ELOG("DEBUG [elite_tetris_agent_run_simulation] : testing for limit_right %i\n",self->p_test_block->x);
            //
            //
        };

        self->p_test_block->x-=1;
        int limit_right=self->p_test_block->x;
        //ELOG("DEBUG [elite_tetris_agent_run_simulation] : limit_right=%i\n",limit_right);
        //
        //


        //step 2 run the hardrop simulations for each column within the limits from step 1
        for (int test_x=limit_left;test_x<=limit_right;test_x++) {
            self->p_test_block->x=test_x;
            self->p_test_block->y=self->p_backup_block->y;
            //ELOG("DEBUG [elite_tetris_agent_run_simulation] : restore_test_view()\n");
            //
            //

            elite_tetris_agent_restore_test_view(self);

            //ELOG("DEBUG [elite_tetris_agent_run_simulation] : hardrop()\n");
            //
            //
            elite_tetris_agent_hard_drop(self,self->p_test_block);

            //ELOG("DEBUG [elite_tetris_agent_run_simulation] : elite_tetris_agent_remove_and_consolidate_lines();");
            //
            //
            num_lines_completed=elite_tetris_agent_remove_and_consolidate_lines(self);

            //ELOG("DEBUG [elite_tetris_agent_run_simulation] : get_tower_height()");
            //
            //
            tower_height=elite_tetris_agent_get_tower_height(self);

            //ELOG("DEBUG [elite_tetris_agent_run_simulation] : get_column_height_sum()");
            //
            //
            column_height_sum=elite_tetris_agent_get_column_height_sum(self);

            //ELOG("DEBUG [elite_tetris_agent_run_simulation] : get_hole_count()");
            //
            //
            hole_count=elite_tetris_agent_get_hole_count(self);

            //ELOG("DEBUG [elite_tetris_agent_run_simulation] : get_bumpiness()");
            //
            //
            bumpiness=elite_tetris_agent_get_bumpiness(self);


            resultset_t test_result_set={
              .tower_height=tower_height,
              .hole_count=hole_count,
              .column_height_sum=column_height_sum,
              .bumpiness=bumpiness,
              .num_lines_completed=num_lines_completed,
              .rotation_count=rotation_count,
              .x=test_x
            };
       //step 3 : ???

//            ELOG("DEBUG [elite_tetris_agent_run_simulation] : evaluate_resultset()");
  //
    //

            float f_test_result_score=elite_tetris_agent_evaluate_resultset(test_result_set);
        //    ELOG("DEBUG [elite_tetris_agent_run_simulation] : f_test_result_score==%f\n",f_test_result_score);
          //
            //

       //step 4 : profit
            if (f_test_result_score>self->f_best_result_score) {
              self->f_best_result_score=f_test_result_score;
              self->best_result_x=test_result_set.x;
              self->best_result_rotation_count=test_result_set.rotation_count;
            //  ELOG("DEBUG [elite_tetris_agent_run_simulation] : f_best_result_score==%f\n",self->f_best_result_score);
            //
            //

            };

        };
    };
  //  ELOG("DEBUG : [elite_tetris_agent_run_simulation] f_best_result_score==%f\n",self->f_best_result_score);
    //
    //
  //  ELOG("DEBUG : [elite_tetris_agent_run_simulation] best_result_x==%i\n",self->best_result_x);
    //
    //
  //  ELOG("DEBUG : [elite_tetris_agent_run_simulation] best_rotation_count==%i\n",self->best_result_rotation_count);
    //

  };
