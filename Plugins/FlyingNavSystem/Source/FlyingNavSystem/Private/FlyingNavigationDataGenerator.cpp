// Copyright Ben Sutherland 2024. All rights reserved.

#include "FlyingNavigationDataGenerator.h"
#include "BoxUnion.h"
#include "FlyingNavigationData.h"
#include "FlyingNavSystemModule.h"
#include "FlyingNavSystemTypes.h"
#include "SVOGraph.h"
#include "ThirdParty/AABBTriangleIntersection.h"
#include "ThirdParty/libmorton/morton.h"

#include "NavigationSystem.h"
#include "AI/NavigationSystemHelpers.h"
#include "Algo/BinarySearch.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NavMesh/RecastHelpers.h"
#include "Net/Core/Misc/ResizableCircularQueue.h"
#include "PhysicsEngine/BodySetup.h"
#include "VisualLogger/VisualLoggerTypes.h"

static_assert(WITH_RECAST, "ERROR: Plugin requires Recast. Please contact the developer of the FlyingNavSystem plugin if you require otherwise.");

#if WITH_PHYSX_FNS
#include "PhysXPublic.h"
#endif

// Used for debug drawing exported geometry
#define SHOW_NAV_EXPORT_PREVIEW 0

#if SHOW_NAV_EXPORT_PREVIEW
#include "Engine/Engine.h"
#endif

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#include "Async/Async.h"
#endif // WITH_EDITOR

// Compile options
#define ALLOW_CANCEL 1
#define OPTIMISE_GEOMETRY 1
#define PRINT_BENCHMARK 0

static const FName NAME_NavigationIgnoreStaticMeshComponentTag(TEXT("NAVIGNORE"));

// Copied from NavMesh/RecastNavMeshGenerator.h, which exports the data (and therefore needs to have identical memory layout).
struct FSVOGeometryCache
{
	struct FHeader
	{
		FNavigationRelevantData::FCollisionDataHeader Validation;

		int32 NumVerts;
		int32 NumFaces;

		// This is so the recast memory padding works, we don't actually need it
		FWalkableSlopeOverride SlopeOverride;
	};

	FHeader Header;

	/** ue4 coords of vertices (size: NumVerts * 3) */
	FCoord* Verts;

	/** vert indices for triangles (size: NumFaces * 3) */
	int32* Indices;

	FSVOGeometryCache() {}
	FSVOGeometryCache(const uint8* Memory)
	{
		Header = *((FHeader*)Memory);
		Verts = (FCoord*)(Memory + sizeof(FSVOGeometryCache));
		Indices = (int32*)(Memory + sizeof(FSVOGeometryCache) + (sizeof(FCoord) * Header.NumVerts * 3));
	}
};

// This is just a stub class, for exporting landscape slices. Recast does every other object
struct FSVOGeometryExport final : FNavigableGeometryExport
{
	FSVOGeometryExport(): Bounds(ForceInit)
	{ }

	// Move RasterisableGeometry
	FSVOGeometryExport(FRasterisableGeometry&& RasterisableGeometry):
		VertexBuffer(MoveTemp(RasterisableGeometry.VertexBuffer)),
		IndexBuffer(MoveTemp(RasterisableGeometry.IndexBuffer))
	{ }
	
	FBox Bounds;
	TNavStatArray<FCoord> VertexBuffer;
	TNavStatArray<int32> IndexBuffer;

#if PHYSICS_INTERFACE_PHYSX_FNS
	virtual void ExportPxTriMesh16Bit(physx::PxTriangleMesh const* const TriMesh, const FTransform& LocalToWorld) override {}
	virtual void ExportPxTriMesh32Bit(physx::PxTriangleMesh const* const TriMesh, const FTransform& LocalToWorld) override {}
	virtual void ExportPxConvexMesh(physx::PxConvexMesh const* const ConvexMesh, const FTransform& LocalToWorld) override {}
	virtual void ExportPxHeightField(physx::PxHeightField const* const HeightField, const FTransform& LocalToWorld) override {}
	virtual void ExportPxHeightFieldSlice(const FNavHeightfieldSamples& PrefetchedHeightfieldSamples, const int32 NumRows, const int32 NumCols, const FTransform& LocalToWorld, const FBox& SliceBox) override;
#endif // PHYSICS_INTERFACE_PHYSX_FNS
#if WITH_CHAOS_FNS
	virtual void ExportChaosTriMesh(const Chaos::FTriangleMeshImplicitObject* const TriMesh, const FTransform& LocalToWorld) override {}
	virtual void ExportChaosConvexMesh(const FKConvexElem* const Convex, const FTransform& LocalToWorld) override {}
	virtual void ExportChaosHeightField(const Chaos::FHeightField* const Heightfield, const FTransform& LocalToWorld) override {}
	virtual void ExportChaosHeightFieldSlice(const FNavHeightfieldSamples& PrefetchedHeightfieldSamples, const int32 NumRows, const int32 NumCols, const FTransform& LocalToWorld, const FBox& SliceBox) override;
#endif
	
	virtual void ExportCustomMesh(const FVector* InVertices, int32 NumVerts, const int32* InIndices, int32 NumIndices, const FTransform& LocalToWorld) override {}
	virtual void ExportRigidBodySetup(UBodySetup& BodySetup, const FTransform& LocalToWorld) override {}
	virtual void AddNavModifiers(const FCompositeNavModifier& Modifiers) override {}
	virtual void SetNavDataPerInstanceTransformDelegate(const FNavDataPerInstanceTransformDelegate& InDelegate) override {}
};

#if WITH_CHAOS_FNS
void FSVOGeometryExport::ExportChaosHeightFieldSlice(const FNavHeightfieldSamples& PrefetchedHeightfieldSamples, const int32 NumRows, const int32 NumCols, const FTransform& LocalToWorld, const FBox& SliceBox) 
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_NavMesh_ExportChaosHeightFieldSlice);

	// calculate the actual start and number of columns we want
	const FBox LocalBox = SliceBox.TransformBy(LocalToWorld.Inverse());
	const bool bMirrored = (LocalToWorld.GetDeterminant() < 0.f);

	const int32 MinX = FMath::Clamp(FMath::FloorToInt(LocalBox.Min.X) - 1, 0, NumCols);
	const int32 MinY = FMath::Clamp(FMath::FloorToInt(LocalBox.Min.Y) - 1, 0, NumRows);
	const int32 MaxX = FMath::Clamp(FMath::CeilToInt(LocalBox.Max.X) + 1, 0, NumCols);
	const int32 MaxY = FMath::Clamp(FMath::CeilToInt(LocalBox.Max.Y) + 1, 0, NumRows);
	const int32 SizeX = MaxX - MinX;
	const int32 SizeY = MaxY - MinY;

	if (SizeX <= 0 || SizeY <= 0)
	{
		// slice is outside bounds, skip
		return;
	}

	const int32 VertOffset = VertexBuffer.Num() / 3;
	const int32 NumVerts = SizeX * SizeY;
	const int32 NumQuads = (SizeX - 1) * (SizeY - 1);
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_RecastGeometryExport_AllocatingMemory);
		VertexBuffer.Reserve(VertexBuffer.Num() + NumVerts * 3);
		IndexBuffer.Reserve(IndexBuffer.Num() + NumQuads * 3 * 2);
	}

	for (int32 IdxY = 0; IdxY < SizeY; IdxY++)
	{
		for (int32 IdxX = 0; IdxX < SizeX; IdxX++)
		{
			const int32 CoordX = IdxX + MinX;
			const int32 CoordY = IdxY + MinY;
			const int32 SampleIdx = CoordY * NumCols + CoordX;
			
			const FVector UnrealCoords = LocalToWorld.TransformPosition(FVector(CoordX, CoordY, PrefetchedHeightfieldSamples.Heights[SampleIdx]));
			Bounds += UnrealCoords;
			
			VertexBuffer.Add(UnrealCoords.X);
			VertexBuffer.Add(UnrealCoords.Y);
			VertexBuffer.Add(UnrealCoords.Z);
		}
	}

	for (int32 IdxY = 0; IdxY < SizeY - 1; IdxY++)
	{
		for (int32 IdxX = 0; IdxX < SizeX - 1; IdxX++)
		{
			const int32 CoordX = IdxX + MinX;
			const int32 CoordY = IdxY + MinY;
			const int32 SampleIdx = CoordY * (NumCols-1) + CoordX;

			const bool bIsHole = PrefetchedHeightfieldSamples.Holes[SampleIdx];
			if (bIsHole)
			{
				continue;
			}

			const int32 I0 = IdxY * SizeX + IdxX;
			int32 I1 = I0 + 1;
			int32 I2 = I0 + SizeX;
			const int32 I3 = I2 + 1;
			if (bMirrored)
			{
				Swap(I1, I2);
			}

			IndexBuffer.Add(VertOffset + I0);
			IndexBuffer.Add(VertOffset + I3);
			IndexBuffer.Add(VertOffset + I1);

			IndexBuffer.Add(VertOffset + I0);
			IndexBuffer.Add(VertOffset + I2);
			IndexBuffer.Add(VertOffset + I3);
		}
	}
}
#endif

