//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim

#include "algolab/Trace.h"

#include <cstdio>

namespace algolab
{
	namespace
	{
		const char* const nodeStateNames[] = {
			"idle", "frontier", "current", "visited", "path", "cycle", "cutside",
			"path0", "path1", "path2", "path3", "path4", "path5" };

		const char* const edgeStateNames[] = {
			"idle", "considering", "tree", "rejected", "back", "cross", "path", "pathreverse",
			"flow", "saturated", "cycle", "cut",
			"path0", "path1", "path2", "path3", "path4", "path5" };

		static_assert(sizeof(nodeStateNames) / sizeof(nodeStateNames[0]) == static_cast<int>(NodeState::Count),
			"node state names out of sync");
		static_assert(sizeof(edgeStateNames) / sizeof(edgeStateNames[0]) == static_cast<int>(EdgeState::Count),
			"edge state names out of sync");

		const int pathColours = 6;

		void appendInts(std::string& out, const std::vector<int>& values)
		{
			out += '[';
			for (std::size_t i = 0; i < values.size(); i++)
			{
				if (i) out += ',';
				out += std::to_string(values[i]);
			}
			out += ']';
		}

		void appendStrings(std::string& out, const std::vector<std::string>& values)
		{
			out += '[';
			for (std::size_t i = 0; i < values.size(); i++)
			{
				if (i) out += ',';
				out += jsonString(values[i]);
			}
			out += ']';
		}

		//! Set `key` to `value`, replacing an earlier entry with the same key.
		void upsert(std::vector<std::pair<std::string, std::string>>& entries, const std::string& key, const std::string& value)
		{
			for (auto& entry : entries)
			{
				if (entry.first == key)
				{
					entry.second = value;
					return;
				}
			}
			entries.emplace_back(key, value);
		}

		void appendObject(std::string& out, const std::vector<std::pair<std::string, std::string>>& entries)
		{
			out += '{';
			for (std::size_t i = 0; i < entries.size(); i++)
			{
				if (i) out += ',';
				out += jsonString(entries[i].first) + ":" + jsonString(entries[i].second);
			}
			out += '}';
		}

		std::string jsonNumber(double value)
		{
			char buffer[48];
			std::snprintf(buffer, sizeof(buffer), "%.6g", value);
			return buffer;
		}
	}

	NodeState nodePathColour(int index)
	{
		return static_cast<NodeState>(static_cast<int>(NodeState::Path0) + index % pathColours);
	}

	EdgeState edgePathColour(int index)
	{
		return static_cast<EdgeState>(static_cast<int>(EdgeState::Path0) + index % pathColours);
	}

	std::string jsonString(const std::string& text)
	{
		std::string out{ "\"" };
		for (const char c : text)
		{
			switch (c)
			{
			case '"': out += "\\\""; break;
			case '\\': out += "\\\\"; break;
			case '\n': out += "\\n"; break;
			case '\r': out += "\\r"; break;
			case '\t': out += "\\t"; break;
			default:
				if (static_cast<unsigned char>(c) < 0x20)
				{
					char buffer[8];
					std::snprintf(buffer, sizeof(buffer), "\\u%04x", c);
					out += buffer;
				}
				else
				{
					out += c;
				}
			}
		}
		out += '"';
		return out;
	}

	int Tracer::addView(const std::string& title, const Graph& graph, int source, int target, bool showEdgeLabels,
		const std::string& layout)
	{
		View view;
		view.title = title;
		view.layout = layout;
		view.graph = graph;
		view.source = source;
		view.target = target;
		view.showEdgeLabels = showEdgeLabels;
		view.nodeStates.assign(graph.nodes.size(), 0);
		view.edgeStates.assign(graph.edges.size(), 0);
		view.nodeLabels.assign(graph.nodes.size(), "");
		view.edgeLabels.assign(graph.edges.size(), "");
		views_.push_back(std::move(view));
		return static_cast<int>(views_.size()) - 1;
	}

	void Tracer::useView(int view) { active_ = view; }

	void Tracer::node(int id, NodeState state) { views_[active_].nodeStates[id] = static_cast<int>(state); }
	void Tracer::edge(int id, EdgeState state) { views_[active_].edgeStates[id] = static_cast<int>(state); }
	NodeState Tracer::nodeState(int id) const { return static_cast<NodeState>(views_[active_].nodeStates[id]); }
	EdgeState Tracer::edgeState(int id) const { return static_cast<EdgeState>(views_[active_].edgeStates[id]); }
	void Tracer::nodeLabel(int id, const std::string& text) { views_[active_].nodeLabels[id] = text; }
	void Tracer::edgeLabel(int id, const std::string& text) { views_[active_].edgeLabels[id] = text; }

	void Tracer::resetStates()
	{
		View& view = views_[active_];
		view.nodeStates.assign(view.nodeStates.size(), 0);
		view.edgeStates.assign(view.edgeStates.size(), 0);
	}

