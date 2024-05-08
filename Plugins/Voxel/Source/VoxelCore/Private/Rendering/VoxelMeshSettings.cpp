// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Rendering/VoxelMeshSettings.h"
#include "Rendering/VoxelMeshComponent.h"

void FVoxelMeshSettings::ApplyToComponent(UVoxelMeshComponent& Component) const
{
	VOXEL_FUNCTION_COUNTER();

#define COPY(VariableName) \
	Component.VariableName = VariableName;

	Component.CastShadow = bCastShadow;
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
	COPY(BoundsExtension);

#undef COPY

	Component.MarkRenderStateDirty();
}