#if PHYSICS_INTERFACE_PHYSX_FNS
void FSVOGeometryExport::ExportPxHeightFieldSlice(const FNavHeightfieldSamples& PrefetchedHeightfieldSamples, const int32 NumRows, const int32 NumCols, const FTransform& LocalToWorld, const FBox& SliceBox) 
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_NavMesh_ExportPxHeightFieldSlice);
	
	static const uint32 SizeOfPx = sizeof(physx::PxI16);
	static const uint32 SizeOfHeight = PrefetchedHeightfieldSamples.Heights.GetTypeSize();
	ensure(SizeOfPx == SizeOfHeight);

	// calculate the actual start and number of columns we want
	const FBox LocalBox = SliceBox.TransformBy(LocalToWorld.Inverse());
	const bool bMirrored = (LocalToWorld.GetDeterminant() < 0.f);

	const int32 MinX = FMath::Clamp(FMath::FloorToInt(LocalBox.Min.X) - 1, 0, NumCols);
	const int32 MinY = FMath::Clamp(FMath::FloorToInt(LocalBox.Min.Y) - 1, 0, NumRows);
	const int32 MaxX = FMath::Clamp(FMath::CeilToInt(LocalBox.Max.X) + 1, 0, NumCols);
	const int32 MaxY = FMath::Clamp(FMath::CeilToInt(LocalBox.Max.Y) + 1, 0, NumRows);
	const int32 SizeX = MaxX - MinX;
	const int32 SizeY = MaxY - MinY;

	if (SizeX <= 0 || SizeY <= 0)
	{
		// slice is outside bounds, skip
		return;
	}

	const int32 VertOffset = VertexBuffer.Num() / 3;
	const int32 NumVerts = SizeX * SizeY;
	const int32 NumQuads = (SizeX - 1) * (SizeY - 1);
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_RecastGeometryExport_AllocatingMemory);
		VertexBuffer.Reserve(VertexBuffer.Num() + NumVerts * 3);
		IndexBuffer.Reserve(IndexBuffer.Num() + NumQuads * 3 * 2);
	}

	for (int32 IdxY = 0; IdxY < SizeY; IdxY++)
	{
		for (int32 IdxX = 0; IdxX < SizeX; IdxX++)
		{
			const int32 CoordX = IdxX + MinX;
			const int32 CoordY = IdxY + MinY;
			const int32 SampleIdx = ((bMirrored ? CoordX : (NumCols - CoordX - 1)) * NumCols) + CoordY;

			const FVector UnrealCoords = LocalToWorld.TransformPosition(FVector(CoordX, CoordY, PrefetchedHeightfieldSamples.Heights[SampleIdx]));
			// My addition to the default code - keep track of bounds
			Bounds += UnrealCoords;
			
			VertexBuffer.Add(UnrealCoords.X);
			VertexBuffer.Add(UnrealCoords.Y);
			VertexBuffer.Add(UnrealCoords.Z);
		}
	}

	for (int32 IdxY = 0; IdxY < SizeY - 1; IdxY++)
	{
		for (int32 IdxX = 0; IdxX < SizeX - 1; IdxX++)
		{
			const int32 CoordX = IdxX + MinX;
			const int32 CoordY = IdxY + MinY;
			const int32 SampleIdx = ((bMirrored ? CoordX : (NumCols - CoordX - 1)) * NumCols) + CoordY;

			if (SampleIdx >= PrefetchedHeightfieldSamples.Holes.Num())
			{
				printw("ERROR: Sample out of range: %d", SampleIdx)
			}
			
			const bool bIsHole = PrefetchedHeightfieldSamples.Holes[SampleIdx];
			if (bIsHole)
			{
				continue;
			}

			const int32 I00 = (IdxX + 0) + (IdxY + 0) * SizeX;
			int32 I01 = (IdxX + 0) + (IdxY + 1) * SizeX;
			int32 I10 = (IdxX + 1) + (IdxY + 0) * SizeX;
			const int32 I11 = (IdxX + 1) + (IdxY + 1) * SizeX;
			if (bMirrored)
			{
				Swap(I01, I10);
			}

			IndexBuffer.Add(VertOffset + I00);
			IndexBuffer.Add(VertOffset + I11);
			IndexBuffer.Add(VertOffset + I10);

			IndexBuffer.Add(VertOffset + I00);
			IndexBuffer.Add(VertOffset + I01);
			IndexBuffer.Add(VertOffset + I11);
		}
	}
}
#endif // PHYSICS_INTERFACE_PHYSX

namespace SVOGeometryExport
{
	static void StoreCollisionCache(FSVOGeometryExport& GeomExport, TNavStatArray<uint8>& CollisionData)
	{
		const int32 NumFaces = GeomExport.IndexBuffer.Num() / 3;
		const int32 NumVerts = GeomExport.VertexBuffer.Num() / 3;

		if (NumFaces == 0 || NumVerts == 0)
		{
			CollisionData.Empty();
			return;
		}

		FSVOGeometryCache::FHeader HeaderInfo;
		HeaderInfo.NumFaces = NumFaces;
		HeaderInfo.NumVerts = NumVerts;

		// allocate memory
		constexpr int32 HeaderSize = sizeof(FSVOGeometryCache);
		const int32 CoordsSize = sizeof(FCoord) * 3 * NumVerts;
		const int32 IndicesSize = sizeof(int32) * 3 * NumFaces;
		const int32 CacheSize = HeaderSize + CoordsSize + IndicesSize;

		HeaderInfo.Validation.DataSize = CacheSize;

		// empty + add combo to allocate exact amount (without any overhead/slack)
		CollisionData.Empty(CacheSize);
		CollisionData.AddUninitialized(CacheSize);

		// store collisions
		uint8* RawMemory = CollisionData.GetData();
		FSVOGeometryCache* CacheMemory = (FSVOGeometryCache*)RawMemory;
		CacheMemory->Header = HeaderInfo;
		CacheMemory->Verts = 0;
		CacheMemory->Indices = 0;

		FMemory::Memcpy(RawMemory + HeaderSize, GeomExport.VertexBuffer.GetData(), CoordsSize);
		FMemory::Memcpy(RawMemory + HeaderSize + CoordsSize, GeomExport.IndexBuffer.GetData(), IndicesSize);
	}
	
	// By default the Navigation octree stores data in recast coordinates, this converts it back
	static void ConvertCoordDataFromRecast(TArray<FCoord>& Coords)
	{
		FCoord* CoordPtr = Coords.GetData();
		const int32 MaxIt = Coords.Num() / 3;
		for (int32 i = 0; i < MaxIt; i++)
		{
			CoordPtr[0] = -CoordPtr[0];

			const FCoord TmpV = CoordPtr[1];
			CoordPtr[1] = -CoordPtr[2];
			CoordPtr[2] = TmpV;

			CoordPtr += 3;
		}
	}
}
	
// Custom TMap FVector hash function for removing duplicate vertices (Tolerance of 0.01f)
struct FVectorMapKeyFuncs: TDefaultMapKeyFuncs<FVector, int32, false>
{
	// Round vector to nearest 0.01f
	static FORCEINLINE FVector RoundedVec(FVector const& Key)
	{
		return FVector(
            FMath::RoundToFloat(Key.X * 100.f),
            FMath::RoundToFloat(Key.Y * 100.f),
            FMath::RoundToFloat(Key.Z * 100.f));
	}
	static FORCEINLINE FVector GetSetKey(TPair<FVector, int32> const& Element)
	{
		return Element.Key;
	}
	static FORCEINLINE uint32 GetKeyHash(FVector const& Key)
	{
		return GetTypeHash(RoundedVec(Key));
	}
	static FORCEINLINE bool Matches(FVector const& A, FVector const& B)
	{
		return (RoundedVec(A) == RoundedVec(B));
	}
};

//----------------------------------------------------------------------//
//
// FRasteriseWorker Implementation
// 
//----------------------------------------------------------------------//

FRasteriseWorker::FRasteriseWorker(FSVOGenerator& ParentGenerator,
                                   const FBox& InGenerationBounds,
                                   const int32 InWorkerIdx,
                                   const int32 InDivisions):
	GenerationBounds(InGenerationBounds),
	WorkerIdx(InWorkerIdx),
	Divisions(InDivisions),
	ParentGeneratorRef(ParentGenerator),
	NavData(ParentGenerator.SVOData)
{
	// Calculate inclusion bounds
	const float AgentRadius = NavData->AgentRadius;
	if (ParentGenerator.bUseAgentRadius)
	{
		GenerationBounds = GenerationBounds.ExpandBy(AgentRadius);
	}
	// Used for landscape slicing
	GenerationBoundsExpandedForAgent = GenerationBounds.ExpandBy(AgentRadius * 2.f);
	
	for (const FBox& Bounds : ParentGenerator.InclusionBounds)
	{
		FBox Intersect = Bounds.Overlap(GenerationBounds);
		if (Intersect.IsValid)
		{
			// Expand by agent radius
			if (ParentGenerator.bUseAgentRadius)
			{
				Intersect = Intersect.ExpandBy(AgentRadius);
			}
			InclusionBounds.Add(Intersect);
		}
	}
	
	// Gather geometry sources on this (parent) thread
#if PRINT_BENCHMARK
	double CurrentTime = FPlatformTime::Seconds();
#endif // PRINT_BENCHMARK

	PrepareGeometrySources();
	
#if PRINT_BENCHMARK
	printw("ID: %d, PrepareGeometrySources: %f", WorkerIdx, FPlatformTime::Seconds() - CurrentTime);
#endif // PRINT_BENCHMARK

	// Append precise bounds
	if (ParentGenerator.PreciseBoundsCollisionData.Num() > 0)
	{
		AppendCollisionData(ParentGenerator.PreciseBoundsCollisionData, GenerationBounds, false);
	}
}

void FRasteriseWorker::DoWork()
{
	// Only build if NavData is still valid
	if (ParentGeneratorRef.DestFlyingNavData == nullptr)
	{
		return;
	}

#if PRINT_BENCHMARK
	double CurrentTime = FPlatformTime::Seconds();
	double StartTime = CurrentTime;
#endif // PRINT_BENCHMARK
	
	GatherGeometryFromSources();

#if PRINT_BENCHMARK
	printw("ID: %d, GatherGeometryFromSources: %f", WorkerIdx, FPlatformTime::Seconds() - CurrentTime);
#endif // PRINT_BENCHMARK

#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL

#if PRINT_BENCHMARK
	CurrentTime = FPlatformTime::Seconds();
#endif // PRINT_BENCHMARK
	
	RasteriseLayerOne();

#if PRINT_BENCHMARK
	printw("ID: %d, RasteriseAtLayerOne: %f", WorkerIdx, FPlatformTime::Seconds() - CurrentTime);
#endif

#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL

#if PRINT_BENCHMARK
	CurrentTime = FPlatformTime::Seconds();
#endif // PRINT_BENCHMARK

	RasteriseLeafLayer();

#if PRINT_BENCHMARK
	printw("ID: %d, RasteriseLeafNodes: %f", WorkerIdx, FPlatformTime::Seconds() - CurrentTime);
	printw("ID: %d, Rasterise Total: %f", WorkerIdx, FPlatformTime::Seconds() - StartTime);
#endif // PRINT_BENCHMARK

	checkSlow(GetLeafLayer().Num() % 8 == 0);
	checkSlow(GetLayer(1).Num() % 8 == 0);

	DumpAsyncData();

	// Notify parent generator
	ParentGeneratorRef.OnRasteriserWorkerFinished();
}

void FRasteriseWorker::PrepareGeometrySources()
{
	NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(ParentGeneratorRef.GetWorld());
	const FNavigationOctree* NavigationOctree = NavSys.IsValid() ? NavSys->GetMutableNavOctree() : nullptr;
	if (NavigationOctree == nullptr)
	{
		return;
	}

	const FNavDataConfig& OwnerNavDataConfig = ParentGeneratorRef.DestFlyingNavData->GetConfig();

	NavigationRelevantData.Reset();
	for (const FBoxCenterAndExtent& Bounds : InclusionBounds)
	{
		NavigationOctree->FindElementsWithBoundsTest(Bounds, [&OwnerNavDataConfig, this](const FNavigationOctreeElement& Element)
		{
			// Must have valid owner
			bool bShouldUse = Element.GetOwner() != nullptr && Element.ShouldUseGeometry(OwnerNavDataConfig);

			// Don't bother with simulated objects
			if (const UStaticMeshComponent* StaticMeshComp = Cast<const UStaticMeshComponent>(Element.GetOwner()))
			{
				bShouldUse &= !StaticMeshComp->IsSimulatingPhysics();
				// Also query tag to selectively remove meshes from relevant data
				bShouldUse &= !StaticMeshComp->ComponentHasTag(NAME_NavigationIgnoreStaticMeshComponentTag);
			}
			// Needs geometry or has lazy geometry pending
			bShouldUse &= Element.Data->HasGeometry() || Element.Data->IsPendingLazyGeometryGathering();

			// Allow multiple sliced objects, otherwise only add one copy
			bShouldUse &= Element.Data->SupportsGatheringGeometrySlices() || !NavigationRelevantData.Contains(Element.Data);
			
			// Check if we've already added this data
            if (bShouldUse)
            {
            	NavigationRelevantData.Add(Element.Data);
            }
        });
	}
}

