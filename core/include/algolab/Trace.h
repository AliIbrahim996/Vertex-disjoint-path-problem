//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim

#ifndef ALGOLAB_TRACE_H
#define ALGOLAB_TRACE_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Graph.h"

namespace algolab
{
	//! How a vertex is drawn in one step. Keep in sync with the name table in Trace.cpp.
	enum class NodeState : int
	{
		Idle,
		Frontier, //!< discovered, waiting in the queue / stack / open set
		Current,  //!< the vertex being processed right now
		Visited,  //!< completely processed
		Path,     //!< part of the answer path
		Cycle,    //!< part of a detected cycle
		CutSide,  //!< on the source side of a minimum cut
		Path0, Path1, Path2, Path3, Path4, Path5,
		Count
	};

	//! How an edge is drawn in one step. Keep in sync with the name table in Trace.cpp.
	enum class EdgeState : int
	{
		Idle,
		Considering, //!< being examined right now
		Tree,        //!< search-tree edge
		Rejected,
		Back,        //!< DFS back edge
		Cross,       //!< DFS forward / cross edge
		Path,        //!< part of the answer / augmenting path
		PathReverse, //!< augmenting path walks this edge backwards (cancels flow)
		Flow,        //!< carries flow
		Saturated,   //!< carries flow equal to its capacity
		Cycle,
		Cut,         //!< crosses the minimum cut
		Path0, Path1, Path2, Path3, Path4, Path5,
		Count
	};

	//! Colour for the i-th of several paths shown together.
	NodeState nodePathColour(int index);
	EdgeState edgePathColour(int index);

	//! Records what an algorithm does as a list of drawable snapshots ("steps").
	//! The algorithms only talk to this class; the UI only reads its JSON.
	class Tracer
	{
	public:
		//! Register a graph to draw. Most algorithms use one view; vertex-disjoint paths
		//! uses two (the original graph and the split flow network).
		//! @returns the view index. The first view added becomes the active one.
		//! `layout` is "graph" (vertices drawn at their positions) or "board" (an N x N grid of squares,
		//! vertex i at column x, row y).
		int addView(const std::string& title, const Graph& graph, int source, int target, bool showEdgeLabels,
			const std::string& layout = "graph");

		//! Switch the view that the state setters below and the next steps refer to.
		void useView(int view);

		void node(int id, NodeState state);
		void edge(int id, EdgeState state);
		NodeState nodeState(int id) const;
		EdgeState edgeState(int id) const;

		//! Text drawn next to a vertex (a distance, discovery time, ...).
		void nodeLabel(int id, const std::string& text);
		//! Text drawn on an edge instead of its weight (for example "3/7" flow/capacity).
		void edgeLabel(int id, const std::string& text);

		//! Set every vertex and edge of the active view back to Idle (labels are kept).
		void resetStates();

		//! Show / replace a named list in the side panel (queue, stack, open set, ...).
		void panel(const std::string& title, const std::vector<std::string>& items);

		//! Take a snapshot. `line` is the 0-based pseudocode line to highlight (-1 for none).
		//! Once the step limit is reached, snapshots are dropped unless `force` is set.
		void step(int line, const std::string& message, bool force = false);

		//! Stop recording ordinary steps after `limit` of them (0 = no limit). For searches that can
		//! take far more steps than anyone would watch.
		void setStepLimit(std::size_t limit) { stepLimit_ = limit; }
		std::size_t skippedSteps() const { return skipped_; }

		//! false once the step limit is reached: an algorithm can then skip preparing a snapshot
		//! that would be dropped, and call skipStep() instead of step().
		bool recording() const { return !stepLimit_ || steps_.size() < stepLimit_; }
		void skipStep() { skipped_++; }

		//! What a state means for this algorithm, shown in the legend (for example the node state
		//! "path" -> "Queen"). Vertex and edge states are labelled separately.
		void nodeLegend(const std::string& state, const std::string& text);
		void edgeLegend(const std::string& state, const std::string& text);

		void setPseudocode(const std::vector<std::string>& lines) { pseudocode_ = lines; }
		void setSummary(const std::string& summary) { summary_ = summary; }

		//! Machine-readable outcome (used by the tests and shown in the Result card).
		void result(const std::string& key, const std::string& value);
		std::string result(const std::string& key) const;

		std::size_t stepCount() const { return steps_.size(); }
		const std::string& summary() const { return summary_; }

		std::string toJson(const std::string& algorithmId) const;

	private:
		struct View
		{
			std::string title;
			std::string layout;
			Graph graph;
			int source{ -1 };
			int target{ -1 };
			bool showEdgeLabels{ true };
			std::vector<int> nodeStates;
			std::vector<int> edgeStates;
			std::vector<std::string> nodeLabels;
			std::vector<std::string> edgeLabels;
		};

		struct Step
		{
			int view{};
			int line{ -1 };
			std::string message;
			std::vector<int> nodeStates;
			std::vector<int> edgeStates;
			std::vector<std::string> nodeLabels;
			std::vector<std::string> edgeLabels;
			std::vector<std::pair<std::string, std::vector<std::string>>> panels;
		};

		std::vector<View> views_;
		int active_{ 0 };
		std::vector<std::pair<std::string, std::vector<std::string>>> panels_;
		std::vector<Step> steps_;
		std::vector<std::string> pseudocode_;
		std::string summary_;
		std::vector<std::pair<std::string, std::string>> results_;
		std::vector<std::pair<std::string, std::string>> nodeLegend_;
		std::vector<std::pair<std::string, std::string>> edgeLegend_;
		std::size_t stepLimit_{ 0 };
		std::size_t skipped_{ 0 };
	};

	//! Quote and escape a string for JSON.
	std::string jsonString(const std::string& text);
}
#endif // !ALGOLAB_TRACE_H
