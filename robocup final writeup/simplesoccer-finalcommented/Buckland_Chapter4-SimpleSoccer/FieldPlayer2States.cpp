#include "FieldPlayer2States.h"
#include "FieldPlayer2.h"
#include "SoccerTeam2.h"
#include "SoccerTeam.h" //the opponents
#include "SoccerBall.h"
#include "Goal.h"
#include <math.h>
#include <time.h>

//class DefendMiddle : public FieldPlayer2BaseState
DefendMiddle* DefendMiddle::instance()  {
	static DefendMiddle instance;
	return &instance;
}

std::string DefendMiddle::getName()  {
	std::string s = "Defend Middle";
	return s;
}

void DefendMiddle::enter(FieldPlayer2* fp)  {
	//anything special go here?
}

void DefendMiddle::execute(FieldPlayer2* fp)  {
	//set orientation
	fp->setTargetOrientation(fp->getBall()->Pos());
	//set target loc, center post
	Vector2D target_pos = Vector2D((fp->getTeam()->HomeGoal()->Center().x + fp->getBall()->Pos().x)/2, 
			(fp->getTeam()->HomeGoal()->Center().y + fp->getBall()->Pos().y)/2);
	fp->setTargetPosition(target_pos);

	//if at the ball, get ready to kick
	if(fp->atTarget(fp->getBall()->Pos()))  {
		fp->setCurrentState(HasBall::instance());
		return;
	}
	//if closest to ball, stop defening and actively move towards ball
	else if(fp->getTeam()->PlayerClosestToBall() == fp)  {
		fp->setCurrentState(ChaseBall2::instance());
		return;
	}	
}

void DefendMiddle::exit(FieldPlayer2* fp)  {
	//anything special here?
}
//class DefendRight : public FieldPlayer2BaseState
DefendRight* DefendRight::instance()  {
	static DefendRight instance;
	return &instance;
}

std::string DefendRight::getName()  {
	std::string s = "Defend Right";
	return s;
}

void DefendRight::enter(FieldPlayer2* fp)  {
	//anything special go here?
}

void DefendRight::execute(FieldPlayer2* fp)  {
	//set orientation
	fp->setTargetOrientation(fp->getBall()->Pos());
	//set target loc, right post
	Vector2D target_pos = Vector2D((fp->getTeam()->HomeGoal()->RightPost().x + fp->getBall()->Pos().x)/2, 
			(fp->getTeam()->HomeGoal()->RightPost().y + fp->getBall()->Pos().y)/2);
	fp->setTargetPosition(target_pos);	

	//if at the ball, get ready to kick
	if(fp->atTarget(fp->getBall()->Pos()))  {
		fp->setCurrentState(HasBall::instance());
		return;
	}
	//if closest to ball, stop defening and actively move towards ball
	else if(fp->getTeam()->PlayerClosestToBall() == fp)  {
		fp->setCurrentState(ChaseBall2::instance());
		return;
	}
}

void DefendRight::exit(FieldPlayer2* fp)  {
	//anything special here?
}
//class DefendRight : public FieldPlayer2BaseState
DefendLeft* DefendLeft::instance()  {
	static DefendLeft instance;
	return &instance;
}

std::string DefendLeft::getName()  {
	std::string s = "Defend Left";
	return s;
}

void DefendLeft::enter(FieldPlayer2* fp)  {
	//anything special go here?
}

void DefendLeft::execute(FieldPlayer2* fp)  {
	//set orientation
	fp->setTargetOrientation(fp->getBall()->Pos());
	//set target loc, left post
	Vector2D target_pos = Vector2D((fp->getTeam()->HomeGoal()->LeftPost().x + fp->getBall()->Pos().x)/2, 
			(fp->getTeam()->HomeGoal()->LeftPost().y + fp->getBall()->Pos().y)/2);
	fp->setTargetPosition(target_pos);	
	//if at the ball, get ready to kick
	if(fp->atTarget(fp->getBall()->Pos()))  {
		fp->setCurrentState(HasBall::instance());
		return;
	}
	//if closest to ball, stop defening and actively move towards ball
	else if(fp->getTeam()->PlayerClosestToBall() == fp)  {
		fp->setCurrentState(ChaseBall2::instance());
		return;
	}	
}

void DefendLeft::exit(FieldPlayer2* fp)  {
	//anything special here?
}

ChaseBall2* ChaseBall2::instance()  {
	static ChaseBall2 instance;
	return &instance;
}

std::string ChaseBall2::getName()  {
	std::string s = "Chase Ball (2)";
	return s;
}

void ChaseBall2::enter(FieldPlayer2 *fp)  {
	
}