	void Tracer::panel(const std::string& title, const std::vector<std::string>& items)
	{
		for (auto& existing : panels_)
		{
			if (existing.first == title)
			{
				existing.second = items;
				return;
			}
		}
		panels_.emplace_back(title, items);
	}

	void Tracer::nodeLegend(const std::string& state, const std::string& text) { upsert(nodeLegend_, state, text); }
	void Tracer::edgeLegend(const std::string& state, const std::string& text) { upsert(edgeLegend_, state, text); }

	void Tracer::step(int line, const std::string& message, bool force)
	{
		if (!force && stepLimit_ && steps_.size() >= stepLimit_)
		{
			skipped_++;
			return;
		}
		const View& view = views_[active_];
		Step step;
		step.view = active_;
		step.line = line;
		step.message = message;
		step.nodeStates = view.nodeStates;
		step.edgeStates = view.edgeStates;
		step.nodeLabels = view.nodeLabels;
		step.edgeLabels = view.edgeLabels;
		step.panels = panels_;
		steps_.push_back(std::move(step));
	}

	void Tracer::result(const std::string& key, const std::string& value) { upsert(results_, key, value); }

	std::string Tracer::result(const std::string& key) const
	{
		for (const auto& existing : results_)
		{
			if (existing.first == key)
			{
				return existing.second;
			}
		}
		return {};
	}

	std::string Tracer::toJson(const std::string& algorithmId) const
	{
		std::string out;
		out.reserve(4096 + steps_.size() * 512);
		out += "{\"algorithm\":" + jsonString(algorithmId);

		out += ",\"nodeStates\":[";
		for (int i = 0; i < static_cast<int>(NodeState::Count); i++)
		{
			if (i) out += ',';
			out += jsonString(nodeStateNames[i]);
		}
		out += "],\"edgeStates\":[";
		for (int i = 0; i < static_cast<int>(EdgeState::Count); i++)
		{
			if (i) out += ',';
			out += jsonString(edgeStateNames[i]);
		}
		out += "],\"pseudocode\":";
		appendStrings(out, pseudocode_);
		out += ",\"summary\":" + jsonString(summary_);

		out += ",\"result\":";
		appendObject(out, results_);
		out += ",\"legend\":{\"node\":";
		appendObject(out, nodeLegend_);
		out += ",\"edge\":";
		appendObject(out, edgeLegend_);
		out += "}";
		out += ",\"skippedSteps\":" + std::to_string(skipped_);

		out += ",\"views\":[";
		for (std::size_t i = 0; i < views_.size(); i++)
		{
			const View& view = views_[i];
			if (i) out += ',';
			out += "{\"title\":" + jsonString(view.title);
			out += ",\"layout\":" + jsonString(view.layout);
			out += ",\"directed\":" + std::string(view.graph.directed ? "true" : "false");
			out += ",\"showEdgeLabels\":" + std::string(view.showEdgeLabels ? "true" : "false");
			out += ",\"source\":" + std::to_string(view.source);
			out += ",\"target\":" + std::to_string(view.target);
			out += ",\"nodes\":[";
			for (std::size_t n = 0; n < view.graph.nodes.size(); n++)
			{
				const Node& node = view.graph.nodes[n];
				if (n) out += ',';
				out += "{\"label\":" + jsonString(node.label) + ",\"x\":" + jsonNumber(node.x) + ",\"y\":" + jsonNumber(node.y) + "}";
			}
			out += "],\"edges\":[";
			for (std::size_t e = 0; e < view.graph.edges.size(); e++)
			{
				const Edge& edge = view.graph.edges[e];
				if (e) out += ',';
				out += "{\"u\":" + std::to_string(edge.u) + ",\"v\":" + std::to_string(edge.v) + ",\"label\":" + jsonString(formatNumber(edge.weight)) + "}";
			}
			out += "]}";
		}
		out += "]";

		out += ",\"steps\":[";
		for (std::size_t i = 0; i < steps_.size(); i++)
		{
			const Step& step = steps_[i];
			if (i) out += ',';
			out += "{\"view\":" + std::to_string(step.view);
			out += ",\"line\":" + std::to_string(step.line);
			out += ",\"msg\":" + jsonString(step.message);
			out += ",\"ns\":";
			appendInts(out, step.nodeStates);
			out += ",\"es\":";
			appendInts(out, step.edgeStates);
			out += ",\"nl\":";
			appendStrings(out, step.nodeLabels);
			out += ",\"el\":";
			appendStrings(out, step.edgeLabels);
			out += ",\"panels\":[";
			for (std::size_t p = 0; p < step.panels.size(); p++)
			{
				if (p) out += ',';
				out += "{\"title\":" + jsonString(step.panels[p].first) + ",\"items\":";
				appendStrings(out, step.panels[p].second);
				out += "}";
			}
			out += "]}";
		}
		out += "]}";
		return out;
	}
}
