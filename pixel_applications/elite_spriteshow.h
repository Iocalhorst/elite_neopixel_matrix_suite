//header only elite_pixel_game_ente bare minimum template example application
#include "elite_pixel_game_ente.h"
#include "elite.h"
#include "main.h"
#pragma once
#include "elite_spriteshow_assets.h"
//since on-demand loading of resources from flash or external locations is noticably time-consuming(seconds maybe even),
//instead of fetching resources "just-in-time" - at the end of the cyle (after fading out the last resources)
//we want to pre-fetch "early" - specifically during the hold-time of the current resource
//this avoids lags/delays when swapping the content and fading back in again when using alpha-blending for transitioning
//and works in favor of (possible) spatial transitioning effects
#define STATE_ERROR -1
#define STATE_INITIAL_HOLD 0 //we use this state to offset the individual container timings a little
#define STATE_FADE_IN 1
#define STATE_HOLD 2
#define STATE_FETCH_NEXT 3
#define STATE_FADE_OUT 4
#define STATE_SWAP 5


typedef struct{
  int container_state;
  int _current_resource_index;
  int _next_resource_index;
  elite_sprite_t *_p_current_sprite;
  elite_sprite_t *_p_next_sprite;
  int _pos_x;
  int _pos_y;
  float _sprite_offset_fx;
  float _sprite_offset_fy;
  float _falpha_modulator;
  float _fx_modulator;
  float _fy_modulator;
  bool has_been_reloaded;
  float state_timer;
  float initial_hold_time;
  float hold_time,fade_in_time,fade_out_time;
}sprite_container_t;

typedef struct {
  char* app_name;
  uint16_t num_resources;
  char **list_resource_locations;
  uint8_t num_sprite_containers;
  sprite_container_t *p_sprite_containers;
  elite_sprite_t *test_sprite;
}spriteshow_t;

static bool sum_flag=true;
bool spriteshow_on_user_update_entered_log=false;
bool spriteshow_on_user_update_leaving_log=false;
bool spriteshow_on_user_update_pre_particle_shower_update_log=false;



