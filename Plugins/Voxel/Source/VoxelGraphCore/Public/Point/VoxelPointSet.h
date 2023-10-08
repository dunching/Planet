// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Point/VoxelPointId.h"
#include "VoxelQueryParameter.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelPointSet.generated.h"

class FVoxelBufferBuilder;
struct FVoxelGraphNodeRef;
INTELLISENSE_ONLY(FVoxelPointId);

struct VOXELGRAPHCORE_API FVoxelPointAttributes
{
	static const FName Id;
	static const FName Mesh;
	static const FName Position;
	static const FName Rotation;
	static const FName Scale;
	static const FName Normal;
	static const FName CustomData;
	static const FName ParentCustomData;
	static const FName ActorClass;

	static FName MakeActor(const FName Name)
	{
		return "Actor." + Name;
	}
	static FName MakeParent(const FName Name)
	{
		return "Parent." + Name;
	}

	static void AddDefaulted(
		FVoxelBufferBuilder& BufferBuilder,
		FName AttributeName,
		int32 NumToAdd);
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelPointSet
	: public FVoxelVirtualStruct
	, public TSharedFromThis<FVoxelPointSet>
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	FORCEINLINE int32 Num() const
	{
		return PrivateNum;
	}
	FORCEINLINE bool Contains(const FName Name) const
	{
		return Attributes.Contains(Name);
	}
	FORCEINLINE TSharedPtr<const FVoxelBuffer> Find(const FName Name) const
	{
		return Attributes.FindRef(Name);
	}
	FORCEINLINE const TVoxelMap<FName, TSharedPtr<const FVoxelBuffer>>& GetAttributes() const
	{
		return Attributes;
	}

public:
	void SetNum(int32 NewNum);
	void Add(FName Name, const TSharedRef<const FVoxelBuffer>& Buffer);
	FVoxelQuery MakeQuery(const FVoxelQuery& Query) const;
	bool CheckNum(const FVoxelNode* Node, int32 BufferNum) const;
	TSharedRef<FVoxelPointSet> Gather(const FVoxelInt32Buffer& Indices) const;
	TVoxelArray<FVoxelFloatBuffer> FindCustomDatas(const FVoxelGraphNodeRef& NodeRef) const;
	int64 GetAllocatedSize() const;
	const TVoxelAddOnlySet<FVoxelPointId>& GetPointIdToIndex() const;

public:
	static TSharedRef<const FVoxelPointSet> Merge(TVoxelArray<TSharedRef<const FVoxelPointSet>> PointSets);

public:
	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelBuffer>::Value>::Type>
	void Add(const FName Name, const T& Buffer)
	{
		this->Add(Name, MakeSharedCopy(Buffer));
	}

private:
	int32 PrivateNum = 0;
	TVoxelMap<FName, TSharedPtr<const FVoxelBuffer>> Attributes;

	mutable FVoxelFastCriticalSection_NoPadding PointIdToIndexCriticalSection;
	mutable TVoxelAddOnlySet<FVoxelPointId> PointIdToIndex_RequiresLock;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelPointSetQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

	TSharedPtr<const FVoxelPointSet> PointSet;
};

#define FindVoxelPointSetAttributeImpl(PointSet, Name, Type, VariableName, ReturnValue) \
	if ((PointSet).Num() > 0) \
	{ \
		const TSharedPtr<const FVoxelBuffer> Buffer = (PointSet).Find(Name); \
		if (!Buffer) \
		{ \
			VOXEL_MESSAGE(Error, "{0}: Missing attribute {1}", this, Name); \
			return ReturnValue; \
		} \
		if (!Buffer->IsA<Type>()) \
		{ \
			VOXEL_MESSAGE(Error, "{0}: attribute {1} has type {2}, but type {3} was expectd", \
				this, \
				Name, \
				Buffer->GetBufferType().ToString(), \
				FVoxelPinType::Make<Type>().ToString()); \
			return ReturnValue; \
		} \
	} \
	const Type VariableName = (PointSet).Num() == 0 ? Type::MakeEmpty() : *CastChecked<Type>((PointSet).Find(Name).ToSharedRef());

#define FindVoxelPointSetAttribute(PointSet, Name, Type, VariableName) \
	FindVoxelPointSetAttributeImpl(PointSet, Name, Type, VariableName, {})

#define FindVoxelPointSetAttributeVoid(PointSet, Name, Type, VariableName) \
	FindVoxelPointSetAttributeImpl(PointSet, Name, Type, VariableName,)

#define FindVoxelPointSetOptionalAttribute(PointSet, Name, Type, VariableName, Default) \
	TSharedPtr<const FVoxelBuffer> VOXEL_APPEND_LINE(_Buffer); \
	{ \
		VOXEL_APPEND_LINE(_Buffer) = (PointSet).Find(Name); \
		if (VOXEL_APPEND_LINE(_Buffer) && \
			!VOXEL_APPEND_LINE(_Buffer)->IsA<Type>()) \
		{ \
			VOXEL_MESSAGE(Error, "{0}: attribute {1} has type {2}, but type {3} was expectd", \
				this, \
				Name, \
				VOXEL_APPEND_LINE(_Buffer)->GetBufferType().ToString(), \
				FVoxelPinType::Make<Type>().ToString()); \
			VOXEL_APPEND_LINE(_Buffer) = nullptr; \
		} \
	} \
	const Type VariableName = \
		VOXEL_APPEND_LINE(_Buffer) \
		? static_cast<const Type&>(*VOXEL_APPEND_LINE(_Buffer)) \
		: Type(Default);
