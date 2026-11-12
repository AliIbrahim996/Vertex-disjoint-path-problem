//! @copyright 2022-2026 Ali Ibrahim. Licensed under the MIT License (see LICENSE).
//! @author Ali Ibrahim
//!
//! The public entry points of the core: the algorithm catalogue, the request format, and the
//! algorithms themselves.

#ifndef ALGOLAB_ALGORITHMS_H
#define ALGOLAB_ALGORITHMS_H

#include <string>
#include <vector>

#include "Graph.h"
#include "Trace.h"

namespace algolab
{
	//! Everything one simulation run needs.
	struct Request
	{
		//! Catalogue id of the algorithm to run ("bfs", "maxflow", "queens", ...).
		std::string algorithm;
		//! The graph to run on. Empty for puzzles that do not use one.
		Graph graph;
		//! Start vertex, or -1 when the algorithm does not need one.
		int source{ -1 };
		//! Goal vertex, or -1 for none (for BFS / DFS / Dijkstra: explore everything).
		int target{ -1 };
		//! "euclidean", "manhattan" or "zero" (A* and greedy best-first).
		std::string heuristic{ "euclidean" };
		//! Canvas units that count as one unit of edge weight for the heuristics.
		double heuristicScale{ 50.0 };
		//! How max-flow looks for augmenting paths: "bfs", "dfs" or "widest".
		std::string strategy{ "bfs" };
		//! Board size for the N-queens puzzle.
		int size{ 8 };
		//! TSP method: "exact", "nearest" or "2opt".
		std::string method{ "exact" };
	};

	//! Whether an algorithm needs a source or target vertex.
	enum class Need
	{
		No,       //!< not used; the UI hides the picker
		Optional, //!< may be left empty (-1)
		Required  //!< must be set; validate() rejects the request otherwise
	};

	//! One entry of a drop-down.
	struct Choice
	{
		std::string id;    //!< value sent in the request
		std::string label; //!< text shown in the drop-down
	};

	//! A drop-down the UI should offer for an algorithm.
	struct Option
	{
		std::string key;             //!< request keyword it sets: "heuristic", "strategy", "size" or "method"
		std::string label;           //!< text shown next to the drop-down
		std::vector<Choice> choices; //!< the first one is the default
	};

	//! Catalogue entry: what the UI needs to know to offer an algorithm.
	struct AlgorithmInfo
	{
		std::string id;          //!< value of Request::algorithm
		std::string name;        //!< display name
		std::string category;    //!< group in the algorithm menu
		std::string description; //!< plain-language explanation shown under the menu
		std::string complexity;  //!< time / space complexity as text
		Need source{ Need::No };
		Need target{ Need::No };
		bool usesWeights{ false }; //!< edge weights matter: the UI shows and edits them
		std::string weightName;    //!< what a weight means here: "weight", "capacity" or "distance"
		std::vector<Option> options;
		//! The implementation. Called only with a request that passed validate().
		void (*run)(const Request&, Tracer&) { nullptr };
		//! false for puzzles that do not work on the drawn graph (N-queens).
		bool usesGraph{ true };
	};

	//! @returns every available algorithm, in menu order.
	const std::vector<AlgorithmInfo>& catalog();

	//! @returns the catalogue as JSON, as the web page reads it (everything except `run`).
	std::string catalogJson();

	//! Parse the line-based request format (see README): one `keyword value...` per line, such as
	//! `algorithm bfs`, `node 100 50 A` or `edge 0 1 4`. Blank lines and `#` comments are ignored.
	//! @param[in]  text    The request.
	//! @param[out] request Filled from the text; fields not mentioned keep their defaults.
	//! @param[out] error   Set to a message naming the bad line when parsing fails.
	//! @returns false on an unknown keyword or a value that cannot be read.
	bool parseRequest(const std::string& text, Request& request, std::string& error);

	//! Check the request against the algorithm's needs: known algorithm, a non-empty graph of at
	//! most 200 vertices, valid edge endpoints, non-negative weights where they matter, and the
	//! required source / target. Some algorithms add their own checks (TSP, N-queens).
	//! @param[in] request The request to check.
	//! @returns "" when the request can be run, otherwise a message for the user.
	std::string validate(const Request& request);