void FRasteriseWorker::GatherGeometryFromSources()
{
	if (!NavSys.IsValid())
	{
		return;
	}

	for (TSharedRef<FNavigationRelevantData, ESPMode::ThreadSafe>& ElementData : NavigationRelevantData)
	{
		if (ElementData->IsPendingLazyGeometryGathering() || ElementData->NeedAnyPendingLazyModifiersGathering())
		{
			NavSys->DemandLazyDataGathering(*ElementData);
		}
			    
		const bool bSupportsSlices = ElementData->IsPendingLazyGeometryGathering() && ElementData->SupportsGatheringGeometrySlices();
			    
		// The only thing that supports slicing is landscapes
		if (bSupportsSlices)
		{
			INavRelevantInterface* NavRelevant = Cast<INavRelevantInterface>(ElementData->GetOwner());
			if (NavRelevant)
			{
				// Export geometry 
				FSVOGeometryExport GeomExport;

				NavRelevant->PrepareGeometryExportSync();
					
				// adding a small bump to avoid special case of zero-expansion when tile bounds
				// overlap landscape's tile bounds
				NavRelevant->GatherGeometrySlice(GeomExport, GenerationBoundsExpandedForAgent);

				// Export and append
				TNavStatArray<uint8> CollisionData;	
				SVOGeometryExport::StoreCollisionCache(GeomExport, CollisionData);
				AppendCollisionData(CollisionData, GeomExport.Bounds, false);
				
				continue;
			} else
			{
				UE_LOG(LogFlyingNavSystem, Warning, TEXT("Skipping an element with no longer valid Owner."));
			}
		}

		// Only export geometry if it's available, and if it's not already gathered or we're slicing a different section
		const bool bExportGeometry = ElementData->HasGeometry();
		if (bExportGeometry)
		{
			ValidateAndAppendGeometry(ElementData.Get()); // Sliced landscape geometry not in Recast coords
		}
	}

	// Don't need to keep these around
	NavigationRelevantData.Reset();
}

// Copy of FNavigationRelevantData::FCollisionDataHeader::IsValid
bool FCollisionDataHeaderIsValid(const uint8* RawData, int32 RawDataSize)
{
	const int32 HeaderSize = sizeof(FNavigationRelevantData::FCollisionDataHeader);
	return (RawDataSize == 0) || ((RawDataSize >= HeaderSize) && (((const FNavigationRelevantData::FCollisionDataHeader*)RawData)->DataSize == RawDataSize));
}
	
void FRasteriseWorker::ValidateAndAppendGeometry(const FNavigationRelevantData& ElementData)
{
	// ElementData.IsCollisionDataValid() without ensure
	const bool bIsValid = FCollisionDataHeaderIsValid(ElementData.CollisionData.GetData(), ElementData.CollisionData.Num());
	if (!bIsValid)
	{
		UE_LOG(LogFlyingNavSystem, Error, TEXT("NavOctree element has corrupted collision data! Owner:%s Bounds:%s"), *GetNameSafe(ElementData.GetOwner()), *ElementData.Bounds.ToString());
		return;
	}

	// Append geometry
	const TNavStatArray<uint8>& RawCollisionData = ElementData.CollisionData;
	if (RawCollisionData.Num() == 0)
	{
		return;
	}
	
	TArray<FTransform> PerInstanceTransforms;
	
	// Gather per instance transforms
	FBox ElementBounds(ForceInit);
	
	const bool bUsesPerInstanceTransforms = ElementData.NavDataPerInstanceTransformDelegate.IsBound();
	if (bUsesPerInstanceTransforms)
	{
		ElementData.NavDataPerInstanceTransformDelegate.Execute(GenerationBounds, PerInstanceTransforms);
		if (PerInstanceTransforms.Num() == 0)
		{
			return;
		}

		for (const FTransform& Transform : PerInstanceTransforms)
		{
			ElementBounds += ElementData.Bounds.TransformBy(Transform);
		}
	} else
	{
		ElementBounds = ElementData.Bounds;
	}

	AppendCollisionData(RawCollisionData, ElementBounds, true, PerInstanceTransforms);
}

void FRasteriseWorker::AppendCollisionData(
	const TNavStatArray<uint8>& RawCollisionData,
	const FBox& ElementBounds,
	const bool bConvertFromRecast,
	const TArray<FTransform>& PerInstanceTransforms)
{
	// Coarse bounds check
	bool bIsGeometryRelevant = false;
	for (const FBoxCenterAndExtent& Bounds : InclusionBounds)
	{
		if (ElementBounds.Intersect(Bounds.GetBox()))
		{
			bIsGeometryRelevant = true;
			break;
		}
	}
	if (!bIsGeometryRelevant)
	{
		return;
	}

	if (!ensureAlways(RawCollisionData.Num() > 0))
	{
		UE_LOG(LogFlyingNavSystem, Warning, TEXT("Empty RawCollisionData passed to AppendCollisionData"))
		return;
	}
	
	const FSVOGeometryCache CollisionCache(RawCollisionData.GetData());
	
	const int32 NumRawVerts = CollisionCache.Header.NumVerts;
	const int32 NumRawTris = CollisionCache.Header.NumFaces;
	const int32 NumRawCoords = NumRawVerts * 3;
	const int32 NumRawIndices = NumRawTris * 3;

	// Duplicate with instance transforms
	bool bUsesPerInstanceTransforms = PerInstanceTransforms.Num() > 0;
	int32 NumTransforms = bUsesPerInstanceTransforms ? PerInstanceTransforms.Num() : 1;
	
	const int32 NumVerts = NumRawVerts * NumTransforms;
	const int32 NumTris = NumRawTris * NumTransforms;
	const int32 NumCoords = NumVerts * 3;
	const int32 NumIndices = NumTris * 3;
	
	if (NumIndices == 0)
	{
		return;
	}
	
	// Copy over coords, convert from recast
	TArray<FCoord> GeomRawCoords;
	GeomRawCoords.SetNumUninitialized(NumRawCoords);
	FMemory::Memcpy(GeomRawCoords.GetData(), CollisionCache.Verts, sizeof(FCoord) * NumRawCoords);

#if PRINT_BENCHMARK
	// printw("ID: %d, RawCoords: %d", WorkerIdx, NumRawCoords)
#endif // PRINT_BENCHMARK
		
#if WITH_RECAST
	if (bConvertFromRecast)
	{
		SVOGeometryExport::ConvertCoordDataFromRecast(GeomRawCoords);
	}
#endif // WITH_RECAST

	// Copy over indices, without UV duplicates
	TArray<int32> GeomRawIndices;
	GeomRawIndices.SetNumUninitialized(NumRawIndices);
	
	// First round of compression - Remove UV duplicates. Custom hash has tolerance
	TMap<FVector, int32, FDefaultSetAllocator, FVectorMapKeyFuncs> VertexMap; // Vertex -> Vertex Idx
	TArray<int32> IndexMap; // OldIdx -> NewIdx
	IndexMap.SetNumUninitialized(NumRawVerts);
	
	for (int32 i = 0; i < NumRawVerts; i++)
	{
		const FVector& Vertex = FlyingNavSystem::CoordToVec(GeomRawCoords, i);
		if (VertexMap.Contains(Vertex))
		{
			IndexMap[i] = VertexMap[Vertex];
		} else
		{
			VertexMap.Add(Vertex, i);
			IndexMap[i] = i;
		}
	}
	// Remap indices
	for (int32 i = 0; i < NumRawIndices; i++)
	{
		GeomRawIndices[i] = IndexMap[CollisionCache.Indices[i]];
	}
	
	// Expand geometry along vertex normals to allow for agent radii
	const bool bUseAgentRadius = ParentGeneratorRef.bUseAgentRadius;
	const float AgentRadius = NavData->AgentRadius;
	TArray<FVector> VertexNormals; // Vertex Index -> Vertex Normal
	if (bUseAgentRadius)
	{
		// Calculate vertex normals
		VertexNormals.SetNumZeroed(NumRawVerts);

		for (int32 i = 0; i < NumRawTris; i++)
		{
			const int32 I0 = GeomRawIndices[3*i + 0];
			const int32 I1 = GeomRawIndices[3*i + 1];
			const int32 I2 = GeomRawIndices[3*i + 2];
			
			const FVector& Vertex0 = FlyingNavSystem::CoordToVec(GeomRawCoords, I0);
			const FVector& Vertex1 = FlyingNavSystem::CoordToVec(GeomRawCoords, I1);
			const FVector& Vertex2 = FlyingNavSystem::CoordToVec(GeomRawCoords, I2);
			
			const FVector Edge1 = (Vertex1 - Vertex0).GetSafeNormal();
			const FVector Edge2 = (Vertex2 - Vertex0).GetSafeNormal();
			const FVector Edge3 = (Vertex1 - Vertex2).GetSafeNormal();
			
			const float Angle0 = FMath::Acos(Edge1 |  Edge2);
			const float Angle1 = FMath::Acos(Edge1 |  Edge3);
			const float Angle2 = FMath::Acos(Edge2 | -Edge3);

			const FVector FaceNormal = (Edge2 ^ Edge1).GetSafeNormal();

			// Weight by angle
			VertexNormals[I0] += FaceNormal * Angle0;
			VertexNormals[I1] += FaceNormal * Angle1;
			VertexNormals[I2] += FaceNormal * Angle2;
		}
		// Normalise vertex normals to AgentRadius size
		for (int32 i = 0; i < NumRawVerts; i++)
		{
			VertexNormals[i] = VertexNormals[i].GetSafeNormal() * AgentRadius;
		}
	}
	
	// Bake instance transform
	TArray<FCoord> TransformedGeomCoords;
	TArray<int32> TransformedGeomIndices;
	if (bUsesPerInstanceTransforms)
	{
		TransformedGeomCoords.SetNumUninitialized(NumCoords);
		TransformedGeomIndices.SetNumUninitialized(NumIndices);
		
		for (int32 i = 0; i < NumTransforms; i++)
		{
			const FTransform& InstanceTransform = PerInstanceTransforms[i];
			for (int32 j = 0; j < NumRawVerts; j++)
			{
				FVector& TransformedVertex = FlyingNavSystem::CoordToVec(TransformedGeomCoords, i*NumRawVerts + j);
				TransformedVertex = InstanceTransform.TransformPosition(FlyingNavSystem::CoordToVec(GeomRawCoords, j));
				
				// Normalise and expand geometry by AgentRadius
				if (bUseAgentRadius)
				{
					TransformedVertex += InstanceTransform.TransformVectorNoScale(VertexNormals[j]);
				}
            }

			// Offset indices
			for (int32 j = 0; j < NumRawTris; j++)
			{
				TransformedGeomIndices[i*NumRawIndices + 3*j + 0] = i*NumRawVerts + GeomRawIndices[3*j + 0];
				TransformedGeomIndices[i*NumRawIndices + 3*j + 1] = i*NumRawVerts + GeomRawIndices[3*j + 1];
				TransformedGeomIndices[i*NumRawIndices + 3*j + 2] = i*NumRawVerts + GeomRawIndices[3*j + 2];
            }
		}
	} else
	{
		// Normalise and expand geometry by AgentRadius
		if (bUseAgentRadius)
		{
			for (int32 i = 0; i < NumRawVerts; i++)
			{
				FVector& Vertex = FlyingNavSystem::CoordToVec(GeomRawCoords, i);
				Vertex += VertexNormals[i];
			}
		}

		// Just move over
		TransformedGeomCoords = MoveTemp(GeomRawCoords);
		TransformedGeomIndices = MoveTemp(GeomRawIndices);
	}

	// Only add triangles that are in the bounding volumes
	int32 NumUsedVerts = 0;
	int32 NumUsedCoords = 0; // NumUsedCoords = 3 * NumUsedVerts (3 coords per vert)
	int32 NumUsedIndices = 0;

	// Final triangles
	TArray<FCoord> GeomCoords;
	TArray<int32> GeomIndices;
	GeomIndices.SetNumUninitialized(NumIndices);
	
	FBox TightElementBounds(ForceInit);
	
	// Used for compressing vertex data
	TBitArray<> UsedVerts(false, NumVerts);
	
	for (int32 i = 0; i < NumTris; i++)
	{
		for (const FBoxCenterAndExtent& Bounds : InclusionBounds)
		{
			// Check if triangle overlaps inclusion bounds
			if (UETriBoxOverlap(Bounds.Center, Bounds.Extent, TransformedGeomIndices, TransformedGeomCoords, i))
			{
				const int32 I0 = TransformedGeomIndices[3*i + 0];
				const int32 I1 = TransformedGeomIndices[3*i + 1];
				const int32 I2 = TransformedGeomIndices[3*i + 2];
				
				// Add triangle
				GeomIndices[NumUsedIndices + 0] = I0;
				GeomIndices[NumUsedIndices + 1] = I1;
				GeomIndices[NumUsedIndices + 2] = I2;
				
				if (!UsedVerts[I0])
				{
					UsedVerts[I0] = true;
					NumUsedVerts++;

					TightElementBounds += FlyingNavSystem::CoordToVec(TransformedGeomCoords, I0);
				}
				if (!UsedVerts[I1])
				{
					UsedVerts[I1] = true;
					NumUsedVerts++;
					
					TightElementBounds += FlyingNavSystem::CoordToVec(TransformedGeomCoords, I1);
				}
				if (!UsedVerts[I2])
				{
					UsedVerts[I2] = true;
					NumUsedVerts++;
					
					TightElementBounds += FlyingNavSystem::CoordToVec(TransformedGeomCoords, I2);
				}
				
				NumUsedIndices += 3;
				
				break;
			}
		}
	}

	if (NumIndices > NumUsedIndices)
	{
		GeomIndices.RemoveAt(NumUsedIndices, NumIndices - NumUsedIndices);
	}
	
	if (NumUsedVerts == 0)
	{
		return;
	}
	
#if OPTIMISE_GEOMETRY
	// Compress vertex data
	NumUsedCoords = NumUsedVerts * 3;
	GeomCoords.SetNumUninitialized(NumUsedCoords);

	IndexMap.SetNumUninitialized(NumVerts);

	int32 NumGaps = 0;
	for (int32 i = 0; i < NumVerts; i++)
	{
		if (UsedVerts[i])
		{
			const int32 VertIdx = i - NumGaps;
			IndexMap[i] = VertIdx;
		
			GeomCoords[3*VertIdx + 0] = TransformedGeomCoords[3*i + 0];
			GeomCoords[3*VertIdx + 1] = TransformedGeomCoords[3*i + 1];
			GeomCoords[3*VertIdx + 2] = TransformedGeomCoords[3*i + 2];
		} else
		{
			NumGaps++;
		}
	}
	for (int32 i = 0; i < NumUsedIndices; i++)
	{
		GeomIndices[i] = IndexMap[GeomIndices[i]];
	}
#else
	// Copy all coords
	NumUsedCoords = NumVerts * 3;
	GeomCoords = MoveTemp(TransformedGeomCoords);
#endif
	
	if (NumUsedCoords > 0)
	{
		RawGeometry.Emplace(MoveTemp(GeomCoords), MoveTemp(GeomIndices), TightElementBounds);
	}
}

