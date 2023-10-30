//pretty hackish collection of copy paste type boilerplate stuff
//nothing to see here - go away


//keep includes in alphabetical orders please
#pragma once
#include "defines.h"
#include "esp_err.h"
#include "esp_flash.h"
#include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_http_server.h"
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



static const int log_delay=25;
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
  size_t max_tx_buf=1000;
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
              sprintf(tx_buf,"log4joy [len=%i]: %s",strlen(global_log_buffer),global_log_buffer);
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
          vTaskDelay(log_delay / portTICK_PERIOD_MS);
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
  char log_str[256]={0};
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
  sprintf(log_str,"INFO : [elite_init_littlefs] mounted partition <%s> at <%s>\n",conf.partition_label,conf.base_path);
  elog(log_str);
  vTaskDelay(log_delay / portTICK_PERIOD_MS);
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
/* Max length a file path can have on storage */
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN)

/* Max size of an individual file. Make sure this
 * value is same as that set in upload_script.html */
#define MAX_FILE_SIZE   (200*1024) // 200 KB
#define MAX_FILE_SIZE_STR "200KB"

/* Scratch buffer size */
#define SCRATCH_BUFSIZE  8192

struct file_server_data {
    /* Base path of file storage */
    char base_path[ESP_VFS_PATH_MAX + 1];

    /* Scratch buffer for temporary storage during file transfer */
    char scratch[SCRATCH_BUFSIZE];
};

//static const char *TAG = "file_server";

/* Handler to redirect incoming GET request for /index.html to /
 * This can be overridden by uploading file with same name */
static esp_err_t index_html_get_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "307 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);  // Response body can be empty
    return ESP_OK;
}

/* Handler to respond with an icon file embedded in flash.
 * Browsers expect to GET website icon at URI /favicon.ico.
 * This can be overridden by uploading file with same name */
static esp_err_t favicon_get_handler(httpd_req_t *req)
{
    extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const unsigned char favicon_ico_end[]   asm("_binary_favicon_ico_end");
    const size_t favicon_ico_size = (favicon_ico_end - favicon_ico_start);
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_size);
    return ESP_OK;
}

/* Send HTTP response with a run-time generated html consisting of
 * a list of all files and folders under the requested path.
 * In case of SPIFFS this returns empty list when path is any
 * string other than '/', since SPIFFS doesn't support directories */
static esp_err_t http_resp_dir_html(httpd_req_t *req, const char *dirpath)
{
    char log_str[512]={0};
    char entrypath[FILE_PATH_MAX];
    char entrysize[16];
    const char *entrytype;

    struct dirent *entry;
    struct stat entry_stat;

    DIR *dir = opendir(dirpath);
    const size_t dirpath_len = strlen(dirpath);

    /* Retrieve the base path of file storage to construct the full path */
    strlcpy(entrypath, dirpath, sizeof(entrypath));

    if (!dir) {
        sprintf(log_str,"ERROR :[http_resp_dir_html] Failed to stat dir : %s\n", dirpath);
        elog(log_str);
        vTaskDelay(log_delay / portTICK_PERIOD_MS);

        /* Respond with 404 Not Found */
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Directory does not exist");
        return ESP_FAIL;
    }

    /* Send HTML file header */
    httpd_resp_sendstr_chunk(req, "<!DOCTYPE html><html><body>");

    /* Get handle to embedded file upload script */
    extern const unsigned char upload_script_start[] asm("_binary_upload_script_html_start");
    extern const unsigned char upload_script_end[]   asm("_binary_upload_script_html_end");
    const size_t upload_script_size = (upload_script_end - upload_script_start);
    /* Add file upload form and script which on execution sends a POST request to /upload */
    httpd_resp_send_chunk(req, (const char *)upload_script_start, upload_script_size);

    /* Send file-list table definition and column labels */
    httpd_resp_sendstr_chunk(req,
        "<table class=\"fixed\" border=\"1\">"
        "<col width=\"800px\" /><col width=\"300px\" /><col width=\"300px\" /><col width=\"100px\" />"
        "<thead><tr><th>Name</th><th>Type</th><th>Size (Bytes)</th><th>Delete</th></tr></thead>"
        "<tbody>");

    /* Iterate over all files / folders and fetch their names and sizes */

    while ((entry = readdir(dir)) != NULL) {
        entrytype = (entry->d_type == DT_DIR ? "directory" : "file");

        strlcpy(entrypath + dirpath_len, entry->d_name, sizeof(entrypath) - dirpath_len);
        if (stat(entrypath, &entry_stat) == -1) {

            sprintf(log_str,"ERROR :[http_resp_dir_html] Failed to stat %s : %s\n", entrytype, entry->d_name);
            elog(log_str);
            continue;
        }
        sprintf(entrysize, "%ld", entry_stat.st_size);
        sprintf(log_str,"INFO :[http_resp_dir_html] Found %s : %s (%s bytes)\n", entrytype, entry->d_name, entrysize);
        elog(log_str);
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        /* Send chunk of HTML file containing table entries with file name and size */
        httpd_resp_sendstr_chunk(req, "<tr><td><a href=\"");
        httpd_resp_sendstr_chunk(req, req->uri);
        httpd_resp_sendstr_chunk(req, entry->d_name);
        if (entry->d_type == DT_DIR) {
            httpd_resp_sendstr_chunk(req, "/");
        }
        httpd_resp_sendstr_chunk(req, "\">");
        httpd_resp_sendstr_chunk(req, entry->d_name);
        httpd_resp_sendstr_chunk(req, "</a></td><td>");
        httpd_resp_sendstr_chunk(req, entrytype);
        httpd_resp_sendstr_chunk(req, "</td><td>");
        httpd_resp_sendstr_chunk(req, entrysize);
        httpd_resp_sendstr_chunk(req, "</td><td>");
        httpd_resp_sendstr_chunk(req, "<form method=\"post\" action=\"/delete");
        httpd_resp_sendstr_chunk(req, req->uri);
        httpd_resp_sendstr_chunk(req, entry->d_name);
        httpd_resp_sendstr_chunk(req, "\"><button type=\"submit\">Delete</button></form>");
        httpd_resp_sendstr_chunk(req, "</td></tr>\n");
    }
    closedir(dir);

    /* Finish the file list table */
    httpd_resp_sendstr_chunk(req, "</tbody></table>");

    /* Send remaining chunk of HTML file to complete it */
    httpd_resp_sendstr_chunk(req, "</body></html>");

    /* Send empty chunk to signal HTTP response completion */
    httpd_resp_sendstr_chunk(req, NULL);
    return ESP_OK;
}

