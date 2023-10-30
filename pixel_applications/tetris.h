#pragma once
#include "elitePixelEngine.h"
#include "eliteSystem.h"
#include "eliteTools.h"
#include "eliteParticle.h"
//#include "AsyncUDP.h"


typedef struct {
   String bodyStr;
   int x;
   int y;
   uint8_t width;
   uint8_t height;
   uint8_t colorIndex;
 }sBlock;

#include "tetrisAgent.h"

 class cTetris:public cElitePixelEngine{


  public :

float startupDelay=3.0f;
 // float tickIntervall=0.0000025f;//speedrun;
  float tickIntervall=0.175f;
//  AsyncUDP udp;
  
  sRGBA red1={255,0,0,255};
  sRGBA green1={0,255,0,255};
  sRGBA blue1={0,0,255,255};
  sRGBA cyan1={0,255,255,255};
  sRGBA magenta1={255,0,255,255};
  sRGBA yellow1={255,204,0,255};
  sRGBA white1={255,255,255,255};
  sRGBA orange1={255,102,0,255};
  sRGBA purple1={96,0,192,255};
  sRGBA black1={0,0,0,255};

  sRGBA red2={132,8,8,255};
  sRGBA green2={8,128,6,255};
  sRGBA blue2={24,16,156,255};//pretty blue
  sRGBA cyan2={12,108,128,255};
  sRGBA magenta2={126,18,104,255};
  sRGBA yellow2={128,124,6,255};
  sRGBA white2={127,127,127,255};
  sRGBA orange2={154,72,4,255};
  sRGBA purple2={42,0,96,255};
  sRGBA black2={0,0,0,255};
        
  sRGBA colorMap[10]={black1,red1,green1,blue1,yellow1,cyan1,magenta1,orange1,purple1,white1};
  static constexpr uint8_t numColorMaps=2;
  sRGBA colorMaps[numColorMaps][10]={{black1,red1,green1,blue1,yellow1,cyan1,magenta1,orange1,purple1,white1},
                                     {black2,red2,green2,blue2,yellow2,cyan2,magenta2,orange2,purple2,white2}};
  uint8_t colorMapIndex=1;
  float nextTick=1.0f;
  uint8_t playField[10][30];
  sBlock currentBlock;  
  String bucketString="";;
  bool gameOver=false;
  cParticleShower particleShower;
  cTetrisAgent agent;
  char lastBlockChar='_';
  uint16_t linesCleared=0;
  uint16_t sumLinesCleared=0;
  uint16_t gameCount=0;
  uint32_t Ocount=0;
  uint32_t Lcount=0;
  uint32_t Icount=0;
  uint32_t Jcount=0;
  uint32_t Scount=0;
  uint32_t Zcount=0;
  uint32_t Tcount=0;

 cTetris();
bool rotateBlock(sBlock &block);
bool moveBlock(String buttonStr, sBlock &block);
void handleKey(String keyStr);
  void resetBlock(sBlock &block);
  void drawPlayfield();
  void drawBlock(sBlock &block);
  bool checkForCollision(sBlock &block);
  void consolidate(sBlock &block);
void removeAndConsolidateLines();
void resetBucketString();
bool handleCommandString(String cmd, bool debug = true) override;
  void advance();
    void resetPlayField();
  virtual bool onUserUpdate(float fElapsedTime)override;
  
};
