// Copyright Ben Sutherland 2024. All rights reserved.

#include "SVORaycast.h"
#include "FlyingNavSystemTypes.h"

FCoord GetFinalT(const FRayIntersect& I)
{
	// Entry plane is defined by maximum of I.t_0
	return FMath::Max3(I.tx0, I.ty0, I.tz0);
}

int32 FSVORaycast::FirstNode(const FRayIntersect& I)
{
	int32 Node = 0;
	// Entry plane is defined by maximum of I.t_0
	if (I.tx0 > I.ty0)
	{
		if (I.tx0 > I.tz0)
		{
			// Y-Z Entry Plane
			if (I.ty1 < I.tx0)
			{
				Node |= (1 << 1);
			}
			if (I.tz1 < I.tx0)
			{
				Node |= (1 << 2);
			}
			return Node;
		}
	}
	else
	{
		if (I.ty0 > I.tz0)
		{
			// X-Z Entry Plane
			if (I.tx1 < I.ty0)
			{
				Node |= (1 << 0);
			}
			if (I.tz1 < I.ty0)
			{
				Node |= (1 << 2);
			}
			return Node;
		}
	}

	// X-Y Entry Plane
	if (I.tx1 < I.tz0)
	{
		Node |= (1 << 0);
	}
	if (I.ty1 < I.tz0)
	{
		Node |= (1 << 1);
	}

	return Node;
}

int32 FSVORaycast::NextNode(const FCoord txM, const int32 X, const FCoord tyM, const int32 Y, const FCoord tzM, const int32 Z)
{
	// Find the minimum of the exit parameters (t_1 for the current node)
	if (txM < tyM)
	{
		if (txM < tzM)
		{
			// Y-Z exit plane
			return X;
		}
	}
	else
	{
		if (tyM < tzM)
		{
			// X-Z exit plane
			return Y;
		}
	}
	// X-Y exit plane
	return Z;
}

bool FSVORaycast::RayIntersectSubNode(const FRayIntersect& I, const int32 ChildIdx, const FSVOLink LeafLink) const
{
	if (!I.IsValid(MaxT))
	{
		return false;
	}
	
	const int32 NodeIdx = LeafLink.GetNodeIndex();
	const FSVOLeafNode& LeafNode = NavData->LeafLayer[NodeIdx];
	
	const FCoord txM = 0.5f * (I.tx0 + I.tx1);
	const FCoord tyM = 0.5f * (I.ty0 + I.ty1);
	const FCoord tzM = 0.5f * (I.tz0 + I.tz1);

	int32 CurrentChildIdx = FirstNode(FRayIntersect(I.tx0, txM, I.ty0, tyM, I.tz0, tzM));
	do
	{
		const int32 SubNodeIdx = (ChildIdx << 3) + (CurrentChildIdx ^ SignMask);

		switch (CurrentChildIdx)
		{
		case 0:
			{
				FRayIntersect SubNodeIntersect(I.tx0, txM, I.ty0, tyM, I.tz0, tzM);
				if (SubNodeIntersect.IsValid(MaxT) && LeafNode.IsIndexBlocked(SubNodeIdx) && SubNodeIntersect.Intersect())
				{
					FinalT = GetFinalT(SubNodeIntersect);
					return true;
				}
				CurrentChildIdx = NextNode(txM, 1, tyM, 2, tzM, 4);
				break;
			}
		case 1:
			{
				FRayIntersect SubNodeIntersect(txM, I.tx1, I.ty0, tyM, I.tz0, tzM);
				if (SubNodeIntersect.IsValid(MaxT) && LeafNode.IsIndexBlocked(SubNodeIdx) && SubNodeIntersect.Intersect())
				{
					FinalT = GetFinalT(SubNodeIntersect);
					return true;
				}
				CurrentChildIdx = NextNode(I.tx1, 8, tyM, 3, tzM, 5);
				break;
			}
		case 2:
			{
				FRayIntersect SubNodeIntersect(I.tx0, txM, tyM, I.ty1, I.tz0, tzM);
				if (SubNodeIntersect.IsValid(MaxT) && LeafNode.IsIndexBlocked(SubNodeIdx) && SubNodeIntersect.Intersect())
				{
					FinalT = GetFinalT(SubNodeIntersect);
					return true;
				}
				CurrentChildIdx = NextNode(txM, 3, I.ty1, 8, tzM, 6);
				break;
			}
		case 3:
			{
				FRayIntersect SubNodeIntersect(txM, I.tx1, tyM, I.ty1, I.tz0, tzM);
				if (SubNodeIntersect.IsValid(MaxT) && LeafNode.IsIndexBlocked(SubNodeIdx) && SubNodeIntersect.Intersect())
				{
					FinalT = GetFinalT(SubNodeIntersect);
					return true;
				}
				CurrentChildIdx = NextNode(I.tx1, 8, I.ty1, 8, tzM, 7);
				break;
			}
		case 4:
			{
				FRayIntersect SubNodeIntersect(I.tx0, txM, I.ty0, tyM, tzM, I.tz1);
				if (SubNodeIntersect.IsValid(MaxT) && LeafNode.IsIndexBlocked(SubNodeIdx) && SubNodeIntersect.Intersect())
				{
					FinalT = GetFinalT(SubNodeIntersect);
					return true;
				}
				CurrentChildIdx = NextNode(txM, 5, tyM, 6, I.tz1, 8);
				break;
			}
		case 5:
			{
				FRayIntersect SubNodeIntersect(txM, I.tx1, I.ty0, tyM, tzM, I.tz1);
				if (SubNodeIntersect.IsValid(MaxT) && LeafNode.IsIndexBlocked(SubNodeIdx) && SubNodeIntersect.Intersect())
				{
					FinalT = GetFinalT(SubNodeIntersect);
					return true;
				}
				CurrentChildIdx = NextNode(I.tx1, 8, tyM, 7, I.tz1, 8);
				break;
			}
		case 6:
			{
				FRayIntersect SubNodeIntersect(I.tx0, txM, tyM, I.ty1, tzM, I.tz1);
				if (SubNodeIntersect.IsValid(MaxT) && LeafNode.IsIndexBlocked(SubNodeIdx) && SubNodeIntersect.Intersect())
				{
					FinalT = GetFinalT(SubNodeIntersect);
					return true;
				}
				CurrentChildIdx = NextNode(txM, 7, I.ty1, 8, I.tz1, 8);
				break;
			}
		case 7:
			{
				FRayIntersect SubNodeIntersect(txM, I.tx1, tyM, I.ty1, tzM, I.tz1);
				if (SubNodeIntersect.IsValid(MaxT) && LeafNode.IsIndexBlocked(SubNodeIdx) && SubNodeIntersect.Intersect())
				{
					FinalT = GetFinalT(SubNodeIntersect);
					return true;
				}
				CurrentChildIdx = 8;
				break;
			}
		default:
			check(false)
            CurrentChildIdx = 8;
			break;
		}
	}
	while (CurrentChildIdx < 8);
	
	return false;
}

