// Copyright Ben Sutherland 2024. All rights reserved.

#include "OctreeRenderingComponent.h"
#include "FlyingNavigationData.h"

#if ATLEAST_UE5_2
#include "MaterialDomain.h"
#include "SceneInterface.h"
#include "Engine/GameViewportClient.h"
#endif // ATLEAST_UE5_2

#include "Engine/CollisionProfile.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

#if WITH_EDITOR
#include "Editor.h"
#include "EditorViewportClient.h"
#endif

// Constants
static const float NeighbourLineThickness = 1.f;

#define NUM_CUBE_VERTS 12
#define NUM_CUBE_TRIANGLES 12
#define NUM_CUBE_INDICES NUM_CUBE_TRIANGLES*3
#define CUBE_EXTENT_OFFSET 0.05f

//----------------------------------------------------------------------//
// FOctreeSceneProxyData Implementation
//----------------------------------------------------------------------//
#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
struct FVertex
{
	FVector Pos;
	FVector2D TexCoord;

	FVertex(const FVector& Pos, const FVector2D& TexCoord): Pos(Pos), TexCoord(TexCoord) {}
};
struct FTriangleIndices
{
	uint32 V0, V1, V2;
	FTriangleIndices(const uint32 V0, const uint32 V1, const uint32 V2): V0(V0), V1(V1), V2(V2) {}
};

// Tex coords require duplication of verts
FVertex CubeVerts[] = {
	FVertex(FVector(-1.f,-1.f,-1.f),FVector2D(0.f, 0.f)), // 0
	FVertex(FVector(-1.f,-1.f, 1.f),FVector2D(1.f, 0.f)), // 1
	FVertex(FVector(-1.f, 1.f,-1.f),FVector2D(0.f, 1.f)), // 2
	FVertex(FVector(-1.f, 1.f, 1.f),FVector2D(1.f, 1.f)), // 3
	
	FVertex(FVector( 1.f,-1.f,-1.f),FVector2D(0.f, 1.f)), // 4
	FVertex(FVector( 1.f,-1.f, 1.f),FVector2D(1.f, 1.f)), // 5
	FVertex(FVector( 1.f, 1.f,-1.f),FVector2D(0.f, 0.f)), // 6
	FVertex(FVector( 1.f, 1.f, 1.f),FVector2D(1.f, 0.f)), // 7

	FVertex(FVector(-1.f, 1.f,-1.f),FVector2D(1.f, 0.f)), // 8
	FVertex(FVector(-1.f, 1.f, 1.f),FVector2D(0.f, 0.f)), // 9
	FVertex(FVector( 1.f, 1.f,-1.f),FVector2D(1.f, 1.f)), // 10
	FVertex(FVector( 1.f, 1.f, 1.f),FVector2D(0.f, 1.f))  // 11
};

FTriangleIndices CubeIndices[] = {
	// Back face
	FTriangleIndices(0, 1, 3),
	FTriangleIndices(0, 3, 2),
	// Left Face
	FTriangleIndices(5, 0, 4),
	FTriangleIndices(5, 1, 0),
	// Front face
	FTriangleIndices(5, 4, 6),
    FTriangleIndices(5, 6, 7),
	
	// Top Face
	FTriangleIndices(1, 5, 11),
	FTriangleIndices(1, 11, 9),
	// Right face
	FTriangleIndices(9, 11, 10),
	FTriangleIndices(9, 10, 8),
	// Bottom face
	FTriangleIndices(4, 0, 8),
	FTriangleIndices(4, 8, 10)
};

// Subclass for quick transform without rotation
struct FTransformNoRot : FTransform
{
	FORCEINLINE FTransformNoRot(const FVector& InTranslation, const FVector& InScale3D):
		FTransform(FQuat::Identity, InTranslation, InScale3D)
	{}
#if ENABLE_VECTORIZED_TRANSFORM

	FORCEINLINE FVector TransformPositionNoRotation(const FVector& V) const
	{
		DiagnosticCheckNaN_All();

		// UE5 Note: VectorRegister, VectorLoadFloat3_W0 etc. use doubles
		const VectorRegister InputVectorW0 = VectorLoadFloat3_W0(&V);
		const VectorRegister ScaledVec = VectorMultiply(Scale3D, InputVectorW0);
		const VectorRegister TranslatedVec = VectorAdd(ScaledVec, Translation);

		FVector Result;
		VectorStoreFloat3(TranslatedVec, &Result);
		return Result;
	}
#else // ENABLE_VECTORIZED_TRANSFORM
	FORCEINLINE FVector TransformPositionNoRotation(const FVector& V) const
	{
		DiagnosticCheckNaN_All();

		return Scale3D*V + Translation;
	}
#endif // ENABLE_VECTORIZED_TRANSFORM
};

// TODO: Draw subnode neighbours without subnodes
bool HasFlag(const int32 Flags, EFlyingDataDisplayFlags TestFlag)
{
	return (Flags & (1 << static_cast<int32>(TestFlag))) != 0;
}