#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename)
{
    if (IS_FILE_EXT(filename, ".pdf")) {
        return httpd_resp_set_type(req, "application/pdf");
    } else if (IS_FILE_EXT(filename, ".html")) {
        return httpd_resp_set_type(req, "text/html");
    } else if (IS_FILE_EXT(filename, ".jpeg")) {
        return httpd_resp_set_type(req, "image/jpeg");
    } else if (IS_FILE_EXT(filename, ".ico")) {
        return httpd_resp_set_type(req, "image/x-icon");
    }
    /* This is a limited set only */
    /* For any other type always set as plain text */
    return httpd_resp_set_type(req, "text/plain");
}

/* Copies the full path into destination buffer and returns
 * pointer to path (skipping the preceding base path) */
static const char* get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
{
    const size_t base_pathlen = strlen(base_path);
    size_t pathlen = strlen(uri);

    const char *quest = strchr(uri, '?');
    if (quest) {
        pathlen = MIN(pathlen, quest - uri);
    }
    const char *hash = strchr(uri, '#');
    if (hash) {
        pathlen = MIN(pathlen, hash - uri);
    }

    if (base_pathlen + pathlen + 1 > destsize) {
        /* Full path string won't fit into destination buffer */
        return NULL;
    }

    /* Construct full path (base + path) */
    strcpy(dest, base_path);
    strlcpy(dest + base_pathlen, uri, pathlen + 1);

    /* Return pointer to path, skipping the base */
    return dest + base_pathlen;
}

