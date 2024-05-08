// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Collision/VoxelCollisionCooker.h"
#include "Collision/VoxelTriangleMeshCollider.h"
#include "Chaos/CollisionConvexMesh.h"
#include "VoxelAABBTree.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELCORE_API, bool, GVoxelCollisionFastCooking, true,
	"voxel.collision.FastCooking",
	"");

namespace Chaos
{
struct FCookTriangleDummy;

template <>
struct FTriangleMeshOverlapVisitorNoMTD<FCookTriangleDummy>
{
	template<typename IndexType>
	static TSharedPtr<FTriangleMeshImplicitObject> CookTriangleMesh(
		const TConstVoxelArrayView<int32> Indices,
		const TConstVoxelArrayView<FVector3f> Vertices,
		const TConstVoxelArrayView<uint16> FaceMaterials)
	{
		VOXEL_FUNCTION_COUNTER();
		VOXEL_ALLOW_MALLOC_SCOPE();

		TParticles<FRealSingle, 3> Particles;
		Particles.AddParticles(Vertices.Num());

		for (int32 Index = 0; Index < Vertices.Num(); Index++)
		{
			Particles.X(Index) = Vertices[Index];
		}

		const int32 NumTriangles = Indices.Num() / 3;

		TCompatibleVoxelArray<TVector<IndexType, 3>> Triangles;
		Triangles.Reserve(NumTriangles);

		for (int32 Index = 0; Index < NumTriangles; Index++)
		{
			const TVector<int32, 3> Triangle{
				Indices[3 * Index + 2],
				Indices[3 * Index + 1],
				Indices[3 * Index + 0]
			};

			if (!FConvexBuilder::IsValidTriangle(
				Particles.X(Triangle.X),
				Particles.X(Triangle.Y),
				Particles.X(Triangle.Z)))
			{
				continue;
			}

			Triangles.Add(Triangle);
		}

		if (Triangles.Num() == 0)
		{
			return nullptr;
		}

		if (!GVoxelCollisionFastCooking)
		{
			VOXEL_SCOPE_COUNTER("Slow cook");

			return MakeVoxelShared<FTriangleMeshImplicitObject>(
				MoveTemp(Particles),
				MoveTemp(Triangles),
				TArray<uint16>(FaceMaterials),
				nullptr,
				nullptr,
				true);
		}

		VOXEL_SCOPE_COUNTER("Fast cook");

		TVoxelArray<FVoxelAABBTree::FElement> Elements;
		FVoxelUtilities::SetNumFast(Elements, Triangles.Num());
		for (int32 Index = 0; Index < Triangles.Num(); Index++)
		{
			const TVector<IndexType, 3>& Triangle = Triangles[Index];
			const FVector3f VertexA = Vertices[Triangle.X];
			const FVector3f VertexB = Vertices[Triangle.Y];
			const FVector3f VertexC = Vertices[Triangle.Z];

			FVoxelAABBTree::FElement& Element = Elements[Index];
			Element.Payload = Index;
			Element.Bounds = FVoxelBox(
				FVoxelUtilities::ComponentMin3(VertexA, VertexB, VertexC),
				FVoxelUtilities::ComponentMax3(VertexA, VertexB, VertexC));
		}

		using FLeaf = TAABBTreeLeafArray<int32, false, float>;
		using FBVHType = TAABBTree<int32, FLeaf, false, float>;
		checkStatic(std::is_same_v<FBVHType, FTriangleMeshImplicitObject::BVHType>);

		// From FTriangleMeshImplicitObject::RebuildBVImp
		constexpr static int32 MaxChildrenInLeaf = 22;
		constexpr static int32 MaxTreeDepth = FBVHType::DefaultMaxTreeDepth;

		FVoxelAABBTree Tree(MaxChildrenInLeaf, MaxTreeDepth);
		Tree.Initialize(MoveTemp(Elements));

		const TVoxelArray<FVoxelAABBTree::FNode>& SrcNodes = Tree.GetNodes();
		const TVoxelArray<FVoxelAABBTree::FLeaf>& SrcLeaves = Tree.GetLeaves();

		FBVHType BVH;
		TCompatibleVoxelArray<TAABBTreeNode<float>>& DestNodes = ToCompatibleVoxelArray(ConstCast(BVH.GetNodes()));
		TCompatibleVoxelArray<FLeaf>& DestLeaves = ToCompatibleVoxelArray(ConstCast(BVH.GetLeaves()));

		FVoxelUtilities::SetNum(DestNodes, SrcNodes.Num());
		FVoxelUtilities::SetNum(DestLeaves, SrcLeaves.Num());

		for (int32 Index = 0; Index < SrcNodes.Num(); Index++)
		{
			const FVoxelAABBTree::FNode& SrcNode = SrcNodes[Index];
			TAABBTreeNode<float>& DestNode = DestNodes[Index];

			if (SrcNode.bLeaf)
			{
				DestNode.bLeaf = true;
				DestNode.ChildrenNodes[0] = SrcNode.LeafIndex;
			}
			else
			{
				DestNode.bLeaf = false;
				DestNode.ChildrenNodes[0] = SrcNode.ChildIndex0;
				DestNode.ChildrenNodes[1] = SrcNode.ChildIndex1;
				DestNode.ChildrenBounds[0] = FAABB3f(SrcNode.ChildBounds0.Min, SrcNode.ChildBounds0.Max);
				DestNode.ChildrenBounds[1] = FAABB3f(SrcNode.ChildBounds1.Min, SrcNode.ChildBounds1.Max);
			}
		}

		for (int32 Index = 0; Index < SrcLeaves.Num(); Index++)
		{
			const FVoxelAABBTree::FLeaf& SrcLeaf = SrcLeaves[Index];
			FLeaf& DestLeaf = DestLeaves[Index];

			FVoxelUtilities::SetNumFast(DestLeaf.Elems, SrcLeaf.Elements.Num());

			for (int32 ElementIndex = 0; ElementIndex < SrcLeaf.Elements.Num(); ElementIndex++)
			{
				const FVoxelAABBTree::FElement& SrcElement = SrcLeaf.Elements[ElementIndex];
				TPayloadBoundsElement<int32, float>& DestElement = DestLeaf.Elems[ElementIndex];

				DestElement.Payload = SrcElement.Payload;
				DestElement.Bounds = FAABB3f(SrcElement.Bounds.Min, SrcElement.Bounds.Max);
			}
		}

		VOXEL_SCOPE_COUNTER("FTriangleMeshImplicitObject::FTriangleMeshImplicitObject");
		return MakeVoxelShareable(new (GVoxelMemory) FTriangleMeshImplicitObject(
			MoveTemp(Particles),
			MoveTemp(Triangles),
			TArray<uint16>(FaceMaterials),
			BVH,
			nullptr,
			nullptr,
			true));
	}
};
}

