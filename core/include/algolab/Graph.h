//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim

#ifndef ALGOLAB_GRAPH_H
#define ALGOLAB_GRAPH_H

#include <string>
#include <vector>

namespace algolab
{
	//! A vertex with a display label and a position on the canvas.
	//! The position is also what the geometric heuristics (A*, greedy) measure.
	struct Node
	{
		std::string label;
		double x{};
		double y{};
	};

	//! An edge u -> v (or u -- v when the graph is undirected).
	//! `weight` is a length for shortest-path algorithms and a capacity for flow algorithms.
	struct Edge
	{
		int u{};
		int v{};
		double weight{ 1.0 };
	};

	struct Graph
	{
		bool directed{ true };
		std::vector<Node> nodes;
		std::vector<Edge> edges;

		int size() const { return static_cast<int>(nodes.size()); }
		int addNode(const std::string& label, double x = 0, double y = 0);
		int addEdge(int u, int v, double weight = 1.0);
	};

	//! One way of leaving a vertex: follow edge `edge` and arrive at `to`.
	struct Arc
	{
		int to{};
		int edge{};
		double weight{};
	};

	using Adjacency = std::vector<std::vector<Arc>>;

	//! Outgoing arcs per vertex, ordered by neighbour index so runs are deterministic.
	//! An undirected edge appears in the lists of both of its endpoints.
	Adjacency buildAdjacency(const Graph& graph);

	//! Incoming arcs per vertex (`to` is the tail of the original edge).
	Adjacency buildReverseAdjacency(const Graph& graph);

	//! "3", "2.5", or the infinity sign.
	std::string formatNumber(double value);
}
#endif // !ALGOLAB_GRAPH_H