#if WITH_EDITOR
void FRasteriseWorker::GatherAndDrawGeometry()
{
	PrepareGeometrySources();
	GatherGeometryFromSources();
	DrawGeometry();
}

void FRasteriseWorker::DrawGeometry()
{
	const int32 NumThreads = FlyingNavSystem::GetNumThreads(ParentGeneratorRef.DestFlyingNavData->ThreadSubdivisions, ParentGeneratorRef.bMultithreaded);
	const FColor Colour = FColor::MakeRedToGreenColorFromScalar(static_cast<float>(NumThreads - WorkerIdx) / static_cast<float>(NumThreads));
	UWorld* World = ParentGeneratorRef.GetWorld();

	// Make sure drawing is done on Game Thread
	if (IsInGameThread())
	{
		for (FSVORawGeometryElement& RawGeom : RawGeometry)
		{
			RawGeom.DrawElement(World, Colour);
		}
	} else
	{
		TArray<FSVORawGeometryElement>& RawGeometryCopy = RawGeometry;
		FBoxCenterAndExtent GenerationBoundsCopy = this->GenerationBounds;
		// Draw on the game thread, copying necessary data
		AsyncTask(ENamedThreads::GameThread, [GenerationBoundsCopy, RawGeometryCopy, World, Colour]() {
			DrawDebugBox(World, GenerationBoundsCopy.Center, GenerationBoundsCopy.Extent, Colour, true);
			for (const FSVORawGeometryElement& RawGeom : RawGeometryCopy)
			{
				RawGeom.DrawElement(World, Colour);
			}
        });
	}
}
#endif // WITH_EDITOR

void FRasteriseWorker::RasteriseLayerOne()
{
	// Rough estimate for number of layer one nodes 
	SortedMortonCodes.Reset();

	const morton_t NumLayerOnePerSide = 1ULL << Divisions;
	const morton_t NumLayerOneNodes = NumLayerOnePerSide * NumLayerOnePerSide * NumLayerOnePerSide;

	const FVector OctreeCentre = NavData->Centre;

	const FCoord LayerOneSideLength = NavData->GetSideLengthForLayer(1);
	const FVector LayerOneExtent = NavData->GetExtentForLayer(1);
	const FCoord LayerOneOffset = NavData->GetNodeOffsetForLayer(1);

	// TODO: Better heuristic? Print dif
	SortedMortonCodes.Reserve(FMath::CeilToInt(FMath::Sqrt(static_cast<float>(NumLayerOneNodes))) + 5);

	const morton_t StartCode = WorkerIdx * NumLayerOneNodes;

	for (morton_t i = StartCode; i < StartCode + NumLayerOneNodes; i++)
	{
		const FVector NodeCentre = FlyingNavSystem::MortonToCoord(i, OctreeCentre, LayerOneSideLength, LayerOneOffset);
		const FBox NodeBox = FBox::BuildAABB(NodeCentre, LayerOneExtent);

		// Only test nodes in the boundary
		if (!NodeBox.Intersect(GenerationBounds))
		{
			continue;
		}

		const bool bOverlap = DoesVoxelOverlapGeometry(NodeCentre, LayerOneExtent);

		if (bOverlap)
		{
			SortedMortonCodes.Add(i);
		}

#if ALLOW_CANCEL
		if (ShouldAbort())
		{
			return;
		}
#endif // ALLOW_CANCEL
	}
}

void FRasteriseWorker::RasteriseLeafLayer()
{
	// Make sure we have data to work with
	const int32 NumLayerOneNodes = SortedMortonCodes.Num();

	// Each LayerOne node has 8 leaf nodes
	const int32 NumLeafNodes = NumLayerOneNodes * 8;

	GeneratedLayerOne.Empty(FMath::Max(NumLayerOneNodes * 4, 1)); // kinda ok estimate: TODO: precalc?
	GeneratedLeafLayer.SetNumUninitialized(NumLeafNodes);

	if (NumLayerOneNodes == 0)
	{
		return;
	}

	// This should be prohibited by the minimum limit on subdivisions
	check(Divisions > 0)
		
	// Offset of the SubNode from the Leaf Centre + Morton-Derived Coordinates
	const FCoord SubNodeOffset = NavData->GetSubNodeOffset();
	const FVector SubNodeExtent = NavData->GetSubNodeExtent();

	const FCoord LeafSideLength = NavData->GetSideLengthForLayer(0);
	const FCoord LeafOffset = NavData->GetNodeOffsetForLayer(0);
	const FVector LeafExtent = NavData->GetExtentForLayer(0);

	// Generate and rasterise 8 leaf nodes for each LayerOneNode
	morton_t LastMortonCode = FlyingNavSystem::FirstChildFromAnyChild(SortedMortonCodes[0]);

	for (int32 i = 0; i < NumLayerOneNodes; i++)
	{
		// Make sure childless nodes are added
		const morton_t LayerOneMortonCode = SortedMortonCodes[i];
		GeneratedLayerOne.PadWithChildlessNodes(LastMortonCode, LayerOneMortonCode, i == 0);
		LastMortonCode = LayerOneMortonCode;

		// Go through and rasterise the 8 child leaf nodes
		const morton_t FirstLeafNode = FlyingNavSystem::FirstChildFromParent(LayerOneMortonCode);

		for (morton_t LeafNodeIndex = 0; LeafNodeIndex < 8; LeafNodeIndex++)
		{
			const morton_t LeafNodeMortonCode = FirstLeafNode + LeafNodeIndex;
			const FVector LeafCentre = FlyingNavSystem::MortonToCoord(LeafNodeMortonCode, NavData->Centre, LeafSideLength, LeafOffset);

			FSVOLeafNode& LeafLayerNode = GeneratedLeafLayer[i * 8 + LeafNodeIndex];
			LeafLayerNode.VoxelGrid = LEAF_UNBLOCKED;
			LeafLayerNode.Parent = FSVOLink(1, GeneratedLayerOne.Num());

			const bool bOverlap = DoesVoxelOverlapGeometry(LeafCentre, LeafExtent);
			if (bOverlap)
			{
				RasteriseLeafNode(LeafLayerNode, LeafCentre, SubNodeOffset, SubNodeExtent);
			}

#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL
		}

		FSVONode& LayerOneNode = GeneratedLayerOne.AddNode();
		LayerOneNode.bHasChildren = true;
		LayerOneNode.FirstChild = FSVOLink(0, i * 8);
		LayerOneNode.MortonCode = LayerOneMortonCode;
	}

	// Fill in last block
	GeneratedLayerOne.PadWithChildlessNodes(LastMortonCode, FlyingNavSystem::LastChildFromAnyChild(LastMortonCode) + 1);
}

