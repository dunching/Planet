// Copyright Ben Sutherland 2024. All rights reserved.

#include "SVOGraph.h"

#include "FlyingNavigationData.h"
#include "FlyingNavSystemModule.h"
#include "FlyingNavSystemTypes.h"
#include "ThirdParty/libmorton/morton.h"


//----------------------------------------------------------------------//
// FSVOGraph Implementation
//----------------------------------------------------------------------//

int32 FSVOGraph::NumNeighbours(const int32 Direction,
                               const FSVOLink NeighbourRef) const
{
	const uint32 NeighbourLayerIdx = NeighbourRef.GetLayerIndex();
	const uint32 NeighbourNodeIdx = NeighbourRef.GetNodeIndex();

	// Neighbour is a leaf layer
	if (NeighbourLayerIdx == 0)
	{
		const FSVOLeafLayer& LeafLayer = SVOData->LeafLayer;

		// Unblocked leaves are 1, blocked are 16 (on a face)
		if (LeafLayer[NeighbourNodeIdx].IsCompletelyFree())
		{
			return 1;
		}
		return 16;
	}

	const FSVONode& NeighbourNode = SVOData->GetLayer(NeighbourLayerIdx)[NeighbourNodeIdx];

	// End of the recursion
	if (!NeighbourNode.bHasChildren)
	{
		return !NeighbourNode.bBlocked;
	}

	const FSVOLink ChildLink = NeighbourNode.FirstChild;

	// Find neighbour direction
	return NumNeighbours(Direction, ChildLink + FlyingNavSystem::NeighbourMortonOffset[Direction][0]) +
		NumNeighbours(Direction, ChildLink + FlyingNavSystem::NeighbourMortonOffset[Direction][1]) +
		NumNeighbours(Direction, ChildLink + FlyingNavSystem::NeighbourMortonOffset[Direction][2]) +
		NumNeighbours(Direction, ChildLink + FlyingNavSystem::NeighbourMortonOffset[Direction][3]);
}

void FSVOGraph::SubdivideNeighbours(const int32 Direction,
                                    const FNodeRef NeighbourRef,
                                    TArray<FNodeRef>& Neighbours) const
{
	const int32 LayerIdx = NeighbourRef.GetLayerIndex();
	const int32 NodeIdx = NeighbourRef.GetNodeIndex();

	if (LayerIdx == 0)
	{
		const FSVOLeafLayer& LeafLayer = SVOData->LeafLayer;
		const FSVOLeafNode& Leaf = LeafLayer[NodeIdx];

		// Unblocked leaves are 1, blocked are 16 (on a face)
		if (Leaf.IsCompletelyFree())
		{
			Neighbours.Add(NeighbourRef);
			return;
		}

		// Check 16 nodes on the face
		for (const small_morton_t SubNodeIndex : FlyingNavSystem::SubNodeMortonsForDirection[Direction])
		{
			if (!Leaf.IsIndexBlocked(SubNodeIndex))
			{
				Neighbours.Add(FSVOLink(0, NodeIdx, SubNodeIndex));
			}
		}
	}
	else
	{
		// LayerIdx > 0
		const FSVONode& NeighbourNode = SVOData->GetLayer(LayerIdx)[NodeIdx];

		// End of the recursion
		if (!NeighbourNode.bHasChildren)
		{
			if (!NeighbourNode.bBlocked)
			{
				Neighbours.Add(FSVOLink(LayerIdx, NodeIdx));
			}
			return;
		}

		const FSVOLink& ChildLink = NeighbourNode.FirstChild;

		// Subdivide face
		SubdivideNeighbours(Direction, ChildLink + FlyingNavSystem::NeighbourMortonOffset[Direction][0], Neighbours);
		SubdivideNeighbours(Direction, ChildLink + FlyingNavSystem::NeighbourMortonOffset[Direction][1], Neighbours);
		SubdivideNeighbours(Direction, ChildLink + FlyingNavSystem::NeighbourMortonOffset[Direction][2], Neighbours);
		SubdivideNeighbours(Direction, ChildLink + FlyingNavSystem::NeighbourMortonOffset[Direction][3], Neighbours);
	}
}

