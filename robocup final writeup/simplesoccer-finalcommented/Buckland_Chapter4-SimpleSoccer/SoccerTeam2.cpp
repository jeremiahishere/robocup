#include "SoccerTeam2.h"
#include "SoccerPitch.h"
#include "Goal.h"
#include "PlayerBase.h"
#include "GoalKeeper.h"
#include "FieldPlayer2.h"
#include "misc/utils.h"
#include "SteeringBehaviors.h"
#include "GoalKeeperStates.h"
#include "ParamLoader.h"
#include "2D/geometry.h"
#include "Game/EntityManager.h"
#include "Messaging/MessageDispatcher.h"
#include "SoccerMessages.h"
#include "TeamStates.h"
#include "FieldPlayerStates.h"
#include "Debug/DebugConsole.h"
#include <windows.h>
using std::vector;

SoccerTeam2::SoccerTeam2(Goal*        home_goal,
             Goal*        opponents_goal,
             SoccerPitch* pitch,
			 team_color   color): SoccerTeam(home_goal, opponents_goal, pitch, color)  {
	OutputDebugString("SoccerTeam2.Constructor\n");
}

void SoccerTeam2::initialize()  {
	OutputDebugString("SoccerTeam2.initialize\n");
  //setup the state machine
  m_pStateMachine = new StateMachine<SoccerTeam>(this);

  m_pStateMachine->SetCurrentState(Defending::Instance());
  m_pStateMachine->SetPreviousState(Defending::Instance());
  m_pStateMachine->SetGlobalState(NULL);

  //create the players and goalkeeper
  CreatePlayers();
  
  //set default steering behaviors
  /*
  std::vector<FieldPlayer2*>::iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    (*it)->Steering()->SeparationOn();   
  }
  */
  //create the sweet spot calculator
  m_pSupportSpotCalc = new SSC2(Prm.NumSupportSpotsX,
                                                 Prm.NumSupportSpotsY,
                                                 this);
  m_pControllingPlayer = NULL;
}
SoccerTeam2::~SoccerTeam2(void)
{
}

/*
//minor hack to get around hard coded team sizes
void SoccerTeam::SetPlayerHomeRegion(int plyr, int region)
{
	//change the assert to an if
	//assert ( (plyr>=0) && (plyr<(int)m_Players.size()) );
	if( (plyr>=0) && (plyr<(int)m_Players.size()) )  {
		m_Players[plyr]->SetHomeRegion(region);
	}
}*/

