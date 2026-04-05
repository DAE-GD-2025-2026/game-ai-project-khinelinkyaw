#include "AStar.h"

#include "GeometryCollection/GeometryCollectionConvexUtility.h"

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: pGraph(pGraph)
	, HeuristicFunction(hFunction)
{
}

std::vector<Node*>AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
	std::vector<Node*> Path{};
	std::vector<NodeRecord> OpenList{};
	std::vector<NodeRecord> ClosedList{};
	
	NodeRecord CurrentNodeRecord{};
	CurrentNodeRecord.pNode = pStartNode;
	CurrentNodeRecord.pConnection = nullptr;
	CurrentNodeRecord.costSoFar = 0.0f;
	CurrentNodeRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);
	OpenList.push_back(CurrentNodeRecord);
	
	NodeRecord NeighborNodeRecord{};
	auto FindNeighborNodeId { [&NeighborNodeRecord](NodeRecord const& InnerRecord){ return InnerRecord.pNode->GetId() == NeighborNodeRecord.pNode->GetId(); } };
	
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
			auto NeigborNodeId { Conn->GetToId() };
			
			NeighborNodeRecord.pNode = pGraph->GetNode(NeigborNodeId).get();
			NeighborNodeRecord.pConnection = Conn;
			NeighborNodeRecord.costSoFar = CurrentNodeRecord.costSoFar + Conn->GetWeight();
			NeighborNodeRecord.estimatedTotalCost = NeighborNodeRecord.costSoFar + GetHeuristicCost(NeighborNodeRecord.pNode, pGoalNode);
			
			auto OpenListIter = std::ranges::find_if(OpenList, FindNeighborNodeId);
			auto ClosedListIter = std::ranges::find_if(ClosedList, FindNeighborNodeId);
			
			if (OpenListIter != OpenList.end() and NeighborNodeRecord.costSoFar < OpenListIter->costSoFar)
			{
				OpenList.erase(OpenListIter);
				OpenList.push_back(NeighborNodeRecord);
				continue;
			}
			if (ClosedListIter != ClosedList.end() and NeighborNodeRecord.costSoFar < ClosedListIter->costSoFar)
			{
				ClosedList.erase(ClosedListIter);
				ClosedList.push_back(NeighborNodeRecord);
				continue;
			}
			
			OpenList.push_back(NeighborNodeRecord);
		}
		
		ClosedList.push_back(CurrentNodeRecord);
		std::erase(OpenList, CurrentNodeRecord);
	}
	
	while (CurrentNodeRecord.pConnection != nullptr)
	{
		Path.push_back(CurrentNodeRecord.pNode);
		
		auto NextNodeIter = std::ranges::find_if(ClosedList, [&CurrentNodeRecord](NodeRecord const& Record)
		{
			auto NextNodeId { CurrentNodeRecord.pConnection->GetFromId() };
			if (NextNodeId == Record.pNode->GetId())
			{
				return true;
			}
			return false;
		});
		
		CurrentNodeRecord = *NextNodeIter;
	}
	
	Path.push_back(CurrentNodeRecord.pNode);
	
	return Path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}