FSVOLink FSVOGraph::GetLeafNeighbour(const FIntVector& LeafPos,
                                     const FSVONode& LeafParent,
                                     const int32 Direction) const
{
	const FIntVector Delta = FlyingNavSystem::GetDelta(Direction);
	const FIntVector LeafNeighbourPos = LeafPos + Delta;

	// Inside parent boundary: 0 <= X,Y,Z <= 1
	if (0 <= LeafNeighbourPos.GetMin() && LeafNeighbourPos.GetMax() <= 1)
	{
		// Add neighbours in the Delta direction
		const small_morton_t LeafNeighbourIdx = libmorton::morton3D_32_encode_v(LeafNeighbourPos);
		const FSVOLink LeafNeighbourLink = LeafParent.FirstChild + LeafNeighbourIdx;

		return LeafNeighbourLink;
	}
	else
	{
		// Outside parent boundary, go up a level to find neighbour
		const FSVOLink ParentNeighbourLink = LeafParent.Neighbours[Direction];
		if (ParentNeighbourLink.IsValid())
		{
			const FSVONode& ParentNeighbour = SVOData->GetNodeForLink(ParentNeighbourLink);

			if (!ParentNeighbour.bBlocked)
			{
				if (ParentNeighbourLink.GetLayerIndex() == 1)
				{
					// Look at parent's neighbour's children
					if (ParentNeighbour.bHasChildren)
					{
						// Working in mod 2 (children can have 0-1 relative coords), in the neighbour system the index is (LeafPos + Delta) % 2 = LeafPos - Delta
						const small_morton_t LeafNeighbourIdx = libmorton::morton3D_32_encode(
							LeafNeighbourPos.X & 0b1,
							LeafNeighbourPos.Y & 0b1,
							LeafNeighbourPos.Z & 0b1);
						const FSVOLink LeafNeighbourLink = ParentNeighbour.FirstChild + LeafNeighbourIdx;

						return LeafNeighbourLink;
					}
				}

				return ParentNeighbourLink;
			}
		}
	}
	return FSVOLink::NULL_LINK;
}

void FSVOGraph::GetNeighbours(const FNodeRef NodeRef,
                              TArray<FNodeRef>& Neighbours) const
{
	const int32 LayerIdx = NodeRef.GetLayerIndex();
	const int32 NodeIdx = NodeRef.GetNodeIndex();

	if (LayerIdx == 0)
	{
		const FSVOLeafLayer& LeafLayer = SVOData->LeafLayer;
		const FSVOLeafNode& Leaf = LeafLayer[NodeIdx];

		// Layer 1 node that contains leaf
		const FSVONode& LeafParent = SVOData->GetLayer(1)[Leaf.Parent.GetNodeIndex()];

		// Find leaf position in parent coordinate system
		const int32 ChildIdx = NodeIdx - LeafParent.FirstChild.GetNodeIndex();
		small_coord_t LeafX, LeafY, LeafZ;
		libmorton::morton3D_32_decode(ChildIdx, LeafX, LeafY, LeafZ);
		const FIntVector LeafPos(LeafX, LeafY, LeafZ);

		// If leaf is completely unblocked, don't worry about SubNodes
		if (Leaf.IsCompletelyFree())
		{
			// Treat as normal node
			for (int i = 0; i < 6; i++)
			{
				const FSVOLink LeafNeighbourLink = GetLeafNeighbour(LeafPos, LeafParent, i);

				if (LeafNeighbourLink.IsValid())
				{
					// If the neighbour is also a leaf node
					if (LeafNeighbourLink.GetLayerIndex() == 0)
					{
						// Only add leaves that aren't completely blocked
						if (!LeafLayer[LeafNeighbourLink.GetNodeIndex()].IsCompletelyBlocked())
						{
							SubdivideNeighbours(i, LeafNeighbourLink, Neighbours);
						}
					}
					else
					{
						// Doesn't have children, just add directly
						if (!SVOData->GetNodeForLink(LeafNeighbourLink).bBlocked)
						{
							Neighbours.Add(LeafNeighbourLink);
						}
					}
				}
			}
		}
		else
		{
			// Leaf has filled voxels, add SubNode neighbours
			const int32 SubNodeIndex = NodeRef.GetSubNodeIndex();
			for (int i = 0; i < 6; i++)
			{
				const FIntVector Delta = FlyingNavSystem::GetDelta(i);
				small_coord_t X, Y, Z;
				libmorton::morton3D_32_decode(SubNodeIndex, X, Y, Z);
				const FIntVector SubNodePos(X, Y, Z);
				FIntVector SubNodeNeighbourPos = SubNodePos + Delta;

				// Inside leaf boundary: 0 <= X, Y, Z < 4
				if (0 <= SubNodeNeighbourPos.GetMin() && SubNodeNeighbourPos.GetMax() < 4)
				{
					// Index in current leaf voxel grid (they're all in morton order)
					const small_morton_t NeighbourIdx = libmorton::morton3D_32_encode_v(SubNodeNeighbourPos);

					// Only add unblocked neighbour
					if (!Leaf.IsIndexBlocked(NeighbourIdx))
					{
						Neighbours.Add(FSVOLink(0, NodeIdx, NeighbourIdx));
					}
				}
				else
				{
					// SubNode is outside leaf boundary
					const FSVOLink LeafNeighbourLink = GetLeafNeighbour(LeafPos, LeafParent, i);

					if (LeafNeighbourLink.IsValid())
					{
						if (LeafNeighbourLink.GetLayerIndex() == 0)
						{
							const FSVOLeafNode& LeafNeighbourNode = LeafLayer[LeafNeighbourLink.GetNodeIndex()];

							if (LeafNeighbourNode.IsCompletelyFree())
							{
								// Add whole leaf
								Neighbours.Add(LeafNeighbourLink);
							}
							else if (!LeafNeighbourNode.IsCompletelyBlocked()) // Leaf node has free subnodes
							{
								// Add adjacent SubNode (& 0b11 is the same as % 4, without negatives)
								SubNodeNeighbourPos = FIntVector(SubNodeNeighbourPos.X & 0b11,
								                                 SubNodeNeighbourPos.Y & 0b11,
								                                 SubNodeNeighbourPos.Z & 0b11);
								const small_morton_t SubNodeNeighbourIdx = libmorton::morton3D_32_encode_v(
									SubNodeNeighbourPos);
								if (!LeafNeighbourNode.IsIndexBlocked(SubNodeNeighbourIdx))
								{
									Neighbours.Add(FSVOLink(0, LeafNeighbourLink.GetNodeIndex(), SubNodeNeighbourIdx));
								}
							}
						}
						else
						{
							// Leaf neighbour doesn't have children
							Neighbours.Add(LeafNeighbourLink);
						}
					}
				}
			}
		}
	}
	else
	{
		// LayerIdx > 0
		const FSVONode& Node = SVOData->GetLayer(LayerIdx)[NodeIdx];

		for (int i = 0; i < 6; i++)
		{
			if (Node.Neighbours[i].IsValid())
			{
				SubdivideNeighbours(i, Node.Neighbours[i], Neighbours);
			}
		}
	}
}

