//!
//! 
//! 
//! 


#ifndef VDP_VERTEX_DISJOINT_PATH_CREATOR_H
#define VDP_VERTEX_DISJOINT_PATH_CREATOR_H

#include "AdjListGraph.h"

namespace VDP
{
	namespace Algorithm
	{
		std::unique_ptr<AdjListGraph> createVertexDisjointPathInput(const AdjListGraph& graph, int src, int dest) {
			std::unique_ptr<AdjListGraph> listGraph = std::make_unique<AdjListGraph>(2 * graph.getSize() - 2);
			int preSize = graph.getSize();
			for (int i = 0; i < graph.getSize(); i++)
			{
				for (Edge e : graph.getGraph()[i])
				{
					listGraph->graph()[i].emplace_back(Edge{ e.dest,1,e.capacity });
				}
			}
			int j = 0;
			int div = listGraph->getSize() - preSize;
			for (int i = 0; i < listGraph->getSize(); i++)
			{
				if (i != src && i != dest)
				{
					if (j < div) {
						for (Edge e : listGraph->graph()[i])
						{
							listGraph->graph()[preSize + j].emplace_back(Edge{ e.dest,1,e.capacity });
							e.weight = 0;
						}
						listGraph->graph()[i].emplace_back(Edge{ preSize + j,-1,0 });
						j++;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}
#endif // !VDP_VERTEX_DISJOINT_PATH_CREATOR_H
