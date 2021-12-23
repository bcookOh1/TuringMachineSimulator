// file: GroupTuringTape.cpp
// Class: CS5800
// Project: Turing Machine Simulator 
// Author: Bennett Cook
// Date: 4/08/2018
// Description: implementation the FLTK group control for the 
// Turing Machine visual simulator 
//
#include "GroupTuringTape.h"
#include <iostream>
#include <iomanip>
#include <boost\lexical_cast.hpp>

using namespace boost;


// ctor, note that the _width and  _height are used in the parnet constructor 
// these 2 vars must be static const types. 
GroupTuringTape::GroupTuringTape(int x, int y) : 
   Fl_Scroll(x, y, _width, _height) {

   Fl_Scroll::type(Fl_Scroll::HORIZONTAL_ALWAYS);

   _initialXcoord = x;
   _initialYcoord = y,

   box(FL_THIN_UP_BOX);
   color(FL_WHITE);

   _tape = new Fl_Box(x + 4, y + 15, TAPE_SIZE, 48, TAPE_BOX_LABEL.c_str()); 
   _tape->box(FL_BORDER_BOX);
   _tape->color(FL_DARK2);
   _tape->align(Fl_Align(FL_ALIGN_TOP_LEFT));

   // the left index value allows 
   // for standard and two-way tape configurations
   _leftIndexValue = 0;

   int xOffset = 8;
   int yOffset = 22;
   for(std::size_t i = 0; i < NUMBER_TAPE_POSITIONS; i++) {
      TapePosition *tp = new TapePosition(_initialXcoord + xOffset,
                                          _initialYcoord + yOffset, 
                                          35, 35);
      tp->SetValue('B');
      tp->SetIndex(i);
      xOffset += HEAD_OFFSET;
      _positions.push_back(tp);

   } // end for

   _initialHeadxPosition = x + 8;
   _initialHeadyPosition = y + 68;

   // all tapeheads under the tape positions, this was not the original 
   // concept, which was make one and move it, but that didn't work with 
   // the fl_scroll, the x value changes with the scroll position  
   _currentHeadPosition = 0;

   // try setting in all positions 
   for(std::size_t i = 0; i < NUMBER_TAPE_POSITIONS; i++) {
      int hp = _initialHeadxPosition + (HEAD_OFFSET * (i));
      TapeHead *h = new TapeHead(hp, _initialHeadyPosition);
      
      // save the initial position  for scroll positioning later 
      h->SetxPosition(hp);

      // show the first hide the rest 
      if(i == _currentHeadPosition) {
         h->Show();
      }
      else {
         h->Hide();
      } // end if 

      _headPositions.push_back(h);

   } // end for 

   end(); // needed by fltk to end() addition of widgets to internal tree

} // end ctor


GroupTuringTape::~GroupTuringTape() {
   // fltk deletes the widgets 
} // end dtor


// SetConfiguration: setup the tape GUI elements
// pre: call this after configuration type is set following 
// successful tm file readin     
void GroupTuringTape::SetConfiguration(TmConfiguration configuration) {

   // set the configurationType
   if(TmConfiguration::Standard == configuration) {
      _leftIndexValue = 0;
   } 
   else {
      _leftIndexValue = TWO_WAY_LEFT_INDEX;
   } // end if

   _currentHeadPosition = 0; // always zero at the start 

   // setup the tape with  blanks and indexes for standard or two_way   
   for(std::size_t i = _currentHeadPosition; i < NUMBER_TAPE_POSITIONS; i++) {
      _positions[i]->SetIndex(static_cast<int>(_leftIndexValue + i));
      _positions[i]->SetValue(TAPE_BLANK_SYMBOL[0]);
   } // end for 

   // set the initial head position
   ResetHeadPosition();

   return;
} // end SetConfiguration


// use this to initialize the entire tape 
// after a run that may have written outside 
// of the input string space
void GroupTuringTape::SetTapeToB() {

   std::for_each(_positions.begin(), _positions.end(), 
   [&](TapePosition *tp){tp->SetValue(TAPE_BLANK_SYMBOL[0]);}); 

   return;
} // end SetTapeToB


// return 0 success
// return -1 index out of range 
int GroupTuringTape::SetSymbolAt(int idx, std::string symbol) {
   int ret = -1;

   if(static_cast<std::size_t>(idx) < _positions.size()) {
      _positions[idx]->SetValue(symbol[0]);
      ret = 0;
   } // end if 

   return ret;
} // end SetSymbolAt


// return 0 success
// return -1 index out of range 
int GroupTuringTape::SetIndexAt(int index, int value) {
   int ret = -1;

   if(static_cast<std::size_t>(index) < _positions.size()) {
      _positions[index]->SetIndex(value);
      ret = 0;
   } // end if 

   return ret;
} // end SetIndexAt


