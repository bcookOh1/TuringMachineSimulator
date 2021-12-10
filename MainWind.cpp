// file: MainWind.cpp
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: 
//
//

#include "MainWind.h"
#include "util.h"
#include "TuringMachine.h"
#include "Transition.h"
#include "TuringMachineFactory.h"
#include <fstream>
#include <iostream>
#include <iomanip>


// constructor 
MainWind::MainWind(int w, int h, const char* title) :
   Fl_Double_Window(20, 20, w, h, title) {
   SetupControls(w, h);
   
   _tm = nullptr;
   _validFile = false;
   _validInputString = false;
   _running = false;
   _complete = false;

} // end ctor

// destructor to cleanup on exit 
MainWind::~MainWind() {
   
   if(_tm != nullptr) {
      delete _tm;
   } // end if 

   // fltk deletes the widgets
} // end dtor

// a fuction to do the intial setup on the GUI elements 
void MainWind::SetupControls(int w, int h) {

   _grpTuringTape = new GroupTuringTape(25, 175); 

   _outFilename = new Fl_Output(25, 25, 490, 25, "input file ");
   _outFilename->align(Fl_Align(FL_ALIGN_TOP_LEFT));
   _outFilename->value("none selected");

   _btnFileDialog = new Fl_Button(525, 20, 35, 35, "@fileopen");
   _btnFileDialog->labelsize(20);
   _btnFileDialog->callback(ShowDefinitionFileDialogCB, this);

   _bwrComputation = new Fl_Browser(25, 330, 490, 180, "definition and computations"); 
   _bwrComputation->textfont(4);
   _bwrComputation->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _btnSaveComputation = new Fl_Button(525, 330, 35, 35, "@filesave");
   _btnSaveComputation->labelsize(20);
   _btnSaveComputation->callback(ShowSaveComputationFileDialogCB, this);

   _outDescription = new Fl_Output(25, 75, 250, 25, "description ");
   _outDescription->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _inString = new Fl_Input(290, 75, 225, 25, INPUT_STRING_LABEL.c_str());
   _inString->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _btnLoadString = new Fl_Button(525, 75, 35, 25, "set");
   // _btnLoadString->labelsize(20);
   _btnLoadString->callback(UserInputStringCB, this);
   _btnLoadString->shortcut(FL_CTRL | 'b');

   _lbExample = new Fl_Box(363, 59, 180, 15, "ex: ");
   _lbExample->align(Fl_Align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT));
   _lbExample->hide();

   _icSpeed = new Fl_Input_Choice(25, 133, 100, 25, "run speed");
   _icSpeed->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   // fill the choice selector from the Rates map and a lambda function 
   std::for_each(Rates.begin(), Rates.end(), [&](RatePair n) 
                                { _icSpeed->add(n.first.c_str()); });
   _icSpeed->value("medium");

   _btnRun = new Fl_Light_Button(140, 133, 80, 25, "run");
   _btnRun->callback(RunButtonCB, this);

   _lbRunState = new Fl_Box(245, 125, 315, 40, "ACCEPTED");
   _lbRunState->box(FL_THIN_UP_BOX);
   _lbRunState->labelfont(FL_BOLD + FL_ITALIC);
   _lbRunState->labelsize(20);
   SetRunStatusBox(RunState::Off);

   _outStatus = new Fl_Output(5, 535, 560, 25, "status");
   _outStatus->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _lbSeq1 = new Fl_Box(560, 20,  15,  15, "1");
   _lbSeq1->labelsize(14);
   _lbSeq1->labelcolor(FL_RED);
   _lbSeq2 = new Fl_Box(560, 71,  15,  15, "2");
   _lbSeq2->labelsize(14);
   _lbSeq2->labelcolor(FL_RED);
   _lbSeq3 = new Fl_Box(220, 133, 15,  15, "3");
   _lbSeq3->labelsize(14);
   _lbSeq3->labelcolor(FL_RED);
   _lbSeq4 = new Fl_Box(560, 330, 15, 15, "4");
   _lbSeq4->labelsize(14);
   _lbSeq4->labelcolor(FL_RED);

   end(); // fltk call, ends additions to gui tree

   SetControlEnables();

   return;
} // end SetupControls