void FOctreeSceneProxyData::GatherNeighbourLines(const FSVOLink NodeLink, const FVector& VoxelCentre, const bool bSimplified, const float CentreRadius, const FColor& Color, const FSVOData& NavData, const FSVOGraph& NavGraph)
{
	// Neighbours
	const FCoord SubNodeSideLength = NavData.SubNodeSideLength;
	const FCoord LeafSideLength = SubNodeSideLength * 4.f;
	const FCoord LeafOffset = NavData.GetNodeOffsetForLayer(0);
	const FCoord SubNodeOffset = NavData.GetSubNodeOffset();
	
	const int32 LayerIdx = NodeLink.GetLayerIndex();
	const FCoord VoxelSideLength = LeafSideLength * (1 << LayerIdx);
	const FSVOLeafLayer& LeafLayer = NavData.LeafLayer;

	const bool bCurrentNodeIsSubNode = NodeLink.GetLayerIndex() == 0 && !LeafLayer[NodeLink.GetNodeIndex()].IsCompletelyFree();
	
	TArray<FSVOLink> Neighbours;
	NavGraph.GetNeighbours(NodeLink, Neighbours);
	for (const FSVOLink& Neighbour : Neighbours)
	{
		if (!Neighbour.IsValid()) { continue; }
		
		const int32 NeighbourLayerIdx = Neighbour.GetLayerIndex();
		const int32 NeighbourNodeIdx = Neighbour.GetNodeIndex();

		if (NeighbourLayerIdx == 0)
		{
			const FSVOLeafNode& Leaf = LeafLayer[NeighbourNodeIdx];
			const FSVONode& LeafParent = NavData.GetLayer(1).GetNode(Leaf.Parent.GetNodeIndex());
			const int32 ChildIdx = NeighbourNodeIdx - LeafParent.FirstChild.GetNodeIndex();
			const morton_t LeafMortonCode = FlyingNavSystem::FirstChildFromParent(LeafParent.MortonCode) + ChildIdx;
			
			FVector LeafCentre = FlyingNavSystem::MortonToCoord(LeafMortonCode, NavData.Centre, LeafSideLength, LeafOffset);
			
			if (Leaf.IsCompletelyFree())
			{
				const FVector NeighbourDirection = FlyingNavSystem::MajorAxisDirectionFromTwoPoints(VoxelCentre, LeafCentre);
				// If the current node is a SubNode
				if (bCurrentNodeIsSubNode)
				{
					const FVector NeighbourCentre = VoxelCentre + NeighbourDirection * SubNodeSideLength * 0.5f;
					NeighbourLines.Add(FDebugRenderSceneProxy::FDebugLine(VoxelCentre, NeighbourCentre, Color, NeighbourLineThickness));
				} else
				{
					FVector StartPoint = VoxelCentre;
					if (bSimplified)
					{
						LeafCentre = VoxelCentre + NeighbourDirection * VoxelSideLength * 0.5;
						StartPoint += NeighbourDirection * CentreRadius;
					} else
					{
						LeafCentre -= NeighbourDirection * LeafSideLength * 0.5;
						StartPoint += (LeafCentre - VoxelCentre).GetSafeNormal() * CentreRadius;
					}
					NeighbourLines.Add(FDebugRenderSceneProxy::FDebugLine(StartPoint, LeafCentre, Color, NeighbourLineThickness));
				}
			} else if (!Leaf.IsIndexBlocked(Neighbour.GetSubNodeIndex()))
			{
				FVector SubNodeCentre = FlyingNavSystem::SmallMortonToCoord(Neighbour.GetSubNodeIndex(), LeafCentre, SubNodeSideLength, SubNodeOffset);
				const FVector NeighbourDirection = FlyingNavSystem::MajorAxisDirectionFromTwoPoints(VoxelCentre, SubNodeCentre);
				FVector StartPoint = VoxelCentre;
				if (bCurrentNodeIsSubNode)
				{
					SubNodeCentre = VoxelCentre + NeighbourDirection * SubNodeSideLength * 0.5f;
				} else
				{
					if (bSimplified)
					{
						SubNodeCentre = VoxelCentre + NeighbourDirection * VoxelSideLength * 0.5;
						StartPoint = VoxelCentre + NeighbourDirection * CentreRadius;
					} else
					{
						SubNodeCentre -= NeighbourDirection * SubNodeSideLength * 0.5f;
						StartPoint = VoxelCentre + (SubNodeCentre - VoxelCentre).GetSafeNormal() * CentreRadius;
					}
				}
				NeighbourLines.Add(FDebugRenderSceneProxy::FDebugLine(StartPoint, SubNodeCentre, Color, NeighbourLineThickness));
			}
		} else
		{
			const FSVONode& NeighbourNode = NavData.GetLayer(NeighbourLayerIdx)[NeighbourNodeIdx];
			const FCoord NeighbourSideLength = LeafSideLength * (1 << NeighbourLayerIdx);
			const FCoord NeighbourOffset = NavData.GetNodeOffsetForLayer(NeighbourLayerIdx);
			
			FVector NeighbourCentre = FlyingNavSystem::MortonToCoord(NeighbourNode.MortonCode, NavData.Centre, NeighbourSideLength, NeighbourOffset);

			const FVector NeighbourDirection = FlyingNavSystem::MajorAxisDirectionFromTwoPoints(VoxelCentre, NeighbourCentre);

			FVector StartPoint = VoxelCentre;
			if (LayerIdx < NeighbourLayerIdx)
			{
				if (bCurrentNodeIsSubNode)
				{
					NeighbourCentre = VoxelCentre + NeighbourDirection * SubNodeSideLength * 0.5f;
				} else
				{
					NeighbourCentre = VoxelCentre + NeighbourDirection * VoxelSideLength * 0.5f;
					StartPoint += NeighbourDirection * CentreRadius;
				}
			} else if (LayerIdx >= NeighbourLayerIdx)
			{
				if (bSimplified)
				{
					NeighbourCentre = VoxelCentre + NeighbourDirection * VoxelSideLength * 0.5f;
					StartPoint += NeighbourDirection * CentreRadius;
				} else
				{
					NeighbourCentre -= NeighbourDirection * NeighbourSideLength * 0.5f;
					StartPoint += (NeighbourCentre - VoxelCentre).GetSafeNormal() * CentreRadius;
				}
			}
			
			NeighbourLines.Add(FDebugRenderSceneProxy::FDebugLine(StartPoint, NeighbourCentre, Color, NeighbourLineThickness));
		}
	}
}

