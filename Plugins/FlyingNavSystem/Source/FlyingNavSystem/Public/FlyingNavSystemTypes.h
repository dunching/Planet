// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlyingNavSystemModule.h"
#include "AI/Navigation/NavigationTypes.h"
#include "ThirdParty/libmorton/morton.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

// Number of layers for navigation data to be valid (2 SubNodeLayers, LeafLayer and LayerOne)
#define MIN_LAYERS 4
#define MIN_NODE_LAYERS MIN_LAYERS-2
// Number of normal node layers allowed (to fit into a FSVOLink), plus two subnode layers
// NOTE: Reduced to 15 from 17 to make sure NodeIndex doesn't overflow (TODO: verify)
#define MAX_LAYERS 15
#define MAX_NODE_LAYERS MAX_LAYERS-2

#define MIN_SUBNODE_RESOLUTION 1.

// Blocked voxel grid is 0xffffffffffffffff (all bits filled)
#define LEAF_BLOCKED UINT64_MAX
#define LEAF_UNBLOCKED 0

// Data versioning (to prevent serialisation crashes with different data formats with updates)
#define SVODATA_VER_LATEST				2
#define SVODATA_VER_MIN_COMPATIBLE		2

// For 32bit morton codes, each of x y z can be up to 2^10 - 1 = 1023, which is up to 10 layers
// For 64bit morton codes, each of x y z can be up to 2^21 - 1 = 2,097,151, up to 21 layers
typedef uint_fast64_t morton_t;
typedef uint_fast32_t coord_t;
typedef uint_fast32_t small_morton_t;
typedef uint_fast16_t small_coord_t;

// In UE5.2 nav path lengths and costs uses LWC
#if ATLEAST_UE5_2
typedef FVector::FReal FPathLengthType;
#else
typedef float FPathLengthType;
#endif // ATLEAST_UE5_2

// LWC Support (double on UE5, float on UE4)
#if UE5
typedef FVector::FReal FCoord;
#else
#define DOUBLE_SMALL_NUMBER 1.e-8
typedef float FCoord;
typedef FVector FVector3f;
typedef FVector2D FVector2f;
#endif // UE5

namespace FlyingNavSystem
{
	//----------------------------------------------------------------------//
	// Common helper inlines
	//----------------------------------------------------------------------//

	// Neighbour directions
	static constexpr int32 DeltaX[6] = {1,-1, 0, 0, 0, 0};
	static constexpr int32 DeltaY[6] = {0, 0, 1,-1, 0, 0};
	static constexpr int32 DeltaZ[6] = {0, 0, 0, 0, 1,-1};
	
	// Returns a direction vector for the 6 DOF neighbours (Direction is [0, 5])
	inline FIntVector GetDelta(const int32 Direction)
	{
		return FIntVector(DeltaX[Direction], DeltaY[Direction], DeltaZ[Direction]);
	}
	
	// Precalculated subnode mortons for a given direction
	static constexpr small_morton_t SubNodeMortonsForDirection[6][16] =
	{
		{ 0x00, 0x04, 0x20, 0x24, 0x02, 0x06, 0x22, 0x26, 0x10, 0x14, 0x30, 0x34, 0x12, 0x16, 0x32, 0x36 },
		{ 0x09, 0x0d, 0x29, 0x2d, 0x0b, 0x0f, 0x2b, 0x2f, 0x19, 0x1d, 0x39, 0x3d, 0x1b, 0x1f, 0x3b, 0x3f },
		{ 0x00, 0x04, 0x20, 0x24, 0x01, 0x05, 0x21, 0x25, 0x08, 0x0c, 0x28, 0x2c, 0x09, 0x0d, 0x29, 0x2d },
		{ 0x12, 0x16, 0x32, 0x36, 0x13, 0x17, 0x33, 0x37, 0x1a, 0x1e, 0x3a, 0x3e, 0x1b, 0x1f, 0x3b, 0x3f },
		{ 0x00, 0x02, 0x10, 0x12, 0x01, 0x03, 0x11, 0x13, 0x08, 0x0a, 0x18, 0x1a, 0x09, 0x0b, 0x19, 0x1b },
		{ 0x24, 0x26, 0x34, 0x36, 0x25, 0x27, 0x35, 0x37, 0x2c, 0x2e, 0x3c, 0x3e, 0x2d, 0x2f, 0x3d, 0x3f }
	};
	// Algorithm to calculate SubNodeMortonsForDirection
	/*****************************************
	inline void CalculateSubNodeMortonsForDirection()
	{
		small_morton_t Result[6][16];
		
		for (int32 Direction = 0; Direction < 6; Direction++)
		{
			if (DeltaX[Direction] != 0)
			{
				const small_coord_t X = DeltaX[Direction] > 0 ? 0 : 3;
				for (small_coord_t Y = 0; Y < 4; Y++)
				{
					for (small_coord_t Z = 0; Z < 4; Z++)
					{
						Result[Direction][4*Y + Z] = libmorton::morton3D_32_encode(X, Y, Z);
					}
				}
			} else if (DeltaY[Direction] != 0)
			{
				const small_coord_t Y = DeltaY[Direction] > 0 ? 0 : 3;
				for (small_coord_t X = 0; X < 4; X++)
				{
					for (small_coord_t Z = 0; Z < 4; Z++)
					{
						Result[Direction][4*X + Z] = libmorton::morton3D_32_encode(X, Y, Z);
					}
				}
			} else
			{
				const small_coord_t Z = DeltaZ[Direction] > 0 ? 0 : 3;
				for (small_coord_t X = 0; X < 4; X++)
				{
					for (small_coord_t Y = 0; Y < 4; Y++)
					{
						Result[Direction][4*X + Y] = libmorton::morton3D_32_encode(X, Y, Z);
					}
				}
			}
		}
		for (const auto& Direction : Result)
		{
			FString FormattedResult = "{ ";
			for (const small_morton_t Morton : Direction)
			{
				FormattedResult += *FString::Printf(TEXT("%02d"), Morton);
				FormattedResult += ", ";
			}
			FormattedResult += "}";
		
			printw("%s", *FormattedResult)
		}
	}
	*****************************************/

