//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim
//!
//! Travelling salesman on the drawn (undirected) graph: a tour may only use edges that exist.
//!   exact    depth-first branch and bound
//!   nearest  nearest-neighbour construction
//!   2opt     nearest neighbour, then 2-opt improvement

#include <algorithm>
#include <cmath>
#include <limits>

#include "../Common.h"
#include "algolab/Algorithms.h"

namespace algolab
{
	namespace
	{
		const double infinity = std::numeric_limits<double>::infinity();
		const double epsilon = 1e-9;
		const std::size_t stepLimit = 3000;

		//! Distance and edge id of the cheapest edge between every pair of cities.
		struct Roads
		{
			int n{};
			std::vector<std::vector<double>> w;
			std::vector<std::vector<int>> edge;
			std::vector<double> cheapest; //!< cheapest edge at each city (for the lower bound)

			explicit Roads(const Graph& g) : n(g.size()), w(n, std::vector<double>(n, infinity)), edge(n, std::vector<int>(n, -1)), cheapest(n, infinity)
			{
				for (int i = 0; i < static_cast<int>(g.edges.size()); i++)
				{
					const Edge& e = g.edges[i];
					if (e.u == e.v || e.weight >= w[e.u][e.v]) continue;
					w[e.u][e.v] = w[e.v][e.u] = e.weight;
					edge[e.u][e.v] = edge[e.v][e.u] = i;
				}
				for (int v = 0; v < n; v++)
				{
					for (int u = 0; u < n; u++) cheapest[v] = std::min(cheapest[v], w[v][u]);
				}
			}

			bool road(int a, int b) const { return !std::isinf(w[a][b]); }

			//! Cost so far + the cheapest edge out of every city that still has to be left
			//! (the last city of the path and each unvisited one). Never more than the real remainder.
			double bound(double cost, int last, const std::vector<char>& used) const
			{
				double total = cost + cheapest[last];
				for (int v = 0; v < n; v++)
				{
					if (!used[v]) total += cheapest[v];
				}
				return total;
			}
		};

		//! Plain branch and bound without tracing, to judge the heuristic tours.
		void bestTourCost(const Roads& roads, int start, std::vector<char>& used, int last, int count, double cost, double& best)
		{
			if (count == roads.n)
			{
				if (roads.road(last, start)) best = std::min(best, cost + roads.w[last][start]);
				return;
			}
			if (roads.bound(cost, last, used) >= best - epsilon) return;
			for (int v = 0; v < roads.n; v++)
			{
				if (used[v] || !roads.road(last, v)) continue;
				used[v] = 1;
				bestTourCost(roads, start, used, v, count + 1, cost + roads.w[last][v], best);
				used[v] = 0;
			}
		}

		double optimalCost(const Roads& roads, int start)
		{
			std::vector<char> used(roads.n, 0);
			used[start] = 1;
			double best = infinity;
			bestTourCost(roads, start, used, start, 1, 0.0, best);
			return best;
		}

		struct TspRun
		{
			const Graph& g;
			const Roads& roads;
			Tracer& tr;
			const int start;

			std::string name(int v) const { return g.nodes[v].label; }

			std::string tourText(const std::vector<int>& tour, bool closed) const
			{
				std::vector<int> walk = tour;
				if (closed) walk.push_back(tour.front());
				return joinLabels(g, walk);
			}

			//! Draw a path (or closed tour) and number the cities in visiting order.
			void draw(const std::vector<int>& tour, bool closed, EdgeState edgeState, NodeState nodeState)
			{
				tr.resetStates();
				for (int v = 0; v < roads.n; v++) tr.nodeLabel(v, "");
				for (std::size_t i = 0; i < tour.size(); i++)
				{
					tr.node(tour[i], nodeState);
					tr.nodeLabel(tour[i], "#" + std::to_string(i + 1));
					if (i) tr.edge(roads.edge[tour[i - 1]][tour[i]], edgeState);
				}
				if (closed && tour.size() > 2) tr.edge(roads.edge[tour.back()][tour.front()], edgeState);
			}
		};

		// ---- Exact: branch and bound -------------------------------------------------------------

		struct BranchAndBound : TspRun
		{
			std::vector<int> path;
			std::vector<char> used;
			double best{ infinity };
			std::vector<int> bestTour;
			long extensions{ 0 };
			long pruned{ 0 };

