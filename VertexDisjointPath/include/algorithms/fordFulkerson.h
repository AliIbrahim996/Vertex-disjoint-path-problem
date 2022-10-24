//!
//! 
//! 


#ifndef VDP_FORD_FULKERSON_H
#define VDP_FORD_FULKERSON_H

#include "AdjListGraph.h"
#include "Algorithms.h"
#include "bfs.h"
#include "widestPath.h"
#include "widestPathWithLargestKey.h"

namespace VDP
{
	namespace Algorithm
	{
		namespace
		{
			template<typename Algo>
			bool findPath(const int src, const int dest, std::vector<std::list<int>>& parent, const std::size_t size, const Graph& graph)
			{
				switch (Alg)
				{
				case Algorithms::BFS:
					return runBFS(src, dest, parent, size, graph);
				case Algorithms::WIDEST_PATH:
					return widestPath(src, dest, parent, size, graph);
				case Algorithms::WIDEST_PATH_WITH_LARGEST_KEY:
					return findWidestPath(src, dest, parent, size, graph);
				default:
					return false;
				}
			}
		}

		void findMinCap(const int src, const int sink, std::vector<std::list<int>>& parent, int& min, AdjListGraph& rGraph)
		{
			int de = 0;
			for (int currNode{ sink }; currNode != src; currNode = de)
			{
				de = parent[currNode].front();
				min = std::min(min, rGraph.getCapacity(de, currNode));
			}
		};

		void updateFlow(const int src, const int sink, std::vector<std::list<int>>& parent, const int min, AdjListGraph& nGraph, AdjListGraph& rGraph)
		{
			int de = 0;
			for (int currNode{ sink }; currNode != src; currNode = de)
			{
				de = parent[currNode].front();
				int idx{ nGraph.getIndex(de,currNode) };

				auto iter1 = nGraph.graph().at(de).begin();
				auto iter2 = rGraph.graph().at(de).begin();

				std::advance(iter1, idx);
				std::advance(iter2, idx);

				auto& e1 = *iter1;
				auto& e2 = *iter2;
				if (e1.weight > 0)
				{
					e1.capacity += min;
					e2.dest -= min;
					rGraph.updateWeight(e2.dest, de, min);
				}
				else
				{
					e1.capacity -= min;
				}
			}
		};

		template<typename Algo>
		inline int runFordFulkerson(AdjListGraph& nGraph, AdjListGraph& rGraph, const int src, const int sink)
		{
			int maxFlow{ 0 };
			std::vector<std::list<int>>& parent;
			const std::size_t size{ nGraph.getSize() };
			parent.reserve(size);
			while (findPath<Algo>(src, sink, parent, size, rGraph.getGraph()))
			{
				int min = INT_MAX;

				findMinCap(src, sink, parent, min, rGraph);
				updateFlow(src, sink, parent, min, nGraph, rGraph);
				maxFlow += min;
			}
			return maxFlow;
		}
	}
}
#endif // !VDP_FORD_FULKERSON_H
