#include "FieldPlayer2.h"
#include "SoccerTeam2.h"
#include "SoccerPitch.h"
#include "SoccerBall.h"
#include "ParamLoader.h"
#include "Goal.h"
#include "PlayerBase.h"
#include "FieldPlayer2States.h"
#include <cmath>
#include <time.h>


FieldPlayer2::FieldPlayer2(SoccerTeam2* in_team, 
						   int in_index,						   
						   //State<FieldPlayer2>* start_state,
						   Vector2D		initial_position,
						   Vector2D in_heading,
						   Vector2D in_velocity,
						   double in_radius, 
						   double in_max_speed, 
						   double in_max_turn_rate):
	index(in_index),	
	team(in_team),
	//state(start_state),
	orientation(in_heading),
	velocity(in_velocity),
	radius(in_radius),
	max_speed(in_max_speed),
	max_turn_rate(in_max_turn_rate),
	BaseGameEntity(in_index)  {

	//initialize random number generator
	srand( time(NULL));
	SetPos(initial_position);
	//initialize the state machine, and set the inital state
	if(index == 0)  {
		start_state = ChaseBall2::instance();
	}
	else if(index == 1)  {
		start_state = DefendMiddle::instance();
	}
	else if(index == 2)  {
		start_state = DefendRight::instance();
	}
	else if(index == 3)  {
		start_state = DefendLeft::instance();
	}
	/*else if(index == 4)  {
		start_state = SetupPass::instance();
	}*/
	//temporary method to make opponents stand still
	else  {
		start_state = HoldPosition::instance();
		}
	current_state = start_state;
	//cant uset setCurrentState because current_state is null;
	current_state->enter(this);
		

	//this code was copied from the constructor in PlayerBase.cpp
	//setup the vertex buffers and calculate the bounding radius
	const int NumPlayerVerts = 4;
	const Vector2D player[NumPlayerVerts] = {Vector2D(-3, 8),
											Vector2D(3,10),
											Vector2D(3,-10),
											Vector2D(-3,-8)};

	for (int vtx=0; vtx<NumPlayerVerts; ++vtx)
	{
		m_vecPlayerVB.push_back(player[vtx]);

		//set the bounding radius to the length of the 
		//greatest extent
		if (abs(player[vtx].x) > m_dBoundingRadius)
		{
			m_dBoundingRadius = abs(player[vtx].x);
		}

		if (abs(player[vtx].y) > m_dBoundingRadius)
		{
			m_dBoundingRadius = abs(player[vtx].y);
		}
	}

	//set up the kick regulator
	//default value is Prm.PlayerKickFrequency = 8
	int kicks_per_second = 2;
	m_pKickLimiter = new Regulator(kicks_per_second);


}

FieldPlayer2::~FieldPlayer2(void)
{
}

void FieldPlayer2::rotateToTarget(Vector2D target)  {
	//this is a bad hack to get around the issue of the orientation being set to nan/inf
	//I believe it happens when the acos value returns something undefined
	//but it should not happen because we are normalizing before calling it
	//as far as I can tell, the input includes normal doubles
	//it is possible that the problem has to do with the ball going slightly out of bounds before being reflected
	//this line means std::isnan(orientation.x) || std::isnan(orientation.y)
	if(orientation.x != orientation.x || orientation.y != orientation.y)  {
		OutputDebugString("FP2.rotateToTarget: fp2 orientation is nan\n");
		orientation = Vector2D(1, 0);
	}
	Vector2D normalized_target = Vec2DNormalize(target - Pos());
	Vector2D normalized_orientation = Vec2DNormalize(orientation);
	double dot_product = normalized_orientation.Dot(normalized_target);
	//needed to keep acos <= |1|, currently normalizing instead
	//Clamp(dot_product, -1, 1);
	//first determine the angle between the heading vector and the target
	double angle = acos(dot_product);
	//can the angle be overrun negative?
	if(angle > max_turn_rate)  {
		angle = max_turn_rate;
	}

	//The next few lines use a rotation matrix to rotate the player's heading
	//vector accordingly
	C2DMatrix RotationMatrix;
	//notice how the direction of rotation has to be determined when creating
	//the rotation matrix
	RotationMatrix.Rotate(angle * normalized_orientation.Sign(normalized_target));
	RotationMatrix.TransformVector2Ds(orientation);
}

//moves the player towards the target by setting the velocity
void FieldPlayer2::moveToTarget(Vector2D target)  {
	double dx = 0;
	double dy = 0;
	
	//search own team and avoid
	std::vector<FieldPlayer2*>::const_iterator it = team->m_Players.begin();
	for (it; it != team->m_Players.end(); ++it)  {
		double alpha = 1;
		double gamma = 0.0002;
		//ignore self
		if(Pos().x != (*it)->Pos().x && Pos().y != (*it)->Pos().y)  {
			dx += getPathGradient(Pos().x, (*it)->Pos().x, alpha, gamma, Pos().DistanceSq((*it)->Pos()));
			dy += getPathGradient(Pos().y, (*it)->Pos().y, alpha, gamma, Pos().DistanceSq((*it)->Pos()));
		}
	}
	
	//avoid opponents team
	std::vector<PlayerBase*>::const_iterator it2 = team->getOpponents()->m_Players.begin();
	for (it2; it2 != team->getOpponents()->m_Players.end(); ++it2)  {
		double alpha = 1;
		double gamma = 0.0002;
		dx += getPathGradient(Pos().x, (*it2)->Pos().x, alpha, gamma, Pos().DistanceSq((*it2)->Pos()));
		dy += getPathGradient(Pos().y, (*it2)->Pos().y, alpha, gamma, Pos().DistanceSq((*it2)->Pos()));
	}

	//go to target
	double alpha = -1;
	double gamma = 0.000001;
	dx += getPathGradient(Pos().x, target.x, alpha, gamma, Pos().DistanceSq(target));
	dy += getPathGradient(Pos().y, target.y, alpha, gamma, Pos().DistanceSq(target));

	Vector2D new_dir = Vector2D(dx, dy);
	new_dir.Normalize();
	
	double speed = min(max_speed, Pos().Distance(target));
	velocity = new_dir * speed;
}

