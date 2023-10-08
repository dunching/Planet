// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPointSet.h"
#include "VoxelNode.h"
#include "VoxelBufferBuilder.h"
#include "VoxelBufferUtilities.h"
#include "VoxelPositionQueryParameter.h"

const FName FVoxelPointAttributes::Id = "Id";
const FName FVoxelPointAttributes::Mesh = "Mesh";
const FName FVoxelPointAttributes::Position = "Position";
const FName FVoxelPointAttributes::Rotation = "Rotation";
const FName FVoxelPointAttributes::Scale = "Scale";
const FName FVoxelPointAttributes::Normal = "Normal";
const FName FVoxelPointAttributes::CustomData = "CustomData";
const FName FVoxelPointAttributes::ParentCustomData = MakeParent("CustomData");
const FName FVoxelPointAttributes::ActorClass = MakeActor("Class");

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPointAttributes::AddDefaulted(
	FVoxelBufferBuilder& BufferBuilder,
	const FName AttributeName,
	const int32 NumToAdd)
{
	ensure(AttributeName != Id);

	if (NumToAdd == 0)
	{
		return;
	}

	if (AttributeName == Rotation &&
		BufferBuilder.InnerType.Is<FQuat>())
	{
		BufferBuilder.BulkAdd(FQuat::Identity, NumToAdd);
	}
	else if (AttributeName == Scale &&
		BufferBuilder.InnerType.Is<FVector>())
	{
		BufferBuilder.BulkAdd(FVector::OneVector, NumToAdd);
	}
	else if (AttributeName == Normal &&
		BufferBuilder.InnerType.Is<FVector>())
	{
		BufferBuilder.BulkAdd(FVector::UpVector, NumToAdd);
	}
	else
	{
		BufferBuilder.AddZeroed(NumToAdd);
	}
}

void FVoxelPointSet::SetNum(const int32 NewNum)
{
	ensure(NewNum > 0);
	ensure(PrivateNum == 0);
	ensure(Attributes.Num() == 0);
	PrivateNum = NewNum;
}

void FVoxelPointSet::Add(const FName Name, const TSharedRef<const FVoxelBuffer>& Buffer)
{
	if (!ensure(
		Buffer->Num() == 1 ||
		Buffer->Num() == Num()))
	{
		return;
	}

	Attributes.Add(Name, Buffer);
}

FVoxelQuery FVoxelPointSet::MakeQuery(const FVoxelQuery& Query) const
{
	ensure(Num() > 0);

	const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
	Parameters->Add<FVoxelPointSetQueryParameter>().PointSet = AsShared();

	const TSharedPtr<const FVoxelBuffer> Position = Attributes.FindRef(FVoxelPointAttributes::Position);
	if (Position &&
		Position->IsA<FVoxelVectorBuffer>())
	{
		Parameters->Add<FVoxelPositionQueryParameter>().Initialize(Position->AsChecked<FVoxelVectorBuffer>());
	}

	return Query.MakeNewQuery(Parameters);
}

bool FVoxelPointSet::CheckNum(const FVoxelNode* Node, int32 BufferNum) const
{
	if (BufferNum != 1 &&
		BufferNum != Num())
	{
		VOXEL_MESSAGE(Error, "{0}: Buffer has Num={1}, but attributes have Num={2}",
			Node,
			BufferNum,
			Num());
		return false;
	}

	return true;
}

TSharedRef<FVoxelPointSet> FVoxelPointSet::Gather(const FVoxelInt32Buffer& Indices) const
{
	VOXEL_SCOPE_COUNTER_FORMAT_COND(Num() > 1024, "FVoxelPointSet::Gather Num=%d", Num());

	if (Indices.Num() == 0)
	{
		return MakeVoxelShared<FVoxelPointSet>();
	}

	const TSharedRef<FVoxelPointSet> Result = MakeVoxelShared<FVoxelPointSet>();;
	Result->SetNum(Indices.Num());

	for (const auto& It : Attributes)
	{
		Result->Add(It.Key, FVoxelBufferUtilities::Gather(*It.Value, Indices));
	}

	return Result;
}

TVoxelArray<FVoxelFloatBuffer> FVoxelPointSet::FindCustomDatas(const FVoxelGraphNodeRef& NodeRef) const
{
	TVoxelArray<FVoxelFloatBuffer> CustomDatas;
	while (true)
	{
		const FName Name(FVoxelPointAttributes::CustomData, CustomDatas.Num());
		if (!Contains(Name))
		{
			break;
		}

		TSharedPtr<const FVoxelBuffer> Buffer = Find(Name);
		if (!Buffer)
		{
			CustomDatas.Add(0.f);
			continue;
		}

		if (!Buffer->IsA<FVoxelFloatBuffer>())
		{
			VOXEL_MESSAGE(Error, "{0}: attribute {1} has type {2}, but type {3} was expectd",
				NodeRef,
				Name,
				Buffer->GetBufferType().ToString(),
				FVoxelPinType::Make<FVoxelFloatBuffer>().ToString());

			CustomDatas.Add(0.f);
			continue;
		}

		CustomDatas.Add(CastChecked<FVoxelFloatBuffer>(*Buffer));
	}
	return CustomDatas;
}

