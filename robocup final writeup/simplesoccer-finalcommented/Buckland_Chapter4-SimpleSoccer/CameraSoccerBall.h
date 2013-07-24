#pragma once
#include "soccerball.h"

class CameraSoccerBall : public SoccerBall
{
public:

	CameraSoccerBall(Vector2D           pos,            
             double               BallSize,
             double               mass,
             std::vector<Wall2D>& PitchBoundary):
		SoccerBall(pos, BallSize, mass, PitchBoundary) {}
	~CameraSoccerBall(void);

	//overriden update that includes a call to the camera
	//Update();
private:
	//gets updates position from camera and changes the ball location
	//getUpdateFromCamera();

	//keep a previous location to possibly guess velocity
	//also keep a guessed location?
	//if not keeping a duplicate guessed location, can store in m_vOldPos

	
};
