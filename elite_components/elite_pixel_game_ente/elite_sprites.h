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
  
  char file_path[138]={0};
  const char *base_path="/littlefs";
  sprintf(file_path,"%s/%s",base_path,self->url);
  FILE *f=fopen(file_path,"r");
  if (f==NULL) {
    //todo : verbose error message : invalid filename, file doesnt exist, readError
    ELOG("ERROR : [elite_sprite_load] could not open <%s> for reading\n",file_path);
    
    
  return false;
}else {
  //todo : verbose error message : invalid filename, file doesnt exist, readError
  ELOG("INFO : [elite_sprite_load] opened <%s> for reading\n",file_path);
  
  

};

  if (self->_p_bitmap!=NULL) {
    ELOG("INFO : [elite_sprite_load] overwriting data at self->p_bitmap\n");
    
    }else {
      size_t _p_bitmap_size=sizeof(sRGB)*self->height*self->width;
      self->_p_bitmap=(sRGB*)malloc(_p_bitmap_size);
      if (self->_p_bitmap==NULL) {
        ELOG("ERROR : [elite_sprite_load] malloc fail\n");
        
        err_t fclose_ret=fclose(f);
        ELOG("DEBUG : [elite_sprite_load] fclose() fclose_ret=%i\n",fclose_ret);
        
        
        return false;
      }else{
        ELOG("DEBUG : [elite_sprite_load] malloc(%dd)==%p success\n",_p_bitmap_size,(void *)&self->_p_bitmap);
        
        
      };
    };

  int num_bytes_read=fread(self->_p_bitmap,1,self->width*self->height*3,f);

  if (num_bytes_read!=self->width*self->height*3){
      free(self->_p_bitmap);
      ELOG("DEBUG : [elite_sprite_load] free(self->_p_bitmap)");
      fclose(f);
      ELOG("ERROR : [elite_sprite_load] fread() fail\n");
      
      return false;
  }else {
      int fclose_ret=fclose(f);
      ELOG("DEBUG : [elite_sprite_load] fclose() fclose_ret=%i\n",fclose_ret);
      
      
      ELOG("DEBUG : [elite_sprite_load] fread()==%i bytes, which is a great success\n",num_bytes_read);
      
      
      uint32_t checksum=0l;
      for (int i=0;i<100;i++) {checksum+=self->_p_bitmap[i].r;checksum+=self->_p_bitmap[i].g;checksum+=self->_p_bitmap[i].b;};
      ELOG("DEBUG : [elite_sprite_load] checksum=%lu\n",checksum);
      
      

  }




  ELOG("DEBUG : [elite_sprite_load] leaving elite_sprite_load, true\n");
  
  return true;

};


elite_sprite_t *elite_sprite_construct(elite_sprite_config_t config){
    
    ELOG("INFO : [spriteshow_construct] entering elite_sprite_construct()\n");
    

    elite_sprite_t *self=(elite_sprite_t*)malloc(sizeof(elite_sprite_t));
    memset(&self->url, 0, sizeof(self->url));
    strncpy(self->url,config.url,strlen(config.url));//lol

    self->height=config.height;
    ELOG("DEBUG : [elite_sprite_construct] self->height=%ul\n",self->height);
    
    
    self->width=config.width;
    ELOG("DEBUG : [elite_sprite_construct] self->width=%ul\n",self->width);
    
    
    self->load_immediatly=config.load_immediatly;
    ELOG("DEBUG : [elite_sprite_construct] self->load_immediatly=%b\n",self->load_immediatly);
    
    
    self->load_failed=false;
    ELOG("DEBUG : [elite_sprite_construct] self->load_failed=false\n");
    
    
    self->_p_bitmap=NULL;
    ELOG("DEBUG : [elite_sprite_construct] self->_p_bitmap=nullptr\n");
    
    

    if (self->load_immediatly==true) {
        ELOG("DEBUG : [elit_sprite_construct] load_immediatly==true, calling elite_sprite_load(self)\n");
        
        if (elite_sprite_load(self)) {
            ELOG("INFO : [elite_sprite_construct] loaded sprite from file <%s> loaded\n",self->url);
            
            
        }else {
            ELOG("ERROR : [elite_sprite_construct] elite_sprite_load(self) failed\n");
            
            
        };
    }else {
        ELOG("DEBUG : [elit_sprite_construct] load_immediatly==false, skipping elite_sprite_load(self)\n");
        
    };

    //if (self->load_immediatly) self->load_failed=elite_sprite_load(self);
    ELOG("DEBUG : [elite_sprite_construct] leaving elite_sprite_construct()\n");
    

    return self;
};
