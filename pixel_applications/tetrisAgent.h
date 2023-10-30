//TODO : exclude square piece from rotation (amonst other redundant permutations) in brute force simulation procedure. for reasons. 
#pragma once
#include "tetris.h"

typedef struct {
  int towerHeight;
  int holeCount;  
  int columnHeightSum;  
  int bumpinessSum;  
  int numLinesCompleted;
  int rotationCount;
  int x;
}sResultSet;

class cTetrisAgent{
      private :
      
          
          uint8_t testView[10][30];//temporary, volatile copy of the playfield. used for simulating hard drops, 
          uint8_t backupView[10][30];//a copy of the playfield - must not touch - use to reset/restore the testView
          sBlock backupBlock;
          int bestX;
          int bestTowerHeight;
          int bestHoleCount;
          int bestR;
          float keyStrokeIntervall=0.5f;
          LinkedList<sResultSet> listResults;

      public :
          float fuse;
          cTetrisAgent();              
          void consolidateTestView(sBlock &block);
          bool checkForCollision(sBlock &block);
          int removeAndConsolidateLines();
          int getAction(float fElapsedTime,sBlock &currentBlock);
          bool hasFilledFieldsAbove(int x,int y);
          void rotateBlock(sBlock &block);
          int getHoleCount();
          void restoreTestView();
          bool rowIsEmpty(int y);
          void harddrop(sBlock &block);
          int getTowerHeight();
          int getColumnHeightSum();
          int getBumpiness();
          int getColumnHeight(int x);
          void runSimulation();
          void setKeyStrokeIntervall(float f);      
          void readPlayField(uint8_t* playFieldPtr,sBlock currentBlock);          
          void processResults();

};