// ShowDefinitionFileDialogCB callback for the _btnFileDialog to use the
// defintion file name to show a fikle dialog to let the user sselect a *.tm 
// file and then call the TmFactory() to create a _tm object 
void MainWind::ShowDefinitionFileDialogCB(Fl_Widget *widget, void *param) {

   std::string desktop = GetDesktopPath();

   // hide the status box 
   static_cast<MainWind*>(param)->SetRunStatusBox(RunState::Off);

   Fl_File_Chooser chooser(desktop.c_str(), "*.tm", 
                           Fl_File_Chooser::SINGLE, "Turing Machine");
   chooser.directory(desktop.c_str());

   chooser.show();

   // block until user selects a button
   while(chooser.shown()) Fl::wait();

   // if user selected a file clear th internal status an setup the tm 
   if(chooser.value() != NULL) {

      static_cast<MainWind*>(param)->_validFile = false;

      // clear the input string 
      static_cast<MainWind*>(param)->_validInputString = false;
      static_cast<MainWind*>(param)->_inString->value("");
      static_cast<MainWind*>(param)->_inString->color(FL_BACKGROUND2_COLOR);

      static_cast<MainWind*>(param)->SetDefinitionfile(chooser.value(0));
      static_cast<MainWind*>(param)->TmFactory();
      static_cast<MainWind*>(param)->_grpTuringTape->SetConfiguration(
                             static_cast<MainWind*>(param)->_tm->GetConfigurationType());

      static_cast<MainWind*>(param)->SetControlEnables();

      // use _partialfname for writing the computation file  
      std::string fname(chooser.value(0));
      GetFilenameFromPath(fname);
      static_cast<MainWind*>(param)->_partialfname = fname;

      // bcook 12-08-2021
      static_cast<MainWind *>(param)->_tm->WriteGraphvizDotFile(fname);

   } // end if 

   return;
} // end ShowDefinitionFileDialogCB


// RunButtonCB callback for _btnRun button, this make sure everything is 
// setup and then it starts the sequncer timer 
void MainWind::RunButtonCB(Fl_Widget *widget, void *param) {

   // if btn value is 1 start the timer else stop the time 
   if(static_cast<Fl_Button*>(widget)->value() == 1) {

      // if the TM is not initialized like after a run then initialize
      if(static_cast<MainWind*>(param)->_tm->GetTmStatus() != TmStatus::Initialized) {
         static_cast<MainWind*>(param)->_tm->Initialize();
         static_cast<MainWind*>(param)->_grpTuringTape->SetTapeToB();
         static_cast<MainWind*>(param)->SetupGUITape(
                              static_cast<MainWind*>(param)->_tm->GetConfigurationType());
      } // end if 
      
      // get the run rate from the dropbox 
      static_cast<MainWind*>(param)->_runRate = 
                 static_cast<MainWind*>(param)->GetSequencerRunRate();
      
      // set status and start the timer 
      static_cast<MainWind*>(param)->_running = true;
      static_cast<MainWind*>(param)->_bwrComputation->clear();
      static_cast<MainWind*>(param)->SetRunStatusBox(RunState::Running);
      static_cast<MainWind*>(param)->_outStatus->value(RunStateText[
                                     static_cast<int>(RunState::Running)].c_str());

      // add the definition to the computation
      StringVector definition = static_cast<MainWind*>(param)->_tm->GetDefinition();
      std::for_each(definition.begin(), definition.end(), [&](std::string s) {
         static_cast<MainWind*>(param)->_bwrComputation->add(s.c_str());
      }); // end of lambda 

      Fl::add_timeout(static_cast<MainWind*>(param)->_runRate, RunTimerCB, param);
   } 
   else {
      static_cast<MainWind*>(param)->_running = false;
      static_cast<MainWind*>(param)->SetRunStatusBox(RunState::Canceled);
      static_cast<MainWind*>(param)->_outStatus->value(RunStateText[
                                     static_cast<int>(RunState::Canceled)].c_str());
      Fl::remove_timeout(RunTimerCB, param);
   } // end if 

   static_cast<MainWind*>(param)->SetControlEnables();

   return;
} // end RunButtonCB

