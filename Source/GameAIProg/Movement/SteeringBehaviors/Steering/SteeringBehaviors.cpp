#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Kismet/KismetMathLibrary.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)
SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	
	// Normalization not needed
	//Steering.LinearVelocity.Normalize();

	// TODO: Show a cool thing

	// TODO: Add debug rendering for grades :)

	return Steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = - (Target.Position - Agent.GetPosition());
	// Normalization not needed
	//Steering.LinearVelocity.Normalize();

	// TODO: Show a cool thing

	// TODO: Add debug rendering for grades :)

	return Steering;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();

	if (auto const TargetDistance = Steering.LinearVelocity.Length(); TargetDistance < SlowRadius)
	{
		if (IsSlowing == false)
		{
			OriginalSpeed = Agent.GetMaxLinearSpeed();
			IsSlowing = true;
		}
		
		auto const SpeedSlowRate = (TargetDistance - TargetRadius)/(SlowRadius - TargetRadius);
		Agent.SetMaxLinearSpeed(OriginalSpeed * SpeedSlowRate);
	}
	else if (IsSlowing == true)
	{
		Agent.SetMaxLinearSpeed(OriginalSpeed);
		IsSlowing = false;
	}
	return Steering;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	Steering.LinearVelocity = FVector2D::Zero();
	
	FVector const FacingVector{ Agent.GetActorRotation().Vector()};
	FVector const TargetDirection{ UKismetMathLibrary::FindLookAtRotation(Agent.GetActorLocation(), FVector{Target.Position, 0.f}).Vector()};
	
	FVector CrossProduct {FVector::CrossProduct(FacingVector, TargetDirection)};
	CrossProduct.Normalize();
	
	Steering.AngularVelocity = CrossProduct.Z;
	
	return Steering;
}