	static constexpr int32 NeighbourMortonOffset[6][4] = {
		{ 0, 2, 4, 6 },
		{ 1, 3, 5, 7 },
		{ 0, 1, 4, 5 },
		{ 2, 3, 6, 7 },
		{ 0, 1, 2, 3 },
		{ 4, 5, 6, 7 }
	};
	// Algorithm to calculate NeighbourMortonOffsetsForDirection
	/*
	inline void CalculateNeighbourMortonOffsets()
	{
		int32 Result[6][4];

		for (int32 Direction = 0; Direction < 6; Direction++)
		{
			// Find neighbour direction
			if (DeltaX[Direction] != 0)
			{
				if (DeltaX[Direction] > 0)
				{
					// Back face
					Result[Direction][0] = 0;
					Result[Direction][1] = 2;
					Result[Direction][2] = 4;
					Result[Direction][3] = 6;
				} else
				{
					// Front face
					Result[Direction][0] = 1;
					Result[Direction][1] = 3;
					Result[Direction][2] = 5;
					Result[Direction][3] = 7;
				}
			} else if (DeltaY[Direction] != 0)
			{
				if (DeltaY[Direction] > 0)
				{
					// Left face
					Result[Direction][0] = 0;
					Result[Direction][1] = 1;
					Result[Direction][2] = 4;
					Result[Direction][3] = 5;
				} else
				{
					// Right face
					Result[Direction][0] = 2;
					Result[Direction][1] = 3;
					Result[Direction][2] = 6;
					Result[Direction][3] = 7;
				}
			} else
			{
				if (DeltaZ[Direction] > 0)
				{
					// Bottom face
					Result[Direction][0] = 0;
					Result[Direction][1] = 1;
					Result[Direction][2] = 2;
					Result[Direction][3] = 3;
				} else
				{
					// Top face
					Result[Direction][0] = 4;
					Result[Direction][1] = 5;
					Result[Direction][2] = 6;
					Result[Direction][3] = 7;
				}
			}
		}
		
		for (const auto& Direction : Result)
		{
			FString FormattedResult = "{ ";
			for (const small_morton_t Morton : Direction)
			{
				FormattedResult += *FString::Printf(TEXT("%d"), Morton);
				FormattedResult += ", ";
			}
			FormattedResult += "}";
		
			printw("%s", *FormattedResult)
		}
	}
	*/
	
	// Helper functions for Morton codes
	// Converts a morton coordinate to a real position, using info about the coordinate system. See SVOData::*Offset methods for details about VoxelOffset
	inline FVector MortonToCoord(const morton_t Code, const FVector& SystemCentre, const FCoord VoxelSideLength, const FCoord VoxelOffset)
	{
		coord_t X, Y, Z;
		libmorton::morton3D_64_decode(Code, X, Y, Z);
		return SystemCentre + FVector(X, Y, Z) * VoxelSideLength - VoxelOffset;
	}
	// Converts a small morton coordinate to a real position, using info about the coordinate system. See SVOData::*Offset methods for details about VoxelOffset
	inline FVector SmallMortonToCoord(const small_morton_t Code, const FVector& SystemCentre, const FCoord VoxelSideLength, const FCoord VoxelOffset)
	{
		small_coord_t X, Y, Z;
		libmorton::morton3D_32_decode(Code, X, Y, Z);
		return SystemCentre + FVector(X, Y, Z) * VoxelSideLength - VoxelOffset;
	}
	
	// Morton helpers
	inline morton_t ParentFromAnyChild(const morton_t& ChildCode) { /* Remove 3 lsb */ return ChildCode >> 3; }
	inline morton_t FirstChildFromParent(const morton_t& ParentCode) { /* Create blank 3 lsb */ return ParentCode << 3; }
	inline morton_t FirstChildFromAnyChild(const morton_t& ChildCode) { /* Clear 3 lsb */ return ChildCode & ~static_cast<morton_t>(0x7); }
	inline morton_t LastChildFromAnyChild(const morton_t& ChildCode) { /* Set 3 lsb */ return ChildCode | static_cast<morton_t>(0x7); }

	// Math helpers
	// Returns axis aligned unit vector that most matches the direction from A to B
	inline FVector MajorAxisDirectionFromTwoPoints(const FVector& A, const FVector& B)
	{
		const FVector Delta = B - A;
		const FVector AbsDelta = Delta.GetAbs();
		const FCoord MaxAxis = AbsDelta.GetMax();
		if (AbsDelta.X == MaxAxis)
		{
			if (Delta.X > 0)
			{
				return FVector::ForwardVector;
			} else
			{
				return FVector::BackwardVector;
			}
		} else if (AbsDelta.Y == MaxAxis)
		{
			if (Delta.Y > 0)
			{
				return FVector::RightVector;
			} else
			{
				return FVector::LeftVector;
			}
		} else
		{
			if (Delta.Z > 0)
			{
				return FVector::UpVector;
			} else
			{
				return FVector::DownVector;
			}
		}
	}
	// For a given octree size and max leaf size, how many layers the Octree will have. Takes into account subdivisions from multithreading
	inline int32 GetNumLayers(const FCoord SideLength, const FCoord MaxDetailSize, const int32 NumThreadSubdivisions)
	{
		// Lowest integer n such that (SideLength / 2^n) <= MaxDetailSize
		return FMath::Clamp(FMath::CeilToInt(FMath::Log2(SideLength / MaxDetailSize)), MIN_LAYERS + NumThreadSubdivisions, MAX_LAYERS);
	}

	// Random generators
	template<typename T>
	T Gauss(const T Mean, const T StdDev)
	{
		// from https://en.wikipedia.org/wiki/Box-Muller_transform
		const T U1 = 1.f - FMath::SRand();
		const T U2 = 1.f - FMath::SRand();
		const T RandStdNormal = FMath::Sqrt(-2.f * FMath::Loge(U1)) * FMath::Sin(2.f * PI * U2); //random normal(0,1)
		return Mean + StdDev * RandStdNormal;
	}
	template<typename T>
	FVector RandPointInSphere(const T Radius)
	{
		// Gaussian distribution is uniform across the surface of a sphere
		FMath::SRandInit(FMath::Rand());
		const FVector UnitVec = FVector(
	        Gauss(0.f, 1.f),
	        Gauss(0.f, 1.f),
	        Gauss(0.f, 1.f)).GetSafeNormal();

		return UnitVec * Radius * FMath::Pow(FMath::FRand(), 1.f / 3.f);
	}
	// Returns positions of voxels inside or on a solid sphere, can be slow (O(R^3))
	inline void RasteriseSphere(const FVector& Centre, const FCoord Radius, const FCoord VoxelSize, TArray<FVector>& SpherePoints)
	{
		const int32 DiameterVoxels = FMath::Abs(FMath::FloorToInt(2.f * Radius / VoxelSize));
		const int32 RadiusVoxels = DiameterVoxels / 2; // Round down

		SpherePoints.Reserve(5 * RadiusVoxels * RadiusVoxels * RadiusVoxels); // > 4/3 pi r^3
		const int32 Threshold = RadiusVoxels*RadiusVoxels + (int32)(RadiusVoxels * 0.8f);

		for (int32 X = -RadiusVoxels; X <= RadiusVoxels; X++)
		{
			for (int32 Y = -RadiusVoxels; Y <= RadiusVoxels; Y++)
			{
				for (int32 Z = -RadiusVoxels; Z <= RadiusVoxels; Z++)
				{
					if (X*X + Y*Y + Z*Z <= Threshold)
					{
						SpherePoints.Add(Centre + FVector(X, Y, Z) * VoxelSize);
					}
				}
			}
		}
	}
	