void FSVOGraph::GetAvailableDirections(const FVector& Position,
                                       const FVector& AgentPosition,
                                       TArray<FDirection>& Directions,
                                       const uint8 Extent) const
{
	const FCoord VoxelSize = SVOData->SubNodeSideLength;
	const FVector VoxelCentre = SVOData->SnapPositionToVoxelGrid(Position);
	const FSVOLink AgentLink = SVOData->GetNodeLinkForPosition(AgentPosition);
	const bool bValidLink = AgentLink.IsValid();

	for (int_fast8_t X = -Extent; X <= Extent; X++)
	{
		for (int_fast8_t Y = -Extent; Y <= Extent; Y++)
		{
			for (int_fast8_t Z = -Extent; Z <= Extent; Z++)
			{
				if (X == 0 && Y == 0 && Z == 0) { continue; }

				// Add direction if it's not blocked
				const FSVOLink DirectionLink = SVOData->GetNodeLinkForPosition(
					VoxelCentre + FVector(X, Y, Z) * VoxelSize);

				if (DirectionLink.IsValid())
				{
					const bool bConnected = bValidLink && SVOData->IsConnected(DirectionLink, AgentLink);
					Directions.Add(FDirection(X, Y, Z, bConnected));
				}
			}
		}
	}
}

bool FSVOPathfindingGraph::ProcessSingleAStarNode(const FGraphNodeRef EndNodeRef,
                                                  const bool bIsBound,
                                                  const FSVOQuerySettings& Filter,
                                                  const FOverrideNodes& Overrides,
                                                  int32& OutBestNodeIndex,
                                                  FCoord& OutBestNodeCost)
{
	// Pop next best node and put it on closed list
	const int32 ConsideredNodeIndex = OpenList.PopIndex();
	FSearchNode& CurrentNodeUnsafe = NodePool[ConsideredNodeIndex];
	CurrentNodeUnsafe.MarkClosed();
	const FSearchNode CurrentNode = CurrentNodeUnsafe; // Make a copy

	const FGraphNodeRef CurrentNodeRef = CurrentNode.NodeRef;
	const FGraphNodeRef ParentNodeRef = CurrentNode.ParentRef;

	const FCoord HeuristicScale = Filter.GetHeuristicScale();

	// consider every neighbor of CurrentNode
	TArray<FGraphNodeRef> Neighbours;
	Graph.GetNeighbours(CurrentNodeRef, Neighbours);
	const int32 NeighbourCount = Neighbours.Num();

	// We're there
	if (bIsBound && (CurrentNode.NodeRef == EndNodeRef))
	{
		OutBestNodeIndex = CurrentNode.SearchNodeIndex;
		OutBestNodeCost = 0.f;
		return false;
	}

	const FCoord CurrentTraversalCost = CurrentNode.TraversalCost;
	const int32 CurrentSearchNodeIdx = CurrentNode.SearchNodeIndex;

	for (int32 NeighbourNodeIndex = 0; NeighbourNodeIndex < NeighbourCount; ++NeighbourNodeIndex)
	{
		const FGraphNodeRef NeighbourRef = Neighbours[NeighbourNodeIndex];

		// validate and sanitize
		if (Graph.IsValidRef(NeighbourRef) == false
			|| NeighbourRef == ParentNodeRef
			|| NeighbourRef == CurrentNodeRef
			|| Filter.IsTraversalAllowed(CurrentNodeRef, NeighbourRef) == false)
		{
			continue;
		}

		FSearchNode& NeighbourNode = NodePool.FindOrAdd(NeighbourRef);

		if (NeighbourNode.bIsClosed)
		{
			continue;
		}

		// Calculate cost and heuristic.
		const FCoord NewTraversalCost = Filter.GetTraversalCost(CurrentNodeRef, NeighbourNode.NodeRef, Overrides) +
			CurrentTraversalCost;
		const FCoord NewHeuristicCost = bIsBound && (NeighbourNode.NodeRef != EndNodeRef)
			                                ? (Filter.GetHeuristicCost(NeighbourNode.NodeRef, EndNodeRef, Overrides) *
				                                HeuristicScale)
			                                : 0.f;
		FCoord NewTotalCost = NewTraversalCost + NewHeuristicCost;

		if (Filter.bUseNodeCompensation)
		{
			// cost *= 1.f - size * comp
			NewTotalCost *= (1.f - Graph.SVOData->GetLayerProportionForLink(NeighbourRef) * Filter.MaxNodeCompensation);
		}

		// check if this is better then the potential previous approach
		if (NewTraversalCost >= NeighbourNode.TraversalCost)
		{
			// if not, skip
			continue;
		}

		// fill in
		NeighbourNode.TraversalCost = NewTraversalCost;
		ensure(NewTraversalCost > 0);
		NeighbourNode.TotalCost = NewTotalCost;
		NeighbourNode.ParentRef = CurrentNodeRef;
		NeighbourNode.ParentNodeIndex = CurrentSearchNodeIdx;
		NeighbourNode.MarkNotClosed();

		if (NeighbourNode.IsOpened() == false)
		{
			OpenList.Push(NeighbourNode);
		}

		// In case there's no path let's store information on
		// "closest to goal" node
		// using Heuristic cost here rather than Traversal or Total cost
		// since this is what we'll care about if there's no solution - this node 
		// will be the one estimated-closest to the goal
		if (NewHeuristicCost < OutBestNodeCost)
		{
			OutBestNodeCost = NewHeuristicCost;
			OutBestNodeIndex = NeighbourNode.SearchNodeIndex;
		}
	}

	// Checks for memory issues
	check(NodePool[ConsideredNodeIndex].SearchNodeIndex == ConsideredNodeIndex)

	return true;
}

