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
	CurrentNodeRecord.estimatedTotalCost = CurrentNodeRecord.costSoFar + GetHeuristicCost(pStartNode, pGoalNode);
	OpenList.push_back(CurrentNodeRecord);
	

	
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
			
			NodeRecord NeighborNodeRecord{};
			NeighborNodeRecord.pNode = pGraph->GetNode(NeigborNodeId).get();
			NeighborNodeRecord.pConnection = Conn;
			NeighborNodeRecord.costSoFar = CurrentNodeRecord.costSoFar;
			NeighborNodeRecord.estimatedTotalCost = NeighborNodeRecord.costSoFar + GetHeuristicCost(NeighborNodeRecord.pNode, pGoalNode);
			
			auto ClosedListIter = std::ranges::find(ClosedList, NeighborNodeRecord);
			
			if (ClosedListIter != ClosedList.end())
			{
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
		
		auto NextNodeIter = std::ranges::find_if(ClosedList, [CurrentNodeRecord](NodeRecord const& Record)
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
	
	return Path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}