	// Vertex handling helpers
	inline const FVector& CoordToVec(const FCoord* Coords, const int32 Index)
	{
		return *reinterpret_cast<const FVector*>(&Coords[3 * Index]);
	}
	inline FVector& CoordToVec(FCoord* Coords, const int32 Index)
	{
		return *reinterpret_cast<FVector*>(&Coords[3 * Index]);
	}
	inline FVector& CoordToVec(TArray<FCoord>& Coords, const int32 Index)
	{
		return CoordToVec(Coords.GetData(), Index);
	}

	// Copies and converts an array of FNavPathPoints to an array of FVectors
	inline void ConvertNavPathPointsToVector(const TArray<FNavPathPoint>& In, TArray<FVector>& Out)
	{
		Out.Reset(In.Num());
		for (const FNavPathPoint& PathPoint : In)
		{
			Out.Add(PathPoint.Location);
		}
	}
}

namespace libmorton
{
	inline small_morton_t morton3D_32_encode_v(const FIntVector& InPos)
	{
		return morton3D_32_encode(InPos.X, InPos.Y, InPos.Z);
	}
}

namespace EFlyingNavPathFlags
{
	/** If set, path updates will be processed asynchronously */
	constexpr int32 AsyncObservationUpdate = (1 << 2);
}

//----------------------------------------------------------------------//
//
// Data types
// 
//----------------------------------------------------------------------//

UENUM(BlueprintType)
enum class EPathfindingAlgorithm: uint8
{
	AStar 			UMETA(DisplayName = "A *"),
	LazyThetaStar 	UMETA(DisplayName = "Lazy Theta *"),
	ThetaStar 		UMETA(DisplayName = "Theta *")
};


//----------------------------------------------------------------------//
// EGenerationThreads
//----------------------------------------------------------------------//
// Stores the number of subdivisions to split volume into for multithreading
UENUM(BlueprintType)
enum class EThreadSubdivisions : uint8
{
	// Run Singlethreaded
	Zero = 0		UMETA(DisplayName = "0"),
	// Subdivide volume once, into 8 subvolumes, each on a separate thread
	One = 1			UMETA(DisplayName = "1"),
	// Subdivide twice, 64 subvolumes
	Two = 2			UMETA(DisplayName = "2"),
	// Subdivide thrice, 512 subvolumes
	Three = 3		UMETA(DisplayName = "3"),
	// Subdivide 4 times, 4096 subvolumes, maybe this is useful for really big scenes, who's to say
	Four = 4		UMETA(DisplayName = "4"),
};

namespace FlyingNavSystem
{
	inline int32 GetThreadSubdivisions(const EThreadSubdivisions& ThreadSubdivisions, const bool bMultithreaded)
	{
		return bMultithreaded ? static_cast<int32>(ThreadSubdivisions) : 0;
	}
	// 8^N Threads for N Thread Subdivisions
	inline int32 GetNumThreads(const EThreadSubdivisions& ThreadSubdivisions, const bool bMultithreaded)
	{
		return 1 << 3*GetThreadSubdivisions(ThreadSubdivisions, bMultithreaded);
	}
	inline bool operator>(const EThreadSubdivisions& ThreadSubdivisions, const int& IntThreads)
	{
		return GetNumThreads(ThreadSubdivisions, true) > IntThreads;
	}
}

//----------------------------------------------------------------------//
// FRasterisableGeometry
// 
// Stores arbitrary geometry data
//----------------------------------------------------------------------//
struct FLYINGNAVSYSTEM_API FRasterisableGeometry
{
	TNavStatArray<FCoord> VertexBuffer;
	TNavStatArray<int32> IndexBuffer;
};

//----------------------------------------------------------------------//
// FSVOLink
// 
// FSVOLink is a 32-bit Link:
// 4 bits  — Layer index 0 to 15. 0 = Leaf Node Layer, 1-15 Normal Node Layers
// 22 bits — Node index 0 to 4,194,303
// 6 bits  — SubNode index 0 to 63 (only used for indexing voxels inside leaf nodes)
//
// New Link Format (To Be Implemented):
// 1 bit — NodeLink = 0, SubNodeLink = 1
// NodeLink:
// 4 bits  — Layer index 0 to 15. 0 = Leaf Node Layer, 1-15 Normal Node Layers
// 25 bits — Node index 0 to 33,554,431
// SubNodeLink:
// 25 bits — Node index 0 to 33,554,431
// 6 bits — SubNode index 0 to 63
//----------------------------------------------------------------------//
struct FLYINGNAVSYSTEM_API FSVOLink
{
private:
	uint32 Link;

public:
	FSVOLink(): Link(NULL_LINK.Link) {}
	
	explicit FSVOLink(const uint32 InLink): Link(InLink) {}

	// Checked constructor
	FSVOLink(const uint32 InLayerIndex, const uint32 InNodeIndex, const uint32 InSubNodeIndex = 0)
	{
		checkf(InLayerIndex < (1 << 4), TEXT("Large Layer Index: %d"), InLayerIndex)
		checkf(InNodeIndex < (1 << 22), TEXT("Large Node Index: %d"), InNodeIndex)
		checkf(InSubNodeIndex < (1 << 6), TEXT("Large SubNode Index: %d"), InSubNodeIndex)
		Link = InLayerIndex << 28;
		Link |= InNodeIndex << 6;
		Link |= InSubNodeIndex;
	}

	uint32 GetLayerIndex() const
	{
		return Link >> 28;
	}
	
