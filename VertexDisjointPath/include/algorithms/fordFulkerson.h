//! @copyright 2022 All rights reserved.
//! @author Ali Ibrahim
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
			//! Find the augmenting path from source to sink in the coressponding graph.
			//! @tparam Algo The algorithm used in to find the path.
			//! @param[in] src The source node.
			//! @param[in] dest The destination node.
			//! @param[in,out] parent The parent list the should be used to store the path.
			//! @param[in] size The graph size.
			//! @parma[in] graph The graph to search in.
			//! @returns true if path is found , false otherwise.
			template <typename Algo>
			bool findPath(const int src, const int dest, std::vector<std::list<int>> &parent, const std::size_t size, const Graph &graph)
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

			//! Update the weight and capacity of edges in the resduial network.
			//! @param[in,out] e1 The first edge.
			//! @param[in,out] e1 The second edge.
			//! @param[in,out] rGraph The resduial network.
			//! @param[in] min The minimu weight in the augmenting path.
			//! @param[in] de The destination node.
			void updateWeight(Edge &e1, Edge &e2, AdjListGraph &rGraph, const int min, const int de)
			{
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

			//! Find the minimum capacity in the augmenting path.
			//! @param[in] src The source node.
			//! @param[in] sink The destination node.
			//! @param[in, out] parent The parent list that would be used to retrive the path.
			//! @parm[out] min The minimum capacity in the augmenting path.
			//! @param[in] rGraph The residual network.
			void findMinCap(const int src, const int sink, const std::vector<std::list<int>>& parent, int& min, const AdjListGraph& rGraph)
			{
				int de = 0;
				for (int currNode{ sink }; currNode != src; currNode = de)
				{
					de = parent[currNode].front();
					min = std::min(min, rGraph.getCapacity(de, currNode));
				}
			}

			//!
			//! 
			//! 
			//! 
			template <bool solveVDP>
			void updateFlow(const int src, const int sink, std::vector<std::list<int>>& parent, const int min, AdjListGraph& nGraph, AdjListGraph& rGraph)
			{
				int de = 0;
				for (int currNode{ sink }; currNode != src; currNode = de)
				{
					de = parent[currNode].front();
					int idx{ nGraph.getIndex(de, currNode) };

					auto iter1 = nGraph.graph().at(de).begin();
					auto iter2 = rGraph.graph().at(de).begin();

					std::advance(iter1, idx);
					std::advance(iter2, idx);

					auto& e1 = *iter1;
					auto& e2 = *iter2;
					if (solveVDP)
					{
						if (e1.weight != -1)
						{
							updateWeight(e1, e2, rGraph, min, de);
						}
					}
					else
					{
						updateWeight(e1, e2, rGraph, min, de);
					}
				}
			}
		}


		//!
		//! 
		//! 
		//! 
		template <typename Algo, bool solveVDP>
		inline int runFordFulkerson(AdjListGraph &nGraph, AdjListGraph &rGraph, const int src, const int sink)
		{
			int maxFlow{0};
			std::vector<std::list<int>> &parent;
			const std::size_t size{nGraph.getSize()};
			parent.reserve(size);
			while (findPath<Algo>(src, sink, parent, size, rGraph.getGraph()))
			{
				int min = INT_MAX;

				findMinCap(src, sink, parent, min, rGraph);
				if (solveVDP)
				{
					if (min == -1)
					{
						min *= -1;
					}
				}
				updateFlow<solveVDP>(src, sink, parent, min, nGraph, rGraph);
				maxFlow += min;
			}
			return maxFlow;
		}

	}
}
#endif // !VDP_FORD_FULKERSON_H
