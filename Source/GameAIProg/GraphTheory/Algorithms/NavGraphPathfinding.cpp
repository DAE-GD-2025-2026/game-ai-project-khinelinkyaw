#include "NavGraphPathfinding.h"

#include <unordered_set>

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

void NavMeshPathfinding::AddConnectionToNodeFromTriangle(TriPolygon::Triangle const* ConnTriangle, NavGraph* pGraph, TriPolygon const* pNavPolygon, int NodeIdToConnect)
{
	auto StartTriEdges { ConnTriangle->GetEdges() };
	
	for (auto& StartEdge : StartTriEdges)
	{
		auto EdgeIndex { pNavPolygon->FindEdgeIndex(StartEdge) };
		
		if (!EdgeIndex.has_value()) continue;
		
		int NodeId { pGraph->GetNodeIdFromEdgeIndex(EdgeIndex.value()) };
		
		if (NodeId == Graphs::InvalidNodeId) continue;
		
		pGraph->AddConnection(NodeId, NodeIdToConnect);
	}
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
                                                    NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals) 
{
	//Create the path to return
	std::vector<FVector2D> finalPath{};

	//Get the start and endTriangle
	auto pNavPoly { pNavGraph->GetNavPolygon() };
	
	auto StartTri { pNavPoly->GetTriangleAtPosition(startPos, true) };
	auto EndTri{ pNavPoly->GetTriangleAtPosition(endPos, true) };
	
	if (StartTri == EndTri or StartTri == nullptr or EndTri == nullptr)
	{
		return finalPath;
	}

	//We have valid start/end triangles and they are not the same
	//=> Start looking for a path
	//Copy the graph
	auto PCopyGraph { pNavGraph->Clone() };

	//Create Extra node for the Start Node (Agent's position
	int StartNodeId { PCopyGraph->AddNode(std::make_unique<NavGraphNode>(startPos, -1)) };
	std::unique_ptr<Node> const& StartNode {PCopyGraph->GetNode(StartNodeId)};
	
	AddConnectionToNodeFromTriangle(StartTri, PCopyGraph.get(), pNavPoly, StartNodeId);
	
	//Create extra node for the endNode
	int EndNodeId { PCopyGraph->AddNode(std::make_unique<NavGraphNode>(endPos, -1)) };
	std::unique_ptr<Node> const& EndNode {PCopyGraph->GetNode(EndNodeId)};

	AddConnectionToNodeFromTriangle(EndTri, PCopyGraph.get(), pNavPoly, EndNodeId);
	//Run A star on new graph

	AStar AStarAlgo {PCopyGraph.get(), HeuristicFunctions::Manhattan};
	
	auto nodePath = AStarAlgo.FindPath(StartNode.get(), EndNode.get());
	
	// Unoptimized path
	// finalPath.reserve(nodePath.size());
	// for (auto iter = nodePath.begin(); iter != nodePath.end(); ++iter)
	// {
	// 	finalPath.emplace_back((*iter)->GetPosition());
	// }
	
	// Extra: Run optimizer on new graph (First check if everything works without SSFA!)
	debugPortals = SSFA::FindPortals(nodePath, *pNavGraph->GetNavPolygon());
	finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
	
	return finalPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
	std::vector<FVector2D> debugNodePositions{};
	std::vector<NavLine> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}