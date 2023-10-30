#pragma once
#include "tetris.h"
#include "tetrisAgent.h"
//#include "FastLED.h"
#include "eliteSystem.h"


cTetrisAgent::cTetrisAgent(){
       this->fuse=0.3f;
       this->listResults=LinkedList<sResultSet>();
       this->keyStrokeIntervall=0.25f;
      };


int cTetrisAgent::getAction(float fElapsedTime,sBlock &currentBlock){
  this->fuse-=fElapsedTime;

  if (this->fuse>0.0f) {return 0;}
    else {
      this->fuse=this->keyStrokeIntervall;
      if (this->bestR>0) {this->bestR-=1;return 3;};
      if (currentBlock.x>this->bestX) return 1;
      if (currentBlock.x<this->bestX) return 2;
      if (this->bestX==currentBlock.x&&this->bestR==0) {this->fuse=0.15f*this->keyStrokeIntervall;return 4;};
    };
    return 0;
};



void cTetrisAgent::readPlayField(uint8_t playfield_arr[10][30],sBlock currentBlock){
//        uint8_t *backupViewPtr=&this->backupView[0][0];
  //         for (int i=0;i<300;i++) {
    //         backupViewPtr[i]=playFieldPtr[i];
             };


        this->backupBlock=currentBlock;
      };
void cTetrisAgent::restoreTestView(){
   uint8_t *backupViewPtr=&this->backupView[0][0];
   uint8_t *testViewPtr=&this->testView[0][0];
           for (int i=0;i<300;i++) {
             testViewPtr[i]=backupViewPtr[i];
           };
};

void cTetrisAgent::consolidateTestView(sBlock &block){
    for (int i=0;i<block.height;i++){
      for (int j=0;j<block.width;j++){
        int x=j+block.x;
        int y=i+block.y;
        if ((x>=0)&&(x<10)&&(y<30)&&(y>=0)) {
          if (block.bodyStr.charAt(block.width*i+j)!='_') {this->testView[x][y]=block.colorIndex;};
        };
      };
    };
};


void cTetrisAgent::setKeyStrokeIntervall(float f){this->keyStrokeIntervall=f*0.25f;};
int cTetrisAgent::removeAndConsolidateLines(){
  int numLinesCleared=0;
  for (int y=0;y<30;y++) {
    int result=1;
    for (int x=0;x<10;x++) {
      result*=this->testView[x][y];
    };
    if (result!=0) {
      numLinesCleared+=1;
      for (int y2=y;y2>0;y2--) {
        for (int x=0;x<10;x++) {
          this->testView[x][y2]=this->testView[x][y2-1];
        };
      };
    };
  };
  return numLinesCleared;
};

bool cTetrisAgent::checkForCollision(sBlock &block){
    for (int i=0;i<block.height;i++){
      for (int j=0;j<block.width;j++){
        int x=j+block.x;
        int y=i+block.y;
        char blockChar=block.bodyStr.charAt(block.width*i+j);
        if (blockChar!='_') {
          if (x<0||x>=10||y>=30||y<0) return true;
         if (((x>=0)&&(x<10)&&(y<30)&&(y>=0))&&(this->testView[x][y]!=0)) return true;
        };
      };
    };
  return false;
};

void cTetrisAgent::harddrop(sBlock &block){

  while (!this->checkForCollision(block)) {block.y+=1;};
  block.y-=1;
  this->consolidateTestView(block);


};
bool cTetrisAgent::rowIsEmpty(int y){
  for (int x=0;x<10;x++) {
      if (this->testView[x][y]>0) return false;
  };
  return true;
};
int cTetrisAgent::getTowerHeight(){
  int row=0;

  while (this->rowIsEmpty(row)) {
    row++;
  };
  int height=30-row;

  return height;
};
bool cTetrisAgent::hasFilledFieldsAbove(int x,int y){
 // cEliteSystem::log("fieldsabove");
  int sum=0;
  for (int i=0;i<y;i++) {
  if (this->testView[x][i]!=0) return true;
  };
 //if (this->testView[x][y-1]!=0) return true;
  return false;

};
int cTetrisAgent::getHoleCount(){
  int holes=0;
 // cEliteSystem::log("holecount");
  for (int y=29;y>0;y--) {
    for (int x=0;x<10;x++) {
      if ((this->testView[x][y]==0)&&(this->hasFilledFieldsAbove(x,y)==true)) holes+=1;
      };
  };
  return holes;
};

