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

#define	INTERVAL		400
#define WAIT	vTaskDelay(INTERVAL)

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

// You have to set these CONFIG value using menuconfig.
#if 0
#define CONFIG_WIDTH  240
#define CONFIG_HEIGHT 320
#define CONFIG_CS_GPIO 5
#define CONFIG_DC_GPIO 26
#define CONFIG_RESET_GPIO 2
#define CONFIG_BL_GPIO 2
#endif

TickType_t FillTest(ILI9340_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	lcdFillScreen(dev, RED);
	vTaskDelay(1);
	lcdFillScreen(dev, GREEN);
	vTaskDelay(1);
	lcdFillScreen(dev, BLUE);
	vTaskDelay(1);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t ColorBarTest(ILI9340_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t y1,y2;
	y1 = height/3;
	y2 = (height/3)*2;
    //ESP_LOGI(TAG, "y1=%d y2=%d",y1,y2);
	lcdDrawFillRect(dev, 0, 0, width, y1, RED);
	vTaskDelay(1);
	lcdDrawFillRect(dev, 0, y1, width, y2, GREEN);
	vTaskDelay(1);
	lcdDrawFillRect(dev, 0, y2, width, height, BLUE);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t ArrowTest(ILI9340_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

    // get font width & height
    uint8_t buffer[FontxGlyphBufSize];
    uint8_t fontWidth;
    uint8_t fontHeight;
    GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
    //ESP_LOGI(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);
	
	uint16_t xpos;
	uint16_t ypos;
	int	stlen;

    uint16_t color;
    //lcdFillScreen(dev, WHITE);
    lcdFillScreen(dev, BLACK);
    lcdSetFontDirection(dev, 0);
    color = RED;
    uint8_t ascii[10];
    lcdDrawFillArrow(dev, 10, 10, 0, 0, 5, color);
    strcpy((char *)ascii, "0,0");
    lcdDrawString(dev, fx, 0, 20, ascii, color);

    color = GREEN;
    //lcdDrawArrow(dev, 69, 10, 79, 0, 5, color);
    lcdDrawFillArrow(dev, width-11, 10, width-1, 0, 5, color);
    //strcpy((char *)ascii, "79,0");
	sprintf((char *)ascii, "%d,0",width-1);
	stlen = strlen((char *)ascii);
	xpos = (width-1) - (fontWidth*stlen);
    lcdDrawString(dev, fx, xpos, 20, ascii, color);

    color = GRAY;
    //lcdDrawArrow(dev, 10, 149, 0, 159, 5, color);
    lcdDrawFillArrow(dev, 10, height-11, 0, height-1, 5, color);
    //strcpy((char *)ascii, "0,159");
	sprintf((char *)ascii, "0,%d",height-1);
	ypos = (height-11) - (fontHeight) - 5;
    lcdDrawString(dev, fx, 0, ypos, ascii, color);

    color = CYAN;
    //lcdDrawArrow(dev, 69, 149, 79, 159, 5, color);
    lcdDrawFillArrow(dev, width-11, height-11, width-1, height-1, 5, color);
    //strcpy((char *)ascii, "79,159");
	sprintf((char *)ascii, "%d,%d",width-1, height-1);
	stlen = strlen((char *)ascii);
	xpos = (width-1) - (fontWidth*stlen);
    lcdDrawString(dev, fx, xpos, ypos, ascii, color);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t DirectionTest(ILI9340_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	lcdFillScreen(dev, BLACK);
    uint8_t ascii[20];

	color = RED;
	strcpy((char *)ascii, "Direction=0");
	lcdSetFontDirection(dev, 0);
	lcdDrawString(dev, fx, 0, 0, ascii, color);

	color = BLUE;
	strcpy((char *)ascii, "Direction=2");
	lcdSetFontDirection(dev, 2);
	lcdDrawString(dev, fx, width-1, height-(fontHeight*1)-1, ascii, color);

	color = CYAN;
	strcpy((char *)ascii, "Direction=1");
	lcdSetFontDirection(dev, 1);
	lcdDrawString(dev, fx, width-fontHeight-1, 0, ascii, color);

	color = GREEN;
	strcpy((char *)ascii, "Direction=3");
	lcdSetFontDirection(dev, 3);
	lcdDrawString(dev, fx, fontHeight-1, height-1, ascii, color);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t HorizontalTest(ILI9340_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	lcdFillScreen(dev, BLACK);
    uint8_t ascii[20];

	color = RED;
	strcpy((char *)ascii, "Direction=0");
	lcdSetFontDirection(dev, 0);
	lcdDrawString(dev, fx, 0, 0, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, 0, 0+(fontHeight*1), ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, GREEN);
	lcdDrawString(dev, fx, 0, 0+(fontHeight*2), ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, 0, 0+(fontHeight*3), ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	color = BLUE;
	strcpy((char *)ascii, "Direction=2");
	lcdSetFontDirection(dev, 2);
	lcdDrawString(dev, fx, width, height-(fontHeight*1)-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, width, height-(fontHeight*2)-1, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, YELLOW);
	lcdDrawString(dev, fx, width, height-(fontHeight*3)-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, width, height-(fontHeight*4)-1, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t VerticalTest(ILI9340_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(TAG,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	lcdFillScreen(dev, BLACK);
    uint8_t ascii[20];

	color = RED;
	strcpy((char *)ascii, "Direction=1");
	lcdSetFontDirection(dev, 1);
	lcdDrawString(dev, fx, width-(fontHeight*1), 0, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, width-(fontHeight*2), 0, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, GREEN);
	lcdDrawString(dev, fx, width-(fontHeight*3), 0, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, width-(fontHeight*4), 0, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	color = BLUE;
	strcpy((char *)ascii, "Direction=3");
	lcdSetFontDirection(dev, 3);
	lcdDrawString(dev, fx, (fontHeight*1)-1, height, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, (fontHeight*2)-1, height, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, YELLOW);
	lcdDrawString(dev, fx, (fontHeight*3)-1, height, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, (fontHeight*4)-1, height, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}


TickType_t LineTest(ILI9340_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color=RED;
	for(int ypos=0;ypos<height;ypos=ypos+10) {
		lcdDrawLine(dev, 0, ypos, width, ypos, color);
	}

	for(int xpos=0;xpos<width;xpos=xpos+10) {
		lcdDrawLine(dev, xpos, 0, xpos, height, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t CircleTest(ILI9340_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = GRAY;
	uint16_t xpos = width/2;
	uint16_t ypos = height/2;
	for(int i=5;i<height;i=i+5) {
		lcdDrawCircle(dev, xpos, ypos, i, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t RoundRectTest(ILI9340_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();


    uint16_t color;
	uint16_t limit = width;
	if (width > height) limit = height;
    //lcdFillScreen(dev, WHITE);
    lcdFillScreen(dev, BLACK);
    color = BLUE;
    for(int i=5;i<limit;i=i+5) {
        if (i > (limit-i-1) ) break;
        //ESP_LOGI(TAG, "i=%d, width-i-1=%d",i, width-i-1);
        lcdDrawRoundRect(dev, i, i, (width-i-1), (height-i-1), 10, color);
    }
#if 0
    for(int i=5;i<width;i=i+5) {
        if (i > (width-i-1) ) break;
        //ESP_LOGI(TAG, "i=%d, width-i-1=%d",i, width-i-1);
        lcdDrawRoundRect(dev, i, i, (width-i-1), (height-i-1), 10, color);
    }
#endif

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t FillRectTest(ILI9340_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

    uint16_t color;
    lcdFillScreen(dev, CYAN);

	uint16_t red;
	uint16_t green;
	uint16_t blue;
	srand( (unsigned int)time( NULL ) );
	for(int i=1;i<100;i++) {
		red=rand()%255;
		green=rand()%255;
		blue=rand()%255;
		color=rgb565_conv(red, green, blue);
		uint16_t xpos=rand()%width;
		uint16_t ypos=rand()%height;
		uint16_t size=rand()%(width/5);
		lcdDrawFillRect(dev, xpos, ypos, xpos+size, ypos+size, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
	return diffTick;
}

TickType_t ColorTest(ILI9340_t * dev, int width, int height) {
    TickType_t startTick, endTick, diffTick;
    startTick = xTaskGetTickCount();

	uint16_t color;
	lcdFillScreen(dev, WHITE);
	color = RED;
	uint16_t delta = height/16;
	uint16_t ypos = 0;
	for(int i=0;i<16;i++) {
		//ESP_LOGI(TAG, "color=0x%x",color);
		lcdDrawFillRect(dev, 0, ypos, width-1, ypos+delta, color);
		color = color >> 1;
		ypos = ypos + delta;
	}

    endTick = xTaskGetTickCount();
    diffTick = endTick - startTick;
    ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%d",diffTick*portTICK_RATE_MS);
    return diffTick;
}


void ILI9341(void *pvParameters)
{
	SPIFFS_Directory("/spiffs/");

    // set font file
	FontxFile fx16G[2];
	FontxFile fx24G[2];
	FontxFile fx32G[2];
    InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 16Dot Gothic
    InitFontx(fx24G,"/spiffs/ILGH24XB.FNT",""); // 24Dot Gothic
    InitFontx(fx32G,"/spiffs/ILGH32XB.FNT",""); // 24Dot Gothic

	FontxFile fx16M[2];
	FontxFile fx24M[2];
	FontxFile fx32M[2];
    InitFontx(fx16M,"/spiffs/ILMH16XB.FNT",""); // 16Dot Mincyo
    InitFontx(fx24M,"/spiffs/ILMH24XB.FNT",""); // 24Dot Mincyo
    InitFontx(fx32M,"/spiffs/ILMH32XB.FNT",""); // 32Dot Mincyo
	
	ILI9340_t dev;
	spi_master_init(&dev, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
	lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);

#if 0
	//for TEST
	lcdDrawFillRect(&dev, 0, 0, 10, 10, RED);
	lcdDrawFillRect(&dev, 10, 10, 20, 20, GREEN);
	lcdDrawFillRect(&dev, 20, 20, 30, 30, BLUE);
#endif

	while(1) {

		FillTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		ColorBarTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		ArrowTest(&dev, fx16G, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		LineTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		CircleTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		RoundRectTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		if (CONFIG_WIDTH >= 240) {
			DirectionTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		} else {
			DirectionTest(&dev, fx16G, CONFIG_WIDTH, CONFIG_HEIGHT);
		}
		WAIT;

		if (CONFIG_WIDTH >= 240) {
			HorizontalTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		} else {
			HorizontalTest(&dev, fx16G, CONFIG_WIDTH, CONFIG_HEIGHT);
		}
		WAIT;

		if (CONFIG_WIDTH >= 240) {
			VerticalTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		} else {
			VerticalTest(&dev, fx16G, CONFIG_WIDTH, CONFIG_HEIGHT);
		}
		WAIT;

		FillRectTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		ColorTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		// Multi Font Test
		uint16_t color;
		uint8_t ascii[40];
		uint16_t xpos = 0;
		uint16_t ypos = 0;
		lcdFillScreen(&dev, BLACK);
		color = WHITE;
		lcdSetFontDirection(&dev, 0);
		int xd = 0;
		int yd = 1;
		if(CONFIG_WIDTH < CONFIG_HEIGHT) {
			lcdSetFontDirection(&dev, 1);
			xpos = CONFIG_WIDTH-16;
			xd = 1;
			yd = 0;
		}
		strcpy((char *)ascii, "16Dot Gothic Font");
		lcdDrawString(&dev, fx16G, xpos, ypos, ascii, color);

		xpos = xpos - (24 * xd);
		ypos = ypos + (16 * yd);
		strcpy((char *)ascii, "24Dot Gothic Font");
		lcdDrawString(&dev, fx24G, xpos, ypos, ascii, color);

		xpos = xpos - (32 * xd);
		ypos = ypos + (24 * yd);
		if (CONFIG_WIDTH >= 240) {
			strcpy((char *)ascii, "32Dot Gothic Font");
			lcdDrawString(&dev, fx32G, xpos, ypos, ascii, color);
			xpos = xpos - (32 * xd);
			ypos = ypos + (32 * yd);
		}

		//xpos = xpos - (10 * xd);
		ypos = ypos + (10 * yd);
		strcpy((char *)ascii, "16Dot Mincyo Font");
		lcdDrawString(&dev, fx16M, xpos, ypos, ascii, color);

		xpos = xpos - (24 * xd);
		ypos = ypos + (16 * yd);
		strcpy((char *)ascii, "24Dot Mincyo Font");
		lcdDrawString(&dev, fx24M, xpos, ypos, ascii, color);

		if (CONFIG_WIDTH >= 240) {
			xpos = xpos - (32 * xd);
			ypos = ypos + (24 * yd);
			strcpy((char *)ascii, "32Dot Mincyo Font");
			lcdDrawString(&dev, fx32M, xpos, ypos, ascii, color);
		}
		lcdSetFontDirection(&dev, 0);
		WAIT;

	} // end while

	// never reach
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
      .max_files = 6,
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

    xTaskCreate(ILI9341, "ILI9341", 1024*4, NULL, 2, NULL);
}
