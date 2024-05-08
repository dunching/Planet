// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

struct FVoxelExampleContent
{
	FString Name;
	FString Version;
	int32 Size = 0;
	FString Hash;
	FString DisplayName;
	FString Description;
	TSharedPtr<TSharedPtr<ISlateBrushSource>> Thumbnail;
	TSharedPtr<TSharedPtr<ISlateBrushSource>> Image;
};

class FVoxelExampleContentManager
{
public:
	FVoxelExampleContentManager() = default;

	void OnExamplesReady(const FSimpleDelegate& OnReady);
	TArray<TSharedPtr<FVoxelExampleContent>> GetExamples() const;

private:
	TArray<TSharedPtr<FVoxelExampleContent>> ExampleContents;

	static TSharedRef<TSharedPtr<ISlateBrushSource>> MakeImage(const FString& Hash);

public:
	static FVoxelExampleContentManager& Get();
};