// Red = 0.f, Blue = 1.f
FColor MakeRedToBlueColorFromScalar(float Scalar)
{
	const float RedProp = FMath::Clamp<float>((1.0f - Scalar)/0.5f,0.f,1.f);
	const float BlueProp = FMath::Clamp<float>((Scalar/0.5f),0.f,1.f);
	const int32 R = FMath::TruncToInt(255 * RedProp);
	const int32 G = 0;
	const int32 B = FMath::TruncToInt(255 * BlueProp);
	return FColor(R, G, B);
}
FColor GetComplement(const FColor& Colour)
{
	// TODO: Colours are rubbish
	FLinearColor HSV = Colour.ReinterpretAsLinear().LinearRGBToHSV();
	HSV.R += 180.f; // Opposite side of colour wheel
	if (HSV.R >= 360.f)
	{
		HSV.R -= 360.f;
	}
	HSV.G = 0.8f;
	HSV.B = 0.5f;
	return HSV.HSVToLinearRGB().ToFColor(true);
}

void FOctreeSceneProxyData::GatherData(const AFlyingNavigationData* FlyingNavData, int32 InNavDetailFlags)
{
	FRWScopeLock Lock(FlyingNavData->SVODataLock, SLT_ReadOnly);
	const FSVOData& NavData = FlyingNavData->GetSVOData();
	
	Reset();

	const bool bShowNavigation = FlyingNavData != nullptr 
#if WITH_EDITORONLY_DATA
		&& (!FlyingNavData->GetWorld()->IsGameWorld() || FlyingNavData->bAllowDrawingInGameWorld)
#endif
	;
	
	NavDetailFlags = InNavDetailFlags;
	if (bShowNavigation && NavDetailFlags && NavData.bValid)
	{
		bNeedsNewData = false;
		bDataGathered = true;

		TArray<FSVOBox> NodeBoxes;
		
		const FSVOGraph& NavGraph(NavData);
		const FVector ExtentMargin = FVector(FlyingNavData->NodeMargin);
		const FCoord LeafSideLength = NavData.GetSideLengthForLayer(0);
		
		const FVector LeafExtent = FVector(LeafSideLength * 0.5f) - ExtentMargin;
		const FVector SubNodeExtent = NavData.GetSubNodeExtent() - ExtentMargin;
		
		const FCoord LeafOffset = NavData.GetNodeOffsetForLayer(0);
		const FCoord SubNodeOffset = NavData.GetSubNodeOffset();

		const bool bDrawNodeBoxes = HasFlag(NavDetailFlags, EFlyingDataDisplayFlags::NodeBoxes);
		const bool bDrawSubNodeBoxes = HasFlag(NavDetailFlags, EFlyingDataDisplayFlags::SubNodeBoxes);
		const bool bDrawOnlyOverlappedSubNodes = HasFlag(NavDetailFlags, EFlyingDataDisplayFlags::OnlyOverlappedSubNodes);
		const bool bDrawNeighbours = HasFlag(NavDetailFlags, EFlyingDataDisplayFlags::NeighbourConnections);
		const bool bDrawSimplifiedNeighbours = HasFlag(NavDetailFlags, EFlyingDataDisplayFlags::SimplifiedConnections);
		const bool bColourByConnected = HasFlag(NavDetailFlags, EFlyingDataDisplayFlags::ColourConnected);

		if (!bDrawNodeBoxes && !bDrawSubNodeBoxes && !bDrawNeighbours)
		{
			return;
		}
		
		FlyingNavData->BeginBatchQuery();

		// Common function for finding colour for node
		TMap<int32, float> GroupColours;
		const float NumComponentsInv = 1.f / static_cast<float>(NavData.NumConnectedComponents + 1);
		auto ColourForNode = [&](const float LayerProp, const FSVOLink NodeLink) -> FColor
		{
			const float ColourProp = 1.f - LayerProp;
			if (bColourByConnected)
			{
				const int32 NodeComponentIndex = NavData.GetComponentIndex(NodeLink);
				if (!GroupColours.Contains(NodeComponentIndex))
				{
					const float Hue = static_cast<float>(GroupColours.Num() + 1) * NumComponentsInv;
					GroupColours.Add(NodeComponentIndex, Hue);
				}
				const float Hue = GroupColours[NodeComponentIndex] * 360.f;
				const float Sat = 1.f;
				const float Val = 0.9f; //FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f), FVector2D(0.5, 1.f), ColourProp);
				return FLinearColor(Hue,  Sat, Val).HSVToLinearRGB().ToFColor(true);
			} else
			{
				return MakeRedToBlueColorFromScalar(ColourProp);
			}
		};

		const float NumLayersInv = 1.f / static_cast<float>(NavData.NumNodeLayers);
		
		// Draw leaf nodes:
		if (bDrawNodeBoxes || bDrawNeighbours || bDrawSubNodeBoxes)
		{
			for (const FSVONode& LayerOneNode : NavData.GetLayer(1).Nodes)
			{
				if (LayerOneNode.bHasChildren)
				{
					for (int32 LeafIdx = 0; LeafIdx < 8; LeafIdx++)
					{
						// Coordinate
						const morton_t LeafMortonCode = FlyingNavSystem::FirstChildFromParent(LayerOneNode.MortonCode) + LeafIdx;
						const FVector LeafCentre = FlyingNavSystem::MortonToCoord(LeafMortonCode, NavData.Centre, LeafSideLength, LeafOffset);

						const FSVOLink& LeafLink = LayerOneNode.FirstChild + LeafIdx;
						const int32 NodeIdx = LeafLink.GetNodeIndex();
						const FSVOLeafNode& LeafNode = NavData.LeafLayer[NodeIdx];
					
						if (LeafNode.IsCompletelyFree())
						{
							const FColor NodeColour = ColourForNode(NumLayersInv, LeafLink);
							if (bDrawNodeBoxes)
							{
								NodeBoxes.Add(FSVOBox(LeafCentre, LeafExtent, NodeColour));
							}
							if (bDrawNeighbours)
							{
								const FColor NeighbourColor = GetComplement(NodeColour);
								GatherNeighbourLines(LeafLink, LeafCentre, bDrawSimplifiedNeighbours, FlyingNavData->NodeCentreRadius, NeighbourColor, NavData, NavGraph);
								NodeSpheres.Add(FDebugRenderSceneProxy::FSphere(FlyingNavData->NodeCentreRadius, LeafCentre, NeighbourColor));
							}
						} else
						{
							// Gather SubNodes
							const FSVOLeafNode& Leaf = NavData.LeafLayer[NodeIdx];
							for (int32 i = 0; i < 64; i++)
							{
								const FVector SubNodeCentre = FlyingNavSystem::SmallMortonToCoord(static_cast<small_morton_t>(i), LeafCentre, NavData.SubNodeSideLength, SubNodeOffset);
								const bool bOverlap = Leaf.IsIndexBlocked(i);

								FColor NodeColour;
								if (bOverlap)
								{
									NodeColour = FlyingNavData->GetConfig().Color;
								} else
								{
									NodeColour = ColourForNode(0.f, FSVOLink(0, NodeIdx, i));
								}
							
								if ((bOverlap || !bDrawOnlyOverlappedSubNodes) && bDrawSubNodeBoxes)
								{
									NodeBoxes.Add(FSVOBox(SubNodeCentre, SubNodeExtent, NodeColour));
								}

								// Only draw subnode connections if subnode drawing is enabled
								if (!bOverlap && bDrawNeighbours && !bDrawOnlyOverlappedSubNodes)
								{
									const FColor NeighbourColor = GetComplement(NodeColour);
									const FSVOLink SubNodeLink = FSVOLink(0, LeafLink.GetNodeIndex(), i);
									GatherNeighbourLines(SubNodeLink, SubNodeCentre, bDrawSimplifiedNeighbours, FlyingNavData->NodeCentreRadius, NeighbourColor, NavData, NavGraph);
								}
							}
						}
					}
				}
			}
		}

		// Draw other layers
		if (bDrawNodeBoxes || bDrawNeighbours)
		{
			for (int32 LayerIdx = 1; LayerIdx < NavData.NumNodeLayers; LayerIdx++)
			{
				const FCoord VoxelSideLength = LeafSideLength * (1 << LayerIdx);
				const FVector VoxelExtent = NavData.GetExtentForLayer(LayerIdx) - ExtentMargin;
				const FCoord VoxelOffset = NavData.GetNodeOffsetForLayer(LayerIdx);
				const FCoord LayerProp = static_cast<FCoord>(LayerIdx + 1) * NumLayersInv;

				for (int32 NodeIndex = 0; NodeIndex < NavData.GetLayer(LayerIdx).Num(); NodeIndex++)
				{
					const FSVONode& LayerNode = NavData.GetLayer(LayerIdx).GetNode(NodeIndex);
					const FSVOLink CurrentNodeLink(LayerIdx, NodeIndex, 0);
				
					const FVector VoxelCentre = FlyingNavSystem::MortonToCoord(LayerNode.MortonCode, NavData.Centre, VoxelSideLength, VoxelOffset);
				
					if (!LayerNode.bHasChildren)
					{
						const FColor NodeColour = LayerNode.bBlocked ? FlyingNavData->GetConfig().Color : ColourForNode(LayerProp, CurrentNodeLink);
						if (bDrawNodeBoxes)
						{
							NodeBoxes.Add(FSVOBox(VoxelCentre, VoxelExtent, NodeColour));
						}
					
						// Draw Neighbour lines
						if (bDrawNeighbours)
						{
							const FColor NeighbourColor = GetComplement(NodeColour);
							GatherNeighbourLines(CurrentNodeLink, VoxelCentre, bDrawSimplifiedNeighbours, FlyingNavData->NodeCentreRadius, NeighbourColor, NavData, NavGraph);
							NodeSpheres.Add(FDebugRenderSceneProxy::FSphere(FlyingNavData->NodeCentreRadius, VoxelCentre, NeighbourColor));
						}
					}
				}
			}
		}

		// Draw Placeholder root
		if (NavData.IsEmptySpace() && bDrawNodeBoxes)
		{
			NodeBoxes.Add(FSVOBox(NavData.Centre, NavData.GetOctreeExtent(), FColor::Purple));
		}
		
		FlyingNavData->FinishBatchQuery();

		//******************************************
		// Build render data
		const int64 NumBoxes = NodeBoxes.Num();
		const int64 NumVerts = NumBoxes * NUM_CUBE_VERTS;     // 12 verts per box
		const int64 NumIndices = NumBoxes * NUM_CUBE_INDICES; // 36 indices per box
		
		Vertices.AddUninitialized(NumVerts);
		Indices.AddUninitialized(NumIndices);
		
		// Add each triangle to the vertex/index buffer
		for(int32 BoxIdx = 0; BoxIdx < NumBoxes; BoxIdx++)
		{
			const FSVOBox& Box = NodeBoxes[BoxIdx];
			const FTransformNoRot BoxTransform(Box.Centre,FVector(Box.Extent - CUBE_EXTENT_OFFSET));

			// Copy over transformed vertices
			FDynamicMeshVertex Vert;
			Vert.Color = Box.Colour;
			
			for (int32 i = 0; i < NUM_CUBE_VERTS; i++)
			{
				// TODO: LWC precision loss
				Vert.Position = FVector3f(BoxTransform.TransformPositionNoRotation(CubeVerts[i].Pos));
				Vert.TextureCoordinate[0] = FVector2f(CubeVerts[i].TexCoord);
				Vertices[BoxIdx * NUM_CUBE_VERTS + i] = Vert;
			}

			// Copy over indices
			for (int32 i = 0; i < NUM_CUBE_TRIANGLES; i++)
			{
				const FTriangleIndices& Tri = CubeIndices[i];
				const int32 BaseIdx = BoxIdx * NUM_CUBE_INDICES + i * 3;
				
				Indices[BaseIdx + 0] = BoxIdx * NUM_CUBE_VERTS + Tri.V0;
				Indices[BaseIdx + 1] = BoxIdx * NUM_CUBE_VERTS + Tri.V1;
				Indices[BaseIdx + 2] = BoxIdx * NUM_CUBE_VERTS + Tri.V2;
			}
		}
	}
}

