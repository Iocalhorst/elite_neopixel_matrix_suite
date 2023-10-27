//abstraction layer between the framebuffer provider and the led driver library
//TODO : implement gamma correction brightness control, color correction
#include "elite.h"
#include "elite_pixel_game_ente.h"

#pragma once
#define LED_STRIP_BLINK_GPIO  26
// Numbers of the LED in the strip
#define LED_STRIP_LED_NUMBERS 300
// 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)





led_strip_handle_t led_strip;


int yx2led[][10]={
   {299, 298, 297, 296, 295, 294, 293, 292, 291, 290},
   {280, 281, 282, 283, 284, 285, 286, 287, 288, 289},
   {279, 278, 277, 276, 275, 274, 273, 272, 271, 270},
   {260, 261, 262, 263, 264, 265, 266, 267, 268, 269},
   {259, 258, 257, 256, 255, 254, 253, 252, 251, 250},
   {240, 241, 242, 243, 244, 245, 246, 247, 248, 249},
   {239, 238, 237, 236, 235, 234, 233, 232, 231, 230},
   {220, 221, 222, 223, 224, 225, 226, 227, 228, 229},
   {219, 218, 217, 216, 215, 214, 213, 212, 211, 210},
   {200, 201, 202, 203, 204, 205, 206, 207, 208, 209},
   {199, 198, 197, 196, 195, 194, 193, 192, 191, 190},
   {180, 181, 182, 183, 184, 185, 186, 187, 188, 189},
   {179, 178, 177, 176, 175, 174, 173, 172, 171, 170},
   {160, 161, 162, 163, 164, 165, 166, 167, 168, 169},
   {159, 158, 157, 156, 155, 154, 153, 152, 151, 150},
   {140, 141, 142, 143, 144, 145, 146, 147, 148, 149},
   {139, 138, 137, 136, 135, 134, 133, 132, 131, 130},
   {120, 121, 122, 123, 124, 125, 126, 127, 128, 129},
   {119, 118, 117, 116, 115, 114, 113, 112, 111, 110},
   {100, 101, 102, 103, 104, 105, 106, 107, 108, 109},
   { 99,  98,  97,  96,  95,  94,  93,  92,  91,  90},
   { 80,  81,  82,  83,  84,  85,  86,  87,  88,  89},
   { 79,  78,  77,  76,  75,  74,  73,  72,  71,  70},
   { 60,  61,  62,  63,  64,  65,  66,  67,  68,  69},
   { 59,  58,  57,  56,  55,  54,  53,  52,  51,  50},
   { 40,  41,  42,  43,  44,  45,  46,  47,  48,  49},
   { 39,  38,  37,  36,  35,  34,  33,  32,  31,  30},
   { 20,  21,  22,  23,  24,  25,  26,  27,  28,  29},
   { 19,  18,  17,  16,  15,  14,  13,  12,  11,  10},
   {  0,   1,   2,   3,  4,    5,   6,   7,   8,   9} };


sRGB pixels[300]={0};
const int height=30;
const int width=10;


static bool stop_drawing=false;

void mr_display_put_pixel(int x,int y,sRGB c){
  if (stop_drawing) return;
  if (eliteAssert(x>=0&&x<width&&y>=0&&y<height,"you've hurt the canvas - game over")) {stop_drawing=true;return;};
  int pixel_index=x+width*y;
  sRGB c_=c;
  c_.r/=1;
  c_.g/=1;
  c_.b/=1;
  pixels[pixel_index]=c_;
};


void mr_display_update_leds(){
    for (int yy=0;yy<height;yy++){
        for (int xx=0;xx<width;xx++){
            int ledNumber=yx2led[yy][width-xx-1];//i flipped the xy2led map... oops thats why
            sRGB c=pixels[yy*width+xx];
            ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, ledNumber, c.r/4,c.g/4,c.b/4));
        };
    };
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));

};

void elite_display_update(sRGB *p_frame_buf){
  for (size_t i=0;i<height*width;i++) {pixels[i]=p_frame_buf[i];};
  mr_display_update_leds();
}



led_strip_handle_t configure_led_spi(void)
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_BLINK_GPIO,   // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_NUMBERS,        // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags.invert_out = false,                // whether to invert the output signal
    };

    // LED strip backend configuration: SPI
    led_strip_spi_config_t spi_config = {
        .clk_src = SPI_CLK_SRC_DEFAULT, // different clock source can lead to different power consumption
        .flags.with_dma = true,         // Using DMA can improve performance and help drive more LEDs
        .spi_bus = SPI2_HOST,           // SPI bus ID
    };

    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with SPI backend");
    return led_strip;
}


led_strip_handle_t configure_led_rmt(void)
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_BLINK_GPIO,   // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_NUMBERS,        // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags.invert_out = false,                // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
    };

    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");
    return led_strip;
}
