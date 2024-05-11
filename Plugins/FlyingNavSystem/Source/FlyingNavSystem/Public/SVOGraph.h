// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlyingNavSystemTypes.h"
#include "SVOQuerySettings.h"
#include "SVORaycast.h"
#include "GraphAStar.h"
#include "AI/Navigation/NavigationTypes.h"

// Forward Declarations
struct FSVOPathfindingGraph;

// Used for storing relative voxel positions. X,Y,Z = [-1,1] gives the 1-step 26 DOF directions.
struct FLYINGNAVSYSTEM_API FDirection
{
	int_fast8_t X;
	int_fast8_t Y;
	int_fast8_t Z;
	int_fast8_t bConnected: 1;

	explicit FDirection(const int_fast8_t X = 0,
	                    const int_fast8_t Y = 0,
	                    const int_fast8_t Z = 0,
	                    const bool bConnected = false)
		: X(X),
		  Y(Y),
		  Z(Z),
		  bConnected(bConnected)
	{
	}

	FVector ToVector(const FCoord VoxelSize) const
	{
		return FVector(X, Y, Z) * VoxelSize;
	}
};

// Sorting directions
inline bool operator<(const FDirection A, const FDirection B)
{
	if (A.bConnected > B.bConnected) { return true; } // Prefer connected
	if (A.Z > B.Z) { return true; } // Sort by highest Z
	return (A.X * A.X + A.Y * A.Y) < (B.X * B.X + B.Y * B.Y); // Sort by shortest horizontal distance
}

struct FLYINGNAVSYSTEM_API FSVOGraph
{
	friend FSVOPathfindingGraph;
	using FNodeRef = FSVOLink;

	FSVOGraph(const FSVOData& InNavigationData)
		: SVOData(InNavigationData.AsShared())
	{
	}

	// Number of neighbours in a given direction. Not trivial, but faster than GetNeighbours
	int32 NumNeighbours(const int32 Direction,
	                    const FNodeRef NeighbourRef) const;

	// Adds all neighbours on a given face of a node
	// Direction is the index into FSVOGenerator::Delta_ neighbour directions, 0 <= Direction < 6
	void SubdivideNeighbours(const int32 Direction,
	                         const FNodeRef NeighbourRef,
	                         TArray<FNodeRef>& Neighbours) const;

	// Returns a link to the neighbour of a leaf node in a given direction, or NULL_LINK if blocked
	FSVOLink GetLeafNeighbour(const FIntVector& LeafPos,
	                          const FSVONode& LeafParent,
	                          const int32 Direction) const;

	void GetNeighbours(const FNodeRef NodeRef,
	                   TArray<FNodeRef>& Neighbours) const;

	// Returns neighbour directions that are available
	// Used for 'projecting' points to free space
	// AgentPosition is used for sorting directions by connected components
	// Extent determines how far out to look for free space
	void GetAvailableDirections(const FVector& Position,
	                            const FVector& AgentPosition,
	                            TArray<FDirection>& Directions,
	                            const uint8 Extent = 1) const;

	// Returns whether given node identification is correct
	static bool IsValidRef(FNodeRef NodeRef) { return NodeRef.IsValid(); }

	// Returns number of neighbours that the graph node identified with NodeRef has
	UE_DEPRECATED(5.1, "DO NOT USE, inefficient. Use GetNeighbours instead.")
	int32 GetNeighbourCount(FNodeRef NodeRef) const
	{
		check(false)
		TArray<FNodeRef> Neighbours;
		GetNeighbours(NodeRef, Neighbours);
		return Neighbours.Num();
	}

	// Returns neighbour ref
	UE_DEPRECATED(5.1, "DO NOT USE, inefficient. Use GetNeighbours instead.")
	FNodeRef GetNeighbour(const FNodeRef NodeRef,
	                      const int32 NeighbourIndex) const
	{
		check(false)
		TArray<FNodeRef> Neighbours;
		GetNeighbours(NodeRef, Neighbours);
		return Neighbours[NeighbourIndex];
	}

protected:
	mutable FSVODataConstRef SVOData;
};

/**
 *	Pathfinding structure for Flying Navigation System
 *	Modified from FGraphAStar, the UE4 generic A* implementation
 *
 *	Use FindPath(...) to find a path between two nodes
 */
struct FLYINGNAVSYSTEM_API FSVOPathfindingGraph
	: FGraphAStar<FSVOGraph, FGraphAStarDefaultPolicy, FGraphAStarDefaultNode<FSVOGraph>>
{
	using FGraphNodeRef = FSVOGraph::FNodeRef;

	TUniquePtr<FSVORaycast> RaycastStruct;

	FSVOPathfindingGraph(const FSVOGraph& InGraph)
		: FGraphAStar(InGraph),
		  RaycastStruct(MakeUnique<FSVORaycast>(InGraph.SVOData.Get()))
	{
	}

	void UpdateNavData(const FSVOData& InNavigationData) const
	{
		RaycastStruct->NavData = InNavigationData.AsShared();
		Graph.SVOData = InNavigationData.AsShared();
	}

	/** 
	* Single run of pathfinding loop: get node from open set and process neighbors 
	* returns true if loop should be continued
	*/
	bool ProcessSingleAStarNode(const FGraphNodeRef EndNodeRef,
	                            const bool bIsBound,
	                            const FSVOQuerySettings& Filter,
	                            const FOverrideNodes& Overrides,
	                            int32& OutBestNodeIndex,
	                            FCoord& OutBestNodeCost);

	bool ProcessSingleThetaStarNode(const FGraphNodeRef EndNodeRef,
	                                const bool bIsBound,
	                                const FSVOQuerySettings& Filter,
	                                const FOverrideNodes& Overrides,
	                                int32& OutBestNodeIndex,
	                                FCoord& OutBestNodeCost);

	bool ProcessSingleLazyThetaStarNode(const FGraphNodeRef EndNodeRef,
	                                    const bool bIsBound,
	                                    const FSVOQuerySettings& Filter,
	                                    const FOverrideNodes& Overrides,
	                                    int32& OutBestNodeIndex,
	                                    FCoord& OutBestNodeCost);

	/** 
	*	Performs the actual search.
	*	@param StartNodeRef - Link to the first node
	*	@param EndNodeRef - Link to the destination node
	*	@param QuerySettings - Filter to determine heuristics, edge costs etc
	*	@param Overrides - Provides override positions for a given node, instead of the centre (start and end locations generally)
	*	@param [OUT] OutPath - on successful search contains a sequence of graph nodes representing 
	*		solution optimal within given constraints
	*/
	EGraphAStarResult FindSVOPath(const FGraphNodeRef StartNodeRef,
	                              const FGraphNodeRef EndNodeRef,
	                              const FSVOQuerySettings& QuerySettings,
	                              const FOverrideNodes& Overrides,
	                              TArray<FGraphNodeRef>& OutPath);

	// Find a path from StartLocation to EndLocation through the Sparse Voxel Octree
	ENavigationQueryResult::Type FindPath(const FVector& StartLocation,
	                                      const FVector& EndLocation,
	                                      const FSVOQuerySettings& QuerySettings,
	                                      TArray<FNavPathPoint>& PathPoints,
	                                      bool& bPartialSolution);

	ENavigationQueryResult::Type FindPath(const FVector& StartLocation,
	                                      const FVector& EndLocation,
	                                      const FSVOQuerySettings& QueryFilter,
	                                      TArray<FNavPathPoint>& PathPoints)
	{
		bool bPartialPaths = false;
		return FindPath(StartLocation, EndLocation, QueryFilter, PathPoints, bPartialPaths);
	}
};