int32 FOctreeSceneProxyData::GetDetailFlags(const AFlyingNavigationData* FlyingNavData)
{
	if (FlyingNavData == nullptr)
	{
		return 0;
	}

	return (FlyingNavData->bDrawOctreeNodes ? (1 << static_cast<int32>(EFlyingDataDisplayFlags::NodeBoxes)) : 0) |
		   (FlyingNavData->bDrawOctreeSubNodes ? (1 << static_cast<int32>(EFlyingDataDisplayFlags::SubNodeBoxes)) : 0) |
		   (FlyingNavData->bDrawOnlyOverlappedSubNodes ? (1 << static_cast<int32>(EFlyingDataDisplayFlags::OnlyOverlappedSubNodes)) : 0) |
		   (FlyingNavData->bDrawNeighbourConnections ? (1 << static_cast<int32>(EFlyingDataDisplayFlags::NeighbourConnections)) : 0) |
		   (FlyingNavData->bDrawSimplifiedConnections ? (1 << static_cast<int32>(EFlyingDataDisplayFlags::SimplifiedConnections)) : 0) |
		   (FlyingNavData->bColourByConnected ? (1 << static_cast<int32>(EFlyingDataDisplayFlags::ColourConnected)) : 0);
}

#endif // !UE_BUILD_SHIPPING && !UE_BUILD_TEST