bool FSVOPathfindingGraph::ProcessSingleThetaStarNode(const FGraphNodeRef EndNodeRef,
                                                      const bool bIsBound,
                                                      const FSVOQuerySettings& Filter,
                                                      const FOverrideNodes& Overrides,
                                                      int32& OutBestNodeIndex,
                                                      FCoord& OutBestNodeCost)
{
	// Pop next best node and put it on closed list
	const int32 ConsideredNodeIndex = OpenList.PopIndex();
	FSearchNode& CurrentNodeUnsafe = NodePool[ConsideredNodeIndex];
	CurrentNodeUnsafe.MarkClosed();
	const FSearchNode CurrentNode = CurrentNodeUnsafe; // Make a copy

	const FGraphNodeRef CurrentNodeRef = CurrentNode.NodeRef;
	const FGraphNodeRef ParentNodeRef = CurrentNode.ParentRef;

	const FCoord HeuristicScale = Filter.GetHeuristicScale();

	// consider every neighbor of CurrentNode
	TArray<FGraphNodeRef> Neighbours;
	Graph.GetNeighbours(CurrentNodeRef, Neighbours);
	const int32 NeighbourCount = Neighbours.Num();

	// We're there, store and move to result composition
	if (bIsBound && (CurrentNode.NodeRef == EndNodeRef))
	{
		OutBestNodeIndex = CurrentNode.SearchNodeIndex;
		OutBestNodeCost = 0.f;
		return false;
	}

	const FCoord CurrentTraversalCost = CurrentNode.TraversalCost;
	const int32 CurrentSearchNodeIdx = CurrentNode.SearchNodeIndex;
	const int32 ParentSearchNodeIdx = CurrentNode.ParentNodeIndex;
	const int32 ParentIdx = ParentSearchNodeIdx == INDEX_NONE ? 0 : ParentSearchNodeIdx;
	const FSearchNode ParentNode = NodePool[ParentIdx];

	for (int32 NeighbourNodeIndex = 0; NeighbourNodeIndex < NeighbourCount; ++NeighbourNodeIndex)
	{
		const FGraphNodeRef NeighbourRef = Neighbours[NeighbourNodeIndex];

		// validate and sanitize
		if (Graph.IsValidRef(NeighbourRef) == false
			|| NeighbourRef == ParentNodeRef
			|| NeighbourRef == CurrentNodeRef
			|| Filter.IsTraversalAllowed(CurrentNodeRef, NeighbourRef) == false)
		{
			continue;
		}

		FSearchNode& NeighbourNode = NodePool.FindOrAdd(NeighbourRef);

		if (NeighbourNode.bIsClosed)
		{
			continue;
		}

		bool bLineOfSight = false;
		if (ParentSearchNodeIdx != INDEX_NONE)
		{
			const FVector ParentPosition = Graph.SVOData->GetPositionForLinkWithOverride(ParentNodeRef, Overrides);
			const FVector NeighbourPosition = Graph.SVOData->GetPositionForLinkWithOverride(NeighbourNode.NodeRef, Overrides);
			bLineOfSight = !RaycastStruct->Raycast(ParentPosition, NeighbourPosition);
		}

		// Calculate cost and heuristic.
		FCoord NewTraversalCost;
		const FCoord NewHeuristicCost = bIsBound && (NeighbourNode.NodeRef != EndNodeRef)
			                                ? (Filter.GetHeuristicCost(NeighbourNode.NodeRef, EndNodeRef, Overrides) *
				                                HeuristicScale)
			                                : 0.f;

		FGraphNodeRef NeighbourParentNodeRef;
		int32 NeighbourParentNodeIdx;
		if (bLineOfSight)
		{
			// Calculate cost from parent to neighbour
			NewTraversalCost = ParentNode.TraversalCost + Filter.GetTraversalCost(
				ParentNode.NodeRef,
				NeighbourNode.NodeRef,
				Overrides);

			NeighbourParentNodeRef = ParentNode.NodeRef;
			NeighbourParentNodeIdx = ParentNode.SearchNodeIndex;
		}
		else
		{
			// Calculate cost from current node to neighbour
			NewTraversalCost = CurrentTraversalCost + Filter.GetTraversalCost(CurrentNodeRef, NeighbourNode.NodeRef, Overrides);
			NeighbourParentNodeRef = CurrentNodeRef;
			NeighbourParentNodeIdx = CurrentSearchNodeIdx;
		}

		FCoord NewTotalCost = NewTraversalCost + NewHeuristicCost;

		if (Filter.bUseNodeCompensation)
		{
			// cost *= 1.f - size * comp
			NewTotalCost *= (1.f - Graph.SVOData->GetLayerProportionForLink(NeighbourRef) * Filter.MaxNodeCompensation);
		}

		// check if this is better then the potential previous approach
		if (NewTraversalCost >= NeighbourNode.TraversalCost)
		{
			// if not, skip
			continue;
		}

		// fill in
		NeighbourNode.TraversalCost = NewTraversalCost;
		ensure(NewTraversalCost > 0);
		NeighbourNode.TotalCost = NewTotalCost;
		NeighbourNode.ParentRef = NeighbourParentNodeRef;
		NeighbourNode.ParentNodeIndex = NeighbourParentNodeIdx;
		NeighbourNode.MarkNotClosed();

		if (NeighbourNode.IsOpened() == false)
		{
			OpenList.Push(NeighbourNode);
		}

		// In case there's no path let's store information on
		// "closest to goal" node
		// using Heuristic cost here rather than Traversal or Total cost
		// since this is what we'll care about if there's no solution - this node 
		// will be the one estimated-closest to the goal
		if (NewHeuristicCost < OutBestNodeCost)
		{
			OutBestNodeCost = NewHeuristicCost;
			OutBestNodeIndex = NeighbourNode.SearchNodeIndex;
		}
	}

	// Checks for memory issues
	check(NodePool[ConsideredNodeIndex].SearchNodeIndex == ConsideredNodeIndex)

	return true;
}

