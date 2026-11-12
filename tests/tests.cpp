//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim
//!
//! Dependency-free checks of the algorithm results. Exit code 0 means everything passed.

#include <cmath>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

#include "algolab/Algorithms.h"

using namespace algolab;

namespace
{
	int failures = 0;
	int checks = 0;

	void expect(const std::string& what, const std::string& actual, const std::string& expected)
	{
		checks++;
		if (actual != expected)
		{
			failures++;
			std::printf("FAIL  %s: expected '%s', got '%s'\n", what.c_str(), expected.c_str(), actual.c_str());
		}
	}

	struct E { int u, v; double w; };

	//! Vertices are named A, B, C, ... and placed at the given positions (or on a line).
	Graph makeGraph(int n, bool directed, const std::vector<E>& edges, const std::vector<std::pair<double, double>>& positions = {})
	{
		Graph g;
		g.directed = directed;
		for (int i = 0; i < n; i++)
		{
			const double x = i < static_cast<int>(positions.size()) ? positions[i].first : i * 100.0;
			const double y = i < static_cast<int>(positions.size()) ? positions[i].second : 0.0;
			g.addNode(std::string(1, static_cast<char>('A' + i)), x, y);
		}
		for (const E& e : edges) g.addEdge(e.u, e.v, e.w);
		return g;
	}

	Tracer simulate(const std::string& algorithm, const Graph& g, int source, int target,
		const std::string& strategy = "bfs", const std::string& heuristic = "euclidean")
	{
		Request request;
		request.algorithm = algorithm;
		request.graph = g;
		request.source = source;
		request.target = target;
		request.strategy = strategy;
		request.heuristic = heuristic;
		request.heuristicScale = 100;
		expect(algorithm + " validates", validate(request), "");
		Tracer tracer;
		for (const AlgorithmInfo& info : catalog())
		{
			if (info.id == algorithm) info.run(request, tracer);
		}
		checks++;
		if (tracer.stepCount() == 0)
		{
			failures++;
			std::printf("FAIL  %s recorded no steps\n", algorithm.c_str());
		}
		return tracer;
	}

	void testTraversal()
	{
		// A - B - D - E, A - C - E: the shortest A..E route has 2 edges.
		const Graph g = makeGraph(5, false, { {0,1,1}, {1,3,1}, {3,4,1}, {0,2,1}, {2,4,1} });
		Tracer bfs = simulate("bfs", g, 0, 4);
		expect("bfs distance", bfs.result("distance"), "2");
		expect("bfs path", bfs.result("path"), "A \xE2\x86\x92 C \xE2\x86\x92 E");

		Tracer dfs = simulate("dfs", g, 0, 4);
		expect("dfs found", dfs.result("found"), "true");
		expect("dfs path", dfs.result("path"), "A \xE2\x86\x92 B \xE2\x86\x92 D \xE2\x86\x92 E");

		const Graph apart = makeGraph(3, true, { {0,1,1} });
		expect("bfs unreachable", simulate("bfs", apart, 0, 2).result("found"), "false");
		expect("dfs unreachable", simulate("dfs", apart, 0, 2).result("found"), "false");
		expect("bfs full traversal", simulate("bfs", g, 0, -1).result("visited"), "5");
		expect("dfs full traversal", simulate("dfs", g, 0, -1).result("visited"), "5");
	}

	void testCycles()
	{
		const Graph dag = makeGraph(4, true, { {0,1,1}, {0,2,1}, {1,3,1}, {2,3,1} });
		Tracer a = simulate("cycle", dag, -1, -1);
		expect("dag has no cycle", a.result("cycle"), "false");
		expect("dag topological order", a.result("topologicalOrder"), "A, C, B, D");

		const Graph loop = makeGraph(4, true, { {0,1,1}, {1,2,1}, {2,3,1}, {3,1,1} });
		Tracer b = simulate("cycle", loop, -1, -1);
		expect("directed cycle", b.result("cycle"), "true");
		expect("directed cycle length", b.result("length"), "3");

		// The same diamond is acyclic when directed but a 4-cycle when undirected.
		Tracer c = simulate("cycle", makeGraph(4, false, { {0,1,1}, {0,2,1}, {1,3,1}, {2,3,1} }), -1, -1);
		expect("undirected cycle", c.result("cycle"), "true");
		expect("undirected cycle length", c.result("length"), "4");

		expect("tree has no cycle", simulate("cycle", makeGraph(4, false, { {0,1,1}, {0,2,1}, {2,3,1} }), -1, -1).result("cycle"), "false");
	}

