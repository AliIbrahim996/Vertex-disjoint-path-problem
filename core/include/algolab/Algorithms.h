//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim

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
		std::string algorithm;
		Graph graph;
		int source{ -1 };
		int target{ -1 };
		//! "euclidean", "manhattan" or "zero" (A* and greedy best-first).
		std::string heuristic{ "euclidean" };
		//! Canvas units that count as one unit of edge weight for the heuristics.
		double heuristicScale{ 50.0 };
		//! How max-flow looks for augmenting paths: "bfs", "dfs" or "widest".
		std::string strategy{ "bfs" };
	};

	enum class Need { No, Optional, Required };

	struct Choice
	{
		std::string id;
		std::string label;
	};

	//! A drop-down the UI should offer for an algorithm.
	struct Option
	{
		std::string key; //!< "heuristic" or "strategy"
		std::string label;
		std::vector<Choice> choices;
	};

	//! Catalogue entry: what the UI needs to know to offer an algorithm.
	struct AlgorithmInfo
	{
		std::string id;
		std::string name;
		std::string category;
		std::string description;
		std::string complexity;
		Need source{ Need::No };
		Need target{ Need::No };
		bool usesWeights{ false };
		std::string weightName; //!< "weight" or "capacity"
		std::vector<Option> options;
		void (*run)(const Request&, Tracer&) { nullptr };
	};

	const std::vector<AlgorithmInfo>& catalog();
	std::string catalogJson();

	//! Parse the line-based request format (see README). Returns false and fills `error` on bad input.
	bool parseRequest(const std::string& text, Request& request, std::string& error);

	//! Check the request against the algorithm's needs. Returns an empty string when it is fine.
	std::string validate(const Request& request);

	//! Validate, run, and return the trace as JSON ({"error": "..."} on failure).
	std::string run(const Request& request);
	std::string runFromText(const std::string& text);

	// The algorithms. Each one assumes a validated request.
	void runBfs(const Request& request, Tracer& tracer);
	void runDfs(const Request& request, Tracer& tracer);
	void runCycleDetection(const Request& request, Tracer& tracer);
	void runDijkstra(const Request& request, Tracer& tracer);
	void runAStar(const Request& request, Tracer& tracer);
	void runGreedyBestFirst(const Request& request, Tracer& tracer);
	void runMaxFlow(const Request& request, Tracer& tracer);
	void runVertexDisjointPaths(const Request& request, Tracer& tracer);
}
#endif // !ALGOLAB_ALGORITHMS_H
