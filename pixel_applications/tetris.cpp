#pragma once
#include "elitePixelEngine.h"
#include "eliteSystem.h"
#include "eliteTools.h"
#include "eliteParticle.h"
//#include "AsyncUDP.h"
#include "tetris.h"






cTetris::cTetris() {  
      this->name="tetris";
      this->constructCanvas(10,30);
      this->nextTick=1.0f;
      float startupDelay=3.0f;
    for (int y=0;y<30;y++) {
      for (int x=0;x<10;x++) {
        this->playField[x][y]=0;
      };
    };
    this->particleShower=cParticleShower();
    this->particleShower.setMaxParticles(100);  
    
    this->gameOver=false;
    this->agent=cTetrisAgent();
    this->agent.setKeyStrokeIntervall(this->tickIntervall);

    
    /*
    for (int i=0;i<10;i++) {sRGBA c=this->colorMaps[0][i];c.r=c.r/2;c.g=c.g/2;c.b=c.b/2;this->colorMaps[0][i]=c;};
    for (int i=0;i<10;i++) {sRGBA c=this->colorMaps[1][i];c.r=c.r/2;c.g=c.g/2;c.b=c.b/2;this->colorMaps[1][i]=c;};
    */
    for (int i=0;i<10;i++) {this->colorMap[i]=this->colorMaps[this->colorMapIndex][i];};
    this->resetBucketString();
 /*   //this->blockString=blockString;
    for (uint32_t i=0;i<=700000;i++) {
    this->resetBlock(this->currentBlock);
    };
    
    String s="RNG Distribution Test - 700000 Blocks\n";
    double relO;
    double relI;
    double relJ;
    double relL;
    double relS;
    double relZ;
    double relT;
    relO=(double)this->Ocount/700000.0f;
    s+="O-Block : "+String(this->Ocount)+" ("+String(relO)+")\n";
    relI=(double)this->Icount/700000.0f;
    s+="I-Block : "+String(this->Icount)+" ("+String(relI)+")\n";
    relL=(double)this->Lcount/700000.0f;
    s+="L-Block : "+String(this->Lcount)+" ("+String(relL)+")\n";
    relJ=(double)this->Jcount/700000.0f;
    s+="J-Block : "+String(this->Jcount)+" ("+String(relJ)+")\n";  
    relS=(double)this->Scount/700000.0f;
    s+="S-Block : "+String(this->Scount)+" ("+String(relS)+")\n";
    relZ=(double)this->Zcount/700000.0f;
    s+="Z-Block : "+String(this->Zcount)+" ("+String(relZ)+")\n";
    relT=(double)this->Tcount/700000.0f;
    s+="T-Block : "+String(this->Tcount)+" ("+String(relT)+")\n";
    double dTotal=relT+relJ+relL+relS+relZ+relI+relO;
    s+="sanitiy check - if "+String(dTotal)+"!=1.0 then it's rigged\n";
    cEliteSystem::log(s);*/
    this->resetPlayField();
    this->linesCleared=0;
   /* if (udp.listen(1337)) {
  //    cEliteSystem::log("Tetris listening on 1337");
    udp.onPacket([this](AsyncUDPPacket packet) {
      
      //String s="Tetris controller signal=";
      String msg = (char*)packet.data();
      //s+=msg;       
      if (msg.endsWith("left")) {this->handleKey("left");};
      if (msg.endsWith("right")) {this->handleKey("right");};
      if (msg.endsWith("up")) {this->handleKey("up");};
      if (msg.endsWith("down")) {this->handleKey("down");};
      if (msg.endsWith("tab")) {this->handleKey("tab");};
      if (msg.endsWith("space")) {this->handleKey("space");};
      
    });};*/
    uint8_t *playFieldPtr=&this->playField[0][0];
    this->agent.readPlayField(playFieldPtr,this->currentBlock);
    this->agent.runSimulation();
};
bool cTetris::handleCommandString(String cmd, bool debug){
  
  if (cmd==String("swap")) {
    this->colorMapIndex+=1;
    this->colorMapIndex%=numColorMaps; 
    for (int i=0;i<10;i++) this->colorMap[i]=this->colorMaps[this->colorMapIndex][i];
    return true;
    };
  return false;
};

