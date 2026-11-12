//! @copyright 2022-2026 Ali Ibrahim. Licensed under the MIT License (see LICENSE).
//! @author Ali Ibrahim
//!
//! Dijkstra, A* and greedy best-first search are the same loop with a different priority:
//!   Dijkstra  f(v) = g(v)            cost so far
//!   A*        f(v) = g(v) + h(v)     cost so far + estimated cost to go
//!   Greedy    f(v) = h(v)            estimated cost to go only

#include <cmath>
#include <limits>
#include <set>
#include <tuple>

#include "../Common.h"
#include "algolab/Algorithms.h"

namespace algolab
{
	namespace
	{
		enum class Mode { Dijkstra, AStar, Greedy };

		const double infinity = std::numeric_limits<double>::infinity();
		const double epsilon = 1e-9;

		//! Plain Dijkstra without tracing, used to judge the answers of the heuristic searches.
		std::vector<double> shortestDistances(const Graph& g, const Adjacency& adjacency, int from)
		{
			std::vector<double> dist(g.size(), infinity);
			std::set<std::pair<double, int>> open;
			dist[from] = 0;
			open.insert({ 0.0, from });
			while (!open.empty())
			{
				const int u = open.begin()->second;
				open.erase(open.begin());
				for (const Arc& arc : adjacency[u])
				{
					if (dist[u] + arc.weight < dist[arc.to] - epsilon)
					{
						open.erase({ dist[arc.to], arc.to });
						dist[arc.to] = dist[u] + arc.weight;
						open.insert({ dist[arc.to], arc.to });
					}
				}
			}
			return dist;
		}

		std::vector<double> heuristicValues(const Request& request)
		{
			const Graph& g = request.graph;
			std::vector<double> h(g.size(), 0.0);
			if (request.target < 0 || request.heuristic == "zero")
			{
				return h;
			}
			const Node& goal = g.nodes[request.target];
			const double scale = request.heuristicScale > 0 ? request.heuristicScale : 1.0;
			for (int v = 0; v < g.size(); v++)
			{
				const double dx = std::fabs(g.nodes[v].x - goal.x);
				const double dy = std::fabs(g.nodes[v].y - goal.y);
				const double distance = request.heuristic == "manhattan" ? dx + dy : std::sqrt(dx * dx + dy * dy);
				h[v] = std::round(distance / scale * 10.0) / 10.0;
			}
			return h;
		}

		std::string heuristicTitle(const std::string& id)
		{
			if (id == "manhattan") return "Manhattan distance";
			if (id == "zero") return "h = 0";
			return "straight-line distance";
		}

		std::vector<std::string> pseudocode(Mode mode)
		{
			switch (mode)
			{
			case Mode::Dijkstra:
				return {
					"Dijkstra(G, s):",
					"  dist[s] = 0, every other dist = \xE2\x88\x9E; put s in the priority queue",
					"  while the queue is not empty:",
					"    u = vertex in the queue with the smallest dist",
					"    if u is the target: stop",
					"    for each edge (u, v) with weight w:",
					"      if dist[u] + w < dist[v]:",
					"        dist[v] = dist[u] + w; parent[v] = u      (relax)",
					"  all reachable vertices have their final distance",
					"  follow parent[] back from the target to read the path" };
			case Mode::AStar:
				return {
					"A*(G, s, t, h):",
					"  g[s] = 0; f[s] = h(s); put s in the open set",
					"  while the open set is not empty:",
					"    u = open vertex with the smallest f = g + h",
					"    if u is the target: stop",
					"    for each edge (u, v) with weight w:",
					"      if g[u] + w < g[v]:",
					"        g[v] = g[u] + w; f[v] = g[v] + h(v); parent[v] = u; open v",
					"  the open set ran empty: the target is unreachable",
					"  follow parent[] back from the target to read the path" };
			default:
				return {
					"GreedyBestFirst(G, s, t, h):",
					"  put s in the open set, ordered by h alone",
					"  while the open set is not empty:",
					"    u = open vertex with the smallest h",
					"    if u is the target: stop",
					"    for each neighbour v of u:",
					"      if v has not been seen before:",
					"        parent[v] = u; open v",
					"  the open set ran empty: the target is unreachable",
					"  follow parent[] back from the target to read the path" };
			}
		}

