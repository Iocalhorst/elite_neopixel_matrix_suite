#pragma once
#include "defines.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool eliteAssert(bool ok,const char* msg);

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
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

static size_t global_log_buffer_size=1024;
static char* global_log_buffer=NULL;
static int n_main_loops_limit=100000;
//static int counter=0;


static bool exit_condition=false;



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

#define TEST_LOG_CMD 1

static void elite_logger_task(void* args){

  elite_logger_task_params_t p_my_params;
  p_my_params.message="moin";

  ESP_LOGI(TAG, "Initializing Logger Socket");
  int s;
  int ret;
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = PP_HTONS(SOCK_TARGET_PORT);
  addr.sin_addr.s_addr = inet_addr(SOCK_TARGET_HOST);

  s = lwip_socket(AF_INET, SOCK_DGRAM, 0);

  if (s>=0) {
    ESP_LOGI(TAG, "Socket %d, udp , target_addr %s, port %d",s,SOCK_TARGET_HOST,SOCK_TARGET_PORT);
  }else ESP_LOGE(TAG, "Socket initalizing failed");

//  ESP_LOGI(TAG, "Testing connect()");

  ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
  if (ret!=0) {ESP_LOGE(TAG, "Connect didnt work: errno %d ret %d", errno,ret);}
  else ESP_LOGD(TAG, "Connect worked: errno %d ret %d", errno,ret);
  LWIP_ASSERT("connect ret == 0", ret==0);

  ESP_LOGI(TAG, "Testing write()");
  const char* m="hello again!\n";
  ret = lwip_write(s, m, sizeof(m));
  //if (ret<14) {ESP_LOGE(TAG, "write didnt work: errno %d ret %d", errno,ret);}
  //else ESP_LOGD(TAG, "write() worked: errno %d ret %d", errno,ret);
  //LWIP_ASSERT("write ret == 13", ret >= 14);
//  FILE *fp = fdopen(s, "w+");
//  fprintf(fp,"lalala socket: %d",s);

  //ESP_LOGI(TAG, "Testing close()");

/*  ret = lwip_close(s);
  if (ret!=0) {ESP_LOGE(TAG, "close() didnt work: errno %d ret %d", errno,ret);}
  else ESP_LOGI(TAG, "close() worked: errno %d ret %d", errno,ret);
  LWIP_ASSERT("close ret == 0", ret == 0);
*/


global_log_buffer=(char*)malloc(global_log_buffer_size*sizeof(char));

   uint32_t ulNotificationValue;
   int num=0;
  while (true) {
    //ESP_LOGI("elitelogger", "messageStr=%s",p_my_params.message);
      if (xTaskNotifyWait(0, 0, &ulNotificationValue, portMAX_DELAY)) {
        switch (ulNotificationValue) {

          case TEST_LOG_CMD : {
              //ESP_LOGI(TAG, "tx_buf : <%s>",tx_buf);
              size_t max_tx_buf=64;
              char* tx_buf=(char*)malloc(max_tx_buf);
              memset(tx_buf,0,max_tx_buf);
              sprintf(tx_buf,"log4joy : %s %d\n",p_my_params.message,num);
              num++;



              size_t tx_len=strlen(tx_buf);
              ret=lwip_write(s,tx_buf,tx_len);

              if (ret!=tx_len) {ESP_LOGE(TAG, "failed to send something via the udp log socket : %s", tx_buf);}
              else {ESP_LOGI(TAG, "send something via the udp log socket : %s",tx_buf);};

              free(tx_buf);

              ulNotificationValue=0;
              break;
              };
        default :{


            size_t max_tx_buf=768;
            char* tx_buf=(char*)malloc(max_tx_buf);
            memset(tx_buf,0,max_tx_buf);

            sprintf(tx_buf,"log4joy : %s",global_log_buffer);

            //this is silly because we pass the message length in ulNotificationValue. why use strlen again?!
            size_t tx_len=strlen(tx_buf);

            ret=lwip_write(s,tx_buf,tx_len);
            LWIP_ASSERT("ret==lwip_write(s,tx_buf,tx_len);", ret==tx_len);

            free(tx_buf);
            //ret=lwip_write(s,globalLogBuffer,ulNotificationValue);
            ulNotificationValue=0;
            //vTaskDelay(100 / portTICK_PERIOD_MS);
          };
      };//switch
    };
  //  vTaskDelay(100 / portTICK_PERIOD_MS);
};


  (void)p_my_params;
  (void)args;
};

TaskHandle_t p_elite_logger_task_handle;


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