// ShowSaveComputationFileDialogCB callback for the _btnSaveComputation button,
// this opens a file dialog to let the user select *.txt file to 
// the defintion and computation data into.
// Remark the dialog is preset to save at the user's desktop with 
// a filename that looks like this "computation_<definition_filename>.txt" 
void MainWind::ShowSaveComputationFileDialogCB(Fl_Widget *widget, void *param) {

   // setup the file chooser to save the computation text  
   std::string desktop = GetDesktopPath();

   Fl_File_Chooser chooser(desktop.c_str(), "*.txt", 
                           Fl_File_Chooser::CREATE, "Save Computation");
   chooser.ok_label("Save");
   chooser.directory(desktop.c_str());

   std::string fullPath = desktop + "\\" + COMPUTATION_FILE_NAME;
   fullPath = ReplaceSubstring(fullPath, PLACEHOLDER, 
                               static_cast<MainWind*>(param)->_partialfname);
   chooser.value(fullPath.c_str());

   chooser.show();

   // block until user selects a button
   while(chooser.shown()) Fl::wait();

   if(chooser.value() != NULL) {
      std::string fname(chooser.value());
      static_cast<MainWind*>(param)->WriteComputationToFile(fname);
   } // end if 

   return;
} // end ShowSaveComputationFileDialogCB


// UserInputStringCB, callback for the _btnLoadString button,
// this reads _inString, course test, and loads it to the _tm and 
// then calls SetupGUITape() to set up the GUI
void MainWind::UserInputStringCB(Fl_Widget *widget, void *param) {

   if(static_cast<MainWind*>(param)->_validFile == true) {
      int ret = static_cast<MainWind*>(param)->TestInputString();
      if(ret == 0) {
         static_cast<MainWind*>(param)->_validInputString = true;
         static_cast<MainWind*>(param)->SetInputStringInTM();

         // initialize _tm with the input string 
         static_cast<MainWind*>(param)->_tm->Initialize();
         static_cast<MainWind*>(param)->_grpTuringTape->SetTapeToB();
         static_cast<MainWind*>(param)->SetupGUITape(static_cast<MainWind*>(param)->
                                       _tm->GetConfigurationType());

      }
      else {
         static_cast<MainWind*>(param)->_validInputString = false;
      } // end if 

      static_cast<MainWind*>(param)->SetControlEnables();

   } // end if  

   return; 
} // end UserInputString


// return 0 match
// return -1 empty
// return -2 not valid
int MainWind::TestInputString() {
   int ret = 0;

   std::string input(_inString->value());

   StringSet combined = _tm->GetAlphabet();
   StringSet tapeSymbols = _tm->GetTapeSymbols();
   combined.insert(tapeSymbols.begin(), tapeSymbols.end());

   ret = IsStringInAlphabet(combined, input);
   if(ret != 0) {
      _outStatus->value("invalid input string");
      _inString->color(FL_RED);
   }
   else {
      _outStatus->value("valid input string");
      _inString->color(FL_BACKGROUND2_COLOR);
   } // end if 

   return ret;
} // end TestInputString


// SetInputStringInTM, set the input string in the _tm object
// return 0 success
int MainWind::SetInputStringInTM() {
   int ret = 0;

   std::string input(_inString->value());
   _tm->SetInputString(input);

   return ret;
} // end SetInputStringInTM


