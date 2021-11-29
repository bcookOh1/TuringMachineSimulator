// file: TapeHead.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: header for FLTK Fl_Box used to simulate
// the head in the Turing Machine visual simulator 
// note: see the .cpp implementation for function comments


#pragma once

#include "FL\Fl_Group.H"
#include <FL\Fl_Box.H>
#include <FL\Fl_Output.H>
#include <string>

class TapeHead { 
private:

   const int WIDTH = 35;
   const int HEIGHT = 40;
   const std::string LABEL = "q0";

   Fl_Box* _boxBorder;
   int _xPosition;

public:

   TapeHead(int x, int y);
   ~TapeHead();

   int MoveTo(int x, int y);
   int SetState(std::string state);

   void SetxPosition(int x){ _xPosition = x;}
   int GetxPosition() { return _xPosition; }

   void Show(){ _boxBorder->show();}
   void Hide() { _boxBorder->hide(); }

}; // end class


