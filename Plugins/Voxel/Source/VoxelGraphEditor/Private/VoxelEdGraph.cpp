// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEdGraph.h"
#include "VoxelGraphToolkit.h"
#include "VoxelGraphSchemaBase.h"
#include "Nodes/VoxelGraphLocalVariableNode.h"
#include "Nodes/VoxelGraphMacroParameterNode.h"

// Needed to load old graphs
constexpr FVoxelGuid GVoxelEdGraphCustomVersionGUID = MAKE_VOXEL_GUID("217DD26F36B54889936719C4ED363B8A");
FCustomVersionRegistration GRegisterVoxelEdGraphCustomVersionGUID(GVoxelEdGraphCustomVersionGUID, 1, TEXT("VoxelEdGraphVer"));

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelEdGraph::SetToolkit(const TSharedRef<FVoxelGraphToolkit>& Toolkit)
{
	ensure(!WeakToolkit.IsValid() || WeakToolkit == Toolkit);
	WeakToolkit = Toolkit;
}

TSharedPtr<FVoxelGraphToolkit> UVoxelEdGraph::GetGraphToolkit() const
{
	return WeakToolkit.Pin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelEdGraph::MigrateIfNeeded()
{
	VOXEL_FUNCTION_COUNTER();

	if (Version != FVersion::LatestVersion)
	{
		MigrateAndReconstructAll();
	}

	ensure(Version == FVersion::LatestVersion);
}

void UVoxelEdGraph::MigrateAndReconstructAll()
{
	VOXEL_FUNCTION_COUNTER();

	// Disable SetDirty
	TGuardValue<bool> SetDirtyGuard(GIsEditorLoadingPackage, true);

	UVoxelGraph* Graph = GetTypedOuter<UVoxelGraph>();
	if (!ensure(Graph))
	{
		return;
	}

	ON_SCOPE_EXIT
	{
		for (const FVoxelGraphParameter& Parameter : Graph->Parameters)
		{
			switch (Parameter.ParameterType)
			{
			default: ensure(false);
			case EVoxelGraphParameterType::Parameter: break;
			case EVoxelGraphParameterType::Input: break;
			case EVoxelGraphParameterType::Output:
			case EVoxelGraphParameterType::LocalVariable:
			{
				ensure(Parameter.DefaultValue == FVoxelPinValue(Parameter.Type.GetExposedType()));
			}
			break;
			}
		}

		for (UEdGraphNode* Node : Nodes)
		{
			Node->ReconstructNode();
		}

		Version = FVersion::LatestVersion;
	};

	if (Version == FVersion::LatestVersion)
	{
		return;
	}

	if (Version < FVersion::SplitInputSetterAndRemoveLocalVariablesDefault)
	{
		TArray<UDEPRECATED_VoxelGraphMacroParameterNode*> ParameterNodes;
		GetNodesOfClass<UDEPRECATED_VoxelGraphMacroParameterNode>(ParameterNodes);

		for (UDEPRECATED_VoxelGraphMacroParameterNode* ParameterNode : ParameterNodes)
		{
			if (ParameterNode->Type == EVoxelGraphParameterType::Input)
			{
				FGraphNodeCreator<UVoxelGraphMacroParameterInputNode> NodeCreator(*this);
				UVoxelGraphMacroParameterInputNode* Node = NodeCreator.CreateNode(false);

				if (const FVoxelGraphParameter* Parameter = ParameterNode->GetParameter())
				{
					Node->bExposeDefaultPin = Parameter->bExposeInputDefaultAsPin_DEPRECATED;
				}

				Node->Guid = ParameterNode->Guid;
				Node->CachedParameter = ParameterNode->CachedParameter;
				Node->NodePosX = ParameterNode->NodePosX;
				Node->NodePosY = ParameterNode->NodePosY;
				NodeCreator.Finalize();

				const UEdGraphPin* OldOutputPin = ParameterNode->GetOutputPin(0);
				UEdGraphPin* NewOutputPin = Node->GetOutputPin(0);

				if (ensure(OldOutputPin) &&
					ensure(NewOutputPin))
				{
					NewOutputPin->CopyPersistentDataFromOldPin(*OldOutputPin);
				}
			}
			else if (ensure(ParameterNode->Type == EVoxelGraphParameterType::Output))
			{
				FGraphNodeCreator<UVoxelGraphMacroParameterOutputNode> NodeCreator(*this);
				UVoxelGraphMacroParameterOutputNode* Node = NodeCreator.CreateNode(false);
				Node->Guid = ParameterNode->Guid;
				Node->CachedParameter = ParameterNode->CachedParameter;
				Node->NodePosX = ParameterNode->NodePosX;
				Node->NodePosY = ParameterNode->NodePosY;
				NodeCreator.Finalize();

				const UEdGraphPin* OldInputPin = ParameterNode->GetInputPin(0);
				UEdGraphPin* NewInputPin = Node->GetInputPin(0);

				if (ensure(OldInputPin) &&
					ensure(NewInputPin))
				{
					NewInputPin->CopyPersistentDataFromOldPin(*OldInputPin);
				}
			}

			ParameterNode->DestroyNode();
		}

		TArray<UVoxelGraphLocalVariableDeclarationNode*> DeclarationNodes;
		GetNodesOfClass<UVoxelGraphLocalVariableDeclarationNode>(DeclarationNodes);

		TSet<FGuid> ParametersWithDeclarationNode;
		for (const UVoxelGraphLocalVariableDeclarationNode* DeclarationNode : DeclarationNodes)
		{
			ParametersWithDeclarationNode.Add(DeclarationNode->Guid);
		}

		for (FVoxelGraphParameter& Parameter : Graph->Parameters)
		{
			if (Parameter.ParameterType != EVoxelGraphParameterType::LocalVariable)
			{
				continue;
			}

			if (!ParametersWithDeclarationNode.Contains(Parameter.Guid))
			{
				FGraphNodeCreator<UVoxelGraphLocalVariableDeclarationNode> NodeCreator(*this);
				UVoxelGraphLocalVariableDeclarationNode* Node = NodeCreator.CreateNode(false);
				Node->Guid = Parameter.Guid;
				Node->CachedParameter = Parameter;
				NodeCreator.Finalize();

				Parameter.DefaultValue.ApplyToPinDefaultValue(*Node->GetInputPin(0));
			}

			Parameter.DefaultValue = {};
			Parameter.Fixup(nullptr);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelEdGraph::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	DelayOnGraphChangedScopeStack.Add(MakeVoxelShared<FVoxelGraphDelayOnGraphChangedScope>());
}

void UVoxelEdGraph::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

	// After undo/redo toolkit may be null
	if (WeakToolkit.IsValid())
	{
		UVoxelGraphSchemaBase::OnGraphChanged(this);
	}

	if (ensure(DelayOnGraphChangedScopeStack.Num() > 0))
	{
		DelayOnGraphChangedScopeStack.Pop();
	}
}