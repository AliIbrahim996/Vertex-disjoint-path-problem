//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim
//!
//! Maximum number of s-t paths that share no vertex other than s and t.
//!
//! Reduction to max-flow: a flow network limits what goes through an *edge*, not through a
//! *vertex*. So every vertex v (except s and t) is split into v_in -> v_out joined by an edge of
//! capacity 1, and every original edge (u, v) becomes u_out -> v_in with capacity 1. At most one
//! unit of flow can now cross any vertex, so an integral max-flow is a set of vertex-disjoint
//! paths, and its value is the answer (Menger's theorem).

#include <set>
#include <utility>

#include "../Common.h"
#include "algolab/Algorithms.h"

namespace algolab
{
	namespace
	{
		const char* const inSuffix = "\xE1\xB5\xA2";  // subscript i
		const char* const outSuffix = "\xE2\x82\x92"; // subscript o
		const double splitOffset = 24.0;

		//! The split network together with the way back to the original graph.
		struct SplitNetwork
		{
			Graph view;                    //!< what is drawn; edge i is link i of the flow network
			std::vector<int> inOf, outOf;  //!< original vertex -> split vertices
			std::vector<int> originalNode; //!< split vertex -> original vertex
			std::vector<int> originalEdge; //!< link -> original edge, or -1 for a v_in -> v_out link
		};

		SplitNetwork split(const Graph& g, int s, int t)
		{
			SplitNetwork net;
			net.view.directed = true;
			net.inOf.assign(g.size(), -1);
			net.outOf.assign(g.size(), -1);

			for (int v = 0; v < g.size(); v++)
			{
				const Node& node = g.nodes[v];
				if (v == s || v == t)
				{
					net.inOf[v] = net.outOf[v] = net.view.addNode(node.label, node.x, node.y);
					net.originalNode.push_back(v);
				}
				else
				{
					net.inOf[v] = net.view.addNode(node.label + inSuffix, node.x - splitOffset, node.y);
					net.outOf[v] = net.view.addNode(node.label + outSuffix, node.x + splitOffset, node.y);
					net.originalNode.push_back(v);
					net.originalNode.push_back(v);
				}
			}

			// One unit may pass through each inner vertex.
			for (int v = 0; v < g.size(); v++)
			{
				if (v != s && v != t)
				{
					net.view.addEdge(net.inOf[v], net.outOf[v], 1);
					net.originalEdge.push_back(-1);
				}
			}

			// Edges into s or out of t can never be part of an s-t path, so they are left out.
			// Parallel edges are merged: a second copy of u -> v is the same route, and for
			// s -> t it would otherwise be counted as an extra path.
			std::set<std::pair<int, int>> added;
			const auto addEdge = [&](int from, int to, int original)
			{
				if (to == s || from == t || from == to) return;
				if (!added.insert({ from, to }).second) return;
				net.view.addEdge(net.outOf[from], net.inOf[to], 1);
				net.originalEdge.push_back(original);
			};
			for (int i = 0; i < static_cast<int>(g.edges.size()); i++)
			{
				const Edge& e = g.edges[i];
				addEdge(e.u, e.v, i);
				if (!g.directed) addEdge(e.v, e.u, i);
			}
			return net;
		}
	}

