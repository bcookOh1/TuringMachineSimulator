// file: MainWind.h
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/7/2018
// Description: header for the main gui window. this is the main gui window class
// instantiated from the main.cpp. It's the main "controller" for the program.  
// It contains all of the GUI elements, the TuringMachine class, and 
// calls the TuringMachineFactory class from TmFactory(). 
// The fltk gui library uses callbacks for element events. The callbacks
// are required to be static with a this pointer passed in. The callback
// functions use the a cast like this: "static_cast<MainWind*>(param)->" to
// access the MainWind members. 
// note: see the .cpp implementation for function comments

 
#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <boost/process.hpp>
#include <cassert>
#include <string>
#include <tuple>
#include <ipcq.h>
#include "CommonDef.h"
// #include "StateMachine.hpp"
#include "GroupTuringTape.h"
#include "GroupRunControls.h"


namespace bp = boost::process;
//namespace sml = boost::sml;
//using Cfsm = tmsim_control_enables;


// sets of colors a = lighter
const Fl_Color s1a = fl_rgb_color(0xB0, 0xC0, 0xCF);
const Fl_Color s1b = fl_rgb_color(0x70, 0x90, 0xA0);
const Fl_Color s1c = fl_rgb_color(0x5F, 0x6F, 0x6F);

// forwards 
class Transition;
class TuringMachine;


class MainWind : public Fl_Double_Window {
public:

   MainWind(int w, int h, const char* title);
   ~MainWind();

   void SetTmSelectBtnState(bool state); 
   void SetLoadStringBtnState(bool state); 
   void SetSaveComputationBtnState(bool state); 
   void DisableAllGrpRunControls();
   void SetRewindBtnState(bool state); 
   void SetStepBackBtnState(bool state);
   void SetPauseBtnState(bool state);
   void SetStepForwardBtnState(bool state);
   void SetRunBtnState(bool state);
   TmStatus GetTmStatus();
   bool ValidTmFile();
   bool ValidInputString();
   int TmStepBackHistorySize();


private:

   // fltk gui controls
   GroupTuringTape* _grpTuringTape;
   Fl_Output* _outFilename;
   Fl_Button* _btnFileDialog;
   Fl_Browser* _bwrComputation;
   Fl_Output* _outDescription;
   Fl_Input* _inString;
   Fl_Button* _btnLoadString;
   Fl_Output* _outStatus;
   Fl_Button* _btnSaveComputation;
   Fl_Box* _lbRunState;
   Fl_Box* _lbExample;

   Fl_Box* _lbSeq1;
   Fl_Box* _lbSeq2;
   Fl_Box* _lbSeq3;
   Fl_Box* _lbSeq4;

   /////////  01-04-222 
   GroupRunControls* _grpRunControls;

   // state machine
   //sm<Cfsm> *_sm;

   // member data

   // flags to set the user control group 
   bool _validFile; 
   bool _validInputString;
   RunControlState _runState;
   bool _running;
   bool _paused;
   bool _complete;

   float _runRate;
   std::string _partialfname;
   std::string _definitionfile;
   TuringMachine *_tm;  // turing machine 

   ipcq::IpcQueueWriter *_ipcqWriter;
   std::unique_ptr<bp::child> _child;

   // control callbacks
   static void ShowDefinitionFileDialogCB(Fl_Widget *widget, void *param);
   static void ShowSaveComputationFileDialogCB(Fl_Widget *widget, void *param);
   static void UserInputStringCB(Fl_Widget *widget, void *param);
   static void MainWindOnCloseCB(Fl_Widget *wind, void *data);
   static void RunTimerCB(void *data);
   static void GroupControlCB(Fl_Widget *widget, void *param);

   std::string GetDefinitionfile();
   void SetDefinitionfile(const std::string &filename);
   void SetupControls(int w, int h);

   void ResetHeadPosition() { _grpTuringTape->ResetHeadPosition(); }
   int SetTapeSymbolAndMoveTheHead(Transition transition);
   void TmFactory();

   int WriteComputationToFile(const std::string &filename);
   int TestInputString();
   int SetInputStringInTM();

   void SetControlEnables();
   float GetSequencerRunRate();
   void SetRunStatusBox(TmStatus rs);

   int SetupGUITape(TmConfiguration configuration);
   void SetStatus(TmStatus rs);

   // used in gv file creation
   std::string _gvFullPath;

   RunControlState GetControlState();

   void RunButton();
   void PauseButton();
   void StepForward();
   void StepBackward();
   void InitializeTm();



}; // end class