int64 FVoxelPointSet::GetAllocatedSize() const
{
	int64 AllocatedSize = Attributes.GetAllocatedSize();
	for (const auto& It : Attributes)
	{
		AllocatedSize += It.Value->GetAllocatedSize();
	}
	AllocatedSize += PointIdToIndex_RequiresLock.GetAllocatedSize();
	return AllocatedSize;
}

const TVoxelAddOnlySet<FVoxelPointId>& FVoxelPointSet::GetPointIdToIndex() const
{
	VOXEL_SCOPE_LOCK(PointIdToIndexCriticalSection);

	if (PointIdToIndex_RequiresLock.Num() == Num())
	{
		return PointIdToIndex_RequiresLock;
	}
	ensure(PointIdToIndex_RequiresLock.Num() == 0);

	const TSharedPtr<const FVoxelBuffer> Buffer = Find(FVoxelPointAttributes::Id);
	if (!Buffer ||
		!ensure(Buffer->IsA<FVoxelPointIdBuffer>()))
	{
		return PointIdToIndex_RequiresLock;
	}

	VOXEL_FUNCTION_COUNTER_NUM(Num(), 128);

	PointIdToIndex_RequiresLock.Reserve(Num());

	const FVoxelPointIdBuffer& PointIds = CastChecked<FVoxelPointIdBuffer>(*Buffer);
	if (!ensure(PointIds.Num() == Num()))
	{
		return PointIdToIndex_RequiresLock;
	}

	PointIdToIndex_RequiresLock.BulkAdd(PointIds);
	return PointIdToIndex_RequiresLock;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<const FVoxelPointSet> FVoxelPointSet::Merge(TVoxelArray<TSharedRef<const FVoxelPointSet>> PointSets)
{
	VOXEL_FUNCTION_COUNTER();

	// Not swap to keep order
	PointSets.RemoveAll([&](const TSharedRef<const FVoxelPointSet>& PointSet)
	{
		return PointSet->Num() == 0;
	});

	if (PointSets.Num() == 0)
	{
		return MakeVoxelShared<FVoxelPointSet>();
	}
	if (PointSets.Num() == 1)
	{
		return PointSets[0];
	}

	TVoxelSet<FName> AttributeNames;
	for (const TSharedRef<const FVoxelPointSet>& PointSet : PointSets)
	{
		for (const auto& It : PointSet->Attributes)
		{
			AttributeNames.Add(It.Key);
		}
	}

	int32 Num = 0;
	for (const TSharedRef<const FVoxelPointSet>& PointSet : PointSets)
	{
		Num += PointSet->Num();
	}

	const TSharedRef<FVoxelPointSet> Result = MakeVoxelShared<FVoxelPointSet>();
	Result->SetNum(Num);

	for (const FName AttributeName : AttributeNames)
	{
		FVoxelPinType InnerType;
		for (const TSharedRef<const FVoxelPointSet>& PointSet : PointSets)
		{
			const TSharedPtr<const FVoxelBuffer> Buffer = PointSet->Find(AttributeName);
			if (!Buffer)
			{
				continue;
			}
			if (!InnerType.IsValid())
			{
				InnerType = Buffer->GetInnerType();
				continue;
			}

			if (InnerType != Buffer->GetInnerType())
			{
				VOXEL_MESSAGE(Error, "Incompatible point attribute type when merging for {0}: {1} vs {2}",
					AttributeName,
					InnerType.ToString(),
					Buffer->GetInnerType().ToString());
			}
		}

		FVoxelBufferBuilder BufferBuilder(InnerType);
		for (const TSharedRef<const FVoxelPointSet>& PointSet : PointSets)
		{
			const TSharedPtr<const FVoxelBuffer> Buffer = PointSet->Find(AttributeName);
			if (!Buffer ||
				Buffer->GetInnerType() != InnerType)
			{
				FVoxelPointAttributes::AddDefaulted(BufferBuilder, AttributeName, PointSet->Num());
				continue;
			}

			BufferBuilder.Append(*Buffer, PointSet->Num());
		}
		Result->Add(AttributeName, BufferBuilder.MakeBuffer());
	}

	return Result;
}