void FRasteriseWorker::RasteriseLeafNode(FSVOLeafNode& Leaf, const FVector& LeafCentre, const FCoord SubNodeOffset, const FVector& SubNodeExtent)
{
	// Check each of the 64 locations in a leaf, in morton order
	for (small_morton_t i = 0; i < 64; i++)
	{
		const FVector SubNodeCentre = FlyingNavSystem::SmallMortonToCoord(static_cast<small_morton_t>(i), LeafCentre,
                                                         NavData->SubNodeSideLength, SubNodeOffset);
		const bool bOverlap = DoesVoxelOverlapGeometry(SubNodeCentre, SubNodeExtent);
		if (bOverlap)
		{
			Leaf.SetIndexBlocked(i);
		}
	}
}

bool FRasteriseWorker::DoesVoxelOverlapGeometry(const FVector& VoxelCentre, FVector VoxelExtent)
{
	// Test against every triangle
	bool bOverlap = false;

	for (const FSVORawGeometryElement& Element : RawGeometry)
	{
		bOverlap = DoesVoxelOverlapWithRawGeometryData(Element, VoxelCentre, VoxelExtent);
		if (bOverlap) { break; }
	}
	return bOverlap;
}

bool FRasteriseWorker::DoesVoxelOverlapWithRawGeometryData(const FSVORawGeometryElement& Geometry,
                                                           const FVector& VoxelCentre,
                                                           const FVector& VoxelExtent)
{
	// Coarse bounding box check
	if (!Geometry.Bounds.Intersect(FBox::BuildAABB(VoxelCentre, VoxelExtent)))
	{
		return false;
	}
	
	const int32 NumTris = Geometry.GeomIndices.Num() / 3;
	const int32* IndicesPtr = Geometry.GeomIndices.GetData();
	const FCoord* CoordsPtr = Geometry.GeomCoords.GetData();
	
	// Check box against each triangle
	for (int i = 0; i < NumTris; i++)
	{
		if (UETriBoxOverlap(VoxelCentre, VoxelExtent, IndicesPtr, CoordsPtr, i))
		{
			return true;
		}
	}
	return false;
}

void FRasteriseWorker::DumpAsyncData()
{
	RawGeometry.Empty();
}

uint32 FRasteriseWorker::GetAllocatedSize() const
{
	uint32 TotalMemory = sizeof(FRasteriseWorker);
	TotalMemory += InclusionBounds.GetAllocatedSize();

	TotalMemory += RawGeometry.GetAllocatedSize();

	for (const FSVORawGeometryElement& Element : RawGeometry)
	{
		TotalMemory += Element.GeomCoords.GetAllocatedSize();
		TotalMemory += Element.GeomIndices.GetAllocatedSize();
	}

	return TotalMemory;
}

//----------------------------------------------------------------------//
//
// FSVOGenerator Implementation
// 
//----------------------------------------------------------------------//

FSVOGenerator::FSVOGenerator(FFlyingNavigationDataGenerator& ParentGenerator):
	WorkerFinishedEvent(FPlatformProcess::GetSynchEventFromPool(true)),
	AllWorkersDispatchedEvent(FPlatformProcess::GetSynchEventFromPool(true)),
	bAllWorkersDispatched(false),
	DestFlyingNavData(ParentGenerator.DestFlyingNavData),
	SVOData(ParentGenerator.GetBuildingNavigationData().AsShared()),
	TotalBounds(ParentGenerator.TotalBounds),
	InclusionBounds(ParentGenerator.InclusionBounds),
	bMultithreaded(ParentGenerator.DestFlyingNavData->bMultithreaded),
	MaxThreads(ParentGenerator.DestFlyingNavData->MaxThreads),
	bUseAgentRadius(ParentGenerator.DestFlyingNavData->bUseAgentRadius),
	bFinished(false)
{
	SVOData->Clear();
	
	// Update build values
	SVOData->SetBounds(TotalBounds);

	// Build bounds box union
	if (ParentGenerator.DestFlyingNavData->bUseExclusiveBounds && ParentGenerator.DestFlyingNavData->bUsePreciseExclusiveBounds)
	{
		FSVOGeometryExport BoundsGeometry = FlyingNavSystem::FindBoxUnion(InclusionBounds);

		// Debug drawing
		/*
		FlyingNavSystem::FBoxUnion BoxUnion(InclusionBounds);
		for (FlyingNavSystem::FFaceInterval FaceInterval : BoxUnion.FaceIntervals)
		{
			DrawDebugBox(GetWorld(), FaceInterval.Intervals.GetCenter(), FaceInterval.Intervals.GetExtent(), FColor::Purple, true);
		}*/
		
		SVOGeometryExport::StoreCollisionCache(BoundsGeometry, PreciseBoundsCollisionData);
	}
	
	// Check if multithreading is supported (otherwise we have unnecessary overhead)
	if (!FPlatformProcess::SupportsMultithreading())
	{
		bMultithreaded = false;
	}
	NumThreadSubdivisions = FlyingNavSystem::GetThreadSubdivisions(DestFlyingNavData->ThreadSubdivisions, bMultithreaded);
	NumThreads			  =	FlyingNavSystem::GetNumThreads(DestFlyingNavData->ThreadSubdivisions, bMultithreaded);
	
	// Num layers is inclusive of Layer 0, the leaf layer, but not the root layer
	const FCoord MaxSubNodeSize = DestFlyingNavData->MaxDetailSize;
	const int32 NumLayers = FlyingNavSystem::GetNumLayers(SVOData->SideLength, MaxSubNodeSize, NumThreadSubdivisions);

	// SVOData NumNodeLayers doesn't include SubNode layers
	SVOData->NumNodeLayers = NumLayers - 2; // @see MIN_NODE_LAYERS

	const int32 SubNodesPerSide = 1 << NumLayers;
	SVOData->SubNodeSideLength = SVOData->SideLength / SubNodesPerSide;

	// Store agent radius in SVOData
	SVOData->AgentRadius = bUseAgentRadius ? DestFlyingNavData->GetConfig().AgentRadius : 0.f;

	// Calculate the number of divisions from each subvolume to level one node.
	// (SVOData->NumNodeLayers - 1) is from level one up to full volume
	Divisions = SVOData->NumNodeLayers - 1 - NumThreadSubdivisions;
	
	NumRemainingTasks.Set(NumThreads);
	NumRunningThreads.Set(0);
	
	{
		FScopeLock Lock(&WorkerTaskLock);
		WorkerTasks.Reserve(NumThreads);
	}
}

FSVOGenerator::~FSVOGenerator()
{
	// Cleanup the FEvents
	FPlatformProcess::ReturnSynchEventToPool(WorkerFinishedEvent);
	FPlatformProcess::ReturnSynchEventToPool(AllWorkersDispatchedEvent);
}

//----------------------------------------------------------------------//
// SVO Generation
//----------------------------------------------------------------------//
void FSVOGenerator::RasteriseTick(const bool bBlockThread)
{
	// Nothing to do
	if (bAllWorkersDispatched)
	{
		return;
	}

	// Get number of worker tasks
	small_morton_t i;
	{
		FScopeLock Lock(&WorkerTaskLock);
		i = WorkerTasks.Num();
	}
	
	// Setup rasterise workers on main thread
	const int32 GenLayer = SVOData->NumNodeLayers - NumThreadSubdivisions;
	const FCoord GenSideLength = SVOData->GetSideLengthForLayer(GenLayer);
	const FCoord GenNodeOffset = SVOData->GetNodeOffsetForLayer(GenLayer);
	const FVector GenExtent = SVOData->GetExtentForLayer(GenLayer);
	for (; i < static_cast<small_morton_t>(NumThreads); i++)
	{
		WorkerFinishedEvent->Reset();
		if (NumRunningThreads.GetValue() >= MaxThreads)
		{
			if (bBlockThread)
			{
				// A rasterise worker will resume this thread when completed
				WorkerFinishedEvent->Wait();
			} else
			{
				// Come back later
				return;
			}
		}
		
		// Construct generation bounds
		const FVector GenCentre = FlyingNavSystem::SmallMortonToCoord(
			i,
			SVOData->Centre,
			GenSideLength,
			GenNodeOffset
		);
		const FBox GenerationBounds = FBox::BuildAABB(GenCentre, GenExtent);

		// Create a worker thread
		NumRunningThreads.Increment();
		{
			FScopeLock Lock(&WorkerTaskLock);
			WorkerTasks.Add(
				MakeShared<FRasteriseWorkerTask, ESPMode::ThreadSafe>(
					*this,
					GenerationBounds,
					i,
					Divisions
				)
			);
		}
	}
	
	AllWorkersDispatchedEvent->Trigger();
	bAllWorkersDispatched = true;
}

void FSVOGenerator::CollateRasterData()
{
	// Reset NavData
	FSVOLeafLayer& LeafLayer = GetLeafLayer();
	LeafLayer.Reset();
	SVOData->Layers.Empty(MAX_NODE_LAYERS);
	FSVOLayer& LayerOne = SVOData->Layers.Emplace_GetRef();
	
	// Wait for every rasterise worker to be dispatched
	AllWorkersDispatchedEvent->Wait();

#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL

	TArray<FWorkerTaskRef> WorkerTasksCopy;
	{
		// Lock worker tasks for reading back the data
		FScopeLock Lock(&WorkerTaskLock);
		WorkerTasksCopy = WorkerTasks;
	}
	
	// Copy over to NavData
	int32 NumLayerOneNodes = 0;
	for (int32 i = 0; i < WorkerTasksCopy.Num(); i++)
	{
		WorkerTasksCopy[i]->EnsureCompletion();

#if ALLOW_CANCEL
		if (ShouldAbort())
		{
			return;
		}
#endif // ALLOW_CANCEL
		
		NumLayerOneNodes += WorkerTasksCopy[i]->GeneratedLayerOne().Num();
	}
	
	const int32 NumLeafNodes = NumLayerOneNodes * 8;

	LeafLayer.Empty(NumLeafNodes);
	LayerOne.Reserve(NumLayerOneNodes);

	// Copy over in order
	int32 NumCopiedLayerOneNodes = 0;
	int32 NumCopiedLeafNodes = 0;
	for (int32 i = 0; i < NumThreads; i++)
	{
		// Calculate leaf-parent links
		for (int32 j = 0; j < WorkerTasksCopy[i]->GeneratedLayerOne().Num(); j++)
		{
			FSVONode& LayerOneNode = WorkerTasksCopy[i]->GeneratedLayerOne()[j];
			if (LayerOneNode.bHasChildren)
			{
				// Index into GeneratedLayerOnes[i]
				const int32 FirstChildIndex = LayerOneNode.FirstChild.GetNodeIndex();
				// Update leaf children
				for (int32 k = 0; k < 8; k++)
				{
					FSVOLeafNode& LeafNode = WorkerTasksCopy[i]->GeneratedLeafLayer()[FirstChildIndex + k];
					LeafNode.Parent = FSVOLink(1, NumCopiedLayerOneNodes + j);
				}
				LayerOneNode.FirstChild = FSVOLink(0, NumCopiedLeafNodes + FirstChildIndex);
			}
		}

		LeafLayer.Append(WorkerTasksCopy[i]->GeneratedLeafLayer());
		NumCopiedLeafNodes += WorkerTasksCopy[i]->GeneratedLeafLayer().Num();
	
		LayerOne.Append(WorkerTasksCopy[i]->GeneratedLayerOne());
		NumCopiedLayerOneNodes += WorkerTasksCopy[i]->GeneratedLayerOne().Num();
	}

	{
		// Copied all the data we need
		FScopeLock Lock(&WorkerTaskLock);
		WorkerTasks.Reset();
	}
}

