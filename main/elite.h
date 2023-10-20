//pretty hackish collection of copy paste type boilerplate stuff
//nothing to see here - go away

#pragma once
#include "defines.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "esp_idf_version.h"
#include "esp_flash.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "esp_chip_info.h"
#include "spi_flash_mmap.h"
#endif


#include "esp_littlefs.h"
static const int log_delay=150;
esp_vfs_littlefs_conf_t conf = {
    .base_path = "/littlefs",
    .partition_label = "littlefs",
    .format_if_mount_failed = true,
    .dont_mount = false,
};

bool eliteAssert(bool ok,const char* msg);
TaskHandle_t p_elite_logger_task_handle;

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static size_t global_log_buffer_size=1024;
static char* global_log_buffer=NULL;

static void elog(const char* s);


static void elite_wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data){
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


static void elite_init_wifi(){
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

        s_wifi_event_group = xEventGroupCreate();

        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;

        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &elite_wifi_event_handler,
                                                            NULL,
                                                            &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &elite_wifi_event_handler,
                                                            NULL,
                                                            &instance_got_ip));

        wifi_config_t wifi_config = {
            .sta = {
                .ssid = EXAMPLE_ESP_WIFI_SSID,
                .password = EXAMPLE_ESP_WIFI_PASS,
                .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
                .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
                .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
            },
        };
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        ESP_ERROR_CHECK(esp_wifi_start() );

        ESP_LOGI(TAG, "wifi_init_sta finished.");

            EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                  WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                  pdFALSE,
                  pdFALSE,
                  portMAX_DELAY);

            if (bits & WIFI_CONNECTED_BIT) {
                ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                        EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
            } else if (bits & WIFI_FAIL_BIT) {
                ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                     EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
                   } else {
                     ESP_LOGE(TAG, "UNEXPECTED EVENT");
                   }

};


static void elite_init_nvs(){
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

};




typedef struct {
  int log_socket;
  int intervall;
  char* message;
} elite_logger_task_params_t;


static void elite_logger_task(void* args){
  (void)args;
  elite_logger_task_params_t p_my_params;
  (void)p_my_params;
  ESP_LOGI(TAG, "Initializing Logger Socket");
  int sock;
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = PP_HTONS(SOCK_TARGET_PORT);
  addr.sin_addr.s_addr = inet_addr(SOCK_TARGET_HOST);

  sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);

  if (sock>=0) {
    ESP_LOGI(TAG, "Socket %d, udp , target_addr %s, port %d",sock,SOCK_TARGET_HOST,SOCK_TARGET_PORT);
  }else ESP_LOGE(TAG, "Socket initalizing failed");
  lwip_connect(sock, (struct sockaddr*)&addr, sizeof(addr));

  global_log_buffer=(char*)malloc(global_log_buffer_size*sizeof(char));
  uint32_t ulNotificationValue;
  size_t max_tx_buf=768;
  char* tx_buf=(char*)malloc(max_tx_buf);
  memset(tx_buf,0,max_tx_buf);
  bool elite_logger_exit_condition=false;

  while (!elite_logger_exit_condition) {
      if (xTaskNotifyWait(0, 0, &ulNotificationValue, portMAX_DELAY)) {
        switch(ulNotificationValue){
            case 0 : {
              elite_logger_exit_condition=true;
              break;
            };
            default :{
              sprintf(tx_buf,"log4joy : %s",global_log_buffer);
              size_t tx_len=strlen(tx_buf);
              send(sock,tx_buf,tx_len,0);
              memset(tx_buf,0,max_tx_buf);
              ulNotificationValue=0;
            };
        };

      };
  };
  free(tx_buf);
  if (sock != -1) {


      elog("INFO : [main] Shutting down log_socket\n");
          vTaskDelay(500 / portTICK_PERIOD_MS);
      ESP_LOGE(TAG, "Shutting down log_socket\n");
      shutdown(sock, 0);
      close(sock);

  }
  vTaskDelete(NULL);
};




void elog(const char* s){

//        size_t *ps=(char)malloc(strlen(s);
configASSERT(global_log_buffer);
size_t i;
memset(global_log_buffer,0,global_log_buffer_size);
for (i=0;((i<strlen(s))&&(i<global_log_buffer_size-1));i++) {global_log_buffer[i]=s[i];};
global_log_buffer[i]=0;//not sure if thats needed but it wont hurt. it could hurt otherwise though
xTaskNotify(p_elite_logger_task_handle,i,3);//3==eSetValueWithOverwrite

};

