#pragma once
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>



typedef struct{
    int posx;
    int posy;
    int buttonl;
    int buttonr;
}elite_mouse_t;


typedef struct{
    int posx;
    int posy;
    int buttonl;
    int buttonr;
}elite_mouse_conf_t;


elite_mouse_t* mr_mouse_global_handle;
int mouse_error=0;

elite_mouse_t* elite_mouse_construct(elite_mouse_conf_t* elite_mouse_conf){
    ELOG("DEBUG [elite_mouse_construc] enter \n");


    elite_mouse_t* self=(elite_mouse_t*)malloc(sizeof(elite_mouse_t*));
    self->posx=elite_mouse_conf->posx;
    self->posy=elite_mouse_conf->posy;
    self->buttonl=elite_mouse_conf->buttonl;
    self->buttonr=elite_mouse_conf->buttonr;
    ELOG("DEBUG [elite_mouse_construct] leaving\n");


    return self;
};


//temporary
void elite_mouse_draw_cursor(){
  if ((mr_mouse_global_handle==NULL)||(mr_displays_global_handle==NULL)||(mouse_error!=0)) return;
  sRGB c={172,172,172};
  int x=mr_mouse_global_handle->posx;
  int y=mr_mouse_global_handle->posy;
  if ((x<10)&&(x>=0)&&(y>=0)&&(y<30)) elite_display_putpixelRGB8(mr_displays_global_handle,x,y,c);
  if ((x+1<10)&&(x+1>=0)&&(y>=0)&&(y<30)) elite_display_putpixelRGB8(mr_displays_global_handle,x+1,y,c);
  if ((x<10)&&(x>=0)&&(y+1>=0)&&(y+1<30)) elite_display_putpixelRGB8(mr_displays_global_handle,x,y+1,c);
};


//this function is supposed to
//create a socket
//throw an error on creation fail
//loop over
//poll the input udp socket for data
//throw an error on polling
//parse that data
//throw an error on invalid data
//update the state of the global_mouse_handle accordingly

void elite_mouse_task(void* vp_params){
  ELOG("DEBUG [elite_mouse_task] enter\n");


  struct pollfd *iofds;
  const int nfds=1;
  const int mouse_socket_idx=0;
  iofds = calloc(nfds, sizeof(struct pollfd));
  iofds[mouse_socket_idx].fd=lwip_socket(AF_INET,SOCK_DGRAM,0);
  iofds[mouse_socket_idx].events = POLLIN;

  if (iofds[mouse_socket_idx].fd<0) {
    ELOG("DEBUG [elite_mouse_task] mouse socket create fail\n");

  }else{
    ELOG("DEBUG [elite_mouse_task] mouse socket create success\n");

  };

  ELOG("DEBUG [elite_mouse_task] before sockaddr gibberish\n");


  //char addr_str[128]={0};

  struct sockaddr_storage dest_addr;
  struct sockaddr_in *dest_addr_ip = (struct sockaddr_in *)&dest_addr;
  dest_addr_ip->sin_addr.s_addr = htonl(INADDR_ANY);
  dest_addr_ip->sin_family = AF_INET;
  dest_addr_ip->sin_port = htons(9004);

  ELOG("DEBUG [elite_mouse_task] setting S0L_SOCKET, S0_REUSEADDR)\n");


  int opt = 1;
  setsockopt(iofds[mouse_socket_idx].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  ELOG("DEBUG [elite_mouse_task] before bind \n");



  int err = bind(iofds[mouse_socket_idx].fd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err < 0) {

    ELOG("DEBUG [elite_mouse_task] mouse socket bind fail : errno==%i\n",errno);


  }else{
      ELOG("DEBUG [elite_mouse_task] mouse socket bind success\n");

  };

  mr_mouse_global_handle->posx=5;
  mr_mouse_global_handle->posy=5;
  mr_mouse_global_handle->buttonl=0;
  mr_mouse_global_handle->buttonr=0;
  if (err==0) {
      while (true){

          if (mr_mouse_global_handle!=NULL) {
          //poll(socket);
              char rx_buffer[32]={0};
              int len = recv(iofds[mouse_socket_idx].fd, rx_buffer, sizeof(rx_buffer) - 1, 0);
              NEVERMIND(len);
              if (strlen(rx_buffer)==strlen("x=xxx,y=yyy")) {
                int mx=0;
                mx+=((rx_buffer[2]-'0')*100);
                mx+=((rx_buffer[3]-'0')*10);
                mx+=((rx_buffer[4]-'0'));
                int my=0;
                my+=((rx_buffer[8]-'0')*100);
                my+=((rx_buffer[9]-'0')*10);
                my+=((rx_buffer[10]-'0'));
                if (mx>=0&&mx<10&&my>=0&&my<30) {
                  mr_mouse_global_handle->posx=mx;
                  mr_mouse_global_handle->posy=my;
                }else{
                  mr_mouse_global_handle->posx=5;
                  mr_mouse_global_handle->posy=5;
                  mr_mouse_global_handle->buttonl=0;
                  mr_mouse_global_handle->buttonr=0;
                };
              }else{
                rx_buffer[11]=0;

                ELOG("DEBUG : [elite_mouse_task] invalid packet <%s>\n",rx_buffer);


              };
          };
      };
  };
  if (iofds[mouse_socket_idx].fd>0) {
    ELOG("DEBUG [elite_mouse_task] closing mouse socket\n");

    close(iofds[mouse_socket_idx].fd);
  };

  ELOG("DEBUG [elite_mouse_task] exititing\n");

  vTaskDelete(NULL);
};


void elite_mouse_create_default(){

  elite_mouse_conf_t elite_mouse_default_conf={
      .posx=5,
      .posy=5,
      .buttonl=0,
      .buttonr=0,
  };


  mr_mouse_global_handle=elite_mouse_construct(&elite_mouse_default_conf);

  ELOG("DEBUG [elite_mouse_create_default] mr_mouse_global_handle=%p\n",(void*)mr_mouse_global_handle);


  ELOG("DEBUG [elite_mouse_create_default] creating mouse_task\n");


  xTaskCreate(&elite_mouse_task, "elite_mouse_task", 4096,mr_mouse_global_handle, 5, NULL);

  ELOG("DEBUG [elite_mouse_create_default] leaving\n");


};
