//!
//! 
//! 
//! 

#ifndef VPD_ALGORITHM_WIDEST_PATH_H
#define VPD_ALGORITHM_WIDEST_PATH_H

#include "AdjListGraph.h"

namespace VDP
{
	namespace Algorithm
	{
		bool widestPath(const int src, const int dest, std::vector<std::list<int>>& parent, const std::size_t size, const Graph& graph)
		{
			bool* status = new bool[size];
			std::list<int> s{};
			std::list<int> wT{};
			parent.reserve(size);

			for(int i=0;i<size;i++)
			{
				parent[i].emplace_back(-1);
			}

			status[src] = true;
			s.emplace_back(src);
			int vertex = 0;
			while (wT.size() < size)
			{
				int max = INT_MAX, z = 0;
				for(int ver:s)
				{
					for(Edge e: graph[ver])
					{
						if(!status[e.dest] && (e.weight!=0)&& (max < e.weight))
						{
							max = e.weight;
							vertex = ver;
							z = e.dest;
						}
					}
				}
				wT.emplace_back(max);
				if(std::find(s.begin(),s.end(),z) == s.end())
				{
					s.emplace_back(z);
					status[z] = true;
					parent[z].pop_front();
					parent[z].emplace_front(vertex);
				}
			}

			return status[dest];
		}
	}
}
#endif