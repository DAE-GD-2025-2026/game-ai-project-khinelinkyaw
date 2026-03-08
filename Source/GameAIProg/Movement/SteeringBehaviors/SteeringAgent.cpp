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
    
    if (bIsDebugRenderingEnabled)
    {
        FVector ActorLocationOnPlane { GetActorLocation() };
        ActorLocationOnPlane.Z = 0.f;
    
        // Facing Direction
        DrawDebugLine(
            GetWorld(),
            ActorLocationOnPlane,
            ActorLocationOnPlane + GetActorRotation().Vector() * 150.f,
            FColor::Red,
            false,
            0.0f,
            0,
            3.0f
        );
    }
    
    if (SteeringBehavior)
    {
        if (SteeringOutput const Output = SteeringBehavior->CalculateSteering(DeltaTime, *this); Output.IsValid) 
        {
            AddMovementInput(FVector{Output.LinearVelocity, 0.f});
            
            if (AAIController* AaiController { Cast<AAIController>(GetController())})
            {
                FRotator NewRotation { GetActorRotation() };
                NewRotation.Yaw += Output.AngularVelocity * GetMaxAngularSpeed() * DeltaTime;
                
                AaiController->SetControlRotation(NewRotation);
                FaceRotation(NewRotation);
                
                if (bIsDebugRenderingEnabled)
                {
                    FVector ActorLocationOnPlane { GetActorLocation() };
                    ActorLocationOnPlane.Z = 0.f;
                    
                    // Destination Vector
                    DrawDebugCircle(
                        GetWorld(),
                        ActorLocationOnPlane + FVector{Output.LinearVelocity, 0.f},
                        10.f,
                        12,
                        FColor::Yellow,
                        false,
                        0.f,
                        0,
                        3.f,
                        FVector::YAxisVector,
                        FVector::XAxisVector,
                        false
                    );
                
                    // Footsteps
                    DrawDebugCircle(
                        GetWorld(),
                        ActorLocationOnPlane,
                        5.f,
                        6,
                        FColor::Red,
                        false,
                        4.0f,
                        0,
                        3.f,
                        FVector::YAxisVector,
                        FVector::XAxisVector,
                        false
                    );
                }

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
