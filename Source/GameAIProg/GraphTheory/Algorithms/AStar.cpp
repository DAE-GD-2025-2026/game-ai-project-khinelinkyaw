#include "AStar.h"

#include "GeometryCollection/GeometryCollectionConvexUtility.h"

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: pGraph(pGraph)
	, HeuristicFunction(hFunction)
{
}

std::vector<Node*> AStar::BacktrackFullPath(std::vector<NodeRecord> const& ClosedList, NodeRecord const& StartingNodeRecord)
{
	std::vector<Node*> Path{};
	NodeRecord CurrentNodeRecord{ StartingNodeRecord };
	
	while (CurrentNodeRecord.pConnection != nullptr)
	{
		Path.push_back(CurrentNodeRecord.pNode);
		
		auto NextNodeIter = std::ranges::find_if(ClosedList, [&CurrentNodeRecord](NodeRecord const& Record)
		{
			return CurrentNodeRecord.pConnection->GetFromId() == Record.pNode->GetId();
		});
		
		CurrentNodeRecord = *NextNodeIter;
	}
	
	Path.push_back(CurrentNodeRecord.pNode);
	std::ranges::reverse(Path);
	return Path;
}

std::vector<Node*>AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
	std::vector<NodeRecord> OpenList{};
	std::vector<NodeRecord> ClosedList{};
	
	NodeRecord CurrentNodeRecord{};
	CurrentNodeRecord.pNode = pStartNode;
	CurrentNodeRecord.pConnection = nullptr;
	CurrentNodeRecord.costSoFar = 0.0f;
	CurrentNodeRecord.distanceToDest = GetHeuristicCost(pStartNode, pGoalNode);
	CurrentNodeRecord.CalculateEstimatedTotalCost();
	OpenList.push_back(CurrentNodeRecord);
	
	NodeRecord NeighborNodeRecord{};
	auto FindNeighborNodeId { [&NeighborNodeRecord](NodeRecord const& InnerRecord)
	{
		return	InnerRecord.pNode->GetId() == NeighborNodeRecord.pNode->GetId();
	} };
	
	while (!OpenList.empty())
	{
		auto NodeRecordIter = std::min_element(OpenList.begin(), OpenList.end());
		CurrentNodeRecord = *NodeRecordIter;
		auto CurrentNodeId { CurrentNodeRecord.pNode->GetId() };
		
		if (CurrentNodeRecord.pNode == pGoalNode)
		{
			ClosedList.push_back(CurrentNodeRecord);
			OpenList.clear();
			break;
		}
		
		auto Connections = pGraph->FindConnectionsFrom(CurrentNodeId);
		
		for (auto Conn : Connections)
		{
			NeighborNodeRecord.pNode = pGraph->GetNode(Conn->GetToId()).get();
			NeighborNodeRecord.pConnection = Conn;
			NeighborNodeRecord.costSoFar = CurrentNodeRecord.costSoFar + Conn->GetWeight();
			NeighborNodeRecord.distanceToDest = GetHeuristicCost(NeighborNodeRecord.pNode, pGoalNode);
			NeighborNodeRecord.CalculateEstimatedTotalCost();
			
			auto OpenListIter = std::ranges::find_if(OpenList, FindNeighborNodeId);
			auto ClosedListIter = std::ranges::find_if(ClosedList, FindNeighborNodeId);
			
			if (OpenListIter != OpenList.end() and NeighborNodeRecord.costSoFar < OpenListIter->costSoFar)
			{
				OpenList.erase(OpenListIter);
			}
			else if (ClosedListIter != ClosedList.end() and NeighborNodeRecord.costSoFar < ClosedListIter->costSoFar)
			{
				ClosedList.erase(ClosedListIter);
			}
			else if (OpenListIter != OpenList.end() or ClosedListIter != ClosedList.end())
			{
				continue;
			}
			
			OpenList.push_back(NeighborNodeRecord);
		}
		
		ClosedList.push_back(CurrentNodeRecord);
		std::erase(OpenList, CurrentNodeRecord);
	}
	
	auto GoalNodeIter = std::ranges::find_if(ClosedList, [&pGoalNode](NodeRecord const& InnerRecord)
	{
		return InnerRecord.pNode->GetId() == pGoalNode->GetId();
	});
	
	if (GoalNodeIter == ClosedList.end())
	{
		auto ClosestNodeIter { std::ranges::min_element(ClosedList, [](NodeRecord const& RecordA, NodeRecord const& RecordB)
		{
			return RecordA.distanceToDest < RecordB.distanceToDest;
		})};
		
		CurrentNodeRecord = *ClosestNodeIter;
	}
	
	return BacktrackFullPath(ClosedList, CurrentNodeRecord);
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}