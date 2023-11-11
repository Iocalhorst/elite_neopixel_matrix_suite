#pragma once

typedef enum {
  RGB8_RAW,
  RGBA8_RAW,
  FLOAT32_RGB,
  FLOAT32_RGBA,
  CHAR_INDEXED_COLORS,
}pixel_data_format_t;


#define PALETTE_COLOR_FMT sfRGBA

//the sprites_update funtion must be called by the engine.
//that requires
//p_current_frame

typedef struct {
  uint16_t width,height,num_frames;
  char url[128];
  bool load_failed;
  PALETTE_COLOR_FMT indexed_colors[16];
  void* *pixel_data;
  pixel_data_format_t fmt;
  sfRGBA p_current_frame;
  float frame_duration;
}elite_animated_sprite_t;






typedef struct {
  uint16_t width,height;
  bool load_immediatly;
  char url[128];
}elite_sprite_config_t;

typedef struct {
  uint16_t width,height;
  char url[128];
  bool load_immediatly;
  bool load_failed;
  sRGB *_p_bitmap;
}elite_sprite_t;



//todo : int elite_err_t elite_sprite_load_bla();
bool elite_sprite_load(elite_sprite_t *self){
  if (self==NULL) return false;
  if (self->load_failed) return false;
  char log_str[256]={0};
  char file_path[138]={0};
  const char *base_path="/littlefs";
  sprintf(file_path,"%s/%s",base_path,self->url);
  FILE *f=fopen(file_path,"r");
  if (f==NULL) {
    //todo : verbose error message : invalid filename, file doesnt exist, readError
    sprintf(log_str,"ERROR : [elite_sprite_load] could not open <%s> for reading\n",file_path);
    elog(log_str);
    vTaskDelay(log_delay/portTICK_PERIOD_MS);
  return false;
}else {
  //todo : verbose error message : invalid filename, file doesnt exist, readError
  sprintf(log_str,"INFO : [elite_sprite_load] opened <%s> for reading\n",file_path);
  elog(log_str);
  vTaskDelay(log_delay/portTICK_PERIOD_MS);

};

  if (self->_p_bitmap!=NULL) {
    elog("INFO : [elite_sprite_load] overwriting data at self->p_bitmap\n");
    vTaskDelay(log_delay/portTICK_PERIOD_MS);
    }else {
      size_t _p_bitmap_size=sizeof(sRGB)*self->height*self->width;
      self->_p_bitmap=(sRGB*)malloc(_p_bitmap_size);
      if (self->_p_bitmap==NULL) {
        elog("ERROR : [elite_sprite_load] malloc fail\n");
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        err_t fclose_ret=fclose(f);
        sprintf(log_str,"DEBUG : [elite_sprite_load] fclose() fclose_ret=%i\n",fclose_ret);
        elog(log_str);
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
        return false;
      }else{
        sprintf(log_str,"DEBUG : [elite_sprite_load] malloc(%dd)==%p success\n",_p_bitmap_size,(void *)&self->_p_bitmap);
        elog(log_str);
        vTaskDelay(log_delay/portTICK_PERIOD_MS);
      };
    };

  int num_bytes_read=fread(self->_p_bitmap,1,self->width*self->height*3,f);

  if (num_bytes_read!=self->width*self->height*3){
      free(self->_p_bitmap);
      elog("DEBUG : [elite_sprite_load] free(self->_p_bitmap)");
      fclose(f);
      elog("ERROR : [elite_sprite_load] fread() fail\n");
      vTaskDelay(log_delay/portTICK_PERIOD_MS);
      return false;
  }else {
      int fclose_ret=fclose(f);
      sprintf(log_str,"DEBUG : [elite_sprite_load] fclose() fclose_ret=%i\n",fclose_ret);
      elog(log_str);
      vTaskDelay(log_delay/portTICK_PERIOD_MS);
      sprintf(log_str,"DEBUG : [elite_sprite_load] fread()==%i bytes, which is a great success\n",num_bytes_read);
      elog(log_str);
      vTaskDelay(log_delay/portTICK_PERIOD_MS);
      uint32_t checksum=0l;
      for (int i=0;i<100;i++) {checksum+=self->_p_bitmap[i].r;checksum+=self->_p_bitmap[i].g;checksum+=self->_p_bitmap[i].b;};
      sprintf(log_str,"DEBUG : [elite_sprite_load] checksum=%lu\n",checksum);
      elog(log_str);
      vTaskDelay(log_delay/portTICK_PERIOD_MS);

  }




  elog("DEBUG : [elite_sprite_load] leaving elite_sprite_load, true\n");
  vTaskDelay(log_delay/portTICK_PERIOD_MS);
  return true;

};


elite_sprite_t *elite_sprite_construct(elite_sprite_config_t config){
    char log_str[256]={0};
    elog("INFO : [spriteshow_construct] entering elite_sprite_construct()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    elite_sprite_t *self=(elite_sprite_t*)malloc(sizeof(elite_sprite_t));
    memset(&self->url, 0, sizeof(self->url));
    strncpy(self->url,config.url,strlen(config.url));//lol

    self->height=config.height;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->height=%ul\n",self->height);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->width=config.width;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->width=%ul\n",self->width);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->load_immediatly=config.load_immediatly;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->load_immediatly=%b\n",self->load_immediatly);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->load_failed=false;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->load_failed=false\n");
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    self->_p_bitmap=NULL;
    sprintf(log_str,"DEBUG : [elite_sprite_construct] self->_p_bitmap=nullptr\n");
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    if (self->load_immediatly==true) {
        elog("DEBUG : [elit_sprite_construct] load_immediatly==true, calling elite_sprite_load(self)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        if (elite_sprite_load(self)) {
            sprintf(log_str,"INFO : [elite_sprite_construct] loaded sprite from file <%s> loaded\n",self->url);
            elog(log_str);
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
        }else {
            sprintf(log_str,"ERROR : [elite_sprite_construct] elite_sprite_load(self) failed\n");
            elog(log_str);
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
        };
    }else {
        elog("DEBUG : [elit_sprite_construct] load_immediatly==false, skipping elite_sprite_load(self)\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
    };

    //if (self->load_immediatly) self->load_failed=elite_sprite_load(self);
    elog("DEBUG : [elite_sprite_construct] leaving elite_sprite_construct()\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    return self;
};