	void ClearNodeIndex()
	{
		Link = (Link & ~0x0fffffc0);
	}
	uint32 GetNodeIndex() const
	{
		return (Link & 0x0fffffc0) >> 6;
	}
	void SetNodeIndex(const uint32 InNodeIndex)
	{
		checkf(InNodeIndex < (1 << 22), TEXT("Large Node Index: %d"), InNodeIndex)
		ClearNodeIndex();
		Link |= InNodeIndex << 6;
	}
	FSVOLink IncrementNodeIndex(const int32 NodeOffset) const
	{
		FSVOLink NewLink(Link);
		NewLink.SetNodeIndex(GetNodeIndex() + NodeOffset);
		return NewLink;
	}
	
	uint32 GetSubNodeIndex() const
	{
		return (Link & 0x0000003f);
	}

	// Impossible link to indicate an invalid link
	static const FSVOLink NULL_LINK;

	bool IsValid() const { return Link != NULL_LINK.Link; }

	FString ToString() const
	{
		return FString::Printf(TEXT("Layer=%d Node=%d SubNode=%d"), GetLayerIndex(), GetNodeIndex(), GetSubNodeIndex());
	}
	NavNodeRef AsNavNodeRef() const
	{
		return static_cast<NavNodeRef>(Link);
	}
	
	FSVOLink& operator+=(const int32 NodeOffset)
	{
		*this = IncrementNodeIndex(NodeOffset);
		return *this;
	}

	friend bool operator==(const FSVOLink A, const FSVOLink B) { return A.Link == B.Link; }
	friend bool operator!=(const FSVOLink A, const FSVOLink B) { return A.Link != B.Link; }
	friend bool operator<(const FSVOLink A, const FSVOLink B)  { return A.Link < B.Link; } // Useful for sorting by layer
	
	friend FArchive& operator<<(FArchive& Ar, FSVOLink& SVOLink)
	{
		Ar << SVOLink.Link;
		return Ar;
	}

	friend uint32 GetTypeHash(const FSVOLink SVOLink)
	{
		return GetTypeHash(SVOLink.Link);
	}
};

inline FSVOLink operator+(const FSVOLink Link, const int32 NodeOffset)
{
	return Link.IncrementNodeIndex(NodeOffset);
}


//----------------------------------------------------------------------//
// FSVONode
//
// Struct representing an octree node in a layer.
//----------------------------------------------------------------------//
struct FLYINGNAVSYSTEM_API FSVONode
{
	morton_t MortonCode;
	FSVOLink FirstChild;
	FSVOLink Parent = FSVOLink::NULL_LINK;
	FSVOLink Neighbours[6];
	bool bHasChildren;
	bool bBlocked = false;
	uint16 NodeGroup = 0; // For future use

	// Uninitialised FSVONode for efficiency
	// ReSharper disable once CppPossiblyUninitializedMember
	FSVONode() {}
	// ReSharper disable once CppPossiblyUninitializedMember
	explicit FSVONode(const morton_t InMortonCode): MortonCode(InMortonCode) {}
};
// Sort by morton code
inline bool operator<(const FSVONode& A, const FSVONode& B) { return A.MortonCode < B.MortonCode; }


//----------------------------------------------------------------------//
// FSVOLeafNode
// Contains the voxel grid of subnodes, and a link to the parent LayerOne node
//----------------------------------------------------------------------//
struct FLYINGNAVSYSTEM_API FSVOLeafNode
{
	// 4x4x4 voxel grid packed into a 64bit integer
	uint64 VoxelGrid;

	FSVOLeafNode(): VoxelGrid(0) {}
	
	// TODO: Check if this is needed for neighbour links
	// Pointer to parent
	FSVOLink Parent;

	// SubNodes stored in Morton Order
	void SetIndexBlocked(const small_morton_t Index)
	{
		VoxelGrid |= (1ULL << Index);
	}
	bool IsIndexBlocked(const small_morton_t Index) const
	{
		return (VoxelGrid >> Index) & 1ULL;
	}

	// All SubNodes are blocked
	bool IsCompletelyBlocked() const{ return VoxelGrid == LEAF_BLOCKED; }

	// All SubNodes are free
	bool IsCompletelyFree() const{ return VoxelGrid == LEAF_UNBLOCKED; }

	friend uint32 GetTypeHash(const FSVOLeafNode& SVOLeafNode)
	{
		return GetTypeHash(SVOLeafNode.VoxelGrid);
	}
};

//----------------------------------------------------------------------//
// FSVONodeGroup
// Stores data about a node group
//----------------------------------------------------------------------//
struct FLYINGNAVSYSTEM_API FSVONodeGroup
{
	// For future use
	float PathWeight = 0.f;
};

//----------------------------------------------------------------------//
// FSVOLayer
// 
// 2D Array Workaround. FSVOData has a TArray<FSVOLayer> to hold each layer
//----------------------------------------------------------------------//
struct FLYINGNAVSYSTEM_API FSVOLayer
{
	TArray<FSVONode> Nodes;
	
	// Convenience functions, passes through to Array
	FSVONode& AddNode() { return Nodes.AddDefaulted_GetRef(); }
	int32 Num() const { return Nodes.Num(); }
	void Append(const FSVOLayer& OtherLayer) { Nodes.Append(OtherLayer.Nodes); }
	
	FSVONode& GetNode(const int32 Index) { return Nodes[Index]; }
	const FSVONode& GetNode(const int32 Index) const { return Nodes[Index]; }
	FSVONode& operator[](const int32 Index) { return Nodes[Index]; }
	const FSVONode& operator[](const int32 Index) const { return Nodes[Index]; }
	
	void Reserve(const int32 Number) { Nodes.Reserve(Number); }
	void Empty(const int32 Number) { Nodes.Empty(Number); }

	/*
	* Adds *Num* FSVONodes with bHasChildren = false to Layer, with MortonCode starting from *StartCode*
	*/
	void AddChildlessNodes(const int32 Num, const morton_t& StartCode)
	{
		if (Num == 0) return;
		for (int i = 0; i < Num; i++)
		{
			FSVONode& ChildlessNode = AddNode();
			ChildlessNode.bHasChildren = false;
			ChildlessNode.MortonCode = StartCode + i;
		}
	}

