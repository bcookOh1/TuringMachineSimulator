
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
   _btnRewind->callback([](Fl_Widget *w, void *p){ToThis(w)->RewindClk();});
   next += span;

   _btnStepBack = new Fl_Button(next, this->y() + 16, 30, 30, "@<");
   _btnStepBack->labelsize(18);
   _btnStepBack->tooltip("step back");
   _btnStepBack->callback([](Fl_Widget *w, void *p){ToThis(w)->StepBackClk();});
   next += span;
   
   _btnPause = new Fl_Button(next, this->y() + 16, 30, 30, "@||");
   _btnPause->labelsize(18);
   _btnPause->tooltip("pause");
   _btnPause->callback([](Fl_Widget *w, void *p){ToThis(w)->PauseClk();});
   next += span;
   
   _btnStepForward = new Fl_Button(next, this->y() + 16, 30, 30, "@>");
   _btnStepForward->labelsize(18);
   _btnStepForward->tooltip("step forward");
   _btnStepForward->callback([](Fl_Widget *w, void *p){ToThis(w)->StepForwardClk();});
   next += span;
   
   _btnRun = new Fl_Button(next, this->y() + 16, 30, 30, "@>[]");
   _btnRun->labelsize(18);
   _btnRun->tooltip("run");
   _btnRun->callback([](Fl_Widget *w, void *p){ToThis(w)->RunClk();});
   
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

   switch(state){
   case RunControlState::NotReady:
      _btnRewind->deactivate();
      _btnStepBack->deactivate();
      _btnPause->deactivate();
      _btnStepForward->deactivate();
      _btnRun->deactivate();
      break;
   case RunControlState::Ready:
      _btnRewind->deactivate();
      _btnStepBack->deactivate();
      _btnPause->deactivate();
      _btnStepForward->deactivate();
      _btnRun->deactivate();
      break;
   case RunControlState::Complete:
      _btnRewind->deactivate();
      _btnStepBack->deactivate();
      _btnPause->deactivate();
      _btnStepForward->deactivate();
      _btnRun->deactivate();
      break;
   case RunControlState::Running:
      _btnRewind->deactivate();
      _btnStepBack->deactivate();
      _btnPause->deactivate();
      _btnStepForward->deactivate();
      _btnRun->deactivate();
      break;
   case RunControlState::Paused:
      _btnRewind->deactivate();
      _btnStepBack->deactivate();
      _btnPause->deactivate();
      _btnStepForward->deactivate();
      _btnRun->deactivate();
      if(atBeginning == false){
         _btnRewind->deactivate();
         _btnStepBack->deactivate();
         _btnPause->deactivate();
         _btnStepForward->deactivate();
         _btnRun->deactivate();
      } // end if 
      break;
   } // end switch

   return;
} // end SetControlEnables


