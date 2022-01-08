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
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <iomanip>


// constructor 
MainWind::MainWind(int w, int h, const char* title) :
   Fl_Double_Window(20, 20, w, h, title) {
   SetupControls(w, h);
   
   this->callback(MainWindOnCloseCB);

   _tm = nullptr;
   _validFile = false;
   _validInputString = false;
   _running = false;
   _complete = false;
   _ipcqWriter = new ipcq::IpcQueueWriter(QueueName);

   // open the ipcq for writing
   int result = _ipcqWriter->Open();
   if(result != 0){
      std::cout << _ipcqWriter->GetErrorStr() << std::endl;
      exit(0);
   } // end if 


} // end ctor

// destructor to cleanup on exit 
MainWind::~MainWind() {
   
   if(_tm != nullptr) {
      delete _tm;
   } // end if 

   delete _ipcqWriter;

   // fltk deletes the widgets
} // end dtor

// event trap callback to ignore the escape key press 
// ref: http://www.fltk.org/articles.php?L378+I0+TFAQ+P1+Q
void MainWind::MainWindOnCloseCB(Fl_Widget *wind, void *data) {
   if(Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
      return; // ignore Escape

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(wind);
   
   // 12-25-2021 close the show_diagram process
   mainwind->_ipcqWriter->Push(static_cast<unsigned int>(IpcqCmd::Close),string("none"));

   mainwind->hide();

} // end MainWindOnCloseCB


// a fuction to do the intial setup on the GUI elements 
void MainWind::SetupControls(int w, int h) {

   _outFilename = new Fl_Output(25, 25, 490, 25, "input file ");
   _outFilename->align(Fl_Align(FL_ALIGN_TOP_LEFT));
   _outFilename->value("none selected");

   _btnFileDialog = new Fl_Button(525, 20, 35, 35, "@fileopen");
   _btnFileDialog->labelsize(20);
   _btnFileDialog->callback(ShowDefinitionFileDialogCB, this);

   _lbSeq1 = new Fl_Box(560, 20,  15,  15, "1");
   _lbSeq1->labelsize(14);
   _lbSeq1->labelcolor(FL_RED);
 
   _outDescription = new Fl_Output(25, 75, 250, 25, "description ");
   _outDescription->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _inString = new Fl_Input(290, 75, 225, 25, INPUT_STRING_LABEL.c_str());
   _inString->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _btnLoadString = new Fl_Button(525, 75, 35, 25, "set");
   _btnLoadString->callback(UserInputStringCB, this);
   _btnLoadString->shortcut(FL_CTRL | 'b');

   _lbSeq2 = new Fl_Box(560, 71,  15,  15, "2");
   _lbSeq2->labelsize(14);
   _lbSeq2->labelcolor(FL_RED);
 
   _lbExample = new Fl_Box(363, 59, 180, 15, "ex: ");
   _lbExample->align(Fl_Align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT));
   _lbExample->hide();

   _lbRunState = new Fl_Box(331, 127, 228, 40, "ACCEPTED");
   _lbRunState->box(FL_THIN_UP_BOX);
   _lbRunState->labelfont(FL_BOLD + FL_ITALIC);
   _lbRunState->labelsize(18);
   SetRunStatusBox(RunState::Off);
 
   _grpRunControls = new GroupRunControls(25,120);
   _grpRunControls->SetMainWnd(this);

   _lbSeq3 = new Fl_Box(315, 115, 15,  15, "3");
   _lbSeq3->labelsize(14);
   _lbSeq3->labelcolor(FL_RED);

   _grpTuringTape = new GroupTuringTape(25, 183);  

   _bwrComputation = new Fl_Browser(25, 340, 490, 220, "definition and computations");  
   _bwrComputation->textfont(4);
   _bwrComputation->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   _btnSaveComputation = new Fl_Button(525, 340, 35, 35, "@filesave");  
   _btnSaveComputation->labelsize(20);
   _btnSaveComputation->callback(ShowSaveComputationFileDialogCB, this);

   _lbSeq4 = new Fl_Box(560, 340, 15, 15, "4");  
   _lbSeq4->labelsize(14);
   _lbSeq4->labelcolor(FL_RED);

   _outStatus = new Fl_Output(5, 585, 560, 25, "status"); 
   _outStatus->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   end(); // fltk call, ends additions to gui tree

   SetControlEnables();

   return;
} // end SetupControls


