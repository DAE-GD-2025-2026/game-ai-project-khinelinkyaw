#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Kismet/KismetMathLibrary.h"

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	
	// FVector const FacingVector{ Agent.GetActorRotation().Vector() };
	// FVector const TargetDirection{ UKismetMathLibrary::FindLookAtRotation(Agent.GetActorLocation(), FVector{Target.Position, 0.f}).Vector() };
	// FVector const CrossProduct {FVector::CrossProduct(FacingVector, TargetDirection)};
	// Steering.AngularVelocity = CrossProduct.Z;
	//
	// FRotator NextRotation { Agent.GetActorRotation() };
	// NextRotation.Yaw += Steering.AngularVelocity * Agent.GetMaxAngularSpeed() * DeltaT;
	//
	// Steering.LinearVelocity = static_cast<FVector2D>(NextRotation.Vector()) * Agent.GetMaxLinearSpeed() * DeltaT;
	// double const TargetDistance = (Target.Position - Agent.GetPosition()).Length();
	//
	// if (TargetDistance < Agent.GetMaxLinearSpeed())
	// {
	// 	Steering.LinearVelocity = Steering.LinearVelocity.GetClampedToSize(0, TargetDistance/Agent.GetMaxLinearSpeed());
	// }
	
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	//Steering.LinearVelocity = Seek::CalculateLinearVelocity(DeltaT, Agent, Target.Position);
	
	return Steering;
}

FVector2D Seek::CalculateLinearVelocity(float DeltaT, ASteeringAgent const& Agent, FVector2D const& TargetPosition, float StopMargin)
{
	FVector2D TargetVelocity = TargetPosition - Agent.GetPosition();
	
	if (TargetVelocity.Length() > Agent.GetMaxLinearSpeed())
	{
		TargetVelocity.Normalize();
		TargetVelocity *= Agent.GetMaxLinearSpeed();
		return Agent.GetLinearVelocity() + TargetVelocity;
	}
	else if (TargetVelocity.Length() < StopMargin)
	{
		return FVector2D{};
	}
	else
	{
		return TargetVelocity;
	}
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	
	Steering.LinearVelocity = -Seek::CalculateLinearVelocity(DeltaT, Agent, Target.Position);

	return Steering;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	auto TargetVector = Target.Position - Agent.GetPosition();
	Steering.LinearVelocity = Seek::CalculateLinearVelocity(DeltaT, Agent, Target.Position);

	if (auto const TargetDistance = TargetVector.Length(); TargetDistance < SlowRadius)
	{
		auto const SpeedSlowRate = (TargetDistance - TargetRadius)/(SlowRadius - TargetRadius);
		Steering.LinearVelocity *= SpeedSlowRate * DeltaT;
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
	
	return CrossProduct.Z;
}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	FVector2D const PredictedPosition { Target.Position + (Target.LinearVelocity) };
	
	Steering.LinearVelocity = Seek::CalculateLinearVelocity(DeltaT, Agent, PredictedPosition);
	
	return Steering;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	FVector2D const PredictedPosition { Target.Position + (Target.LinearVelocity) };
	
	auto PredictedDistance {(PredictedPosition - Agent.GetPosition()).Length()};
	if (PredictedDistance > EvadeRadius)
	{
		Steering.IsValid = false;
		return Steering;
	}
	
	FVector ActorLocationOnPlane { Agent.GetActorLocation() };
	ActorLocationOnPlane.Z = 0.f;
	
	if (Agent.GetDebugRenderingEnabled())
	{
		DrawDebugLine(
			Agent.GetWorld(),
			ActorLocationOnPlane,
			ActorLocationOnPlane + FVector(PredictedPosition, 0.f),
			FColor::Cyan,
			false,
			0.0f,
			0,
			3.0f
		);
	}
	
	Steering.LinearVelocity = - Seek::CalculateLinearVelocity(DeltaT, Agent, PredictedPosition);
	
	return Steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	
	FVector2D const CircleCenter { Agent.GetPosition() + Agent.GetForwardVector() * CircleOffset };
	
	if (Agent.GetDebugRenderingEnabled())
	{
		// Destination Vector
		DrawDebugCircle(
			Agent.GetWorld(),
			FVector{CircleCenter, 0.f},
			CircleRadius,
			12,
			FColor::Orange,
			false,
			0.f,
			0,
			3.f,
			FVector::YAxisVector,
			FVector::XAxisVector,
			false
		);
	}
	
	auto const AngleChange = FMath::RandRange(WanderRange.Key,WanderRange.Value)/ 10.f;
	CurrentAngle += AngleChange;
	
	FVector2D WanderPosition{};
	
	WanderPosition.X = (FMath::Cos(CurrentAngle) * CircleRadius) + CircleCenter.X;
	WanderPosition.Y = (FMath::Sin(CurrentAngle) * CircleRadius) + CircleCenter.Y;
	
	if (Agent.GetDebugRenderingEnabled())
	{
		DrawDebugCircle(
			Agent.GetWorld(),
			FVector{WanderPosition, 0.f},
			5.f,
			12,
			FColor::Green,
			false,
			0.f,
			0,
			3.f,
			FVector::YAxisVector,
			FVector::XAxisVector,
			false
		);
	}
	
	Steering.LinearVelocity = Seek::CalculateLinearVelocity(DeltaT, Agent, WanderPosition);
	
	return Steering;
}