	/*
	* Each layer is a multiple of 8, because every node either has 8 or 0 children. We have the nodes with 8 children
	* from the previous layer, but we need to fill in all the nodes with 0 children as padding.
	*
	* Fills in the amount of childless nodes required to satisfy this requirement when
	* jumping between nodes at FirstMorton and LastMorton, which have children.
	* By default fills in the range (FirstMorton, LastMorton) skipping any completely empty blocks, but can include
	* FirstMorton as a childless node for when the layer generation algorithm begins
	*/
	void PadWithChildlessNodes(const morton_t& FirstMorton, const morton_t& LastMorton, bool bIncludeFirst = false)
	{
		// Fill in blank (childless) nodes
		const bool bSameParent = FlyingNavSystem::ParentFromAnyChild(FirstMorton) == FlyingNavSystem::ParentFromAnyChild(LastMorton);

		// Only include LastMorton if they're not the same
		bIncludeFirst &= FirstMorton != LastMorton;
		
		if (bSameParent)
		{
			// From (FirstMorton > LastMorton)
			AddChildlessNodes((LastMorton - FirstMorton + (bIncludeFirst ? 0 : -1)), FirstMorton + (bIncludeFirst ? 0 : 1));
		} else
		{
			// From (FirstMorton > EndOfBlock]
			const morton_t EndOfBlock = FlyingNavSystem::LastChildFromAnyChild(FirstMorton);
			AddChildlessNodes((EndOfBlock - FirstMorton + (bIncludeFirst ? 1 : 0)), FirstMorton + (bIncludeFirst ? 0 : 1));
			
			// From [StartOfBlock > LastMorton)
			const morton_t StartOfBlock = FlyingNavSystem::FirstChildFromAnyChild(LastMorton);
			AddChildlessNodes((LastMorton - StartOfBlock), StartOfBlock);
		}
	}
};
// For consistency
typedef TArray<FSVOLeafNode> FSVOLeafLayer;

// Map used to override node locations (for start and end locations)
typedef TMap<FSVOLink, FVector> FOverrideNodes;

//----------------------------------------------------------------------//
//
// FSVOData definition
// 
// Serialisable data containing everything needed for flying pathfinding, with helpers
// Please note that modifying this struct is not thread safe, use a thread lock
//
// Most functions assume bValid to be true, so make sure SVO data is built before trying to access it
//----------------------------------------------------------------------//
struct FLYINGNAVSYSTEM_API FSVOData : TSharedFromThis<FSVOData, ESPMode::ThreadSafe>
{
	// Leaf storage, each leaf is a 4x4x4 voxel grid packed into a 64-bit integer
	FSVOLeafLayer LeafLayer;

	// Stores Layer 1 to n (in index 0 to n-1)
	TArray<FSVOLayer> Layers;

	// Stores precomputed connectivity between nodes. Nodes with same index are in the same graph
	TMap<FSVOLink, int32> NodeComponent;

	// Stores data about groups of nodes, such as path weight multipliers
	TArray<FSVONodeGroup> NodeGroups;
	
	// Metadata (filled in before generation)
	
	// Total bounds in which pathfinding is supported, guaranteed to be a cube
	FBox Bounds;
	// Centre of the full Octree cube
	FVector Centre;
	// Side length of the full Octree cube
	FCoord SideLength;
	// Actual rasterised resolution of Octree
	FCoord SubNodeSideLength;
	// Number of layers [Leaf->Root). Does not include SubNode layers
	int32 NumNodeLayers;
	// Number of graph components in volume (connected areas)
	int32 NumConnectedComponents;
	// Radius of agent this NavData is built for
	float AgentRadius;

	// Flag to mark if NavData can be used or not
	bool bValid;

	FSVOData():
		SideLength(0),
		SubNodeSideLength(0),
		NumNodeLayers(MIN_NODE_LAYERS),
		NumConnectedComponents(0),
		AgentRadius(0),
		bValid(false)
	{}

	// Sets octree bounds (centre and side length). Octree is always a cube, even if InBounds is not
	void SetBounds(const FBox& InBounds)
	{
		SetBounds(InBounds.GetCenter(), InBounds.GetSize().GetMax());
	}
	void SetBounds(const FVector& InCentre, const FCoord InSideLength)
	{
		Centre = InCentre;
		SideLength = InSideLength;
		Bounds = FBox::BuildAABB(Centre, FVector(InSideLength*0.5f));
	}


	// Checks if SVO is empty (true if no colliding geometry was used)
	bool IsEmptySpace() const { return bValid && Layers.Num() == 1; }

	// Invalidates SVOData
	void Clear()
	{
		LeafLayer.Reset();
		Layers.Reset();
		NodeComponent.Reset();
		bValid = false;
	}

	//----------------------------------------------------------------------//
	// Node accessors
	//----------------------------------------------------------------------//
	
	// Get any layer except layer 0 (Use LeafLayer instead)
	FSVOLayer& GetLayer(const int32 LayerNum)
	{
		check(0 < LayerNum && LayerNum <= Layers.Num())
		return Layers[LayerNum-1];
	}
	// Get any layer except layer 0 (Use LeafLayer instead)
	const FSVOLayer& GetLayer(const int32 LayerNum) const
	{
		check(0 < LayerNum && LayerNum <= Layers.Num())
		return Layers[LayerNum-1];
	}
	// Gets top level node
	const FSVONode& GetRoot() const
	{
		check(bValid && Layers.Num() > 0)
		return GetLayer(Layers.Num())[0];
	}
	FSVOLink GetRootLink() const
	{
		check(Layers.Num() > 0)
		return FSVOLink(Layers.Num(), 0);
	}
	
	// Finds index of node in layer from morton code
	int32 FindNodeInLayer(const int32 LayerNum, const morton_t& NodeMorton) const
	{
		return Algo::BinarySearch<const TArray<FSVONode>, FSVONode>(GetLayer(LayerNum).Nodes, FSVONode(NodeMorton));
	}
	const FSVONode& GetNodeForLink(const FSVOLink NodeRef) const
	{
		return GetLayer(NodeRef.GetLayerIndex()).GetNode(NodeRef.GetNodeIndex());
	}
	FSVONode& GetNodeForLink(const FSVOLink NodeRef)
	{
		return GetLayer(NodeRef.GetLayerIndex()).GetNode(NodeRef.GetNodeIndex());
	}
	FSVOLeafNode& GetLeafNodeForLink(const FSVOLink NodeRef)
	{
		return LeafLayer[NodeRef.GetNodeIndex()];
	}
	const FSVOLeafNode& GetLeafNodeForLink(const FSVOLink NodeRef) const
	{
		return LeafLayer[NodeRef.GetNodeIndex()];
	}
	