bool FSVOPathfindingGraph::ProcessSingleLazyThetaStarNode(const FGraphNodeRef EndNodeRef,
                                                          const bool bIsBound,
                                                          const FSVOQuerySettings& Filter,
                                                          const FOverrideNodes& Overrides,
                                                          int32& OutBestNodeIndex,
                                                          FCoord& OutBestNodeCost)
{
	// Pop next best node and put it on closed list
	const int32 ConsideredNodeIndex = OpenList.PopIndex();
	FSearchNode* CurrentNode = &NodePool[ConsideredNodeIndex];
	// CurrentNode is valid as long as NodePool isn't modified
	CurrentNode->MarkClosed();

	const FGraphNodeRef CurrentNodeRef = CurrentNode->NodeRef;
	const FGraphNodeRef ParentNodeRef = CurrentNode->ParentRef;

	const FCoord HeuristicScale = Filter.GetHeuristicScale();

	// consider every neighbor of CurrentNode
	TArray<FGraphNodeRef> Neighbours;
	Graph.GetNeighbours(CurrentNodeRef, Neighbours);
	const int32 NeighbourCount = Neighbours.Num();

	// Check if neighbour actually has line of sight
	if (CurrentNode->ParentNodeIndex != INDEX_NONE)
	{
		// Check line of sight between parent and current node
		const FVector ParentPosition = Graph.SVOData->GetPositionForLinkWithOverride(ParentNodeRef, Overrides);
		const FVector CurrentPosition = Graph.SVOData->GetPositionForLinkWithOverride(CurrentNodeRef, Overrides);
		const bool bLineOfSight = !RaycastStruct->Raycast(ParentPosition, CurrentPosition);
		if (!bLineOfSight)
		{
			// Update parent
			FCoord MinTraversalCost = FLT_MAX;
			for (int32 NeighbourNodeIndex = 0; NeighbourNodeIndex < NeighbourCount; ++NeighbourNodeIndex)
			{
				const FGraphNodeRef NeighbourRef = Neighbours[NeighbourNodeIndex];
				FSearchNode& NeighbourNode = NodePool.FindOrAdd(NeighbourRef);
				CurrentNode = &NodePool[ConsideredNodeIndex]; // NodePool updated, update CurrentNode

				if (NeighbourNode.bIsClosed)
				{
					const FCoord TraversalCost = NeighbourNode.TraversalCost + Filter.GetTraversalCost(
						NeighbourNode.NodeRef,
						CurrentNodeRef,
						Overrides);
					const FCoord HeuristicCost = bIsBound && (NeighbourNode.NodeRef != EndNodeRef)
						                             ? (Filter.GetHeuristicCost(
							                             NeighbourNode.NodeRef,
							                             EndNodeRef,
							                             Overrides) * HeuristicScale)
						                             : 0.f;
					if (MinTraversalCost > TraversalCost)
					{
						MinTraversalCost = TraversalCost;
						CurrentNode->TraversalCost = TraversalCost;
						CurrentNode->TotalCost = TraversalCost + HeuristicCost;
						CurrentNode->ParentRef = NeighbourRef;
						CurrentNode->ParentNodeIndex = NeighbourNode.SearchNodeIndex;
					}
				}
			}
		}
	}

	// We're there
	if (bIsBound && (CurrentNode->NodeRef == EndNodeRef))
	{
		OutBestNodeIndex = CurrentNode->SearchNodeIndex;
		OutBestNodeCost = 0.f;
		return false;
	}

	const FCoord CurrentTraversalCost = CurrentNode->TraversalCost;
	const int32 CurrentSearchNodeIdx = CurrentNode->SearchNodeIndex;
	const int32 ParentSearchNodeIdx = CurrentNode->ParentNodeIndex;
	const int32 ParentIdx = ParentSearchNodeIdx == INDEX_NONE ? 0 : ParentSearchNodeIdx;
	const FSearchNode ParentNode = NodePool[ParentIdx];

	// Shouldn't access this past this point
	CurrentNode = nullptr;

	for (int32 NeighbourNodeIndex = 0; NeighbourNodeIndex < NeighbourCount; ++NeighbourNodeIndex)
	{
		const FGraphNodeRef NeighbourRef = Neighbours[NeighbourNodeIndex];

		// validate and sanitize
		if (Graph.IsValidRef(NeighbourRef) == false
			|| NeighbourRef == ParentNodeRef
			|| NeighbourRef == CurrentNodeRef
			|| Filter.IsTraversalAllowed(CurrentNodeRef, NeighbourRef) == false)
		{
			continue;
		}

		FSearchNode& NeighbourNode = NodePool.FindOrAdd(NeighbourRef);

		if (NeighbourNode.bIsClosed)
		{
			continue;
		}

		// Calculate cost and heuristic.
		FCoord NewTraversalCost;
		const FCoord NewHeuristicCost = bIsBound && (NeighbourNode.NodeRef != EndNodeRef)
			                                ? (Filter.GetHeuristicCost(NeighbourNode.NodeRef, EndNodeRef, Overrides) *
				                                HeuristicScale)
			                                : 0.f;

		FGraphNodeRef NeighbourParentNodeRef;
		int32 NeighbourParentNodeIdx;
		if (ParentSearchNodeIdx != INDEX_NONE)
		{
			// Calculate cost from parent to neighbour
			NewTraversalCost = ParentNode.TraversalCost + Filter.GetTraversalCost(
				ParentNode.NodeRef,
				NeighbourNode.NodeRef,
				Overrides);

			NeighbourParentNodeRef = ParentNode.NodeRef;
			NeighbourParentNodeIdx = ParentNode.SearchNodeIndex;
		}
		else
		{
			// Calculate cost from current node to neighbour
			NewTraversalCost = CurrentTraversalCost + Filter.GetTraversalCost(CurrentNodeRef, NeighbourNode.NodeRef, Overrides);
			NeighbourParentNodeRef = CurrentNodeRef;
			NeighbourParentNodeIdx = CurrentSearchNodeIdx;
		}

		FCoord NewTotalCost = NewTraversalCost + NewHeuristicCost;

		if (Filter.bUseNodeCompensation)
		{
			// cost *= 1.f - size * comp
			NewTotalCost *= (1.f - Graph.SVOData->GetLayerProportionForLink(NeighbourRef) * Filter.MaxNodeCompensation);
		}

		// check if this is better then the potential previous approach
		if (NewTraversalCost >= NeighbourNode.TraversalCost)
		{
			// if not, skip
			continue;
		}

		// fill in
		NeighbourNode.TraversalCost = NewTraversalCost;
		ensure(NewTraversalCost > 0);
		NeighbourNode.TotalCost = NewTotalCost;
		NeighbourNode.ParentRef = NeighbourParentNodeRef;
		NeighbourNode.ParentNodeIndex = NeighbourParentNodeIdx;
		NeighbourNode.MarkNotClosed();

		if (NeighbourNode.IsOpened() == false)
		{
			OpenList.Push(NeighbourNode);
		}

		// In case there's no path let's store information on
		// "closest to goal" node
		// using Heuristic cost here rather than Traversal or Total cost
		// since this is what we'll care about if there's no solution - this node 
		// will be the one estimated-closest to the goal
		if (NewHeuristicCost < OutBestNodeCost)
		{
			OutBestNodeCost = NewHeuristicCost;
			OutBestNodeIndex = NeighbourNode.SearchNodeIndex;
		}
	}

	// Checks for memory issues
	check(NodePool[ConsideredNodeIndex].SearchNodeIndex == ConsideredNodeIndex)

	return true;
}

