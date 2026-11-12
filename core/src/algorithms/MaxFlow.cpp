//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim

#include <cmath>

#include "../Common.h"
#include "algolab/Algorithms.h"

namespace algolab
{
	namespace
	{
		const double epsilon = 1e-9;
	}

	void showFlow(const FlowNetwork& network, Tracer& tr, const Graph& view, bool unitCapacities)
	{
		for (int link = 0; link < network.linkCount(); link++)
		{
			const double flow = network.linkFlow(link);
			const double capacity = network.linkCapacity(link);
			const double amount = std::fabs(flow);
			tr.edge(link, amount < epsilon ? EdgeState::Idle : amount > capacity - epsilon ? EdgeState::Saturated : EdgeState::Flow);
			if (unitCapacities)
			{
				continue;
			}
			std::string label = formatNumber(amount) + "/" + formatNumber(capacity);
			if (network.undirected(link) && amount > epsilon)
			{
				// An undirected edge can carry flow either way: say which.
				const Edge& e = view.edges[link];
				label += " " + view.nodes[flow > 0 ? e.u : e.v].label + "\xE2\x86\x92" + view.nodes[flow > 0 ? e.v : e.u].label;
			}
			tr.edgeLabel(link, label);
		}
	}

	double traceAugmentingPaths(FlowNetwork& network, int source, int sink, FlowNetwork::Strategy strategy,
		Tracer& tr, const Graph& view, const FlowLines& lines, bool unitCapacities)
	{
		const auto name = [&](int v) { return view.nodes[v].label; };
		double total = 0;
		std::vector<std::string> history;
		const auto showPanels = [&]()
		{
			tr.panel("Flow value", { formatNumber(total) });
			tr.panel("Augmenting paths", history);
		};
		showPanels();

		while (true)
		{
			const FlowNetwork::Search search = network.findPath(source, sink, strategy);

			tr.resetStates();
			showFlow(network, tr, view, unitCapacities);
			for (int v = 0; v < network.vertexCount(); v++)
			{
				if (search.reached[v]) tr.node(v, NodeState::Visited);
			}
			if (!search.found)
			{
				tr.step(lines.search, "Search the residual graph again: only the shaded vertices can still be reached from "
					+ name(source) + ", and " + name(sink) + " is not one of them. No augmenting path is left.");
				break;
			}

			std::vector<int> vertices{ source };
			std::string capacities;
			bool cancels = false;
			for (const int a : search.path)
			{
				const bool backwards = (a % 2 == 1) && !network.undirected(a / 2);
				cancels = cancels || backwards;
				tr.edge(a / 2, backwards ? EdgeState::PathReverse : EdgeState::Path);
				vertices.push_back(network.arc(a).to);
				capacities += (capacities.empty() ? "" : ", ") + formatNumber(network.residual(a));
			}
			for (const int v : vertices) tr.node(v, NodeState::Path);
			const std::string text = joinLabels(view, vertices);
			tr.step(lines.search, "Using " + FlowNetwork::strategyTitle(strategy) + " on the residual graph: found the augmenting path " + text + "."
				+ (cancels ? " The dashed edge is walked backwards, which means some of its flow gets rerouted." : ""));

			const double amount = network.bottleneck(search.path);
			tr.step(lines.bottleneck, "Residual capacities along the path: " + capacities + ". The smallest one, " + formatNumber(amount) + ", is the bottleneck.");

			network.augment(search.path, amount);
			total += amount;
			history.push_back(text + "  (+" + formatNumber(amount) + ")");
			tr.resetStates();
			showFlow(network, tr, view, unitCapacities);
			showPanels();
			tr.step(lines.push, "Push " + formatNumber(amount) + " along the path. The total flow is now " + formatNumber(total) + ".");
		}
		return total;
	}

	void runMaxFlow(const Request& request, Tracer& tr)
	{
		const Graph& g = request.graph;
		const int s = request.source;
		const int t = request.target;
		const FlowNetwork::Strategy strategy = FlowNetwork::strategyFromName(request.strategy);
		const auto name = [&](int v) { return g.nodes[v].label; };

		const std::string finder = strategy == FlowNetwork::Strategy::Bfs ? "shortest path, BFS: Edmonds\xE2\x80\x93Karp"
			: strategy == FlowNetwork::Strategy::Dfs ? "any path, DFS: classic Ford\xE2\x80\x93" "Fulkerson" : "widest path";
		tr.setPseudocode({
			"MaxFlow(G, s, t):",
			"  flow(e) = 0 for every edge e",
			"  while the residual graph has an s \xE2\x86\x92 t path P:      (" + finder + ")",
			"    b = smallest residual capacity on P       (bottleneck)",
			"    push b along P: forward edges +b, backward edges \xE2\x88\x92" "b",
			"  no augmenting path is left: the flow is maximum",
			"  vertices still reachable from s give a minimum cut" });
		tr.addView("Flow network", g, s, t, true);

		FlowNetwork network{ g.size() };
		for (const Edge& e : g.edges)
		{
			network.addLink(e.u, e.v, e.weight, !g.directed);
		}

		showFlow(network, tr, g, false);
		tr.step(1, "Edge labels read flow/capacity. Start with zero flow everywhere.");

		const double total = traceAugmentingPaths(network, s, t, strategy, tr, g, FlowLines{ 2, 3, 4 }, false);

		tr.resetStates();
		showFlow(network, tr, g, false);
		tr.step(5, "No augmenting path exists, so the flow is maximum: " + formatNumber(total) + " units leave " + name(s) + " and arrive at " + name(t) + ".");

		// Max-flow min-cut: the saturated edges leaving the source side certify optimality.
		const std::vector<char> reachable = network.reachableFrom(s);
		std::string cutText;
		double cutCapacity = 0;
		for (int v = 0; v < g.size(); v++)
		{
			if (reachable[v]) tr.node(v, NodeState::CutSide);
		}
		for (int i = 0; i < static_cast<int>(g.edges.size()); i++)
		{
			const Edge& e = g.edges[i];
			const bool crosses = g.directed ? (reachable[e.u] && !reachable[e.v]) : (reachable[e.u] != reachable[e.v]);
			if (crosses)
			{
				tr.edge(i, EdgeState::Cut);
				cutCapacity += e.weight;
				const int from = reachable[e.u] ? e.u : e.v;
				const int to = reachable[e.u] ? e.v : e.u;
				cutText += (cutText.empty() ? "" : ", ") + name(from) + "\xE2\x86\x92" + name(to);
			}
		}
		tr.step(6, "The shaded vertices are still reachable from " + name(s) + " in the residual graph. The edges leaving that set ("
			+ (cutText.empty() ? "none" : cutText) + ") are all full, and their capacities add up to " + formatNumber(cutCapacity)
			+ ": a minimum cut equal to the maximum flow.");

		tr.setSummary("Maximum flow from " + name(s) + " to " + name(t) + " = " + formatNumber(total) + ". Minimum cut: "
			+ (cutText.empty() ? "none" : cutText) + " (capacity " + formatNumber(cutCapacity) + ").");
		tr.result("maxFlow", formatNumber(total));
		tr.result("minCut", formatNumber(cutCapacity));
		tr.result("cutEdges", cutText);
	}
}