	void testShortestPaths()
	{
		// CLRS figure 24.6.
		const Graph g = makeGraph(5, true, {
			{0,1,10}, {0,2,5}, {1,2,2}, {1,3,1}, {2,1,3}, {2,3,9}, {2,4,2}, {3,4,4}, {4,3,6}, {4,0,7} });
		Tracer d = simulate("dijkstra", g, 0, 3);
		expect("dijkstra cost", d.result("cost"), "9");
		expect("dijkstra path", d.result("path"), "A \xE2\x86\x92 C \xE2\x86\x92 B \xE2\x86\x92 D");
		expect("dijkstra all targets", simulate("dijkstra", g, 0, -1).result("expanded"), "5");

		// A map where the road that heads straight for the goal ends in an expensive edge.
		//   A(0,0) --1-- B(100,0) --10-- D(200,0)        A --1.5-- C(100,100) --1.5-- D
		const Graph fixed = makeGraph(4, false, { {0,1,1}, {1,3,10}, {0,2,1.5}, {2,3,1.5} },
			{ {0,0}, {100,0}, {100,100}, {200,0} });

		Tracer astar = simulate("astar", fixed, 0, 3);
		expect("a* cost", astar.result("cost"), "3");
		expect("a* optimal", astar.result("optimal"), "true");
		expect("a* admissible", astar.result("admissible"), "true");

		Tracer greedy = simulate("greedy", fixed, 0, 3);
		expect("greedy takes the tempting road", greedy.result("path"), "A \xE2\x86\x92 B \xE2\x86\x92 D");
		expect("greedy cost", greedy.result("cost"), "11");
		expect("greedy not optimal", greedy.result("optimal"), "false");

		Tracer blind = simulate("astar", fixed, 0, 3, "bfs", "zero");
		expect("a* with h=0 equals dijkstra", blind.result("cost"), "3");

		// An overestimating heuristic (Manhattan on a diagonal shortcut) must be reported.
		const Graph diagonal = makeGraph(3, false, { {0,1,1.5}, {0,2,1}, {2,1,1} }, { {0,0}, {100,100}, {100,0} });
		expect("inadmissible heuristic is flagged", simulate("astar", diagonal, 0, 1, "bfs", "manhattan").result("admissible"), "false");
	}

	void testMaxFlow()
	{
		// CLRS figure 26.6: s=A, v1=B, v2=C, v3=D, v4=E, t=F. Maximum flow 23.
		const Graph g = makeGraph(6, true, {
			{0,1,16}, {0,2,13}, {1,3,12}, {2,1,4}, {2,4,14}, {3,2,9}, {3,5,20}, {4,3,7}, {4,5,4} });
		for (const std::string strategy : { "bfs", "dfs", "widest" })
		{
			Tracer f = simulate("maxflow", g, 0, 5, strategy);
			expect("max flow (" + strategy + ")", f.result("maxFlow"), "23");
			expect("min cut (" + strategy + ")", f.result("minCut"), "23");
		}

		// The textbook case where DFS must cancel flow on the middle edge.
		const Graph cancel = makeGraph(4, true, { {0,1,1}, {0,2,1}, {1,2,1}, {1,3,1}, {2,3,1} });
		expect("flow with cancellation", simulate("maxflow", cancel, 0, 3, "dfs").result("maxFlow"), "2");

		const Graph undirected = makeGraph(4, false, { {0,1,3}, {0,2,2}, {1,2,5}, {1,3,2}, {2,3,3} });
		Tracer u = simulate("maxflow", undirected, 0, 3);
		expect("undirected max flow", u.result("maxFlow"), "5");
		expect("undirected min cut", u.result("minCut"), "5");

		expect("no route", simulate("maxflow", makeGraph(3, true, { {0,1,4} }), 0, 2).result("maxFlow"), "0");
	}

