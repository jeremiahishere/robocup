#ifndef FIELDPLAYERSTATES_H
#define FIELDPLAYERSTATES_H

//the structure of this file is based on FieldPlayerSates.h
#include "FieldPlayer2BaseState.h"
class FieldPlayer2;

/*
moves the player to a spot halfway between the ball and the center of the goal
changes to chaseball2 when it is closest to the ball
*/
class DefendMiddle : public FieldPlayer2BaseState  {
private:
	DefendMiddle(){}
public:
	static DefendMiddle* instance();
	void enter(FieldPlayer2* fp);
	void execute(FieldPlayer2* fp);
	void exit(FieldPlayer2* fp);
	std::string getName();
};

/*
moves the player to a spot halwayf between the ball and the left side  of the goal
changes to chaseball2 when it is closest to the ball
*/
class DefendLeft : public FieldPlayer2BaseState  {
private:
	DefendLeft(){}
public:
	static DefendLeft* instance();
	void enter(FieldPlayer2* fp);
	void execute(FieldPlayer2* fp);
	void exit(FieldPlayer2* fp);
	std::string getName();
};

/*
moves the player to a spot halfway between the ball and the right side of the goal
changes to chaseball2 when it is closest to the ball
*/
class DefendRight : public FieldPlayer2BaseState  {
private:
	DefendRight(){}
public:
	static DefendRight* instance();
	void enter(FieldPlayer2* fp);
	void execute(FieldPlayer2* fp);
	void exit(FieldPlayer2* fp);
	std::string getName();
};

/*
moves the player to the current position of the ball
changes state when it gets to the ball or when it is no longer the closest player to the ball

renamed because there was a naming conflict with ChaseBall for the fieldplayer
*/
class ChaseBall2 : public FieldPlayer2BaseState  {
private:
	ChaseBall2(){}
public:
	static ChaseBall2* instance();
	void enter(FieldPlayer2* fp);
	void execute(FieldPlayer2* fp);
	void exit(FieldPlayer2* fp);
	std::string getName();
};

/*
moves the player to point to the goal and kick the ball
changes states when the player loses control of the ball or the ball gets kicked
*/
class HasBall : public FieldPlayer2BaseState  {
private:
	HasBall(){}
public:
	static HasBall* instance();
	void enter(FieldPlayer2* fp);
	void execute(FieldPlayer2* fp);
	void exit(FieldPlayer2* fp);
	std::string getName();
};

/*
moves the player based on the supportspot calculator to a spot that opens it up for a pass
currently not working
this class is switched into when another  player gets control of the ball and the player has the state Chaseball2
this class is left when the team loses control of the ball or it becomes the closest player to the ball
*/
class SetupPass : public FieldPlayer2BaseState  {
private:
	SetupPass(){}
public:
	static SetupPass* instance();
	void enter(FieldPlayer2* fp);
	void execute(FieldPlayer2* fp);
	void exit(FieldPlayer2* fp);
	std::string getName();
};

/*
keeps the player still, used for demo videos
*/
class HoldPosition : public FieldPlayer2BaseState  {
private:
	HoldPosition()  {}
public:
	static HoldPosition* instance();
	void enter(FieldPlayer2* fp);
	void execute(FieldPlayer2* fp);
	void exit(FieldPlayer2* fp);
	std::string getName();
};
#endif