	//----------------------------------------------------------------------//
	// Size and offset calculations
	//----------------------------------------------------------------------//

	// Returns side length for any layer. Leaf is Layer 0.
	FCoord GetSideLengthForLayer(const int32 Layer) const { return SubNodeSideLength * (4 << Layer); }
	// Returns side length for any node link. Useful for when link could be either FSVONode or FSVOLeafNode
	FCoord GetSideLengthForLink(const FSVOLink NodeRef) const
	{
		const int32 LayerIdx = NodeRef.GetLayerIndex();
		if (LayerIdx == 0)
		{
			const int32 NodeIdx = NodeRef.GetNodeIndex();
			if (LeafLayer[NodeIdx].IsCompletelyFree())
			{
				return GetSideLengthForLayer(0);
			} else
			{
				return SubNodeSideLength;
			}
		} else
		{
			return GetSideLengthForLayer(LayerIdx);
		}
	}
	
	// Offset is for converting Morton Codes into world or local coordinates
	// Easiest to understand as (SystemSideLength * 0.5f - VoxelSideLength * 0.5f). Translates morton code coordinates (with 0 at the bottom left) to SVO Coordinates (with 0 in the centre). Drawing a diagram helps.
	static FCoord GetOffset(const FCoord SystemSideLength, const FCoord VoxelSideLength) { return (SystemSideLength - VoxelSideLength) * 0.5f; } 
	// Returns node offset for a given node layer in the SVO.
	FCoord GetNodeOffsetForLayer(const int32 Layer) const { return GetOffset(SideLength, GetSideLengthForLayer(Layer)); }
	// Returns node offset for a SubNode (in the leaf node coordinate system)
	FCoord GetSubNodeOffset() const { return 1.5f * SubNodeSideLength; } // Manual simplification of GetOffset(SubNodeSize*4, SubNodeSize)

	// Extent calculations to prevent bugs (factor of 0.5f)
	
	// Returns extent of SubNode (for use in FBox::BuildAABB)
	FVector GetSubNodeExtent() const{ return FVector(SubNodeSideLength * 0.5f); }
	// Returns extent of layer (for use in FBox::BuildAABB)
	FVector GetExtentForLayer(const int32 Layer) const { return FVector(SubNodeSideLength * (2 << Layer)); } // Extent is half of side length, so this is the same as FVector(SubNodeSideLength * (4 << Layer)) * 0.5f
	// Returns extent of the whole octree (for use in FBox::BuildAABB)
	FVector GetOctreeExtent() const { return FVector(SideLength * 0.5f); }
	// Returns extent of node references by link
	FVector GetExtentForLink(const FSVOLink NodeRef) const
	{
		const int32 LayerIdx = NodeRef.GetLayerIndex();
		if (LayerIdx == 0)
		{
			const int32 NodeIdx = NodeRef.GetNodeIndex();
			if (LeafLayer[NodeIdx].IsCompletelyFree())
			{
				return GetExtentForLayer(0);
			} else
			{
				return GetSubNodeExtent();
			}
		} else
		{
			return GetExtentForLayer(LayerIdx);
		}
	}
	// Returns the FBox for any given node link
	FBox GetNodeBoxForLink(const FSVOLink NodeRef) const
	{
		return FBox::BuildAABB(GetPositionForLink(NodeRef), GetExtentForLink(NodeRef));
	}
	
	// Finds a world location for a given non-leaf link, optimised
	FVector GetPositionForNonLeafLink(const FSVOLink NodeRef) const
	{
		const int32 LayerIdx = NodeRef.GetLayerIndex();
		const int32 NodeIdx = NodeRef.GetNodeIndex();
		check(LayerIdx > 0)

		const FSVONode& Node = GetLayer(LayerIdx).GetNode(NodeIdx);
		return FlyingNavSystem::MortonToCoord(Node.MortonCode, Centre, GetSideLengthForLayer(LayerIdx), GetNodeOffsetForLayer(LayerIdx));
	}
	// Finds a world location for any given link, can also return position of leaves and SubNodes
	FVector GetPositionForLink(const FSVOLink NodeRef, bool bSearchSubNodes = true) const
	{
		if (!NodeRef.IsValid())
		{
			return FNavigationSystem::InvalidLocation;
		}
		const int32 LayerIdx = NodeRef.GetLayerIndex();
		const int32 NodeIdx = NodeRef.GetNodeIndex();

		if (LayerIdx == 0)
		{
			const FSVOLeafNode& Leaf = LeafLayer[NodeIdx];
			const FSVONode& LeafParent = GetLayer(1).GetNode(Leaf.Parent.GetNodeIndex());
			const int32 ChildIdx = NodeIdx - LeafParent.FirstChild.GetNodeIndex();
			const morton_t LeafMorton = FlyingNavSystem::FirstChildFromParent(LeafParent.MortonCode) + ChildIdx;
			
			const FVector LeafPosition = FlyingNavSystem::MortonToCoord(LeafMorton, Centre, GetSideLengthForLayer(0), GetNodeOffsetForLayer(0));

			if (Leaf.IsCompletelyFree() || !bSearchSubNodes)
			{
				return LeafPosition;
			}
			
			// Find position of SubNode
			return FlyingNavSystem::MortonToCoord(NodeRef.GetSubNodeIndex(), LeafPosition, SubNodeSideLength, GetSubNodeOffset());
		} else
		{
			return GetPositionForNonLeafLink(NodeRef);
		}
	}
	// Finds a world location for any given link, can also return position of leaves and SubNodes, but checks TempNodes to override position values
	FVector GetPositionForLinkWithOverride(const FSVOLink NodeRef,
	                                    const FOverrideNodes& Override,
	                                    const bool bSearchSubNodes = true) const
	{
		if (Override.Contains(NodeRef))
		{
			return Override[NodeRef];
		}
		return GetPositionForLink(NodeRef, bSearchSubNodes);
	}

	// Snaps given position to regular subnode grid
	FVector SnapPositionToVoxelGrid(const FVector& Position) const
	{
		return Centre + ((Position - Centre) + GetSubNodeExtent()).GridSnap(SubNodeSideLength) - GetSubNodeExtent();
	}
	
