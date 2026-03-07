#include "SteeringBehaviors.h"

#include "AITestsCommon.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Kismet/KismetMathLibrary.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)
SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	
	Steering.AngularVelocity = Face::CalculateAngularVelocity(DeltaT, Agent, Target);

	FVector2D TargetVelocity = Target.Position - Agent.GetPosition();
	
	if (TargetVelocity.Length() > Agent.GetMaxLinearSpeed())
	{
		TargetVelocity.Normalize();
		TargetVelocity *= Agent.GetMaxLinearSpeed();
		Steering.LinearVelocity = Agent.GetLinearVelocity() + TargetVelocity;
	}
	else
	{
		Steering.LinearVelocity = TargetVelocity;
	}
	
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
	
	Steering.AngularVelocity = CalculateAngularVelocity(DeltaT, Agent, Target);
	
	return Steering;
}

float Face::CalculateAngularVelocity(float DeltaT, ASteeringAgent const& Agent, FTargetData const& Target)
{
	FVector const FacingVector{ Agent.GetActorRotation().Vector() };
	FVector const TargetDirection{ UKismetMathLibrary::FindLookAtRotation(Agent.GetActorLocation(), FVector{Target.Position, 0.f}).Vector() };
	
	FVector CrossProduct {FVector::CrossProduct(FacingVector, TargetDirection)};
	CrossProduct.Normalize();
	
	return CrossProduct.Z;
}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	FVector2D const PredictedPosition { Target.Position + (Target.LinearVelocity) };
	
	Steering.LinearVelocity = PredictedPosition - Agent.GetPosition();
	
	return Steering;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	FVector2D const PredictedPosition { Target.Position + (Target.LinearVelocity) };
	
	Steering.LinearVelocity = PredictedPosition - Agent.GetPosition();
	
	return Steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	return Steering;
}
