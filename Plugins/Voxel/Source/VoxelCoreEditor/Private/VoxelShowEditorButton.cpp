// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"

class FShowEditorButtonDetails : public IDetailCustomization
{
public:
	FShowEditorButtonDetails() = default;

private:
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override
	{
		TArray<TWeakObjectPtr<UObject>> Objects;
		DetailLayout.GetObjectsBeingCustomized(Objects);

		UClass* Class = Objects[0]->GetClass();
		for (const TWeakObjectPtr<UObject>& Object : Objects)
		{
			while (!Object->IsA(Class))
			{
				Class = Class->GetSuperClass();
			}
		}

		TArray<UFunction*> Functions;
		for (const UClass* ClassIt = Class; ClassIt != UObject::StaticClass(); ClassIt = ClassIt->GetSuperClass())
		{
			Functions.Append(GetClassFunctions(ClassIt));
		}

		for (UFunction* Function : Functions)
		{
			if (!Function->HasMetaData(STATIC_FNAME("ShowEditorButton")))
			{
				continue;
			}

			FString Category = Function->GetMetaData("Category");
			if (Category.IsEmpty())
			{
				Category = "Functions";
			}

			const FString CategoryPriority = Function->GetMetaData("CategoryPriority");
			if (!CategoryPriority.IsEmpty())
			{
				FVoxelEditorUtilities::SetSortOrder(DetailLayout, *Category, ECategoryPriority::Type(2 + FCString::Atoi(*CategoryPriority)), 0);
			}

			const bool bAdvanced = Function->HasMetaData("AdvancedDisplay");

			DetailLayout.EditCategory(*Category)
			.AddCustomRow(Function->GetToolTipText(), bAdvanced)
			.NameContent()
			[
				SNew(SVoxelDetailText)
				.Text(Function->GetDisplayNameText())
			]
			.ValueContent()
			[
				SNew(SVoxelDetailButton)
				.Text(Function->GetDisplayNameText())
				.ToolTipText(Function->GetToolTipText())
				.OnClicked_Lambda([=]
				{
					FScopedTransaction Transaction(*Function->GetName(), Function->GetDisplayNameText(), nullptr);

					for (const TWeakObjectPtr<UObject>& Object : Objects)
					{
						FVoxelObjectUtilities::InvokeFunctionWithNoParameters(Object.Get(), Function);
					}

					return FReply::Handled();
				})
			];
		}
	}
};

VOXEL_RUN_ON_STARTUP_EDITOR(RegisterShowEditorButton)
{
	VOXEL_FUNCTION_COUNTER();

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	TSet<FName> ClassNames;
	ForEachObjectOfClass<UFunction>([&](UFunction* Function)
	{
		if (!Function->HasMetaData(STATIC_FNAME("ShowEditorButton")))
		{
			return;
		}
		if (Function->Children)
		{
			ensureMsgf(false, TEXT("Function %s has ShowEditorButton but has parameters!"), *Function->GetDisplayNameText().ToString());
			Function->RemoveMetaData(STATIC_FNAME("ShowEditorButton"));
			return;
		}

		UClass* Class = Function->GetOuterUClass();
		check(Class);
		ClassNames.Add(Class->GetFName());
	});

	for (FName ClassName : ClassNames)
	{
		ensure(!PropertyModule.GetClassNameToDetailLayoutNameMap().Contains(ClassName));

		PropertyModule.RegisterCustomClassLayout(ClassName, FOnGetDetailCustomizationInstance::CreateLambda([]
		{
			return MakeVoxelShared<FShowEditorButtonDetails>();
		}));
	}
	PropertyModule.NotifyCustomizationModuleChanged();
}