// this is the sequencer for the TM, it calls _tm->TransitionStep()
// and then looks at the tm status to decide how to continue or stop 
void MainWind::RunTimerCB(void *data) {
   TmStatus status = TmStatus::InProgress;

   Transition transition;
   std::string computation;

   // return 0 success
   // return -1 something went wrong 
   // sets _status to:
   // TmStatus::InProgress, valid next transition
   // TmStatus::CompleteOnEmptyTransition, no final states
   // TmStatus::AcceptedOnFinalState, if there are final states
   // TmStatus::RejectedNotOnFinalState, if there are final states
   // TmStatus::Abnormal_Termination, two - way left on #
   // TmStatus::SomethingWentWrong, should not happen, but ?
   int ret = static_cast<MainWind*>(data)->_tm->TransitionStep();
   status = static_cast<MainWind*>(data)->_tm->GetTmStatus();
   if(0 == ret){

      transition = static_cast<MainWind*>(data)->_tm->GetCurrentTransition();

      switch(status) {

      case TmStatus::InProgress:

         static_cast<MainWind*>(data)->SetTapeSymbolAndMoveTheHead(transition);

         // update the browser list 
         computation = static_cast<MainWind*>(data)->_tm->GetComputation();
         static_cast<MainWind*>(data)->_bwrComputation->add(computation.c_str());

         // restart the timer 
         Fl::repeat_timeout(static_cast<MainWind*>(data)->_runRate, RunTimerCB, data);
      
         break;

      // complete on empty transition but no final states in tm 
      case TmStatus::CompleteOnEmptyTransition:

         static_cast<MainWind*>(data)->_btnRun->value(0);
         RunButtonCB(static_cast<MainWind*>(data)->_btnRun, data);

         // write result to GUI
         static_cast<MainWind*>(data)->_bwrComputation->add(RunStateText[static_cast<std::size_t>(
                                       RunState::CompleteOnEmptyTransition)].c_str());
         static_cast<MainWind*>(data)->SetStatus(RunState::CompleteOnEmptyTransition);

         break;

      // only if there are final states 
      case TmStatus::AcceptedOnFinalState:

         static_cast<MainWind*>(data)->_btnRun->value(0);
         RunButtonCB(static_cast<MainWind*>(data)->_btnRun, data);

         // write result to GUI
         static_cast<MainWind*>(data)->_bwrComputation->add(RunStateText[static_cast<std::size_t>(
                                       RunState::AcceptedOnFinalState)].c_str());
         static_cast<MainWind*>(data)->SetStatus(RunState::AcceptedOnFinalState);

         break;

      case TmStatus::RejectedNotOnFinalState:

         static_cast<MainWind*>(data)->_btnRun->value(0);
         RunButtonCB(static_cast<MainWind*>(data)->_btnRun, data);

         // write result to GUI
         static_cast<MainWind*>(data)->_bwrComputation->add(RunStateText[static_cast<std::size_t>(
                                       RunState::RejectedNotOnFinalState)].c_str());
         static_cast<MainWind*>(data)->SetStatus(RunState::RejectedNotOnFinalState);

         break;

      // only if two_way and left on #
      case TmStatus::InvalidLeftMove:

         static_cast<MainWind*>(data)->_btnRun->value(0);
         RunButtonCB(static_cast<MainWind*>(data)->_btnRun, data);

         // write result to GUI
         static_cast<MainWind*>(data)->_bwrComputation->add(RunStateText[static_cast<std::size_t>(
                                      RunState::InvalidLeftMove)].c_str());
         static_cast<MainWind*>(data)->SetStatus(RunState::InvalidLeftMove);

         break;

      // should not happen 
      default: 
         break;
      } // end switch 

   }
   else {

      static_cast<MainWind*>(data)->_btnRun->value(0);
      RunButtonCB(static_cast<MainWind*>(data)->_btnRun, data);
      static_cast<MainWind*>(data)->SetStatus(RunState::SomethingWentWrong);

   } // end if 

   return;
} // end RunButtonCB


// returns the definiton file memebr 
std::string MainWind::GetDefinitionfile() {
   return _definitionfile;
} // end GetDefinitionfile


