// file: GroupRunControls.h
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 01/004/2022
// Description: header for the FLTK group that controls the running the  
// Turing Machine visual simulator 
// note: see the .cpp implementation for function comments

#pragma once

#include <FL\Fl.H>
#include <FL\fl_draw.H>
#include <FL\Fl_Group.H>
#include <FL\Fl_Input_Choice.H>
#include <FL\Fl_Button.H>
#include <FL\Fl_Pack.H>
#include <string>
#include <iostream>

#include "CommonDef.h"

using namespace std;

class MainWind;

const std::string Title = "run controls";


// subclass the Fl_Scroll control to allow long 
// but fixed size tape, simpler then trying to make a 
// (pseudo) infinite tape by prepending or appending positions   
class GroupRunControls : public Fl_Group { 
private:

   MainWind *_mainwind;

   static const int _h = 53;
   static const int _w = 290;

   /////////  01-04-222 
   Fl_Input_Choice* _icSpeed;
   Fl_Button* _btnRewind;
   Fl_Button* _btnStepBack;
   Fl_Button* _btnPause;
   Fl_Button* _btnStepForward;
   Fl_Button* _btnRun;

public:
   GroupRunControls(int x, int y);
   ~GroupRunControls();

   void SetMainWind(MainWind *mainwind);
   void SetControlEnables(RunControlState state, bool atBeginning);

   std::string GetSpeedString() {return _icSpeed->value();} 

   void RewindClick(); 
   void StepBackClick();
   void PauseClick();
   void StepForwardClick();
   void RunClick();

}; // end class

// must come after the class, used in callback lambdas,
// it's a helper, allowing compact code in the lambdas 
inline GroupRunControls* ToThis(void *param) {return static_cast<GroupRunControls*>(param);}