void FSVOGenerator::GenerateSVOLayer(const int32 LayerNum)
{
	check(1 < LayerNum && LayerNum < 16 && GetLayers().Num() == LayerNum-1) // Only build layers 2-15 with this function

	FSVOLayer& LastLayer = GetLayer(LayerNum - 1);
	const int32 NumNodesInLastLayer = LastLayer.Num();
	if (NumNodesInLastLayer == 0)
	{
		return;
	}

	// Add a new layer
	FSVOLayer& CurrentLayer = GetLayers().Emplace_GetRef();

	// Because of how childless node padding works, last layer is guaranteed to be a multiple of 8
	const int32 NumNodesInCurrentLayer = NumNodesInLastLayer >> 3; // = NumNodesInLastLayer / 8

	// Used for childless node padding calculation
	morton_t LastMortonCode = FlyingNavSystem::FirstChildFromAnyChild(FlyingNavSystem::ParentFromAnyChild(LastLayer[0].MortonCode));

	for (int32 i = 0; i < NumNodesInCurrentLayer; i++)
	{
		// Make sure childless nodes are added
		const morton_t NodeMortonCode = FlyingNavSystem::ParentFromAnyChild(LastLayer[i * 8].MortonCode);
		CurrentLayer.PadWithChildlessNodes(LastMortonCode, NodeMortonCode, i == 0);
		LastMortonCode = NodeMortonCode;

		// Create new node at the current layer, with children
		FSVONode& CurrentNode = CurrentLayer.AddNode();
		CurrentNode.MortonCode = NodeMortonCode;
		CurrentNode.FirstChild = FSVOLink(LayerNum - 1, i * 8);
		CurrentNode.Parent = FSVOLink::NULL_LINK;
		CurrentNode.bHasChildren = true;

		// Fill in parent links of child nodes:
		for (int32 Child = 0; Child < 8; Child++)
		{
			LastLayer[i * 8 + Child].Parent = FSVOLink(LayerNum, CurrentLayer.Num() - 1);
		}

#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL
	}

	// Fill in last block, if not the root:
	if (LayerNum != SVOData->NumNodeLayers)
	{
		CurrentLayer.PadWithChildlessNodes(LastMortonCode, FlyingNavSystem::LastChildFromAnyChild(LastMortonCode) + 1);
	}
}

int32 FSVOGenerator::FindNodeInLayer(const int32 LayerNum, const morton_t& NodeMorton) const
{
	return SVOData->FindNodeInLayer(LayerNum, NodeMorton);
}

void FSVOGenerator::GenerateNeighbourLinks(const int32 LayerNum)
{
	// TODO: Precalc easy neighbour links? (benchmark)
	FSVOLayer& Layer = GetLayer(LayerNum);
	const int32 MaxCoord = (1 << (SVOData->NumNodeLayers - LayerNum)) - 1;
	// Leaf Layer (0) has 2^NumLayers nodes per side, etc

	for (int32 NodeIdx = 0; NodeIdx < Layer.Num(); NodeIdx++)
	{
		FSVONode& CurrentNode = Layer[NodeIdx];
		const morton_t CurrentMorton = CurrentNode.MortonCode;

		coord_t X, Y, Z;
		libmorton::morton3D_64_decode(CurrentMorton, X, Y, Z);

		for (int i = 0; i < 6; i++)
		{
			const int32 NewX = X + FlyingNavSystem::DeltaX[i], NewY = Y + FlyingNavSystem::DeltaY[i], NewZ = Z + FlyingNavSystem::DeltaZ[i];

			// Check if neighbour is in SVO bounds at all
			if (0 <= NewX && NewX <= MaxCoord && 0 <= NewY && NewY <= MaxCoord && 0 <= NewZ && NewZ <= MaxCoord)
			{
				morton_t NeighbourMorton = libmorton::morton3D_64_encode((coord_t)NewX, (coord_t)NewY, (coord_t)NewZ);

				const int32 NeighbourIdx = SVOData->FindNodeInLayer(LayerNum, NeighbourMorton);
				if (NeighbourIdx != INDEX_NONE)
				{
					CurrentNode.Neighbours[i] = FSVOLink(LayerNum, NeighbourIdx);
				}
				else
				{
					// Check upper layers
					bool bFoundNeighbour = false;
					for (int32 UpperLayer = LayerNum + 1; UpperLayer < SVOData->NumNodeLayers; UpperLayer++)
					{
						// Go up a level
						NeighbourMorton = FlyingNavSystem::ParentFromAnyChild(NeighbourMorton);
						const int32 UpperNeighbourIdx = SVOData->FindNodeInLayer(UpperLayer, NeighbourMorton);
						if (UpperNeighbourIdx != INDEX_NONE)
						{
							CurrentNode.Neighbours[i] = FSVOLink(UpperLayer, UpperNeighbourIdx);
							bFoundNeighbour = true;
							break;
						}
					}

					check(bFoundNeighbour)
					if (!bFoundNeighbour)
					{
						CurrentNode.Neighbours[i] = FSVOLink::NULL_LINK;
					}
				}
			}
			else
			{
				// Out of bounds, no link
				CurrentNode.Neighbours[i] = FSVOLink::NULL_LINK;
			}
		}
	}
}

bool FSVOGenerator::ShouldNodeBeExcluded(const FSVOLink NodeLink)
{
	const FBox NodeBox = SVOData->GetNodeBoxForLink(NodeLink);
	for (const FBox& Box: InclusionBounds)
	{
		if (Box.Intersect(NodeBox))
		{
			return false;
		}
	}
	return true;
}

void FSVOGenerator::ExcludeNodesOutsideBounds()
{
	if (!DestFlyingNavData->bUseExclusiveBounds)
	{
		return;
	}
	
	SVOData->RunOnAllChildlessNodes([this](const FSVOLink& NodeLink)
	{
		if (ShouldNodeBeExcluded(NodeLink))
		{
			if (NodeLink.GetLayerIndex() == 0)
			{
				FSVOLeafNode& LeafNode = SVOData->GetLeafNodeForLink(NodeLink);
				if (LeafNode.IsCompletelyFree())
				{
					LeafNode.VoxelGrid = LEAF_BLOCKED;
				} else
				{
					LeafNode.SetIndexBlocked(NodeLink.GetSubNodeIndex());
				}
				
			} else
			{
				SVOData->GetNodeForLink(NodeLink).bBlocked = true;
			}
		}
	});
}

void FSVOGenerator::FindConnectedComponents()
{
	// Assumes NavData has been generated
	
	const FSVOGraph Graph(SVOData.Get());

	// Count childless nodes
	int32 NumChildlessNodes = 0;
	SVOData->RunOnAllChildlessNodes([&NumChildlessNodes](const FSVOLink StartLink)
	{
		NumChildlessNodes++;
	});
	
	SVOData->NodeComponent.Reset();
	SVOData->NodeComponent.Reserve(NumChildlessNodes);
	SVOData->NumConnectedComponents = 0;

	TResizableCircularQueue<FSVOLink> BFSQueue(FMath::RoundUpToPowerOfTwo(NumChildlessNodes >> 4)); // Seems to work well as an estimate

	TArray<FSVOLink> Neighbours;
	Neighbours.Reserve(128);
	SVOData->RunOnAllChildlessNodes([this, &Graph, &BFSQueue, &Neighbours](const FSVOLink StartLink)
	{
#if ALLOW_CANCEL
		if (ShouldAbort())
		{
			return;
		}
#endif // ALLOW_CANCEL
		
		if (!SVOData->NodeComponent.Contains(StartLink))
		{
			SVOData->NodeComponent.Add(StartLink, SVOData->NumConnectedComponents);
			
			// BFS
			BFSQueue.Reset();
			BFSQueue.Enqueue(StartLink);
			while (!BFSQueue.IsEmpty())
			{
				// Pop next node
				const FSVOLink NodeLink = BFSQueue.PeekNoCheck(); BFSQueue.PopNoCheck();

				// Add neighbours of NodeLink
				Neighbours.Reset();
				Graph.GetNeighbours(NodeLink, Neighbours);
				for (const FSVOLink& NeighbourLink : Neighbours)
				{
					if (!SVOData->NodeComponent.Contains(NeighbourLink))
					{
						SVOData->NodeComponent.Add(NeighbourLink, SVOData->NumConnectedComponents);
						BFSQueue.Enqueue(NeighbourLink);
					}
				}
			}

			SVOData->NumConnectedComponents++;
		}
	});
}

void FSVOGenerator::AddPlaceholderRoot()
{
	SVOData->bValid = true;
	GetLayers().Reset();
	SVOData->NumNodeLayers = 1;
	FSVOLayer& LayerOne = GetLayers().Emplace_GetRef();
	FSVONode& RootNode = LayerOne.AddNode();
	RootNode.Parent = FSVOLink::NULL_LINK;
	RootNode.bHasChildren = false;
}

void FSVOGenerator::DoWork()
{
	bFinished = false;
	
	// Build
	BuildAsync();

	// Cleanup
	DumpAsyncData();
	
	bFinished = true;
}