void FOctreeSceneProxyData::Reset()
{
	//NodeBoxes.Reset();
	NeighbourLines.Reset();
	NodeSpheres.Reset();
	Vertices.Reset();
	Indices.Reset();

	bNeedsNewData = true;
	bDataGathered = false;
	NavDetailFlags = 0;
}

/*
void FOctreeSceneProxyData::Serialize(FArchive& Ar)
{
	// Serialize lines
	int32 NumItems = NeighbourLines.Num();
	Ar << NumItems;
	if (Ar.IsLoading())
	{
		NeighbourLines.Reset(NumItems);
		NeighbourLines.AddUninitialized(NumItems);
	}

	for (int32 Idx = 0; Idx < NumItems; Idx++)
	{
		Ar << NeighbourLines[Idx].Thickness;
		Ar << NeighbourLines[Idx].Start;
		Ar << NeighbourLines[Idx].End;
		Ar << NeighbourLines[Idx].Color;
	}

	// Serialize Spheres
	int32 NumSpheres = NodeSpheres.Num();
	Ar << NumSpheres;
	if (Ar.IsLoading())
	{
		NodeSpheres.Reset(NumSpheres);
		NodeSpheres.AddUninitialized(NumSpheres);
	}

	for (int32 Idx = 0; Idx < NumSpheres; Idx++)
	{
		Ar << NodeSpheres[Idx].Radius;
		Ar << NodeSpheres[Idx].Location;
		Ar << NodeSpheres[Idx].Color;
	}

	// Serialize boxes
	int32 NumBoxes = NodeBoxes.Num();
	Ar << NumBoxes;
	if (Ar.IsLoading())
	{
		FSVOBox TmpBox = FSVOBox();
		NodeBoxes.Reserve(NumBoxes);
		for (int32 Idx = 0; Idx < NumBoxes; Idx++)
		{
			Ar << TmpBox.Centre;
			Ar << TmpBox.Extent;
			Ar << TmpBox.Colour;

			NodeBoxes.Add(TmpBox);
		}
	}
	else
	{
		for (int32 Idx = 0; Idx < NumBoxes; Idx++)
		{
			Ar << NodeBoxes[Idx].Centre;
			Ar << NodeBoxes[Idx].Extent;
			Ar << NodeBoxes[Idx].Colour;
		}
	}

	Ar << NavDetailFlags;

	int32 BitFlags = ((bDataGathered ? 1 : 0) << 0) | ((bNeedsNewData ? 1 : 0) << 1);
	Ar << BitFlags;
	bDataGathered = (BitFlags & (1 << 0)) != 0;
	bNeedsNewData = (BitFlags & (1 << 1)) != 0;
}
*/