//------------------------- CreatePlayers --------------------------------
//
//  creates the players
//------------------------------------------------------------------------
void SoccerTeam2::CreatePlayers()  {
	OutputDebugString("SoccerTeam2.CreatePlayers\n");
  if (Color() == blue)
  {
	  /*m_Players.push_back(new FieldPlayer2(this,
								0,
								//Wait::Instance,
								Vector2D(100, 100),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
								//PlayerBase::attacker));*/
	m_Players.push_back(new FieldPlayer2(this,
								0,
								//Wait::Instance,
								Vector2D(500, 100),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
								//PlayerBase::attacker));
	m_Players.push_back(new FieldPlayer2(this,
								1,
								//Wait::Instance,
								Vector2D(500, 140),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
								//PlayerBase::attacker));
		m_Players.push_back(new FieldPlayer2(this,
								2,
								//Wait::Instance,
								Vector2D(500, 170),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
		m_Players.push_back(new FieldPlayer2(this,
								3,
								//Wait::Instance,
								Vector2D(500, 200),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
								//PlayerBase::attacker));
  }

  else
  {

	  /*m_Players.push_back(new FieldPlayer2(this,
								4,
								//Wait::Instance,
								Vector2D(250, 250),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));*/
								//PlayerBase::attacker));
	m_Players.push_back(new FieldPlayer2(this,
								0,
								//Wait::Instance,
								Vector2D(300, 300),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
								//PlayerBase::attacker));
	m_Players.push_back(new FieldPlayer2(this,
								1,
								//Wait::Instance,
								Vector2D(100, 140),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
								//PlayerBase::attacker));
		m_Players.push_back(new FieldPlayer2(this,
								2,
								//Wait::Instance,
								Vector2D(100, 170),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
		m_Players.push_back(new FieldPlayer2(this,
								3,
								//Wait::Instance,
								Vector2D(100, 200),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
								//PlayerBase::attacker));
		/*m_Players.push_back(new FieldPlayer2(this,
								4,
								//Wait::Instance,
								Vector2D(300, 175),
								Vector2D(1, 1),
								Vector2D(1,1),
								1,
								Prm.PlayerMaxSpeedWithoutBall,
								Prm.PlayerMaxTurnRate));
								//PlayerBase::attacker));*/
                      
  }
  //m_pControllingPlayer = m_Players[0];
	/*
  //register the players with the entity manager
  std::vector<FieldPlayer2*>::iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    EntityMgr->RegisterEntity(*it);
  }*/
}


//------------------------ CalculateClosestPlayerToBall ------------------
//
//  sets m_iClosestPlayerToBall to the player closest to the ball
//------------------------------------------------------------------------
void SoccerTeam2::CalculateClosestPlayerToBall()
{
	//OutputDebugString("SoccerTeam2::CalculateClosestPlayerToBall\n");
  double ClosestSoFar = MaxFloat;

  std::vector<FieldPlayer2*>::iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    //calculate the dist. Use the squared value to avoid sqrt
    double dist = Vec2DDistanceSq((*it)->Pos(), Pitch()->Ball()->Pos());

    //keep a record of this value for each player
    (*it)->SetDistSqToBall(dist);
    
    if (dist < ClosestSoFar)
    {
      ClosestSoFar = dist;

      m_pPlayerClosestToBall = *it;
    }
  }

  m_dDistSqToBallOfClosestPlayer = ClosestSoFar;
}

//-------------------------- update --------------------------------------
//
//  iterates through each player's update function and calculates 
//  frequently accessed info
//------------------------------------------------------------------------
void SoccerTeam2::Update()
{
	//5OutputDebugString("SoccerTeam2.Update\n");
  //this information is used frequently so it's more efficient to 
  //calculate it just once each frame
  CalculateClosestPlayerToBall();

  //the team state machine switches between attack/defense behavior. It
  //also handles the 'kick off' state where a team must return to their
  //kick off positions before the whistle is blown
  m_pStateMachine->Update();
  
  //now update each player
  std::vector<FieldPlayer2*>::iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    (*it)->Update();
  }

}


//--------------------------- Render -------------------------------------
//
//  renders the players and any team related info
//------------------------------------------------------------------------
void SoccerTeam2::Render()  {
	//OutputDebugString("SoccerTeam2::Render\n");
  std::vector<FieldPlayer2*>::const_iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    (*it)->Render();
  }

  //show the controlling team and player at the top of the display
  if (Prm.bShowControllingTeam)
  {
    gdi->TextColor(Cgdi::white);
    
    if ( (Color() == blue) && InControl())
    {
      gdi->TextAtPos(20,3,"Blue in Control");
    }
    else if ( (Color() == red) && InControl())
    {
      gdi->TextAtPos(20,3,"Red in Control");
    }
    if (m_pControllingPlayer != NULL)
    {
		//OutputDebugString("SoccerTeam2.render: attempting to show controlling player\n");
		//gdi->TextAtPos(Pitch()->cxClient()-150, 3, "Controlling Player: " + ttos(m_pControllingPlayer->ID()));
    }
  }
//OutputDebugString("SoccerTeam2.render: continuing to render\n");
  //render the sweet spots
  if (Prm.bSupportSpots && InControl())
  {
    m_pSupportSpotCalc->Render();
  }

//#define SHOW_TEAM_STATE
#ifdef SHOW_TEAM_STATE
  if (Color() == red)
  {
    gdi->TextColor(Cgdi::white);

    if (CurrentState() == Attacking::Instance())
    {
      gdi->TextAtPos(160, 20, "Attacking");
    }
    if (CurrentState() == Defending::Instance())
    {
      gdi->TextAtPos(160, 20, "Defending");
    }
    if (CurrentState() == PrepareForKickOff::Instance())
    {
      gdi->TextAtPos(160, 20, "Kickoff");
    }
  }
  else
  {
    if (CurrentState() == Attacking::Instance())
    {
      gdi->TextAtPos(160, Pitch()->cyClient()-40, "Attacking");
    }
    if (CurrentState() == Defending::Instance())
    {
      gdi->TextAtPos(160, Pitch()->cyClient()-40, "Defending");
    }
    if (CurrentState() == PrepareForKickOff::Instance())
    {
      gdi->TextAtPos(160, Pitch()->cyClient()-40, "Kickoff");
    }
  }
#endif

//#define SHOW_SUPPORTING_PLAYERS_TARGET
#ifdef SHOW_SUPPORTING_PLAYERS_TARGET
  if (m_pSupportingPlayer)
  {
    gdi->BlueBrush();
    gdi->RedPen();
    gdi->Circle(m_pSupportingPlayer->Steering()->Target(), 4);

  }
#endif

}