bool sprite_container_sum_entered_log=false;
bool sprite_container_sum_leaving_log=false;
bool sprite_container_sum(sprite_container_t *self,elite_pixel_game_t *ente){

    //trace in
    if (sprite_container_sum_entered_log==false) {
        sprite_container_sum_entered_log=true;
        elog("DEBUG : [sprite_container_sum] entered sprite_container_sum() - once\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
    }

    //debug_block
    if (self==NULL||ente==NULL) {
        elog("DEBUG : [sprite_container_sum] @param *self==nullptr\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };
    if (self->_p_current_sprite==NULL) {
        elog("DEBUG : [sprite_container_sum] self->_p_current_sprite==nullptr\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };
    if (self->_p_current_sprite->load_failed==true) {
        elog("DEBUG : [sprite_container_sum] self->load_failed==true\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };
    if (self->_p_current_sprite->height==0||self->_p_current_sprite->width==0) {
        elog("DEBUG : [sprite_container_sum] _p_current_sprite->height==0||self->_p_current_sprite->width==0\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };
    if (self->_p_current_sprite->_p_bitmap==NULL) {
        elog("DEBUG : [sprite_container_sum] _p_bitmap==nullptr\b");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };


    uint16_t r_sum=0;
    uint16_t g_sum=0;
    uint16_t b_sum=0;


    for (size_t i=0;i<10;i++) {//self->_p_current_sprite->height;
        for (size_t j=0;j<10;j++) {//self->_p_current_sprite->height;
            r_sum+=self->_p_current_sprite->_p_bitmap[i*10+j].r;
            g_sum+=self->_p_current_sprite->_p_bitmap[i*10+j].g;
            b_sum+=self->_p_current_sprite->_p_bitmap[i*10+j].b;

            /*c_tmp.fr=25.0f;
            c_tmp.fg=127.0f;
            c_tmp.fb=25.0f;*/
            //c_tmp.fa=255.0f;
            //c_tmp.fa=255.0f*self->_falpha_modulator;
            //TODO : update _sprite_offset_fx,_sprite_offset_fx on some lovely sunny day
            //int x=self->_pos_x+(int16_t)self->_sprite_offset_fx+j;
        //    int y=self->_pos_y+(int16_t)self->_sprite_offset_fy+i;
            //one last boundary check
            //if (x>=0&&x<ente->screen_width&&y>=0&&y<ente->screen_height){
                //finally the time has come that we draw some pixels ... hopefully

          //      elite_pixel_game_fputpixelRGBA(ente,x,y,c_tmp);
            //    elite_pixel_game_fputpixel(ente,x,y,c_tmp);
            };
        };
      char log_str[256]={0};
      sprintf(log_str,"DEBUG : [sprite_container_sum] sum(_p_current_sprite->bitmap[].r = %ul | .g = %ul | .b = %ul\n",r_sum,g_sum,b_sum);
      elog(log_str);
      vTaskDelay(log_delay/portTICK_PERIOD_MS);


    if (sprite_container_sum_leaving_log==false) {
        sprite_container_sum_leaving_log=true;
        elog("DEBUG : [sprite_container_draw] leaving sprite_container_sum() - once\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
    }
    return true;
};


bool sprite_container_update(sprite_container_t *self,float fElapsedTime){
    switch (self->container_state) {
        case STATE_INITIAL_HOLD : {
            self->_falpha_modulator=0.0f;
            self->state_timer-=fElapsedTime;
            if (self->state_timer<=0.0f) {
                self->state_timer=self->fade_in_time;
                self->container_state=STATE_FADE_IN;
                elog("DEBUG : [spriteshow_container_update] INITIAL_HOLD->FADE_IN\n");
                vTaskDelay(log_delay/portTICK_PERIOD_MS);
            //self->_fx_modulator=0.0f;
            //self->_fy_modulator=0.0f;
            };
            break;
        };
        case STATE_FADE_IN : {
            self->state_timer-=fElapsedTime;
            self->_falpha_modulator=(self->fade_in_time-self->state_timer)/self->fade_in_time;

            if (self->state_timer<=0.0f) {
              //  self->_falpha_modulator=1.0f;
                self->state_timer=self->hold_time;
                self->container_state=STATE_HOLD;
                self->has_been_reloaded=false;
                elog("DEBUG : [spriteshow_container_update] FADE_IN->HOLD\n");
                vTaskDelay(log_delay/portTICK_PERIOD_MS);
                elog("DEBUG : [spriteshow_container_update] has_been_reloaded=false\n");
                vTaskDelay(log_delay/portTICK_PERIOD_MS);

            };
            break;
        };

        case STATE_HOLD : {
            self->state_timer-=fElapsedTime;
            //self->current_sprite_falpha=1.0f;
            if (self->state_timer<=0.0f) {
                self->state_timer=self->fade_out_time;
                self->container_state=STATE_FADE_OUT;
                elog("DEBUG : [spriteshow_container_update] HOLD->FADE_OUT\n");
                vTaskDelay(log_delay/portTICK_PERIOD_MS);
              //
              //We do not (asynchronously) retrieve self->p_next_sprite in here!
              //
              //reason :      retrieval might cause blocking calls(sockets,flash memory reads etc) when resource aquisition is performed
              //              and cause pending/"fading" containers to hang/lag
              //workaround :  we do assing p_next_sprite synchronously in the on_user_update for all containers at once,
              //              ensuring that all containers are on hold at this point
              //note :        proper time span tolerances must be taken into account
              //              this workaround can be removed if non-blocking resources retrieval is implemented
              //todo :        implement non blocking resource aquisition
            };
            break;
        };

        case STATE_FADE_OUT : {
            self->state_timer-=fElapsedTime;
            self->_falpha_modulator=1.0f-((self->fade_in_time-self->state_timer)/self->fade_in_time);
            if (self->state_timer<=0.0f) {
              //  self->_falpha_modulator=0.0f;//clamping needed cause coefficient might turn out to be negativ
                self->state_timer=self->fade_in_time;
                self->container_state=STATE_SWAP;
                elog("DEBUG : [spriteshow_container_update] FADE_OUT->SWAP\n");
                vTaskDelay(log_delay/portTICK_PERIOD_MS);


            };
            break;
        };
        case STATE_SWAP : {
                  //  self->_falpha_modulator=0.0f;//clamping needed cause coefficient might turn out to be negativ

                vTaskDelay(log_delay/portTICK_PERIOD_MS);
                if (self->_p_next_sprite!=NULL&&self->_p_next_sprite->load_failed==false) {
                    //elog("DEBUG : [spriteshow_container_update] memcpy(self->_p_current_sprite,self->_p_next_sprite,sizeof(elite_sprite_t);\n");
                    elog("DEBUG : [spriteshow_container_update] SWAP!\n");
                    vTaskDelay(log_delay/portTICK_PERIOD_MS);
                    memcpy(self->_p_current_sprite->_p_bitmap,self->_p_next_sprite->_p_bitmap,sizeof(sRGB)*100);
                };
                self->container_state=STATE_FADE_IN;
                elog("DEBUG : [spriteshow_container_update] FADE_OUT->FADE_IN\n");
            break;
        };


        case STATE_ERROR : {
            break;
        };

        default : {
          elog("DEBUG : [spriteshow_container_update] STATE ERROR\n");
          vTaskDelay(log_delay/portTICK_PERIOD_MS);

            self->container_state=STATE_ERROR;
            elog("ERROR : [elite_sprite_container_update] unreachable");
            return false;
            break;
        };
    };
    if (self->_falpha_modulator<=0.0f) {self->_falpha_modulator=0.0f;};
    if (self->_falpha_modulator>=1.0f) {self->_falpha_modulator=1.0f;};
    return true;
};
bool sprite_container_draw_entered_log=false;
bool sprite_container_draw_leaving_log=false;
bool sprite_container_draw(sprite_container_t *self,elite_pixel_game_t *ente){

    //trace in
    if (sprite_container_draw_entered_log==false) {
        sprite_container_draw_entered_log=true;
        elog("DEBUG : [sprite_container_draw] entered sprite_container_draw() - once\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
    }
    //debug_block
    if (self==NULL||ente==NULL) {
        elog("DEBUG : [sprite_container_draw] @param *self==nullptr\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };
    if (self->_p_current_sprite==NULL) {
        elog("DEBUG : [sprite_container_draw] self->_p_current_sprite==nullptr\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };
    if (self->_p_current_sprite->load_failed==true) {
        elog("DEBUG : [sprite_container_draw] self->load_failed==true\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };
    if (self->_p_current_sprite->height==0||self->_p_current_sprite->width==0) {
        elog("DEBUG : [sprite_container_draw] _p_current_sprite->height==0||self->_p_current_sprite->width==0\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };
    if (self->_p_current_sprite->_p_bitmap==NULL) {
        elog("DEBUG : [sprite_container_draw] _p_bitmap==nullptr\b");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
    };




    for (size_t i=0;i<self->_p_current_sprite->height;i++) {
        for (size_t j=0;j<self->_p_current_sprite->width;j++) {
            sRGB pixel=self->_p_current_sprite->_p_bitmap[i*10+j];
            if (pixel.r+pixel.g+pixel.b==0) continue;
            sfRGBA c_tmp;
            c_tmp.fr=pixel.r;
            c_tmp.fg=pixel.g;
            c_tmp.fb=pixel.b;
            c_tmp.fa=255.0f;

            //c_tmp.fa=255.0f;
            c_tmp.fa*=self->_falpha_modulator;//*self->_falpha_modulator;
            //TODO : update _sprite_offset_fx,_sprite_offset_fx on some lovely sunny day
            int x=self->_pos_x+(int16_t)self->_sprite_offset_fx+j;
            int y=self->_pos_y+(int16_t)self->_sprite_offset_fy+i;
            //one last boundary check
            if (x>=0&&x<ente->screen_width&&y>=0&&y<ente->screen_height){
                //finally the time has come that we draw some pixels ... hopefully

          //      elite_pixel_game_fputpixelRGBA(ente,x,y,c_tmp);
                elite_pixel_game_fputpixelRGBA(ente,x,y,c_tmp);
            };
        };
    };

    if (sprite_container_draw_leaving_log==false) {
        sprite_container_draw_leaving_log=true;
        elog("DEBUG : [sprite_container_draw] leaving sprite_container_draw() - once\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
    }
    return true;
};

spriteshow_t* spriteshow_construct(elite_pixel_game_t* ente){

    elog("INFO : [spriteshow_construct] entering spriteshow_construct()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elog("INFO : [spriteshow_construct] allocating self(spriteshow_t)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    spriteshow_t *self=malloc(sizeof(spriteshow_t));

    if (self!=NULL) {
        elog("INFO : [spriteshow_construct] successfully allocated self(spriteshow_t)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else {
        elog("ERROR : [spriteshow_construct] failed to allocated self(spriteshow_t); returning NULL from constructor\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        return NULL;
    };



    //initializer block self

    self->app_name="spriteshow";
    self->num_resources=NUM_URLS;

    self->list_resource_locations = malloc(NUM_URLS * sizeof(char*));

    //self->resource_locations=url_list;
    for (size_t r=0;r<NUM_URLS;r++) {
        self->list_resource_locations[r] = malloc(64 * sizeof(char));
    };
    for (size_t r=0;r<NUM_URLS;r++) {
        self->list_resource_locations[r] = malloc(64 * sizeof(char));
        self->list_resource_locations[r] = url_list[r];

        //sprintf(self->list_resource_locations[r],"/littlefs/%s",url_list[r]);
    //    self->resource_locations[r]=calloc(strlen(full_path),sizeof(char));
        //TODO : do some strncat thingy to play it a little safer
        //strcpy(self->list_resource_locations[r],full_path);//danger will robinson!
    };
    self->num_sprite_containers=3;
    self->p_sprite_containers=(sprite_container_t*)malloc(sizeof(sprite_container_t)*self->num_sprite_containers);
    if (self->p_sprite_containers==NULL) {
        elog("ERROR : [elite_spriteshow_construct] self->p_sprite_containers==NULL\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);

      //todo error message+handling
        return NULL;
    };

    elog("INFO : [sprite_show_construct] copied url_list to self->list_resource_locations\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    for (int i=0;i<self->num_sprite_containers;i++){

        self->p_sprite_containers[i].container_state=STATE_INITIAL_HOLD;
        self->p_sprite_containers[i].initial_hold_time=0.60f*((float)i+1.0f);
        self->p_sprite_containers[i].fade_in_time=0.95f;
        self->p_sprite_containers[i].hold_time=15.0f;
        self->p_sprite_containers[i].fade_out_time=0.75f;
        self->p_sprite_containers[i].has_been_reloaded=true;//false;//we'll come back to that right next
        self->p_sprite_containers[i]._current_resource_index=i;
        self->p_sprite_containers[i]._next_resource_index=i+3;
        self->p_sprite_containers[i]._pos_x=0;
        self->p_sprite_containers[i]._pos_y=10*i;
        self->p_sprite_containers[i]._falpha_modulator=1.0f;
        self->p_sprite_containers[i]._fy_modulator=0.0f;
        self->p_sprite_containers[i]._fx_modulator=0.0f;
        self->p_sprite_containers[i]._sprite_offset_fx=0.0f;
        self->p_sprite_containers[i]._sprite_offset_fy=0.0f;
        self->p_sprite_containers[i].state_timer=self->p_sprite_containers[i].initial_hold_time;

        elite_sprite_config_t sprite_config={
            .width=10,
            .height=10,
            .load_immediatly=true,
            .url={0}
        };
        int ri=self->p_sprite_containers[i]._current_resource_index;
        strncpy(sprite_config.url,self->list_resource_locations[ri],strlen(self->list_resource_locations[ri]));
        self->p_sprite_containers[i]._p_current_sprite=elite_sprite_construct(sprite_config);
      //  self->p_sprite_containers[i]._p_next_sprite=elite_sprite_construct(sprite_config);

        if (self->p_sprite_containers[i]._p_current_sprite==NULL) {
            elog("ERROR : [spriteshow_construct] elite_sprite_construct(sprite_config) failed\n");
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
        }else{
            elog("INFO : [spriteshow_construct] elite_sprite_construct(sprite_config) returned elite_sprite_t object\n");
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
            if (self->p_sprite_containers[i]._p_current_sprite->_p_bitmap==NULL) {
                elog("ERROR : [spriteshow_construct] elite_sprite_construct() _._p_current_sprite->_p_bitmap==nullptr\n");
                vTaskDelay(log_delay*2 / portTICK_PERIOD_MS);
            }else{
                char log_str[128]={0};
                uint32_t checksum=0l;
                for (int j=0;j<100;j++) {
                  checksum+=self->p_sprite_containers[i]._p_current_sprite->_p_bitmap[j].r;
                  checksum+=self->p_sprite_containers[i]._p_current_sprite->_p_bitmap[j].g;
                  checksum+=self->p_sprite_containers[i]._p_current_sprite->_p_bitmap[j].b;
                  };
                sprintf(log_str,"DEBUG : [elite_sprite_load] checksum=%lu\n",checksum);
                elog(log_str);
                vTaskDelay(log_delay/portTICK_PERIOD_MS);
                elog("INFO : [spriteshow_construct] elite_sprite_construct() _._p_current_sprite->_p_bitmap!=nullptr\n");
                vTaskDelay(log_delay*2 / portTICK_PERIOD_MS);
            };
        };


        elite_sprite_config_t next_sprite_config={
            .width=10,
            .height=10,
            .load_immediatly=true,
            .url={0}
        };

        int ri2=(self->p_sprite_containers[i]._current_resource_index)%self->num_resources;
        strncpy(next_sprite_config.url,self->list_resource_locations[ri2],strlen(self->list_resource_locations[ri2]));
        self->p_sprite_containers[i]._p_next_sprite=elite_sprite_construct(next_sprite_config);
        if (self->p_sprite_containers[i]._p_next_sprite==NULL) {
            elog("ERROR : [spriteshow_construct] elite_sprite_construct(next_sprite_config) failed\n");
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
        }else{
            elog("INFO : [spriteshow_construct] elite_sprite_construct(next_sprite_config) returned elite_sprite_t object\n");
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
            if (self->p_sprite_containers[i]._p_next_sprite->_p_bitmap==NULL) {
                elog("ERROR : [spriteshow_construct] elite_sprite_construct() _._p_next_sprite->_p_bitmap==nullptr\n");
                vTaskDelay(log_delay*2 / portTICK_PERIOD_MS);
            }else{
                char log_str[128]={0};
                uint32_t checksum=0l;
                for (int j=0;j<100;j++) {
                  checksum+=self->p_sprite_containers[i]._p_next_sprite->_p_bitmap[j].r;
                  checksum+=self->p_sprite_containers[i]._p_next_sprite->_p_bitmap[j].g;
                  checksum+=self->p_sprite_containers[i]._p_next_sprite->_p_bitmap[j].b;
                  };
                sprintf(log_str,"DEBUG : [elite_sprite_load] checksum=%lu\n",checksum);
                elog(log_str);
                vTaskDelay(log_delay/portTICK_PERIOD_MS);
                elog("INFO : [spriteshow_construct] elite_sprite_construct() _._p_next_sprite->_p_bitmap!=nullptr\n");
                vTaskDelay(log_delay*2 / portTICK_PERIOD_MS);
            };
        };
    };
        elog("INFO : [spriteshow_construct] successfully constructed self(spriteshow_t); returning self from constructor\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        return self;
};

bool spriteshow_on_user_update(void* params,elite_pixel_game_t *ente,float fElapsedTime){
//debug tracing in
    if (spriteshow_on_user_update_entered_log==false) {
        spriteshow_on_user_update_entered_log=true;
        elog("INFO : [spriteshow_update] entered spriteshow_update() - this notification will only occur once\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };


//body
    spriteshow_t *self=(spriteshow_t*)params;
    elite_pixel_game_set_target_layer(ente,0);
    sfRGBA c={4.0f,16.0f,4.0f,255.0f};
    elite_pixel_game_fill_flayerRGBA(ente,c);

    /*for (int y=0;y<self->test_sprite->height;y++) {
        for (int x=0;x<self->test_sprite->width;x++) {
            sRGB c=self->test_sprite->p_bitmap[i++];
            if (c.r>0||c.b>0||c.r>0) {
                sfRGB cf={c.r,c.g,c.b};
                elite_pixel_game_fputpixel(ente,x,y+10,cf);
            };
        };
    };*/

    for (int i=0;i<self->num_sprite_containers;i++) {
        sprite_container_update(&self->p_sprite_containers[i],fElapsedTime);
    };

    int hold_count=0;
    for (int i=0;i<self->num_sprite_containers;i++){
      if (self->p_sprite_containers[i].container_state==STATE_HOLD) hold_count+=1;
    }

    if (hold_count==self->num_sprite_containers) {
        for (int i=0;i<self->num_sprite_containers;i++) {
            if (self->p_sprite_containers[i].container_state==STATE_HOLD&&self->p_sprite_containers[i].has_been_reloaded==false) {

                self->p_sprite_containers[i]._next_resource_index+=3;
                self->p_sprite_containers[i]._next_resource_index%=self->num_resources;

                elog("DEBUG: [spriteshow_on_user_update] loading new _p_next_sprite into container\n");
                vTaskDelay(log_delay / portTICK_PERIOD_MS);
            //advance the url_list index
                elog("DEBUG : [spriteshow_on_user_update] current_resource_index++\n");
                vTaskDelay(log_delay / portTICK_PERIOD_MS);
            //wrap it
            //clear the old sprite url char buffer
                elog("DEBUG : [spriteshow_on_user_update] clearing old url\n");
                vTaskDelay(log_delay / portTICK_PERIOD_MS);
            //for (int i=0;i<128;i++){self->p_sprite_containers[i]._p_next_sprite->url[i]='\0';}

            //readability
                int ri=self->p_sprite_containers[i]._next_resource_index;
                int len=strlen(self->list_resource_locations[ri]);
            //copy the new path_str into the sprite struct url buffer;
                elog("DEBUG : [spriteshow_on_user_update] strncopy(new_url)\n");
                vTaskDelay(log_delay / portTICK_PERIOD_MS);
                strncpy(self->p_sprite_containers[i]._p_next_sprite->url,self->list_resource_locations[ri],len);
                self->p_sprite_containers[i]._p_next_sprite->url[len]=0;
            //and now lets see if the sprite can load new data from the new url
                elog("DEBUG : [spriteshow_on_user_update] elite_sprite_load\n");
                vTaskDelay(log_delay / portTICK_PERIOD_MS);
                if (elite_sprite_load(self->p_sprite_containers[i]._p_next_sprite)==true) {
                //yay :-)
                    elog("INFO : [spriteshow_on_user_update] elite_sprite_load(p_next_sprite) succeeded\n");
                    vTaskDelay(log_delay / portTICK_PERIOD_MS);
                    self->p_sprite_containers[i]._current_resource_index=self->p_sprite_containers[i]._next_resource_index;

                }else{
                //we messed up :-(
                    self->p_sprite_containers[i].container_state=STATE_ERROR;
                    elog("ERROR : [spriteshow_on_user_update] elite_sprite_load(p_next_sprite) failed\n");
                    vTaskDelay(log_delay / portTICK_PERIOD_MS);
                };
                self->p_sprite_containers[i].has_been_reloaded=true;
            };
        };
    };


//   now the rendering partä
    if (sum_flag==true) {
        elog("DEBUG : [spriteshow_on_user_update] sum_flag set\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        for (int i=0;i<self->num_sprite_containers;i++) {
            if (self->p_sprite_containers[i].container_state!=STATE_ERROR) {
                sprite_container_sum(&self->p_sprite_containers[i],ente);
            }else {
                elog("ERROR : [spriteshow_on_user_update] container_state==STATE_ERROR\n");
                vTaskDelay(log_delay / portTICK_PERIOD_MS);
            }
        };
        sum_flag=false;
        elog("DEBUG : [spriteshow_on_user_update] unsetting sum_flag\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };
  for (int i=0;i<self->num_sprite_containers;i++) {
      if (self->p_sprite_containers[i].container_state!=STATE_ERROR) {
          sprite_container_draw(&self->p_sprite_containers[i],ente);
      };
  };

  //debug tracing out

  if (spriteshow_on_user_update_leaving_log==false) {
      spriteshow_on_user_update_leaving_log=true;
      elog("INFO : [spriteshow_on_user_update] leaving spriteshow_on_user_update() - this notification will only occur once\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };

  return true;

};


bool spriteshow_on_user_destroy(void* params){

    elog("INFO : [spriteshow_on_user_update] entering spriteshow_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    spriteshow_t *self=(spriteshow_t*)params;
    elog("INFO : [spriteshow_on_user_update] deallocating self(spriteshow)\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    if (self!=NULL) {
        free(self);
        elog("INFO : [spriteshow_on_user_update] successfully deallocated self(spriteshow)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    }else {
        elog("ERROR : [spriteshow_on_user_update] failed to deallocated self(spriteshow); returning false from spriteshow_on_user_destroy()\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        return false;
    };
    elog("INFO : [spriteshow_on_user_update] returning true from template_on_user_destroy()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
  return true;

};



void spriteshow_start_task(){

    elog("INFO : [main_start_pixel_game_task] entered main_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elog("INFO : [main_start_pixel_game_task] creating &pixel_game_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    elite_pixel_game_config_t pixel_game_config={
        .app_name="spriteshow",
        .screen_width=10,
        .screen_height=30,
        .on_user_construct=(void*)&spriteshow_construct,
        .on_user_update=&spriteshow_on_user_update,
        .on_user_destroy=&spriteshow_on_user_destroy
    };
    xTaskCreate(&elite_pixel_game_task, "elite_pixel_game_task", 4096,&pixel_game_config, 5, NULL);
    elog("INFO : [main_start_pixelapp_task] leaving main_start_pixelapp_task\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

};