	//! Validate the request, run the algorithm and serialise its trace.
	//! @param[in] request The request.
	//! @returns the trace as JSON (see Tracer::toJson()), or {"error": "..."} if validation fails.
	std::string run(const Request& request);

	//! parseRequest() followed by run(). This is what the web page calls.
	//! @param[in] text A request in the line-based format.
	//! @returns the trace as JSON, or {"error": "..."} if the text cannot be parsed or validated.
	std::string runFromText(const std::string& text);

	// The algorithms. Each one assumes a validated request and records its steps on `tracer`.
	// Besides the steps, each sets a summary and results that the tests check.

	//! Breadth-first search from `source`, stopping at `target` if one is set.
	//! Results: "visited"; with a target also "found", "distance" (edges) and "path".
	//! @param[in]     request The validated request.
	//! @param[in,out] tracer  Receives the steps.
	void runBfs(const Request& request, Tracer& tracer);

	//! Depth-first search from `source`, stopping at `target` if one is set. Labels each vertex
	//! with its discovery/finish times and classifies every edge.
	//! Results: "visited"; with a target also "found" and "path".
	//! @param[in]     request The validated request.
	//! @param[in,out] tracer  Receives the steps.
	void runDfs(const Request& request, Tracer& tracer);

	//! Cycle detection by three-colour DFS over the whole graph.
	//! Results: "cycle"; if found also "length" and "vertices"; for a DAG "topologicalOrder".
	//! @param[in]     request The validated request (source and target are ignored).
	//! @param[in,out] tracer  Receives the steps.
	void runCycleDetection(const Request& request, Tracer& tracer);

	//! Dijkstra's shortest paths from `source` (to `target` if one is set).
	//! Results: "expanded"; with a target also "found", "cost", "optimalCost", "optimal" and "path".
	//! @param[in]     request The validated request; weights are non-negative.
	//! @param[in,out] tracer  Receives the steps.
	void runDijkstra(const Request& request, Tracer& tracer);

	//! A* search from `source` to `target`, using `heuristic` measured on the canvas.
	//! Results: as runDijkstra(), plus "admissible".
	//! @param[in]     request The validated request; weights are non-negative.
	//! @param[in,out] tracer  Receives the steps.
	void runAStar(const Request& request, Tracer& tracer);

	//! Greedy best-first search from `source` to `target`, guided by `heuristic` alone.
	//! Results: as runAStar().
	//! @param[in]     request The validated request; weights are non-negative.
	//! @param[in,out] tracer  Receives the steps.
	void runGreedyBestFirst(const Request& request, Tracer& tracer);

	//! Maximum flow from `source` to `target` by augmenting paths (see `strategy`), followed by
	//! the minimum cut. Edge weights are capacities.
	//! Results: "maxFlow", "minCut", "cutEdges".
	//! @param[in]     request The validated request; source and target differ.
	//! @param[in,out] tracer  Receives the steps.
	void runMaxFlow(const Request& request, Tracer& tracer);

	//! Maximum number of vertex-disjoint paths from `source` to `target`, by splitting every
	//! inner vertex and running max-flow on the result. Uses two views.
	//! Results: "paths" (the count) and "list".
	//! @param[in]     request The validated request; source and target differ.
	//! @param[in,out] tracer  Receives the steps.
	void runVertexDisjointPaths(const Request& request, Tracer& tracer);

	//! N-queens on a `size` x `size` board by row-by-row backtracking (no graph needed).
	//! Results: "solved", "placements", "backtracks"; if solved also "columns" (1-based, per row).
	//! @param[in]     request The validated request; only `size` is used.
	//! @param[in,out] tracer  Receives the steps.
	void runEightQueens(const Request& request, Tracer& tracer);

	//! Travelling salesman tour from `source` over the drawn undirected roads, by `method`:
	//! branch and bound, nearest neighbour, or nearest neighbour + 2-opt.
	//! Results: "found"; if found also "cost", "tour" and "optimal"; heuristics add "optimalCost".
	//! @param[in]     request The validated request: undirected, 3 to 10 cities.
	//! @param[in,out] tracer  Receives the steps.
	void runTravellingSalesman(const Request& request, Tracer& tracer);
}
#endif // !ALGOLAB_ALGORITHMS_H
