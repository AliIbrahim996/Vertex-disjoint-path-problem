//!
//! 
//! 

#ifndef VPD_ALGORITHM_BFS_H
#define VPD_ALGORITHM_BFS_H

#include <queue>

#include "AdjListGraph.h"

namespace VDP
{
	namespace Algorithm
	{
		//!
		bool runBFS(const int src,const int dest, std::vector<std::list<int>>& parent, const std::size_t size,const Graph& graph)
		{
			 std::queue<int> queue{};
		     bool* status= new bool[size];
			 parent.reserve(size);
			 status[src] = true;
			 parent[src].push_back(-1);
			 queue.push(src);

			 while (!queue.empty()) 
			 {
				 int next = queue.front();
				 queue.pop();
				 for(Edge e: graph[next])
				 {
					 if(!status[e.dest])
					 {
						 if(e.weight!=0)
						 {
							 status[e.dest] = true;
							 parent[e.dest].emplace_back(next);
							 queue.emplace(e.dest);
						 }
					 }
				 }
			 }
			 return status[dest];
		}
	}
}
#endif // !VPD_ALGORITHM_BFS_H