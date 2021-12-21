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
#include "CommonDef.h"
#include "GroupTuringTape.h"
#include <string>
#include <tuple>


// sets of colors a = lighter
const Fl_Color s1a = fl_rgb_color(0xB0, 0xC0, 0xCF);
const Fl_Color s1b = fl_rgb_color(0x70, 0x90, 0xA0);
const Fl_Color s1c = fl_rgb_color(0x5F, 0x6F, 0x6F);

// forwards 
class Transition;
class TuringMachine;


class MainWind : public Fl_Double_Window {
private:

   // fltk gui controls
   GroupTuringTape* _grpTuringTape;
   Fl_Output* _outFilename;
   Fl_Button* _btnFileDialog;
   Fl_Browser* _bwrComputation;
   Fl_Output* _outDescription;
   Fl_Input_Choice* _icSpeed;
   Fl_Input* _inString;
   Fl_Button* _btnLoadString;
   Fl_Light_Button* _btnRun;
   Fl_Output* _outStatus;
   Fl_Button* _btnSaveComputation;
   Fl_Box* _lbRunState;
   Fl_Box* _lbExample;

   Fl_Box* _lbSeq1;
   Fl_Box* _lbSeq2;
   Fl_Box* _lbSeq3;
   Fl_Box* _lbSeq4;


   // member data
   bool _validFile; 
   bool _validInputString;
   bool _running;
   bool _complete;
   float _runRate;
   std::string _partialfname;
   std::string _definitionfile;
   TuringMachine *_tm;  // turing machine 

   // control callbacks
   static void ShowDefinitionFileDialogCB(Fl_Widget *widget, void *param);
   static void RunButtonCB(Fl_Widget *widget, void *param);
   static void ShowSaveComputationFileDialogCB(Fl_Widget *widget, void *param);
   static void UserInputStringCB(Fl_Widget *widget, void *param);
   static void RunTimerCB(void *data);

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
   void SetRunStatusBox(RunState rs);

   int SetupGUITape(TmConfiguration configuration);
   void SetStatus(RunState rs);

   // used in gv file creation
   std::string _gvFullPath;


public:
   MainWind(int w, int h, const char* title);
   ~MainWind();

}; // end class