int cTetrisAgent::getColumnHeight(int x){
    int columnHeight=0;
    for (int y=0;y<30;y++) {
        if (this->testView[x][y]!=0) {columnHeight=30-y;return columnHeight;};
    };
    return columnHeight;
};

int cTetrisAgent::getColumnHeightSum(){
    int sum=0;
    for (int x=0;x<10;x++) {
        sum+=this->getColumnHeight(x);
    };
    return sum;
};

int cTetrisAgent::getBumpiness(){
    int sum=0;
    for (int x=0;x<9;x++) {
        int r1=this->getColumnHeight(x);
        int r2=this->getColumnHeight(x+1);
        int diff=0;
        if (r1>r2) diff=r2-r1;
        if (r2<r1) diff=r1-r2;
        sum+=diff;
    };

    return sum;
};

void cTetrisAgent::rotateBlock(sBlock &block){
    sBlock newBlock;
    newBlock=block;
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
      newBlock.bodyStr=rString;
      block=newBlock;

};

void cTetrisAgent::runSimulation(){
    this->restoreTestView();
    String s="current hole count :"+String(this->getHoleCount());
    s="current columnHeightSum :"+String(this->getColumnHeightSum());
    s="current bumpiness :"+String(this->getBumpiness());
    this->bestX=this->backupBlock.x;
    this->bestR=0;
    sBlock testBlock=this->backupBlock;
    this->restoreTestView();
    int towerHeight;
    int holeCount;
    int columnHeightSum;
    int numLinesCompleted;
    int bumpiness;
    this->listResults.clear();
    for (int r=0;r<=3;r++) {
        this->restoreTestView();
        testBlock=this->backupBlock;
        if (r==0) {};
        if (r==1) {this->rotateBlock(testBlock);};
        if (r==2) {this->rotateBlock(testBlock);this->rotateBlock(testBlock);};
        if (r==3) {this->rotateBlock(testBlock);this->rotateBlock(testBlock);this->rotateBlock(testBlock);};
        while (!this->checkForCollision(testBlock)) {testBlock.x-=1;};
        testBlock.x+=1;
        int limitLeft=testBlock.x;
        while (!this->checkForCollision(testBlock)) {testBlock.x+=1;};
        testBlock.x-=1;
        int limitRight=testBlock.x;
        for (int testX=limitLeft;testX<=limitRight;testX++) {
            testBlock.x=testX;
            testBlock.y=this->backupBlock.y;
            this->restoreTestView();
            this->harddrop(testBlock);
            numLinesCompleted=this->removeAndConsolidateLines();
            towerHeight=this->getTowerHeight();
            columnHeightSum=this->getColumnHeightSum();
            holeCount=this->getHoleCount();
            bumpiness=this->getBumpiness();
            sResultSet result={towerHeight,holeCount,columnHeightSum,bumpiness,numLinesCompleted,r,testX};
            this->listResults.add(result);
        };
    };
    this->processResults();
};


void cTetrisAgent::processResults(){
    float coeffHeight=-0.25f;
    float coeffHoles=-0.8f;//-0.8
    float coeffBump=-0.33;
    int bestResultIndex=0;
    float fbestScore;
    sResultSet currentResultSet=this->listResults.get(bestResultIndex);
    fbestScore=currentResultSet.holeCount*coeffHoles;
    fbestScore+=currentResultSet.columnHeightSum*coeffHeight;
    fbestScore+=currentResultSet.bumpinessSum*coeffBump;
    for (int i=0;i<this->listResults.size();i++) {
      float fscore;
      sResultSet currentResultSet=this->listResults.get(i);
      fscore=currentResultSet.holeCount*coeffHoles;
      fscore+=currentResultSet.columnHeightSum*coeffHeight;
      fscore+=currentResultSet.bumpinessSum*coeffBump;
      if (fscore>fbestScore) {fbestScore=fscore;bestResultIndex=i;};
    };
    sResultSet bestResult=listResults.get(bestResultIndex);
    this->bestX=bestResult.x;
    this->bestR=bestResult.rotationCount;
    this->listResults.clear();
};