bool cTetris::rotateBlock(sBlock &block){
  if (!this->gameOver) {
    sBlock testBlock;
    testBlock=block;
    String rString;
    if (block.width==3&&block.height==3) {
      rString="";
      rString+=block.bodyStr.charAt(6);
      rString+=block.bodyStr.charAt(3);
      rString+=block.bodyStr.charAt(0);
      rString+=block.bodyStr.charAt(7);
      rString+=block.bodyStr.charAt(4);
      rString+=block.bodyStr.charAt(1);
      rString+=block.bodyStr.charAt(8);
      rString+=block.bodyStr.charAt(5);
      rString+=block.bodyStr.charAt(2);     
    };
  if (block.width==4&&block.height==4) {
      rString="";
      rString+=block.bodyStr.charAt(12);
      rString+=block.bodyStr.charAt(8);
      rString+=block.bodyStr.charAt(4);
      rString+=block.bodyStr.charAt(0);
      rString+=block.bodyStr.charAt(13);
      rString+=block.bodyStr.charAt(9);
      rString+=block.bodyStr.charAt(5);
      rString+=block.bodyStr.charAt(1);
      rString+=block.bodyStr.charAt(14);
      rString+=block.bodyStr.charAt(10);
      rString+=block.bodyStr.charAt(6);     
      rString+=block.bodyStr.charAt(2);     
      rString+=block.bodyStr.charAt(15);     
      rString+=block.bodyStr.charAt(11);     
      rString+=block.bodyStr.charAt(7);     
      rString+=block.bodyStr.charAt(3);     
    };
      testBlock.bodyStr=rString;
      if (!this->checkForCollision(testBlock)) {block=testBlock;return true;}
      else return false;
  };
};

bool cTetris::moveBlock(String buttonStr, sBlock &block){
  sBlock testBlock=block;
  if (buttonStr=="left") testBlock.x-=1;
  if (buttonStr=="right") testBlock.x+=1;
  if (buttonStr=="down") testBlock.y+=1;
  if (buttonStr=="up") testBlock.y-=1;
  if (!this->checkForCollision(testBlock)) {block=testBlock;return true;}
  else return false;
};

void cTetris::handleKey(String keyStr){
  String logStr="TetrisController handling [";
  logStr+=keyStr;
  logStr+="]";
  //cEliteSystem::log(logStr);
  if (keyStr=="space"||keyStr=="tab") this->rotateBlock(this->currentBlock);
  else this->moveBlock(keyStr,this->currentBlock);
};

void cTetris::resetBucketString(){    
    this->bucketString="";
    this->bucketString+="OJLTIZSOJLTIZS";
    //this->bucketString+="IIIILLJJTTTOO"; //bias
    this->bucketString+="OJLTIZSOJLTIZSOJLTIZSOJLTIZS";
    
};
          
