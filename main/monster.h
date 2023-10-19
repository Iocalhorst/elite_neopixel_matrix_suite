#include "esp_random.h"
#include "elite.h"
#include "elite_pixel_game_ente.h"
#pragma once
const int minimum_eyes_delay=2500;
const int maximum_eyes_add_delay=2500;

#define NUM_MONSTERS 3
const int monster_bitmap[100]={ 0,0,0,0,0,0,0,0,0,0,
                                0,0,0,1,1,1,2,0,0,0,
                                0,0,1,1,1,1,1,2,0,0,
                                0,1,1,3,3,1,3,3,2,0,
                                0,1,1,3,3,1,3,3,2,0,
                                0,1,1,1,1,1,1,2,2,0,
                                0,1,1,1,1,1,1,2,2,0,
                                0,1,1,1,1,1,1,2,2,0,
                                0,1,0,1,1,0,1,2,2,0,
                                0,0,0,0,0,0,0,0,0,0 };



typedef struct {
    int eyes_direction;
    sRGB cols[5];
}s_monster_t;


#define UP_LEFT 0
#define UP_CENTER 1
#define UP_RIGHT 2
#define DOWN_LEFT 3
#define DOWN_CENTER 4
#define DOWN_RIGHT 5


s_monster_t monsters[NUM_MONSTERS]={{0,{black,yellow,dark_yellow,white,black}},
                                    {2,{black,pink,dark_pink,white,black}},
                                    {4,{black,blue,dark_blue,white,black}}};



static int monsters_get_new_eye_direction(int old_direction){
      //to prevent single eye movement because it just looks stupid;
      int new_dir=esp_random()%6;
      int old_dir=old_direction;
      //elog("getting new eye_dir\n");
      if (old_dir==DOWN_LEFT) {
          for (;;) {
            if ((new_dir==DOWN_RIGHT)||(new_dir==UP_LEFT)||(new_dir==UP_CENTER)||(new_dir==UP_RIGHT)) {break;}
            else {new_dir+=1;new_dir%=6;};
          };
      };

    if (old_dir==DOWN_CENTER) {
          for (;;) {
                                            if ((new_dir==UP_LEFT)||(new_dir==UP_CENTER)||(new_dir==UP_RIGHT)){break;}
                                            else {new_dir+=1;new_dir%=6;};
                                          };
                                        };

                                        if (old_dir==DOWN_RIGHT) {
                                          for (;;) {
                                            if ((new_dir==UP_LEFT)||(new_dir==UP_CENTER)||(new_dir==UP_RIGHT)||(new_dir==DOWN_LEFT)){break;}
                                            else{new_dir+=1;new_dir%=6;};
                                          };
                                        };

                                        if (old_dir==UP_LEFT) {
                                          for (;;) {
                                            if ((new_dir==UP_RIGHT)||(new_dir==DOWN_LEFT)||(new_dir==DOWN_CENTER)||(new_dir==DOWN_RIGHT)){break;}
                                            else{new_dir+=1;new_dir%=6;};
                                          };
                                        };

                                        if (old_dir==UP_CENTER) {
                                          for (;;) {
                                            if ((new_dir==DOWN_LEFT)||(new_dir==DOWN_CENTER)||(new_dir==DOWN_RIGHT)){break;}
                                            else{new_dir+=1;new_dir%=6;};
                                          };
                                        };

                                        if (old_dir==UP_RIGHT) {
                                          for (;;) {
                                            if ((new_dir==UP_LEFT)||(new_dir==DOWN_CENTER)||(new_dir==DOWN_RIGHT)||(new_dir==DOWN_LEFT)){break;}
                                            else {new_dir+=1;new_dir%=6;};
                                          };
                                        };
                                        //elog("returning new eye_dir\n");
                                          return new_dir;

                                      };

//smonstermonsters[0].cols[1]=black;
//s_monster_t monsters[NUM_MONSTERS];

//monsters[0].cols={black,black,black,black,black},

/*monsters[0].cols[0]=black;
monsters[0].cols[1]=yellow;
monsters[0].cols[2]=dark_yellow;
monsters[0].cols[3]=white;
monsters[0].cols[4]=black;
monsters[1].cols[0]=black;
monsters[1].cols[1]=yellow;
monsters[1].cols[2]=dark_yellow;
monsters[1].cols[3]=white;
monsters[1].cols[4]=black;
monsters[2].cols[0]=black;
monsters[2].cols[1]=yellow;
monsters[2].cols[2]=dark_yellow;
monsters[2].cols[3]=white;
monsters[2].cols[4]=black;
*/
