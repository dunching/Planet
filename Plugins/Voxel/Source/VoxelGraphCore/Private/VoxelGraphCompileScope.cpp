// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphCompileScope.h"
#include "VoxelGraph.h"
#include "EdGraph/EdGraph.h"
#include "Logging/TokenizedMessage.h"

FVoxelGraphCompileScope* GVoxelGraphCompileScope = nullptr;

FVoxelGraphCompileScope::FVoxelGraphCompileScope(const UVoxelGraph& Graph)
	: Graph(Graph)
{
	PreviousScope = GVoxelGraphCompileScope;
	GVoxelGraphCompileScope = this;

	ScopedMessageConsumer = MakeVoxelUnique<FVoxelScopedMessageConsumer>([this](const TSharedRef<FTokenizedMessage>& Message)
	{
		if (Message->GetSeverity() == EMessageSeverity::Error)
		{
			bHasError = true;
		}

		const FString GraphPath = this->Graph.GetPathName();

		switch (Message->GetSeverity())
		{
		default: ensure(false);
		case EMessageSeverity::Info:
		{
			LOG_VOXEL(Log, "%s: %s", *GraphPath, *Message->ToText().ToString());
		}
		break;
		case EMessageSeverity::Warning:
		{
			LOG_VOXEL(Warning, "%s: %s", *GraphPath, *Message->ToText().ToString());
		}
		break;
		case EMessageSeverity::Error:
		{
			if (IsRunningCookCommandlet() ||
				IsRunningCookOnTheFly())
			{
				// Don't fail cooking
				LOG_VOXEL(Warning, "%s: %s", *GraphPath, *Message->ToText().ToString());
			}
			else
			{
				LOG_VOXEL(Error, "%s: %s", *GraphPath, *Message->ToText().ToString());
			}
		}
		break;
		}

		Messages.Add(Message);
	});
}

FVoxelGraphCompileScope::~FVoxelGraphCompileScope()
{
	ensure(GVoxelGraphCompileScope == this);
	GVoxelGraphCompileScope = PreviousScope;
}