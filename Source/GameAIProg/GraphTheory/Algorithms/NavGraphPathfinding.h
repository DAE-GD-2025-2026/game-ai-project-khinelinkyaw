#pragma once
#include <memory>
#include <vector>

#include "Movement/Pathfinding/Navmesh/TriPolygon.h"

namespace GameAI
{
	class NavGraph;

	struct NavLine
	{
		FVector2D P1, P2;	
	};

	class NavMeshPathfinding
	{
	public:
		static void AddConnectionToNodeFromTriangle(TriPolygon::Triangle const* ConnTriangle, NavGraph* pGraph, TriPolygon const* pNavPolygon, int NodeIdToConnect);
		static std::vector<FVector2D> FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph,
		                                       std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals);
		static std::vector<FVector2D> FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph);
	};
}