			BranchAndBound(const Graph& graph, const Roads& r, Tracer& tracer, int s)
				: TspRun{ graph, r, tracer, s }, path{ s }, used(r.n, 0)
			{
				used[s] = 1;
			}

			void panels(double cost)
			{
				tr.panel("Current path", { joinLabels(g, path) });
				tr.panel("Cost so far", { formatNumber(cost) });
				tr.panel("Best tour so far", { bestTour.empty() ? "none yet" : tourText(bestTour, true) + "  (" + formatNumber(best) + ")" });
				tr.panel("Branches pruned", { std::to_string(pruned) });
			}

			void search(double cost)
			{
				const int last = path.back();
				if (static_cast<int>(path.size()) == roads.n)
				{
					if (!roads.road(last, start))
					{
						if (!tr.recording())
						{
							tr.skipStep();
							return;
						}
						draw(path, false, EdgeState::Tree, NodeState::Visited);
						tr.node(last, NodeState::Current);
						panels(cost);
						tr.step(2, "Every city is visited, but there is no road from " + name(last) + " back to " + name(start) + ", so this is not a tour.");
						return;
					}
					const double total = cost + roads.w[last][start];
					if (total < best - epsilon)
					{
						best = total;
						bestTour = path;
						draw(path, true, EdgeState::Path, NodeState::Path);
						panels(total);
						tr.step(2, "Close the tour with " + name(last) + " \xE2\x86\x92 " + name(start) + ": total " + formatNumber(total) + ". That is the best tour so far.");
					}
					else if (!tr.recording())
					{
						tr.skipStep();
					}
					else
					{
						draw(path, true, EdgeState::Tree, NodeState::Visited);
						panels(total);
						tr.step(2, "Close the tour with " + name(last) + " \xE2\x86\x92 " + name(start) + ": total " + formatNumber(total) + ", not better than the best (" + formatNumber(best) + ").");
					}
					return;
				}

				const double bound = roads.bound(cost, last, used);
				if (bound >= best - epsilon)
				{
					pruned++;
					if (!tr.recording())
					{
						tr.skipStep();
						return;
					}
					draw(path, false, EdgeState::Tree, NodeState::Visited);
					tr.node(last, NodeState::Cycle);
					panels(cost);
					tr.step(4, "Any tour that starts like this costs at least " + formatNumber(cost) + " + " + formatNumber(bound - cost) + " = " + formatNumber(bound)
						+ " (the cost so far plus the cheapest road out of every city still to leave). The best tour costs " + formatNumber(best) + ", so prune this branch.");
					return;
				}

				for (int v = 0; v < roads.n; v++)
				{
					if (used[v] || !roads.road(last, v)) continue;
					const double next = cost + roads.w[last][v];
					used[v] = 1;
					path.push_back(v);
					extensions++;
					if (tr.recording())
					{
						draw(path, false, EdgeState::Tree, NodeState::Visited);
						tr.node(v, NodeState::Current);
						tr.edge(roads.edge[last][v], EdgeState::Considering);
						panels(next);
						tr.step(6, "Extend the path to " + name(v) + ": " + formatNumber(cost) + " + " + formatNumber(roads.w[last][v]) + " = " + formatNumber(next) + ".");
					}
					else
					{
						tr.skipStep();
					}
					search(next);
					path.pop_back();
					used[v] = 0;
				}
			}
		};

		void runExact(const Graph& g, const Roads& roads, Tracer& tr, int s)
		{
			tr.setPseudocode({
				"BranchAndBound(path, cost):",
				"  if the path visits every city:",
				"    close the tour back to the start; keep it if it beats the best",
				"  bound = cost + cheapest road out of each city still to leave",
				"  if bound \xE2\x89\xA5 best: prune (no tour from here can win)",
				"  for each unvisited neighbour v of the last city:",
				"    BranchAndBound(path + v, cost + w(last, v))",
				"the best tour found is optimal",
				"no tour visits every city exactly once" });
			tr.setStepLimit(stepLimit);

			BranchAndBound bb{ g, roads, tr, s };
			bb.draw(bb.path, false, EdgeState::Tree, NodeState::Current);
			bb.panels(0);
			tr.step(0, "Start at " + g.nodes[s].label + " and build paths one city at a time. Whenever a path provably cannot beat the best tour found so far, give up on it.");
			bb.search(0.0);

			const std::string note = tr.skippedSteps() ? " Only the first " + std::to_string(stepLimit) + " steps are animated; the search ran to the end." : "";
			tr.result("extensions", std::to_string(bb.extensions));
			tr.result("pruned", std::to_string(bb.pruned));
			if (bb.bestTour.empty())
			{
				tr.resetStates();
				tr.step(8, "Every branch has been explored and none visits every city and returns to the start. Try adding roads." + note, true);
				tr.setSummary("No tour exists on this graph.");
				tr.result("found", "false");
				return;
			}
			bb.draw(bb.bestTour, true, EdgeState::Path, NodeState::Path);
			bb.panels(bb.best);
			const std::string text = bb.tourText(bb.bestTour, true);
			tr.step(7, "Every branch is either explored or pruned, so the best tour is optimal: " + text + ", cost " + formatNumber(bb.best) + "." + note, true);
			tr.setSummary("Optimal tour: " + text + ", cost " + formatNumber(bb.best) + ". The search extended " + std::to_string(bb.extensions)
				+ " paths and pruned " + std::to_string(bb.pruned) + " branches." + note);
			tr.result("found", "true");
			tr.result("cost", formatNumber(bb.best));
			tr.result("tour", text);
			tr.result("optimal", "true");
		}