	void testVertexDisjointPaths()
	{
		// Two edge-disjoint routes that both need C: only one vertex-disjoint path.
		const Graph bottleneck = makeGraph(6, true, { {0,1,1}, {0,2,1}, {1,2,1}, {2,3,1}, {2,4,1}, {3,5,1}, {4,5,1}, {1,3,1} });
		expect("paths through a cut vertex", simulate("vdp", makeGraph(5, true, { {0,1,1}, {0,2,1}, {1,3,1}, {2,3,1}, {3,4,1} }), 0, 4).result("paths"), "1");
		expect("two disjoint paths", simulate("vdp", bottleneck, 0, 5).result("paths"), "2");

		// K4 (undirected): direct edge plus one path through each of the two other vertices.
		const Graph k4 = makeGraph(4, false, { {0,1,1}, {0,2,1}, {0,3,1}, {1,2,1}, {1,3,1}, {2,3,1} });
		for (const std::string strategy : { "bfs", "dfs", "widest" })
		{
			expect("K4 (" + strategy + ")", simulate("vdp", k4, 0, 3, strategy).result("paths"), "3");
		}

		// A 3x3 grid, corner to corner: two paths (the corner has only two neighbours).
		std::vector<E> grid;
		for (int r = 0; r < 3; r++)
		{
			for (int c = 0; c < 3; c++)
			{
				if (c < 2) grid.push_back({ r * 3 + c, r * 3 + c + 1, 1 });
				if (r < 2) grid.push_back({ r * 3 + c, r * 3 + c + 3, 1 });
			}
		}
		expect("grid corner to corner", simulate("vdp", makeGraph(9, false, grid), 0, 8).result("paths"), "2");
		expect("unreachable target", simulate("vdp", makeGraph(3, true, { {0,1,1} }), 0, 2).result("paths"), "0");

		// Parallel copies of an edge are one route, not several.
		expect("parallel s-t edges", simulate("vdp", makeGraph(2, true, { {0,1,1}, {0,1,1} }), 0, 1).result("paths"), "1");
		expect("parallel inner edges", simulate("vdp", makeGraph(3, false, { {0,1,1}, {0,1,1}, {1,2,1}, {2,1,1} }), 0, 2).result("paths"), "1");
	}

	Tracer queens(int size)
	{
		Request request;
		request.algorithm = "queens";
		request.size = size;
		expect("queens validates", validate(request), "");
		Tracer tracer;
		runEightQueens(request, tracer);
		return tracer;
	}

	void testEightQueens()
	{
		// Row-by-row backtracking with columns tried left to right finds these first.
		expect("4 queens", queens(4).result("columns"), "2 4 1 3");
		Tracer eight = queens(8);
		expect("8 queens", eight.result("columns"), "1 5 8 6 3 7 2 4");
		expect("8 queens placements", eight.result("placements"), "876");
		expect("3 queens has no solution", queens(3).result("solved"), "false");
		expect("2 queens has no solution", queens(2).result("solved"), "false");
		expect("1 queen", queens(1).result("columns"), "1");

		// The puzzle needs no graph, but the board size is checked.
		expect("queens via text", runFromText("algorithm queens\nsize 5\n").find("\"solved\":\"true\"") != std::string::npos ? "ok" : "missing", "ok");
		expect("queens size checked", runFromText("algorithm queens\nsize 11\n").rfind("{\"error\"", 0) == 0 ? "error" : "ok", "error");
	}

	//! Complete graph on points of a line, weight = distance.
	Graph lineCities(const std::vector<double>& positions)
	{
		std::vector<E> edges;
		const int n = static_cast<int>(positions.size());
		for (int a = 0; a < n; a++)
		{
			for (int b = a + 1; b < n; b++) edges.push_back({ a, b, std::fabs(positions[a] - positions[b]) });
		}
		std::vector<std::pair<double, double>> points;
		for (const double p : positions) points.push_back({ p * 100 + 400, 200 });
		return makeGraph(n, false, edges, points);
	}

