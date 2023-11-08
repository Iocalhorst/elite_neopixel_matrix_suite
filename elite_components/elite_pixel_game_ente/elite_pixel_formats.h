#pragma once
typedef struct {
  uint8_t r,g,b;
}sRGB;


typedef struct {
  float fr,fg,fb;
}sfRGB;


typedef struct {
  float fr,fg,fb,fa;
}sfRGBA;


#define FRAMEBUF_PIXFORMAT sfRGB
#define LAYER_PIXFORMAT sfRGBA