// ShowDefinitionFileDialogCB callback for the _btnFileDialog to use the
// defintion file name to show a fikle dialog to let the user sselect a *.tm 
// file and then call the TmFactory() to create a _tm object 
void MainWind::ShowDefinitionFileDialogCB(Fl_Widget *widget, void *param) {

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);

   std::string desktop = GetDesktopPath();

   // hide the status box 
   mainwind->SetRunStatusBox(RunState::Off);

   Fl_File_Chooser chooser(desktop.c_str(), "*.tm", 
                           Fl_File_Chooser::SINGLE, "Turing Machine");
   chooser.directory("./tms" /*desktop.c_str()*/);

   chooser.show();

   // block until user selects a button
   while(chooser.shown()) Fl::wait();

   // if user selected a file clear th internal status an setup the tm 
   if(chooser.value() != NULL) {

      mainwind->_validFile = false;

      // clear the input string 
      mainwind->_validInputString = false;
      mainwind->_inString->value("");
      mainwind->_inString->color(FL_BACKGROUND2_COLOR);

      mainwind->SetDefinitionfile(chooser.value(0));
      mainwind->TmFactory();
      mainwind->_grpTuringTape->SetConfiguration(mainwind->_tm->GetConfigurationType());

      mainwind->SetControlEnables();

      // use _partialfname for writing the computation file  
      std::string fname(chooser.value(0));
      GetFilenameFromPath(fname);
      mainwind->_partialfname = fname;

      mainwind->_tm->WriteGraphvizDotFile(fname, mainwind->_gvFullPath);

      // if child is closed, open it
      if(mainwind->_child == nullptr) {
         mainwind->_child = unique_ptr<bp::child>(new bp::child("gviz.exe"));
      }
      else {
         // if child was closed, reopen
         if(mainwind->_child->running() == false){
            mainwind->_child = unique_ptr<bp::child>(new bp::child("gviz.exe"));
         } // end uf 
      } // end if 
      
      // push the new gv file cmd and the full path to the child (ipcq reader)
      int result = mainwind->_ipcqWriter->Push(static_cast<unsigned int>(IpcqCmd::GvFile),mainwind->_gvFullPath);
      assert((result == 0)&&("ShowDefinitionFileDialogCB: ipcq push failed"));

   } // end if

   return;
} // end ShowDefinitionFileDialogCB


// RunButton called from the GroupRunControl, this makes sure everything is 
// setup and then it starts the sequncer timer 
void MainWind::RunButton() {

   // get the run rate from the dropbox 
   _runRate = GetSequencerRunRate();
   
   // set status and start the timer 
   _running = true;
   SetRunStatusBox(RunState::Running);
   _outStatus->value(RunStateText[static_cast<int>(RunState::Running)].c_str());

   // add the definition to the computation
   StringVector definition = _tm->GetDefinition();
   std::for_each(definition.begin(), definition.end(), [&](std::string s) {
      _bwrComputation->add(s.c_str());
   }); // end of lambda 

   Fl::add_timeout(_runRate, RunTimerCB, this);

   SetControlEnables();

   return;
} // end RunButton

void MainWind::PauseButton() {

   _running = false;
   SetRunStatusBox(RunState::Paused);
   _outStatus->value(RunStateText[static_cast<int>(RunState::Paused)].c_str());
   Fl::remove_timeout(RunTimerCB, this);

   return;
} // end PauseButton


