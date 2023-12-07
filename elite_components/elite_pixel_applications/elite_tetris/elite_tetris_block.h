#pragma once
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

     char tmp_str[16]={0};

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

     char tmp_str[16]={0};

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



 void elite_tetris_block_create(elite_tetris_block_t* p_block,char c_block){


     char* tetromino_chars="OJLTIZS";
     bool is_valid_char=false;
     for (int i=0;i<strlen(tetromino_chars);i++) {
       if (c_block==tetromino_chars[i]) {is_valid_char=true;break;};
     };
     if (is_valid_char!=true) {
       ELOG("ERROR : [elite_tetris_block_create] invalid arg c_block\n");
       elite_panic(__FUNCTION__,"invalid arg c_block",__LINE__);
     };
     ELITE_CHECK(__FUNCTION__);

     if (p_block==NULL) {
        ELOG("ERROR : [elite_tetris_block_create] invalid arg p_block==NULL\n");
        elite_panic(__FUNCTION__,"nullptr p_block",__LINE__);
     };
     ELITE_CHECK(__FUNCTION__);


     switch (c_block) {

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