// set the state string at current head position 
// return 0 success
int GroupTuringTape::SetHeadState(std::string state) {
   int ret = 0;
   _headPositions[_currentHeadPosition]->SetState(state);
   return ret;
} // end SetHeadState


// return 0 success
int GroupTuringTape::SetInitialHeadPosition() {
   int ret = 0;
   ResetHeadPosition();
   return ret;
} // end SetInitialHeadPosition 


// move the tape head so its positioned 
// under the zero index tape cell
void GroupTuringTape::ResetHeadPosition() {

   // ensure both head and tape vectors are the same size 
   if(_headPositions.size() != _positions.size()){
      assert(false);
   } // end if 
   
   _currentHeadPosition = GetZeroIndex();

   for(std::size_t i = 0; i < _headPositions.size(); ++i){
      if(i == _currentHeadPosition) {
         _headPositions[i]->Show();
      }
      else {
         _headPositions[i]->Hide();
      } // end if 

   } // end for 

   KeepHeadInView();

   return;
} // end ResetHeadPosition


// return the zero index value on the tape
// this is the index value not at _positions[0].
// needed to find zero cell in the two_way config 
std::size_t GroupTuringTape::GetZeroIndex(){
   std::size_t ret = 0;

   for(std::size_t i = 0; i < _positions.size(); ++i) {
      if(_positions[i]->GetIndex() == 0) {
         ret = i;
         break;
      } // end if 
   } // end for 

   return ret;
} // end for 


// move the tape head one cell to the right and 
// keep the head in the scroll view. 
// this will "show" the head at _currentHeadPosition (index)
// and hide the rest.
// return 0 on success 
// return -1 on _currentHeadPosition >= NUMBER_TAPE_POSITIONS
int GroupTuringTape::MoveHeadRight() {
   int ret = -1;

   if(_currentHeadPosition < NUMBER_TAPE_POSITIONS){

      _currentHeadPosition++;
      for(std::size_t i = 0; i < _headPositions.size(); ++i) {
         if(i == _currentHeadPosition) {
            _headPositions[i]->Show();
         } 
         else {
            _headPositions[i]->Hide();
         } // end if 

      } // end for 

      ret = 0;
   } // end if 

   KeepHeadInView();

   return ret;
} // end MoveHeadRight


// move the tape head one cell to the left and 
// keep the head in the scroll view. 
// this will "show" the head at _currentHeadPosition (index)
// and hide the rest.
// return 0 on success 
// return -1 on _currentHeadPosition <= 0 
int GroupTuringTape::MoveHeadLeft() {
   int ret = -1;

   if(_currentHeadPosition > 0) {

      _currentHeadPosition--;
      for(std::size_t i = 0; i < _headPositions.size(); ++i) {
         if(i == _currentHeadPosition) {
            _headPositions[i]->Show();
         } 
         else {
            _headPositions[i]->Hide();
         } // end if 

      } // end for 

      ret = 0;
   } // end if 

   KeepHeadInView();

   return ret;
} // end MoveHeadLeft


// use the current head position to keep 
// the head in the scroll view, use the actual head x position
// stored in the TapeHead class which allows the user to 
// move the scroll bar and then on the next move KeepHeadInView() 
// will put the head in the center (about) of the window
void GroupTuringTape::KeepHeadInView() {

   int xPixelViewPosition = xposition();
   int initialX = _headPositions[_currentHeadPosition]->GetxPosition();

// used to debug moving the scroll position
#ifdef _DEBUG
   std::cout << "xv: " << xPixelViewPosition << ", initialX: " 
             << initialX << std::endl;
#endif
   
   // this will move the scroll position to center the head 
   // in the scroll window. it will only do this if necessary
   // which shows about +/- 5 head positions until it moves again   
   if(initialX  > xPixelViewPosition + _width) {  // move right
      int right = 0;
      
      // set the right but limit to tape size right edge 
      if(initialX + (_width/2) < TAPE_SIZE)
         right = initialX - (_width / 2);
      else
         right = TAPE_SIZE - _width;

      scroll_to(right, Y_COORD_VIEW_POSITION);
   } 
   else if(initialX < xPixelViewPosition) {  // move left 
      int left = 0;
      
      // set the left but limit to tape size left edge
      if(initialX - (_width / 2) > HEAD_OFFSET)
         left = initialX - (_width / 2);
      else 
         left = 0;

      scroll_to(left, Y_COORD_VIEW_POSITION);
   }  // end if 

   return;
} // end KeepHeadInView