using ProcessNodeFunction = bool(FSVOPathfindingGraph::*)(const FSVOPathfindingGraph::FGraphNodeRef EndNodeRef,
                                                          const bool,
                                                          const FSVOQuerySettings&,
                                                          const FOverrideNodes&,
                                                          int32&,
                                                          FCoord&);

EGraphAStarResult FSVOPathfindingGraph::FindSVOPath(const FGraphNodeRef StartNodeRef,
                                                    const FGraphNodeRef EndNodeRef,
                                                    const FSVOQuerySettings& QuerySettings,
                                                    const FOverrideNodes& Overrides,
                                                    TArray<FGraphNodeRef>& OutPath)
{
	if (!(Graph.IsValidRef(StartNodeRef) && Graph.IsValidRef(EndNodeRef)))
	{
		return SearchFail;
	}

	if (StartNodeRef == EndNodeRef)
	{
		return SearchSuccess;
	}

	if (FGraphAStarDefaultPolicy::bReuseNodePoolInSubsequentSearches)
	{
		NodePool.ReinitNodes();
	}
	else
	{
		NodePool.Reset();
	}
	OpenList.Reset();

	// kick off the search with the first node
	FSearchNode& StartNode = NodePool.Add(FSearchNode(StartNodeRef));
	StartNode.TraversalCost = 0;
	StartNode.TotalCost = QuerySettings.GetHeuristicCost(StartNodeRef, EndNodeRef, Overrides) * QuerySettings.
		GetHeuristicScale();

	OpenList.Push(StartNode);

	int32 BestNodeIndex = StartNode.SearchNodeIndex;
	FCoord BestNodeCost = StartNode.TotalCost;

	EGraphAStarResult Result = EGraphAStarResult::SearchSuccess;
	constexpr bool bIsBound = true;

	// Store function pointer to the process function
	ProcessNodeFunction ProcessNode = nullptr;

	switch (QuerySettings.PathfindingAlgorithm)
	{
	case EPathfindingAlgorithm::AStar:
		ProcessNode = &FSVOPathfindingGraph::ProcessSingleAStarNode;
		break;
	case EPathfindingAlgorithm::LazyThetaStar:
		ProcessNode = &FSVOPathfindingGraph::ProcessSingleLazyThetaStarNode;
		break;
	case EPathfindingAlgorithm::ThetaStar:
		ProcessNode = &FSVOPathfindingGraph::ProcessSingleThetaStarNode;
	}

	bool bProcessNodes = true;
	const bool bIterationTimeout = QuerySettings.MaxIterations > 0;
	int32 NumIterations = 0;
	while (OpenList.Num() > 0 && bProcessNodes)
	{
		bProcessNodes = (this->*ProcessNode)(EndNodeRef, bIsBound, QuerySettings, Overrides, BestNodeIndex, BestNodeCost);
		if (bIterationTimeout)
		{
			NumIterations++;
			bProcessNodes &= NumIterations <= QuerySettings.MaxIterations;
		}
	}

	// check if we've reached the goal
	if (BestNodeCost != 0.f)
	{
		Result = EGraphAStarResult::GoalUnreachable;
	}

	// no point to waste perf creating the path if querier doesn't want it
	if (Result == EGraphAStarResult::SearchSuccess || QuerySettings.WantsPartialSolution())
	{
		// store the path. Note that it will be reversed!
		int32 SearchNodeIndex = BestNodeIndex;
		int32 PathLength = 0;
		do
		{
			PathLength++;
			SearchNodeIndex = NodePool[SearchNodeIndex].ParentNodeIndex;
		}
		while (NodePool.IsValidIndex(SearchNodeIndex) && NodePool[SearchNodeIndex].NodeRef != StartNodeRef && ensure(
			PathLength < FGraphAStarDefaultPolicy::FatalPathLength));

		if (PathLength >= FGraphAStarDefaultPolicy::FatalPathLength)
		{
			Result = EGraphAStarResult::InfiniteLoop;
		}

		OutPath.Reset(PathLength);
		OutPath.AddZeroed(PathLength);

		// store the path
		SearchNodeIndex = BestNodeIndex;
		int32 ResultNodeIndex = PathLength - 1;
		do
		{
			OutPath[ResultNodeIndex--] = NodePool[SearchNodeIndex].NodeRef;
			SearchNodeIndex = NodePool[SearchNodeIndex].ParentNodeIndex;
		}
		while (ResultNodeIndex >= 0);
	}

	return Result;
}

