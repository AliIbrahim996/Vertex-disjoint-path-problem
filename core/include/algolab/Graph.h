//! @copyright 2022-2026 Ali Ibrahim. Licensed under the MIT License (see LICENSE).
//! @author Ali Ibrahim
//!
//! The graph model shared by every algorithm: vertices with canvas positions, weighted edges,
//! and adjacency lists built from them.

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
		//! Name shown inside the vertex and used in explanations ("A", "S", "c4").
		std::string label;
		//! Horizontal canvas position (for a board view: the column).
		double x{};
		//! Vertical canvas position, growing downwards (for a board view: the row).
		double y{};
	};

	//! An edge u -> v (or u -- v when the graph is undirected).
	//! `weight` is a length for shortest-path algorithms and a capacity for flow algorithms.
	struct Edge
	{
		//! Index of the tail vertex (where a directed edge starts).
		int u{};
		//! Index of the head vertex (where a directed edge ends).
		int v{};
		//! Length, distance or capacity, depending on the algorithm. Ignored by unweighted ones.
		double weight{ 1.0 };
	};

	//! A graph as the user drew it. Vertices and edges are identified by their index in
	//! `nodes` and `edges`, and those indices are what the tracer and the UI refer to.
	struct Graph
	{
		//! true: every edge goes from `u` to `v` only. false: every edge can be used both ways.
		bool directed{ true };
		std::vector<Node> nodes;
		std::vector<Edge> edges;

		//! @returns the number of vertices.
		int size() const { return static_cast<int>(nodes.size()); }

		//! Append a vertex.
		//! @param[in] label The display name.
		//! @param[in] x     Canvas position, horizontal.
		//! @param[in] y     Canvas position, vertical.
		//! @returns the index of the new vertex.
		int addNode(const std::string& label, double x = 0, double y = 0);

		//! Append an edge. The endpoints are not checked here; see validate().
		//! @param[in] u      Index of the tail vertex.
		//! @param[in] v      Index of the head vertex.
		//! @param[in] weight Length or capacity.
		//! @returns the index of the new edge.
		int addEdge(int u, int v, double weight = 1.0);
	};

	//! One way of leaving a vertex: follow edge `edge` and arrive at `to`.
	struct Arc
	{
		//! The vertex this arc leads to.
		int to{};
		//! Index of the graph edge it follows, so the tracer can colour that edge.
		int edge{};
		//! Copy of the edge's weight.
		double weight{};
	};

	//! `adjacency[v]` lists the arcs leaving vertex v.
	using Adjacency = std::vector<std::vector<Arc>>;

	//! Build the outgoing arcs of every vertex, ordered by neighbour index so that runs are
	//! deterministic. An undirected edge appears in the lists of both of its endpoints.
	//! @param[in] graph The graph to index.
	//! @returns one list of outgoing arcs per vertex.
	Adjacency buildAdjacency(const Graph& graph);

	//! Build the incoming arcs of every vertex: for an edge u -> v, vertex v gets an arc whose
	//! `to` is u. Used to compute distances *to* a vertex. Same as buildAdjacency() for an
	//! undirected graph.
	//! @param[in] graph The graph to index.
	//! @returns one list of incoming arcs per vertex.
	Adjacency buildReverseAdjacency(const Graph& graph);

	//! Format a number for display: whole numbers without decimals, others with up to two.
	//! @param[in] value The number to format.
	//! @returns "3", "2.5", or the infinity sign for an infinite value.
	std::string formatNumber(double value);
}
#endif // !ALGOLAB_GRAPH_H
