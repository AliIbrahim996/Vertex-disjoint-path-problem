//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim
//!
//! The step recorder. An algorithm describes what it is doing through a Tracer (colour this
//! vertex, label that edge, show this queue) and takes a snapshot at each moment worth showing.
//! The finished list of snapshots is serialised to JSON, which the web page plays back.

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
		Idle,     //!< not touched yet
		Frontier, //!< discovered, waiting in the queue / stack / open set
		Current,  //!< the vertex being processed right now
		Visited,  //!< completely processed
		Path,     //!< part of the answer path
		Cycle,    //!< part of a detected cycle
		CutSide,  //!< on the source side of a minimum cut
		Path0, Path1, Path2, Path3, Path4, Path5, //!< one colour per path when several are shown
		Count     //!< number of states, not a state
	};

	//! How an edge is drawn in one step. Keep in sync with the name table in Trace.cpp.
	enum class EdgeState : int
	{
		Idle,        //!< not touched yet
		Considering, //!< being examined right now
		Tree,        //!< search-tree edge
		Rejected,    //!< examined and turned down
		Back,        //!< DFS back edge
		Cross,       //!< DFS forward / cross edge
		Path,        //!< part of the answer / augmenting path
		PathReverse, //!< augmenting path walks this edge backwards (cancels flow)
		Flow,        //!< carries flow
		Saturated,   //!< carries flow equal to its capacity
		Cycle,       //!< part of a detected cycle
		Cut,         //!< crosses the minimum cut
		Path0, Path1, Path2, Path3, Path4, Path5, //!< one colour per path when several are shown
		Count        //!< number of states, not a state
	};

	//! Vertex colour for the i-th of several paths shown together.
	//! @param[in] index Which path (0-based). Colours repeat after six paths.
	//! @returns one of NodeState::Path0 ... NodeState::Path5.
	NodeState nodePathColour(int index);

	//! Edge colour for the i-th of several paths shown together.
	//! @param[in] index Which path (0-based). Colours repeat after six paths.
	//! @returns one of EdgeState::Path0 ... EdgeState::Path5.
	EdgeState edgePathColour(int index);

	//! Records what an algorithm does as a list of drawable snapshots ("steps").
	//! The algorithms only talk to this class; the UI only reads its JSON.
	//!
	//! Typical use: set states and labels on the active view, update panels, then call step()
	//! with an explanation. Everything set before step() appears in that snapshot.
	class Tracer
	{
	public:
		//! Register a graph to draw. Most algorithms use one view; vertex-disjoint paths
		//! uses two (the original graph and the split flow network).
		//! The first view added becomes the active one.
		//! @param[in] title          Shown above the canvas when there is more than one view.
		//! @param[in] graph          The graph to draw (copied).
		//! @param[in] source         Vertex marked "S", or -1.
		//! @param[in] target         Vertex marked "T", or -1.
		//! @param[in] showEdgeLabels Draw edge weights when no other edge label is set.
		//! @param[in] layout         "graph" (vertices drawn at their positions) or "board" (an
		//!                           N x N grid of squares, vertex i at column x, row y).
		//! @returns the view index, for useView().
		int addView(const std::string& title, const Graph& graph, int source, int target, bool showEdgeLabels,
			const std::string& layout = "graph");

		//! Switch the view that the state setters below and the next steps refer to.
		//! Each view keeps its own states and labels.
		//! @param[in] view Index returned by addView().
		void useView(int view);

		//! Set how a vertex of the active view is drawn from now on.
		//! @param[in] id    Vertex index in the active view.
		//! @param[in] state The new state.
		void node(int id, NodeState state);

		//! Set how an edge of the active view is drawn from now on.
		//! @param[in] id    Edge index in the active view.
		//! @param[in] state The new state.
		void edge(int id, EdgeState state);

		//! @param[in] id Vertex index in the active view.
		//! @returns the vertex's current state.
		NodeState nodeState(int id) const;

		//! @param[in] id Edge index in the active view.
		//! @returns the edge's current state, for example to restore it after highlighting.
		EdgeState edgeState(int id) const;

		//! Set the text drawn next to a vertex (a distance, discovery time, ...).
		//! On a board view it is drawn inside the square instead.
		//! @param[in] id   Vertex index in the active view.
		//! @param[in] text The text, or "" for none.
		void nodeLabel(int id, const std::string& text);

		//! Set the text drawn on an edge instead of its weight (for example "3/7" flow/capacity).
		//! @param[in] id   Edge index in the active view.
		//! @param[in] text The text, or "" to fall back to the weight.
		void edgeLabel(int id, const std::string& text);

		//! Set every vertex and edge of the active view back to Idle (labels are kept).
		void resetStates();

		//! Show or replace a named list in the side panel (queue, stack, open set, ...).
		//! Panels stay until replaced, across steps and views.
		//! @param[in] title Panel heading; a second call with the same title replaces the list.
		//! @param[in] items The entries, in display order.
		void panel(const std::string& title, const std::vector<std::string>& items);

		//! Take a snapshot of the active view, its labels and the panels.
		//! Once the step limit is reached, snapshots are dropped unless `force` is set.
		//! @param[in] line    0-based pseudocode line to highlight, or -1 for none.
		//! @param[in] message Explanation shown under the canvas.
		//! @param[in] force   Record even past the step limit (use for the final steps).
		void step(int line, const std::string& message, bool force = false);

		//! Stop recording ordinary steps after `limit` of them. For searches that can take far
		//! more steps than anyone would watch; the algorithm still runs to the end.
		//! @param[in] limit Maximum number of steps to keep, or 0 for no limit.
		void setStepLimit(std::size_t limit) { stepLimit_ = limit; }

		//! @returns how many steps were dropped because of the step limit.
		std::size_t skippedSteps() const { return skipped_; }

		//! Whether a step() taken now would be kept. Once it is false, an algorithm can skip
		//! preparing a snapshot that would be dropped and call skipStep() instead of step().
		//! @returns false once the step limit has been reached.
		bool recording() const { return !stepLimit_ || steps_.size() < stepLimit_; }

		//! Count a step that was not prepared because recording() is false.
		void skipStep() { skipped_++; }

		//! Say what a vertex state means for this algorithm, shown in the legend instead of the
		//! default wording (for example "path" -> "Queen").
		//! @param[in] state Name of the state as in the JSON ("path", "visited", ...).
		//! @param[in] text  Legend text; a second call for the same state replaces it.
		void nodeLegend(const std::string& state, const std::string& text);

		//! Say what an edge state means for this algorithm, shown in the legend instead of the
		//! default wording (for example "cycle" -> "Road swapped out").
		//! @param[in] state Name of the state as in the JSON ("tree", "considering", ...).
		//! @param[in] text  Legend text; a second call for the same state replaces it.
		void edgeLegend(const std::string& state, const std::string& text);

		//! Set the pseudocode shown beside the canvas. step() highlights one of these lines.
		//! @param[in] lines One string per line; leading spaces show the nesting.
		void setPseudocode(const std::vector<std::string>& lines) { pseudocode_ = lines; }

		//! Set the one-paragraph outcome shown in the Result card after the last step.
		//! @param[in] summary The text.
		void setSummary(const std::string& summary) { summary_ = summary; }

		//! Record a machine-readable outcome (used by the tests and included in the JSON).
		//! @param[in] key   Name of the value ("cost", "path", ...); setting it again replaces it.
		//! @param[in] value The value, as text.
		void result(const std::string& key, const std::string& value);

		//! @param[in] key Name of the value.
		//! @returns the value recorded for `key`, or "" if there is none.
		std::string result(const std::string& key) const;

		//! @returns the number of steps recorded so far.
		std::size_t stepCount() const { return steps_.size(); }

		//! @returns the text set with setSummary().
		const std::string& summary() const { return summary_; }

		//! Serialise everything recorded: state names, pseudocode, summary, results, legend,
		//! views and steps. This is the format the web page plays back.
		//! @param[in] algorithmId Catalogue id of the algorithm that ran.
		//! @returns the trace as a JSON object.
		std::string toJson(const std::string& algorithmId) const;

	private:
		//! A graph being drawn and the current state of each of its parts.
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

		//! One snapshot: a full copy of the states and labels, so the UI can jump to any step.
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
	//! @param[in] text UTF-8 text.
	//! @returns the text in double quotes, with quotes, backslashes and control characters escaped.
	std::string jsonString(const std::string& text);
}
#endif // !ALGOLAB_TRACE_H
