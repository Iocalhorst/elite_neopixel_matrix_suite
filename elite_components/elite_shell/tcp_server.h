//tcp server copy-paste boilerplate stuff
//creates socket
//bind,listen
//call elite_shell_construct(); on socket.accept()
//then loop socket.recv() and passes bytes into elice_shell_handle_input(instance,bytes,len);
//passes received bytes to elite_shell_handle_input
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
#include "elite_shell.h"
#include "defines.h"
#pragma once

//static const char *TAG = "tcp_server";

static void handle_socket(const int sock){
    elite_shell_t *mr_shell=elite_shell_construct();
    mr_shell->outfd=sock;
    int len;
    char rx_buffer[128];
    do {
        elite_shell_send_prompt(mr_shell,sock,0);
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0) {
            //ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);#
            ELOG("ERROR : [tcp_server] Error occured during receiving\n");

        }else {
            if (len == 0) {
                //ESP_LOGW(TAG, "Connection closed");
                ELOG("INFO : [tcp_server] Connection closed\n");

            }else {
                rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
                //ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);

                ELOG("INFO : [tcp_server] Received %d bytes: %s\n", len, rx_buffer);


                // send() can return less bytes than supplied length.
                // Walk-around for robust implementation.
                //int to_write = len;
                //while (to_write > 0) {
                if (elite_shell_handle_input(mr_shell,sock,rx_buffer, len,0)==false){
                    ELOG("INFO: [tcp_server] Closing socket due to receive error or exit command\n");

                    return;
                }else {
                    ELOG("INFO : [tcp_server_handle_socket] mr_shell returned true\n");

                };
            }
        };
    } while (len > 0);
}

static void tcp_server_task(void *pvParameters)
{
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;

    if (addr_family == AF_INET) {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(TCP_SERVER_PORT);
        ip_protocol = IPPROTO_IP;
    }

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ELOG("ERROR : [tcp_server] Unable to create socket\n");

        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ELOG("INFO : [tcp_server] socket created\n");

    //ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ELOG("ERROR : [tcp_server] Socket unable to bind.\n");


        //ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ELOG("INFO : [tcp_server] Socket bound to port 9002\n");


    err = listen(listen_sock, 1);
    if (err != 0) {
        //ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        ELOG("ERROR : [tcp_server] Error occurred during listen\n");

        goto CLEAN_UP;
    }

    while (1) {
        ELOG("INFO : [tcp_server] Socket listening\n");


    //    ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
//            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            ELOG("ERROR : [tcp_server] Unable to accept connection\n");


            break;
        }


        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string

        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }

//        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

        ELOG("INFO : [tcp_server] Socket accepted connection from %s\n",addr_str);



      //ELOG("INFO : [tcp_server] Socket accepted connection");

      //


        handle_socket(sock);

        shutdown(sock, 0);
        close(sock);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}