/* Handler to download a file kept on the server */
static esp_err_t download_get_handler(httpd_req_t *req)
{   char log_str[256]={0};
    char filepath[FILE_PATH_MAX];
    FILE *fd = NULL;
    struct stat file_stat;

    const char *filename = get_path_from_uri(filepath, ((struct file_server_data *)req->user_ctx)->base_path,
                                             req->uri, sizeof(filepath));
    if (!filename) {
        elog("ERROR : [download_get_handler] Filename is too long\n");
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
        return ESP_FAIL;
    }

    /* If name has trailing '/', respond with directory contents */
    if (filename[strlen(filename) - 1] == '/') {
        return http_resp_dir_html(req, filepath);
    }

    if (stat(filepath, &file_stat) == -1) {
        /* If file not present on SPIFFS check if URI
         * corresponds to one of the hardcoded paths */
        if (strcmp(filename, "/index.html") == 0) {
            return index_html_get_handler(req);
        } else if (strcmp(filename, "/favicon.ico") == 0) {
            return favicon_get_handler(req);
        }
        sprintf(log_str,"ERROR : [download_get_handler] Failed to stat file : %s\n", filepath);
        elog(log_str);
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        /* Respond with 404 Not Found */
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File does not exist");
        return ESP_FAIL;
    }

    fd = fopen(filepath, "r");
    if (!fd) {
        sprintf(log_str,"ERROR : [download_get_handler] Failed to read existing file : %s\n", filepath);
        elog(log_str);
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    sprintf(log_str,"INFO : [download_get_handler] Sending file : %s (%ld bytes)\n", filename, file_stat.st_size);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    set_content_type_from_file(req, filename);

    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *chunk = ((struct file_server_data *)req->user_ctx)->scratch;
    size_t chunksize;
    do {
        /* Read file in chunks into the scratch buffer */
        chunksize = fread(chunk, 1, SCRATCH_BUFSIZE, fd);

        if (chunksize > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK) {
                fclose(fd);
                elog("ERROR : [download_get_handler] File sending failed!\n");
                vTaskDelay(log_delay / portTICK_PERIOD_MS);
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
               return ESP_FAIL;
           }
        }

        /* Keep looping till the whole file is sent */
    } while (chunksize != 0);

    /* Close file after sending complete */
    fclose(fd);
    elog("INFO : [download_get_handler] File sending complete\n");

    /* Respond with an empty chunk to signal HTTP response completion */
#ifdef CONFIG_EXAMPLE_HTTPD_CONN_CLOSE_HEADER
    httpd_resp_set_hdr(req, "Connection", "close");
#endif
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* Handler to upload a file onto the server */
static esp_err_t upload_post_handler(httpd_req_t *req)
{
    char log_str[256]={0};
    char filepath[FILE_PATH_MAX];
    FILE *fd = NULL;
    struct stat file_stat;

    /* Skip leading "/upload" from URI to get filename */
    /* Note sizeof() counts NULL termination hence the -1 */
    const char *filename = get_path_from_uri(filepath, ((struct file_server_data *)req->user_ctx)->base_path,
                                             req->uri + sizeof("/upload") - 1, sizeof(filepath));
    if (!filename) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
        return ESP_FAIL;
    }

    /* Filename cannot have a trailing '/' */
    if (filename[strlen(filename) - 1] == '/') {
        sprintf(log_str,"ERROR : [upload_post_handler] Invalid filename : %s\n", filename);
        elog(log_str);
        vTaskDelay(log_delay / portTICK_PERIOD_MS);

        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Invalid filename");
        return ESP_FAIL;
    }

    if (stat(filepath, &file_stat) == 0) {

        sprintf(log_str,"ERROR : [upload_post_handler] File already exists : %s\n", filepath);
        elog(log_str);
        vTaskDelay(log_delay / portTICK_PERIOD_MS);

        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "File already exists");
        return ESP_FAIL;
    }

    /* File cannot be larger than a limit */
    if (req->content_len > MAX_FILE_SIZE) {

        sprintf(log_str,"ERROR : [upload_post_handler] File too large : %d bytes\n", req->content_len);
        elog(log_str);
        vTaskDelay(log_delay / portTICK_PERIOD_MS);

        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "File size must be less than "
                            MAX_FILE_SIZE_STR "!");
        /* Return failure to close underlying connection else the
         * incoming file content will keep the socket busy */
        return ESP_FAIL;
    }

    fd = fopen(filepath, "w");
    if (!fd) {
      sprintf(log_str,"ERROR : [upload_post_handler] Failed to create file : %s\n", filepath);
      elog(log_str);
      vTaskDelay(log_delay / portTICK_PERIOD_MS);

        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to create file");
        return ESP_FAIL;
    }


    sprintf(log_str,"INFO : [upload_post_handler] Receiving file : %s\n", filename);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);


    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *buf = ((struct file_server_data *)req->user_ctx)->scratch;
    int received;

    /* Content length of the request gives
     * the size of the file being uploaded */
    int remaining = req->content_len;

    while (remaining > 0) {

        ESP_LOGI(TAG, "Remaining size : %d", remaining);
        /* Receive the file part by part into a buffer */
        if ((received = httpd_req_recv(req, buf, MIN(remaining, SCRATCH_BUFSIZE))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry if timeout occurred */
                continue;
            }

            /* In case of unrecoverable error,
             * close and delete the unfinished file*/
            fclose(fd);
            unlink(filepath);

            elog("ERROR : [upload_post_handler] File reception failed!\n");
            vTaskDelay(log_delay / portTICK_PERIOD_MS);


            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
            return ESP_FAIL;
        }

        /* Write buffer content to file on storage */
        if (received && (received != fwrite(buf, 1, received, fd))) {
            /* Couldn't write everything to file!
             * Storage may be full? */
            fclose(fd);
            unlink(filepath);
            elog("ERROR [upload_post_handler] File write failed!\n");
            vTaskDelay(log_delay / portTICK_PERIOD_MS);
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to write file to storage");
            return ESP_FAIL;
        }

        /* Keep track of remaining size of
         * the file left to be uploaded */
        remaining -= received;
    }

    /* Close file upon upload completion */
    fclose(fd);
    elog("ERROR [upload_post_handler] File reception complete\n");
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    /* Redirect onto root to see the updated file list */
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
#ifdef CONFIG_EXAMPLE_HTTPD_CONN_CLOSE_HEADER
    httpd_resp_set_hdr(req, "Connection", "close");
