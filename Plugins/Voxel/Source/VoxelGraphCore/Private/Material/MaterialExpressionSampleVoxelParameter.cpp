// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Material/MaterialExpressionSampleVoxelParameter.h"
#include "MaterialCompiler.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture2DArray.h"
#include "Material/VoxelMaterialDefinition.h"
#include "Runtime/Engine/Private/Materials/MaterialUniformExpressions.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelMaterialTextureMemory);

TMap<TWeakObjectPtr<UMaterialExpressionVoxelParameter>, FGuid> GMaterialExpressionVoxelParameterToGuid;
TMap<FGuid, TWeakObjectPtr<UMaterialExpressionVoxelParameter>> GGuidToMaterialExpressionVoxelParameter;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UMaterialExpressionVoxelParameter::UMaterialExpressionVoxelParameter()
{
	bHidePreviewWindow = true;
}

UObject* UMaterialExpressionVoxelParameter::GetReferencedTexture() const
{
	return FVoxelTextureUtilities::GetDefaultTexture2DArray();
}

#if WITH_EDITOR
uint32 UMaterialExpressionVoxelParameter::GetOutputType(int32 OutputIndex)
{
	return MCT_TextureExternal;
}

int32 UMaterialExpressionVoxelParameter::Compile(FMaterialCompiler* Compiler, const int32 OutputIndex)
{
	FGuid& Guid = GMaterialExpressionVoxelParameterToGuid.FindOrAdd(this);
	if (!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}
	GGuidToMaterialExpressionVoxelParameter.Add(Guid, this);

	return Compiler->ExternalTexture(Guid);
}

