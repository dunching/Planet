// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Material/VoxelMaterialDefinition.h"
#include "Material/VoxelMaterialDefinitionInstance.h"
#include "Material/MaterialExpressionSampleVoxelParameter.h"
#include "VoxelParameterView.h"

DEFINE_VOXEL_FACTORY(UVoxelMaterialDefinition);

VOXEL_CONSOLE_COMMAND(
	RebuildMaterialTextures,
	"voxel.RebuildMaterialTextures",
	"")
{
	ForEachObjectOfClass<UVoxelMaterialDefinition>([&](UVoxelMaterialDefinition* Definition)
	{
		Definition->QueueRebuildTextures();
	});
}

VOXEL_CONSOLE_COMMAND(
	PurgeMaterialTextures,
	"voxel.PurgeMaterialTextures",
	"")
{
	ForEachObjectOfClass<UVoxelMaterialDefinition>([&](UVoxelMaterialDefinition* Definition)
	{
		for (auto& It : Definition->GuidToParameterData)
		{
			if (!ensure(It.Value))
			{
				continue;
			}

			for (const FProperty& Property : GetStructProperties(It.Value.GetScriptStruct()))
			{
				if (!Property.HasAnyPropertyFlags(CPF_Transient))
				{
					continue;
				}

				Property.ClearValue_InContainer(It.Value.GetPtr());
			}
		}

		Definition->QueueRebuildTextures();
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
void UVoxelMaterialDefinition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FVoxelParameter::FixupParameterArray(this, Parameters);

	// Fixup GuidToParameterData
	{
		TSet<FGuid> ValidGuids;
		for (const FVoxelParameter& Parameter : Parameters)
		{
			const UMaterialExpressionSampleVoxelParameter* Template = UMaterialExpressionSampleVoxelParameter::GetTemplate(Parameter.Type);
			if (!Template)
			{
				continue;
			}

			ValidGuids.Add(Parameter.Guid);

			UScriptStruct* Struct = Template->GetVoxelParameterDataType();
			if (!ensure(Struct))
			{
				continue;
			}

			TVoxelInstancedStruct<FVoxelMaterialParameterData>& ParameterData = GuidToParameterData.FindOrAdd(Parameter.Guid);
			if (ParameterData.GetScriptStruct() != Struct)
			{
				ParameterData = FVoxelInstancedStruct(Struct);
			}
		}

		for (auto It = GuidToParameterData.CreateIterator(); It; ++It)
		{
			if (!ValidGuids.Contains(It.Key()))
			{
				It.RemoveCurrent();
				continue;
			}
			if (!It.Value())
			{
				continue;
			}
			It.Value()->Fixup();
		}
	}

	// Fixup categories
	{
		TArray<FString> CategoriesList;
		for (const FVoxelParameter& Parameter : Parameters)
		{
			CategoriesList.Add(Parameter.Category);
		}
		Categories.Fixup(CategoriesList);
	}

	OnParametersChanged.Broadcast();
	QueueRebuildTextures();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelMaterialDefinitionParameterView : public IVoxelParameterView
{
public:
	TWeakObjectPtr<UVoxelMaterialDefinition> Definition;
	FGuid Guid;

	using IVoxelParameterView::IVoxelParameterView;

	virtual const FVoxelParameter* GetParameter() const override
	{
		if (!ensure(Definition.IsValid()))
		{
			return nullptr;
		}

		const FVoxelParameter* Parameter = Definition->FindParameterByGuid(Guid);
		if (!ensure(Parameter))
		{
			return nullptr;
		}

		return Parameter;
	}
};

class FVoxelMaterialDefinitionParameterRootView : public IVoxelParameterRootView
{
public:
	TWeakObjectPtr<UVoxelMaterialDefinition> Definition;
	FVoxelParameterPath BasePath;
	TVoxelMap<FGuid, TSharedPtr<FVoxelMaterialDefinitionParameterView>> GuidToParameterView;

	using IVoxelParameterRootView::IVoxelParameterRootView;

	virtual TVoxelArray<IVoxelParameterView*> GetChildren() override
	{
		if (!ensure(Definition.IsValid()))
		{
			return {};
		}

		TVoxelArray<IVoxelParameterView*> Children;
		for (const FVoxelParameter& Parameter : Definition->Parameters)
		{
			TSharedPtr<FVoxelMaterialDefinitionParameterView>& ParameterView = GuidToParameterView.FindOrAdd(Parameter.Guid);
			if (!ParameterView)
			{
				ParameterView = MakeVoxelShared<FVoxelMaterialDefinitionParameterView>(*this, BasePath.MakeChild(Parameter.Guid));
				ParameterView->Definition = Definition;
				ParameterView->Guid = Parameter.Guid;
			}
			Children.Add(ParameterView.Get());
		}
		return Children;
	}
	virtual const FVoxelParameterCategories* GetCategories() const override
	{
		if (!ensure(Definition.IsValid()))
		{
			return nullptr;
		}
		return &ConstCast(*Definition).Categories;
	}
};

void UVoxelMaterialDefinition::AddOnChanged(const FSimpleDelegate& Delegate)
{
	OnParametersChanged.Add(Delegate);
}

TSharedPtr<IVoxelParameterRootView> UVoxelMaterialDefinition::GetParameterViewImpl(const FVoxelParameterPath& BasePath)
{
	const TSharedRef<FVoxelMaterialDefinitionParameterRootView> ParameterRootView = MakeVoxelShared<FVoxelMaterialDefinitionParameterRootView>(this);
	ParameterRootView->Definition = this;
	ParameterRootView->BasePath = BasePath;
	return ParameterRootView;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelMaterialDefinition::QueueRebuildTextures()
{
	GVoxelMaterialDefinitionManager->QueueRebuildTextures(*this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelMaterialDefinition::RebuildTextures()
{
	VOXEL_FUNCTION_COUNTER();

	if (!FApp::CanEverRender())
	{
		return;
	}

	using FInstance = UMaterialExpressionSampleVoxelParameter::FInstance;

	struct FInstances
	{
		const UMaterialExpressionSampleVoxelParameter* Template = nullptr;
		FName DebugName;
		TVoxelArray<FInstance> Instances;
	};
	TVoxelAddOnlyMap<FGuid, FInstances> ParameterToInstances;
	{
		VOXEL_SCOPE_COUNTER("Build instances");

		// Add self
		{
			const int32 Index = GVoxelMaterialDefinitionManager->Register_GameThread(*this).Index;

			for (const FVoxelParameter& Parameter : Parameters)
			{
				const UMaterialExpressionSampleVoxelParameter* Template = UMaterialExpressionSampleVoxelParameter::GetTemplate(Parameter.Type);
				if (!Template)
				{
					continue;
				}

				FInstances& Instances = ParameterToInstances.Add_CheckNew(Parameter.Guid);
				Instances.Template = Template;
				Instances.DebugName = GetName() + "_" + Parameter.Name;
				Instances.Instances.Add(FInstance
				{
					this,
					Index,
					Parameter.DefaultValue.AsTerminalValue()
				});
			}
		}

		ForEachObjectOfClass<UVoxelMaterialDefinitionInstance>([&](UVoxelMaterialDefinitionInstance* InstanceObject)
		{
			if (InstanceObject->GetDefinition() != this)
			{
				return;
			}

			const int32 Index = GVoxelMaterialDefinitionManager->Register_GameThread(*InstanceObject).Index;

			const TSharedPtr<IVoxelParameterRootView> ParameterRootView = InstanceObject->GetParameterView();
			if (!ensure(ParameterRootView))
			{
				return;
			}

			for (const IVoxelParameterView* ParameterView : ParameterRootView->GetChildren())
			{
				FInstances* Instances = ParameterToInstances.Find(ParameterView->Path.Leaf());
				if (!Instances)
				{
					checkVoxelSlow(!UMaterialExpressionSampleVoxelParameter::GetTemplate(ParameterView->GetType()));
					continue;
				}

				Instances->Instances.Add(FInstance
				{
					InstanceObject,
					Index,
					ParameterView->GetValue().AsTerminalValue()
				});
			}
		});
	}

	TMap<FGuid, TVoxelInstancedStruct<FVoxelMaterialParameterData>> OldGuidToParameterData = MoveTemp(GuidToParameterData);
	for (auto& It : ParameterToInstances)
	{
		TVoxelInstancedStruct<FVoxelMaterialParameterData>& ParameterData = GuidToParameterData.Add(It.Key);
		if (TVoxelInstancedStruct<FVoxelMaterialParameterData>* ExistingParameterData = OldGuidToParameterData.Find(It.Key))
		{
			ParameterData = MoveTemp(*ExistingParameterData);
		}

		const FInstances& Instances = It.Value;

		UScriptStruct* Struct = Instances.Template->GetVoxelParameterDataType();
		if (!ensure(Struct))
		{
			continue;
		}
		if (!ensure(ParameterData.GetScriptStruct() == Struct))
		{
			ParameterData = FVoxelInstancedStruct(Struct);
		}

		Instances.Template->UpdateVoxelParameterData(
			Instances.DebugName,
			Instances.Instances,
			ParameterData);
	}
}