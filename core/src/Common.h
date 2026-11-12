//! @copyright 2022-2026 Ali Ibrahim. Licensed under the MIT License (see LICENSE).
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
	//! Join vertex labels for explanations, for example "A → B → C".
	//! @param[in] graph     Graph the vertex indices belong to.
	//! @param[in] vertices  Vertex indices, in order.
	//! @param[in] separator Text placed between labels (an arrow by default).
	//! @returns the joined labels.
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

	//! Walk parent pointers back from `target` to the root of a search tree.
	//! @param[in] target The last vertex of the path.
	//! @param[in] parent Parent of each vertex, -1 for the root.
	//! @returns the vertices from the root to `target`.
	inline std::vector<int> pathTo(int target, const std::vector<int>& parent)
	{
		std::vector<int> reversed;
		for (int v = target; v != -1; v = parent[v])
		{
			reversed.push_back(v);
		}
		return { reversed.rbegin(), reversed.rend() };
	}

	//! Pseudocode lines the shared max-flow loop should highlight. Max-flow and vertex-disjoint
	//! paths have different pseudocode but share traceAugmentingPaths().
	struct FlowLines
	{
		int search{};     //!< "find an augmenting path"
		int bottleneck{}; //!< "take the smallest residual capacity"
		int push{};       //!< "push flow along the path"
	};

	//! Run the augmenting-path loop on `network`, recording every iteration on the tracer's
	//! active view. Link `i` of the network must be edge `i` of that view.
	//! @param[in,out] network        The flow network; holds the maximum flow afterwards.
	//! @param[in]     source         Where flow starts.
	//! @param[in]     sink           Where flow ends.
	//! @param[in]     strategy       How augmenting paths are chosen.
	//! @param[in,out] tracer         Receives the steps.
	//! @param[in]     view           The graph drawn in the active view (for vertex names).
	//! @param[in]     lines          Pseudocode lines to highlight.
	//! @param[in]     unitCapacities All capacities are 1: don't draw "flow/capacity" labels.
	//! @returns the value of the maximum flow.
	double traceAugmentingPaths(FlowNetwork& network, int source, int sink, FlowNetwork::Strategy strategy,
		Tracer& tracer, const Graph& view, const FlowLines& lines, bool unitCapacities);

	//! Colour the edges of the active view by how much flow they carry (none, some, full) and
	//! refresh their "flow/capacity" labels.
	//! @param[in]     network        The flow network. Link `i` must be edge `i` of the view.
	//! @param[in,out] tracer         The tracer whose active view is updated.
	//! @param[in]     view           The graph drawn in the active view.
	//! @param[in]     unitCapacities All capacities are 1: colour the edges but leave their labels alone.
	void showFlow(const FlowNetwork& network, Tracer& tracer, const Graph& view, bool unitCapacities);
}
#endif // !ALGOLAB_COMMON_H
