//! @copyright 2022-2026 Ali Ibrahim. Licensed under the MIT License (see LICENSE).
//! @author Ali Ibrahim

#include <cmath>
#include <sstream>

#include "algolab/Algorithms.h"

namespace algolab
{
	namespace
	{
		const int maxVertices = 200;

		const Option heuristicOption{ "heuristic", "Heuristic", {
			{ "euclidean", "Straight-line distance" },
			{ "manhattan", "Manhattan distance" },
			{ "zero", "h = 0 (no guidance)" } } };

		const Option strategyOption{ "strategy", "Augmenting path", {
			{ "bfs", "Shortest (BFS, Edmonds-Karp)" },
			{ "dfs", "Any (DFS, Ford-Fulkerson)" },
			{ "widest", "Widest path" } } };

		const Option sizeOption{ "size", "Board size", {
			{ "8", "8 \xC3\x97 8 (eight queens)" },
			{ "4", "4 \xC3\x97 4" },
			{ "5", "5 \xC3\x97 5" },
			{ "6", "6 \xC3\x97 6" },
			{ "7", "7 \xC3\x97 7" },
			{ "3", "3 \xC3\x97 3 (no solution)" } } };

		const Option methodOption{ "method", "Method", {
			{ "exact", "Exact: branch and bound" },
			{ "nearest", "Nearest neighbour (greedy)" },
			{ "2opt", "Nearest neighbour + 2-opt" } } };

		const int maxCities = 10;

		std::vector<AlgorithmInfo> buildCatalog()
		{
			return {
				{ "bfs", "Breadth-first search", "Traversal",
					"Explores the graph in rings around the source: first every vertex one edge away, then two edges away, and so on. "
					"A queue keeps the vertices in the order they were discovered, which is why BFS finds the path with the fewest edges.",
					"O(V + E) time, O(V) space", Need::Required, Need::Optional, false, "", {}, &runBfs },
				{ "dfs", "Depth-first search", "Traversal",
					"Follows one path as deep as it can and only backtracks when it is stuck. The discovery/finish times it stamps on "
					"vertices classify every edge as tree, back, forward or cross, which is the basis of cycle detection, topological "
					"sorting and strongly connected components.",
					"O(V + E) time, O(V) space", Need::Required, Need::Optional, false, "", {}, &runDfs },
				{ "cycle", "Cycle detection", "Traversal",
					"Runs a depth-first search with three colours: white (untouched), grey (on the current path) and black (finished). "
					"An edge into a grey vertex returns to the path being explored, so it closes a cycle. If that never happens a directed "
					"graph is a DAG and the reversed finish order is a topological order.",
					"O(V + E) time, O(V) space", Need::No, Need::No, false, "", {}, &runCycleDetection },
				{ "dijkstra", "Dijkstra's shortest paths", "Shortest path",
					"Grows a set of vertices whose shortest distance is final. Each round it takes the closest vertex not yet in the set "
					"and relaxes its outgoing edges. It needs non-negative weights: only then can a vertex taken from the queue never be "
					"improved later.",
					"O((V + E) log V) with a binary heap", Need::Required, Need::Optional, true, "weight", {}, &runDijkstra },
				{ "astar", "A* search", "Shortest path",
					"Dijkstra with a sense of direction. Vertices are ordered by f = g + h: the cost already paid plus a heuristic estimate "
					"of the cost still to go. If h never overestimates (it is admissible) the first path found is optimal, usually after "
					"expanding far fewer vertices than Dijkstra. Here h is measured on the canvas, so drag vertices to change it.",
					"O((V + E) log V) worst case; depends on the heuristic", Need::Required, Need::Required, true, "weight", { heuristicOption }, &runAStar },
				{ "greedy", "Greedy best-first (heuristic search)", "Shortest path",
					"Trusts the heuristic completely: it always expands the vertex that looks closest to the target and ignores the cost "
					"paid so far. It is fast and often good enough, but it can be led down a path that looks promising and turns out long. "
					"Compare its result with A* on the same graph.",
					"O((V + E) log V) worst case; not optimal", Need::Required, Need::Required, true, "weight", { heuristicOption }, &runGreedyBestFirst },
				{ "maxflow", "Maximum flow", "Network flow",
					"Edge weights are capacities. The Ford-Fulkerson method repeatedly finds a path from source to sink that still has "
					"spare capacity in the residual graph and pushes as much as the path's bottleneck allows. Walking an edge backwards "
					"undoes earlier flow. When no such path is left the flow is maximum, and the full edges leaving the source side form a "
					"minimum cut of the same value.",
					"O(V E\xC2\xB2) with BFS (Edmonds-Karp); O(E \xC2\xB7 maxflow) with DFS", Need::Required, Need::Required, true, "capacity", { strategyOption }, &runMaxFlow },
				{ "vdp", "Vertex-disjoint paths", "Network flow",
					"How many routes lead from s to t without sharing any intermediate vertex? Split every vertex into an entrance and an "
					"exit joined by an edge of capacity 1, give every edge capacity 1, and compute a maximum flow: each unit of flow is one "
					"path and no two units can squeeze through the same vertex. By Menger's theorem the answer also equals the fewest "
					"vertices whose removal disconnects s from t (when s and t are not adjacent).",
					"O(V E) with augmenting paths", Need::Required, Need::Required, false, "", { strategyOption }, &runVertexDisjointPaths },
				{ "queens", "Eight queens (backtracking)", "Combinatorial search",
					"Place N queens on an N \xC3\x97 N board so that no two share a row, a column or a diagonal. Backtracking fills the "
					"board one row at a time and tries each column in turn. When no column in a row is safe, it removes the previous "
					"queen and moves that one to its next column. Most of the possible boards are never looked at.",
					"O(N!) worst case", Need::No, Need::No, false, "", { sizeOption }, &runEightQueens, false },
				{ "tsp", "Travelling salesman (TSP)", "Combinatorial search",
					"Find the shortest round trip that visits every city exactly once and returns to the start, using the roads you "
					"drew. The problem is NP-hard: branch and bound finds the optimum but may explore a huge number of partial tours, "
					"while nearest neighbour is fast but can be far off. 2-opt then improves a tour by swapping pairs of edges until "
					"no swap makes it shorter.",
					"Exact: O(n!) worst case; nearest neighbour: O(n\xC2\xB2); 2-opt: O(n\xC2\xB2) per pass", Need::Required, Need::No, true, "distance", { methodOption }, &runTravellingSalesman } };
		}