	void testTravellingSalesman()
	{
		// Cities at 0, 1, -2 and 4. Any optimal tour sweeps out and back: 2 * (4 - -2) = 12.
		// Nearest neighbour from A goes to B, then (tie at 3) to C, then D: 1 + 3 + 6 + 4 = 14.
		const Graph line = lineCities({ 0, 1, -2, 4 });
		const auto tsp = [](const Graph& g, const std::string& method)
		{
			Request request;
			request.algorithm = "tsp";
			request.graph = g;
			request.source = 0;
			request.method = method;
			expect("tsp validates (" + method + ")", validate(request), "");
			Tracer tracer;
			runTravellingSalesman(request, tracer);
			return tracer;
		};
		expect("tsp exact", tsp(line, "exact").result("cost"), "12");
		Tracer nearest = tsp(line, "nearest");
		expect("tsp nearest neighbour", nearest.result("cost"), "14");
		expect("tsp nearest not optimal", nearest.result("optimal"), "false");
		expect("tsp nearest knows the optimum", nearest.result("optimalCost"), "12");
		Tracer twoOpt = tsp(line, "2opt");
		expect("tsp 2-opt repairs it", twoOpt.result("cost"), "12");
		expect("tsp 2-opt swaps", twoOpt.result("swaps"), "1");

		// A square with both diagonals: the perimeter (4) beats any tour using a diagonal.
		const Graph square = makeGraph(4, false, { {0,1,1}, {1,2,1}, {2,3,1}, {3,0,1}, {0,2,1.5}, {1,3,1.5} });
		expect("tsp square", tsp(square, "exact").result("cost"), "4");

		// A star has no tour at all.
		const Graph star = makeGraph(4, false, { {0,1,1}, {0,2,1}, {0,3,1} });
		expect("tsp star exact", tsp(star, "exact").result("found"), "false");
		expect("tsp star nearest", tsp(star, "nearest").result("found"), "false");

		// Missing roads: tours may only use drawn edges. The only tour here is A-B-C-D-E-A (13).
		const Graph sparse = makeGraph(5, false, { {0,1,1}, {1,2,1}, {2,3,5}, {0,4,3}, {3,4,3}, {1,3,9} });
		expect("tsp sparse exact", tsp(sparse, "exact").result("cost"), "13");
		expect("tsp sparse nearest", tsp(sparse, "nearest").result("cost"), "13");

		// Nearest neighbour can paint itself into a corner: from A it takes the cheap road to B,
		// then C, and C's only other road leads back to A. Branch and bound finds A-C-B-D-A.
		const Graph corner = makeGraph(4, false, { {0,1,1}, {1,2,1}, {0,2,5}, {1,3,4}, {0,3,4} });
		expect("tsp corner nearest stuck", tsp(corner, "nearest").result("found"), "false");
		expect("tsp corner exact", tsp(corner, "exact").result("cost"), "14");

		// A 10-city instance runs to completion even though only part of it is animated.
		std::vector<double> ten;
		for (int i = 0; i < 10; i++) ten.push_back((i * 7) % 10);
		Tracer big = tsp(lineCities(ten), "exact");
		expect("tsp 10 cities", big.result("cost"), "18");

		// Every city has a free road to city A, so "cheapest road out" is 0 everywhere and the lower
		// bound is just the cost so far: it prunes very little and the search is long, mostly past
		// the step limit. It must still finish, find the optimum and report the skipped steps.
		// A tour uses exactly two free roads (into and out of A), so the optimum is the cheapest
		// path through B..J using roads of weight 10 + (a * b) % 7: 88 (checked by brute force).
		std::vector<E> weak;
		for (int a = 0; a < 10; a++)
		{
			for (int b = a + 1; b < 10; b++) weak.push_back({ a, b, a == 0 ? 0.0 : 10.0 + (a * b) % 7 });
		}
		Tracer flat = tsp(makeGraph(10, false, weak), "exact");
		expect("tsp weak bound", flat.result("cost"), "88");
		checks++;
		if (flat.stepCount() > 3002 || flat.skippedSteps() == 0)
		{
			failures++;
			std::printf("FAIL  tsp step limit: %zu steps kept, %zu skipped\n", flat.stepCount(), flat.skippedSteps());
		}

		const auto isError = [](const std::string& result) { return result.rfind("{\"error\"", 0) == 0 ? "error" : "ok"; };
		expect("tsp rejects directed", isError(runFromText("algorithm tsp\ndirected 1\nsource 0\nnode 0 0 A\nnode 1 0 B\nnode 2 0 C\n")), "error");
		expect("tsp needs three cities", isError(runFromText("algorithm tsp\nsource 0\nnode 0 0 A\nnode 1 0 B\nedge 0 1 1\n")), "error");
	}

