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


const std::string Title = "run controls";
enum class GroupButton : int {
   None = 0,
   Rewind,
   StepBack,
   Pause,
   StepForward,
   Run
}; // end enum

// subclass the Fl_Scroll control to allow long 
// but fixed size tape, simpler then trying to make a 
// (pseudo) infinite tape by prepending or appending positions   
class GroupRunControls : public Fl_Group { 
private:

   static const int _h = 53;
   static const int _w = 290;

   /////////  01-04-222 
   Fl_Input_Choice* _icSpeed;
   Fl_Button* _btnRewind;
   Fl_Button* _btnStepBack;
   Fl_Button* _btnPause;
   Fl_Button* _btnStepForward;
   Fl_Button* _btnRun;

   GroupButton _clicked;

public:
   GroupRunControls(int x, int y);
   ~GroupRunControls();

   GroupButton GetClickedBtn(){ return _clicked; };
   std::string GetSpeedString() {return _icSpeed->value();} 

   void RewindClick(); 
   void StepBackClick();
   void PauseClick();
   void StepForwardClick();
   void RunClick();

   void DeactivateAll();
   void SetRewindBtnState(bool state) { (state == true ? _btnRewind->activate() : _btnRewind->deactivate());}
   void SetStepBackBtnState(bool state) { (state == true ? _btnStepBack->activate() : _btnStepBack->deactivate());}
   void SetPauseBtnState(bool state) { (state == true ? _btnPause->activate() : _btnPause->deactivate());}
   void SetStepForwardBtnState(bool state) { (state == true ? _btnStepForward->activate() : _btnStepForward->deactivate());}
   void SetRunBtnState(bool state) { (state == true ? _btnRun->activate() : _btnRun->deactivate());}

}; // end class

// must come after the class, used in callback lambdas,
// it's a helper, allowing compact code in the lambdas 
inline GroupRunControls* ToThis(void *param) {return static_cast<GroupRunControls*>(param);}
