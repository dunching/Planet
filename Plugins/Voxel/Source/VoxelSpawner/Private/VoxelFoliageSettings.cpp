// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelFoliageSettings.h"
#include "VoxelHierarchicalMeshComponent.h"

FVoxelFoliageSettings::FVoxelFoliageSettings(const FVoxelFoliageSettings& GlobalSettings, const FVoxelFoliageSettings& OverridableSettings)
	: FVoxelFoliageSettings(GlobalSettings)
{
	MaterialOverrides = OverridableSettings.MaterialOverrides;
	CopyOverridenParameters(OverridableSettings);
}

void FVoxelFoliageSettings::ApplyToComponent(UInstancedStaticMeshComponent& Component) const
{
	VOXEL_FUNCTION_COUNTER();

#define COPY(VariableName) \
	Component.VariableName = VariableName;

	Component.OverrideMaterials = MaterialOverrides;

	Component.CastShadow = bCastShadow;
	COPY(bCastDistanceFieldIndirectShadow);
	COPY(DistanceFieldIndirectShadowMinVisibility);
	COPY(bOverrideDistanceFieldSelfShadowBias);
	COPY(DistanceFieldSelfShadowBias);
	COPY(bSortTriangles);
	COPY(bReverseCulling);
	COPY(bOverrideLightMapRes);
	COPY(OverriddenLightMapRes);
	COPY(IndirectLightingCacheQuality);
	COPY(LightmapType);
	COPY(bEmissiveLightSource);
	COPY(bAffectDynamicIndirectLighting);
	COPY(bAffectIndirectLightingWhileHidden);
	COPY(bAffectDistanceFieldLighting);
	COPY(bCastDynamicShadow);
	COPY(bCastStaticShadow);
	COPY(bCastVolumetricTranslucentShadow);
	COPY(bCastContactShadow);
	COPY(bSelfShadowOnly);
	COPY(bCastFarShadow);
	COPY(bCastInsetShadow);
	COPY(bCastCinematicShadow);
	COPY(bCastHiddenShadow);
	COPY(bCastShadowAsTwoSided);
	COPY(bLightAttachmentsAsGroup);
	COPY(bExcludeFromLightAttachmentGroup);
	COPY(bSingleSampleShadowFromStationaryLights);
	COPY(LightingChannels);
	COPY(bDisallowNanite);
	COPY(bEvaluateWorldPositionOffset);
	COPY(WorldPositionOffsetDisableDistance);
	COPY(bVisibleInReflectionCaptures);
	COPY(bVisibleInRealTimeSkyCaptures);
	COPY(bVisibleInRayTracing);
	COPY(bRenderInMainPass);
	COPY(bRenderInDepthPass);
	COPY(bReceivesDecals);
	COPY(bOwnerNoSee);
	COPY(bOnlyOwnerSee);
	COPY(bTreatAsBackgroundForOcclusion);
	COPY(bRenderCustomDepth);
	COPY(CustomDepthStencilValue);
	COPY(bVisibleInSceneCaptureOnly);
	COPY(bHiddenInSceneCapture);
	COPY(TranslucencySortPriority);
	COPY(TranslucencySortDistanceOffset);
	COPY(BoundsScale);

#undef COPY

	Component.MarkRenderStateDirty();
}