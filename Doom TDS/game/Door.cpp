#include "Door.h"


void CDoor::SetPositions(CVector open, CVector close)
{
	openPosition=open;
	closePosition=close;
	
}

void CDoor::Open()
{
   // use automove function to set the door into motion for the open position
   MoveTo(openPosition.x,openPosition.z, 300);
   // MoveTo also rotates door, need to correct this
   if (GetRotation() != 180) { Rotate(90); }
   // use status 1 to indicate that door is open(ing)
   SetStatus(1);
}

void CDoor::Close()
{
  // use automove function to set the door into motion for the close position
  MoveTo(closePosition.x,closePosition.z, 100);
  // MoveTo also rotates door, need to correct this
  Rotate(90);
  // use status 0 to indicate that door is close(ing)
  SetStatus(0);
}

   