	void testRequests()
	{
		const std::string text =
			"algorithm dijkstra\ndirected 0\nsource 0\ntarget 2\n"
			"node 0 0 A\nnode 100 0 B\nnode 200 0 C\n"
			"edge 0 1 2\nedge 1 2 3\n";
		const std::string json = runFromText(text);
		checks++;
		if (json.find("\"cost\":\"5\"") == std::string::npos)
		{
			failures++;
			std::printf("FAIL  text request did not produce cost 5\n");
		}

		const auto isError = [](const std::string& result) { return result.rfind("{\"error\"", 0) == 0 ? "error" : "ok"; };
		expect("unknown algorithm", isError(runFromText("algorithm nope\nnode 0 0 A\n")), "error");
		expect("missing source", isError(runFromText("algorithm bfs\nnode 0 0 A\n")), "error");
		expect("bad edge", isError(runFromText("algorithm cycle\nnode 0 0 A\nedge 0 7\n")), "error");
		expect("negative weight", isError(runFromText("algorithm dijkstra\nsource 0\nnode 0 0 A\nnode 1 1 B\nedge 0 1 -2\n")), "error");
		expect("same source and sink", isError(runFromText("algorithm maxflow\nsource 0\ntarget 0\nnode 0 0 A\n")), "error");
		expect("garbage", isError(runFromText("hello world\n")), "error");
		expect("bad edge weight", isError(runFromText("algorithm maxflow\nsource 0\ntarget 1\nnode 0 0 A\nnode 1 1 B\nedge 0 1 abc\n")), "error");
		expect("missing weight defaults to 1", isError(runFromText("algorithm maxflow\nsource 0\ntarget 1\nnode 0 0 A\nnode 1 1 B\nedge 0 1   \n")), "ok");

		// The largest graph the core accepts, shaped to make every recursive search go 200 deep,
		// sent as text the way the browser sends it.
		std::string chain = "directed 1\nsource 0\ntarget 199\n";
		for (int i = 0; i < 200; i++) chain += "node " + std::to_string(i) + " 0 Vertex" + std::to_string(i) + "\n";
		for (int i = 0; i < 199; i++) chain += "edge " + std::to_string(i) + " " + std::to_string(i + 1) + " 1\n";
		chain += "edge 199 0 1\n";
		for (const std::string algorithm : { "dfs", "cycle", "maxflow", "vdp" })
		{
			const std::string extra = algorithm == "maxflow" || algorithm == "vdp" ? "strategy dfs\n" : "";
			expect("200-vertex chain (" + algorithm + ")", isError(runFromText("algorithm " + algorithm + "\n" + extra + chain)), "ok");
		}
		expect("catalog size", std::to_string(catalog().size()), "10");
	}
}

int main()
{
	testTraversal();
	testCycles();
	testShortestPaths();
	testMaxFlow();
	testVertexDisjointPaths();
	testEightQueens();
	testTravellingSalesman();
	testRequests();
	std::printf("%d checks, %d failed\n", checks, failures);
	return failures == 0 ? 0 : 1;
}
