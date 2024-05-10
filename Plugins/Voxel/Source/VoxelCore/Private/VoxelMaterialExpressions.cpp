// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMaterialExpressions.h"

#if WITH_EDITOR
#if VOXEL_ENGINE_VERSION == 504

#include "MaterialHLSLGenerator.h"
#include "MaterialHLSLTree.h"
#include "HLSLTree/HLSLTree.h"

namespace UE::Landscape
{
	bool IsMobileWeightmapTextureArrayEnabled()
	{
		static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("landscape.MobileWeightTextureArray"));
		return CVar->GetInt() != 0;	
	}
}

bool UMaterialExpressionLandscapeLayerBlend::GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const
{
	using namespace UE::HLSLTree;

	bool bNeedsRenormalize = false;
	FTree& Tree = Generator.GetTree();
	const FExpression* ConstantOne = Tree.NewConstant(1.f);
	const FExpression* WeightSumExpression = Tree.NewConstant(0.f);
	TArray<const FExpression*> WeightExpressions;

	WeightExpressions.Empty(Layers.Num());

	for (int32 LayerIdx = 0; LayerIdx < Layers.Num(); LayerIdx++)
	{
		WeightExpressions.Add(nullptr);

		const FLayerBlendInput& Layer = Layers[LayerIdx];

		// LB_AlphaBlend layers are blended last
		if (Layer.BlendType != LB_AlphaBlend)
		{
			const FExpression* WeightExpression = nullptr;
			const bool bTextureArrayEnabled = UE::Landscape::IsMobileWeightmapTextureArrayEnabled();
			verify(GenerateStaticTerrainLayerWeightExpression(Layer.LayerName, Layer.PreviewWeight, bTextureArrayEnabled, Generator, WeightExpression));

			if (WeightExpression)
			{
				switch (Layer.BlendType)
				{
				case LB_WeightBlend:
				{
					// Store the weight plus accumulate the sum of all weights so far
					WeightExpressions[LayerIdx] = WeightExpression;
					WeightSumExpression = Tree.NewAdd(WeightSumExpression, WeightExpression);
				}
				break;
				case LB_HeightBlend:
				{
					bNeedsRenormalize = true;

					// Modify weight with height
					const FExpression* HeightExpression = Layer.HeightInput.AcquireHLSLExpressionOrConstant(Generator, Scope, Layer.ConstHeightInput);
					const FExpression* ModifiedWeight = Tree.NewLerp(Tree.NewConstant(-1.f), ConstantOne, WeightExpression);
					ModifiedWeight = Tree.NewAdd(ModifiedWeight, HeightExpression);
					ModifiedWeight = Tree.NewMin(Tree.NewMax(ModifiedWeight, Tree.NewConstant(0.0001f)), ConstantOne);

					// Store the final weight plus accumulate the sum of all weights so far
					WeightExpressions[LayerIdx] = ModifiedWeight;
					WeightSumExpression = Tree.NewAdd(WeightSumExpression, ModifiedWeight);
				}
				break;
				}
			}
		}
	}

	const FExpression* InvWeightSumExpression = Tree.NewDiv(ConstantOne, WeightSumExpression);
	OutExpression = Tree.NewConstant(0.f);

	for (int32 LayerIdx = 0; LayerIdx < Layers.Num(); LayerIdx++)
	{
		const FLayerBlendInput& Layer = Layers[LayerIdx];

		if (WeightExpressions[LayerIdx])
		{
			const FExpression* LayerExpression = Layer.LayerInput.AcquireHLSLExpressionOrConstant(Generator, Scope, FVector3f(Layer.ConstLayerInput));

			if (bNeedsRenormalize)
			{
				// Renormalize the weights as our height modification has made them non-uniform
				OutExpression = Tree.NewAdd(OutExpression, Tree.NewMul(LayerExpression, Tree.NewMul(InvWeightSumExpression, WeightExpressions[LayerIdx])));
			}
			else
			{
				// No renormalization is necessary, so just add the weights
				OutExpression = Tree.NewAdd(OutExpression, Tree.NewMul(LayerExpression, WeightExpressions[LayerIdx]));
			}
		}
	}

	// Blend in LB_AlphaBlend layers
	for (const FLayerBlendInput& Layer : Layers)
	{
		if (Layer.BlendType == LB_AlphaBlend)
		{
			const FExpression* WeightExpression = nullptr;
			const bool bTextureArrayEnabled = UE::Landscape::IsMobileWeightmapTextureArrayEnabled();
			verify(GenerateStaticTerrainLayerWeightExpression(Layer.LayerName, Layer.PreviewWeight, bTextureArrayEnabled, Generator, WeightExpression));

			if (WeightExpression)
			{
				const FExpression* LayerExpression = Layer.LayerInput.AcquireHLSLExpressionOrConstant(Generator, Scope, FVector3f(Layer.ConstLayerInput));

				// Blend in the layer using the alpha value
				OutExpression = Tree.NewLerp(OutExpression, LayerExpression, WeightExpression);
			}
		}
	}

	return true;
}

bool UMaterialExpressionLandscapeLayerSample::GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const
{
	const bool bTextureArrayEnabled = UE::Landscape::IsMobileWeightmapTextureArrayEnabled();
	return GenerateStaticTerrainLayerWeightExpression(ParameterName, PreviewWeight, bTextureArrayEnabled, Generator, OutExpression);
}

bool UMaterialExpressionLandscapeLayerSwitch::GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const
{
	using namespace UE::HLSLTree;

	const FExpression* Inputs[] = {
		LayerNotUsed.TryAcquireHLSLExpression(Generator, Scope),
		LayerUsed.TryAcquireHLSLExpression(Generator, Scope)
	};

	OutExpression = Generator.GetTree().NewExpression<Material::FExpressionLandscapeLayerSwitch>(Inputs, ParameterName, PreviewUsed!=0);
	return OutExpression != nullptr;
}

bool UMaterialExpressionLandscapeVisibilityMask::GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const
{
	using namespace UE::HLSLTree;

	const FExpression* MaskLayerExpression = nullptr;
	const bool bTextureArrayEnabled = UE::Landscape::IsMobileWeightmapTextureArrayEnabled();
	verify(GenerateStaticTerrainLayerWeightExpression(ParameterName, 0.f, bTextureArrayEnabled, Generator, MaskLayerExpression));

	FTree& Tree = Generator.GetTree();
	const FExpression* ConstantOne = Tree.NewConstant(1.f);
	OutExpression = MaskLayerExpression ? Tree.NewSub(ConstantOne, MaskLayerExpression) : ConstantOne;
	return true;
}

#elif VOXEL_ENGINE_VERSION > 504
#error "Check implementation"
#endif
#endif