void FSVOGenerator::BuildAsync()
{
#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL
	
#if PRINT_BENCHMARK
	double CurrentTime = FPlatformTime::Seconds();
#endif // PRINT_BENCHMARK
	
	CollateRasterData();

#if PRINT_BENCHMARK
	printw("Rasterise: %f", FPlatformTime::Seconds() - CurrentTime);
#endif // PRINT_BENCHMARK

	// Only generate SVO if we have geometry
	if (GetLayer(1).Num() == 0)
	{
		// Single root node, indicating free space
		AddPlaceholderRoot();
		return;
	}
	
#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL
	
	check(GetLeafLayer().Num() % 8 == 0);
	check(GetLayer(1).Num() % 8 == 0);

	// Generate SVO layers 2 and up, including root
	for (int32 Layer = 2; Layer <= SVOData->NumNodeLayers; Layer++)
	{
#if PRINT_BENCHMARK
		CurrentTime = FPlatformTime::Seconds();
#endif // PRINT_BENCHMARK

		GenerateSVOLayer(Layer);

#if PRINT_BENCHMARK
		printw("GenerateSVOLayer(%d): %f", Layer, FPlatformTime::Seconds() - CurrentTime);
#endif // PRINT_BENCHMARK

		// Sanity check
		check((Layer == SVOData->NumNodeLayers && GetLayer(Layer).Num() == 1) ||
			  (Layer != SVOData->NumNodeLayers && GetLayer(Layer).Num() % 8 == 0));

#if ALLOW_CANCEL
		if (ShouldAbort())
		{
			return;
		}
#endif // ALLOW_CANCEL
	}

	// Generate neighbour links for FSVONode layers
#if PRINT_BENCHMARK
	CurrentTime = FPlatformTime::Seconds();
#endif // PRINT_BENCHMARK

	for (int32 Layer = 1; Layer <= SVOData->NumNodeLayers; Layer++)
	{
		GenerateNeighbourLinks(Layer);
	}

#if PRINT_BENCHMARK
	printw("GenerateNeighbourLinks: %f", FPlatformTime::Seconds() - CurrentTime);
#endif // PRINT_BENCHMARK

#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL

#if PRINT_BENCHMARK
	CurrentTime = FPlatformTime::Seconds();
#endif // PRINT_BENCHMARK
	
	ExcludeNodesOutsideBounds();

#if PRINT_BENCHMARK
	printw("ExcludeNodes: %f", FPlatformTime::Seconds() - CurrentTime);
#endif // PRINT_BENCHMARK

#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL

#if PRINT_BENCHMARK
	CurrentTime = FPlatformTime::Seconds();
#endif // PRINT_BENCHMARK

	FindConnectedComponents();

#if PRINT_BENCHMARK
	printw("FindConnectedComponents: %f", FPlatformTime::Seconds() - CurrentTime);
#endif // PRINT_BENCHMARK

#if ALLOW_CANCEL
	if (ShouldAbort())
	{
		return;
	}
#endif // ALLOW_CANCEL

	SVOData->bValid = true;
}

void FSVOGenerator::DumpAsyncData()
{
	// TODO: Modifiers
	//Modifiers.Empty();
}

void FSVOGenerator::OnRasteriserWorkerFinished()
{
	NumRunningThreads.Decrement();
	NumRemainingTasks.Decrement();
	WorkerFinishedEvent->Trigger();
}

#if WITH_EDITOR
void FSVOGenerator::DebugDrawSubNodes(UWorld* World, const FSVOLink LeafLink, const FVector& LeafCentre, const FSVOData& NavigationData)
{
	check(LeafLink.GetLayerIndex() == 0);

	const FSVOLeafNode& Leaf = NavigationData.LeafLayer[LeafLink.GetNodeIndex()];
	const FCoord SubNodeSideLength = NavigationData.SubNodeSideLength;
	const FCoord SubNodeOffset = NavigationData.GetSubNodeOffset();
	for (int32 i = 0; i < 64; i++)
	{
		const FVector SubNodeCentre = FlyingNavSystem::SmallMortonToCoord(static_cast<small_morton_t>(i), LeafCentre, SubNodeSideLength,
		                                                 SubNodeOffset);
		//const bool bOverlap = ((Leaf.VoxelGrid >> i) & 1ui64) > 0;
		const bool bOverlap = Leaf.IsIndexBlocked(i);

		if (World)
		{
			if (bOverlap)
			{
				DrawDebugBox(World, SubNodeCentre, FVector(SubNodeSideLength * 0.5f), FColor::Red, true);
			}
			else
			{
				DrawDebugBox(World, SubNodeCentre, FVector(SubNodeSideLength * 0.5f), FColor::Blue, true);
				DebugDrawNeighbours(World, FSVOLink(0, LeafLink.GetNodeIndex(), i), SubNodeCentre, NavigationData);
			}
		}
	}
}

void FSVOGenerator::DebugDrawNeighbours(UWorld* World, const FSVOLink NodeLink, const FVector& VoxelCentre, const FSVOData& NavigationData)
{
	// Neighbours
	const FCoord OctreeSideLength = NavigationData.SideLength;
	const FVector OctreeCentre = NavigationData.Centre;
	const FCoord SubNodeSideLength = NavigationData.SubNodeSideLength;
	const FCoord LeafSideLength = SubNodeSideLength * 4.f;
	const FCoord LeafOffset = NavigationData.GetNodeOffsetForLayer(0);
	const FCoord SubNodeOffset = NavigationData.GetSubNodeOffset();

	const int32 LayerIdx = NodeLink.GetLayerIndex();
	const FCoord VoxelSideLength = LeafSideLength * (1 << LayerIdx);
	const FSVOLeafLayer& LeafLayer = NavigationData.LeafLayer;

	const bool bCurrentNodeIsSubNode = NodeLink.GetLayerIndex() == 0 && !LeafLayer[NodeLink.GetNodeIndex()].
		IsCompletelyFree();

	const FSVOGraph Graph(NavigationData);
	TArray<FSVOLink> Neighbours;
	Graph.GetNeighbours(NodeLink, Neighbours);
	for (const FSVOLink& Neighbour : Neighbours)
	{
		if (!Neighbour.IsValid())
		{
			printw("Null link in neighbours")
		}

		const int32 NeighbourLayerIdx = Neighbour.GetLayerIndex();
		const int32 NeighbourNodeIdx = Neighbour.GetNodeIndex();

		if (NeighbourLayerIdx == 0)
		{
			const FSVOLeafNode& Leaf = LeafLayer[NeighbourNodeIdx];
			const FSVONode& LeafParent = NavigationData.GetLayer(1)[Leaf.Parent.GetNodeIndex()];
			const int32 ChildIdx = NeighbourNodeIdx - LeafParent.FirstChild.GetNodeIndex();
			const morton_t LeafMortonCode = FlyingNavSystem::FirstChildFromParent(LeafParent.MortonCode) + ChildIdx;

			FVector LeafCentre = FlyingNavSystem::MortonToCoord(LeafMortonCode, OctreeCentre, LeafSideLength, LeafOffset);

			if (Leaf.IsCompletelyFree())
			{
				// If the current node is a SubNode
				if (bCurrentNodeIsSubNode)
				{
					const FVector NeighbourDirection = FlyingNavSystem::MajorAxisDirectionFromTwoPoints(VoxelCentre, LeafCentre);
					const FVector NeighbourCentre = VoxelCentre + NeighbourDirection * SubNodeSideLength * 0.5;
					DrawDebugLine(World, VoxelCentre, NeighbourCentre, FColor::Emerald, true);
				}
				else
				{
					const FVector NeighbourDirection = FlyingNavSystem::MajorAxisDirectionFromTwoPoints(VoxelCentre, LeafCentre);
					LeafCentre -= NeighbourDirection * LeafSideLength * 0.5;
					DrawDebugLine(World, VoxelCentre, LeafCentre, FColor::Emerald, true);
				}
			}
			else if (!Leaf.IsIndexBlocked(Neighbour.GetSubNodeIndex()))
			{
				if (bCurrentNodeIsSubNode)
				{
					FVector SubNodeCentre = FlyingNavSystem::MortonToCoord(Neighbour.GetSubNodeIndex(), LeafCentre, SubNodeSideLength,
					                                      SubNodeOffset);
					const FVector NeighbourDirection = FlyingNavSystem::MajorAxisDirectionFromTwoPoints(VoxelCentre, SubNodeCentre);
					SubNodeCentre = VoxelCentre + NeighbourDirection * SubNodeSideLength * 0.5;
					DrawDebugLine(World, VoxelCentre, SubNodeCentre, FColor::Emerald, true);
				}
				else
				{
					FVector SubNodeCentre = FlyingNavSystem::MortonToCoord(Neighbour.GetSubNodeIndex(), LeafCentre, SubNodeSideLength,
					                                      SubNodeOffset);
					const FVector NeighbourDirection = FlyingNavSystem::MajorAxisDirectionFromTwoPoints(VoxelCentre, SubNodeCentre);
					SubNodeCentre -= NeighbourDirection * SubNodeSideLength * 0.5;
					DrawDebugLine(World, VoxelCentre, SubNodeCentre, FColor::Emerald, true);
				}
			}
		}
		else
		{
			const FSVONode& NeighbourNode = NavigationData.GetLayer(NeighbourLayerIdx)[NeighbourNodeIdx];
			const FCoord NeighbourSideLength = LeafSideLength * (1 << NeighbourLayerIdx);
			const FCoord NeighbourOffset = NavigationData.GetOffset(OctreeSideLength, NeighbourSideLength);

			FVector NeighbourCentre = FlyingNavSystem::MortonToCoord(NeighbourNode.MortonCode, OctreeCentre, NeighbourSideLength, NeighbourOffset);

			const FVector NeighbourDirection = FlyingNavSystem::MajorAxisDirectionFromTwoPoints(VoxelCentre, NeighbourCentre);

			if (LayerIdx > NeighbourLayerIdx)
			{
				NeighbourCentre -= NeighbourDirection * NeighbourSideLength * 0.5;
			}
			else if (LayerIdx < NeighbourLayerIdx)
			{
				if (bCurrentNodeIsSubNode)
				{
					NeighbourCentre = VoxelCentre + NeighbourDirection * SubNodeSideLength * 0.5;
				}
				else
				{
					NeighbourCentre = VoxelCentre + NeighbourDirection * VoxelSideLength * 0.5;
				}
			}
			DrawDebugLine(World, VoxelCentre, NeighbourCentre, FColor::Emerald, true);
		}
	}
}