#endif
    httpd_resp_sendstr(req, "File uploaded successfully");
    return ESP_OK;
}

/* Handler to delete a file from the server */
static esp_err_t delete_post_handler(httpd_req_t *req)
{
    char log_str[256]={0};
    char filepath[FILE_PATH_MAX];
    struct stat file_stat;

    /* Skip leading "/delete" from URI to get filename */
    /* Note sizeof() counts NULL termination hence the -1 */
    const char *filename = get_path_from_uri(filepath, ((struct file_server_data *)req->user_ctx)->base_path,
                                             req->uri  + sizeof("/delete") - 1, sizeof(filepath));
    if (!filename) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
        return ESP_FAIL;
    }

    /* Filename cannot have a trailing '/' */
    if (filename[strlen(filename) - 1] == '/') {
        sprintf(log_str,"ERROR : [delete_post_handler] Invalid filename : %s\n", filename);
        elog(log_str);
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Invalid filename");
        return ESP_FAIL;
    }

    if (stat(filepath, &file_stat) == -1) {
        sprintf(log_str,"ERROR : [delete_post_handler] File does not exist : %s\n", filename);
        elog(log_str);
        vTaskDelay(log_delay / portTICK_PERIOD_MS);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "File does not exist");
        return ESP_FAIL;
    }


    sprintf(log_str,"INFO : [delete_post_handler] Deleting file : %s\n", filename);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);

    /* Delete file */
    unlink(filepath);

    /* Redirect onto root to see the updated file list */
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
#ifdef CONFIG_EXAMPLE_HTTPD_CONN_CLOSE_HEADER
    httpd_resp_set_hdr(req, "Connection", "close");
#endif
    httpd_resp_sendstr(req, "File deleted successfully");
    return ESP_OK;
  }


esp_err_t elite_start_file_server(const char *base_path)
{
    static struct file_server_data *server_data = NULL;

    if (server_data) {
        elog("ERROR : [elite_start_file_server] File server already started\n");
        return ESP_ERR_INVALID_STATE;
    }

    /* Allocate memory for server data */
    server_data = calloc(1, sizeof(struct file_server_data));
    if (!server_data) {
        elog("ERROR [elite_start_file_server] Failed to allocate memory for server data\n");
        return ESP_ERR_NO_MEM;
    }
    strlcpy(server_data->base_path, base_path,
            sizeof(server_data->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Use the URI wildcard matching function in order to
     * allow the same handler to respond to multiple different
     * target URIs which match the wildcard scheme */
    config.uri_match_fn = httpd_uri_match_wildcard;

    char log_str[256]={0};
    sprintf(log_str,"INFO : [start_file_server ] Starting HTTP Server on port: '%d'\n", config.server_port);
    elog(log_str);
    vTaskDelay(log_delay / portTICK_PERIOD_MS);
    if (httpd_start(&server, &config) != ESP_OK) {
        elog("ERROR : [elite_start_file_server] Failed to start file server!\n");
        return ESP_FAIL;
    }

    /* URI handler for getting uploaded files */
    httpd_uri_t file_download = {
        .uri       = "/*",  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = download_get_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_download);

    /* URI handler for uploading files to server */
    httpd_uri_t file_upload = {
        .uri       = "/upload/*",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = upload_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_upload);

    /* URI handler for deleting files from server */
    httpd_uri_t file_delete = {
        .uri       = "/delete/*",   // Match all URIs of type /delete/path/to/file
        .method    = HTTP_POST,
        .handler   = delete_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_delete);

    return ESP_OK;
}