uint32 FOctreeSceneProxyData::GetAllocatedSize() const
{
	return NodeSpheres.GetAllocatedSize() +
           NeighbourLines.GetAllocatedSize() +
           Vertices.GetAllocatedSize() +
           Indices.GetAllocatedSize();
}

//----------------------------------------------------------------------//
// FOctreeSceneProxy Implementation
//----------------------------------------------------------------------//

/**
 * Scene proxy to represent octree
 * Derived from UCustomMeshComponent
 */
class FOctreeSceneProxy final : public FPrimitiveSceneProxy
{
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FOctreeSceneProxy(UOctreeRenderingComponent* Component, FOctreeSceneProxyData& ProxyData, const bool bForceRendering = false):
		FPrimitiveSceneProxy(Component),
		bForceRendering(bForceRendering),
		//ProxyData(InProxyData),
		VertexFactory(GetScene().GetFeatureLevel(), "FSVOSceneProxy"),
		MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
	{
		// Move over spheres and lines
		NodeSpheres = ProxyData.NodeSpheres;
		NeighbourLines = ProxyData.NeighbourLines;
		
		if (ProxyData.Vertices.Num() == 0)
		{
			bHasMeshData = false;
			return;
		}
		bHasMeshData = true;

		IndexBuffer.Indices = ProxyData.Indices;
		VertexBuffers.InitFromDynamicVertex(&VertexFactory, ProxyData.Vertices);

		// Enqueue initialization of render resource
		BeginInitResource(&VertexBuffers.PositionVertexBuffer);
		BeginInitResource(&VertexBuffers.StaticMeshVertexBuffer);
		BeginInitResource(&VertexBuffers.ColorVertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);

		// Grab material
		Material = Component->GetMaterial(0);
		if (Material == nullptr)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}
	}

	virtual ~FOctreeSceneProxy()
	{
		if (!bHasMeshData)
		{
			return;
		}

		//ProxyData.NodeSpheres = MoveTemp(NodeSpheres);
		//ProxyData.NeighbourLines = MoveTemp(NeighbourLines);
		
		/*if (ProxyData.Vertices.Num() == 0)
		{
			bHasMeshData = false;
			return;
		}
		bHasMeshData = true;*/

		//ProxyData.Indices = MoveTemp(IndexBuffer.Indices);
		
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_SVOSceneProxy_GetDynamicMeshElements);

		// Draw lines and spheres
		// From FDebugRenderSceneProxy
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

				const int32 LinesNum = NeighbourLines.Num();
				PDI->AddReserveLines(SDPG_World, LinesNum, false, false);
				for (const auto& CurrentLine : NeighbourLines)
				{
					PDI->DrawLine(CurrentLine.Start, CurrentLine.End, CurrentLine.Color, SDPG_World, CurrentLine.Thickness, 0, CurrentLine.Thickness > 0);
				}

				for (const auto& CurrentSphere : NodeSpheres)
				{
					if (FDebugRenderSceneProxy::PointInView(CurrentSphere.Location, View))
					{
						DrawWireSphere(PDI, CurrentSphere.Location, CurrentSphere.Color.WithAlpha(255), CurrentSphere.Radius, 20, SDPG_World,0, 0, true);
					}
				}
			}
		}
		
		/*
		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : nullptr,
			FLinearColor(0, 0.5f, 1.f)
			);
		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
		*/

		if (!bHasMeshData)
		{
			return;
		}

		FMaterialRenderProxy* MaterialProxy = Material->GetRenderProxy();
		
		/*
		if (bWireframe)
		{
			MaterialProxy = WireframeMaterialInstance;
		} else
		{
			MaterialProxy = Material->GetRenderProxy();
		}
		*/

		// From UCustomMeshComponent::GetDynamicMeshElements
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				// Draw the mesh.
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				//Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;

				bool bHasPrecomputedVolumetricLightmap;
				FMatrix PreviousLocalToWorld;
				int32 SingleCaptureIndex;
				bool bOutputVelocity;
				GetScene().GetPrimitiveUniformShaderParameters_RenderThread(GetPrimitiveSceneInfo(), bHasPrecomputedVolumetricLightmap, PreviousLocalToWorld, SingleCaptureIndex, bOutputVelocity);

				FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
