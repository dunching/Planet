// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPointAttributeNodes.h"
#include "VoxelBufferUtilities.h"
#include "VoxelCompiledGraph.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_SetPointAttribute, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);

	return VOXEL_ON_COMPLETE(Points)
	{
		if (Points->Num() == 0)
		{
			return {};
		}

		const TValue<FName> Name = Get(NamePin, Query);
		const TValue<FVoxelBuffer> Value = Get<FVoxelBuffer>(ValuePin, Points->MakeQuery(Query));

		return VOXEL_ON_COMPLETE(Points, Name, Value)
		{
			const FName FinalName = GetAttributeName(Name);

			if (!Points->CheckNum(this, Value->Num()))
			{
				return Points;
			}

			const TSharedRef<FVoxelPointSet> NewPoints = Points->MakeSharedCopy();
			NewPoints->Add(FinalName, Value);
			return NewPoints;
		};
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_SetPointAttribute::GetPromotionTypes(const FVoxelPin& Pin) const
{
	return FVoxelPinTypeSet::AllBuffers();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetPointAttribute, Value)
{
	const TValue<FName> Name = Get(NamePin, Query);

	return VOXEL_ON_COMPLETE(Name)
	{
		FindVoxelQueryParameter(FVoxelPointSetQueryParameter, PointSetQueryParameter);

		if (!ensure(PointSetQueryParameter->PointSet))
		{
			return {};
		}

		const FName FinalName = GetAttributeName(Name);

		const TSharedPtr<const FVoxelBuffer> Attribute = PointSetQueryParameter->PointSet->Find(FinalName);
		if (!Attribute)
		{
			VOXEL_MESSAGE(Error, "{0}: No attribute named {1} found", this, FinalName);
			return {};
		}

		if (!Attribute->GetBufferType().CanBeCastedTo(ReturnPinType))
		{
			VOXEL_MESSAGE(Error, "{0}: Found attribute named {1} of type {2}, cannot cast to {3}",
				this,
				FinalName,
				Attribute->GetBufferType().ToString(),
				ReturnPinType.ToString());
			return {};
		}

		return FVoxelRuntimePinValue::Make(Attribute.ToSharedRef(), ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_GetPointAttribute::GetPromotionTypes(const FVoxelPin& Pin) const
{
	return FVoxelPinTypeSet::AllBuffers();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNode_SetPointAttributeBase::ExpandPins(
	FNode& Node,
	TArray<FPin*> Pins,
	const TArray<FPin*>& AllPins) const
{
	check(Pins.Num() == 2);
	check(AllPins.Num() == 3);

	Pins.Insert(MakeConstant(Node, FVoxelPinValue::Make(GetAttributeName())), 1);

	return Call_Single<FVoxelNode_SetPointAttribute>(Pins);
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNode_GetPointAttributeBase::ExpandPins(
	FNode& Node,
	TArray<FPin*> Pins,
	const TArray<FPin*>& AllPins) const
{
	check(Pins.Num() == 0);
	check(AllPins.Num() == 1);

	Pins.Add(MakeConstant(Node, FVoxelPinValue::Make(GetAttributeName())));

	return Call_Single<FVoxelNode_GetPointAttribute>(Pins, AllPins[0]->Type);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_SetPointCustomData, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);

	return VOXEL_ON_COMPLETE(Points)
	{
		if (Points->Num() == 0)
		{
			return {};
		}

		const TValue<int32> Index = Get(IndexPin, Query);
		const TValue<FVoxelFloatBuffer> Value = Get(ValuePin, Points->MakeQuery(Query));

		return VOXEL_ON_COMPLETE(Points, Index, Value)
		{
			if (!(0 <= Index && Index < 16))
			{
				VOXEL_MESSAGE(Error, "{0}: Index should be between 0 and 15, is {1}", this, Index);
				return Points;
			}

			if (!Points->CheckNum(this, Value.Num()))
			{
				return Points;
			}

			const FName Name(FVoxelPointAttributes::CustomData, Index);

			const TSharedRef<FVoxelPointSet> NewPoints = Points->MakeSharedCopy();
			NewPoints->Add(Name, Value);
			return NewPoints;
		};
	};
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetPointCustomData, Value)
{
	const TValue<int32> Index = Get(IndexPin, Query);

	return VOXEL_ON_COMPLETE(Index)
	{
		FindVoxelQueryParameter(FVoxelPointSetQueryParameter, PointSetQueryParameter);

		if (!ensure(PointSetQueryParameter->PointSet))
		{
			return {};
		}

		if (!(0 <= Index && Index < 16))
		{
			VOXEL_MESSAGE(Error, "{0}: Index should be between 0 and 15, is {1}", this, Index);
			return {};
		}

		const FName Name(FVoxelPointAttributes::CustomData, Index);

		FindVoxelPointSetAttribute(*PointSetQueryParameter->PointSet, Name, FVoxelFloatBuffer, Buffer);
		return Buffer;
	};
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetParentPointCustomData, Value)
{
	const TValue<int32> Index = Get(IndexPin, Query);

	return VOXEL_ON_COMPLETE(Index)
	{
		FindVoxelQueryParameter(FVoxelPointSetQueryParameter, PointSetQueryParameter);

		if (!ensure(PointSetQueryParameter->PointSet))
		{
			return {};
		}

		if (!(0 <= Index && Index < 16))
		{
			VOXEL_MESSAGE(Error, "{0}: Index should be between 0 and 15, is {1}", this, Index);
			return {};
		}

		const FName Name(FVoxelPointAttributes::ParentCustomData, Index);

		FindVoxelPointSetAttribute(*PointSetQueryParameter->PointSet, Name, FVoxelFloatBuffer, Buffer);
		return Buffer;
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetPointSeed, Value)
{
	FindVoxelQueryParameter(FVoxelPointSetQueryParameter, PointSetQueryParameter);

	if (!ensure(PointSetQueryParameter->PointSet))
	{
		return {};
	}

	FindVoxelPointSetAttribute(*PointSetQueryParameter->PointSet, FVoxelPointAttributes::Id, FVoxelPointIdBuffer, Buffer);

	return FVoxelBufferUtilities::PointIdToSeed(Buffer);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ApplyTranslation, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);

	return VOXEL_ON_COMPLETE(Points)
	{
		if (Points->Num() == 0)
		{
			return {};
		}

		const TValue<FVoxelVectorBuffer> Translation = Get(TranslationPin, Points->MakeQuery(Query));

		return VOXEL_ON_COMPLETE(Points, Translation)
		{
			CheckVoxelBuffersNum(*Points, Translation);
			FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Position, FVoxelVectorBuffer, PositionBuffer);

			const TSharedRef<FVoxelPointSet> NewPoints = Points->MakeSharedCopy();
			NewPoints->Add(FVoxelPointAttributes::Position, FVoxelBufferUtilities::Add(PositionBuffer, Translation));
			return NewPoints;
		};
	};
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ApplyRotation, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);

	return VOXEL_ON_COMPLETE(Points)
	{
		if (Points->Num() == 0)
		{
			return {};
		}

		const TValue<FVoxelQuaternionBuffer> Rotation = Get(RotationPin, Points->MakeQuery(Query));

		return VOXEL_ON_COMPLETE(Points, Rotation)
		{
			CheckVoxelBuffersNum(*Points, Rotation);
			FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Rotation, FVoxelQuaternionBuffer, RotationBuffer);

			const TSharedRef<FVoxelPointSet> NewPoints = Points->MakeSharedCopy();
			NewPoints->Add(FVoxelPointAttributes::Rotation, FVoxelBufferUtilities::Combine(RotationBuffer, Rotation));
			return NewPoints;
		};
	};
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ApplyScale, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);

	return VOXEL_ON_COMPLETE(Points)
	{
		if (Points->Num() == 0)
		{
			return {};
		}

		const TValue<FVoxelVectorBuffer> Scale = Get(ScalePin, Points->MakeQuery(Query));

		return VOXEL_ON_COMPLETE(Points, Scale)
		{
			CheckVoxelBuffersNum(*Points, Scale);
			FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Position, FVoxelVectorBuffer, PositionBuffer);

			const TSharedRef<FVoxelPointSet> NewPoints = Points->MakeSharedCopy();
			NewPoints->Add(FVoxelPointAttributes::Position, FVoxelBufferUtilities::Multiply(PositionBuffer, Scale));
			return NewPoints;
		};
	};
}