#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringBehaviors/SteeringAgent.h"
#include "../SteeringBehaviors/SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	Target.Clear();
	Target.Position = pFlock->GetAverageNeighborPos();
	
	return Seek::CalculateSteering(deltaT, pAgent);
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};
	Steering.LinearVelocity = FVector2D::ZeroVector;
	
	for (int i = 0; i < pFlock->GetNrOfNeighbors(); ++i)
	{	
		FVector2D NeighborRelativePos { pFlock->GetNeighbors()[i]->GetPosition() - pAgent.GetPosition() };
		Steering.LinearVelocity += -NeighborRelativePos/NeighborRelativePos.Length();
	}
	
	return Steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelMatch::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};
	
	Steering.LinearVelocity = pFlock->GetAverageNeighborVelocity() * deltaT;
	
	return Steering;
}
