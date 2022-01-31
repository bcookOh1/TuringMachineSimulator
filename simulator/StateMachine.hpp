
#pragma once

// ref: https://boost-ext.github.io/sml/index.html
// https://boost-ext.github.io/sml/examples.html, hello world
#include <iostream>
#include <iomanip>
#include <functional>
#include <boost/sml.hpp>
#include "CommonDef.h"

using namespace std;
namespace sml = boost::sml;


// used in state machine and callback
struct BtnEnables {
   bool SelectTm;
   bool SetInputStr;
   bool Rewind;
   bool StepBack;
   bool Pause;
   bool StepForward;
   bool Run;
   bool SaveComputation;
}; // end struct

// callback from the state machine
template<typename T>
void SetBtnState(T btn, bool state) {(state == true ? btn->activate() : btn->deactivate());}


// anonymous namespace 
namespace {

// events 
struct eInit {};
struct eSelectTm {AppStatus status;};
struct eSetInput {AppStatus status;};
struct eTmReady {AppStatus status;};
struct ePause {AppStatus status;};
struct eLoop {AppStatus status;};
struct eStepForward {AppStatus status;};
struct eStepBackward {AppStatus status;};
struct eRewind {AppStatus status;};
struct eHalt {AppStatus status;};

// for this implementation, states are just empty classes
class Idle;
class SetTm;
class SetString;
class Paused;
class Running;
class Halt;

// transition table
// example: src_state +event<>[guard] / action = dst_state,
// transition from src_state to dst_state on event with guard and action
class tmsim_control_enables {
   using self = tmsim_control_enables;
public:

   tmsim_control_enables() {
      AllOff();
   } // end ctor 


   int SetStateMachineCB(std::function<void(const BtnEnables &)> cb){
      int ret = 0;
      _cb = cb; 
      return ret;
   } // end SetStateMachineCB


   int SetHasHistoryCB(std::function<bool(void)> hasHistory){
      int ret = 0;
      _hasHistory = hasHistory; 
      return ret;
   } // end SetHasHistoryCB


   auto operator()()  {
      using namespace sml;

      // guards
      auto validTm = [] (const auto &event) -> bool {
         return event.status.validTm;
      }; // end validTm

      auto validString = [] (const auto &event) -> bool {
         return event.status.validInput;
      }; // end validString

      auto inProgress = [] (const auto &event) -> bool {
         return event.status.tm == TmStatus::InProgress;
      }; // end inProgress

      auto ready = [] (const auto &event) -> bool {
         return event.status.tm == TmStatus::Ready;
      }; // end inProgress

      auto halt = [] (const auto &event) -> bool {
         return event.status.complete;
      }; // end halt

      return make_transition_table (

// example: src_state + event<eEvent>[guard] / action = dst_state,

         *state<Idle> + event<eInit> / [] {cout << "SetTm\n"; } = state<SetTm>,
         state<SetTm> + event<eSelectTm>[validTm] / [] {cout << "SetString\n"; } = state<SetString>,
         state<SetString> + event<eSetInput>[validString] / [] {cout << "Paused\n"; } = state<Paused>,

         state<Paused> + event<eLoop>[(ready || inProgress)] / [] {cout << "Running\n"; } = state<Running>,
         state<Paused> + event<eStepForward>[inProgress] / [] {cout << "Paused\n"; } = state<Paused>,
         state<Paused> + event<eStepBackward>[inProgress] / [] {cout << "Paused\n"; } = state<Paused>,
         state<Paused> + event<ePause> / [] {cout << "Paused\n"; } = state<Paused>,
         
         state<Running> + event<eLoop>[inProgress] / [] {cout << "Running\n"; } = state<Running>,
         state<Running> + event<ePause>[inProgress] / [] {cout << "Paused\n"; } = state<Paused>,
         state<Running> + event<eLoop>[inProgress] / [] {cout << "Running\n"; } = state<Running>,
         state<Running> + event<eHalt>[halt] / [] {cout << "Paused\n"; } = state<Paused>,

         state<Halt> + event<eRewind> / [] {cout << "Paused\n"; } = state<Paused>, 
         state<Halt> + event<eStepBackward> / [] {cout << "Paused\n"; } = state<Paused>,

         state<SetTm> + sml::on_entry<sml::back::_> / [&] { setTmAction(); },
         state<SetString> + sml::on_entry<sml::back::_> / [&] { setStringAction();},
         state<Paused> + sml::on_entry<sml::back::_> / [&] { pausedAction(); },
         state<Running> + sml::on_entry<sml::back::_> / [&] { runningAction(); },
         state<Halt> + sml::on_entry<sml::back::_> / [&] { haltAction(); }

      );

   } // end operator() 

private:

   BtnEnables _be; 
   std::function<void(const BtnEnables &)> _cb; 
   std::function<bool(void)> _hasHistory; 

   void AllOff() {
      _be.SelectTm = false;
      _be.SetInputStr = false;
      _be.Rewind = false;
      _be.StepBack = false;
      _be.Pause = false;
      _be.StepForward = false;
      _be.Run = false;
      _be.SaveComputation = false;
   } // end DisableGroupControlBtns

   void setTmAction(){
      AllOff(); 
      _be.SelectTm = true;
      _cb(_be);
   } // end setTmAction

   void setStringAction(){
      AllOff();
      _be.SelectTm = true;
      _be.SetInputStr = true; 
      _be.SaveComputation = true;
      _cb(_be);
   } // end setStringAction

   void pausedAction(){
      AllOff(); 
      _be.SelectTm = true;
      _be.SetInputStr = true; 
      _be.Run = true;
      _be.StepForward = true; 
      _be.SaveComputation = true; 
      
      if(_hasHistory()) {
         _be.Rewind = true;
         _be.StepBack = true; 
      } // end if 
      
      _cb(_be);
   } // end pausedAction

   void runningAction(){
      AllOff(); 
      _be.Pause = true; 
      _cb(_be);
   } // end runningAction

   void haltAction(){
      AllOff(); 

      if(_hasHistory()) {
         _be.Rewind = true;
         _be.StepBack = true; 
      } // end if 

      _be.SaveComputation = true; 
      _cb(_be);
   } // end haltAction

}; // end struct

} // end anonymous namespace


