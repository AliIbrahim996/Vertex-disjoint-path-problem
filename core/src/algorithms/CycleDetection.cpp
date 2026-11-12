//! @copyright 2022-2026 Ali Ibrahim. Licensed under the MIT License (see LICENSE).
//! @author Ali Ibrahim

#include <algorithm>

#include "../Common.h"
#include "algolab/Algorithms.h"

namespace algolab
{
	namespace
	{
		enum Colour { White, Grey, Black };

		struct CycleRun
		{
			const Graph& g;
			Tracer& tr;
			Adjacency adjacency;
			std::vector<int> colour, parent, parentEdge;
			std::vector<int> stack;
			std::vector<int> finishOrder;
			std::vector<int> cycle; //!< vertices of the cycle, in order, once found

			CycleRun(const Graph& graph, Tracer& tracer)
				: g(graph), tr(tracer), adjacency(buildAdjacency(graph)),
				colour(graph.size(), White), parent(graph.size(), -1), parentEdge(graph.size(), -1)
			{
			}

			std::string name(int v) const { return g.nodes[v].label; }

			void showStack()
			{
				std::vector<std::string> items;
				for (auto it = stack.rbegin(); it != stack.rend(); ++it) items.push_back(name(*it));
				tr.panel("Current path (top first)", items);
			}

			//! The closing edge u -> v was found while v is on the current path.
			void closeCycle(int u, int v, int closingEdge)
			{
				std::vector<int> reversed;
				tr.edge(closingEdge, EdgeState::Cycle);
				for (int w = u; ; w = parent[w])
				{
					reversed.push_back(w);
					tr.node(w, NodeState::Cycle);
					if (w == v) break;
					tr.edge(parentEdge[w], EdgeState::Cycle);
				}
				cycle.assign(reversed.rbegin(), reversed.rend());
			}

			//! @returns true when a cycle was found.
			bool visit(int u)
			{
				colour[u] = Grey;
				stack.push_back(u);
				tr.node(u, NodeState::Current);
				showStack();
				tr.step(2, "Enter " + name(u) + " and colour it grey: it is now on the current path.");

				for (const Arc& arc : adjacency[u])
				{
					const int v = arc.to;
					if (!g.directed && arc.edge == parentEdge[u])
					{
						continue; // walking straight back along the same edge is not a cycle
					}
					const EdgeState before = tr.edgeState(arc.edge);
					tr.edge(arc.edge, EdgeState::Considering);
					if (colour[v] == Grey)
					{
						closeCycle(u, v, arc.edge);
						tr.step(4, name(v) + " is grey, so it is already on the current path: the edge " + name(u) + " \xE2\x86\x92 " + name(v) + " closes a cycle.");
						return true;
					}
					if (colour[v] == White)
					{
						tr.step(5, name(v) + " is white (unvisited): continue the path through it.");
						tr.edge(arc.edge, EdgeState::Tree);
						tr.node(u, NodeState::Frontier);
						parent[v] = u;
						parentEdge[v] = arc.edge;
						if (visit(v))
						{
							return true;
						}
						tr.node(u, NodeState::Current);
					}
					else
					{
						tr.step(3, name(v) + " is black (finished): nothing reachable from it leads back to the current path, so this edge is harmless.");
						tr.edge(arc.edge, before == EdgeState::Idle ? EdgeState::Cross : before);
					}
				}

				colour[u] = Black;
				stack.pop_back();
				finishOrder.push_back(u);
				tr.node(u, NodeState::Visited);
				showStack();
				tr.step(6, "Every edge out of " + name(u) + " is checked: colour it black and backtrack.");
				return false;
			}
		};
	}

	void runCycleDetection(const Request& request, Tracer& tr)
	{
		const Graph& g = request.graph;
		const std::string edgeText = g.directed ? "u \xE2\x86\x92 v" : "{u, v} except the one we arrived by";

		tr.setPseudocode({
			"for each vertex s: if s is white: VISIT(s)",
			"VISIT(u):",
			"  colour u grey            (on the current path)",
			"  for each edge " + edgeText + ":",
			"    if v is grey: cycle found, stop",
			"    if v is white: parent[v] = u; VISIT(v)",
			"  colour u black           (finished)",
			"no edge ever reached a grey vertex: the graph is acyclic" });
		tr.addView("Graph", g, -1, -1, false);

		CycleRun run{ g, tr };
		bool found = false;
		for (int s = 0; s < g.size() && !found; s++)
		{
			if (run.colour[s] != White) continue;
			tr.step(0, "Start a new search from " + g.nodes[s].label + ", which has not been visited yet.");
			found = run.visit(s);
		}

		if (found)
		{
			std::vector<int> loop = run.cycle;
			loop.push_back(run.cycle.front());
			const std::string text = joinLabels(g, loop, g.directed ? " \xE2\x86\x92 " : " \xE2\x80\x94 ");
			tr.setSummary("Cycle found: " + text + ".");
			tr.result("cycle", "true");
			tr.result("length", std::to_string(run.cycle.size()));
			tr.result("vertices", text);
			return;
		}

		tr.result("cycle", "false");
		if (g.directed)
		{
			std::vector<int> topological(run.finishOrder.rbegin(), run.finishOrder.rend());
			const std::string text = joinLabels(g, topological, ", ");
			tr.step(7, "All vertices are black and no back edge appeared: the graph is a DAG. Reversing the finish order gives a topological order: " + text + ".");
			tr.setSummary("No cycle: the graph is a DAG. One topological order: " + text + ".");
			tr.result("topologicalOrder", text);
		}
		else
		{
			tr.step(7, "All vertices are black and no edge led back to a visited vertex: the graph is a forest (no cycles).");
			tr.setSummary("No cycle: the graph is a forest.");
		}
	}
}
