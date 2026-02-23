// Fill out your copyright notice in the Description page of Project Settings.

#include "SteeringAgent.h"

#include "AIController.h"


// Sets default values
ASteeringAgent::ASteeringAgent()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASteeringAgent::BeginPlay()
{
    Super::BeginPlay();
}

void ASteeringAgent::BeginDestroy()
{
    Super::BeginDestroy();
}

// Called every frame
void ASteeringAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (SteeringBehavior)
    {
        if (SteeringOutput const Output = SteeringBehavior->CalculateSteering(DeltaTime, *this); Output.IsValid) 
        {
            if (AAIController* AaiController { Cast<AAIController>(GetController())})
            {
                AddMovementInput(FVector{Output.LinearVelocity, 0.f});

                FRotator NewRotation { GetActorRotation() };
                NewRotation.Yaw += Output.AngularVelocity * GetMaxAngularSpeed() * DeltaTime;

                AaiController->SetControlRotation(NewRotation);
                FaceRotation(NewRotation);
            }
        }
    }
}

// Called to bind functionality to input
void ASteeringAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASteeringAgent::SetSteeringBehavior(ISteeringBehavior* NewSteeringBehavior)
{
    SteeringBehavior = NewSteeringBehavior;
}
