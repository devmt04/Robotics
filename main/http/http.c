#include "../common/common.h"
#include "http.h"
#include <esp_http_server.h>
#include "esp_log.h"
#include <time.h>
#include <string.h>
#include <sys/param.h>
#include "cJSON.h"
#include "freertos/ringbuf.h"

static esp_err_t controller_handler(httpd_req_t *req){
    // const char* resp_str = req->user_ctx;
    //Create ring buffer
    
  const char resp_str[] =
"<!DOCTYPE html>"
"<html>"
"<body>"
"<h1>Hello from ESP32-S3</h1>"
"<p>This page scales on mobile!</p>"
"<button onclick='sendData(0)'>Forward</button>"
"<button onclick='sendData(1)'>Left</button>"
"<button onclick='sendData(2)'>Backward</button>"
"<button onclick='sendData(3)'>Right</button><br>"
"<button onclick='sendData(4)'>Break</button>"
"<script>"
"function sendData(dir) {"
"    <!--Ajax request-->"
"    const xhr = new XMLHttpRequest();"
"    xhr.open('POST', '/', true);"
"    xhr.setRequestHeader('Content-Type', 'application/json');"
"    xhr.send(JSON.stringify({dir: dir}));"
"}"
"</script>"
"</body>"
"</html>";

    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    char buf[100];
    int ret, remaining = req->content_len;
    
    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        remaining -= ret;
        /* Send back the same data */
        // httpd_resp_send_chunk(req, buf, ret);

        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");

        cJSON *root = cJSON_Parse(buf);
        if (root == NULL) {
            printf("Failed to parse JSON\n");
            const char* resp = "Invalid JSON";
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
        cJSON *dir_item = cJSON_GetObjectItem(root, "dir");
        if (cJSON_IsNumber(dir_item)) {
            int dir = dir_item->valueint;

            //Send an item vai ring buffer
            UBaseType_t res =  xRingbufferSend(motor_ringbuf, &dir, sizeof(dir), 0);
            if (res != pdTRUE) {
                printf("Failed to send item\n");
            }
        }

        cJSON_Delete(root);
    }

    
    httpd_resp_send_chunk(req, NULL, 0); // end of response, as per HTTP/1.1 spec
    return ESP_OK;
}

static const httpd_uri_t controller = {
    .uri = "/",
    .method = HTTP_ANY,
    .handler = controller_handler,
    .user_ctx = NULL//"<b>Hello World</b>" // user context data which will be avilable to handler
};

static httpd_handle_t start_webserver(void){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &controller);
    }
    return server;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

void http_server_init(void){
    // printf("Init http server");
    static httpd_handle_t server = NULL;
    // printf("Start webserver");
    server = start_webserver();
}