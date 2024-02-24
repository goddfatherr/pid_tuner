#include "nvs_flash.h"
#include "esp_log.h"

#include "inc/helper.h"

static const char *TAG = "helper";

static void handle_error_code(esp_err_t error_code) {
    switch (error_code) {
        case ESP_FAIL:
            ESP_LOGI(TAG, "Internal error: ESP_FAIL\n");
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAG, "Requested key doesn't exist: ESP_ERR_NVS_NOT_FOUND\n");
            break;
        case ESP_ERR_NVS_INVALID_HANDLE:
            ESP_LOGI(TAG, "Invalid handle or handle closed: ESP_ERR_NVS_INVALID_HANDLE\n");
            break;
        case ESP_ERR_NVS_INVALID_NAME:
            ESP_LOGI(TAG, "Invalid key name: ESP_ERR_NVS_INVALID_NAME\n");
            break;
        case ESP_ERR_NVS_INVALID_LENGTH:
            ESP_LOGI(TAG, "Insufficient length to store data: ESP_ERR_NVS_INVALID_LENGTH\n");
            break;
        default:
            ESP_LOGI(TAG, "Unknown error code\n");
            break;
    }
}

esp_err_t persist_pid_values(){
    nvs_handle_t pid_data_handle;
    esp_err_t err;

    err = nvs_open("pid_data", NVS_READWRITE, &pid_data_handle);
    if (err != ESP_OK){
        handle_error_code(err);
        return err;
    }

    err = nvs_set_i32(pid_data_handle, "p", P);
    if (err != ESP_OK) {
        handle_error_code(err);
        return err;
    }
    
    err = nvs_set_i32(pid_data_handle, "i", I);
    if (err != ESP_OK) {
        handle_error_code(err);
        return err;
    }
    
    err = nvs_set_i32(pid_data_handle, "d", D);
    if (err != ESP_OK) {
        handle_error_code(err);
        return err;
    }

    err = nvs_commit(pid_data_handle);
    if (err != ESP_OK) {
        handle_error_code(err);
        return err;
    }

    nvs_close(pid_data_handle);
    ESP_LOGI(TAG, "PID saved to NVS: P = %ld, I = %ld, D = %ld", P, I, D);
    return ESP_OK;
}

esp_err_t load_pid_values(){
    nvs_handle_t pid_data_handle;
    esp_err_t err;

    err = nvs_open("init_data", NVS_READWRITE, &pid_data_handle);
    if (err != ESP_OK){
        handle_error_code(err);
        return err;
    }

    //PID should be int32
    err = nvs_get_i32(pid_data_handle, "p", &P);
    if (err != ESP_OK) {
        handle_error_code(err);
        return err;
    }
    
    err = nvs_get_i32(pid_data_handle, "i", &I);
    if (err != ESP_OK) {
        handle_error_code(err);
        return err;
    }

    err = nvs_get_i32(pid_data_handle, "d", &D);
    if (err != ESP_OK) {
        handle_error_code(err);
        return err;
    }

    err = nvs_commit(pid_data_handle);
    if (err != ESP_OK) {
        handle_error_code(err);
        return err;
    }

    nvs_close(pid_data_handle);
    ESP_LOGI(TAG, "PID loaded from NVS: P = %ld, I = %ld, D = %ld", P, I, D);

    return ESP_OK;

}
