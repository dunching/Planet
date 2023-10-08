// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Material/VoxelMaterial.h"
#include "Material/VoxelMaterialDefinitionInterface.h"
#include "VoxelTaskGroup.h"

void FVoxelComputedMaterialParameter::Append(const FVoxelComputedMaterialParameter& Other)
{
	ScalarParameters.Append(Other.ScalarParameters);
	VectorParameters.Append(Other.VectorParameters);
	TextureParameters.Append(Other.TextureParameters);
	DynamicParameters.Append(Other.DynamicParameters);
	Resources.Append(Other.Resources);
}

TSharedRef<FVoxelMaterialRef> FVoxelComputedMaterial::MakeMaterial_GameThread() const
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	TSharedPtr<FVoxelMaterialRef> Material;
	if (ParentMaterial)
	{
		Material = FVoxelMaterialRef::MakeInstance(ParentMaterial->GetMaterial());
		// Make sure to keep the parent resources alive
		Material->AddResource(ParentMaterial);
	}
	else
	{
		Material = FVoxelMaterialRef::MakeInstance(nullptr);
	}

	Material->SetDynamicParameter_GameThread(
		STATIC_FNAME("GVoxelMaterialDefinitionManager"),
		GVoxelMaterialDefinitionManager->DynamicParameter);

	for (const auto& It : Parameters.ScalarParameters)
	{
		Material->SetScalarParameter_GameThread(It.Key, It.Value);
	}
	for (const auto& It : Parameters.VectorParameters)
	{
		Material->SetVectorParameter_GameThread(It.Key, It.Value);
	}
	for (const auto& It : Parameters.TextureParameters)
	{
		Material->SetTextureParameter_GameThread(It.Key, It.Value.Get());
	}
	for (const auto& It : Parameters.DynamicParameters)
	{
		Material->SetDynamicParameter_GameThread(It.Key, It.Value.ToSharedRef());
	}

	return Material.ToSharedRef();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TVoxelFutureValue<FVoxelComputedMaterial> FVoxelMaterial::Compute(const FVoxelQuery& Query) const
{
	VOXEL_FUNCTION_COUNTER();

	TVoxelArray<TVoxelFutureValue<FVoxelComputedMaterialParameter>> ComputedParameters;
	ComputedParameters.Reserve(Parameters.Num());

	for (const TSharedRef<const FVoxelMaterialParameter>& Parameter : Parameters)
	{
		ensure(!Parameter->Node.IsExplicitlyNull());
		FVoxelTaskGroup::Get().GetReferencer().AddRef(Parameter);

		TVoxelFutureValue<FVoxelComputedMaterialParameter> ComputedParameter = Parameter->Compute(Query);
		if (!ComputedParameter.IsValid())
		{
			ComputedParameter = FVoxelComputedMaterialParameter();
		}

		ComputedParameters.Add(ComputedParameter);
	}

	return
		MakeVoxelTask(STATIC_FNAME("FVoxelMaterial.Compute"))
		.Dependencies(ComputedParameters)
		.Execute<FVoxelComputedMaterial>([ParentMaterial = ParentMaterial, Parameters = Parameters, ComputedParameters]
		{
			TVoxelAddOnlyMap<FName, const FVoxelMaterialParameter*> NameToParameter;
			NameToParameter.Reserve(2 * Parameters.Num());

			FVoxelComputedMaterialParameter MergedParameter;
			for (int32 Index = 0; Index < Parameters.Num(); Index++)
			{
				const FVoxelMaterialParameter& Parameter = *Parameters[Index];
				const FVoxelComputedMaterialParameter& ComputedParameter = ComputedParameters[Index].Get_CheckCompleted();

				ComputedParameter.ForeachKey([&](const FName Name)
				{
					if (const FVoxelMaterialParameter** ParameterPtr = NameToParameter.Find(Name))
					{
						VOXEL_MESSAGE(Error, "Material parameter {0} is set by {1} and {2}",
							Name,
							(**ParameterPtr).Node,
							Parameter.Node);
						return;
					}
					NameToParameter.Add_CheckNew(Name, &Parameter);
				});

				MergedParameter.Append(ComputedParameter);
			}

			FVoxelComputedMaterial Material;
			Material.ParentMaterial = ParentMaterial;
			Material.Parameters = MergedParameter;
			return Material;
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMaterialPinType::Convert(const bool bSetObject, TWeakObjectPtr<UMaterialInterface>& Object, FVoxelMaterial& Struct) const
{
	if (bSetObject)
	{
		if (Struct.ParentMaterial)
		{
			Object = Struct.ParentMaterial->GetWeakMaterial();
		}
	}
	else
	{
		Struct.ParentMaterial = FVoxelMaterialRef::Make(ConstCast(Object.Get()));
	}
}