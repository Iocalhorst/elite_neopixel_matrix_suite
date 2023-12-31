//abstraction layer between the framebuffer provider and the led driver library
//TODO : implement gamma correction brightness control, color correction
#pragma once

#include "elite.h"
#include "elite_pixel_formats.h"
#include "elite_led_driver.h"
#include "elite_display_settings_presets.h"
#include "elite_pixel_game_ente.h"
#include "freertos/stream_buffer.h"
#include "math.h"


#define INPUT_FRAMEBUF_PIXFORMAT sfRGB
#define OUTPUT_PIXFORMAT sRGB

StreamBufferHandle_t mr_displays_global_inputstream_handle;

SemaphoreHandle_t displaySemaphore;
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
   float fgamma;
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
    INPUT_FRAMEBUF_PIXFORMAT* p_tmp_framebuf;
    size_t p_tmp_framebuf_size;
    OUTPUT_PIXFORMAT output_framebuf[300];
    int udp_state;
}elite_display_t;

#define UDP_UNINITIALIZED 0
#define UDP_START 1
#define UDP_SOCK_RDY 2
#define UDP_SOCK_FAIL 3
#define UDP_CONN_OK 4
#define UDP_CONN_FAIL 5
#define UDP_STOP 6
#define UDP_RUNNING 42
#define UDP_SEND_FAIL 69


elite_display_t *mr_displays_global_handle;

elite_display_t* elite_display_construct(elite_display_config_t* conf);
void elite_display_update_leds(elite_display_t* self);
void elite_display_putpixelRGB8(elite_display_t* self,int x,int y,OUTPUT_PIXFORMAT c);
void elite_display_apply_color_correction(elite_display_t* self);
void elite_display_apply_brightness(elite_display_t* self);
void elite_display_apply_gamma_correction(elite_display_t* self);
void elite_display_prepare_output_framebuf(elite_display_t* self);
void elite_display_load_parameter_set(elite_display_t* self,display_settings_t* settings);
void elite_display_set_named_parameter(elite_display_t* self,const char* name,float value);


void elite_display_prepare_output_framebuf(elite_display_t* self);
void elite_display_task(void* pv_params);



elite_display_t* elite_display_construct(elite_display_config_t *conf){

    elite_display_t* self=(elite_display_t*)e_mall0c(__FUNCTION__,sizeof(elite_display_t));
    self->conf.width=conf->width;
    self->conf.height=conf->height;
    self->conf.color_correction_r=conf->color_correction_r;
    self->conf.color_correction_g=conf->color_correction_g;
    self->conf.color_correction_b=conf->color_correction_b;
    self->conf.brightness=conf->brightness;
    self->conf.fgamma=conf->fgamma;
    self->conf.fps=conf->fps;
    self->p_input_framebuf_size=sizeof(INPUT_FRAMEBUF_PIXFORMAT)*self->conf.height*self->conf.width;
    self->p_input_framebuf=e_mall0c(__FUNCTION__,self->p_input_framebuf_size);
    //self->p_output_framebuf=(OUTPUT_PIXFORMAT*)e_mall0c(__FUNCTION__,sizeof(OUTPUT_PIXFORMAT)*self->conf.height*self->conf.width);
    for (size_t i=0;i<self->conf.height*self->conf.width;i++) {
        INPUT_FRAMEBUF_PIXFORMAT c={0.0f,0.0f,0.0f};
        self->p_input_framebuf[i]=c;
        OUTPUT_PIXFORMAT c_out={0.0f,0.0f,0.0f};
        self->output_framebuf[i]=c_out;
    };

    mr_displays_global_inputstream_handle=xStreamBufferCreate(self->p_input_framebuf_size,self->p_input_framebuf_size);
    if (displaySemaphore==NULL) displaySemaphore=xSemaphoreCreateMutex();
    return self;
};

static bool stop_drawing=false;