	// Finds a node link for a given world position. By default doesn't return blocked links
	FSVOLink GetNodeLinkForPosition(const FVector& Position, const bool bAllowBlocked = false) const
	{
		// Check bounds
		if (!Bounds.IsInside(Position) || !bValid)
		{
			return FSVOLink::NULL_LINK;
		}
		// Snap to grid to prevent borderline case
		const FVector NodePosition = SnapPositionToVoxelGrid(Position);
		
		// Start at root
		FSVOLink ParentLink = GetRootLink();
		bool bFoundNode = false;
		for (int32 LayerIdx = ParentLink.GetLayerIndex()-1; LayerIdx >= 0; LayerIdx--)
		{
			const FSVONode& ParentNode = GetNodeForLink(ParentLink);
			if (ParentNode.bHasChildren)
			{
				const int32 FirstChildIdx = ParentNode.FirstChild.GetNodeIndex();
				const int32 ChildLayerIdx = ParentNode.FirstChild.GetLayerIndex();

				check(ChildLayerIdx == LayerIdx);

				bool bFoundChild = false;
				
				const FVector NodeExtent = GetExtentForLayer(ChildLayerIdx);
				
				for (int32 ChildIdx = 0; ChildIdx < 8; ChildIdx++)
				{
					// Check if child node contains position
					const FSVOLink ChildLink(ChildLayerIdx, FirstChildIdx + ChildIdx);
				
					const FVector NodeCentre = GetPositionForLink(ChildLink, false);
					const FBox NodeBounds = FBox::BuildAABB(NodeCentre, NodeExtent);
					if (NodeBounds.IsInsideOrOn(NodePosition))
					{
						ParentLink = ChildLink;
						bFoundChild = true;
						break;
					}
				}
				
				check(bFoundChild);
			} else
			{
				// Disallow blocked nodes
				if (!bAllowBlocked && ParentNode.bBlocked)
				{
					return FSVOLink::NULL_LINK;
				}
				
				bFoundNode = true;
				break;
			}
		}
		// Position is in a large node
		if (bFoundNode)
		{
			return ParentLink;
		}
		
		// Position is in leaf or SubNode
		check(ParentLink.GetLayerIndex() == 0)

	    const int32 LeafIndex = ParentLink.GetNodeIndex();
		const FSVOLeafNode& LeafNode = LeafLayer[LeafIndex];
		if (LeafNode.IsCompletelyFree())
		{
			return ParentLink;
		}

		if (LeafNode.IsCompletelyBlocked())
		{
			return FSVOLink::NULL_LINK;
		}
		
		// Find SubNode
		const FVector SubNodeExtent = GetSubNodeExtent();
		
		for (small_morton_t SubNodeIdx = 0; SubNodeIdx < 64; SubNodeIdx++)
		{
			const FSVOLink SubNodeLink(0, LeafIndex, SubNodeIdx);
			const FVector SubNodeCentre = GetPositionForLink(SubNodeLink, true);
			const FBox NodeBounds = FBox::BuildAABB(SubNodeCentre, SubNodeExtent);
			if (NodeBounds.IsInsideOrOn(NodePosition) && (bAllowBlocked || !LeafNode.IsIndexBlocked(SubNodeIdx)))
			{
				return SubNodeLink;
			}
		}
		
		return FSVOLink::NULL_LINK;
	}

	// Returns float in range [0, 1] for 0 = Leaf, 1 = Root
	float GetLayerProportionForLink(const FSVOLink NodeRef) const
	{
		return static_cast<float>(NodeRef.GetLayerIndex()) / static_cast<float>(Layers.Num() + 1);
	}

	// Checks if a position is blocked in the SVO representation
	bool IsPositionBlocked(const FVector& NodePosition) const
	{
		return !GetNodeLinkForPosition(NodePosition, false).IsValid();
	}

	// World Origin Rebasing support
	void ApplyWorldOffset(const FVector& WorldOffset)
	{
		SetBounds(Centre + WorldOffset, SideLength);
	}

	void RunOnChildlessNodes(const FSVOLink CurrentNode, const TFunctionRef<void (const FSVOLink& NodeLink)> Func) const {
		const int32 LayerIdx = CurrentNode.GetLayerIndex();
		const int32 NodeIdx = CurrentNode.GetNodeIndex();
		
		if (LayerIdx == 0)
		{
			const FSVOLeafNode& LeafNode = LeafLayer[NodeIdx];

			if (LeafNode.IsCompletelyFree())
			{
				Func(CurrentNode);
				return;
			}
			
			if (LeafNode.IsCompletelyBlocked())
			{
				return;
			}

			// Add all unblocked SubNodes
			for (small_morton_t i = 0; i < 64; i++)
			{
				if (!LeafNode.IsIndexBlocked(i))
				{
					Func(FSVOLink(0, NodeIdx, i));
				}
			}
		} else
		{
			const FSVONode& Node = GetLayer(LayerIdx).GetNode(NodeIdx);
			// Ignore blocked nodes
			if (Node.bBlocked)
			{
				return;
			}
			
			if (Node.bHasChildren)
			{
				// Recurse to children
				const FSVOLink& FirstChild = Node.FirstChild;
				for (int32 i = 0; i < 8; i++)
				{
					RunOnChildlessNodes(FirstChild + i, Func);
				}
			} else
			{
				Func(CurrentNode);
			}
		}
	}

	void RunOnAllChildlessNodes(const TFunctionRef<void (const FSVOLink& NodeLink)> Func) const
	{
		RunOnChildlessNodes(GetRootLink(), Func);
	}
	
	// Fills an array with references to nodes that are empty below a given node reference. Includes leaf and SubNodes
	void GetChildlessNodes(const FSVOLink CurrentNode, TArray<FSVOLink>& ChildlessNodes) const
	{
		RunOnChildlessNodes(CurrentNode, [&ChildlessNodes](const FSVOLink& NodeLink)
		{
			ChildlessNodes.Add(NodeLink);
		});
	}
	
	// Fills an array with references to all nodes that are empty. Includes leaf and SubNodes
	void GetAllChildlessNodes(TArray<FSVOLink>& ChildlessNodes) const
	{
		GetChildlessNodes(GetRootLink(), ChildlessNodes);
	}

