#pragma once
#include <stack>
#include "Shared/Graph/Graph.h"

namespace GameAI
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<Node*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// TODO If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected())
		{
			return Eulerianity::notEulerian;
		}
		
		// TODO Count nodes with odd degree 
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		std::vector<Node*> OddDegreeNodes{};
		
		for (auto Node : Nodes)
		{
			if (m_pGraph->FindConnectionsFrom(Node->GetId()).size() % 2 != 0)
			{
				OddDegreeNodes.push_back(Node);
			}
		}
		
		// TODO A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (OddDegreeNodes.size() > 2)
		{
			return Eulerianity::notEulerian;
		}
		
		// TODO A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// TODO An Euler trail can be made, but only starting and ending in these 2 nodes
		if (OddDegreeNodes.size() == 2)
		{
			if (Nodes.size() == 2)
			{
				return Eulerianity::eulerian;
			}
			
			return Eulerianity::semiEulerian;
		}
		
		// TODO A connected graph with no odd nodes is Eulerian
		if (OddDegreeNodes.size() == 0)
		{
			return Eulerianity::eulerian;
		}
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph graphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = graphCopy.GetActiveNodes();
		int currentNodeId{ Graphs::InvalidNodeId };
		
		// TODO Check if there can be an Euler path
		auto PathEulerCheck { IsEulerian() };
		
		// TODO If this graph is not eulerian, return the empty path
		if (PathEulerCheck == Eulerianity::notEulerian)
		{
			return Path;
		}
		
		// TODO Start algorithm loop
		std::stack<int> NodeStack;
		
		if (PathEulerCheck == Eulerianity::semiEulerian)
		{
			for (Node* const Node : Nodes)
			{
				auto const Connections { graphCopy.FindConnectionsFrom(Node->GetId())};
				
				if (Connections.size() % 2 != 0)
				{
					currentNodeId = Node->GetId();
					break;
				}
			}
			
			// auto OddNodeIter { std::ranges::find_if(Nodes, [graphCopy](Node const* CurNode)
			// {
			// 	auto Connections { graphCopy.FindConnectionsFrom(CurNode->GetId())};
			// 	
			// 	if (Connections.size() % 2 != 0)
			// 	{
			// 		return true;
			// 	}
			// 	return false;
			// })};
			//
			// if (OddNodeIter == Nodes.end())
			// {
			// 	return Path;
			// }
			//
			// currentNodeId = (*OddNodeIter)->GetId();
		}
		else
		{
			currentNodeId = Nodes.back()->GetId();
		}
		
		while (NodeStack.size() > 0 and graphCopy.FindConnectionsFrom(currentNodeId).size() > 0)
		{
			NodeStack.push(currentNodeId);
			
			auto Connections { graphCopy.FindConnectionsFrom(currentNodeId) };
			
			if (Connections.size() == 0)
			{
				while (NodeStack.size() > 0)
				{
					Path.push_back(m_pGraph->GetNode(NodeStack.top()).get());
					NodeStack.pop();
				}
				currentNodeId = Nodes.back()->GetId();
				
				continue;
			}
			
			currentNodeId = Connections.front()->GetToId();
			graphCopy.RemoveConnection(Connections.front());
		}

		std::ranges::reverse(Path);
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		// TODO Mark the visited node
		if (visited[startIndex])
		{
			return;
		}
		
		visited[startIndex] = true;

		// TODO Ask the graph for the connections from that node
		
		auto Connections { m_pGraph->FindConnectionsFrom(Nodes[startIndex]->GetId()) };
		if (Connections.size() == 0)
		{
			return;
		}
		
		// TODO recursively visit any valid connected nodes that were not visited before
		for (auto Connection : Connections)
		{
			for (int i = 0; i < Nodes.size(); ++i)
			{
				if (Nodes[i]->GetId() == Connection->GetToId())
				{
					VisitAllNodesDFS(Nodes, visited, i);
				}
			}
		}
		// TODO Tip: use an index-based for-loop to find the correct index
	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		if (Nodes.size() == 0)
			return false;
		
		std::vector<bool> VisitedNodeFlags (Nodes.size(), false);
		
		// TODO choose a starting node
		
		// TODO start a depth-first-search traversal from the node that has at least one connection
		VisitAllNodesDFS(Nodes, VisitedNodeFlags , 0);
		
		// TODO if a node was never visited, this graph is not connected
		for (bool VisitedFlag : VisitedNodeFlags )
		{
			if (!VisitedFlag)
			{
				return false;
			}
		}
		
		return true;
	}
}