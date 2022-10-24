//!
//! 
//! 

#ifndef VPD_ALGORITHM_WIDEST_PATH_WITH_LARGEST_KEY_H
#define VPD_ALGORITHM_WIDEST_PATH_WITH_LARGEST_KEY_H

#include <queue>

#include "AdjListGraph.h"

namespace VDP
{
	namespace Algorithm 
	{
		namespace
		{
			//!
			int findMaxKey(const std::vector<int>& keys, const bool* status, const std::size_t size)
			{
				int maxVertex = INT_MIN,maxX= INT_MIN;
				for (int i = 0; i < size; i++) 
				{
					if (keys[i] > maxX && !status[i]) 
					{
						maxX = keys[i];
						maxVertex = i;
					}
				}

				return maxVertex;
			}
		}

		//!
		bool findWidestPath(const int src, const int dest, std::vector<std::list<int>>& parent, const std::size_t size, const Graph& graph)
		{
			bool* status = new bool[size];
			std::vector<int> keys;
			keys.reserve(size);
			parent.reserve(size);
			for(int i=0;i<size;i++)
			{
				parent[i].emplace_back(-1);
				keys[i] = INT_MIN;
			}

			keys[src] = 0;
			int v = findMaxKey(keys, status,size);
			while(v!=INT_MIN)
			{
				status[v] = true;
				for(Edge e: graph[v])
				{
					if(e.weight!=0)
					{
						if(!status[e.dest] && (e.weight > keys[e.dest]))
						{
							keys[e.dest] = e.weight;
							parent[e.dest].pop_front();
							parent[e.dest].emplace_front(v);
						}
					}
				}
				v = findMaxKey(keys, status, size);
			}

			return status[dest];
		}
	}
}

#endif