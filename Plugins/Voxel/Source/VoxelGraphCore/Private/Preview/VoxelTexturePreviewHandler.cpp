// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Preview/VoxelTexturePreviewHandler.h"
#include "VoxelGraphExecutor.h"
#include "VoxelPositionQueryParameter.h"
#include "Engine/Texture2D.h"
#include "Slate/DeferredCleanupSlateBrush.h"

const FSlateBrush* FVoxelTexturePreviewHandler::GetBrush_Texture() const
{
	return Brush ? Brush->GetSlateBrush() : nullptr;
}

void FVoxelTexturePreviewHandler::Create(const FVoxelPinType& Type)
{
	if (!ensure(SupportsType(Type)))
	{
		return;
	}

	const TSharedRef<const FVoxelComputeValue> Compute = GVoxelGraphExecutorManager->MakeCompute_GameThread(Type, PinRef);

	DynamicValue = FVoxelDynamicValueFactory(
		MakeVoxelShared<FVoxelComputeValue>([Compute, PreviewSize = PreviewSize, FinalType = GetFinalValueType(Type), WeakThis = MakeWeakPtr(this)](const FVoxelQuery& Query)
		{
			VOXEL_SCOPE_COUNTER("Write preview positions");

			const FMatrix44f LocalToWorld = FMatrix44f(Query.GetQueryToWorld().Get(Query));

			FVoxelFloatBufferStorage PositionsX;
			FVoxelFloatBufferStorage PositionsY;
			FVoxelFloatBufferStorage PositionsZ;
			PositionsX.Allocate(PreviewSize * PreviewSize);
			PositionsY.Allocate(PreviewSize * PreviewSize);
			PositionsZ.Allocate(PreviewSize * PreviewSize);

			ParallelFor(PreviewSize, [&](const int32 Y)
			{
				for (int32 X = 0; X < PreviewSize; X++)
				{
					const int32 Index = FVoxelUtilities::Get2DIndex<int32>(PreviewSize, X, Y);
					const FVector3f Position = LocalToWorld.TransformPosition(FVector3f(X, Y, 0));

					PositionsX[Index] = Position.X;
					PositionsY[Index] = Position.Y;
					PositionsZ[Index] = Position.Z;
				}
			});

			const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();

			Parameters->Add<FVoxelPositionQueryParameter>().Initialize(
				FVoxelVectorBuffer::Make(PositionsX, PositionsY, PositionsZ));

			Parameters->Add<FVoxelGradientStepQueryParameter>().Step = LocalToWorld.GetScaleVector().GetAbsMax();

			const FVoxelQuery NewQuery = Query.MakeNewQuery(Parameters);
			const FVoxelFutureValue Value = (*Compute)(NewQuery);
			return
				MakeVoxelTask()
				.Dependency(Value)
				.Execute(FinalType, [=]() -> FVoxelFutureValue
				{
					const TSharedPtr<FVoxelTexturePreviewHandler> This = WeakThis.Pin();
					if (!This)
					{
						return {};
					}
					return This->GetFinalValue(NewQuery, Value.GetValue_CheckCompleted());
				});
		}),
		GetFinalValueType(Type),
		GetStruct()->GetFName())
		.Compute(QueryContext.ToSharedRef());

	DynamicValue.OnChanged(MakeWeakPtrLambda(this, [this](const FVoxelRuntimePinValue& NewValue)
	{
		{
			VOXEL_SCOPE_LOCK(CriticalSection);
			LastValue_RequiresLock = NewValue;
		}

		TVoxelArray<FLinearColor> Colors;
		FVoxelUtilities::SetNum(Colors, PreviewSize * PreviewSize);
		GenerateColors(NewValue, Colors);

		const TSharedRef<TVoxelArray<uint8>> Bytes = MakeVoxelShared<TVoxelArray<uint8>>();
		FVoxelUtilities::SetNumFast(*Bytes, PreviewSize * PreviewSize * sizeof(FColor));
		{
			VOXEL_SCOPE_COUNTER("Write colors");

			const TVoxelArrayView<FColor> FinalColors = ReinterpretCastVoxelArrayView<FColor>(*Bytes);

			ParallelFor(PreviewSize, [&](const int32 Y)
			{
				const int32 ReadIndex = FVoxelUtilities::Get2DIndex<int32>(PreviewSize, 0, Y);
				const int32 WriteIndex = FVoxelUtilities::Get2DIndex<int32>(PreviewSize, 0, PreviewSize - 1 - Y);
				for (int32 X = 0; X < PreviewSize; X++)
				{
					checkVoxelSlow(ReadIndex + X == FVoxelUtilities::Get2DIndex<int32>(PreviewSize, X, Y));
					checkVoxelSlow(WriteIndex + X == FVoxelUtilities::Get2DIndex<int32>(PreviewSize, X, PreviewSize - 1 - Y));

					FLinearColor Color = Colors[ReadIndex + X];
					Color.A = 1.f;
					FinalColors[WriteIndex + X] = Color.ToFColor(false);
				}
			});
		}

		FVoxelUtilities::RunOnGameThread(MakeWeakPtrLambda(this, [=]
		{
			if (!Texture)
			{
				Texture = FVoxelTextureUtilities::CreateTexture2D(
					"Preview",
					PreviewSize,
					PreviewSize,
					false,
					TF_Bilinear,
					PF_B8G8R8A8);
			}

			if (!Brush)
			{
				Brush = FDeferredCleanupSlateBrush::CreateBrush(
					Texture,
					FVector2D(PreviewSize, PreviewSize));
			}

			FVoxelRenderUtilities::AsyncCopyTexture(Texture, Bytes, {});

			if (QueuedMousePosition.IsSet())
			{
				UpdateStats(QueuedMousePosition.GetValue());
				QueuedMousePosition.Reset();
			}
		}));
	}));
}

void FVoxelTexturePreviewHandler::BuildStats(const FAddStat& AddStat)
{
	Super::BuildStats(AddStat);

	AddStat(
		"Value",
		"The value at the position being previewed",
		MakeWeakPtrLambda(this, [this]
		{
			return CurrentValue;
		}));

	AddStat(
		"Min Value",
		"The min value in the area being previewed",
		MakeWeakPtrLambda(this, [this]
		{
			return GetMinValue();
		}));

	AddStat(
		"Max Value",
		"The max value in the area being previewed",
		MakeWeakPtrLambda(this, [this]
		{
			return GetMaxValue();
		}));
}

void FVoxelTexturePreviewHandler::UpdateStats(const FVector2D& MousePosition)
{
	Super::UpdateStats(MousePosition);

	FVoxelRuntimePinValue LastValue;
	{
		VOXEL_SCOPE_LOCK(CriticalSection);
		LastValue = LastValue_RequiresLock;
	}
	if (!LastValue.IsValid())
	{
		QueuedMousePosition = MousePosition;
		return;
	}

	const FIntPoint Position = FVoxelUtilities::FloorToInt(MousePosition);
	const int32 Index = FVoxelUtilities::Get2DIndex<int32>(PreviewSize, FVoxelUtilities::Clamp(Position, 0, PreviewSize - 1));

	CurrentValue = GetValueAt(LastValue, Index);
}