#pragma once

//this class is prety much copied and pasted from State.h
class FieldPlayer2;
#include <string>
/*
states for fieldplayer2
based on state.h except without the messaging
on every update, execute should be called
on a state change exit on the current state is called and enter on the newe state
*/
class FieldPlayer2BaseState  {
public:
  virtual ~FieldPlayer2BaseState(){}

  //this will execute when the state is entered
  virtual void enter(FieldPlayer2*)=0;

  //this is the states normal update function
  virtual void execute(FieldPlayer2*)=0;

  //this will execute when the state is exited. 
  virtual void exit(FieldPlayer2*)=0;

  //name of the state
  virtual std::string getName()=0;
};