	// Finds random point in sphere (by trial and error). Optionally reachable. Returns true if a position is found
	bool RandomNavigablePointInRadius(const FVector& Origin, const FCoord Radius, FNavLocation& OutResult, const bool bReachable = false) const
	{
		// Old implementation. Uses slow sphere rasterisation, but always finds a point if available
		/*
		const FSVOLink StartLink = GetNodeLinkForPosition(Origin);
		const FVector SphereCentre = SnapPositionToVoxelGrid(Origin);

		TArray<FVector> SpherePoints;
		FlyingNavSystem::RasteriseSphere(SphereCentre, Radius, SubNodeSideLength, SpherePoints);

		TArray<FVector> ValidVoxelCentres;
		ValidVoxelCentres.Reserve(SpherePoints.Num());

		for (const FVector& Point : SpherePoints)
		{
			if (!IsPositionBlocked(Point) && (!bReachable || IsConnected(StartLink, GetNodeLinkForPosition(Point))))
			{
				ValidVoxelCentres.Add(Point);
			}
		}

		if (ValidVoxelCentres.Num() == 0)
		{
			return false;
		}
	
		const FVector VoxelCentre = ValidVoxelCentres[FMath::RandHelper(ValidVoxelCentres.Num())];
		const FSVOLink VoxelLink = GetNodeLinkForPosition(VoxelCentre);

		OutResult.NodeRef = VoxelLink.AsNavNodeRef();
		OutResult.Location = FMath::RandPointInBox(FBox::BuildAABB(VoxelCentre, GetSubNodeExtent()));

		return true;
		*/

		const FSVOLink& StartLink = GetNodeLinkForPosition(Origin);
		int32 Count = 0;
		bool bFoundValid;
		FSVOLink NodeLink;
		FVector RandPoint;
		do
		{
			RandPoint = Origin + FlyingNavSystem::RandPointInSphere(Radius);
			NodeLink = GetNodeLinkForPosition(RandPoint, false);
			bFoundValid = NodeLink.IsValid() && (!bReachable || IsConnected(StartLink, NodeLink));
			Count++;
		} while (!bFoundValid && Count < 20);

		// Tried 20 times, probably no spot
		if (!bFoundValid)
		{
			return false;
		}
	
		OutResult.Location = RandPoint;
		OutResult.NodeRef = NodeLink.AsNavNodeRef();
		
		return true;
	}
	
	// Returns number of octree subdivisions stored in this SVOData
	int32 GetSubdivisions() const { return FlyingNavSystem::GetNumLayers(SideLength, SubNodeSideLength, 0); }
	
#if WITH_EDITOR
	// Given a node link, draws the node in world
	void DrawLink(UWorld* World, const FSVOLink Link)
	{
		const FVector PathPoint = GetPositionForLink(Link);
		const FVector VoxelCentre = GetPositionForLink(Link);
		FVector VoxelExtent;
		if (Link.GetLayerIndex() == 0 && !LeafLayer[Link.GetNodeIndex()].IsCompletelyFree())
		{
			VoxelExtent = GetSubNodeExtent();
		} else
		{
			VoxelExtent = GetExtentForLayer(Link.GetLayerIndex());
		}
		
		DrawDebugBox(World, VoxelCentre, VoxelExtent, FColor::Orange, true);
		DrawDebugSphere(World, PathPoint, 32.f, 8, FColor::Cyan, true);
	}
#endif

	// Index of connected component. Invalid links return INDEX_NONE
	int32 GetComponentIndex(const FSVOLink Link) const { return NodeComponent.Contains(Link) ? NodeComponent[Link] : INDEX_NONE; }

	// Checks if two node references are reachable (blocked nodes are never reachable)
	bool IsConnected(const FSVOLink LinkA, const FSVOLink LinkB) const
	{
		if (!NodeComponent.Contains(LinkA) || !NodeComponent.Contains(LinkB))
		{
			return false;
		}
        return NodeComponent[LinkA] == NodeComponent[LinkB];
	}

	uint32 GetAllocatedSize() const
	{
		return sizeof(FSVOData) + LeafLayer.GetAllocatedSize() + Layers.GetAllocatedSize() + NodeComponent.GetAllocatedSize();
	}
};

//----------------------------------------------------------------------//
// SVO Serialisation
//----------------------------------------------------------------------//
#if PLATFORM_LINUX || PLATFORM_ANDROID || PLATFORM_PS5
// Prevents weird clang errors caused from serialising uint_fast64_t
inline FArchive& operator<<(FArchive& Ar, uint_fast64_t& Value)
{
	uint64 TempValue;
	if (Ar.IsLoading())
	{
		Ar << TempValue;
		Value = TempValue;
	} else
	{
		TempValue = Value;
		Ar << TempValue;
	}
	
	return Ar;
}
#endif

inline FArchive& operator<<(FArchive& Ar, FSVONode& Node)
{
	Ar << Node.bHasChildren;
	if (Node.bHasChildren)
	{
		Ar << Node.FirstChild;
	}
	Ar << Node.Parent;
	for (int i = 0; i < 6; i++)
	{
		Ar << Node.Neighbours[i];
	}
	Ar << Node.MortonCode;
	
	return Ar;
}
inline FArchive& operator<<(FArchive& Ar, FSVOLeafNode& LeafNode)
{
	Ar << LeafNode.VoxelGrid;
	Ar << LeafNode.Parent;
	return Ar;
}
inline FArchive& operator<<(FArchive& Ar, FSVOLayer& LayerData)
{
	Ar << LayerData.Nodes;
	return Ar;
}
inline FArchive& operator<<(FArchive& Ar, FSVOData& Data)
{
	Ar << Data.LeafLayer;
	Ar << Data.Layers;
	Ar << Data.NodeComponent;
	Ar << Data.Bounds;
	Ar << Data.Centre;
	Ar << Data.SideLength;
	Ar << Data.SubNodeSideLength;
	Ar << Data.NumNodeLayers;
	Ar << Data.NumConnectedComponents;
	Ar << Data.AgentRadius;

	if (Ar.IsLoading())
	{
		if (Data.Layers.Num() > 0 && Data.Bounds.IsValid && Data.SubNodeSideLength >= MIN_SUBNODE_RESOLUTION)
		{
			Data.bValid = true;
		} else
		{
			Data.Clear();
		}
	}

	return Ar;
}

//----------------------------------------------------------------------//
// Useful typedefs
//----------------------------------------------------------------------//
typedef TSharedRef<		 FSVOData, ESPMode::ThreadSafe>	FSVODataRef;
typedef TSharedRef<const FSVOData, ESPMode::ThreadSafe> FSVODataConstRef;
typedef TSharedPtr<		 FSVOData, ESPMode::ThreadSafe>	FSVODataPtr;
typedef TSharedPtr<const FSVOData, ESPMode::ThreadSafe> FSVODataConstPtr;