	void runVertexDisjointPaths(const Request& request, Tracer& tr)
	{
		const Graph& g = request.graph;
		const int s = request.source;
		const int t = request.target;
		const FlowNetwork::Strategy strategy = FlowNetwork::strategyFromName(request.strategy);
		const auto name = [&](int v) { return g.nodes[v].label; };

		tr.setPseudocode({
			"VertexDisjointPaths(G, s, t):",
			"  split every vertex v \xE2\x89\xA0 s, t into v\xE1\xB5\xA2 \xE2\x86\x92 v\xE2\x82\x92 with capacity 1",
			"  turn every edge (u, v) into u\xE2\x82\x92 \xE2\x86\x92 v\xE1\xB5\xA2 with capacity 1",
			"  run max-flow from s to t on the new network:",
			"    while the residual graph has an s \xE2\x86\x92 t path P:",
			"      the bottleneck is 1 (every capacity is 1)",
			"      push 1 unit along P",
			"  max-flow value = number of vertex-disjoint paths",
			"  follow the flow out of s to read each path" });

		const int original = tr.addView("Original graph", g, s, t, false);
		const SplitNetwork net = split(g, s, t);
		const int network = tr.addView("Split flow network (all capacities 1)", net.view, net.inOf[s], net.inOf[t], false);

		tr.useView(original);
		tr.step(0, "Goal: as many paths from " + name(s) + " to " + name(t) + " as possible such that no two of them share a vertex (other than "
			+ name(s) + " and " + name(t) + "). Max-flow limits edges, not vertices, so the graph is rebuilt first.");

		tr.useView(network);
		for (int link = 0; link < static_cast<int>(net.view.edges.size()); link++)
		{
			if (net.originalEdge[link] < 0) tr.edge(link, EdgeState::Considering);
		}
		tr.step(1, "Each inner vertex v becomes an entrance v\xE1\xB5\xA2 and an exit v\xE2\x82\x92 joined by one edge of capacity 1 (highlighted). "
			"Only one unit of flow can ever get through a vertex.");
		for (int link = 0; link < static_cast<int>(net.view.edges.size()); link++)
		{
			tr.edge(link, net.originalEdge[link] < 0 ? EdgeState::Idle : EdgeState::Considering);
		}
		tr.step(2, "Each original edge (u, v) now leaves the exit u\xE2\x82\x92 and arrives at the entrance v\xE1\xB5\xA2, also with capacity 1.");
		tr.resetStates();

		FlowNetwork flow{ net.view.size() };
		for (const Edge& e : net.view.edges)
		{
			flow.addLink(e.u, e.v, 1.0);
		}
		tr.step(3, "Now run max-flow from " + name(s) + " to " + name(t) + " on this network. Every unit of flow will be one path.");

		const double total = traceAugmentingPaths(flow, net.inOf[s], net.inOf[t], strategy, tr, net.view, FlowLines{ 4, 5, 6 }, true);
		const int count = static_cast<int>(total + 0.5);

		tr.resetStates();
		showFlow(flow, tr, net.view, true);
		tr.step(7, "The maximum flow is " + std::to_string(count) + ". Because each vertex lets only one unit through, that is exactly the number of vertex-disjoint paths.");

		// Decompose the flow: leave s along an unused edge carrying flow and keep going to t.
		std::vector<char> used(net.view.edges.size(), 0);
		std::vector<std::vector<int>> pathVertices, pathEdges;
		for (int p = 0; p < count; p++)
		{
			std::vector<int> vertices{ s }, edges;
			int at = net.inOf[s];
			while (at != net.inOf[t])
			{
				int next = -1;
				for (const int a : flow.out(at))
				{
					if (a % 2 == 0 && !used[a / 2] && flow.linkFlow(a / 2) > 0.5)
					{
						next = a;
						break;
					}
				}
				if (next < 0) break; // cannot happen for a valid flow
				used[next / 2] = 1;
				at = flow.arc(next).to;
				if (net.originalEdge[next / 2] >= 0)
				{
					edges.push_back(net.originalEdge[next / 2]);
					vertices.push_back(net.originalNode[at]);
				}
			}
			pathVertices.push_back(vertices);
			pathEdges.push_back(edges);
		}

		tr.useView(original);
		std::vector<std::string> list;
		for (int p = 0; p < count; p++)
		{
			for (const int v : pathVertices[p])
			{
				if (v != s && v != t) tr.node(v, nodePathColour(p));
			}
			for (const int e : pathEdges[p]) tr.edge(e, edgePathColour(p));
			list.push_back(joinLabels(g, pathVertices[p]));
		}
		tr.panel("Vertex-disjoint paths", list);
		const std::string plural = count == 1 ? " path" : " paths";
		if (count == 0)
		{
			tr.step(8, "The flow is 0: " + name(t) + " cannot be reached from " + name(s) + " at all.");
		}
		else
		{
			tr.step(8, "Back on the original graph: merging each v\xE1\xB5\xA2/v\xE2\x82\x92 pair turns the flow into " + std::to_string(count)
				+ " vertex-disjoint" + plural + ", one colour each. By Menger's theorem no larger set exists.");
		}

		std::string text;
		for (std::size_t i = 0; i < list.size(); i++) text += (i ? "; " : "") + list[i];
		tr.setSummary(std::to_string(count) + " vertex-disjoint" + plural + " from " + name(s) + " to " + name(t) + (count ? ": " + text + "." : "."));
		tr.result("paths", std::to_string(count));
		tr.result("list", text);
	}
}