void elite_display_putpixelRGB8(elite_display_t* self,int x,int y,OUTPUT_PIXFORMAT cRGB8){

    if (stop_drawing) return;
    if (eliteAssert(x>=0&&x<self->conf.width&&y>=0&&y<self->conf.height,"you've hurt the canvas - game over")) {stop_drawing=true;return;};
    int pixel_index=x+self->conf.width*y;
    INPUT_FRAMEBUF_PIXFORMAT cfRGB;
    cfRGB.fr=cRGB8.r;
    cfRGB.fg=cRGB8.g;
    cfRGB.fb=cRGB8.b;
    self->p_input_framebuf[pixel_index]=cfRGB;

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
        c.fr=255.0f*pow(c.fr/255.0f,self->conf.fgamma);
        c.fg=255.0f*pow(c.fg/255.0f,self->conf.fgamma);
        c.fb=255.0f*pow(c.fb/255.0f,self->conf.fgamma);
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

void elite_display_print_settings(elite_display_t* p_display){

    ELOG("INFO : [elite_display_] brightness : %f gamma: %f\n",p_display->conf.brightness,p_display->conf.fgamma);


};

int num_frame=0;
bool elite_display_send_output_framebuf_udp(elite_display_t* self,int sockfd){
  //char tmp[16]={0};
//  sprintf(tmp,"num_frame=%i\n",num_frame);
  //int r=send(sockfd,tmp,16,0);
//  if (r<16) {return false;};
//  num_frame++;
//  return true;*/
  int r=send(sockfd,self->output_framebuf,900,0);
  if (r!=900) return false;
  return true;
};

void elite_display_udp_start(elite_display_t *self){
  self->udp_state=UDP_START;
};

void elite_display_udp_stop(elite_display_t *self){
  self->udp_state=UDP_STOP;
};

void elite_display_task(void* pv_params){
    elite_display_t* self=(elite_display_t*)pv_params;

    elite_display_print_settings(self);
int sockfd=-1;
self->udp_state=UDP_UNINITIALIZED;
struct sockaddr_in udp_display_addr;
memset(&udp_display_addr, 0, sizeof(udp_display_addr));
udp_display_addr.sin_len = sizeof(udp_display_addr);
udp_display_addr.sin_family = AF_INET;
udp_display_addr.sin_port = PP_HTONS(9003);
udp_display_addr.sin_addr.s_addr = inet_addr(SOCK_TARGET_HOST);

    while (true) {
        if (elite_theres_a_pixel_game_running==true) {
            xStreamBufferReceive(mr_displays_global_inputstream_handle,self->p_input_framebuf,self->p_input_framebuf_size,1000/portTICK_PERIOD_MS);
        };


            //    elite_display_apply_color_correction(self);
        if (self->udp_state==UDP_START) {
          ELOG("DEBUG : [elite_display_task] udp_start\n");

            sockfd = lwip_socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd<0) {
              self->udp_state=UDP_SOCK_FAIL;
              ELOG("ERROR : [elite_display_task] lwip_socket() fail\n");

              }
            else {
              ELOG("DEBUG : [elite_display_task] lwip_socket() success\n");

              self->udp_state=UDP_SOCK_RDY;
            };
        };
        if (self->udp_state==UDP_SOCK_RDY){
            if (lwip_connect(sockfd, (struct sockaddr*)&udp_display_addr, sizeof(udp_display_addr))==0) {
                self->udp_state=UDP_CONN_OK;
                ELOG("DEBUG : [elite_display_task] sockfd connect() success\n");

                ELOG("DEBUG : [elite_display_task] udp_running\n");

            }else{
                ELOG("DEBUG : [elite_display_task] sockfd connect() fail\n");

                self->udp_state=UDP_CONN_FAIL;
            };
        };
        if (self->udp_state==UDP_CONN_OK) {
            self->udp_state=UDP_RUNNING;
            ELOG("DEBUG : [elite_display_task] udp_state==running\n");

        };


        xSemaphoreTake(displaySemaphore,portMAX_DELAY);

        if (self->udp_state==UDP_RUNNING) {
            elite_display_prepare_output_framebuf(self);
            if (elite_display_send_output_framebuf_udp(self,sockfd)!=true) {
                self->udp_state=UDP_SEND_FAIL;
                ELOG("ERROR : [elite_display_task] udp_send_fail\n");

            };
        };

// Todo: implement subcontext for different framebuffer output pipelines.


        elite_display_apply_brightness(self);
        elite_display_apply_gamma_correction(self);
        elite_display_prepare_output_framebuf(self);

        elite_display_update_leds(self);
        xSemaphoreGive(displaySemaphore);

        vTaskDelay(5/portTICK_PERIOD_MS);//it sucks but things behave weird and asynchronously
    };
    vTaskDelete(NULL);
};

void elite_display_brightness_down(elite_display_t* self){
    self->conf.brightness-=0.01f;
    if (self->conf.brightness<=0.00f) {
        self->conf.brightness=0.00f;
    };
    elite_display_print_settings(self);
};

void elite_display_brightness_up(elite_display_t* self){
    self->conf.brightness+=0.01f;
    if (self->conf.brightness>=1.0f) {
        self->conf.brightness=1.0f;
    };
    elite_display_print_settings(self);
};


void elite_display_gamma_up(elite_display_t* self){
    self->conf.fgamma+=0.01f;
    if (self->conf.fgamma>=2.0f) {
        self->conf.fgamma=2.0f;
    };
    elite_display_print_settings(self);
};

void elite_display_gamma_down(elite_display_t* self){
    self->conf.fgamma-=0.01f;
    if (self->conf.fgamma<=0.1f) {
        self->conf.fgamma=0.1f;
    };
    elite_display_print_settings(self);
};

elite_display_t* elite_display_create_default(){

      elite_display_config_t elite_display_config={
            .brightness=display_settings_default_high.brightness,
            .fgamma=display_settings_default_high.fgamma,
            .color_correction_r=display_settings_default_high.col_correction_coeffs[0],
            .color_correction_g=display_settings_default_high.col_correction_coeffs[1],
            .color_correction_b=display_settings_default_high.col_correction_coeffs[2],
            .height=30,
            .width=10,
            .fps=24
      };

      elite_display_t* mr_display=elite_display_construct(&elite_display_config);
      xTaskCreate(&elite_display_task, "elite_display_task", 4096,mr_display, 5, NULL);

      return mr_display;
};