ENavigationQueryResult::Type FSVOPathfindingGraph::FindPath(const FVector& StartLocation,
                                                            const FVector& EndLocation,
                                                            const FSVOQuerySettings& QuerySettings,
                                                            TArray<FNavPathPoint>& PathPoints,
                                                            bool& bPartialSolution)
{
	const FSVOData& NavData = *QuerySettings.SVOData.Get();

	PathPoints.Reset();
	bPartialSolution = false;

	// Check start and end are not blocked
	const FSVOLink StartLink = Graph.SVOData->GetNodeLinkForPosition(StartLocation);
	const FSVOLink EndLink = Graph.SVOData->GetNodeLinkForPosition(EndLocation);

	if (!StartLink.IsValid() || !EndLink.IsValid())
	{
		return ENavigationQueryResult::Invalid;
	}

	// Same start-end link case
	if (StartLink == EndLink)
	{
		if ((StartLocation - EndLocation).IsNearlyZero())
		{
			// Same point, just endpoint
			PathPoints.Add(FNavPathPoint(EndLocation, EndLink.AsNavNodeRef()));
		}
		else
		{
			// Same box, straight line
			PathPoints.Add(FNavPathPoint(StartLocation, StartLink.AsNavNodeRef()));
			PathPoints.Add(FNavPathPoint(EndLocation, EndLink.AsNavNodeRef()));
		}

		return ENavigationQueryResult::Success;
	}

	// Early out for partial paths
	if (!QuerySettings.bAllowPartialPaths && !NavData.IsConnected(StartLink, EndLink))
	{
		return ENavigationQueryResult::Fail;
	}

#if WITH_EDITOR
	if (QuerySettings.bAllowPartialPaths)
	{
		printw("bAllowPartialPaths == true. Can cause very large pathfinding time.")
	}
#endif

	ENavigationQueryResult::Type Result = ENavigationQueryResult::Fail;

	// Add temporary locations for start and end nodes
	FOverrideNodes OverrideNodes;
	OverrideNodes.Add(StartLink, StartLocation);
	OverrideNodes.Add(EndLink, EndLocation);

	TArray<FSVOLink> LinkPath;
	const EGraphAStarResult PathResult = FindSVOPath(StartLink, EndLink, QuerySettings, OverrideNodes, LinkPath);
	bPartialSolution = PathResult == EGraphAStarResult::GoalUnreachable;

	// Return complete or partial solution
	if (PathResult == EGraphAStarResult::SearchSuccess || (QuerySettings.bAllowPartialPaths && bPartialSolution))
	{
		// Add start location
		PathPoints.Add(FNavPathPoint(StartLocation, StartLink.AsNavNodeRef()));
		for (const FSVOLink& Link : LinkPath)
		{
			const FVector PathPoint = NavData.GetPositionForLinkWithOverride(Link, OverrideNodes);
			PathPoints.Add(FNavPathPoint(PathPoint, Link.AsNavNodeRef()));
		}

		Result = ENavigationQueryResult::Success;
	}
	else if (PathResult == EGraphAStarResult::InfiniteLoop)
	{
		Result = ENavigationQueryResult::Error;
	}

	return Result;
}
