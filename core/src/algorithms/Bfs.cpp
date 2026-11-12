//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim

#include <deque>

#include "../Common.h"
#include "algolab/Algorithms.h"

namespace algolab
{
	void runBfs(const Request& request, Tracer& tr)
	{
		const Graph& g = request.graph;
		const int n = g.size();
		const int s = request.source;
		const int t = request.target;
		const Adjacency adjacency = buildAdjacency(g);
		const auto name = [&](int v) { return g.nodes[v].label; };

		tr.setPseudocode({
			"BFS(G, s):",
			"  mark s discovered; dist[s] = 0; enqueue s",
			"  while the queue is not empty:",
			"    u = dequeue()",
			"    if u is the target: stop",
			"    for each neighbour v of u:",
			"      if v is not discovered:",
			"        dist[v] = dist[u] + 1; parent[v] = u; enqueue v",
			"  every reachable vertex has been visited",
			"  follow parent[] back from the target to read the path" });
		tr.addView("Graph", g, s, t, false);

		std::vector<int> dist(n, -1), parent(n, -1), parentEdge(n, -1);
		std::deque<int> queue;
		std::vector<std::string> order;
		const auto showQueue = [&]()
		{
			std::vector<std::string> items;
			for (const int v : queue) items.push_back(name(v));
			tr.panel("Queue (front \xE2\x86\x92 back)", items);
			tr.panel("Visit order", order);
		};

		dist[s] = 0;
		queue.push_back(s);
		tr.node(s, NodeState::Frontier);
		tr.nodeLabel(s, "d=0");
		showQueue();
		tr.step(1, "Start at " + name(s) + ": mark it discovered at distance 0 and put it in the queue.");

		bool found = false;
		while (!queue.empty())
		{
			const int u = queue.front();
			queue.pop_front();
			order.push_back(name(u));
			tr.node(u, NodeState::Current);
			showQueue();
			tr.step(3, "Dequeue " + name(u) + ", the vertex that has waited longest (distance " + std::to_string(dist[u]) + ").");

			if (u == t)
			{
				found = true;
				tr.step(4, name(u) + " is the target. BFS reaches vertices in order of distance, so this is a shortest path by number of edges.");
				break;
			}

			for (const Arc& arc : adjacency[u])
			{
				const int v = arc.to;
				if (!g.directed && arc.edge == parentEdge[u])
				{
					continue; // the edge we arrived by
				}
				const EdgeState before = tr.edgeState(arc.edge);
				tr.edge(arc.edge, EdgeState::Considering);
				if (dist[v] < 0)
				{
					dist[v] = dist[u] + 1;
					parent[v] = u;
					parentEdge[v] = arc.edge;
					queue.push_back(v);
					tr.node(v, NodeState::Frontier);
					tr.nodeLabel(v, "d=" + std::to_string(dist[v]));
					showQueue();
					tr.step(7, name(v) + " is new: distance " + std::to_string(dist[v]) + ", parent " + name(u) + ". Enqueue it.");
					tr.edge(arc.edge, EdgeState::Tree);
				}
				else
				{
					tr.step(6, name(v) + " was already discovered (distance " + std::to_string(dist[v]) + "), so skip it.");
					tr.edge(arc.edge, before);
				}
			}
			tr.node(u, NodeState::Visited);
		}

		tr.result("visited", std::to_string(order.size()));
		if (t >= 0 && found)
		{
			const std::vector<int> path = pathTo(t, parent);
			for (const int v : path)
			{
				tr.node(v, NodeState::Path);
				if (parentEdge[v] >= 0) tr.edge(parentEdge[v], EdgeState::Path);
			}
			const std::string text = joinLabels(g, path);
			tr.step(9, "Follow the parent pointers back from " + name(t) + ": " + text + " (" + std::to_string(dist[t]) + " edges).");
			tr.setSummary("Shortest path by edge count: " + text + " (" + std::to_string(dist[t]) + " edges). BFS visited " + std::to_string(order.size()) + " vertices.");
			tr.result("found", "true");
			tr.result("distance", std::to_string(dist[t]));
			tr.result("path", text);
		}
		else
		{
			showQueue();
			if (t >= 0)
			{
				tr.step(8, "The queue is empty and " + name(t) + " was never discovered: it is not reachable from " + name(s) + ".");
				tr.setSummary(name(t) + " is not reachable from " + name(s) + ".");
				tr.result("found", "false");
			}
			else
			{
				tr.step(8, "The queue is empty: every vertex reachable from " + name(s) + " has been visited. The highlighted edges form the BFS tree.");
				std::string text;
				for (std::size_t i = 0; i < order.size(); i++) text += (i ? ", " : "") + order[i];
				tr.setSummary("BFS order: " + text + ".");
			}
		}
	}
}
