//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim

#include "../Common.h"
#include "algolab/Algorithms.h"

namespace algolab
{
	namespace
	{
		struct DfsRun
		{
			const Graph& g;
			const int target;
			Tracer& tr;
			Adjacency adjacency;
			std::vector<int> discovered, finished, parent, parentEdge;
			std::vector<int> stack;
			std::vector<std::string> order;
			int time{ 0 };

			DfsRun(const Graph& graph, int target, Tracer& tracer)
				: g(graph), target(target), tr(tracer), adjacency(buildAdjacency(graph)),
				discovered(graph.size(), 0), finished(graph.size(), 0), parent(graph.size(), -1), parentEdge(graph.size(), -1)
			{
			}

			std::string name(int v) const { return g.nodes[v].label; }

			void showStack()
			{
				std::vector<std::string> items;
				for (auto it = stack.rbegin(); it != stack.rend(); ++it) items.push_back(name(*it));
				tr.panel("Call stack (top first)", items);
				tr.panel("Discovery order", order);
			}

			//! @returns true when the target was reached and the search should unwind.
			bool visit(int u)
			{
				discovered[u] = ++time;
				stack.push_back(u);
				order.push_back(name(u));
				tr.node(u, NodeState::Current);
				tr.nodeLabel(u, std::to_string(discovered[u]) + "/");
				showStack();
				tr.step(1, "Visit " + name(u) + ": discovered at time " + std::to_string(discovered[u]) + ".");
				if (u == target)
				{
					return true;
				}

				for (const Arc& arc : adjacency[u])
				{
					const int v = arc.to;
					const EdgeState before = tr.edgeState(arc.edge);
					if (!g.directed && (arc.edge == parentEdge[u] || before == EdgeState::Back || before == EdgeState::Tree))
					{
						continue; // already classified from the other endpoint
					}
					tr.edge(arc.edge, EdgeState::Considering);
					if (!discovered[v])
					{
						tr.step(3, name(v) + " has not been discovered: go deeper along " + name(u) + " \xE2\x86\x92 " + name(v) + " (tree edge).");
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
					else if (!finished[v])
					{
						tr.step(5, name(v) + " is still on the call stack, so " + name(u) + " \xE2\x86\x92 " + name(v) + " is a back edge: it closes a cycle.");
						tr.edge(arc.edge, EdgeState::Back);
					}
					else
					{
						const bool forward = discovered[u] < discovered[v];
						tr.step(5, name(v) + " is already finished: " + name(u) + " \xE2\x86\x92 " + name(v) + " is a " + (forward ? "forward edge (to a descendant)." : "cross edge (to another branch)."));
						tr.edge(arc.edge, EdgeState::Cross);
					}
				}

				finished[u] = ++time;
				stack.pop_back();
				tr.node(u, NodeState::Visited);
				tr.nodeLabel(u, std::to_string(discovered[u]) + "/" + std::to_string(finished[u]));
				showStack();
				tr.step(6, "All neighbours of " + name(u) + " are explored: finish it at time " + std::to_string(finished[u]) + " and backtrack.");
				return false;
			}
		};
	}

	void runDfs(const Request& request, Tracer& tr)
	{
		const Graph& g = request.graph;
		const int s = request.source;
		const int t = request.target;

		tr.setPseudocode({
			"DFS(u):",
			"  mark u discovered; d[u] = ++time",
			"  for each neighbour v of u:",
			"    if v is not discovered:",
			"      parent[v] = u; DFS(v)",
			"    else: classify the edge (back / forward / cross)",
			"  mark u finished; f[u] = ++time",
			"DFS(s) has returned: everything reachable is visited",
			"the call stack is the path from s to the target" });
		tr.addView("Graph", g, s, t, false);

		DfsRun run{ g, t, tr };
		const bool found = run.visit(s);

		tr.result("visited", std::to_string(run.order.size()));
		if (found)
		{
			for (const int v : run.stack)
			{
				tr.node(v, NodeState::Path);
				if (run.parentEdge[v] >= 0) tr.edge(run.parentEdge[v], EdgeState::Path);
			}
			const std::string text = joinLabels(g, run.stack);
			tr.step(8, "Reached the target " + g.nodes[t].label + ". The call stack is a path from the source: " + text + ". DFS does not promise the shortest one.");
			tr.setSummary("DFS found the path " + text + " (" + std::to_string(run.stack.size() - 1) + " edges, not necessarily shortest) after discovering " + std::to_string(run.order.size()) + " vertices.");
			tr.result("found", "true");
			tr.result("path", text);
		}
		else if (t >= 0)
		{
			tr.step(7, "DFS has returned without meeting " + g.nodes[t].label + ": it is not reachable from " + g.nodes[s].label + ".");
			tr.setSummary(g.nodes[t].label + " is not reachable from " + g.nodes[s].label + ".");
			tr.result("found", "false");
		}
		else
		{
			tr.step(7, "DFS has returned to the start: every reachable vertex is finished. Labels show discovery/finish times.");
			std::string text;
			for (std::size_t i = 0; i < run.order.size(); i++) text += (i ? ", " : "") + run.order[i];
			tr.setSummary("DFS discovery order: " + text + ".");
		}
	}
}