		// ---- Heuristics: nearest neighbour, then 2-opt ---------------------------------------------

		void runHeuristic(const Graph& g, const Roads& roads, Tracer& tr, int s, bool improve)
		{
			std::vector<std::string> code{
				"NearestNeighbour(start):",
				"  tour = [start]",
				"  while some city is unvisited:",
				"    go to the closest unvisited neighbour of the last city",
				"    if there is none: stuck, this method finds no tour",
				"  return to the start to close the tour" };
			if (improve)
			{
				code.insert(code.end(), {
					"repeat until nothing changes:          (2-opt)",
					"  for each pair of tour edges a\xE2\x80\x93" "b and c\xE2\x80\x93" "d:",
					"    if w(a,c) + w(b,d) < w(a,b) + w(c,d): reverse b \xE2\x80\xA6 c",
					"no swap shortens the tour: it is 2-optimal" });
			}
			tr.setPseudocode(code);

			TspRun run{ g, roads, tr, s };
			const int n = roads.n;
			std::vector<int> tour{ s };
			std::vector<char> used(n, 0);
			used[s] = 1;
			double cost = 0;
			const auto panels = [&]()
			{
				tr.panel("Tour so far", { joinLabels(g, tour) });
				tr.panel("Cost", { formatNumber(cost) });
			};

			run.draw(tour, false, EdgeState::Tree, NodeState::Current);
			panels();
			tr.step(1, "Start at " + run.name(s) + ". Always drive to the closest city not visited yet.");

			bool stuck = false;
			while (static_cast<int>(tour.size()) < n)
			{
				const int last = tour.back();
				int next = -1;
				std::string options;
				run.draw(tour, false, EdgeState::Tree, NodeState::Visited);
				tr.node(last, NodeState::Current);
				for (int v = 0; v < n; v++)
				{
					if (used[v] || !roads.road(last, v)) continue;
					tr.edge(roads.edge[last][v], EdgeState::Considering);
					tr.node(v, NodeState::Frontier);
					options += (options.empty() ? "" : ", ") + run.name(v) + " (" + formatNumber(roads.w[last][v]) + ")";
					if (next < 0 || roads.w[last][v] < roads.w[last][next] - epsilon) next = v;
				}
				panels();
				if (next < 0)
				{
					tr.step(4, "No road leads from " + run.name(last) + " to a city that has not been visited: nearest neighbour is stuck.");
					stuck = true;
					break;
				}
				tr.step(3, "Unvisited neighbours of " + run.name(last) + ": " + options + ". The closest is " + run.name(next) + ".");
				cost += roads.w[last][next];
				used[next] = 1;
				tour.push_back(next);
			}

			bool closed = false;
			if (!stuck)
			{
				const int last = tour.back();
				if (roads.road(last, s))
				{
					cost += roads.w[last][s];
					closed = true;
					run.draw(tour, true, EdgeState::Path, NodeState::Path);
					panels();
					tr.panel("Tour so far", { run.tourText(tour, true) });
					tr.step(5, "Every city is visited: drive back from " + run.name(last) + " to " + run.name(s) + ". Total cost " + formatNumber(cost) + ".");
				}
				else
				{
					run.draw(tour, false, EdgeState::Tree, NodeState::Visited);
					tr.node(last, NodeState::Cycle);
					panels();
					tr.step(5, "Every city is visited, but there is no road from " + run.name(last) + " back to " + run.name(s) + ".");
				}
			}

			if (closed && improve)
			{
				int swaps = 0;
				bool changed = true;
				while (changed)
				{
					changed = false;
					for (int i = 0; i + 2 < n; i++)
					{
						for (int j = i + 2; j < n; j++)
						{
							if (i == 0 && j == n - 1) continue; // these two edges share a city
							const int a = tour[i], b = tour[i + 1], c = tour[j], d = tour[(j + 1) % n];
							if (!roads.road(a, c) || !roads.road(b, d)) continue;
							const double before = roads.w[a][b] + roads.w[c][d];
							const double after = roads.w[a][c] + roads.w[b][d];
							if (after >= before - epsilon) continue;

							run.draw(tour, true, EdgeState::Path, NodeState::Path);
							tr.edge(roads.edge[a][b], EdgeState::Cycle);
							tr.edge(roads.edge[c][d], EdgeState::Cycle);
							tr.edge(roads.edge[a][c], EdgeState::Considering);
							tr.edge(roads.edge[b][d], EdgeState::Considering);
							tr.step(8, "Swap " + run.name(a) + "\xE2\x80\x93" + run.name(b) + " and " + run.name(c) + "\xE2\x80\x93" + run.name(d) + " (" + formatNumber(before)
								+ ") for " + run.name(a) + "\xE2\x80\x93" + run.name(c) + " and " + run.name(b) + "\xE2\x80\x93" + run.name(d) + " (" + formatNumber(after)
								+ "), saving " + formatNumber(before - after) + ". The cities between them are visited in reverse.");

							std::reverse(tour.begin() + i + 1, tour.begin() + j + 1);
							cost += after - before;
							swaps++;
							changed = true;
							run.draw(tour, true, EdgeState::Path, NodeState::Path);
							panels();
							tr.panel("Tour so far", { run.tourText(tour, true) });
							tr.step(6, "Tour after the swap costs " + formatNumber(cost) + ". Look for another improving swap.");
						}
					}
				}
				run.draw(tour, true, EdgeState::Path, NodeState::Path);
				tr.step(9, swaps ? "No swap of two edges makes the tour shorter any more: it is 2-optimal. That is a local optimum, not necessarily the best tour."
					: "No swap of two edges makes the nearest-neighbour tour shorter: it is already 2-optimal.");
				tr.result("swaps", std::to_string(swaps));
			}

			const std::string method = improve ? "Nearest neighbour + 2-opt" : "Nearest neighbour";
			if (!closed)
			{
				tr.setSummary(method + " found no tour. Branch and bound can tell whether one exists.");
				tr.result("found", "false");
				return;
			}
			const double optimum = optimalCost(roads, s);
			const bool optimal = cost <= optimum + 1e-6;
			const std::string text = run.tourText(tour, true);
			tr.setSummary(method + " tour: " + text + ", cost " + formatNumber(cost) + ". "
				+ (optimal ? "That happens to be optimal." : "The optimal tour costs " + formatNumber(optimum) + ", so this one is "
					+ formatNumber((cost / optimum - 1) * 100) + "% longer."));
			tr.result("found", "true");
			tr.result("cost", formatNumber(cost));
			tr.result("tour", text);
			tr.result("optimalCost", formatNumber(optimum));
			tr.result("optimal", optimal ? "true" : "false");
		}
	}

	void runTravellingSalesman(const Request& request, Tracer& tr)
	{
		const Graph& g = request.graph;
		const Roads roads{ g };
		tr.addView("Cities and roads", g, request.source, -1, true);
		tr.nodeLegend("path", "On the tour");
		tr.nodeLegend("visited", "On the current path");
		tr.nodeLegend("current", "Current city");
		tr.nodeLegend("frontier", "Candidate next city");
		tr.nodeLegend("cycle", request.method == "exact" ? "Branch pruned here" : "No road back to the start");
		tr.edgeLegend("path", "Tour");
		tr.edgeLegend("tree", "Path being built");
		tr.edgeLegend("considering", request.method == "2opt" ? "Road being tried / swapped in" : "Road being tried");
		tr.edgeLegend("cycle", "Road swapped out");
		if (request.method == "exact")
		{
			runExact(g, roads, tr, request.source);
		}
		else
		{
			runHeuristic(g, roads, tr, request.source, request.method == "2opt");
		}
	}
}
