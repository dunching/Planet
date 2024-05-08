// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

class FVoxelQuery;
class FVoxelDependency;
class FVoxelTransformRefImpl;

class IVoxelTransformProvider
{
public:
	virtual ~IVoxelTransformProvider() = default;

	virtual FName GetName() const = 0;
	virtual FMatrix GetTransform() const = 0;
};

class VOXELGRAPHCORE_API FVoxelTransformRef
{
public:
	FORCEINLINE static FVoxelTransformRef Identity()
	{
		return {};
	}
	static FVoxelTransformRef Make(const AActor& Actor);
	static FVoxelTransformRef Make(const USceneComponent& Component);
	static FVoxelTransformRef Make(const TSharedRef<const IVoxelTransformProvider>& Provider);

	static void NotifyTransformChanged(const USceneComponent& Component);

public:
	FVoxelTransformRef() = default;

	bool IsIdentity() const;
	FMatrix Get(const FVoxelQuery& Query) const;
	FMatrix Get_NoDependency() const;
	FVoxelTransformRef Inverse() const;
	FVoxelTransformRef operator*(const FVoxelTransformRef& Other) const;

	DECLARE_DELEGATE_OneParam(FOnChanged, const FMatrix& NewTransform);
	void AddOnChanged(const FOnChanged& OnChanged, bool bFireNow = true) const;

	FORCEINLINE bool operator==(const FVoxelTransformRef& Other) const
	{
		return Impl == Other.Impl;
	}
	FORCEINLINE bool operator!=(const FVoxelTransformRef& Other) const
	{
		return Impl != Other.Impl;
	}
	FORCEINLINE friend uint32 GetTypeHash(const FVoxelTransformRef& Ref)
	{
		return GetTypeHash(Ref.Impl);
	}

private:
	explicit FVoxelTransformRef(const TSharedRef<const FVoxelTransformRefImpl>& Impl)
		: Impl(Impl)
	{
	}

	TSharedPtr<const FVoxelTransformRefImpl> Impl;
};