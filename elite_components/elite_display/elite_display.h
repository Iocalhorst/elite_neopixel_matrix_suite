//abstraction layer between the framebuffer provider and the led driver library
//TODO : implement gamma correction brightness control, color correction
#pragma once

#include "elite.h"
#include "elite_pixel_formats.h"
#include "elite_led_driver.h"
#include "elite_pixel_game_ente.h"
#include "freertos/stream_buffer.h"
#include "math.h"


#define INPUT_FRAMEBUF_PIXFORMAT sfRGB
#define OUTPUT_PIXFORMAT sRGB

StreamBufferHandle_t mr_displays_global_inputstream_handle;



led_strip_handle_t led_strip;

int yx2led[][10]={
   {299, 298, 297, 296, 295, 294, 293, 292, 291, 290},
   {280, 281, 282, 283, 284, 285, 286, 287, 288, 289},
   {279, 278, 277, 276, 275, 274, 273, 272, 271, 270},
   {260, 261, 262, 263, 264, 265, 266, 267, 268, 269},
   {259, 258, 257, 256, 255, 254, 253, 252, 251, 250},
   {240, 241, 242, 243, 244, 245, 246, 247, 248, 249},
   {239, 238, 237, 236, 235, 234, 233, 232, 231, 230},
   {220, 221, 222, 223, 224, 225, 226, 227, 228, 229},
   {219, 218, 217, 216, 215, 214, 213, 212, 211, 210},
   {200, 201, 202, 203, 204, 205, 206, 207, 208, 209},
   {199, 198, 197, 196, 195, 194, 193, 192, 191, 190},
   {180, 181, 182, 183, 184, 185, 186, 187, 188, 189},
   {179, 178, 177, 176, 175, 174, 173, 172, 171, 170},
   {160, 161, 162, 163, 164, 165, 166, 167, 168, 169},
   {159, 158, 157, 156, 155, 154, 153, 152, 151, 150},
   {140, 141, 142, 143, 144, 145, 146, 147, 148, 149},
   {139, 138, 137, 136, 135, 134, 133, 132, 131, 130},
   {120, 121, 122, 123, 124, 125, 126, 127, 128, 129},
   {119, 118, 117, 116, 115, 114, 113, 112, 111, 110},
   {100, 101, 102, 103, 104, 105, 106, 107, 108, 109},
   { 99,  98,  97,  96,  95,  94,  93,  92,  91,  90},
   { 80,  81,  82,  83,  84,  85,  86,  87,  88,  89},
   { 79,  78,  77,  76,  75,  74,  73,  72,  71,  70},
   { 60,  61,  62,  63,  64,  65,  66,  67,  68,  69},
   { 59,  58,  57,  56,  55,  54,  53,  52,  51,  50},
   { 40,  41,  42,  43,  44,  45,  46,  47,  48,  49},
   { 39,  38,  37,  36,  35,  34,  33,  32,  31,  30},
   { 20,  21,  22,  23,  24,  25,  26,  27,  28,  29},
   { 19,  18,  17,  16,  15,  14,  13,  12,  11,  10},
   {  0,   1,   2,   3,  4,    5,   6,   7,   8,   9} };






typedef struct{
   float brightness;
   float gamma_correction;
   float color_correction_r;
   float color_correction_g;
   float color_correction_b;
   int height;
   int width;
   float fps;
}elite_display_config_t;

typedef struct{
    elite_display_config_t conf;
    INPUT_FRAMEBUF_PIXFORMAT* p_input_framebuf;
    size_t p_input_framebuf_size;
    OUTPUT_PIXFORMAT output_framebuf[300];
}elite_display_t;

elite_display_t *mr_displays_global_handle;

elite_display_t* elite_display_construct(elite_display_config_t* conf);
void elite_display_update_leds(elite_display_t* self);
void elite_display_putpixel(elite_display_t* self,int x,int y,OUTPUT_PIXFORMAT c);
void elite_display_fetch_framebuf(elite_display_t* self);
void elite_display_apply_color_correction(elite_display_t* self);
void elite_display_apply_brightness(elite_display_t* self);
void elite_display_apply_gamma_correction(elite_display_t* self);
void elite_display_prepare_output_framebuf(elite_display_t* self);

void elite_display_prepare_output_framebuf(elite_display_t* self);
void elite_display_task(void* pv_params);



elite_display_t* elite_display_construct(elite_display_config_t *conf){

    elite_display_t* self=(elite_display_t*)malloc(sizeof(elite_display_t));
    self->conf.width=conf->width;
    self->conf.height=conf->height;
    self->conf.color_correction_r=conf->color_correction_r;
    self->conf.color_correction_g=conf->color_correction_g;
    self->conf.color_correction_b=conf->color_correction_b;
    self->conf.brightness=conf->brightness;
    self->conf.gamma_correction=conf->gamma_correction;
    self->conf.fps=conf->fps;
    self->p_input_framebuf_size=sizeof(INPUT_FRAMEBUF_PIXFORMAT)*self->conf.height*self->conf.width;
    self->p_input_framebuf=malloc(self->p_input_framebuf_size);
    //self->p_output_framebuf=(OUTPUT_PIXFORMAT*)malloc(sizeof(OUTPUT_PIXFORMAT)*self->conf.height*self->conf.width);
    for (size_t i=0;i<self->conf.height*self->conf.width;i++) {
        INPUT_FRAMEBUF_PIXFORMAT c={0.0f,0.0f,0.0f};
        self->p_input_framebuf[i]=c;
        OUTPUT_PIXFORMAT c_out={0.0f,0.0f,0.0f};
        self->output_framebuf[i]=c_out;
    };
    mr_displays_global_inputstream_handle=xStreamBufferCreate(self->p_input_framebuf_size,self->p_input_framebuf_size);
    return self;
};

