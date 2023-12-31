#pragma once
#define MAX_SPACE_BLOCK_BODY_STR_LEN 145
#define MAX_TETRIS_BLOCK_BODY_STR_LEN 145

typedef struct {
   char *body_str;
   int x;
   int y;
   uint8_t width;
   uint8_t height;
   uint8_t color_index;
   char block_char;
 }elite_tetris_block_t;

 void elite_tetris_block_rotate_l(elite_tetris_block_t* p_block){

     if (!((p_block->width==3&&p_block->height==3)||(p_block->width==4&&p_block->height==4))) return;

     char tmp_str[MAX_TETRIS_BLOCK_BODY_STR_LEN]={0};

     for (int i=0;i<p_block->height*p_block->width;i++){
       tmp_str[i]=p_block->body_str[i];
     }

     if (p_block->width==3&&p_block->height==3) {
             p_block->body_str[0]=tmp_str[2];
             p_block->body_str[1]=tmp_str[5];
             p_block->body_str[2]=tmp_str[8];
             p_block->body_str[3]=tmp_str[1];
             p_block->body_str[4]=tmp_str[4];
             p_block->body_str[5]=tmp_str[7];
             p_block->body_str[6]=tmp_str[0];
             p_block->body_str[7]=tmp_str[3];
             p_block->body_str[8]=tmp_str[6];
     };

     if (p_block->width==4&&p_block->height==4) {

           p_block->body_str[0]=tmp_str[3];
           p_block->body_str[1]=tmp_str[7];
           p_block->body_str[2]=tmp_str[11];
           p_block->body_str[3]=tmp_str[15];
           p_block->body_str[4]=tmp_str[2];
           p_block->body_str[5]=tmp_str[6];
           p_block->body_str[6]=tmp_str[10];
           p_block->body_str[7]=tmp_str[14];
           p_block->body_str[8]=tmp_str[1];
           p_block->body_str[9]=tmp_str[5];
           p_block->body_str[10]=tmp_str[9];
           p_block->body_str[11]=tmp_str[13];
           p_block->body_str[12]=tmp_str[0];
           p_block->body_str[13]=tmp_str[4];
           p_block->body_str[14]=tmp_str[8];
           p_block->body_str[15]=tmp_str[12];


     };

 };

 void elite_tetris_block_rotate_r(elite_tetris_block_t* p_block){

     if (!((p_block->width==3&&p_block->height==3)||(p_block->width==4&&p_block->height==4))) return;

     char tmp_str[MAX_TETRIS_BLOCK_BODY_STR_LEN]={0};

     for (int i=0;i<p_block->height*p_block->width;i++){
       tmp_str[i]=p_block->body_str[i];
     }

     if (p_block->width==3&&p_block->height==3) {
             p_block->body_str[0]=tmp_str[6];
             p_block->body_str[1]=tmp_str[3];
             p_block->body_str[2]=tmp_str[0];
             p_block->body_str[3]=tmp_str[7];
             p_block->body_str[4]=tmp_str[4];
             p_block->body_str[5]=tmp_str[1];
             p_block->body_str[6]=tmp_str[8];
             p_block->body_str[7]=tmp_str[5];
             p_block->body_str[8]=tmp_str[2];
     };

     if (p_block->width==4&&p_block->height==4) {

           p_block->body_str[0]=tmp_str[12];
           p_block->body_str[1]=tmp_str[8];
           p_block->body_str[2]=tmp_str[4];
           p_block->body_str[3]=tmp_str[0];
           p_block->body_str[4]=tmp_str[13];
           p_block->body_str[5]=tmp_str[9];
           p_block->body_str[6]=tmp_str[5];
           p_block->body_str[7]=tmp_str[1];
           p_block->body_str[8]=tmp_str[14];
           p_block->body_str[9]=tmp_str[10];
           p_block->body_str[10]=tmp_str[6];
           p_block->body_str[11]=tmp_str[2];
           p_block->body_str[12]=tmp_str[15];
           p_block->body_str[13]=tmp_str[11];
           p_block->body_str[14]=tmp_str[7];
           p_block->body_str[15]=tmp_str[3];


     };

 };