// SetTapeSymbolAndMoveTheHead using transition param
// return 0 on success 
// return -1 on GUI tape limit hit
int MainWind::SetTapeSymbolAndMoveTheHead(Transition transition) {
   int ret = 0;

   int leftIndexValue = _grpTuringTape->GetLeftIndexValue();

   _grpTuringTape->SetSymbolAt(transition.GetIndex() - leftIndexValue, transition.GetSymbol());

   if(transition.GetDirection() == TAPE_RIGHT_SYMBOL)
      ret = _grpTuringTape->MoveHeadRight();
   else
      ret = _grpTuringTape->MoveHeadLeft();

   _grpTuringTape->SetHeadState(transition.GetState());

   return ret;
} // end SetTapeSymbolAndMoveTheHead


// SetDefinitionfile, set the definiton file member and the 
// _outFilename text box
void MainWind::SetDefinitionfile(const std::string &filename) {
   
   _definitionfile = filename;
   _outFilename->value(filename.c_str());
   _outFilename->position(filename.length());

   return;
} // SetDefinitionfile


// SetControlEnables, set the active/deactive state on 
// the gui controls to help the user operate the program
void MainWind::SetControlEnables() {

   _btnFileDialog->deactivate();
   _inString->deactivate();
   _btnLoadString->deactivate();
   _lbExample->deactivate();
   _btnRun->deactivate();

   if(_validFile == true) {
      _inString->activate();
      _btnLoadString->activate();
      _lbExample->activate();
   } // end if 
   
   if(_validFile == true && _validInputString == true) {
      _icSpeed->activate();
      _btnRun->activate();
   } // end if 

   if(_running == true) {
      _btnFileDialog->deactivate();
      _inString->deactivate();
      _btnLoadString->deactivate();
      _lbExample->deactivate();
      _icSpeed->deactivate();
   }
   else {
      _btnFileDialog->activate();
      _icSpeed->activate();
   } // end if 

   if(_bwrComputation->size() > 0 && _running == false) {
      _btnSaveComputation->activate();
   }
   else {
      _btnSaveComputation->deactivate();
   } // end if 
   
   return;
} // end SetControlEnables


// GetSequencerRunRate, return the seconds float value for the 
// selected _icSpeed dropbox value 
// return seconds sequencer rate
float MainWind::GetSequencerRunRate() {
   float ret = 0.0f;
   std::string rateString = _icSpeed->value();

   auto iter = Rates.find(rateString);
   if(iter != Rates.end()){
      ret = iter->second;
   }
   else {
      assert(false);
   } // end if 

   return ret;
} // end GetSequencerRunRate


// TmFactory, this does the definition file readin and creates the 
// new _tm object. if an error occurs during the readin, _tm 
// is not created and the errors are shown in the status text box 
void MainWind::TmFactory() {

   _validFile = false;

   // delete the old _tm if it exists  
   if(_tm != nullptr){
      delete _tm;
      _tm = nullptr;
   } // end if 
   
   TuringMachineFactory tmf;
   int ret = tmf.ReadinDefinitionFile(_definitionfile);
   if(ret == 0) {
      _tm = tmf.GetTuringMachine();
      _outDescription->value(_tm->GetDescription().c_str());

      std::string label = "ex: " + _tm->GetInputExample();
      _lbExample->copy_label(label.c_str());
      _lbExample->show();

      _validFile = true;
      _outStatus->value("valid definition file loaded");

      _inString->value(_tm->GetInputExample().c_str());

   }
   else {
      _outStatus->value(tmf.GetErrorString().c_str());
   } // end if 
   
   return; 
} // end TmFactory


