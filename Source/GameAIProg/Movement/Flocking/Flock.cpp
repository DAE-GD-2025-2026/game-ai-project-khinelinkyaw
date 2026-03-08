#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"


Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pWorld}
	, FlockSize{ FlockSize }
	, pAgentToEvade{pAgentToEvade}
{
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pSeparationBehavior = std::make_unique<Separation>(this);
	pVelMatchBehavior = std::make_unique<VelMatch>(this);
	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();
	pEvadeBehavior = std::make_unique<Evade>();
	
	pBlendedSteering = std::make_unique<BlendedSteering>();
	pBlendedSteering->AddBehaviour(pCohesionBehavior.get(), 0.0f);
	pBlendedSteering->AddBehaviour(pSeparationBehavior.get(), 0.0f);
	pBlendedSteering->AddBehaviour(pVelMatchBehavior.get(), 0.0f);
	//pBlendedSteering->AddBehaviour(pSeekBehavior.get(), 1.f);
	pBlendedSteering->AddBehaviour(pWanderBehavior.get(), 0.0f);
	//pBlendedSteering->AddBehaviour(pEvadeBehavior.get(), 1.f);
	
	Agents.SetNum(FlockSize);
	
	for (int i = 0; i < FlockSize; ++i)
	{
		ASteeringAgent* NewAgent {};
		
		while (NewAgent == nullptr)
		{
			auto const XPos = static_cast<double>(FMath::RandRange(-1000, 1000));
			auto const YPos = static_cast<double>(FMath::RandRange(-1000, 1000));
			NewAgent = pWorld->SpawnActor<ASteeringAgent>(AgentClass, FVector{XPos,YPos,90}, FRotator::ZeroRotator);
		
			if (NewAgent)
			{
				Agents[i] = std::move(NewAgent);
				Agents[i]->SetSteeringBehavior(pBlendedSteering.get());
			}
		}
	}

 // TODO: initialize the flock and the memory pool
	Neighbors.SetNum(MaxNeighbors);
}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
}

void Flock::BeginPlay()
{
}

void Flock::Tick(float DeltaTime)
{
 // TODO: update the flock
 // TODO: for every agent:
  // TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
  // TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
  // TODO: trim the agent to the world
	
	for (ASteeringAgent* Agent : Agents)
	{
		RegisterNeighbors(Agent);
		Agent->Tick(DeltaTime);
	}
	
	
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

  // TODO: implement ImGUI checkboxes for debug rendering here

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Cohesion",
			pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
			[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Separation",
		pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
		[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Velo Match",
		pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight, 0.f, 1.f,
		[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
		pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight, 0.f, 1.f,
		[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight = InVal; }, "%.2f");
		
		ImGui::Spacing();
		
  // TODO: implement ImGUI sliders for steering behavior weights here
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
	NrOfNeighbors = 0;
	
	for (ASteeringAgent* const Agent : Agents)
	{
		if (pAgent == Agent)
		{
			continue;
		}
		
		auto const NeighborDistance { (Agent->GetPosition() - pAgent->GetPosition()).Length() };
		
		if (NeighborDistance < NeighborhoodRadius)
		{
			Neighbors[NrOfNeighbors] = Agent;
			++NrOfNeighbors;
			
			if (NrOfNeighbors >= MaxNeighbors)
			{
				return;
			}
		}
	}
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = FVector2D::ZeroVector;
	
	if (NrOfNeighbors == 0)
	{
		return avgPosition;
	}
	
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		avgPosition += Neighbors[i]->GetPosition();
	}
	
	avgPosition /= NrOfNeighbors;
	
	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;
	
	if (NrOfNeighbors == 0)
	{
		return avgVelocity;
	}
	
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		avgVelocity += Neighbors[i]->GetLinearVelocity();
	}

	avgVelocity /= NrOfNeighbors;
	
	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
	pSeekBehavior->SetTarget(Target);
}

