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

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "esp_chip_info.h"
#include "spi_flash_mmap.h"
#endif
#pragma once

const char* filenames900[9]={"cats.raw",
"sprites1.raw",
"sprites2.raw",
"sprites3.raw",
"sprites4.raw",
"sprites5.raw",
"sprites6.raw",
"sprites7.raw",
"sprites8.raw"};
const char* filenames300[276]={
"nes11.raw",
"col_3_6.raw",
"col_3_14b.raw",
"fishpair.raw",
"cut2.raw",
"col_5_1.raw",
"nes31.raw",
"nes48.raw",
"fishpair_.raw",
"col_4_17.raw",
"homer.raw",
"col_6_18.raw",
"cartoon_unknown2.raw",
"pacmanmonster1.raw",
"lisa.raw",
"col_3_6b.raw",
"col_5_17.raw",
"nes57.raw",
"nes79.raw",
"crab_cut2.raw",
"col_3_11.raw",
"nes55.raw",
"nes112.raw",
"nes80.raw",
"nes110.raw",
"nes54.raw",
"col_4_27.raw",
"homer_.raw",
"nes97.raw",
"nes28.raw",
"nes13.raw",
"col_7_24.raw",
"fish2.raw",
"seahorse_.raw",
"col_5_11.raw",
"flanders.raw",
"col_6_4.raw",
"floor.raw",
"nes18.raw",
"col_5_23.raw",
"nes63.raw",
"nes78.raw",
"col_6_2.raw",
"nes102.raw",
"col_3_31.raw",
"col_4_20.raw",
"nes69.raw",
"col_6_17.raw",
"col_4_39.raw",
"nes99.raw",
"aquariumfloor.raw",
"col_3_14.raw",
"cartoongirl.raw",
"col_6_7.raw",
"crab_cut1.raw",
"plantpair_.raw",
"nes2.raw",
"nes24.raw",
"nes92.raw",
"col_7_21.raw",
"nes59.raw",
"cut3.raw",
"simpsons_blackguy.raw",
"nes89.raw",
"fish3_raw",
"sheep.raw",
"nes46.raw",
"col_5_10.raw",
"nes77.raw",
"col_4_29.raw",
"mutantfish.raw",
"nes43.raw",
"col_5_7.raw",
"nes22.raw",
"nes95b.raw",
"nes6.raw",
"nes41.raw",
"nes65.raw",
"col_5_19.raw",
"nes74.raw",
"nes30.raw",
"pikachu.raw",
"col_5_9.raw",
"nes68.raw",
"col_5_15.raw",
"purpletentacle.raw",
"simsons_dog.raw",
"plantpair.raw",
"simpsons_unknown2.raw",
"col_5_18.raw",
"col_6_1.raw",
"nes21.raw",
"nes70.raw",
"col_5_28.raw",
"nes9.raw",
"nes88.raw",
"nes60.raw",
"col_5_13.raw",
"col_5_24.raw",
"col_7_1.raw",
"col_5_25.raw",
"nes82.raw",
"nes73.raw",
"nes16.raw",
"col_6_9.raw",
"nes36.raw",
"turtles3.raw",
"col_5_2.raw",
"nes66.raw",
"fish1.raw",
"col_3_11b.raw",
"nes25.raw",
"col_3_28.raw",
"col_7_14.raw",
"nes20.raw",
"cut0.raw",
"batman.raw",
"nes108.raw",
"turtles4.raw",
"nes4.raw",
"col_5_22.raw",
"maggie.raw",
"crab.raw",
"col_5_8.raw",
"nes109.raw",
"col_3_29.raw",
"crab_cut1",
"nes15.raw",
"pacmanmonster2.raw",
"col_7_28.raw",
"barney.raw",
"col_5_4.raw",
"col_5_21.raw",
"nes84.raw",
"nes34.raw",
"nes5.raw",
"nes29.raw",
"col_5_26.raw",
"fish1_.raw",
"nes81.raw",
"col_3_13.raw",
"nes47.raw",
"col_4_37.raw",
"nes53b.raw",
"fish2_.raw",
"nes49.raw",
"nes107.raw",
"col_5_14.raw",
"cartoon_unknown3.raw",
"col_5_30.raw",
"nes94.raw",
"nes96.raw",
"col_7_13.raw",
"nes51.raw",
"bart.raw",
"col_4_26.raw",
"nes104.raw",
"nes72.raw",
"nes1.raw",
"col_3_31b.raw",
"nes67.raw",
"col_3_27.raw",
"nes42.raw",
"col_5_12.raw",
"nes58.raw",
"urls.txt",
"nes23.raw",
"col_3_16.raw",
"nes40.raw",
"nes8.raw",
"nes32.raw",
"cartoon_unknown1.raw",
"col_7_8.raw",
"nes98.raw",
"nes44.raw",
"marge_.raw",
"krusty.raw",
"nes12.raw",
"col_5_5.raw",
"nes39.raw",
"nes55b.raw",
"nes7.raw",
"mutantfish_.raw",
"turtles2.raw",
"aquariumfloor.raw.1",
"nes87.raw",
"ballerina.raw",
"col_5_6.raw",
"nes100.raw",
"nes101.raw",
"pacmanmonster4.raw",
"kirby.raw",
"col_6_8.raw",
"nes75.raw",
"nes10.raw",
"col_6_10.raw",
"bart_.raw",
"yoshi.raw",
"nes76.raw",
"nes86.raw",
"nes35.raw",
"nes106.raw",
"nes75.rawwwwwwww",
"seahorse.raw",
"nes33.raw",
"nes83.raw",
"doublefish.raw",
"col_6_15.raw",
"col_6_16.raw",
"nes17.raw",
"nes53.raw",
"col_5_16.raw",
"nes52.raw",
"col_4_18.raw",
"nes45.raw",
"crab_cut3.raw",
"pacmanmonster3.raw",
"col_6_13.raw",
"nes103.raw",
"nes3.raw",
"col_6_12.raw",
"nes111.raw",
"crab_.raw",
"col_5_27.raw",
"megaman.raw",
"col_5_3.raw",
"simpsons_unknown1.raw",
"col_6_14.raw",
"turtles1.raw",
"nes50.raw",
"test.py",
"col_7_23.raw",
"col_6_6.raw",
"nes19.raw",
"nes38.raw",
"nes105.raw",
"col_5_29.raw",
"nes61.raw",
"nes54b.raw",
"marge.raw",
"nes91.raw",
"nes93.raw",
"nes14.raw",
"nes62.raw",
"nes27.raw",
"doubleplants.raw",
"oceanfloor.raw",
"crabeyesup.raw",
"nes56.raw",
"nes85.raw",
"cut1.raw",
"col_6_3.raw",
"simsons_cat.raw",
"nes26.raw",
"nes95.raw",
"greententacle.raw",
"col_5_20.raw",
"col_6_5.raw",
"nes37.raw",
"nes64.raw",
"col_6_11.raw",
"nes90.raw",
"col_6_19.raw",
"cartoon_unknown4.raw",
"col_6_20.raw",
"fish3.raw",
"nes71.raw"};

