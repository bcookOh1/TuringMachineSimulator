
#include "GroupRunControls.h"
#include "MainWind.h"


GroupRunControls::GroupRunControls(int x, int y) : 
   Fl_Group(x, y, _w, _h, Title.c_str()) {

   box(FL_BORDER_BOX);
   align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _icSpeed = new Fl_Input_Choice(this->x() + 6, this->y() + 18, 100, 25, "speed");
   _icSpeed->align(Fl_Align(FL_ALIGN_TOP));

   // fill the choice selector from the Rates map and a lambda function 
   std::for_each(Rates.begin(), Rates.end(), [&](RatePair n) 
                                { _icSpeed->add(n.first.c_str()); });
   _icSpeed->value("medium");

   int start = this->x() + 115; 
   int span = 35; 
   int next = start; 

   _btnRewind = new Fl_Button(next, this->y() + 16, 30, 30, "@|<");
   _btnRewind->labelsize(18);
   _btnRewind->tooltip("rewind");

   // ref: https://andreldm.com/2016/04/02/replacing-fltk-callbacks-lambdas.html
   // this callback/lambda is different from ref, it's the widget callback 
   // with 'p' as the param set to 'this' at the end of the callback() function 
   _btnRewind->callback([](Fl_Widget *w, void *p){ToThis(p)->RewindClick();}, this);
   next += span;

   _btnStepBack = new Fl_Button(next, this->y() + 16, 30, 30, "@<");
   _btnStepBack->labelsize(18);
   _btnStepBack->tooltip("step back");
   _btnStepBack->callback([](Fl_Widget *w, void *p){ToThis(p)->StepBackClick();}, this);
   next += span;
   
   _btnPause = new Fl_Button(next, this->y() + 16, 30, 30, "@||");
   _btnPause->labelsize(18);
   _btnPause->tooltip("pause");
   _btnPause->callback([](Fl_Widget *w, void *p){ToThis(p)->PauseClick();}, this);
   next += span;
   
   _btnStepForward = new Fl_Button(next, this->y() + 16, 30, 30, "@>");
   _btnStepForward->labelsize(18);
   _btnStepForward->tooltip("step forward");
   _btnStepForward->callback([](Fl_Widget *w, void *p){ToThis(p)->StepForwardClick();}, this);
   next += span;
   
   _btnRun = new Fl_Button(next, this->y() + 16, 30, 30, "@>[]");
   _btnRun->labelsize(18);
   _btnRun->tooltip("run");
   _btnRun->callback([](Fl_Widget *w, void *p){ToThis(p)->RunClick();}, this);
  
   end();

} // end ctor 

GroupRunControls::~GroupRunControls(){
   // fltk deletes the widgets 
} // end dtor 


// not ready
//    all deactive
// ready
//    enable forward step
//    enable run
// complete
//    enable rewind
// running 
//    enable pause
// paused
//    enable single step
//    enable run
//    if NOT at beginning
//        enable back step
//        enable rewind
void GroupRunControls::SetControlEnables(RunControlState state, bool atBeginning){

   _btnRewind->deactivate();
   _btnStepBack->deactivate();
   _btnPause->deactivate();
   _btnStepForward->deactivate();
   _btnRun->deactivate();

   switch(state){
   case RunControlState::NotReady:
      break;
   case RunControlState::Ready:
      _btnStepForward->activate();
      _btnRun->activate();
      if(atBeginning == false){
         _btnStepBack->activate();
      } // end if 
      break;
   case RunControlState::Complete:
      _btnRewind->activate();
      break;
   case RunControlState::Running:
      _btnPause->activate();
      break;
   case RunControlState::Paused:
      _btnStepForward->activate();
      _btnRun->activate();
      if(atBeginning == false){
         _btnRewind->activate();
         _btnStepBack->activate();
      } // end if 
      break;
   } // end switch

   return;
} // end SetControlEnables


void GroupRunControls::SetMainWind(MainWind *mainwind) {
   _mainwind = mainwind;
} // end SetMainWnd

void GroupRunControls::RewindClick() {
    _mainwind->InitializeTm();
   return;
} // end RewindClk

void GroupRunControls::StepBackClick() {
   _mainwind->StepBackward(); 
   return;
} // end StepBackClk

void GroupRunControls::PauseClick() { 
   _mainwind->PauseButton();
   return;
} // end PauseClk

void GroupRunControls::StepForwardClick() {
    _mainwind->StepForward(); 
   return;
} // end StepForwardClk

void GroupRunControls::RunClick() { 
   _mainwind->RunButton();
   return;
} // end RunClk



