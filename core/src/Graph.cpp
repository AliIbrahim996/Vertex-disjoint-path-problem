//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim

#include "algolab/Graph.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace algolab
{
	int Graph::addNode(const std::string& label, double x, double y)
	{
		nodes.push_back(Node{ label, x, y });
		return size() - 1;
	}

	int Graph::addEdge(int u, int v, double weight)
	{
		edges.push_back(Edge{ u, v, weight });
		return static_cast<int>(edges.size()) - 1;
	}

	namespace
	{
		void sortByNeighbour(Adjacency& adjacency)
		{
			for (auto& arcs : adjacency)
			{
				std::stable_sort(arcs.begin(), arcs.end(), [](const Arc& a, const Arc& b) { return a.to < b.to; });
			}
		}
	}

	Adjacency buildAdjacency(const Graph& graph)
	{
		Adjacency adjacency(graph.nodes.size());
		for (int i = 0; i < static_cast<int>(graph.edges.size()); i++)
		{
			const Edge& e = graph.edges[i];
			adjacency[e.u].push_back(Arc{ e.v, i, e.weight });
			if (!graph.directed && e.u != e.v)
			{
				adjacency[e.v].push_back(Arc{ e.u, i, e.weight });
			}
		}
		sortByNeighbour(adjacency);
		return adjacency;
	}

	Adjacency buildReverseAdjacency(const Graph& graph)
	{
		if (!graph.directed)
		{
			return buildAdjacency(graph);
		}
		Adjacency adjacency(graph.nodes.size());
		for (int i = 0; i < static_cast<int>(graph.edges.size()); i++)
		{
			const Edge& e = graph.edges[i];
			adjacency[e.v].push_back(Arc{ e.u, i, e.weight });
		}
		sortByNeighbour(adjacency);
		return adjacency;
	}

	std::string formatNumber(double value)
	{
		if (std::isinf(value))
		{
			return value > 0 ? "\xE2\x88\x9E" : "-\xE2\x88\x9E"; // U+221E
		}
		const double rounded = std::round(value);
		char buffer[48];
		if (std::fabs(value - rounded) < 1e-9)
		{
			std::snprintf(buffer, sizeof(buffer), "%.0f", rounded == 0 ? 0.0 : rounded);
			return buffer;
		}
		std::snprintf(buffer, sizeof(buffer), "%.2f", value);
		std::string text{ buffer };
		while (!text.empty() && text.back() == '0')
		{
			text.pop_back();
		}
		return text;
	}
}
