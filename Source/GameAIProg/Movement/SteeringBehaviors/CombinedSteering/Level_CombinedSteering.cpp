#include "Level_CombinedSteering.h"

#include "imgui.h"


// Sets default values
ALevel_CombinedSteering::ALevel_CombinedSteering()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_CombinedSteering::BeginPlay()
{
	std::vector WeightedBehaviors {
		BlendedSteering::WeightedBehavior(new Seek(), 0.5f),
		BlendedSteering::WeightedBehavior(new Wander(), 0.5f),
	};
	
	PBlendedSteering = new BlendedSteering(WeightedBehaviors);
	
	Super::BeginPlay();

}

void ALevel_CombinedSteering::BeginDestroy()
{
	Super::BeginDestroy();

}

// Called every frame
void ALevel_CombinedSteering::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#pragma region UI
	//UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
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
		ImGui::Spacing();
	
		ImGui::Text("Flocking");
		ImGui::Spacing();
		ImGui::Spacing();
	
		if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
		{
   // TODO: Handle the debug rendering of your agents here :)
		}
		ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
		if (TrimWorld->bShouldTrimWorld)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
				TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
				[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
		}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("Behavior Weights");
		ImGui::Spacing();
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
			PBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
			[this](float InVal) { PBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
		PBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
		[this](float InVal) { PBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
	
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		
		ImGui::Text("Agents");
		ImGui::Spacing();
		
		if (ImGui::Button("Add Drunk Agent"))
		{
			AddAgent(EAgentType::DrunkAgent);
		}
		
		if (ImGui::Button("Add Evading Agent"))
		{
			AddAgent(EAgentType::EvadingAgent);
		}
		
		//End
		ImGui::End();
	}
#pragma endregion
	// Combined Steering Update
	UpdateDrunkAgents();
 // TODO: implement handling mouse click input for seek
 // TODO: implement Make sure to also evade the wanderer
}

bool ALevel_CombinedSteering::AddAgent(EAgentType AgentType)
{
	ImGui_Agent ImGuiAgent = {};
	ImGuiAgent.Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{0,0,90}, FRotator::ZeroRotator);
	if (IsValid(ImGuiAgent.Agent))
	{
		ImGuiAgent.AgentType = AgentType;
		
		switch (AgentType)
		{
		case EAgentType::DrunkAgent:
			ImGuiAgent.Agent->SetSteeringBehavior(PBlendedSteering);
			break;
		case EAgentType::EvadingAgent:
			ImGuiAgent.Agent->SetSteeringBehavior(PPrioritySteering);
			break;
		}
		
		SteeringAgents.push_back(std::move(ImGuiAgent));
		UpdateAgentVectors();
		return true;
	}

	return false;
}

void ALevel_CombinedSteering::UpdateAgentVectors()
{
	DrunkSteeringAgents = {};
	EvadingSteeringAgents = {};
	
	for (auto& Agent : SteeringAgents)
	{
		switch (Agent.AgentType)
		{
		case EAgentType::DrunkAgent:
			DrunkSteeringAgents.push_back(&Agent);
			break;
		case EAgentType::EvadingAgent:
			EvadingSteeringAgents.push_back(&Agent);
			break;
		}
	}
}

void ALevel_CombinedSteering::UpdateDrunkAgents()
{
	PBlendedSteering->GetWeightedBehaviorsRef()[0].pBehavior->SetTarget(MouseTarget);
}
