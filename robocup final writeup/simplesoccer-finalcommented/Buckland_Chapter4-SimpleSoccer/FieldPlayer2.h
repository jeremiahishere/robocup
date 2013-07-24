#pragma once

#include "2D/Vector2D.h"
#include "Game/BaseGameEntity.h"
#include "time/Regulator.h"
#include "FieldPlayer2BaseState.h"
class SoccerTeam2;
class SoccerPitch;
class SoccerBall;

/*
replacement for the field player
extends basegameentity for position vector and to remain compatiable with the rest of the objects in the program
*/
class FieldPlayer2 : public BaseGameEntity {
private:
	//state of the player
	FieldPlayer2BaseState* current_state;
	//state to revert to when going to default movement
	FieldPlayer2BaseState* start_state;
	//player index
	int index;
	//players team
	SoccerTeam2* team;
	//location of location player is trying to get o
	Vector2D target_position;
	//orientation player is trying to rotate to
	Vector2D target_orientation;
	//size of player
	double radius;
	//velocity and direction of player movement
	Vector2D velocity;
	//normalized vector of where the player is pointing
	Vector2D orientation;
	//maximum velocity of the robot
	double max_speed;
	//maximum angular velocity in radians
	double max_turn_rate;
	//returns the pitch from the team
	SoccerPitch* getPitch();
	//distance squared to ball
	double m_dDistSqToBall;
	//the vertex buffer, used to store the coordinates of the player
	std::vector<Vector2D>   m_vecPlayerVB;
	//the buffer for the transformed vertices, just used for rendering
	std::vector<Vector2D>   m_vecPlayerVBTrans;
	//determines player actions with the state system
	//StateMachine<FieldPlayer2> state_machine;
	/*
	rotates the player to face target (ball, goal, teammate, etc)
	*/
	void rotateToTarget(Vector2D target);
	//moves the player towards the target by setting the velocity
	void moveToTarget(Vector2D target);
	/*
	returns the angle the player should move based on a single axis based on the position
	of an attractor or repulsor on the field
	paramters:
		p_x position of the player on 1 axis (x or y)
		x position of other game entity on 1 axis (x or y)
		alpha: maximum influence of other game entity on the player
		gamma: variance of distribution of influence from other game entity
		dist_sq: distance squared between the two objects, note that this is total distance not component distance
	*/
	double getPathGradient(double p_x, double x, double alpha, double gamma, double dist_sq);

	//limits the number of kicks a player may take per second
	Regulator* m_pKickLimiter;
	bool isReadyForNextKick()const{return m_pKickLimiter->isReady();}

	//list of previous positions for the player
	std::vector<Vector2D> prev_pos;

public:
	/*
	be careful with the index, it determines the initial player states
	a better solution would be to reimplement the initial state parameter
	*/
	FieldPlayer2::FieldPlayer2(SoccerTeam2* in_team, 
						   int in_index,						   
						   //State<FieldPlayer2>* start_state,
						   Vector2D initial_position,
						   Vector2D in_heading,
						   Vector2D in_velocity,
						   double in_radius, 
						   double in_max_speed, 
						   double in_max_turn_rate);
	~FieldPlayer2(void);

	//updates the player position, state, etc
	//determines player noise as well
	void Update();
	//draws the player
	//also draws the player trails
	void Render();
	//distance from player to ball squared (to avoid suqare roots)
	void SetDistSqToBall(double dist);
	//detemrines whether the player is at it's target location
	bool atTarget(Vector2D in_target);
	//determines whether the player is at its target angle
	bool atTargetOrientation(Vector2D in_target);
	//controls state information
	FieldPlayer2BaseState* getStartState()  {return start_state;}
	FieldPlayer2BaseState* getCurrentState()  {return current_state;}
	void setCurrentState(FieldPlayer2BaseState* s);
	//returns the soccer ball from the team
	SoccerBall* getBall();
	//returns the team
	SoccerTeam2* getTeam() {return team;}
	//set targets to determine movement
	void setTargetPosition(Vector2D v)  {target_position = v;}
	void setTargetOrientation(Vector2D v)  {target_orientation = v;}
	//kick in the direction the player is facing
	void kickBall(double strength);
	//returns the orientation of the player
	Vector2D getOrientation()  {return orientation;}
	//amount of time steps it will take the player to reach teh ball, running at max speed
	double getTimeToBall();
};
