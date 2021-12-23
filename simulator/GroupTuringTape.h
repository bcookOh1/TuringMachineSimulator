// file: GroupTuringTape.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: header for the FLTK group control for the 
// Turing Machine visual simulator 
// note: see the .cpp implementation for function comments

#pragma once

#include <FL\Fl.H>
#include <FL\fl_draw.H>
#include <FL\Fl_Group.H>
#include <FL\Fl_Scroll.H>
#include <FL\Fl_Pack.H>
#include <FL\Fl_Box.H>
#include "CommonDef.h"
#include "TapePosition.h"
#include "TapeHead.h"
#include <vector>
#include <string>

// subclass the Fl_Scroll control to allow long 
// but fixed size tape, simpler then trying to make a 
// (pseudo) infinite tape by prepending or appending positions   
class GroupTuringTape : public Fl_Scroll { 

private:

   // these are related, the tape size is 
   // big enough to fit 100 positions with a head offset of 43  
   // class constants 
   const int HEAD_OFFSET = 43;
   const int TAPE_SIZE = 4300; 
   const int Y_COORD_VIEW_POSITION = -14; // y coord after initialize
   const std::size_t NUMBER_TAPE_POSITIONS = 100;   
   const int TWO_WAY_LEFT_INDEX = -49; 
   const std::string TAPE_BOX_LABEL = "tape";
   static const int _width = 535;
   static const int _height = 130;

   int _initialXcoord;
   int _initialYcoord;

   Fl_Box* _tape;
   TapeHead* _head;

   std::vector<TapePosition*> _positions;
   int _headXcoord;

   int _initialHeadxPosition;
   int _initialHeadyPosition;

   std::vector<TapeHead*> _headPositions;
   std::size_t _currentHeadPosition;
   std::size_t GetZeroIndex();

   // left index value for the standard or two way configurations  
   int _leftIndexValue;

   void KeepHeadInView();

public:

   GroupTuringTape(int x, int y);
   ~GroupTuringTape();

   void SetConfiguration(TmConfiguration configuration);

   int GetNumberOfTapePositions(){return NUMBER_TAPE_POSITIONS;}

   int SetSymbolAt(int idx, std::string symbol);
   int SetIndexAt(int idx, int index);

   int GetLeftIndexValue() { return _leftIndexValue;}

   void SetTapeToB();
   int SetInitialHeadPosition();
   void ResetHeadPosition();

   int SetHeadState(std::string state);
   int MoveHeadRight();
   int MoveHeadLeft();

}; // end class

