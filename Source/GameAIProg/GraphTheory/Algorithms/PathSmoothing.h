#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
	class SSFA final
{
public:
	//=== SSFA Functions ===
	//--- References ---
	//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
	//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
	static std::vector<NavLine> FindPortals(std::vector<Node*> const & Path, TriPolygon const & NavPoly)
	{
		//Container
		std::vector<NavLine> Portals = {};
		
		//For each node received, get it's corresponding line
		for (auto iter = Path.begin(); iter != Path.end(); ++iter)
		{
			int edgeIdx {reinterpret_cast<NavGraphNode*>(*iter)->GetEdgeIdx()};
			
			if (edgeIdx < 0)
			{
				auto nodePosition { (*iter)->GetPosition() };
				Portals.push_back(NavLine{nodePosition,nodePosition});
				continue;
			}
			
			auto edge {NavPoly.GetEdges()[edgeIdx]};
			auto p1 { FVector2D{edge.GetP1(NavPoly)} };
			auto p2 { FVector2D{edge.GetP2(NavPoly)} };
			
			FVector2D pathOrientation{};
			auto nextIter { iter + 1 };
			FVector2D relativeP1{};
			
			if (nextIter != Path.end())
			{
				pathOrientation = (*nextIter)->GetPosition() - (*iter)->GetPosition();
				relativeP1 = p1 - (*iter)->GetPosition(); 
			}
			else
			{
				auto prevIter { iter - 1 };
				pathOrientation = (*iter)->GetPosition() - (*prevIter)->GetPosition();
				relativeP1 = p1 - (*prevIter)->GetPosition();
			}
			
			auto CrossResult {FVector2D::CrossProduct(pathOrientation, relativeP1)};
			
			NavLine newNavLine{};
			
			if (CrossResult > 0)
			{
				newNavLine = NavLine{ p1, p2 };
			}
			else
			{
				newNavLine = NavLine{ p2, p1 };
			}
			
			Portals.push_back(newNavLine);
		}
		
			//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point

			//Store portal

		//Add degenerate portal to force end evaluation

		return Portals;
	}

	static std::vector<FVector2D> OptimizePortals( std::vector<NavLine> const & Portals, TriPolygon const & NavPoly)
	{
		std::vector<FVector2D> Path{};
		auto apexPoint { Portals[0].P1};
		Path.push_back(apexPoint);
		
		auto rightLegIter { std::next(Portals.begin(), 1) };
		auto rightLeg { rightLegIter->P1 - apexPoint };
		auto leftLegIter { std::next(Portals.begin(), 1) };
		auto leftLeg { leftLegIter->P2 - apexPoint };
		
		auto portalIter = std::next(Portals.begin(), 2);
		
		while (portalIter < Portals.end())
		{
			// --- Right leg ---
			auto newRightLeg { portalIter->P1 - apexPoint };
			auto crossResult { FVector2D::CrossProduct(rightLeg, newRightLeg) };
			
			// Check if the new right leg is crossing the current one
			if (crossResult <= 0.0f)
			{
				crossResult = FVector2D::CrossProduct(leftLeg, newRightLeg);
				
				// Check if the new right leg is crossing the left one (Counterclockwise)
				if (crossResult < 0.0f)
				{
					apexPoint = leftLegIter->P2;
					Path.push_back(apexPoint);
					portalIter = std::next(leftLegIter);
					
					if (portalIter != Portals.end())
					{
						rightLegIter = leftLegIter = portalIter;
						rightLeg = rightLegIter->P1 - apexPoint;
						leftLeg = leftLegIter->P2 - apexPoint;
					}
				}
				else
				{
					rightLeg = newRightLeg;
					rightLegIter = portalIter;
				}
			}
			
			// --- Left leg ---
			auto newLeftLeg { portalIter->P2 - apexPoint };
			crossResult = FVector2D::CrossProduct(leftLeg, newLeftLeg);
			
			// Check if the new left leg is crossing the current one (Clockwise)
			if (crossResult >= 0.0f)
			{
				crossResult = FVector2D::CrossProduct(rightLeg, newLeftLeg);
				
				// Check if the new left leg is crossing the right one (Clockwise)
				if (crossResult > 0.0f)
				{
					apexPoint  = rightLegIter->P1;
					Path.push_back(apexPoint);
					portalIter = std::next(rightLegIter);
					
					if (portalIter != Portals.end())
					{
						rightLegIter = leftLegIter = portalIter;
						rightLeg = rightLegIter->P1 - apexPoint;
						leftLeg = leftLegIter->P2 - apexPoint;
					}
				}
				else
				{
					leftLeg = newLeftLeg;
					leftLegIter = portalIter;
				}
			}
			
			++portalIter;
		}
		
		Path.push_back(Portals.back().P1);
		
		//P1 == right point of portal, P2 == left point of portal
		
			//--- RIGHT CHECK ---
			//1. See if moving funnel inwards - RIGHT
			
				//2. See if new line degenerates a line segment - RIGHT
				
					//Leftleg becomes new apex point

					//Calculate new legs (if not the end)


			//--- LEFT CHECK ---
			//1. See if moving funnel inwards - LEFT

				//2. See if new line degenerates a line segment - LEFT

					//Rightleg becomes new apex point

					//Calculate new legs (if not the end)


		// Add last path point

		return Path;
	}
private:
	SSFA() {};
	~SSFA() {};
};
}