void ChaseBall2::execute(FieldPlayer2 *fp)  {
	//look at current ball position
	fp->setTargetOrientation(fp->getBall()->Pos());
	//move to current ball pos, old method
	fp->setTargetPosition(fp->getBall()->Pos());
	//move to future ball pos based on time to cover dist to ball
	//fp->setTargetPosition(fp->getBall()->FuturePosition(fp->getTimeToBall()));

	//if at ball and a teammate does not have it
	if(fp->atTarget(fp->getBall()->Pos()) && fp->getTeam()->PlayerClosestToBall() != NULL)  {
		fp->setCurrentState(HasBall::instance());
	}
	//a teammate has the ball, setup a pass
	/*else if(fp->getTeam()->InControl())  {
		//char buf[2048];
		//sprintf(buf,"ChaseBall2.execute: controlling player %g, %g \n", controller->Pos().x, controller->Pos().y);
		//OutputDebugString(buf);
		fp->setCurrentState(SetupPass::instance());
	}*/
	//if not closest player to ball, go back to base state
	else if(fp->getTeam()->PlayerClosestToBall() != fp && fp->getStartState() != ChaseBall2::instance())  {
		fp->setCurrentState(fp->getStartState());
	}
}

void ChaseBall2::exit(FieldPlayer2 *fp)  {

}

HasBall* HasBall::instance()  {
	static HasBall instance;
	return &instance;
}


std::string HasBall::getName()  {
	std::string s = "Has Ball";
	return s;
}

void HasBall::enter(FieldPlayer2* fp)  {
	//this is wrong, should be a call to set the controlling player from null to a player on the team
	fp->getTeam()->SetControllingPlayer(fp);
	fp->getTeam()->getOpponents()->LostControl();

	int kick_force = 0;  //small kick to control the ball
	fp->kickBall(kick_force);
}

void HasBall::execute(FieldPlayer2* fp)  {
	Vector2D target_loc = fp->getBall()->Pos();
	Vector2D target_angle;
	//find a good shot and take it
	target_angle = fp->getTeam()->OpponentsGoal()->Center();
	fp->setTargetOrientation(target_angle);
	fp->setTargetPosition(target_loc);

	//srand( time(NULL));
	if(fp->getTeam()->ControllingPlayer() != fp)  {
		fp->setCurrentState(fp->getStartState());
	}
	else if(/*rand() % 3 == 0 && */fp->atTarget(target_loc) && fp->atTargetOrientation(target_angle))  {
		fp->kickBall(7);
		fp->setCurrentState(fp->getStartState());
		//enter/exit?
	}
	//dribble ball until next generation
	else if(fp->atTarget(target_loc)) { 
		//fp->getBall()->Trap();
		fp->kickBall(1);
	}
	//lost the ball
	else if(fp->getTeam()->ControllingPlayer() != fp || !fp->atTarget(target_loc))  {
		fp->setCurrentState(fp->getStartState());
	}
	//find a good pass and switch to pass?	
}

void HasBall::exit(FieldPlayer2* fp)  {
	fp->getTeam()->LostControl();
}


SetupPass* SetupPass::instance()  {
	static SetupPass instance;
	return &instance;
}


std::string SetupPass::getName()  {
	std::string s = "Setup Pass";
	return s;
}

void SetupPass::enter(FieldPlayer2* fp)  {
	OutputDebugString("Entering setuppass state\n");
}

void SetupPass::execute(FieldPlayer2* fp)  {
	//if team has lost the ball
	if(!fp->getTeam()->InControl())  {
		fp->setCurrentState(fp->getStartState());
		return;
	}
	OutputDebugString("SEtupPass.execute: controlling player shouldnts be null at this point\n");
	//go to prime passing location, looking at player with ball
	fp->setTargetPosition(fp->getTeam()->GetSupportSpot());
	fp->setTargetOrientation(fp->getTeam()->ControllingPlayer()->Pos());
	//switch to chase ball if the closest player  (intercepts the pass)
	if(fp->getTeam()->PlayerClosestToBall() == fp)  {
		fp->setCurrentState(ChaseBall2::instance());
	}

}

void SetupPass::exit(FieldPlayer2* fp)  {
	
}

HoldPosition* HoldPosition::instance()  {
	static HoldPosition instance;
	return &instance;
}


std::string HoldPosition::getName()  {
	std::string s = "Hold Position";
	return s;
}

void HoldPosition::enter(FieldPlayer2* fp)  {
	OutputDebugString("Entering HoldPosition state\n");
}
//this state just keeps the player in its current location
void HoldPosition::execute(FieldPlayer2* fp)  {
	fp->setTargetPosition(fp->Pos());
	fp->setTargetOrientation(fp->getBall()->Pos());
}

void HoldPosition::exit(FieldPlayer2* fp)  {
	
}