		const char* needName(Need need)
		{
			switch (need)
			{
			case Need::Optional: return "optional";
			case Need::Required: return "required";
			default: return "no";
			}
		}

		const AlgorithmInfo* find(const std::string& id)
		{
			for (const AlgorithmInfo& info : catalog())
			{
				if (info.id == id) return &info;
			}
			return nullptr;
		}

		std::string errorJson(const std::string& message)
		{
			return "{\"error\":" + jsonString(message) + "}";
		}
	}

	const std::vector<AlgorithmInfo>& catalog()
	{
		static const std::vector<AlgorithmInfo> entries = buildCatalog();
		return entries;
	}

	std::string catalogJson()
	{
		std::string out{ "[" };
		bool first = true;
		for (const AlgorithmInfo& info : catalog())
		{
			if (!first) out += ',';
			first = false;
			out += "{\"id\":" + jsonString(info.id);
			out += ",\"name\":" + jsonString(info.name);
			out += ",\"category\":" + jsonString(info.category);
			out += ",\"description\":" + jsonString(info.description);
			out += ",\"complexity\":" + jsonString(info.complexity);
			out += ",\"source\":" + jsonString(needName(info.source));
			out += ",\"target\":" + jsonString(needName(info.target));
			out += ",\"usesWeights\":" + std::string(info.usesWeights ? "true" : "false");
			out += ",\"usesGraph\":" + std::string(info.usesGraph ? "true" : "false");
			out += ",\"weightName\":" + jsonString(info.weightName);
			out += ",\"options\":[";
			for (std::size_t o = 0; o < info.options.size(); o++)
			{
				const Option& option = info.options[o];
				if (o) out += ',';
				out += "{\"key\":" + jsonString(option.key) + ",\"label\":" + jsonString(option.label) + ",\"choices\":[";
				for (std::size_t c = 0; c < option.choices.size(); c++)
				{
					if (c) out += ',';
					out += "{\"id\":" + jsonString(option.choices[c].id) + ",\"label\":" + jsonString(option.choices[c].label) + "}";
				}
				out += "]}";
			}
			out += "]}";
		}
		out += "]";
		return out;
	}