bool eliteAssert(bool ok,const char* msg){
  if (!ok) {elog(msg);};
  return !ok;
};



bool elite_init_littlefs(){
  esp_err_t ret = esp_vfs_littlefs_register(&conf);
  if (ret==ESP_OK) return true;
  if (ret != ESP_OK) {
          if (ret == ESP_FAIL)
          {
                  elog("ERROR : [elite_init_littlefs] Failed to mount or format filesystem\n");
          }
          else if (ret == ESP_ERR_NOT_FOUND)
          {
                  elog("ERROR : [elite_init_littlefs] Failed to find LittleFS partition\n");
          }
          else
          {
                  elog("ERROR : [elite_init_littlefs] Failed to initialize LittleFS\n");
          }
          return false;
  }
  return true;
};
void elite_test_little_fs(){
  size_t total = 0, used = 0;int ret;
  ret = esp_littlefs_info(conf.partition_label, &total, &used);
  if (ret != ESP_OK)
  {
    //  ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s)", esp_err_to_name(ret));
      elog("ERROR : [elite_test_little_fs] Failed to get LittleFS partition information\n");
      vTaskDelay(log_delay / portTICK_PERIOD_MS);
  }
          else
          {
            char log_str[512]={0};
            sprintf(log_str,"INFO : [elite_test_little_fs] Partition size: total: %d, used: %d\n", total, used);
            elog(log_str);
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
          }

          // Use POSIX and C standard library functions to work with files.
          // First create a file.
          elog("INFO : [elite_test_little_fs] Opening file\n");
          FILE *f = fopen("/littlefs/hello.txt", "w");
          if (f == NULL)
          {
                  elog("ERROR :[elite_test_little_fs] Failed to open file for writing\n");
                  return;
          }
          fprintf(f, "Hier koennte ihre Werbung stehen!\n");
          fclose(f);
          elog("INFO : [elite_test_little_fs] File written\n");
          vTaskDelay(log_delay / portTICK_PERIOD_MS);

          // Check if destination file exists before renaming
          struct stat st;
          if (stat("/littlefs/foo.txt", &st) == 0)
          {
                  // Delete it if it exists
                  unlink("/littlefs/foo.txt");
          }

          // Rename original file
          elog("INFO : [elite_test_little_fs] Renaming file\n");
          vTaskDelay(log_delay / portTICK_PERIOD_MS);
          if (rename("/littlefs/hello.txt", "/littlefs/foo.txt") != 0)
          {
                  elog("ERROR : [elite_test_little_fs] Rename failed\n");
                  vTaskDelay(log_delay / portTICK_PERIOD_MS);
                  return;
          }

          // Open renamed file for reading
          elog("INFO : [elite_test_little_fs] Reading file\n");
          vTaskDelay(log_delay / portTICK_PERIOD_MS);
          f = fopen("/littlefs/foo.txt", "r");
          if (f == NULL)
          {
                  elog("ERROR : [elite_test_little_fs] Failed to open file for reading\n");
                  vTaskDelay(log_delay / portTICK_PERIOD_MS);
                  return;
          }

          char line[128];
          char *pos;

          fgets(line, sizeof(line), f);
          fclose(f);
          // strip newline
          pos = strchr(line, '\n');
          if (pos)
          {
                  *pos = '\0';
          }

          char log_str2[512];
          sprintf(log_str2,"INFO : [elite_test_little_fs] Read from file : %s\n", line);
          elog(log_str2);
          vTaskDelay(log_delay / portTICK_PERIOD_MS);
          esp_vfs_littlefs_unregister(conf.partition_label);
          elog("INFO : [elite_test_little_fs] LittleFS unmounted");
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
          /*elog("INFO : [elite_test_little_fs] Reading from flashed filesystem example.txt");
          f = fopen("/littlefs/example.txt", "r");
          if (f == NULL)
          {
                  ESP_LOGE(TAG, "Failed to open file for reading");
                  return;
          }
          fgets(line, sizeof(line), f);
          fclose(f);
          // strip newline
          pos = strchr(line, '\n');
          if (pos)
          {
                  *pos = '\0';
          }
          ESP_LOGI(TAG, "Read from file: '%s'", line);
          */
          // All done, unmount partition and disable LittleFS

}
