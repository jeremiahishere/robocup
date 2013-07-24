#pragma once
#include "SoccerTeam.h"
#include "SSC2.h"
class Goal;
class PlayerBase;
class FieldPlayer2;
class SoccerPitch;
//class GoalKeeper;
//class SupportSpotCalculator2;

/*
The new soccer team using field players
It extends the old soccer team because it calls methods on the structure in the opponent pointer
One of the next steps would be to completely remove the old team.
Switching to 2 soccerteam2 teams playing against each other just required some pointer changes in the soccer pitch class
*/
class SoccerTeam2 : public SoccerTeam  {

protected:
	/*
	Creates all the players for this team
	Be careful about where you spawn the players and the player indexes for this clalss
	*/
	void CreatePlayers();

	/*
	Pointer to opposing team
	Should change this if you are moving to two of the new teams
	*/
	SoccerTeam* m_pOpponents;
	/*
	Pointers to key players
	The only ones used right now is controlling and closest player because most of the ai has been moved from the team to the player
	*/
	FieldPlayer2*               m_pControllingPlayer;
	FieldPlayer2*               m_pSupportingPlayer;
	FieldPlayer2*               m_pReceivingPlayer;
	FieldPlayer2*               m_pPlayerClosestToBall;

	/*
	the squared distance the closest player is from the ball
	calculated during the update
	*/
	double                     m_dDistSqToBallOfClosestPlayer;
	/*
	A test to see if the support spot calculator could be changed so that it works for the new team
	Got the ssc to work but still need to work on changing over methods in the original soccer team
	*/
	SSC2* m_pSupportSpotCalc;

public:
	/*
	list of players
	*/
	std::vector<FieldPlayer2*>  m_Players;
	/*
	Contstructor
	most of actual initialization done by initialize method
	this just sets the data structres

	parameters:
		goal objects
		parent pitch
		color of team (red or blue)
	*/
	SoccerTeam2(Goal*        home_goal,
             Goal*        opponents_goal,
             SoccerPitch* pitch,
			 team_color   color);
	/*
	destructor
	empty, need to make sure you dont have a leak
	*/
	~SoccerTeam2(void);
	/*
	sets up state machine and creates players
	*/
	void initialize();
	/*
	global update that calls update on all players, updates player/dist closest to ball
	*/
	void Update();
	/*
	draws the field and calls all the player render methods
	*/
	void Render();
	/*
	determines the closest player to ball
	called on every update
	should save information into m_pPlayerClosestToBall
	*/
	void CalculateClosestPlayerToBall();
	/*
	returns the list of players, do not use
	instead use Members()
	*/
	std::vector<FieldPlayer2*> getPlayers()  {return m_Players;}
	/*
	returns the player closest to ball
	duplicaed because it returns a fieldplayer2 rather than a fieldplayer
	*/
	FieldPlayer2* PlayerClosestToBall()const{return m_pPlayerClosestToBall;}
	/*
	determines team
	*/
	int getColor()  {if(Color() == red)  return 0; return 1;}
	//gets opponents, useful for opponents that are extending soccerteam
	SoccerTeam*const getOpponents()const{return m_pOpponents;}
	//see above
	void SetOpponents(SoccerTeam* opps){m_pOpponents = opps;}
	//temporary method to make sure lostControl is being called on SoccerTeam2 rather than the parent method directly
	//void lostControl() {SoccerTeam::LostControl();}
	//overrides that set a FieldPlayer2 rather than a PlayerBase
	void SetPlayerClosestToBall(FieldPlayer2* plyr){m_pPlayerClosestToBall=plyr;}
	void SetSupportingPlayer(FieldPlayer2* plyr){m_pSupportingPlayer = plyr;}
	void SetReceiver(FieldPlayer2* plyr){m_pReceivingPlayer = plyr;}
	void SetControllingPlayer(FieldPlayer2* plyr){m_pControllingPlayer = plyr;}
	FieldPlayer2* ControllingPlayer()  {return m_pControllingPlayer;}
	FieldPlayer2* getControllingPlayer()  {return m_pControllingPlayer;}

	/*
	team gains control or loses control of the ball
	*/
	bool  InControl()const{if(m_pControllingPlayer)return true; else return false;}
	void  LostControl(){m_pControllingPlayer = NULL;}
	/*
	test to try and get the ssc working for the fieldplayer2
	didnt work
	*/
	//Vector2D GetSupportSpot()const{return m_pSupportSpotCalc->GetBestSupportingSpot();}
};