void MainWind::StepTm(bool forward){

   TmStatus status = TmStatus::InProgress;

   Transition transition;
   std::string computation;

   // return 0 success
   // return -1 something went wrong
   // any return sets _status to:
   // TmStatus::InProgress, valid next transition
   // TmStatus::CompleteOnEmptyTransition, no final states
   // TmStatus::AcceptedOnFinalState, if there are final states
   // TmStatus::RejectedNotOnFinalState, if there are final states
   // TmStatus::Abnormal_Termination, two - way left on #
   // TmStatus::SomethingWentWrong, should not happen, but ?
   int ret = _tm->TransitionStep();
   status = _tm->GetTmStatus();
   if(0 == ret){

      transition = _tm->GetCurrentTransition();

      switch(status) {

      case TmStatus::InProgress:

         SetTapeSymbolAndMoveTheHead(transition);

         // update the browser list 
         computation = _tm->GetComputation();
         _bwrComputation->add(computation.c_str());

         // restart the timer 
         Fl::repeat_timeout(_runRate, RunTimerCB, this);
      
         break;

      // complete on empty transition but no final states in tm 
      case TmStatus::CompleteOnEmptyTransition:

         // write result to GUI
         _bwrComputation->add(RunStateText[static_cast<std::size_t>(
                              RunState::CompleteOnEmptyTransition)].c_str());
         SetStatus(RunState::CompleteOnEmptyTransition);

         break;

      // only if there are final states 
      case TmStatus::AcceptedOnFinalState:

         // write result to GUI
         _bwrComputation->add(RunStateText[static_cast<std::size_t>(
                              RunState::AcceptedOnFinalState)].c_str());
         SetStatus(RunState::AcceptedOnFinalState);

         break;

      case TmStatus::RejectedNotOnFinalState:

         // write result to GUI
         _bwrComputation->add(RunStateText[static_cast<std::size_t>(
                              RunState::RejectedNotOnFinalState)].c_str());
         SetStatus(RunState::RejectedNotOnFinalState);

         break;

      // only if two_way and left on #
      case TmStatus::InvalidLeftMove:

         // write result to GUI
         _bwrComputation->add(RunStateText[static_cast<std::size_t>(
                              RunState::InvalidLeftMove)].c_str());
         SetStatus(RunState::InvalidLeftMove);

         break;

      // added to be complete but should not happen
      case TmStatus::InvalidRightMove:

         // write result to GUI
         _bwrComputation->add(RunStateText[static_cast<std::size_t>(
                              RunState::InvalidRightMove)].c_str());
         SetStatus(RunState::InvalidRightMove);

         break;

      // should not happen 
      default: 
         break;
      } // end switch 

   }
   else {

      SetStatus(RunState::SomethingWentWrong);

   } // end if 

   return;
} // end StepTm


void MainWind::InitializeTm(){

   // if the TM is not initialized like after a run then initialize
   if(_tm->GetTmStatus() != TmStatus::Initialized) {
      _tm->Initialize();
      _grpTuringTape->SetTapeToB();
      SetupGUITape(_tm->GetConfigurationType());
      _bwrComputation->clear();
   } // end if

   return;
} // end InitializeTm


// ShowSaveComputationFileDialogCB callback for the _btnSaveComputation button,
// this opens a file dialog to let the user select *.txt file to 
// the defintion and computation data into.
// Remark the dialog is preset to save at the user's desktop with 
// a filename that looks like this "computation_<definition_filename>.txt" 
void MainWind::ShowSaveComputationFileDialogCB(Fl_Widget *widget, void *param) {

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);

   // setup the file chooser to save the computation text  
   std::string desktop = GetDesktopPath();

   Fl_File_Chooser chooser(desktop.c_str(), "*.txt", 
                           Fl_File_Chooser::CREATE, "Save Computation");
   chooser.ok_label("Save");
   chooser.directory(desktop.c_str());

   std::string fullPath = desktop + "\\" + COMPUTATION_FILE_NAME;
   fullPath = ReplaceSubstring(fullPath, PLACEHOLDER, mainwind->_partialfname);
   chooser.value(fullPath.c_str());

   chooser.show();

   // block until user selects a button
   while(chooser.shown()) Fl::wait();

   if(chooser.value() != NULL) {
      std::string fname(chooser.value());
      mainwind->WriteComputationToFile(fname);
   } // end if 

   return;
} // end ShowSaveComputationFileDialogCB