#if ATLEAST_UE5_4
				DynamicPrimitiveUniformBuffer.Set(Collector.GetRHICommandList(), GetLocalToWorld(), PreviousLocalToWorld, GetBounds(), GetLocalBounds(), true, bHasPrecomputedVolumetricLightmap, false);
#elif ATLEAST_UE5_1
				DynamicPrimitiveUniformBuffer.Set(GetLocalToWorld(), PreviousLocalToWorld, GetBounds(), GetLocalBounds(), true, bHasPrecomputedVolumetricLightmap, false);
#else// UE5_1
				DynamicPrimitiveUniformBuffer.Set(GetLocalToWorld(), PreviousLocalToWorld, GetBounds(), GetLocalBounds(), true, bHasPrecomputedVolumetricLightmap, DrawsVelocity(), false);
#endif // UE5_1
				BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}
	
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		const bool bVisible = !!View->Family->EngineShowFlags.Navigation || bForceRendering;
		
		// From UCustomMeshComponent::GetViewRelevance
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = bVisible && IsShown(View);
		Result.bShadowRelevance = false;
		Result.bDynamicRelevance = true;
		Result.bStaticRelevance = false;
		Result.bRenderInMainPass = ShouldRenderInMainPass();
		Result.bUsesLightingChannels = false;
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		Result.bTranslucentSelfShadow = false;
		Result.bVelocityRelevance = false;
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		return Result;
	}

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint() const override { return sizeof(*this) + GetAllocatedSize(); }
	uint32 GetAllocatedSize() const
	{
		return FPrimitiveSceneProxy::GetAllocatedSize() +
			NodeSpheres.GetAllocatedSize() +
			NeighbourLines.GetAllocatedSize() +
			IndexBuffer.Indices.GetAllocatedSize() +
			VertexBuffers.PositionVertexBuffer.GetNumVertices() * VertexBuffers.PositionVertexBuffer.GetStride() +
			VertexBuffers.StaticMeshVertexBuffer.GetResourceSize() +
			VertexBuffers.ColorVertexBuffer.GetNumVertices() * VertexBuffers.ColorVertexBuffer.GetStride();
	}

	uint32 bForceRendering: 1;
	
private:
	uint32 bHasMeshData: 1;

	// Debug objects
	TArray<FDebugRenderSceneProxy::FSphere> NodeSpheres;
	TArray<FDebugRenderSceneProxy::FDebugLine> NeighbourLines;
	
	UMaterialInterface* Material;
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FLocalVertexFactory VertexFactory;

	FMaterialRelevance MaterialRelevance;
};


//----------------------------------------------------------------------//
// UOctreeRenderingComponent Implementation
//----------------------------------------------------------------------//

#if WITH_EDITOR
static bool AreAnyViewportsRelevantNew(const UWorld* World)
{
	FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(World);
	if (WorldContext && WorldContext->GameViewport)
	{
		return true;
	}
	for (FEditorViewportClient* CurrentViewport : GEditor->GetAllViewportClients())
	{
		if (CurrentViewport && CurrentViewport->IsVisible())
		{
			return true;
		}
	}

	return false;
}
#endif