void UMaterialExpressionVoxelParameter::GetCaption(TArray<FString>& OutCaptions) const
{
	if (!MaterialDefinition)
	{
		OutCaptions.Add("Voxel Parameter");
		OutCaptions.Add("<invalid>");
		return;
	}

	const FVoxelParameter* Parameter = MaterialDefinition->Parameters.FindByKey(ParameterGuid);
	if (!Parameter)
	{
		OutCaptions.Add("Voxel Parameter");
		OutCaptions.Add("<invalid>");
		return;
	}

	OutCaptions.Add("Voxel " + Parameter->Type.ToString() + " Parameter");
	OutCaptions.Add(Parameter->Name.ToString());
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UMaterialExpressionSampleVoxelParameter::UMaterialExpressionSampleVoxelParameter()
{
	bHidePreviewWindow = true;

#if WITH_EDITOR
	CachedInputs.Empty();
	CachedInputs.Add(&ParameterInput);
	CachedInputs.Add(&MaterialIdInput);
#endif
}

const UMaterialExpressionSampleVoxelParameter* UMaterialExpressionSampleVoxelParameter::GetTemplate(const FVoxelPinType& Type)
{
	static TMap<FVoxelPinType, TSubclassOf<UMaterialExpressionSampleVoxelParameter>> TypeToGetVoxelMaterialClass;
	if (TypeToGetVoxelMaterialClass.Num() == 0)
	{
		for (const TSubclassOf<UMaterialExpressionSampleVoxelParameter> Class : GetDerivedClasses<UMaterialExpressionSampleVoxelParameter>())
		{
			if (Class->HasAnyClassFlags(CLASS_Abstract))
			{
				continue;
			}
			TypeToGetVoxelMaterialClass.Add(Class.GetDefaultObject()->GetVoxelParameterType(), Class);
		}
	}
	const TSubclassOf<UMaterialExpressionSampleVoxelParameter> Class = TypeToGetVoxelMaterialClass.FindRef(Type);
	if (!Class)
	{
		return nullptr;
	}
	return Class.GetDefaultObject();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UObject* UMaterialExpressionSampleVoxelParameter::GetReferencedTexture() const
{
	return FVoxelTextureUtilities::GetDefaultTexture2D();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
#if VOXEL_ENGINE_VERSION >= 503
TArrayView<FExpressionInput*> UMaterialExpressionSampleVoxelParameter::GetInputsView()
{
	return CachedInputs;
}
#else
const TArray<FExpressionInput*> UMaterialExpressionSampleVoxelParameter::GetInputs()
{
	return CachedInputs;
}
#endif

FExpressionInput* UMaterialExpressionSampleVoxelParameter::GetInput(const int32 InputIndex)
{
	return CachedInputs[InputIndex];
}

FName UMaterialExpressionSampleVoxelParameter::GetInputName(const int32 InputIndex) const
{
	if (InputIndex == 0)
	{
		return "Parameter";
	}
	else if (InputIndex == 1)
	{
		return "Material Id";
	}
	else
	{
		ensure(false);
		return {};
	}
}

uint32 UMaterialExpressionSampleVoxelParameter::GetInputType(const int32 InputIndex)
{
	if (InputIndex == 0)
	{
		return MCT_TextureExternal;
	}
	else if (InputIndex == 1)
	{
		return MCT_Float;
	}
	else
	{
		ensure(false);
		return {};
	}
}

bool UMaterialExpressionSampleVoxelParameter::IsInputConnectionRequired(const int32 InputIndex) const
{
	if (InputIndex == 0)
	{
		return true;
	}
	else if (InputIndex == 1)
	{
		return false;
	}
	else
	{
		ensure(false);
		return {};
	}
}

void UMaterialExpressionSampleVoxelParameter::GetConnectorToolTip(
	const int32 InputIndex,
	const int32 OutputIndex,
	TArray<FString>& OutToolTip)
{
	if (InputIndex == -1)
	{
		return;
	}

	if (InputIndex == 0)
	{
		OutToolTip.Add("Parameter to use. To pass this through a Material Function set the Input Type to Texture External");
	}
	else if (InputIndex == 1)
	{
		OutToolTip.Add("If not set will query the Voxel Material Ids and linearly blend the output");
	}
	else
	{
		ensure(false);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
int32 UMaterialExpressionSampleVoxelParameter::Compile(FMaterialCompiler* Compiler, const int32 OutputIndex)
{
	const int32 ParameterCode = ParameterInput.Compile(Compiler);
	if (ParameterCode == -1)
	{
		return Compiler->Errorf(TEXT("Invalid Parameter, connect a Voxel Parameter node to the Parameter pin"));
	}

	FMaterialUniformExpression* UniformExpression = Compiler->GetParameterUniformExpression(ParameterCode);
	if (!ensure(UniformExpression))
	{
		return Compiler->Errorf(TEXT("Invalid Parameter"));
	}

	struct FHack : FMaterialUniformExpressionExternalTexture
	{
		static FGuid GetGuid(FMaterialUniformExpression* UniformExpression)
		{
			return static_cast<FHack*>(UniformExpression)->ExternalTextureGuid;
		}
	};
	const FGuid Guid = FHack::GetGuid(UniformExpression);

	UMaterialExpressionVoxelParameter* VoxelParameterExpression = GGuidToMaterialExpressionVoxelParameter.FindRef(Guid).Get();
	if (!ensure(VoxelParameterExpression))
	{
		return Compiler->Errorf(TEXT("Invalid Parameter"));
	}

	UVoxelMaterialDefinition* MaterialDefinition = VoxelParameterExpression->MaterialDefinition;
	const FGuid ParameterGuid = VoxelParameterExpression->ParameterGuid;

	if (!MaterialDefinition)
	{
		Compiler->AppendExpressionError(VoxelParameterExpression, TEXT("MaterialDefinition is null"));
		return Compiler->Error(TEXT("Invalid Parameter"));
	}

	const FVoxelParameter* Parameter = MaterialDefinition->Parameters.FindByKey(ParameterGuid);
	if (!Parameter)
	{
		Compiler->AppendExpressionError(
			VoxelParameterExpression,
			*FString::Printf(
				TEXT("No parameter with GUID %s on %s"),
				*ParameterGuid.ToString(),
				*MaterialDefinition->GetPathName()));

		return Compiler->Error(TEXT("Invalid Parameter"));
	}

	if (Parameter->Type != GetVoxelParameterType())
	{
		Compiler->AppendExpressionError(
			VoxelParameterExpression,
			*FString::Printf(
				TEXT("Parameter %s has type %s but type %s was expected"),
				*Parameter->Name.ToString(),
				*Parameter->Type.ToString(),
				*GetVoxelParameterType().ToString()));

		return Compiler->Errorf(
			TEXT("%s is expecting a parameter of type %s"),
			*GetDescription(),
			*GetVoxelParameterType().ToString());
	}

	if (!ensure(MaterialDefinition) ||
		!ensure(Parameter->Type == GetVoxelParameterType()) ||
		!ensure(FVoxelTextureUtilities::GetDefaultTexture2D()) ||
		!ensure(FVoxelTextureUtilities::GetDefaultTexture2DArray()))
	{
		return -1;
	}

	ensure(MaterialDefinition->GuidToParameterData.Contains(ParameterGuid));
	TVoxelInstancedStruct<FVoxelMaterialParameterData>& ParameterData = MaterialDefinition->GuidToParameterData.FindOrAdd(ParameterGuid);

	const FString Name = "VOXELPARAM_" + MaterialDefinition->GetGuid().ToString() + "_" + ParameterGuid.ToString();
	const int32 MaterialId = MaterialIdInput.Compile(Compiler);
	const int32 PreviewMaterialId = Compiler->ScalarParameter("PreviewMaterialId", 0.f);

	UMaterialExpressionCustom* Custom = NewObject<UMaterialExpressionCustom>();
	Custom->Inputs.Empty();

	TArray<int32> Inputs;
	CompileVoxel(
		*Compiler,
		*Custom,
		*ParameterData,
		FName(Name),
		MaterialId,
		PreviewMaterialId,
		Inputs);

	if (!ensure(!Inputs.Contains(-1)))
	{
		return -1;
	}

	return Compiler->CustomExpression(Custom, 0, Inputs);
}

void UMaterialExpressionSampleVoxelParameter::GetCaption(TArray<FString>& OutCaptions) const
{
	FString Name = GetClass()->GetDisplayNameText().ToString();
	Name.RemoveFromStart("Material Expression ");
	OutCaptions.Add(Name);
}
#endif