void cTetris::resetBlock(sBlock &block){
    //cEliteSystem::log("resetting block");
    if (this->bucketString.length()<=7) this->resetBucketString();
    int index=random8(this->bucketString.length());
    
    char blockChar=this->bucketString.charAt(index);
    if (this->lastBlockChar==blockChar) {
      index=random8(this->bucketString.length());    
      blockChar=this->bucketString.charAt(index);
    };
    this->lastBlockChar=blockChar;
    //this->bucketString.remove(index);
    block.bodyStr=String("");
    //if (blockType==1) {    
    switch (blockChar) {
      case 'O' : {
              this->Ocount+=1;
              block.bodyStr+="____";
              block.bodyStr+="_11_";
              block.bodyStr+="_11_";
              block.bodyStr+="____";
              block.width=4;
              block.height=4;
              block.colorIndex=1;
              break;           
              };
    case 'I' : {
              this->Icount+=1;
              block.bodyStr+="____";
              block.bodyStr+="____";
              block.bodyStr+="2222";
              block.bodyStr+="____";      
              block.width=4;
              block.height=4;
              block.colorIndex=2;
              break;
              };
    case 'J' : {
              this->Jcount+=1;
              block.bodyStr+="3__";
              block.bodyStr+="333";
              block.bodyStr+="___";
              block.width=3;
              block.height=3;
              block.colorIndex=3;
              break;
              };
    case 'L' :{
              this->Lcount+=1;
              block.bodyStr+="__4";
              block.bodyStr+="444";
              block.bodyStr+="___";                   
              block.height=3;
              block.width=3;
              block.colorIndex=4;
              break;
              };
    case 'T' : {
              this->Tcount+=1;
              block.bodyStr+="_5_";
              block.bodyStr+="555";
              block.bodyStr+="___";
              block.width=3;
              block.height=3;  
              block.colorIndex=5;            
              break;
              };
    case 'S' : {
              this->Scount+=1;
              block.bodyStr+="_66";
              block.bodyStr+="66_";
              block.bodyStr+="___";              
              block.width=3;
              block.height=3;
              block.colorIndex=6;
              break;
              };
    case 'Z' : {
              this->Zcount+=1;
              block.bodyStr+="77_";
              block.bodyStr+="_77";              
              block.bodyStr+="___";
              block.width=3;
              block.height=3;
              block.colorIndex=7;
              break;
              };
    default : {              
              block.bodyStr="___";
              block.bodyStr="___";
              block.bodyStr="___";
              block.width=3;
              block.height=3;
              block.colorIndex=0;
              break;       
              };
    };
    block.x=4;
    block.y=0;
    if (this->checkForCollision(block)) {
      this->consolidate(block);
      this->gameOver=true;
      String s;
      s="Lines cleared : "+String(this->linesCleared);
      cEliteSystem::log(s);
      this->sumLinesCleared+=this->linesCleared;
      float avg=(float)this->sumLinesCleared/(float)this->gameCount;      
      s="Total Lines cleared : "+String(sumLinesCleared)+" in "+String(this->gameCount)+" games played";
      cEliteSystem::log(s);
      s="Average Lines cleared per game : "+String(avg);
      cEliteSystem::log(s);
      this->nextTick=1.0f;
      this->linesCleared=0;
    };   
    
   
      
};

void cTetris::drawPlayfield(){    
    //cEliteSystem::log("drawing playfield");
    for (int y=0;y<30;y++) {
        for (int x=0;x<10;x++) {
          sRGBA color;
          if (this->playField[x][y]>0) {
            color=this->colorMap[this->playField[x][y]]; 
            color.r=color.r;
            color.g=color.g;
            color.b=color.b;
            this->putPixelRGBA(x,y,color);
          };
        };
    };
  };


void cTetris::drawBlock(sBlock &block){
    //cEliteSystem::log("drawing block");
    uint8_t colorIndex=block.colorIndex;
    sRGBA color=this->colorMap[colorIndex];
       color.r=color.r;
          color.g=color.g;
          color.b=color.b;
    for (int y=0;y<block.height;y++) {
      for (int x=0;x<block.width;x++) {
        char c=block.bodyStr.charAt(block.height*y+x);
        if (c!='_') { 
       
          
          this->putPixelRGBA(x+block.x,y+block.y,color);};        
        };
      };
};


bool cTetris::checkForCollision(sBlock &block){
    for (int i=0;i<block.height;i++){
      for (int j=0;j<block.width;j++){
        int x=j+block.x;
        int y=i+block.y;
        char blockChar=block.bodyStr.charAt(block.width*i+j);
        if (blockChar!='_') {
          if (x<0||x>=10||y>=30||y<0) return true;
         if (((x>=0)&&(x<10)&&(y<30)&&(y>=0))&&(this->playField[x][y]!=0)) return true;
        };
      };
    };
  return false;    
};


