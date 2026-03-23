#include "NavGraph.h"

#include "NavGraphNode.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
	: Graph{false}
	, pNavPoly{std::move(NavPoly)}
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	Nodes.reserve(Other.Nodes.size());
	for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
	{
		Nodes.push_back(std::make_unique<NavGraphNode>(*dynamic_cast<NavGraphNode*>(OtherNode.get())));
	}
        
	Connections.reserve(Other.Connections.size());
	for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
	if (EdgeIdx >= 0)
	{
		for (auto const & pNode : Nodes)
		{
			if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
			{
				return pNode->GetId();
			}
		}
	}
	
	return Graphs::InvalidNodeId;
}

void GameAI::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigation mesh and create nodes
			// Create node here
	
	// Get all the lines
	auto NavLines { pNavPoly->GetEdges() };
	
	for (int i = 0; i < NavLines.size(); ++i)
	{
		// Check how many triangles the line is connected to
		auto NavTriangles {pNavPoly->GetTrianglesFromLineIndex(NavLines[i])};
		
		// If there's less than 2 triangles connected, discard that line
		if (NavTriangles.size() < 2)
		{
			continue;
		}
		
		auto NavPos { FVector2D{NavLines[i].GetMidPoint(*pNavPoly)} };
		
		// Else, make a node in the middle of the line
		AddNode(std::make_unique<NavGraphNode>(NavPos, i));
	}
	
	auto NavTrigs { pNavPoly->GetTriangles() };
	
	for (auto const & Trig : NavTrigs)
	{
		auto Edges {Trig.GetEdges()};
		
		std::vector<int> NodeIds{};
		for (auto const & Edge : Edges)
		{
			auto EdgeIndex { pNavPoly->FindEdgeIndex(Edge) };
			
			if (!EdgeIndex.has_value())
			{
				continue;
			}
			
			auto NewNodeId { GetNodeIdFromEdgeIndex(EdgeIndex.value()) };
			
			if (NewNodeId != Graphs::InvalidNodeId)
			{
				NodeIds.push_back(NewNodeId);
			}
			
		}
		
		if (NodeIds.size() == 2)
		{
			AddConnection(NodeIds[0], NodeIds[1]);
		}
		else if (NodeIds.size() == 3)
		{
			AddConnection(NodeIds[0], NodeIds[1]);
			AddConnection(NodeIds[1], NodeIds[2]);
			AddConnection(NodeIds[2], NodeIds[0]);
		}
	}
	
	//2. Create connections now that every node is created	
		//2 valid nodes -> 1 connection
		//3 valid nodes -> 3 connections
		
	//3. Set the connections cost to the actual distance
}
