// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelParameter.h"
#include "VoxelParameterPath.h"

class IVoxelParameterView;
class IVoxelParameterProvider;
class IVoxelParameterRootView;
class UVoxelParameterContainer;
struct FVoxelParameterValueOverride;

extern VOXELGRAPHCORE_API int32 GVoxelMaxRecursionDepth;

class VOXELGRAPHCORE_API IVoxelParameterViewBase
{
public:
	explicit IVoxelParameterViewBase() = default;
	virtual ~IVoxelParameterViewBase() = default;

	virtual TVoxelArray<IVoxelParameterView*> GetChildren() = 0;
	virtual IVoxelParameterView* FindByGuid(const FGuid& Guid);
	virtual IVoxelParameterView* FindByName(FName Name);
	virtual const FVoxelParameterCategories* GetCategories() const = 0;

public:
	IVoxelParameterView* FindChild(const FVoxelParameterPath& Path);

	TArray<FString> GetValidParameters();

public:
	static TVoxelArray<TVoxelArray<IVoxelParameterView*>> GetCommonChildren(TConstVoxelArrayView<IVoxelParameterViewBase*> ParameterViews);
};

class VOXELGRAPHCORE_API IVoxelParameterView : public IVoxelParameterViewBase
{
public:
	IVoxelParameterRootView& RootView;
	const FVoxelParameterPath Path;

	IVoxelParameterView(
		IVoxelParameterRootView& RootView,
		const FVoxelParameterPath& Path)
		: RootView(RootView)
		, Path(Path)
	{
	}
	UE_NONCOPYABLE(IVoxelParameterView)

	bool IsInlineGraph() const;

	FGuid GetGuid() const;
	FName GetName() const;
	FVoxelPinType GetType() const;
	FString GetCategory() const;
	FString GetDescription() const;
	FVoxelPinValue GetValue() const;
#if WITH_EDITOR
	TMap<FName, FString> GetMetaData() const;
#endif

	FVoxelParameter GetAsParameter() const;

	virtual TVoxelArray<IVoxelParameterView*> GetChildren() final override;
	virtual IVoxelParameterView* FindByGuid(const FGuid& Guid) final override;
	virtual IVoxelParameterView* FindByName(FName Name) final override;
	virtual const FVoxelParameterCategories* GetCategories() const final override;

protected:
	virtual const FVoxelParameter* GetParameter() const = 0;

private:
	IVoxelParameterRootView* GetInlineRootView() const;
};

struct VOXELGRAPHCORE_API FVoxelParameterContainerRef
{
	FVoxelParameterPath BasePath;
	TWeakObjectPtr<UVoxelParameterContainer> Container;

	explicit FVoxelParameterContainerRef() = default;

	static FVoxelParameterContainerRef MakeRoot(UVoxelParameterContainer* Container);
	static FVoxelParameterContainerRef Make(const FVoxelParameterPath& BasePath, UVoxelParameterContainer* Container);

	FORCEINLINE bool operator==(const FVoxelParameterContainerRef& Other) const
	{
		return
			BasePath == Other.BasePath &&
			Container == Other.Container;
	}
	FORCEINLINE friend uint32 GetTypeHash(const FVoxelParameterContainerRef& ContainerRef)
	{
		return
			GetTypeHash(ContainerRef.BasePath) ^
			GetTypeHash(ContainerRef.Container);
	}
};

class VOXELGRAPHCORE_API FVoxelParameterViewContext : public TSharedFromThis<FVoxelParameterViewContext>
{
public:
	TVoxelAddOnlySet<TWeakInterfacePtr<IVoxelParameterProvider>> VisitedProviders;

	FVoxelParameterViewContext() = default;

	IVoxelParameterRootView* MakeRootView(
		const FVoxelParameterPath& Path,
		IVoxelParameterProvider* Provider);

	void AddContainer(const FVoxelParameterContainerRef& Container);
	void AddContainerToForceEnable(const FVoxelParameterContainerRef& Container);
	void AddValueOverrideToIgnore(const FVoxelParameterContainerRef& Container, const FVoxelParameterPath& Path);
	void RemoveValueOverrideToIgnore(const FVoxelParameterContainerRef& Container, const FVoxelParameterPath& Path);
	const FVoxelParameterValueOverride* FindValueOverride(const FVoxelParameterPath& Path) const;

private:
	// List of containers to apply on top of the view
	TVoxelArray<FVoxelParameterContainerRef> Containers;
	// Used by detail panel to always display override value
	TVoxelSet<FVoxelParameterContainerRef> ContainersToForceEnable;
	// Used to skip ourselves when checking CanResetToDefault
	TVoxelMap<FVoxelParameterContainerRef, TVoxelSet<FVoxelParameterPath>> ContainerToValueOverridesToIgnore;
	// RootView cache, used to keep alive the same IVoxelParameterView* across multiple GetChildren
	TVoxelMap<
		TPair<FVoxelParameterPath, TWeakInterfacePtr<IVoxelParameterProvider>>,
		TSharedPtr<IVoxelParameterRootView>> PathAndProviderToRootView;
};

class VOXELGRAPHCORE_API IVoxelParameterRootView : public IVoxelParameterViewBase
{
public:
	explicit IVoxelParameterRootView(const TWeakInterfacePtr<IVoxelParameterProvider>& Provider);

	FVoxelParameterViewContext& GetContext() const
	{
		check(Context);
		return *Context;
	}

private:
	TSharedPtr<FVoxelParameterViewContext> SharedContext;
	FVoxelParameterViewContext* Context = nullptr;

	friend FVoxelParameterViewContext;
};