void cTetris::consolidate(sBlock &block){
    for (int i=0;i<block.height;i++){
      for (int j=0;j<block.width;j++){
        int x=j+block.x;
        int y=i+block.y;
        if ((x>=0)&&(x<10)&&(y<30)&&(y>=0)) {
          if (block.bodyStr.charAt(block.width*i+j)!='_') {this->playField[x][y]=block.colorIndex;};
        };       
      };
    };        
};
  

void cTetris::removeAndConsolidateLines(){
  for (int y=0;y<30;y++) {
    int result=1;
    for (int x=0;x<10;x++) {      
      result*=this->playField[x][y];
    };
    if (result!=0) {
      this->linesCleared+=1;
      for (int y2=y;y2>0;y2--) {
        for (int x=0;x<10;x++) {     
          this->playField[x][y2]=this->playField[x][y2-1];
        };
      };
    };
  };
};
        
         

void cTetris::advance(){
    this->currentBlock.y+=1;
    if (this->checkForCollision(this->currentBlock)) {      
      this->currentBlock.y-=1;
      this->consolidate(this->currentBlock);
      this->removeAndConsolidateLines();
      this->resetBlock(this->currentBlock);
      if (!gameOver) {
          uint8_t *playFieldPtr=&this->playField[0][0];
          this->agent.readPlayField(playFieldPtr,this->currentBlock);
          this->agent.runSimulation();
      };
      //this->gameOver=false;
    };
};


void cTetris::resetPlayField(){
    for (int y=0;y<30;y++) {
          for (int x=0;x<10;x++) {
            this->playField[x][y]=0;
          };
        };
    this->resetBlock(this->currentBlock);
    this->gameOver=false;
    this->gameCount++;
    String s="Starting new game : "+String(this->gameCount);
    cEliteSystem::log(s);
};


bool cTetris::onUserUpdate(float fElapsedTime){

    if (this->sigKill==true) {
      this->particleShower.cleanup();    
 //     (this->agent)->cleanup();
      //  delete this->agent;
      this->cleanUpEngine();
    //udp.close();
    
    
      return false;
    };
    if (this->startupDelay>=0.0f) {this->startupDelay-=fElapsedTime;};
    if (this->startupDelay<=0.0f) {
          this->nextTick-=fElapsedTime;  
          if (this->nextTick<=0.0f) {
              if (this->gameOver) {
                this->resetPlayField();
              }
              else {      
                this->nextTick=this->tickIntervall;
                this->advance();                
              };
          };
          int action=this->agent.getAction(fElapsedTime,this->currentBlock);
          switch (action) {
              case 1 : {this->handleKey("left");break;};
              case 2 : {this->handleKey("right");break;};
              case 3 : {this->handleKey("space");break;};
              case 4 : {this->handleKey("down");break;};
              default : {};
          };
    };  
  
  //cEliteSystem::log("clearing canvas");
  this->particleShower.updateSelf(fElapsedTime);  
    this->clearCanvas();   
    this->fillLayer(ZERO_LAYER,eliteColors::SOLID_NOTHING);   
    this->fillLayer(BG_LAYER,eliteColors::SOLID_BLACK);   
    this->fillLayer(FG_LAYER,eliteColors::SOLID_NOTHING);   
    this->setTargetLayer(BG_LAYER);
   this->particleShower.drawSelf(this);
    this->setTargetLayer(FG_LAYER);
    if (this->startupDelay<=0.0f) {
      this->drawPlayfield();
      this->drawBlock(this->currentBlock);
    };
    this->renderLayer(BG_LAYER);
    this->renderLayer(FG_LAYER);
    return true;
};
  