double FieldPlayer2::getPathGradient(double p_x, double x, double alpha, double gamma, double dist_sq)  {
	//exponential potential field
	double output = alpha * gamma * (p_x - x) * exp(gamma * x * -0.5 * dist_sq);
	//formula for dE/dx, replace a.x with a.y for formula for dE/dy
	return output;
}

bool FieldPlayer2::atTarget(Vector2D in_target)  {
	double dx = Pos().x - in_target.x;
	double dy = Pos().y - in_target.y;
	//see params.ini, default = 6
	if(abs(dx) < Prm.PlayerKickingDistance && abs(dy) < Prm.PlayerKickingDistance)  {
		return true;
	}
	return false;
}
//determines if the player is looking at the correct target
bool FieldPlayer2::atTargetOrientation(Vector2D in_target)  {
	Vector2D normalized_target = Vec2DNormalize(in_target - Pos());
	Vector2D normalized_orientation = Vec2DNormalize(orientation);
	double dot_product = normalized_orientation.Dot(normalized_target);
	double angle = acos(dot_product);
	//current error is fairly high pi/8 = 22.5 degrees
	//should probably be reduced
	if(angle < 3.14 / 8)  {
		return true;
	}
	return false;
}
//kicks the ball at the specified strength in the direction the robot is facing
void FieldPlayer2::kickBall(double strength)  {
	  getBall()->Kick(orientation, strength);
}
//sets the state and makes sure state changes are done correctly
void FieldPlayer2::setCurrentState(FieldPlayer2BaseState *s)  {
	current_state->exit(this);
	current_state = s;
	current_state->enter(this);
	//dont execute here because when in state HasBall and a kick is executed, this is called leading to an infinite loop
	//current_state->execute(this);
}

//updates the player position, state, etc
void FieldPlayer2::Update()  {
	//execute state and update position/orientation
	current_state->execute(this);
	moveToTarget(target_position);
	rotateToTarget(target_orientation);
	//update tracking position
	if(current_state->getName().compare("Chase Ball (2)") == 0)  {
		if(prev_pos.size() > 200)  {
			prev_pos.clear();
		}
		prev_pos.push_back(Pos());
	}
	//move the player
	SetPos(Pos() + velocity);
	
	//add noise
	/*
	double x = rand() % 100 - 50;
	double y = rand() % 100 - 50;
	//1/(sqrt(2) * pi) * exp(-1/2 * x^2)
	m_vPosition.x += 50 * (1/4.44288294) * exp(-(x*x)/2);
	m_vPosition.y += 50 * (1/4.44288294) * exp(-(y*y)/2);	
	*/
}
//draws the player
//this code was copied and pasted from FieldPlayer.cpp
void FieldPlayer2::Render()  {
	//OutputDebugString("FieldPlayer2::render\n");
	gdi->TransparentText();
	gdi->TextColor(Cgdi::grey);

	//set appropriate team color
	if (team->Color() == SoccerTeam::blue){gdi->BluePen();}
	else{gdi->RedPen();}



	//render the player's body
	m_vecPlayerVBTrans = WorldTransform(m_vecPlayerVB,
										 Pos(),
										 orientation,
										 orientation.Perp(),
										 Scale());
	gdi->ClosedShape(m_vecPlayerVBTrans);  

	//and 'is 'ead
	gdi->BrownBrush();
	/*
	if (Prm.bHighlightIfThreatened && (team->ControllingPlayer() == this) && isThreatened()) 
		gdi->YellowBrush();
	*/
	gdi->Circle(Pos(), 6);

	//draw player path
	if(current_state->getName().compare("Chase Ball (2)") == 0 && index == 0)  {
		std::vector<Vector2D>::const_iterator it = prev_pos.begin();
		for (it; it != prev_pos.end(); ++it)  {
			gdi->Circle(*it, 2);
		}
	}

	//render the state
	
	if (Prm.bStates)  {  
		gdi->TextColor(0, 170, 0);
		gdi->TextAtPos(m_vPosition.x, m_vPosition.y -20, current_state->getName());
	}
	
	//show IDs
	
	if (Prm.bIDs)  {
		gdi->TextColor(0, 170, 0);
		gdi->TextAtPos(Pos().x-20, Pos().y-20, ttos(ID()));
	}

	
	if (Prm.bViewTargets)  {
		gdi->RedBrush();
		gdi->Circle(target_position, 3);
		gdi->TextAtPos(target_position, ttos(ID()));
	}
	
	
}


void FieldPlayer2::SetDistSqToBall(double dist)  {
	m_dDistSqToBall = dist;
}
//returns the soccer ball from the team
SoccerBall* FieldPlayer2::getBall()  {
  return team->Pitch()->Ball();
}

//returns the pitch from the team
SoccerPitch* FieldPlayer2::getPitch()  {
  return team->Pitch();
}

double FieldPlayer2::getTimeToBall()  {
	double dist = Pos().Distance(team->Pitch()->Ball()->Pos());
	dist = dist/max_speed;
	return dist;
}