// WriteComputationToFile, called from the 
// ShowSaveComputationFileDialogCB to write the _bwrComputation strings
// to a file from filename (a path)
// return 0 success
// return -1 nothing to write
int MainWind::WriteComputationToFile(const std::string &filename) {
   int ret = 0;

   // return if the browser is empty 
   int count = _bwrComputation->size();
   if(count == 0) return -1;

   // open the file and overwrite if already exists 
   std::ofstream outfile;
   outfile.open(filename.c_str(),std::ofstream::trunc);

   if(outfile.fail()) {
      _outStatus->value("cannot open the computation file for writing");
      return -1;
   } // end if 

   // write the content of the fl_browser to the file 
   // start at index 1, querk of fl_browser 
   for(int i = 1; i <= count; i++) {

      const char *l = _bwrComputation->text(i);
      std::string line(l);

      line += '\n';
      outfile << line;
   } // end for  

   outfile.close();
   _outStatus->value("definition and computations file save to disk");

   return ret;
} // end WriteComputationToFile


// SetRunStatusBox, use the GUI side RunState and set 
// the run state box widget label and color 
void MainWind::SetRunStatusBox(RunState rs) {

   switch(rs) {
      case RunState::CompleteOnEmptyTransition:
      _lbRunState->color(FL_GREEN);
      _lbRunState->label(RunStateText[static_cast<int>(
                         RunState::CompleteOnEmptyTransition)].c_str());
      _lbRunState->show();
      break;
      case RunState::AcceptedOnFinalState:
      _lbRunState->color(FL_GREEN);
      _lbRunState->label(RunStateText[static_cast<int>(
                         RunState::AcceptedOnFinalState)].c_str());
      _lbRunState->show();
      break;
   case RunState::RejectedNotOnFinalState:
      _lbRunState->color(FL_RED);
      _lbRunState->label(RunStateText[static_cast<int>(
                         RunState::RejectedNotOnFinalState)].c_str());
      _lbRunState->show();
      break;
   case RunState::Canceled:
      _lbRunState->color(FL_CYAN);
      _lbRunState->label(RunStateText[static_cast<int>(
                         RunState::Canceled)].c_str());
      _lbRunState->show();
      break;
   case RunState::Running:
      _lbRunState->color(FL_BACKGROUND_COLOR);
      _lbRunState->label(RunStateText[static_cast<int>(
                         RunState::Running)].c_str());
      _lbRunState->show();
      break;
   case RunState::InvalidLeftMove:
      _lbRunState->color(FL_RED);
      _lbRunState->label(RunStateText[static_cast<int>(
      RunState::InvalidLeftMove)].c_str());
      _lbRunState->show();
   break;
   case RunState::SomethingWentWrong:
      _lbRunState->color(FL_RED);
      _lbRunState->label(RunStateText[static_cast<int>(
                         RunState::SomethingWentWrong)].c_str());
      _lbRunState->show();
      break;
   case RunState::Off:
      _lbRunState->hide();
      break;
   } // end switch

   return;
} // end SetRunStatusBox


void MainWind::SetStatus(RunState rs) {

   SetRunStatusBox(rs);
   if(rs != RunState::Off)
      _outStatus->value(RunStateText[static_cast<int>(rs)].c_str());

   return;
} // end SetStatus


// SetupGUITape: setup the GUI tape and head with configuration, 
// standard or two-way
int MainWind::SetupGUITape(TmConfiguration configuration) {
   int ret = -1;

   int idx = 0;
   int positions = _grpTuringTape->GetNumberOfTapePositions();
   int leftIndexValue = _grpTuringTape->GetLeftIndexValue();

   TapeData tapeData;
   bool result = _tm->GetFirstTapePosition(tapeData);
   while(result == true) {

      if(idx < positions) {
         _grpTuringTape->SetSymbolAt(idx - leftIndexValue, std::get<0>(tapeData));
         _grpTuringTape->SetIndexAt(idx - leftIndexValue, std::get<1>(tapeData));
      } // end if 
      
      result = _tm->GetNextTapePosition(tapeData);
      idx++;
   } // end while 

   _grpTuringTape->SetHeadState(_tm->GetState());
   _grpTuringTape->SetInitialHeadPosition();

   return ret;
} // end SetupGUITape





