//!
//! 
//! 
//! 

#ifndef VDP_ADJ_LIST_GRAPH_H
#define VDP_ADJ_LIST_GRAPH_H

#include <list>
#include <vector>
#include <memory>

#include "Edge.h"

namespace VDP
{
	using Graph = std::vector<std::list<Edge>>;

	class AdjListGraph
	{	
	public:
		//!
		AdjListGraph(int size);

		//!
		void addEdge(int u, int v, int w);

		//!
		void zeroFlow();

		//!
		int getCapacity(int u, int v);

		//!
		void updateWeight(int s, int dest, int w);

		//!
		std::unique_ptr<AdjListGraph> createResidualGraph();

		//!
		Graph& graph() { return graph_; }

	private:

		//!
		bool checkEdge(int u, int v);

		//!
		Graph graph_{};
		//!
		int size_{};
	};
}
#endif