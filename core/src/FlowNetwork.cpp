//! @copyright 2022-2026 Ali Ibrahim. Licensed under the MIT License (see LICENSE).
//! @author Ali Ibrahim

#include "algolab/FlowNetwork.h"

#include <algorithm>
#include <deque>
#include <limits>
#include <set>

namespace algolab
{
	namespace
	{
		const double epsilon = 1e-9;
	}

	FlowNetwork::FlowNetwork(int vertexCount) : out_(vertexCount) {}

	int FlowNetwork::addLink(int from, int to, double capacity, bool undirected)
	{
		const int id = static_cast<int>(arcs_.size());
		arcs_.push_back(Arc{ from, to, capacity, 0.0 });
		arcs_.push_back(Arc{ to, from, undirected ? capacity : 0.0, 0.0 });
		undirected_.push_back(undirected ? 1 : 0);

		const auto insertSorted = [this](int vertex, int arcId)
		{
			auto& list = out_[vertex];
			const auto position = std::upper_bound(list.begin(), list.end(), arcId,
				[this](int a, int b) { return arcs_[a].to < arcs_[b].to; });
			list.insert(position, arcId);
		};
		insertSorted(from, id);
		insertSorted(to, id + 1);
		return id / 2;
	}

	FlowNetwork::Search FlowNetwork::findPath(int source, int sink, Strategy strategy) const
	{
		const int n = vertexCount();
		Search search;
		search.reached.assign(n, 0);
		search.reached[source] = 1;
		std::vector<int> via(n, -1); // arc used to enter each vertex

		if (strategy == Strategy::Dfs)
		{
			search.found = dfs(source, sink, search.reached, search.path);
			std::reverse(search.path.begin(), search.path.end());
			return search;
		}

		if (strategy == Strategy::Bfs)
		{
			// Shortest augmenting path (Edmonds-Karp).
			std::deque<int> queue{ source };
			while (!queue.empty() && !search.reached[sink])
			{
				const int u = queue.front();
				queue.pop_front();
				for (const int a : out_[u])
				{
					const int v = arcs_[a].to;
					if (!search.reached[v] && residual(a) > epsilon)
					{
						search.reached[v] = 1;
						via[v] = a;
						queue.push_back(v);
					}
				}
			}
		}
		else
		{
			// Widest path: like Dijkstra, but a path is as good as its narrowest arc.
			std::vector<double> width(n, 0.0);
			std::vector<char> done(n, 0);
			std::set<std::pair<double, int>> open; // (-width, vertex)
			width[source] = std::numeric_limits<double>::infinity();
			open.insert({ -width[source], source });
			search.reached.assign(n, 0);
			while (!open.empty())
			{
				const int u = open.begin()->second;
				open.erase(open.begin());
				done[u] = 1;
				search.reached[u] = 1;
				if (u == sink) break;
				for (const int a : out_[u])
				{
					const int v = arcs_[a].to;
					const double candidate = std::min(width[u], residual(a));
					if (!done[v] && candidate > epsilon && candidate > width[v] + epsilon)
					{
						open.erase({ -width[v], v });
						width[v] = candidate;
						via[v] = a;
						open.insert({ -width[v], v });
					}
				}
			}
		}

		search.found = search.reached[sink] != 0;
		if (search.found)
		{
			for (int v = sink; v != source; v = arcs_[via[v]].from)
			{
				search.path.push_back(via[v]);
			}
			std::reverse(search.path.begin(), search.path.end());
		}
		return search;
	}

	bool FlowNetwork::dfs(int vertex, int sink, std::vector<char>& reached, std::vector<int>& path) const
	{
		if (vertex == sink)
		{
			return true;
		}
		for (const int a : out_[vertex])
		{
			const int v = arcs_[a].to;
			if (!reached[v] && residual(a) > epsilon)
			{
				reached[v] = 1;
				if (dfs(v, sink, reached, path))
				{
					path.push_back(a); // collected sink-first; the caller reverses it
					return true;
				}
			}
		}
		return false;
	}

	double FlowNetwork::bottleneck(const std::vector<int>& path) const
	{
		double smallest = std::numeric_limits<double>::infinity();
		for (const int a : path)
		{
			smallest = std::min(smallest, residual(a));
		}
		return smallest;
	}

	void FlowNetwork::augment(const std::vector<int>& path, double amount)
	{
		for (const int a : path)
		{
			arcs_[a].flow += amount;
			arcs_[a ^ 1].flow -= amount;
		}
	}

	std::vector<char> FlowNetwork::reachableFrom(int source) const
	{
		std::vector<char> reached(vertexCount(), 0);
		std::deque<int> queue{ source };
		reached[source] = 1;
		while (!queue.empty())
		{
			const int u = queue.front();
			queue.pop_front();
			for (const int a : out_[u])
			{
				const int v = arcs_[a].to;
				if (!reached[v] && residual(a) > epsilon)
				{
					reached[v] = 1;
					queue.push_back(v);
				}
			}
		}
		return reached;
	}

	FlowNetwork::Strategy FlowNetwork::strategyFromName(const std::string& name)
	{
		if (name == "dfs") return Strategy::Dfs;
		if (name == "widest") return Strategy::Widest;
		return Strategy::Bfs;
	}

	std::string FlowNetwork::strategyTitle(Strategy strategy)
	{
		switch (strategy)
		{
		case Strategy::Dfs: return "depth-first search";
		case Strategy::Widest: return "a widest-path search";
		default: return "breadth-first search";
		}
	}
}