static bool stop_drawing=false;

void elite_display_put_pixel(elite_display_t* self,int x,int y,OUTPUT_PIXFORMAT c){
  if (stop_drawing) return;
  if (eliteAssert(x>=0&&x<self->conf.width&&y>=0&&y<self->conf.height,"you've hurt the canvas - game over")) {stop_drawing=true;return;};
  int pixel_index=x+self->conf.width*y;
  INPUT_FRAMEBUF_PIXFORMAT c_;
  c_.fr=c.r;
  c_.fg=c.g;
  c_.fb=c.b;
  self->p_input_framebuf[pixel_index]=c_;
};


void elite_display_update_leds(elite_display_t* self){
    for (int yy=0;yy<self->conf.height;yy++){
        for (int xx=0;xx<self->conf.width;xx++){
            int ledNumber=yx2led[yy][self->conf.width-xx-1];//i flipped the xy2led map... oops thats why
            OUTPUT_PIXFORMAT c=self->output_framebuf[yy*self->conf.width+xx];
            ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, ledNumber, c.r,c.g,c.b));
        };
    };
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));

};

void elite_display_fetch_framebuf(elite_display_t* self){
  xStreamBufferReceive(mr_displays_global_inputstream_handle,
                         self->p_input_framebuf,
                         self->p_input_framebuf_size,
                         portMAX_DELAY);
};


void elite_display_apply_color_correction(elite_display_t* self){
    for (size_t i=0;i<self->conf.height*self->conf.width;i++) {
        self->p_input_framebuf[i].fr*=self->conf.color_correction_r;
        self->p_input_framebuf[i].fg*=self->conf.color_correction_g;
        self->p_input_framebuf[i].fb*=self->conf.color_correction_b;
      };
};

void elite_display_apply_brightness(elite_display_t* self){
    for (size_t i=0;i<self->conf.height*self->conf.width;i++) {
        self->p_input_framebuf[i].fr*=self->conf.brightness;
        self->p_input_framebuf[i].fg*=self->conf.brightness;
        self->p_input_framebuf[i].fb*=self->conf.brightness;

    };
};

void elite_display_apply_gamma_correction(elite_display_t* self){

        for (size_t i=0;i<self->conf.height*self->conf.width;i++) {
              INPUT_FRAMEBUF_PIXFORMAT c=self->p_input_framebuf[i];
              c.fr=255.0f*pow(c.fr/255.0f,self->conf.gamma_correction);
              c.fg=255.0f*pow(c.fg/255.0f,self->conf.gamma_correction);
              c.fb=255.0f*pow(c.fb/255.0f,self->conf.gamma_correction);
              if (c.fr<=0.0f) c.fr=0.0f;
              if (c.fg<=0.0f) c.fg=0.0f;
              if (c.fb<=0.0f) c.fb=0.0f;
              if (c.fr>=255.0f) c.fr=255.0f;
              if (c.fg>=255.0f) c.fg=255.0f;
              if (c.fb>=255.0f) c.fb=255.0f;
              self->p_input_framebuf[i]=c;
        };
};

void elite_display_prepare_output_framebuf(elite_display_t* self){
  OUTPUT_PIXFORMAT c_out;
  INPUT_FRAMEBUF_PIXFORMAT c_in;
  for (size_t i=0;i<self->conf.height*self->conf.width;i++) {
      c_in=self->p_input_framebuf[i];
      c_out.r=(uint8_t)c_in.fr;
      c_out.g=(uint8_t)c_in.fg;
      c_out.b=(uint8_t)c_in.fb;
      self->output_framebuf[i]=c_out;
  };

};
void elite_display_task(void* pv_params){
    elite_display_t* self=(elite_display_t*)pv_params;

    while (true) {

      if (elite_theres_a_pixel_game_running==true) xStreamBufferReceive(mr_displays_global_inputstream_handle,self->p_input_framebuf,self->p_input_framebuf_size,1000/portTICK_PERIOD_MS);
    //    elite_display_apply_color_correction(self);
        elite_display_apply_brightness(self);
        elite_display_apply_gamma_correction(self);
        elite_display_prepare_output_framebuf(self);
        elite_display_update_leds(self);
    vTaskDelay(20/portTICK_PERIOD_MS);
    };

};
void elite_display_brightness_down(elite_display_t* self){
    self->conf.brightness-=0.025f;
    if (self->conf.brightness<=0.00f) self->conf.brightness=0.00f;
};

void elite_display_brightness_up(elite_display_t* self){
    self->conf.brightness+=0.025f;
    if (self->conf.brightness>=1.0f) self->conf.brightness=1.0f;
};



void elite_display_gamma_up(elite_display_t* self){
  self->conf.gamma_correction+=0.025f;
  if (self->conf.gamma_correction>=2.0f) self->conf.gamma_correction=2.0f;
};

void elite_display_gamma_down(elite_display_t* self){
    self->conf.gamma_correction-=0.025f;
    if (self->conf.gamma_correction<=0.1f) self->conf.gamma_correction=0.1f;
};
elite_display_t* elite_display_create_default(){

      elite_display_config_t elite_display_config={
            .brightness=0.1f,
            .gamma_correction=0.85f,
            .color_correction_r=1.0f,
            .color_correction_g=1.0f,
            .color_correction_b=1.0f,
            .height=30,
            .width=10,
            .fps=24
      };

      elite_display_t* mr_display=elite_display_construct(&elite_display_config);

      xTaskCreate(&elite_display_task, "elite_display_task", 4096,mr_display, 5, NULL);
return mr_display;

};