// UserInputStringCB, callback for the _btnLoadString button,
// this reads _inString, course test, and loads it to the _tm and 
// then calls SetupGUITape() to set up the GUI
void MainWind::UserInputStringCB(Fl_Widget *widget, void *param) {

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(param);

   if(mainwind->_validFile == true) {
      int ret = mainwind->TestInputString();
      if(ret == 0) {
         mainwind->_validInputString = true;
         mainwind->SetInputStringInTM();

         // initialize _tm with the input string 
         mainwind->_tm->Initialize();
         mainwind->_grpTuringTape->SetTapeToB();
         mainwind->SetupGUITape(mainwind->_tm->GetConfigurationType());
      }
      else {
         mainwind->_validInputString = false;
      } // end if 

      mainwind->SetControlEnables();

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

   // use like a 'this' pointer
   auto mainwind = static_cast<MainWind *>(data);

   TmStatus status = TmStatus::InProgress;

   Transition transition;
   std::string computation;

   // return 0 success
   // return -1 something went wrong
   // any return sets _status to:
   // TmStatus::InProgress, valid next transition
   // TmStatus::CompleteOnEmptyTransition, no final states
   // TmStatus::AcceptedOnFinalState, if there are final states
   // TmStatus::RejectedNotOnFinalState, if there are final states
   // TmStatus::Abnormal_Termination, two - way left on #
   // TmStatus::SomethingWentWrong, should not happen, but ?
   int ret = mainwind->_tm->TransitionStep();
   status = mainwind->_tm->GetTmStatus();
   if(0 == ret){

      transition = mainwind->_tm->GetCurrentTransition();

      switch(status) {

      case TmStatus::InProgress:

         mainwind->SetTapeSymbolAndMoveTheHead(transition);

         // update the browser list 
         computation = mainwind->_tm->GetComputation();
         mainwind->_bwrComputation->add(computation.c_str());

         // restart the timer 
         Fl::repeat_timeout(mainwind->_runRate, RunTimerCB, data);
      
         break;

      // complete on empty transition but no final states in tm 
      case TmStatus::CompleteOnEmptyTransition:

         // mainwind->_btnRun->value(0);
         // RunButtonCB(mainwind->_btnRun, data);

         // write result to GUI
         mainwind->_bwrComputation->add(RunStateText[static_cast<std::size_t>(
                                       RunState::CompleteOnEmptyTransition)].c_str());
         mainwind->SetStatus(RunState::CompleteOnEmptyTransition);

         break;

      // only if there are final states 
      case TmStatus::AcceptedOnFinalState:

         // mainwind->_btnRun->value(0);
         // RunButtonCB(mainwind->_btnRun, data);

         // write result to GUI
         mainwind->_bwrComputation->add(RunStateText[static_cast<std::size_t>(
                                       RunState::AcceptedOnFinalState)].c_str());
         mainwind->SetStatus(RunState::AcceptedOnFinalState);

         break;

      case TmStatus::RejectedNotOnFinalState:

         // mainwind->_btnRun->value(0);
         // RunButtonCB(mainwind->_btnRun, data);

         // write result to GUI
         mainwind->_bwrComputation->add(RunStateText[static_cast<std::size_t>(
                                       RunState::RejectedNotOnFinalState)].c_str());
         mainwind->SetStatus(RunState::RejectedNotOnFinalState);

         break;

      // only if two_way and left on #
      case TmStatus::InvalidLeftMove:

         // mainwind->_btnRun->value(0);
         // RunButtonCB(mainwind->_btnRun, data);

         // write result to GUI
         mainwind->_bwrComputation->add(RunStateText[static_cast<std::size_t>(
                                      RunState::InvalidLeftMove)].c_str());
         mainwind->SetStatus(RunState::InvalidLeftMove);

         break;

      // added to be complete but should not happen
      case TmStatus::InvalidRightMove:

         // mainwind->_btnRun->value(0);
         // RunButtonCB(mainwind->_btnRun, data);

         // write result to GUI
         mainwind->_bwrComputation->add(RunStateText[static_cast<std::size_t>(
                                      RunState::InvalidRightMove)].c_str());
         mainwind->SetStatus(RunState::InvalidRightMove);

         break;

      // should not happen 
      default: 
         break;
      } // end switch 

   }
   else {

      // mainwind->_btnRun->value(0);
      // RunButtonCB(mainwind->_btnRun, data);
      mainwind->SetStatus(RunState::SomethingWentWrong);

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

   _grpTuringTape->SetSymbolAt(transition.GetIndex() - leftIndexValue, 
                               transition.GetSymbol());

   if(transition.GetDirection() == TAPE_RIGHT_SYMBOL)
      ret = _grpTuringTape->MoveHeadRight();
   else
      ret = _grpTuringTape->MoveHeadLeft();

   _grpTuringTape->SetHeadState(transition.GetState());

   // 12-23-2021 send to show_diagram
   _ipcqWriter->Push(static_cast<unsigned int>(IpcqCmd::NewState),transition.GetState());

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

   if(_validFile == true) {
      _inString->activate();
      _btnLoadString->activate();
      _lbExample->activate();
   } // end if 
   
   // if(_validFile == true && _validInputString == true) {
   // } // end if 

   if(_running == true) {
      _btnFileDialog->deactivate();
      _inString->deactivate();
      _btnLoadString->deactivate();
      _lbExample->deactivate();
   }
   else {
      _btnFileDialog->activate();
   } // end if 

   if(_bwrComputation->size() > 0 && _running == false) {
      _btnSaveComputation->activate();
   }
   else {
      _btnSaveComputation->deactivate();
   } // end if 

   _grpRunControls->SetControlEnables(GetControlState(),false);
   
   return;
} // end SetControlEnables

RunControlState MainWind::GetControlState(){
   RunControlState ret = RunControlState::NotReady;

   if(_validFile == true && _validInputString == true){
      if(_running == false){
         if(_complete == false){
            ret = RunControlState::Ready;
         }
         else {
            ret = RunControlState::Complete;
         } // end if 
      }
      else {
         ret = RunControlState::Running;
      } // end if 
   } // end if 

   return ret;
} // end GetControlState



// GetSequencerRunRate, return the seconds float value for the 
// selected _icSpeed dropbox value 
// return seconds sequencer rate
float MainWind::GetSequencerRunRate() {
   float ret = 0.0f;
   std::string rateString = _grpRunControls->GetSpeedString();

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
   case RunState::Paused:
      _lbRunState->color(FL_BACKGROUND_COLOR);
      _lbRunState->label(RunStateText[static_cast<int>(
                         RunState::Paused)].c_str());
      _lbRunState->show();
      break;
   case RunState::InvalidLeftMove:
      _lbRunState->color(FL_RED);
      _lbRunState->label(RunStateText[static_cast<int>(
      RunState::InvalidLeftMove)].c_str());
      _lbRunState->show();
   break;
   case RunState::InvalidRightMove:
      _lbRunState->color(FL_RED);
      _lbRunState->label(RunStateText[static_cast<int>(
      RunState::InvalidRightMove)].c_str());
      _lbRunState->show();
   break;
   case RunState::SomethingWentWrong:
      _lbRunState->color(FL_RED);
      _lbRunState->label(RunStateText[static_cast<int>(
                         RunState::SomethingWentWrong)].c_str());
      _lbRunState->show();
      break;
   case RunState::Off:
      //_lbRunState->hide();
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





