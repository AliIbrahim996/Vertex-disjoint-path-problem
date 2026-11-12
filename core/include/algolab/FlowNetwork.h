//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim

#ifndef ALGOLAB_FLOW_NETWORK_H
#define ALGOLAB_FLOW_NETWORK_H

#include <string>
#include <vector>

namespace algolab
{
	//! A flow network stored as paired arcs: link `i` owns arc `2i` (as added) and arc `2i+1`
	//! (its reverse). The residual capacity of an arc is `capacity - flow`, and pushing flow
	//! on an arc removes the same amount from its partner, so the residual graph is implicit.
	class FlowNetwork
	{
	public:
		enum class Strategy { Bfs, Dfs, Widest };

		struct Arc
		{
			int from{};
			int to{};
			double capacity{};
			double flow{};
		};

		//! Result of looking for an augmenting path.
		struct Search
		{
			bool found{ false };
			std::vector<int> path;     //!< arc ids from source to sink
			std::vector<char> reached; //!< vertices the search touched
		};

		explicit FlowNetwork(int vertexCount);

		//! Add a link. An undirected link can carry `capacity` in either direction.
		//! @returns the link index.
		int addLink(int from, int to, double capacity, bool undirected = false);

		int vertexCount() const { return static_cast<int>(out_.size()); }
		int linkCount() const { return static_cast<int>(arcs_.size() / 2); }
		const Arc& arc(int id) const { return arcs_[id]; }
		bool undirected(int link) const { return undirected_[link] != 0; }

		//! Net flow on a link in its `from -> to` direction (negative on an undirected link
		//! means the flow runs the other way).
		double linkFlow(int link) const { return arcs_[2 * link].flow; }
		double linkCapacity(int link) const { return arcs_[2 * link].capacity; }

		double residual(int arcId) const { return arcs_[arcId].capacity - arcs_[arcId].flow; }

		//! Look for a source -> sink path that only uses arcs with residual capacity left.
		Search findPath(int source, int sink, Strategy strategy) const;

		//! Smallest residual capacity along a path.
		double bottleneck(const std::vector<int>& path) const;

		//! Push `amount` units along a path.
		void augment(const std::vector<int>& path, double amount);

		//! Vertices reachable from `source` in the residual graph (the source side of a min cut
		//! once the flow is maximum).
		std::vector<char> reachableFrom(int source) const;

		//! Arc ids leaving a vertex, ordered by head vertex.
		const std::vector<int>& out(int vertex) const { return out_[vertex]; }

		static Strategy strategyFromName(const std::string& name);
		static std::string strategyTitle(Strategy strategy);

	private:
		bool dfs(int vertex, int sink, std::vector<char>& reached, std::vector<int>& path) const;

		std::vector<Arc> arcs_;
		std::vector<char> undirected_;
		std::vector<std::vector<int>> out_;
	};
}
#endif // !ALGOLAB_FLOW_NETWORK_H
