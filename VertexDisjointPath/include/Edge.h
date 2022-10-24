//!
//! 
//! 
//! 

#ifndef VDP_EDGE_H
#define VDP_EDGE_H

namespace VDP
{
	struct Edge 
	{
		//! Destination node.
		int dest;
		//! The weight of the edge.
		int weight;
		//! The capacity of the edge.
		int capacity;

		//!
		explicit Edge(int dest, int weight, int capacity) :dest(dest), weight(weight),capacity(capacity){}

		//!
		void updateCapacity(int c)
		{
			capacity = c;
		}

		//!
		void updateWeight(int w) 
		{
			weight = w;
		}

		friend bool operator == (const Edge& lhs, const Edge& rhs) 
		{
			return (lhs.capacity == rhs.capacity) && (lhs.dest == rhs.dest) && (lhs.weight == rhs.weight);
		};
	};

}
#endif