#pragma once

#include <Movement/SteeringBehaviors/SteeringHelpers.h>

class ASteeringAgent;

// SteeringBehavior base, all steering behaviors should derive from this.
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	// Override to implement your own behavior
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) = 0;

	void SetTarget(const FTargetData& NewTarget) { Target = NewTarget; }
	
	template<class T, std::enable_if_t<std::is_base_of_v<ISteeringBehavior, T>>* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	FTargetData Target;
};

// Your own SteeringBehaviors should follow here...
class Seek : public ISteeringBehavior
{
private:
	float NearRadius{50.f};
public:
	Seek() = default;
	virtual ~Seek() override = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
	
	static FVector2D CalculateLinearVelocity(float DeltaT, ASteeringAgent const& Agent, FVector2D const& TargetPosition, float StopMargin = 5.0f);
};

class Flee : public ISteeringBehavior
{
public:
	Flee() = default;
	virtual ~Flee() override = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Arrive : public ISteeringBehavior
{
private:
	float TargetRadius{200.f};
	float SlowRadius{600.f};
	float OriginalSpeed{};
	bool IsSlowing{false};
	
public:
	Arrive() = default;
	virtual ~Arrive() override = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
	void SetTargetRadius(float NewTargetRadius) { TargetRadius = NewTargetRadius; }
};

class Face : public ISteeringBehavior
{
	public:
	Face() = default;
	virtual ~Face() override = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
	
	static float CalculateAngularVelocity(float DeltaT, ASteeringAgent const& Agent, FTargetData const& Target);
};

class Pursuit final : public ISteeringBehavior
{
	public:
	Pursuit() = default;
	virtual ~Pursuit() override = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Evade final : public ISteeringBehavior
{
private:
	float EvadeRadius{200.f};
	
public:
	Evade() = default;
	virtual ~Evade() override = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Wander final : public ISteeringBehavior
{
private:
	float CurrentAngle{};
	float CircleRadius{75.f};
	float CircleOffset{ 250.f };
	TPair<int32,int32> WanderRange{-50,50};

public:
	Wander() = default;
	virtual ~Wander() override = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};