#include "defines.h"
#include "esp_err.h"
#include "esp_flash.h"
#include "esp_idf_version.h"
#include "esp_spiffs.h"
#include "esp_littlefs.h"
#include "esp_system.h"
#include "esp_vfs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <dirent.h>
#include "defines.h"
#include "elite_spriteshow_assets.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "esp_chip_info.h"
#include "spi_flash_mmap.h"
#endif
#pragma once

#define ASSET_SERVER_PORT 8000
void get_sprite(int i){
  char log_str[256]={0};
  int sprite_sock;
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = PP_HTONS(ASSET_SERVER_PORT);
  addr.sin_addr.s_addr = inet_addr(SOCK_TARGET_HOST);

  sprite_sock = lwip_socket(AF_INET, SOCK_STREAM, 0);

  lwip_connect(sprite_sock, (struct sockaddr*)&addr, sizeof(addr));
  if (sprite_sock>=0) {
    sprintf(log_str,"INFO : [get_sprites] Socket %d, target_addr %s, port %d\n",sprite_sock,SOCK_TARGET_HOST,ASSET_SERVER_PORT);
    //ESP_LOGI(TAG, "Socket %d, udp , target_addr %s, port %d",sock,SOCK_TARGET_HOST,SOCK_TARGET_PORT);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

  }else{
  elog("ERROR : Socket initalizing failed");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };

  lwip_connect(sprite_sock, (struct sockaddr*)&addr, sizeof(addr));

  char tx_buf[128]={0};
  const char *file_name=url_list[i];
  sprintf(tx_buf,"GET /%s\n\n",file_name);
  size_t tx_len=strlen(tx_buf);
  send(sprite_sock,tx_buf,tx_len,0);
  char rx_buf[300]={0};
  int bytes_received = recv(sprite_sock, rx_buf, sizeof(rx_buf), 0);
  sprintf(log_str,"INFO : [get_sprites] received %i bytes\n",bytes_received);
  //ESP_LOGI(TAG, "Socket %d, udp , target_addr %s, port %d",sock,SOCK_TARGET_HOST,SOCK_TARGET_PORT);
  elog(log_str);
  vTaskDelay(log_delay / portTICK_PERIOD_MS);

  char file_path[128]={0};
  const char *base_path="/littlefs";
  sprintf(file_path,"%s/%s",base_path,file_name);
  struct stat st;
  if (stat(file_path, &st) == 0) {
          // Delete it if it exists
          unlink(file_path);
  }
  FILE *f = fopen(file_path, "w");
  if (f == NULL) {
          sprintf(log_str,"ERROR : [get_sprites] Failed to open file <%s> for writing\n",file_path);
          elog(log_str);
          vTaskDelay(log_delay/ portTICK_PERIOD_MS);
          return;
  }
  int bytes_written=fwrite(rx_buf,1,sizeof(rx_buf),f);
  fclose(f);
  sprintf(log_str,"INFO : [get_sprites] written %d bytes from %d bytes received to <%s/%s> written\n",bytes_written,bytes_received,base_path,file_name);
  elog(log_str);

  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  const char* close_msg="\n\n\0";
    send(sprite_sock,close_msg,sizeof(close_msg),0);
  // Check if destination file exists before renaming


  memset(tx_buf,0,128);

  if (sprite_sock != -1) {
      elog("[INFO : [get_sprites] Shutting down socket\n");
          vTaskDelay(log_delay/ portTICK_PERIOD_MS);
      shutdown(sprite_sock, 0);
      close(sprite_sock);

  };

};
