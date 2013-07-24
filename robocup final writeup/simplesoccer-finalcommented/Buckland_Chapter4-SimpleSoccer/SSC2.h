#pragma once
#include "supportspotcalculator.h"

class SoccerTeam2;
class FieldPlayer2;

class SSC2 :
	public SupportSpotCalculator
{
private:
	SoccerTeam2*               m_pTeam;
public:
	SSC2(int numX, int numY, SoccerTeam2* team);

	  //draws the spots to the screen as a hollow circles. The higher the 
  //score, the bigger the circle. The best supporting spot is drawn in
  //bright green.
  void       Render()const;

  //this method iterates through each possible spot and calculates its
  //score.
  Vector2D  DetermineBestSupportingPosition();

  //returns the best supporting spot if there is one. If one hasn't been
  //calculated yet, this method calls DetermineBestSupportingPosition and
  //returns the result.
  Vector2D  GetBestSupportingSpot();
};