UOctreeRenderingComponent::UOctreeRenderingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	UPrimitiveComponent::SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	bIsEditorOnly = true;
	bSelectable = false;
	bCollectNavigationData = false;
	CastShadow = false;

	static const ConstructorHelpers::FObjectFinder<UMaterial> WireMat(TEXT("Material'/FlyingNavSystem/M_Wireframe.M_Wireframe'"));
	
	WireMaterial = UMaterialInstanceDynamic::Create(WireMat.Object, this, TEXT("WireMaterial"));
	UMeshComponent::SetMaterial(0, WireMaterial);
}

FPrimitiveSceneProxy* UOctreeRenderingComponent::CreateSceneProxy()
{
#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	FOctreeSceneProxy* OctreeSceneProxy = nullptr;

	const bool bShowNavigation = IsNavigationShowFlagSet(GetWorld());

	bCollectNavigationData = bShowNavigation;

	if (bCollectNavigationData && IsVisible())
	{
		AFlyingNavigationData* FlyingNavData = Cast<AFlyingNavigationData>(GetOwner());
		if (FlyingNavData && FlyingNavData->IsDrawingEnabled())
		{
			if (bGatherData)
			{
				GatherData(*FlyingNavData, ProxyData);
			}
			bGatherData = true;
			
			OctreeSceneProxy = new FOctreeSceneProxy(this, ProxyData);
		}
	}
	
	return OctreeSceneProxy;
	
#else
	return nullptr;
#endif 
}

void UOctreeRenderingComponent::SetWireThickness(const float Thickness)
{
	WireMaterial->SetScalarParameterValue(TEXT("Thickness"), FMath::Clamp(Thickness, 0.f, 1.f));
}

int32 UOctreeRenderingComponent::GetNumMaterials() const
{
	return 1;
}

void UOctreeRenderingComponent::TimerFunction()
{
	const UWorld* World = GetWorld();
#if WITH_EDITOR
	if (GEditor && (AreAnyViewportsRelevantNew(World) == false))
	{
		return;
	}
#endif // WITH_EDITOR

	const bool bShowNavigation = bForceUpdate || IsNavigationShowFlagSet(World);

	if (bShowNavigation != !!bCollectNavigationData && bShowNavigation == true)
	{
		bForceUpdate = false;
		bCollectNavigationData = bShowNavigation;
		MarkRenderStateDirty();
	}
}

void UOctreeRenderingComponent::OnRegister()
{
	Super::OnRegister();

	AFlyingNavigationData* FlyingNavData = Cast<AFlyingNavigationData>(GetOwner());
	SetWireThickness(FlyingNavData->WireThickness);
	
#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	// it's a kind of HACK but there is no event or other information that show flag was changed by user => we have to check it periodically
#if WITH_EDITOR
	if (GEditor)
	{
		GEditor->GetTimerManager()->SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UOctreeRenderingComponent::TimerFunction), 1, true);
	}
	else
#endif //WITH_EDITOR
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UOctreeRenderingComponent::TimerFunction), 1, true);
	}
#endif // !UE_BUILD_SHIPPING && !UE_BUILD_TEST
}

void UOctreeRenderingComponent::OnUnregister()
{
#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	// it's a kind of HACK but there is no event or other information that show flag was changed by user => we have to check it periodically
#if WITH_EDITOR
	if (GEditor)
	{
		GEditor->GetTimerManager()->ClearTimer(TimerHandle);
	} else
#endif //WITH_EDITOR
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
#endif // !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	Super::OnUnregister();
}

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
void UOctreeRenderingComponent::GatherData(const AFlyingNavigationData& NavData, FOctreeSceneProxyData& OutProxyData)
{
	OutProxyData.GatherData(&NavData, OutProxyData.GetDetailFlags(&NavData));
}
#endif // !UE_BUILD_SHIPPING && !UE_BUILD_TEST

bool UOctreeRenderingComponent::IsNavigationShowFlagSet(const UWorld* World)
{
	bool bShowNavigation;

	FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(World);

#if WITH_EDITOR
	if (GEditor && WorldContext && WorldContext->WorldType != EWorldType::Game)
	{
		bShowNavigation = WorldContext->GameViewport != nullptr && WorldContext->GameViewport->EngineShowFlags.Navigation;
		if (bShowNavigation == false)
		{
			// we have to check all viewports because we can't to distinguish between SIE and PIE at this point.
			for (FEditorViewportClient* CurrentViewport : GEditor->GetAllViewportClients())
			{
				if (CurrentViewport && CurrentViewport->EngineShowFlags.Navigation)
				{
					bShowNavigation = true;
					break;
				}
			}
		}
	}
	else
#endif //WITH_EDITOR
	{
		bShowNavigation = WorldContext && WorldContext->GameViewport && WorldContext->GameViewport->EngineShowFlags.Navigation;
	}

	return bShowNavigation;
}

FBoxSphereBounds UOctreeRenderingComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBox BoundingBox(ForceInit);

	AFlyingNavigationData* NavMesh = Cast<AFlyingNavigationData>(GetOwner());
	if (NavMesh)
	{
		BoundingBox = NavMesh->GetFlyingBounds();
	}

	return FBoxSphereBounds(BoundingBox);
}