	bool parseRequest(const std::string& text, Request& request, std::string& error)
	{
		std::istringstream input{ text };
		std::string line;
		int lineNumber = 0;
		while (std::getline(input, line))
		{
			lineNumber++;
			if (!line.empty() && line.back() == '\r') line.pop_back();
			std::istringstream fields{ line };
			std::string keyword;
			if (!(fields >> keyword) || keyword[0] == '#')
			{
				continue;
			}

			bool ok = true;
			if (keyword == "algorithm") ok = static_cast<bool>(fields >> request.algorithm);
			else if (keyword == "heuristic") ok = static_cast<bool>(fields >> request.heuristic);
			else if (keyword == "strategy") ok = static_cast<bool>(fields >> request.strategy);
			else if (keyword == "method") ok = static_cast<bool>(fields >> request.method);
			else if (keyword == "size") ok = static_cast<bool>(fields >> request.size);
			else if (keyword == "source") ok = static_cast<bool>(fields >> request.source);
			else if (keyword == "target") ok = static_cast<bool>(fields >> request.target);
			else if (keyword == "scale") ok = static_cast<bool>(fields >> request.heuristicScale);
			else if (keyword == "directed")
			{
				int flag = 0;
				ok = static_cast<bool>(fields >> flag);
				request.graph.directed = flag != 0;
			}
			else if (keyword == "node")
			{
				double x = 0, y = 0;
				std::string label;
				ok = static_cast<bool>(fields >> x >> y);
				std::getline(fields >> std::ws, label);
				if (label.empty()) label = std::to_string(request.graph.size());
				if (ok) request.graph.addNode(label, x, y);
			}
			else if (keyword == "edge")
			{
				int u = 0, v = 0;
				double weight = 1;
				ok = static_cast<bool>(fields >> u >> v);
				// The weight is optional (default 1), but if present it must be a number.
				if (ok && !(fields >> std::ws).eof()) ok = static_cast<bool>(fields >> weight);
				if (ok) request.graph.addEdge(u, v, weight);
			}
			else
			{
				error = "Line " + std::to_string(lineNumber) + ": unknown keyword '" + keyword + "'.";
				return false;
			}

			if (!ok)
			{
				error = "Line " + std::to_string(lineNumber) + ": could not read '" + keyword + "'.";
				return false;
			}
		}
		return true;
	}

	std::string validate(const Request& request)
	{
		const AlgorithmInfo* info = find(request.algorithm);
		if (!info)
		{
			return "Unknown algorithm '" + request.algorithm + "'.";
		}
		if (!info->usesGraph)
		{
			if (info->id == "queens" && (request.size < 1 || request.size > 10))
			{
				return "The board size must be between 1 and 10.";
			}
			return {};
		}
		const Graph& g = request.graph;
		const int n = g.size();
		if (n == 0)
		{
			return "The graph is empty. Add some vertices first.";
		}
		if (n > maxVertices)
		{
			return "The graph is too large to animate (limit: " + std::to_string(maxVertices) + " vertices).";
		}
		for (const Edge& e : g.edges)
		{
			if (e.u < 0 || e.u >= n || e.v < 0 || e.v >= n)
			{
				return "An edge refers to a vertex that does not exist.";
			}
			if (info->usesWeights && !(e.weight >= 0 && std::isfinite(e.weight)))
			{
				return "Edge " + g.nodes[e.u].label + " \xE2\x86\x92 " + g.nodes[e.v].label + " has a negative " + info->weightName
					+ ". " + info->name + " needs non-negative values.";
			}
		}
		if (info->source == Need::Required && (request.source < 0 || request.source >= n))
		{
			return "Choose a source vertex.";
		}
		if (info->target == Need::Required && (request.target < 0 || request.target >= n))
		{
			return "Choose a target vertex.";
		}
		if (request.target >= n)
		{
			return "The target vertex does not exist.";
		}
		if (info->category == "Network flow" && request.source == request.target)
		{
			return "The source and the target must be different vertices.";
		}
		if (info->id == "tsp")
		{
			if (g.directed) return "The travelling salesman problem here uses two-way roads. Untick \"Directed edges\".";
			if (n < 3) return "Add at least three cities.";
			if (n > maxCities) return "Use at most " + std::to_string(maxCities) + " cities: the exact search grows factorially.";
			if (request.method != "exact" && request.method != "nearest" && request.method != "2opt") return "Unknown TSP method.";
		}
		return {};
	}

	std::string run(const Request& original)
	{
		const std::string problem = validate(original);
		if (!problem.empty())
		{
			return errorJson(problem);
		}
		const AlgorithmInfo* info = find(original.algorithm);
		Request request = original;
		if (info->source == Need::No) request.source = -1;
		if (info->target == Need::No || request.target < 0) request.target = -1;

		Tracer tracer;
		info->run(request, tracer);
		return tracer.toJson(info->id);
	}

	std::string runFromText(const std::string& text)
	{
		Request request;
		std::string error;
		if (!parseRequest(text, request, error))
		{
			return errorJson(error);
		}
		return run(request);
	}
}