TSharedPtr<FVoxelTriangleMeshCollider> FVoxelCollisionCooker::CookTriangleMesh(
	const TConstVoxelArrayView<int32> Indices,
	const TConstVoxelArrayView<FVector3f> Vertices,
	const TConstVoxelArrayView<uint16> FaceMaterials)
{
	VOXEL_FUNCTION_COUNTER();

	if (Indices.Num() == 0 ||
		!ensure(Indices.Num() % 3 == 0))
	{
		return nullptr;
	}

	using FCooker = Chaos::FTriangleMeshOverlapVisitorNoMTD<Chaos::FCookTriangleDummy>;

	TSharedPtr<Chaos::FTriangleMeshImplicitObject> TriangleMesh;
	if (Vertices.Num() < MAX_uint16)
	{
		TriangleMesh = FCooker::CookTriangleMesh<uint16>(Indices, Vertices, FaceMaterials);
	}
	else
	{
		TriangleMesh = FCooker::CookTriangleMesh<int32>(Indices, Vertices, FaceMaterials);
	}

	if (!TriangleMesh)
	{
		return nullptr;
	}

	const TSharedRef<FVoxelTriangleMeshCollider> Collider = MakeVoxelShared<FVoxelTriangleMeshCollider>();
	Collider->TriangleMesh = TriangleMesh;

	{
		VOXEL_SCOPE_COUNTER("Compute bounds");
		Collider->LocalBounds = FVoxelBox::FromPositions(Vertices);
	}

	return Collider;
}