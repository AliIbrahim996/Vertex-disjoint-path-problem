//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim
//!
//! Small helpers shared by the algorithm implementations.

#ifndef ALGOLAB_COMMON_H
#define ALGOLAB_COMMON_H

#include <string>
#include <vector>

#include "algolab/FlowNetwork.h"
#include "algolab/Graph.h"
#include "algolab/Trace.h"

namespace algolab
{
	//! "A → B → C"
	inline std::string joinLabels(const Graph& graph, const std::vector<int>& vertices, const std::string& separator = " \xE2\x86\x92 ")
	{
		std::string text;
		for (std::size_t i = 0; i < vertices.size(); i++)
		{
			if (i) text += separator;
			text += graph.nodes[vertices[i]].label;
		}
		return text;
	}

	//! Walk parent pointers back from `target`; returns the vertices from the root to `target`.
	inline std::vector<int> pathTo(int target, const std::vector<int>& parent)
	{
		std::vector<int> reversed;
		for (int v = target; v != -1; v = parent[v])
		{
			reversed.push_back(v);
		}
		return { reversed.rbegin(), reversed.rend() };
	}

	//! Pseudocode lines the shared max-flow loop should highlight.
	struct FlowLines
	{
		int search{};
		int bottleneck{};
		int push{};
	};

	//! Run the augmenting-path loop on `network`, recording every iteration on the tracer's
	//! active view. Link `i` of the network must be edge `i` of that view.
	//! @param unitCapacities  all capacities are 1: don't draw "flow/capacity" labels.
	//! @returns the value of the maximum flow.
	double traceAugmentingPaths(FlowNetwork& network, int source, int sink, FlowNetwork::Strategy strategy,
		Tracer& tracer, const Graph& view, const FlowLines& lines, bool unitCapacities);

	//! Colour the edges of the active view by how much flow they carry and refresh their labels.
	void showFlow(const FlowNetwork& network, Tracer& tracer, const Graph& view, bool unitCapacities);
}
#endif // !ALGOLAB_COMMON_H
