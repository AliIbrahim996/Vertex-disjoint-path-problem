//! @copyright 2022 All rights reserved.
//! @author Ali Ibrahim
//! 

#ifndef VDP_VERTEX_DISJOINT_PATH_CREATOR_H
#define VDP_VERTEX_DISJOINT_PATH_CREATOR_H

#include "AdjListGraph.h"

namespace VDP
{
	namespace Algorithm
	{
		//! Create the required flow network from the input flow network following these steps
		//!  Split each node v in the graph into to nodes: vin and vout.
		//! 
		//!		1- For each node v, add an edge of capacity one from vin to vout.
		//!		2- Replace each other edge(u, v) in the graph with an edge from uout to vin of capacity 1.
		//!		3- Add in a new dedicated destination node t.
		//! For each of the target nodes v, add an edge from vin to t with capacity 1.
		//! Find a max - flow from sout to t.The value of the flow is the number of node - disjoint paths.
		//! The idea behind this construction is as follows.Any flow path from the start node s to the destination node t must have capacity one, 
		//! since all edges have capacity one.Since all capacities are integral, there exists an integral max - flow.
		//! No two flow paths can pass through the same intermediary node, because in passing through a node in the graph the flow path must cross the edge from vin to vout, and the capacity here has been restricted to one.
		//! Additionally, this flow path must arrive at t by ending at one of the three special nodes you've identified, then following the edge from that node to t.
		//! Thus each flow path represents a node-disjoint path from the source node s to one of the three destination nodes.
		//! Accordingly, computing a max-flow here corresponds to finding the maximum number of node-disjoint paths you can take from s to any of the three destinations.
		//! @param[in] graph The flow network which will be transformed.
		//! @param[in] src The source node.
		//! @param[in] dest The destenation node.
		//! @returns The corresponding flow notwork after replacing each other edge(u, v) in the graph with an edge from uout to vin of capacity 1.
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
