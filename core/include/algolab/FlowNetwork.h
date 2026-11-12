//! @copyright 2022-2026 Ali Ibrahim. Licensed under the MIT License (see LICENSE).
//! @author Ali Ibrahim
//!
//! The residual network used by maximum flow and vertex-disjoint paths.

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
		//! How findPath() looks for an augmenting path.
		enum class Strategy
		{
			Bfs,   //!< fewest arcs first (Edmonds-Karp)
			Dfs,   //!< the first path a depth-first search meets (classic Ford-Fulkerson)
			Widest //!< the path whose smallest residual capacity is largest
		};

		//! One direction of a link.
		struct Arc
		{
			int from{};         //!< tail vertex
			int to{};           //!< head vertex
			double capacity{};  //!< most flow this direction can carry
			double flow{};      //!< current flow; negative on a reverse arc that cancels flow
		};

		//! Result of looking for an augmenting path.
		struct Search
		{
			bool found{ false };       //!< a path to the sink exists
			std::vector<int> path;     //!< arc ids from source to sink
			std::vector<char> reached; //!< vertices the search touched
		};

		//! Create a network with no links.
		//! @param[in] vertexCount Number of vertices, numbered 0 .. vertexCount - 1.
		explicit FlowNetwork(int vertexCount);

		//! Add a link with zero flow.
		//! @param[in] from       Tail vertex.
		//! @param[in] to         Head vertex.
		//! @param[in] capacity   Most flow the link can carry.
		//! @param[in] undirected true: the link can carry `capacity` in either direction.
		//! @returns the link index (link i owns arcs 2i and 2i + 1).
		int addLink(int from, int to, double capacity, bool undirected = false);

		//! @returns the number of vertices.
		int vertexCount() const { return static_cast<int>(out_.size()); }

		//! @returns the number of links added.
		int linkCount() const { return static_cast<int>(arcs_.size() / 2); }

		//! @param[in] id Arc index (2 * link for the forward direction, 2 * link + 1 for the reverse).
		//! @returns the arc.
		const Arc& arc(int id) const { return arcs_[id]; }

		//! @param[in] link Link index.
		//! @returns true if the link was added as undirected.
		bool undirected(int link) const { return undirected_[link] != 0; }

		//! Net flow on a link in its `from -> to` direction.
		//! @param[in] link Link index.
		//! @returns the flow; negative on an undirected link means the flow runs `to -> from`.
		double linkFlow(int link) const { return arcs_[2 * link].flow; }

		//! @param[in] link Link index.
		//! @returns the capacity the link was added with.
		double linkCapacity(int link) const { return arcs_[2 * link].capacity; }

		//! @param[in] arcId Arc index.
		//! @returns how much more flow the arc can take: `capacity - flow`.
		double residual(int arcId) const { return arcs_[arcId].capacity - arcs_[arcId].flow; }

		//! Look for a source -> sink path that only uses arcs with residual capacity left.
		//! @param[in] source   Where the path starts.
		//! @param[in] sink     Where the path ends.
		//! @param[in] strategy Which path to prefer when there are several.
		//! @returns whether a path was found, the path, and the vertices the search reached.
		Search findPath(int source, int sink, Strategy strategy) const;

		//! @param[in] path Arc ids, as returned in Search::path.
		//! @returns the smallest residual capacity along the path: how much can be pushed.
		double bottleneck(const std::vector<int>& path) const;

		//! Push flow along a path: add it on every arc and subtract it from each reverse arc.
		//! @param[in] path   Arc ids, as returned in Search::path.
		//! @param[in] amount Flow to push; at most bottleneck(path).
		void augment(const std::vector<int>& path, double amount);

		//! Find the vertices reachable from `source` in the residual graph. Once the flow is
		//! maximum, these are the source side of a minimum cut.
		//! @param[in] source Start vertex.
		//! @returns one flag per vertex, non-zero when reachable.
		std::vector<char> reachableFrom(int source) const;

		//! @param[in] vertex A vertex.
		//! @returns the arc ids leaving it (forward and reverse arcs), ordered by head vertex.
		const std::vector<int>& out(int vertex) const { return out_[vertex]; }

		//! @param[in] name "bfs", "dfs" or "widest"; anything else means "bfs".
		//! @returns the matching strategy.
		static Strategy strategyFromName(const std::string& name);

		//! @param[in] strategy A strategy.
		//! @returns its name for explanations, such as "breadth-first search".
		static std::string strategyTitle(Strategy strategy);

	private:
		//! Depth-first search for Strategy::Dfs.
		//! @param[in]     vertex  The vertex being expanded.
		//! @param[in]     sink    The vertex to reach.
		//! @param[in,out] reached Vertices already visited; updated as the search goes.
		//! @param[out]    path    Arc ids collected sink-first while unwinding (the caller reverses them).
		//! @returns true once the sink has been reached.
		bool dfs(int vertex, int sink, std::vector<char>& reached, std::vector<int>& path) const;

		std::vector<Arc> arcs_;             //!< all arcs; link i owns arcs 2i and 2i + 1
		std::vector<char> undirected_;      //!< per link: added as undirected
		std::vector<std::vector<int>> out_; //!< per vertex: arc ids leaving it, sorted by head
	};
}
#endif // !ALGOLAB_FLOW_NETWORK_H