void FSVOGenerator::DebugDraw(UWorld* World, const FSVOData& NavigationData)
{
	//FlushPersistentDebugLines(World);

	const FCoord OctreeSideLength = NavigationData.SideLength;
	const FVector OctreeCentre = NavigationData.Centre;
	const FCoord SubNodeSideLength = NavigationData.SubNodeSideLength;
	const FCoord LeafSideLength = SubNodeSideLength * 4.f;
	const FVector LeafExtent = FVector(LeafSideLength * 0.5f);
	const int32 NumLayers = NavigationData.Layers.Num(); // Includes Leaf Layer, Exclude Root

	const FCoord LeafOffset = NavigationData.GetOffset(OctreeSideLength, LeafSideLength);
	if (World)
	{
		// Draw leaf nodes:
		for (const FSVONode& LayerOneNode : NavigationData.GetLayer(1).Nodes)
		{
			if (LayerOneNode.bHasChildren)
			{
				for (int32 Leaf = 0; Leaf < 8; Leaf++)
				{
					// Coordinate
					const morton_t LeafMortonCode = FlyingNavSystem::FirstChildFromParent(LayerOneNode.MortonCode) + Leaf;
					const FVector LeafCentre = FlyingNavSystem::MortonToCoord(LeafMortonCode, OctreeCentre, LeafSideLength, LeafOffset);

					const FSVOLink& LeafLink = LayerOneNode.FirstChild + Leaf;
					const FSVOLeafNode& LeafNode = NavigationData.LeafLayer[LeafLink.GetNodeIndex()];

					const float LayerProp = (1.f) / static_cast<float>(NumLayers);
					const FColor BoxColor = FColor(255 * LayerProp, 0, 255 * (1 - LayerProp));

					if (LeafNode.IsCompletelyFree())
					{
						DrawDebugBox(World, LeafCentre, LeafExtent, BoxColor, true);
						DebugDrawNeighbours(World, LeafLink, LeafCentre, NavigationData);
					}
					else
					{
						DebugDrawSubNodes(World, LeafLink, LeafCentre, NavigationData);
					}
				}
			}
		}

		// Draw other layers
		for (int32 Layer = 1; Layer < NumLayers; Layer++)
		{
			const FCoord VoxelSideLength = LeafSideLength * (1 << Layer);
			const FVector VoxelExtent(VoxelSideLength * 0.5f);
			const FCoord VoxelOffset = NavigationData.GetOffset(OctreeSideLength, VoxelSideLength);

			for (int32 NodeIndex = 0; NodeIndex < NavigationData.GetLayer(Layer).Num(); NodeIndex++)
			{
				const FSVONode& LayerNode = NavigationData.GetLayer(Layer)[NodeIndex];
				const FSVOLink CurrentNodeLink(Layer, NodeIndex, 0);

				const FVector VoxelCentre = FlyingNavSystem::MortonToCoord(LayerNode.MortonCode, OctreeCentre, VoxelSideLength,
				                                          VoxelOffset);

				if (!LayerNode.bHasChildren)
				{
					// Color
					const float LayerProp = static_cast<float>(Layer + 1) / static_cast<float>(NumLayers);
					const FColor BoxColor = FColor(255 * LayerProp, 0, 255 * (1 - LayerProp));
					DrawDebugBox(World, VoxelCentre, VoxelExtent, BoxColor, true);

					// Draw Neighbours:
					DebugDrawNeighbours(World, CurrentNodeLink, VoxelCentre, NavigationData);
				}
			}
		}
	}
}
#endif // WITH_EDITOR

//----------------------------------------------------------------------//
//
// FFlyingNavigationDataGenerator Implementation
// 
//----------------------------------------------------------------------//

FFlyingNavigationDataGenerator::FFlyingNavigationDataGenerator(AFlyingNavigationData& InDestFlyingNavData):
	bIsPendingBuild(false),
	bIsBuilding(false),
	DestFlyingNavData(&InDestFlyingNavData),
	GeneratorTask(nullptr)
{
	check(DestFlyingNavData);
	UpdateNavigationBounds();
}


void FFlyingNavigationDataGenerator::SyncBuild()
{
	FSVOGenerator Gen(*this);
	Gen.RasteriseTick(true);
	Gen.DoWork();
}


//----------------------------------------------------------------------//
// FNavDataGenerator Interface
//----------------------------------------------------------------------//
bool FFlyingNavigationDataGenerator::RebuildAll()
{
	check(DestFlyingNavData)

	bIsPendingBuild = true;

	return true;
}

void FFlyingNavigationDataGenerator::EnsureBuildCompletion()
{
	// Start build if not already building
	if (!GeneratorTask.IsValid())
	{
		TickAsyncBuild(0);
	}
	
	if (GeneratorTask.IsValid())
	{
		GeneratorTask->EnsureCompletion();
		GeneratorTask.Reset();
		
		bIsPendingBuild = false;
		bIsBuilding = false;

		DestFlyingNavData->OnOctreeGenerationFinished();
	}
}

void FFlyingNavigationDataGenerator::CancelBuild()
{
	bIsPendingBuild = false;
	bIsBuilding = false;
	if (GeneratorTask.IsValid())
	{
		GeneratorTask->Stop();
		// Destroying the thread waits for it to finish
		GeneratorTask.Reset();

		ClearNavigationData();
	}
}

void FFlyingNavigationDataGenerator::TickAsyncBuild(float DeltaSeconds)
{
	// Create new task if we need to
	if (bIsPendingBuild && !bIsBuilding)
	{
		bIsBuilding = true;
		bIsPendingBuild = false;

		GeneratorTask = MakeUnique<FSVOGeneratorTask>(*this);
	}

	if (GeneratorTask.IsValid())
	{
		// Check completion
		if (GeneratorTask->IsFinished())
		{
			bIsBuilding = false;
			GeneratorTask.Reset();

			DestFlyingNavData->OnOctreeGenerationFinished();
		} else
		{
			GeneratorTask->RasteriseTick();
		}
	}
}

void FFlyingNavigationDataGenerator::OnNavigationBoundsChanged()
{
	GetBuildingNavigationData().bValid = false;
	UpdateNavigationBounds();
	

	// TODO: DirtyAreas...
}

void FFlyingNavigationDataGenerator::UpdateNavigationBounds()
{
	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		FBox BoundsSum(ForceInit);
		if (DestFlyingNavData)
		{
			TArray<FBox> SupportedBounds;
			NavSys->GetNavigationBoundsForNavData(*DestFlyingNavData, SupportedBounds);
			InclusionBounds.Reset(SupportedBounds.Num());

			for (const FBox& Box : SupportedBounds)
			{
				InclusionBounds.Add(Box);
				BoundsSum += Box;
			}
		}
		TotalBounds = BoundsSum;

#if	WITH_EDITORONLY_DATA
		DestFlyingNavData->UpdateSubdivisions(TotalBounds.GetSize().GetMax());
#endif // WITH_EDITORONLY_DATA
	}
	else
	{
		TotalBounds = FBox(ForceInit);
	}
}

void FFlyingNavigationDataGenerator::RebuildDirtyAreas(const TArray<FNavigationDirtyArea>& DirtyAreas)
{
	// TODO: Dirty Areas
}
	
bool FFlyingNavigationDataGenerator::IsBuildInProgressCheckDirty() const
{
	return (bIsPendingBuild || bIsBuilding);
}
	
int32 FFlyingNavigationDataGenerator::GetNumRemaningBuildTasks() const
{
	// Main SVOGenerator task + Rasterise workers
	return (bIsPendingBuild || bIsBuilding) + (GeneratorTask ? GeneratorTask->GetNumRemainingTasks() : 1);
}

int32 FFlyingNavigationDataGenerator::GetNumRunningBuildTasks() const
{
	return bIsBuilding + (GeneratorTask ? GeneratorTask->GetNumRunningTasks() : 0);
}

void FFlyingNavigationDataGenerator::ClearNavigationData()
{
	GetBuildingNavigationData().Clear();
}

//----------------------------------------------------------------------//
// debug
//----------------------------------------------------------------------//

uint32 FFlyingNavigationDataGenerator::LogMemUsed() const
{
	uint32 MemUsed = sizeof(FFlyingNavigationDataGenerator);
	UE_LOG(LogFlyingNavSystem, Display, TEXT("    FFlyingNavigationDataGenerator: self %d"), MemUsed);
	
	MemUsed += InclusionBounds.GetAllocatedSize();
	if (GeneratorTask)
	{
		MemUsed += GeneratorTask->GetAllocatedSize();
	}

	UE_LOG(LogFlyingNavSystem, Display, TEXT("    FFlyingNavigationDataGenerator: Total Generator Size %u, Running %d Task(s)"), MemUsed, GeneratorTask->GetNumRunningTasks());
	
	return MemUsed;
}

#if ENABLE_VISUAL_LOG
void FFlyingNavigationDataGenerator::GrabDebugSnapshot(FVisualLogEntry* Snapshot, const FBox& BoundingBox, const FName& CategoryName, ELogVerbosity::Type Verbosity) const
{
	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	const FNavigationOctree* NavOctree = NavSys ? NavSys->GetNavOctree() : nullptr;
	if (Snapshot == nullptr)
	{
		return;
	}

	if (NavOctree == nullptr)
	{
		UE_LOG(LogFlyingNavSystem, Error, TEXT("Failed to vlog navigation data due to %s being NULL"), NavSys == NULL ? TEXT("NavigationSystem") : TEXT("NavOctree"));
		return;
	}

	const FBox RelevantBox = BoundingBox.Overlap(TotalBounds);

	for (int32 Index = 0; Index < NavSys->NavDataSet.Num(); ++Index)
	{
		TArray<FVector> CoordBuffer;
		TArray<int32> Indices;
		TNavStatArray<FVector> Faces;
		const AFlyingNavigationData* NavData = Cast<const AFlyingNavigationData>(NavSys->NavDataSet[Index]);
		if (NavData)
		{
			// Different octree iterator
			NavOctree->FindElementsWithBoundsTest(RelevantBox, [this, &CoordBuffer, &Indices, Snapshot, &CategoryName, Verbosity](const FNavigationOctreeElement& Element)
            {
				const bool bExportGeometry = Element.Data->HasGeometry() && Element.ShouldUseGeometry(DestFlyingNavData->GetConfig());

				TArray<FTransform> InstanceTransforms;
				Element.Data->NavDataPerInstanceTransformDelegate.ExecuteIfBound(Element.Bounds.GetBox(), InstanceTransforms);

				if (bExportGeometry && Element.Data->CollisionData.Num())
				{
					FSVOGeometryCache CachedGeometry(Element.Data->CollisionData.GetData());
					
					const uint32 NumIndices = CachedGeometry.Header.NumFaces * 3;
					Indices.SetNum(NumIndices, false);
					for (uint32 IndicesIdx = 0; IndicesIdx < NumIndices; ++IndicesIdx)
					{
						Indices[IndicesIdx] = CachedGeometry.Indices[IndicesIdx];
					}

					auto AddElementFunc = [&](const FTransform& Transform)
					{
						const uint32 NumVerts = CachedGeometry.Header.NumVerts;
						CoordBuffer.Reset(NumVerts);
						for (uint32 VertIdx = 0; VertIdx < NumVerts * 3; VertIdx += 3)
						{
							CoordBuffer.Add(Transform.TransformPosition(Recast2UnrealPoint(&CachedGeometry.Verts[VertIdx])));
						}

#if ATLEAST_UE5_4
						Snapshot->AddMesh(CoordBuffer, Indices, CategoryName, Verbosity, FColorList::LightGrey.WithAlpha(255));
#else
						Snapshot->AddElement(CoordBuffer, Indices, CategoryName, Verbosity, FColorList::LightGrey.WithAlpha(255));
#endif
					};

					if (InstanceTransforms.Num() == 0)
					{
						AddElementFunc(FTransform::Identity);
					}
					for (const FTransform& InstanceTransform : InstanceTransforms)
					{
						AddElementFunc(InstanceTransform);
					}
				}
			});
		}
	}
}
#endif

#if WITH_EDITOR
void FFlyingNavigationDataGenerator::GatherAndDrawGeometry()
{
	FSVOGenerator Generator(*this);
	FRasteriseWorker Worker(Generator, TotalBounds, 0, MIN_NODE_LAYERS-1);
	Worker.GatherAndDrawGeometry();
}
#endif
