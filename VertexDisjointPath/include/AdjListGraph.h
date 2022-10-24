//!
//! 
//! 
//! 

#ifndef VDP_ADJ_LIST_GRAPH_H
#define VDP_ADJ_LIST_GRAPH_H

#include <list>
#include <vector>
#include <memory>
#include <algorithm>

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
		const int getCapacity(int u, int v) const;

		//!
		void updateWeight(int s, int dest, int w);

		//!
		std::unique_ptr<AdjListGraph> createResidualGraph() const;

		//!
		Graph& graph() { return graph_; }

		//!
		const Graph& getGraph() const { return graph_; }

		//!
		const std::size_t getSize() const { return size_; }

		//!
		int getIndex(int u, int v) const;

		//!
		void setSize(const int size) { size_ = size; }

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