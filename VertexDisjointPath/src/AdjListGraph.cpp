//!
//! 
//!  @author Ali Ibrahim

#include "AdjListGraph.h"

namespace VDP
{
	AdjListGraph::AdjListGraph(int size) :size_(size)
	{
		graph_.reserve(size);
	}

	void AdjListGraph::addEdge(int u, int v, int w)
	{
		graph_[u].push_back(Edge{ v,w,0 });
		graph_[v].push_back(Edge{ u,0,0 });
	}

	void AdjListGraph::zeroFlow()
	{
		for (int i = 0; i < size_; i++)
		{
			for (Edge e : graph_[i])
			{
				e.capacity = 0;
			}
		}
	}
	int AdjListGraph::getCapacity(int u, int v)
	{
		for (Edge e : graph_[u])
		{
			if (e.dest == v)
			{
				return e.capacity;
			}
		}
		return 0;
	}
	void AdjListGraph::updateWeight(int s, int dest, int w)
	{
		for (Edge e : graph_[s])
		{
			if (e.dest == dest)
			{
				e.weight += w;
			}
		}
	}
	std::unique_ptr<AdjListGraph> AdjListGraph::createResidualGraph()
	{
		std::unique_ptr<AdjListGraph> residualGraph{ std::make_unique<AdjListGraph>(size_) };
		for (int i = 0; i < size_; i++)
		{
			for (Edge e : graph_[i])
			{
				residualGraph->graph().at(i).emplace_back(e);
			}
		}
		return std::move(residualGraph);
	}
	bool AdjListGraph::checkEdge(int u, int v)
	{
		for (Edge e : graph_[u])
		{
			if (e.dest == v)
				return true;
		}
		return false;
	}
}