// Returns if ray intersects blocking voxel in this node
bool FSVORaycast::RayIntersectNode(const FRayIntersect& I, const FSVOLink NodeLink) const
{
	if (!I.IsValid(MaxT))
	{
		return false;
	}

	const FCoord txM = 0.5f * (I.tx0 + I.tx1);
	const FCoord tyM = 0.5f * (I.ty0 + I.ty1);
	const FCoord tzM = 0.5f * (I.tz0 + I.tz1);

	const int32 LayerIdx = NodeLink.GetLayerIndex();
	const int32 NodeIdx = NodeLink.GetNodeIndex();
	
	if (LayerIdx == 0)
	{
		const FSVOLeafNode& LeafNode = NavData->LeafLayer[NodeIdx];
		if (LeafNode.IsCompletelyFree())
		{
			return false;
		}
		
		int32 CurrentChildIdx = FirstNode(FRayIntersect(I.tx0, txM, I.ty0, tyM, I.tz0, tzM));

		do
		{
			switch (CurrentChildIdx)
			{
			case 0:
				if (RayIntersectSubNode(FRayIntersect(I.tx0, txM, I.ty0, tyM, I.tz0, tzM), SignMask, NodeLink))
				{
					return true;
				}
				CurrentChildIdx = NextNode(txM, 1, tyM, 2, tzM, 4);
				break;
			case 1:
				if (RayIntersectSubNode(FRayIntersect(txM, I.tx1, I.ty0, tyM, I.tz0, tzM), (1^SignMask), NodeLink))
				{
					return true;
				}
				CurrentChildIdx = NextNode(I.tx1, 8, tyM, 3, tzM, 5);
				break;
			case 2:
				if (RayIntersectSubNode(FRayIntersect(I.tx0, txM, tyM, I.ty1, I.tz0, tzM), (2^SignMask), NodeLink))
				{
					return true;
				}
				CurrentChildIdx = NextNode(txM, 3, I.ty1, 8, tzM, 6);
				break;
			case 3:
				if (RayIntersectSubNode(FRayIntersect(txM, I.tx1, tyM, I.ty1, I.tz0, tzM), (3^SignMask), NodeLink))
				{
					return true;
				}
				CurrentChildIdx = NextNode(I.tx1, 8, I.ty1, 8, tzM, 7);
				break;
			case 4:
				if (RayIntersectSubNode(FRayIntersect(I.tx0, txM, I.ty0, tyM, tzM, I.tz1), (4^SignMask), NodeLink))
				{
					return true;
				}
				CurrentChildIdx = NextNode(txM, 5, tyM, 6, I.tz1, 8);
				break;
			case 5:
				if (RayIntersectSubNode(FRayIntersect(txM, I.tx1, I.ty0, tyM, tzM, I.tz1),  (5^SignMask), NodeLink))
				{
					return true;
				}
				CurrentChildIdx = NextNode(I.tx1, 8, tyM, 7, I.tz1, 8);
				break;
			case 6:
				if (RayIntersectSubNode(FRayIntersect(I.tx0, txM, tyM, I.ty1, tzM, I.tz1), (6^SignMask), NodeLink))
				{
					return true;
				}
				CurrentChildIdx = NextNode(txM, 7, I.ty1, 8, I.tz1, 8);
				break;
			case 7:
				if (RayIntersectSubNode(FRayIntersect(txM, I.tx1, tyM, I.ty1, tzM, I.tz1), (7^SignMask), NodeLink))
				{
					return true;
				}
				CurrentChildIdx = 8;
				break;
			default:
				check(false)
				CurrentChildIdx = 8;
				break;
			}
		}
		while (CurrentChildIdx < 8);

		return false;
	}

	const FSVONode& Node = NavData->GetLayer(LayerIdx)[NodeIdx];
	if (!Node.bHasChildren)
	{
		// All nodes without children don't have blocking geometry
		return false;
	}
	
	// Recurse to children
	const FSVOLink& FirstChild = Node.FirstChild;

	int32 CurrentChildIdx = FirstNode(FRayIntersect(I.tx0, txM, I.ty0, tyM, I.tz0, tzM));
	do
	{
		switch (CurrentChildIdx)
		{
		case 0:
			if (RayIntersectNode(FRayIntersect(I.tx0, txM, I.ty0, tyM, I.tz0, tzM), FirstChild + SignMask))
			{
				return true;
			}
			CurrentChildIdx = NextNode(txM, 1, tyM, 2, tzM, 4);
			break;
		case 1:
			if (RayIntersectNode(FRayIntersect(txM, I.tx1, I.ty0, tyM, I.tz0, tzM), FirstChild + (1^SignMask)))
			{
				return true;
			}
			CurrentChildIdx = NextNode(I.tx1, 8, tyM, 3, tzM, 5);
			break;
		case 2:
			if (RayIntersectNode(FRayIntersect(I.tx0, txM, tyM, I.ty1, I.tz0, tzM), FirstChild + (2^SignMask)))
			{
				return true;
			}
			CurrentChildIdx = NextNode(txM, 3, I.ty1, 8, tzM, 6);
			break;
		case 3:
			if (RayIntersectNode(FRayIntersect(txM, I.tx1, tyM, I.ty1, I.tz0, tzM), FirstChild + (3^SignMask)))
			{
				return true;
			}
			CurrentChildIdx = NextNode(I.tx1, 8, I.ty1, 8, tzM, 7);
			break;
		case 4:
			if (RayIntersectNode(FRayIntersect(I.tx0, txM, I.ty0, tyM, tzM, I.tz1), FirstChild + (4^SignMask)))
			{
				return true;
			}
			CurrentChildIdx = NextNode(txM, 5, tyM, 6, I.tz1, 8);
			break;
		case 5:
			if (RayIntersectNode(FRayIntersect(txM, I.tx1, I.ty0, tyM, tzM, I.tz1), FirstChild + (5^SignMask)))
			{
				return true;
			}
			CurrentChildIdx = NextNode(I.tx1, 8, tyM, 7, I.tz1, 8);
			break;
		case 6:
			if (RayIntersectNode(FRayIntersect(I.tx0, txM, tyM, I.ty1, tzM, I.tz1), FirstChild + (6^SignMask)))
			{
				return true;
			}
			CurrentChildIdx = NextNode(txM, 7, I.ty1, 8, I.tz1, 8);
			break;
		case 7:
			if (RayIntersectNode(FRayIntersect(txM, I.tx1, tyM, I.ty1, tzM, I.tz1), FirstChild + (7^SignMask)))
			{
				return true;
			}
			CurrentChildIdx = 8;
			break;
		default:
			check(false)
			CurrentChildIdx = 8;
			break;
		}
	}
	while (CurrentChildIdx < 8);
	
	return false;
}