		void runBestFirst(const Request& request, Tracer& tr, Mode mode)
		{
			const Graph& g = request.graph;
			const int n = g.size();
			const int s = request.source;
			const int t = request.target;
			const Adjacency adjacency = buildAdjacency(g);
			const std::vector<double> h = mode == Mode::Dijkstra ? std::vector<double>(n, 0.0) : heuristicValues(request);
			const auto name = [&](int v) { return g.nodes[v].label; };

			tr.setPseudocode(pseudocode(mode));
			tr.addView("Graph", g, s, t, true);

			std::vector<double> cost(n, infinity); // g(v): cheapest known cost from s
			std::vector<int> parent(n, -1), parentEdge(n, -1);
			std::vector<char> seen(n, 0);
			// (priority, tie-break, vertex). Ties prefer the vertex closer to the goal.
			std::set<std::tuple<double, double, int>> open;
			int expansions = 0;

			const auto priority = [&](int v)
			{
				switch (mode)
				{
				case Mode::Dijkstra: return cost[v];
				case Mode::AStar: return cost[v] + h[v];
				default: return h[v];
				}
			};
			const auto key = [&](int v) { return std::make_tuple(priority(v), h[v], v); };
			const auto label = [&](int v)
			{
				switch (mode)
				{
				case Mode::Dijkstra: tr.nodeLabel(v, "d=" + formatNumber(cost[v])); break;
				case Mode::AStar: tr.nodeLabel(v, formatNumber(cost[v]) + "+" + formatNumber(h[v]) + "=" + formatNumber(cost[v] + h[v])); break;
				default: tr.nodeLabel(v, "h=" + formatNumber(h[v])); break;
				}
			};
			const std::string openTitle = mode == Mode::Dijkstra ? "Priority queue (smallest dist first)"
				: mode == Mode::AStar ? "Open set (smallest f first)" : "Open set (smallest h first)";
			const auto showOpen = [&]()
			{
				std::vector<std::string> items;
				for (const auto& entry : open)
				{
					items.push_back(name(std::get<2>(entry)) + "  " + formatNumber(std::get<0>(entry)));
				}
				tr.panel(openTitle, items);
			};

			if (mode != Mode::Dijkstra)
			{
				for (int v = 0; v < n; v++) tr.nodeLabel(v, "h=" + formatNumber(h[v]));
			}
			else
			{
				for (int v = 0; v < n; v++) tr.nodeLabel(v, "d=" + formatNumber(infinity));
			}

			cost[s] = 0;
			seen[s] = 1;
			open.insert(key(s));
			tr.node(s, NodeState::Frontier);
			label(s);
			showOpen();
			switch (mode)
			{
			case Mode::Dijkstra:
				tr.step(1, "Every distance starts at \xE2\x88\x9E except the source " + name(s) + ", which is 0.");
				break;
			case Mode::AStar:
				tr.step(1, "Labels read g+h=f. h is the " + heuristicTitle(request.heuristic) + " to " + name(t) + ". Start with " + name(s) + ": g = 0, f = " + formatNumber(h[s]) + ".");
				break;
			default:
				tr.step(1, "h is the " + heuristicTitle(request.heuristic) + " to " + name(t) + ". Greedy search ignores the cost already paid and always moves to the vertex that looks closest.");
				break;
			}

			bool found = false;
			while (!open.empty())
			{
				const int u = std::get<2>(*open.begin());
				open.erase(open.begin());
				expansions++;
				tr.node(u, NodeState::Current);
				showOpen();
				switch (mode)
				{
				case Mode::Dijkstra:
					tr.step(3, "Take " + name(u) + ": it has the smallest tentative distance (" + formatNumber(cost[u]) + "), which is now final.");
					break;
				case Mode::AStar:
					tr.step(3, "Take " + name(u) + ": smallest f = " + formatNumber(cost[u]) + " + " + formatNumber(h[u]) + " = " + formatNumber(cost[u] + h[u]) + ".");
					break;
				default:
					tr.step(3, "Take " + name(u) + ": it looks closest to the target (h = " + formatNumber(h[u]) + ").");
					break;
				}

				if (u == t)
				{
					found = true;
					tr.step(4, name(u) + " is the target: stop.");
					break;
				}

				for (const Arc& arc : adjacency[u])
				{
					const int v = arc.to;
					if (!g.directed && arc.edge == parentEdge[u])
					{
						continue;
					}
					const EdgeState before = tr.edgeState(arc.edge);
					tr.edge(arc.edge, EdgeState::Considering);

					if (mode == Mode::Greedy)
					{
						if (!seen[v])
						{
							seen[v] = 1;
							cost[v] = cost[u] + arc.weight;
							parent[v] = u;
							parentEdge[v] = arc.edge;
							open.insert(key(v));
							tr.node(v, NodeState::Frontier);
							showOpen();
							tr.step(7, name(v) + " is new: add it to the open set with h = " + formatNumber(h[v]) + ".");
							tr.edge(arc.edge, EdgeState::Tree);
						}
						else
						{
							tr.step(6, name(v) + " has been seen already. Greedy search never reconsiders a vertex, even if this way is cheaper.");
							tr.edge(arc.edge, before);
						}
						continue;
					}

					const double candidate = cost[u] + arc.weight;
					const std::string sum = formatNumber(cost[u]) + " + " + formatNumber(arc.weight) + " = " + formatNumber(candidate);
					if (candidate < cost[v] - epsilon)
					{
						const bool reopened = seen[v] && tr.nodeState(v) == NodeState::Visited;
						open.erase(key(v));
						if (parentEdge[v] >= 0)
						{
							tr.edge(parentEdge[v], EdgeState::Idle); // the old tree edge is no longer the best way in
						}
						const std::string old = formatNumber(cost[v]);
						cost[v] = candidate;
						seen[v] = 1;
						parent[v] = u;
						parentEdge[v] = arc.edge;
						open.insert(key(v));
						tr.node(v, NodeState::Frontier);
						label(v);
						showOpen();
						tr.edge(arc.edge, EdgeState::Considering);
						tr.step(7, "Relax " + name(u) + " \xE2\x86\x92 " + name(v) + ": " + sum + " beats " + old + ", so "
							+ (mode == Mode::Dijkstra ? "dist[" : "g[") + name(v) + "] = " + formatNumber(candidate) + " via " + name(u) + "."
							+ (reopened ? " " + name(v) + " was already expanded, so it is reopened (the heuristic is not consistent)." : ""));
						tr.edge(arc.edge, EdgeState::Tree);
					}
					else
					{
						tr.step(6, "Going to " + name(v) + " through " + name(u) + " would cost " + sum + ", no better than " + formatNumber(cost[v]) + ": keep the old value.");
						tr.edge(arc.edge, before);
					}
				}
				tr.node(u, NodeState::Visited);
			}

			tr.result("expanded", std::to_string(expansions));
			const std::string algorithm = mode == Mode::Dijkstra ? "Dijkstra" : mode == Mode::AStar ? "A*" : "Greedy best-first";

			if (t < 0)
			{
				showOpen();
				tr.step(8, "The queue is empty: every reachable vertex has its final shortest distance, and the highlighted edges form the shortest-path tree.");
				tr.setSummary("Shortest distances from " + name(s) + " computed for every reachable vertex.");
				return;
			}
			if (!found)
			{
				showOpen();
				tr.step(8, "Nothing is left to explore and " + name(t) + " was never reached: it is not reachable from " + name(s) + ".");
				tr.setSummary(name(t) + " is not reachable from " + name(s) + ".");
				tr.result("found", "false");
				return;
			}

			const std::vector<int> path = pathTo(t, parent);
			double pathCost = 0;
			for (const int v : path)
			{
				tr.node(v, NodeState::Path);
				if (parentEdge[v] >= 0)
				{
					tr.edge(parentEdge[v], EdgeState::Path);
					pathCost += g.edges[parentEdge[v]].weight;
				}
			}
			const std::string text = joinLabels(g, path);
			const double optimal = shortestDistances(g, adjacency, s)[t];
			const bool isOptimal = pathCost <= optimal + 1e-6;

			std::string verdict;
			if (mode == Mode::Dijkstra)
			{
				verdict = "With non-negative weights Dijkstra's answer is always optimal.";
			}
			else
			{
				// A heuristic is admissible when it never overestimates the true remaining cost.
				const std::vector<double> toGoal = shortestDistances(g, buildReverseAdjacency(g), t);
				int offender = -1;
				for (int v = 0; v < n && offender < 0; v++)
				{
					if (!std::isinf(toGoal[v]) && h[v] > toGoal[v] + 1e-6) offender = v;
				}
				tr.result("admissible", offender < 0 ? "true" : "false");
				const std::string admissibility = offender < 0
					? "The heuristic never overestimates on this graph (it is admissible)."
					: "The heuristic overestimates at " + name(offender) + " (h = " + formatNumber(h[offender]) + ", true remaining cost " + formatNumber(toGoal[offender]) + "), so it is not admissible.";
				if (isOptimal)
				{
					verdict = "This path is optimal. " + admissibility;
				}
				else
				{
					verdict = "The optimal cost is " + formatNumber(optimal) + ", so this path is not the shortest. " + admissibility;
				}
				if (mode == Mode::Greedy)
				{
					verdict += " Greedy search gives no optimality guarantee either way.";
				}
			}

			tr.step(9, "Follow the parent pointers back from " + name(t) + ": " + text + ", total cost " + formatNumber(pathCost) + ". " + verdict);
			tr.setSummary(algorithm + " path: " + text + ", cost " + formatNumber(pathCost) + ", " + std::to_string(expansions) + " vertices expanded. " + verdict);
			tr.result("found", "true");
			tr.result("cost", formatNumber(pathCost));
			tr.result("optimalCost", formatNumber(optimal));
			tr.result("optimal", isOptimal ? "true" : "false");
			tr.result("path", text);
		}
	}

	void runDijkstra(const Request& request, Tracer& tracer) { runBestFirst(request, tracer, Mode::Dijkstra); }
	void runAStar(const Request& request, Tracer& tracer) { runBestFirst(request, tracer, Mode::AStar); }
	void runGreedyBestFirst(const Request& request, Tracer& tracer) { runBestFirst(request, tracer, Mode::Greedy); }
}
