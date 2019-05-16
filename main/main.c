#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "ili9340.h"
#include "fontx.h"

FontxFile fx16[2];
FontxFile fx24[2];
FontxFile fx32[2];

#define _DEBUG_ 1

static const char *TAG = "ILI9340";

static void SPIFFS_Directory(char * path) {
    DIR* dir = opendir(path);
    assert(dir != NULL);
    while (true) {
        struct dirent*pe = readdir(dir);
        if (!pe) break;
        ESP_LOGI(TAG,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
    }
    closedir(dir);
}

#define CONFIG_GPIO_CS 5
#define CONFIG_GPIO_DC 26
#define CONFIG_GPIO_RESET 2
#define CONFIG_HEIGHT 320
#define CONFIG_WIDTH  240

TickType_t fillTest(ILI9340_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
	lcdFillScreen(dev, RED);
	vTaskDelay(1);
	lcdFillScreen(dev, GREEN);
	vTaskDelay(1);
	lcdFillScreen(dev, BLUE);
	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "diffTick=%d",diffTick);
	return diffTick;
}

TickType_t barTest(ILI9340_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();
	uint16_t Y1,Y2;
	Y1 = height/3;
	Y2 = (height/3)*2;
    //ESP_LOGI(TAG, "Y1=%d Y2=%d",Y1,Y2);
	lcdDrawFillRect(dev, 0, 0, width, Y1, RED);
	vTaskDelay(1);
	lcdDrawFillRect(dev, 0, Y1, width, Y2, GREEN);
	vTaskDelay(1);
	lcdDrawFillRect(dev, 0, Y2, width, height, BLUE);
	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "diffTick=%d",diffTick);
	return diffTick;
}


void ILI9341(void *pvParameters)
{
    /* フォントファイルの指定(お好みで) */
    FontxFile fx[2];
    InitFontx(fx16,"ILGH16XB.FNT",""); // 16Dot Gothic
    InitFontx(fx24,"ILGH24XB.FNT",""); // 24Dot Gothic
    InitFontx(fx24,"ILGH32XB.FNT",""); // 24Dot Gothic
	
	ILI9340_t dev;
	spi_master_init(&dev, CONFIG_GPIO_CS, CONFIG_GPIO_DC, CONFIG_GPIO_RESET);
	lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);

	fillTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
	barTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);

#if 0

#endif

    while (1) {
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}


void app_main(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed =true
    };

    // Use settings defined above toinitialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is anall-in-one convenience function.
    esp_err_t ret =esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret ==ESP_FAIL) {
           ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret== ESP_ERR_NOT_FOUND) {
           ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
           ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total,&used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
    }

    SPIFFS_Directory("/spiffs");

    xTaskCreate(ILI9341, "ILI9341", 4096, NULL, 2, NULL);
}
