// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFoliageSettings.generated.h"

class UVoxelHierarchicalMeshComponent;

USTRUCT(BlueprintType)
struct VOXELSPAWNER_API FVoxelFoliageSettings : public FVoxelOverridableSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ShowOverride = IfOverriden))
	TArray<TObjectPtr<UMaterialInterface>> MaterialOverrides;

	// Controls whether the foliage should cast a shadow or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	bool bCastShadow = true;

	// Whether to use the mesh distance field representation (when present) for shadowing indirect lighting (from lightmaps or skylight) on Movable components.
	// This works like capsule shadows on skeletal meshes, except using the mesh distance field so no physics asset is required.
	// The StaticMesh must have 'Generate Mesh Distance Field' enabled, or the project must have 'Generate Mesh Distance Fields' enabled for this feature to work.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (DisplayName = "Distance Field Indirect Shadow", Overridable))
	bool bCastDistanceFieldIndirectShadow = false;

	// Controls how dark the dynamic indirect shadow can be.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (UIMin = "0", UIMax = "1", DisplayName = "Distance Field Indirect Shadow Min Visibility", EditCondition = "bCastDistanceFieldIndirectShadow", Overridable))
	float DistanceFieldIndirectShadowMinVisibility = 0.1f;

	// Whether to override the DistanceFieldSelfShadowBias setting of the static mesh asset with the DistanceFieldSelfShadowBias of this component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	bool bOverrideDistanceFieldSelfShadowBias = false;

	// Useful for reducing self shadowing from distance field methods when using world position offset to animate the mesh's vertices.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "bOverrideDistanceFieldSelfShadowBias", Overridable))
	float DistanceFieldSelfShadowBias = 0.f;

	// Enable dynamic sort mesh's triangles to remove ordering issue when rendered with a translucent material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (UIMin = "0", UIMax = "1", DisplayName = "Sort Triangles (Experimental)", Overridable))
	bool bSortTriangles = false;

	// Controls whether the static mesh component's backface culling should be reversed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	bool bReverseCulling = false;

	// Whether to override the lightmap resolution defined in the static mesh.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	bool bOverrideLightMapRes = false;

	// Light map resolution to use on this component, used if bOverrideLightMapRes is true and there is a valid StaticMesh.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (ClampMax = 4096, EditCondition="bOverrideLightMapRes", Overridable))
	int32 OverriddenLightMapRes = 64;

	// Quality of indirect lighting for Movable primitives.  This has a large effect on Indirect Lighting Cache update time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	TEnumAsByte<EIndirectLightingCacheQuality> IndirectLightingCacheQuality = ILCQ_Point;

	// Controls the type of lightmap used for this component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	ELightmapType LightmapType = ELightmapType::Default;

	// Whether the primitive will be used as an emissive light source.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	bool bEmissiveLightSource = false;

	// Controls whether the primitive should inject light into the Light Propagation Volume.  This flag is only used if CastShadow is true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", Overridable))
	bool bAffectDynamicIndirectLighting = true;

	// Controls whether the primitive should affect indirect lighting when hidden. This flag is only used if bAffectDynamicIndirectLighting is true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "bAffectDynamicIndirectLighting", Overridable))
	bool bAffectIndirectLightingWhileHidden = false;

	// Controls whether the primitive should affect dynamic distance field lighting methods.  This flag is only used if CastShadow is true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	bool bAffectDistanceFieldLighting = false;

	// Controls whether the primitive should cast shadows in the case of non precomputed shadowing.  This flag is only used if CastShadow is true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", DisplayName = "Dynamic Shadow", Overridable))
	bool bCastDynamicShadow = true;

	// Whether the object should cast a static shadow from shadow casting lights.  This flag is only used if CastShadow is true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", DisplayName = "Static Shadow", Overridable))
	bool bCastStaticShadow = true;

	// Whether the object should cast a volumetric translucent shadow.
	// Volumetric translucent shadows are useful for primitives with smoothly changing opacity like particles representing a volume,
	// But have artifacts when used on highly opaque surfaces.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", DisplayName = "Volumetric Translucent Shadow", Overridable))
	bool bCastVolumetricTranslucentShadow = false;

	// Whether the object should cast contact shadows.
	// This flag is only used if CastShadow is true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", DisplayName = "Contact Shadow", Overridable))
	bool bCastContactShadow = true;

	// When enabled, the component will only cast a shadow on itself and not other components in the world.
	// This is especially useful for first person weapons, and forces bCastInsetShadow to be enabled.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", Overridable))
	bool bSelfShadowOnly = false;

	// When enabled, the component will be rendering into the far shadow cascades (only for directional lights).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", DisplayName = "Far Shadow", Overridable))
	bool bCastFarShadow = false;

	// Whether this component should create a per-object shadow that gives higher effective shadow resolution.
	// Useful for cinematic character shadowing. Assumed to be enabled if bSelfShadowOnly is enabled.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", DisplayName = "Dynamic Inset Shadow", Overridable))
	bool bCastInsetShadow = false;

	// Whether this component should cast shadows from lights that have bCastShadowsFromCinematicObjectsOnly enabled.
	// This is useful for characters in a cinematic with special cinematic lights, where the cost of shadowmap rendering of the environment is undesired.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", Overridable))
	bool bCastCinematicShadow = false;

	// If true, the primitive will cast shadows even if bHidden is true.
	// Controls whether the primitive should cast shadows when hidden.
	// This flag is only used if CastShadow is true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", DisplayName = "Hidden Shadow", Overridable))
	bool bCastHiddenShadow = false;

	// Whether this primitive should cast dynamic shadows as if it were a two sided material.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (EditCondition = "CastShadow", DisplayName = "Shadow Two Sided", Overridable))
	bool bCastShadowAsTwoSided = false;

	// Whether to light this component and any attachments as a group.  This only has effect on the root component of an attachment tree.
	// When enabled, attached component shadowing settings like bCastInsetShadow, bCastVolumetricTranslucentShadow, etc, will be ignored.
	// This is useful for improving performance when multiple movable components are attached together.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	bool bLightAttachmentsAsGroup = false;

	// If set, then it overrides any bLightAttachmentsAsGroup set in a parent.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	bool bExcludeFromLightAttachmentGroup = false;

	// Whether the whole component should be shadowed as one from stationary lights, which makes shadow receiving much cheaper.
	// When enabled shadowing data comes from the volume lighting samples precomputed by Lightmass, which are very sparse.
	// This is currently only used on stationary directional lights.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	bool bSingleSampleShadowFromStationaryLights = false;

	// Lighting channels that placed foliage will be assigned. Lights with matching channels will affect the foliage.
	// These channels only apply to opaque materials, direct lighting, and dynamic lighting and shadowing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (Overridable))
	FLightingChannels LightingChannels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bDisallowNanite = false;

	// Whether to evaluate World Position Offset.
	// This is only used when running with r.OptimizedWPO=1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bEvaluateWorldPositionOffset = true;

	// Distance at which to disable World Position Offset for an entire instance (0 = Never disable WPO).
	// NOTE: Currently works with Nanite only.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	int32 WorldPositionOffsetDisableDistance = 0;

	// If true, this component will be visible in reflection captures.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bVisibleInReflectionCaptures = true;

	// If true, this component will be visible in real-time sky light reflection captures.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bVisibleInRealTimeSkyCaptures = true;

	// If true, this component will be visible in ray tracing effects. Turning this off will remove it from ray traced reflections, shadows, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bVisibleInRayTracing = true;

	// If true, this component will be rendered in the main pass (z prepass, basepass, transparency)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bRenderInMainPass = true;

	// If true, this component will be rendered in the depth pass even if it's not rendered in the main pass
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (EditCondition = "!bRenderInMainPass", Overridable))
	bool bRenderInDepthPass = true;

	// Whether the primitive receives decals.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bReceivesDecals = true;

	// If this is True, this component won't be visible when the view actor is the component's owner, directly or indirectly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bOwnerNoSee = false;

	// If this is True, this component will only be visible when the view actor is the component's owner, directly or indirectly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bOnlyOwnerSee = false;

	// Treat this primitive as part of the background for occlusion purposes. This can be used as an optimization to reduce the cost of rendering skyboxes, large ground planes that are part of the vista, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	bool bTreatAsBackgroundForOcclusion = false;

	// If true, this component will be rendered in the CustomDepth pass (usually used for outlines)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (DisplayName = "Render CustomDepth Pass", Overridable))
	bool bRenderCustomDepth = false;

	// Optionally write this 0-255 value to the stencil buffer in CustomDepth pass (Requires project setting or r.CustomDepth == 3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (UIMin = "0", UIMax = "255", EditCondition = "bRenderCustomDepth", DisplayName = "CustomDepth Stencil Value", Overridable))
	int32 CustomDepthStencilValue = 0;

	// When true, will only be visible in Scene Capture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (DisplayName = "Visible In Scene Capture Only", Overridable))
	bool bVisibleInSceneCaptureOnly = false;

	// When true, will not be captured by Scene Capture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (DisplayName = "Hidden In Scene Capture", Overridable))
	bool bHiddenInSceneCapture = false;

	// Translucent objects with a lower sort priority draw behind objects with a higher priority.
	// Translucent objects with the same priority are rendered from back-to-front based on their bounds origin.
	// This setting is also used to sort objects being drawn into a runtime virtual texture.
	// Ignored if the object is not translucent.  The default priority is zero.
	// Warning: This should never be set to a non-default value unless you know what you are doing, as it will prevent the renderer from sorting correctly.
	// It is especially problematic on dynamic gameplay effects.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	int32 TranslucencySortPriority = 0;

	// Modified sort distance offset for translucent objects in world units.
	// A positive number will move the sort distance further and a negative number will move the distance closer.
	// Ignored if the object is not translucent.
	// Warning: Adjusting this value will prevent the renderer from correctly sorting based on distance.  Only modify this value if you are certain it will not cause visual artifacts.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (Overridable))
	float TranslucencySortDistanceOffset = 0.f;

	// Scales the bounds of the object.
	// This is useful when using World Position Offset to animate the vertices of the object outside of its bounds.
	// Warning: Increasing the bounds of an object will reduce performance and shadow quality!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering", meta = (UIMin = "1", UIMax = "10.0", Overridable))
	float BoundsScale = 1.f;

	FVoxelFoliageSettings() = default;
	FVoxelFoliageSettings(const FVoxelFoliageSettings& GlobalSettings, const FVoxelFoliageSettings& OverridableSettings);

	void ApplyToComponent(UInstancedStaticMeshComponent& Component) const;
};