#define SPRITE_HOST "192.168.0.249"
#define SPRITE_PORT 8000
void get_sprites(){
  char log_str[256]={0};
  int sprite_sock;
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = PP_HTONS(SPRITE_PORT);
  addr.sin_addr.s_addr = inet_addr(SPRITE_HOST);

  sprite_sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
  lwip_connect(sprite_sock, (struct sockaddr*)&addr, sizeof(addr));
  if (sprite_sock>=0) {
    sprintf(log_str,"INFO : [get_sprites] Socket %d, target_addr %s, port %d\n",sprite_sock,SPRITE_HOST,SPRITE_PORT);
    //ESP_LOGI(TAG, "Socket %d, udp , target_addr %s, port %d",sock,SOCK_TARGET_HOST,SOCK_TARGET_PORT);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

  }else{
  elog("ERROR : Socket initalizing failed");
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
  };

  lwip_connect(sprite_sock, (struct sockaddr*)&addr, sizeof(addr));

  char tx_buf[128]={0};
  const char *file_name=filenames300[0];
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

  // Check if destination file exists before renaming


  memset(tx_buf,0,128);

  if (sprite_sock != -1) {
      elog("[INFO : [get_sprites] Shutting down socket\n");
          vTaskDelay(log_delay/ portTICK_PERIOD_MS);
      shutdown(sprite_sock, 0);
      close(sprite_sock);

  }
};
