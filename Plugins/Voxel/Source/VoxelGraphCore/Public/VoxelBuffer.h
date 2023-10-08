// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBufferStorage.h"
#include "VoxelComplexBufferStorage.h"
#include "VoxelBuffer.generated.h"

struct FVoxelTerminalBuffer;
struct FVoxelSimpleTerminalBuffer;
struct FVoxelComplexTerminalBuffer;

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelBuffer : public FVoxelBufferInterface
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

#if INTELLISENSE_PARSER
	FVoxelBuffer() = default;
	UE_NONCOPYABLE(FVoxelBuffer);
#endif

public:
	static TSharedRef<FVoxelBuffer> Make(const FVoxelPinType& InnerType);
	static FVoxelPinType FindInnerType_NotComplex(UScriptStruct* Struct);

	FORCEINLINE FVoxelPinType GetBufferType() const
	{
		return GetInnerType().GetBufferType();
	}
	FORCEINLINE const FVoxelPinType& GetInnerType() const
	{
		checkVoxelSlow(PrivateInnerType.IsValid());
		return PrivateInnerType;
	}
	FORCEINLINE int32 Num() const
	{
		if (PrivateNum == -1)
		{
			ConstCast(this)->UpdateNum();
		}
		checkVoxelSlow(PrivateNum == Num_Slow());
		return PrivateNum;
	}
	FORCEINLINE bool IsConstant() const
	{
		return Num() == 1;
	}
	FORCEINLINE bool IsValidIndex(const int32 Index) const
	{
		return 0 <= Index && (IsConstant() || Index < Num());
	}
	FORCEINLINE void CheckSlow() const
	{
		if (GVoxelCheckNaNs)
		{
			CheckSlowImpl();
		}
	}

public:
	virtual void SetAsEmpty();
	virtual void Shrink();
	virtual void CheckSlowImpl() const;
	virtual int64 GetAllocatedSize() const;
	virtual int32 Num_Slow() const override;
	virtual bool IsValid_Slow() const final override;

	FVoxelRuntimePinValue GetGenericConstant() const;

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) VOXEL_PURE_VIRTUAL();
	virtual FVoxelRuntimePinValue GetGeneric(int32 Index) const VOXEL_PURE_VIRTUAL({});

public:
	FORCEINLINE int32 NumTerminalBuffers() const
	{
		return PrivateBufferOffsets.Num();
	}
	FORCEINLINE FVoxelTerminalBuffer& GetTerminalBuffer(const int32 Index)
	{
		return *reinterpret_cast<FVoxelTerminalBuffer*>(reinterpret_cast<uint8*>(this) + PrivateBufferOffsets[Index]);
	}
	FORCEINLINE const FVoxelTerminalBuffer& GetTerminalBuffer(const int32 Index) const
	{
		return ConstCast(this)->GetTerminalBuffer(Index);
	}

	template<typename Type>
	struct TVoxelTerminalBufferIterator
	{
		FVoxelBuffer* Buffer = nullptr;

		struct FIterator
		{
			FVoxelBuffer* Buffer = nullptr;
			const int64* OffsetPtr = nullptr;

			FORCEINLINE Type& operator*() const
			{
				FVoxelBuffer* TerminalBuffer = reinterpret_cast<FVoxelBuffer*>(reinterpret_cast<uint8*>(Buffer) + *OffsetPtr);
				return CastChecked<Type>(*TerminalBuffer);
			}
			FORCEINLINE void operator++()
			{
				++OffsetPtr;
			}
			FORCEINLINE bool operator!=(const FIterator& Other) const
			{
				return OffsetPtr != Other.OffsetPtr;
			}
		};
		FIterator begin() const
		{
			return { Buffer, Buffer->PrivateBufferOffsets.GetData() };
		}
		FIterator end() const
		{
			return { Buffer, Buffer->PrivateBufferOffsets.GetData() + Buffer->PrivateBufferOffsets.Num() };
		}
	};
	FORCEINLINE TVoxelTerminalBufferIterator<FVoxelTerminalBuffer> GetTerminalBuffers()
	{
		return { this };
	}
	FORCEINLINE TVoxelTerminalBufferIterator<const FVoxelTerminalBuffer> GetTerminalBuffers() const
	{
		return { ConstCast(this) };
	}

public:
	bool Identical(const FVoxelBuffer* Other, uint32 PortFlags) const
	{
		if (Num() == 0 &&
			Other->Num() == 0)
		{
			return true;
		}

		ensure(false);
		return false;
	}

protected:
	int32 PrivateNum = -1;

	template<typename T>
	FORCEINLINE void Initialize()
	{
		struct FStatics
		{
			FVoxelPinType Type = FVoxelPinType::Make<T>();
			TArray<int64> BufferOffsets;

			explicit FStatics(const FVoxelBuffer* Template)
			{
				ComputeBuffers(Template, BufferOffsets);
			}
		};

		static const FStatics* Statics = nullptr;
		if (!Statics)
		{
			Statics = new FStatics(this);
		}

		PrivateInnerType = Statics->Type;
		PrivateBufferOffsets = Statics->BufferOffsets;
	}

private:
	FVoxelPinType PrivateInnerType;
	TConstVoxelArrayView<int64> PrivateBufferOffsets;

	void UpdateNum();

	static void ComputeBuffers(
		const FVoxelBuffer* Template,
		TArray<int64>& BufferOffsets);

	friend FVoxelTerminalBuffer;
	friend FVoxelSimpleTerminalBuffer;
	friend FVoxelComplexTerminalBuffer;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTerminalBuffer : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	FORCEINLINE int32 Num() const
	{
		checkVoxelSlow(PrivateNum == Num_Slow());
		return PrivateNum;
	}
	FORCEINLINE bool IsConstant() const
	{
		return PrivateNum == 1;
	}
	FORCEINLINE bool IsValidIndex(const int32 Index) const
	{
		return 0 <= Index && (IsConstant() || Index < Num());
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelSimpleTerminalBuffer : public FVoxelTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	FORCEINLINE int32 GetTypeSize() const
	{
		return PrivateStorage->GetTypeSize();
	}
	FORCEINLINE void CheckSlow() const
	{
		if (GVoxelCheckNaNs)
		{
			CheckSlowImpl();
		}
	}

	FORCEINLINE const TSharedRef<const FVoxelBufferStorage>& GetSharedStorage() const
	{
		return PrivateStorage;
	}

	FORCEINLINE const FVoxelBufferStorage& GetStorage() const
	{
		return *PrivateStorage;
	}
	template<typename Type>
	FORCEINLINE const TVoxelBufferStorage<Type>& GetStorage() const
	{
		return GetStorage().As<Type>();
	}

	FORCEINLINE FVoxelBufferStorage& GetMutableStorage() const
	{
		checkVoxelSlow(PrivateStorage.GetSharedReferenceCount() == 1);
		return ConstCast(*PrivateStorage);
	}
	template<typename Type>
	FORCEINLINE TVoxelBufferStorage<Type>& GetMutableStorage() const
	{
		return GetMutableStorage().As<Type>();
	}

	FORCEINLINE bool IsDefault() const
	{
		return PrivateStorage.GetSharedReferenceCount() == 0;
	}

public:
	virtual void SetAsEmpty() final override;
	virtual void Shrink() final override;
	virtual void CheckSlowImpl() const final override;
	virtual int64 GetAllocatedSize() const final override;
	virtual int32 Num_Slow() const override;

	TSharedRef<FVoxelBufferStorage> MakeNewStorage() const;
	void SetStorage(const TSharedRef<const FVoxelBufferStorage>& Storage);

protected:
	template<typename T>
	void SetConstant(const T Constant)
	{
		checkVoxelSlow(GetInnerType().Is<T>());
		checkVoxelSlow(IsDefault());

		const TSharedRef<TVoxelBufferStorage<T>> Storage = MakeVoxelShared<TVoxelBufferStorage<T>>();
		Storage->SetConstant(Constant);
		this->SetStorage(Storage);
	}

protected:
	template<typename T>
	FORCEINLINE void Initialize()
	{
		struct FStatics
		{
			FVoxelPinType Type = FVoxelPinType::Make<T>();
			TSharedRef<TVoxelBufferStorage<T>> Storage = MakeVoxelShared<TVoxelBufferStorage<T>>();
			TArray<int64> BufferOffsets = { 0 };

			FStatics()
			{
				Storage->SetConstant(FVoxelUtilities::MakeSafe<T>());

				// Clear the reference count to avoid a lot of contention on a few atomics
				ClearSharedRefReferencer(Storage);
			}
		};

		static const FStatics* Statics = nullptr;
		if (!Statics)
		{
			VOXEL_ALLOW_LEAK_SCOPE();
			Statics = new FStatics();
		}

		PrivateStorage = Statics->Storage;
		PrivateNum = PrivateStorage->Num();
		PrivateInnerType = Statics->Type;
		PrivateInnerType = Statics->Type;
		PrivateBufferOffsets = Statics->BufferOffsets;

		checkVoxelSlow(IsDefault());
	}

private:
	TSharedRef<const FVoxelBufferStorage> PrivateStorage = MakeNullSharedRef<const FVoxelBufferStorage>();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelComplexTerminalBuffer final : public FVoxelTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
#if INTELLISENSE_PARSER
	FVoxelComplexTerminalBuffer(FVoxelComplexTerminalBuffer&&);
	FVoxelComplexTerminalBuffer(const FVoxelComplexTerminalBuffer&);
	FVoxelComplexTerminalBuffer& operator=(const FVoxelComplexTerminalBuffer&);
	FVoxelComplexTerminalBuffer& operator=(FVoxelComplexTerminalBuffer&&);
#endif

	FVoxelComplexTerminalBuffer();

	FORCEINLINE UScriptStruct* GetInnerStruct() const
	{
		return PrivateStorage->GetInnerStruct();
	}

	FORCEINLINE const TSharedRef<const FVoxelComplexBufferStorage>& GetSharedStorage() const
	{
		return PrivateStorage;
	}

	FORCEINLINE const FVoxelComplexBufferStorage& GetStorage() const
	{
		return *PrivateStorage;
	}
	FORCEINLINE FVoxelComplexBufferStorage& GetMutableStorage() const
	{
		checkVoxelSlow(PrivateStorage.GetSharedReferenceCount() == 1);
		return ConstCast(*PrivateStorage);
	}

	FORCEINLINE bool IsDefault() const
	{
		return PrivateStorage.GetSharedReferenceCount() == 0;
	}

public:
	template<typename T, typename LambdaType>
	void Foreach(LambdaType&& Lambda) const
	{
		if (!ensureVoxelSlow(GetInnerStruct() == StaticStructFast<T>()))
		{
			return;
		}

		for (int32 Index = 0; Index < GetStorage().Num(); Index++)
		{
			Lambda(GetStorage()[Index].Get<T>());
		}
	}

public:
	virtual void SetAsEmpty() override;
	virtual void Shrink() override;
	virtual void CheckSlowImpl() const override;
	virtual int64 GetAllocatedSize() const override;
	virtual int32 Num_Slow() const override;
	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
	virtual FVoxelRuntimePinValue GetGeneric(int32 Index) const override;

	TSharedRef<FVoxelComplexBufferStorage> MakeNewStorage() const;
	void Initialize(const FVoxelPinType& InnerType);
	void SetStorage(const TSharedRef<const FVoxelComplexBufferStorage>& Storage);

private:
	TSharedRef<const FVoxelComplexBufferStorage> PrivateStorage = MakeNullSharedRef<FVoxelComplexBufferStorage>();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct VOXELGRAPHCORE_API FVoxelBufferAccessor
{
	FVoxelBufferAccessor() = default;

	template<typename... TArgs>
	explicit FVoxelBufferAccessor(const TArgs&... Args)
	{
		checkStatic(sizeof...(Args) > 1);

		TArray<int32, TVoxelInlineAllocator<16>> Nums;
		VOXEL_FOLD_EXPRESSION(Nums.Add(Args.Num()));

		PrivateNum = 1;
		for (const int32 Num : Nums)
		{
			if (PrivateNum == 1)
			{
				PrivateNum = Num;
			}
			else if (PrivateNum != Num && Num != 1)
			{
				PrivateNum = -1;
			}
		}
	}

	template<typename T>
	FORCEINLINE static bool MergeNum(int32& Num, const T& Buffer)
	{
		const int32 BufferNum = Buffer.Num();

		if (Num == 1)
		{
			Num = BufferNum;
			return true;
		}

		if (BufferNum == Num ||
			BufferNum == 1)
		{
			return true;
		}

		return false;
	}

	FORCEINLINE bool IsValid() const
	{
		return PrivateNum != -1;
	}
	FORCEINLINE int32 Num() const
	{
		return PrivateNum;
	}

private:
	int32 PrivateNum = -1;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelSwitchTerminalTypeSize
{
	const int32 TypeSize;

	template<typename T>
	FORCENOINLINE auto operator+(T&& Lambda) -> decltype(auto)
	{
		switch (TypeSize)
		{
		default: VOXEL_ASSUME(false);
		case 1: return Lambda(uint8());
		case 2: return Lambda(uint16());
		case 4: return Lambda(uint32());
		case 8: return Lambda(uint64());
		}
	}
};

#if INTELLISENSE_PARSER
#define VOXEL_SWITCH_TERMINAL_TYPE_SIZE(TypeSize) \
	FVoxelSwitchTerminalTypeSize{ TypeSize } + \
	[&](const uint64 TypeInstance)
#else
#define VOXEL_SWITCH_TERMINAL_TYPE_SIZE(TypeSize) \
	FVoxelSwitchTerminalTypeSize{ TypeSize } + \
	[&](const auto TypeInstance)
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define DECLARE_VOXEL_BUFFER(InBufferType, InUniformType) \
	checkStatic(TIsTriviallyDestructible<InUniformType>::Value); \
	struct InBufferType; \
	\
	template<> \
	struct TVoxelBufferType<InUniformType> \
	{ \
		using Type = InBufferType; \
	}; \
	template <> \
	struct TVoxelBufferInnerType<InBufferType> \
	{ \
		using Type = InUniformType; \
	}; \
	template<> \
	struct TIsVoxelBuffer<InBufferType> \
	{ \
		static constexpr bool Value = true; \
	}; \
	template<> \
	struct TIsPODType<InBufferType> \
	{ \
		static constexpr bool Value = false; \
	}; \
	template<> \
	struct TStructOpsTypeTraits<InBufferType> : public TStructOpsTypeTraitsBase2<InBufferType> \
	{ \
		enum \
		{ \
			WithIdentical = true, \
		}; \
	};

#define DECLARE_VOXEL_TERMINAL_BUFFER(InBufferType, InUniformType) \
	DECLARE_VOXEL_BUFFER(InBufferType, InUniformType) \
	using InBufferType ## Storage = TVoxelBufferStorage<InUniformType>;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if INTELLISENSE_PARSER
#define GENERATED_VOXEL_BUFFER_BODY_INTELLISENSE(InThisType) \
	InThisType(InThisType&&); \
	InThisType(const InThisType&); \
	InThisType& operator=(const InThisType&); \
	InThisType& operator=(InThisType&&);
#else
#define GENERATED_VOXEL_BUFFER_BODY_INTELLISENSE(InBufferType)
#endif

#define GENERATED_VOXEL_BUFFER_BODY(InThisType, InType) \
	GENERATED_VIRTUAL_STRUCT_BODY() \
	GENERATED_VOXEL_BUFFER_BODY_INTELLISENSE(InThisType); \
	using UniformType = InType; \
	\
	FORCEINLINE auto GetConstant() const -> decltype(auto) \
	{ \
		checkVoxelSlow(IsConstant()); \
		return operator[](0); \
	} \
	\
	virtual FVoxelRuntimePinValue GetGeneric(const int32 Index) const override \
	{ \
		return FVoxelRuntimePinValue::Make(UniformType(operator[](Index))); \
	} \
	\
	FORCEINLINE InThisType() \
	{ \
		checkStatic(std::is_final_v<VOXEL_THIS_TYPE>); \
		checkStatic(std::is_same_v<Super, FVoxelBuffer>); \
		Initialize<UniformType>(); \
	} \
	FORCEINLINE InThisType(const InType Constant) \
		: InThisType() \
	{ \
		InitializeFromConstant(FVoxelRuntimePinValue::Make(Constant)); \
	} \
	static InThisType MakeEmpty() \
	{ \
		InThisType Result; \
		Result.SetAsEmpty(); \
		return Result; \
	} \

#define GENERATED_VOXEL_TERMINAL_BUFFER_BODY(InThisType, InType) \
	GENERATED_VIRTUAL_STRUCT_BODY() \
	GENERATED_VOXEL_BUFFER_BODY_INTELLISENSE(InThisType); \
	using UniformType = InType; \
	using BufferType = TVoxelBufferType<InType>::Type; \
	\
	static InThisType MakeEmpty() \
	{ \
		BufferType Result; \
		Result.SetStorage(MakeVoxelShared<InThisType ## Storage>()); \
		return Result; \
	} \
	static InThisType Make(InThisType ## Storage& Storage) \
	{ \
		BufferType Result; \
		Result.SetStorage(Storage); \
		return Result; \
	} \
	static InThisType Make(const TSharedRef<const InThisType ## Storage>& Storage) \
	{ \
		BufferType Result; \
		Result.SetStorage(Storage); \
		return Result; \
	} \
	static InThisType Make(InType Constant) \
	{ \
		BufferType Result; \
		Result.SetConstant(Constant); \
		return Result; \
	} \
	static InThisType MakeSafe() \
	{ \
		return Make(FVoxelUtilities::MakeSafe<InType>()); \
	} \
	using FVoxelSimpleTerminalBuffer::SetStorage; \
	FORCEINLINE void SetStorage(InThisType ## Storage& Storage) \
	{ \
		SetStorage(MakeSharedCopy(MoveTemp(Storage))); \
	} \
	\
	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) final override \
	{ \
		if (ensure(Constant.CanBeCastedTo<InType>())) \
		{ \
			SetConstant(Constant.Get<InType>()); \
		} \
	} \
	virtual FVoxelRuntimePinValue GetGeneric(const int32 Index) const override \
	{ \
		return FVoxelRuntimePinValue::Make(operator[](Index)); \
	} \
	\
	FORCEINLINE const InThisType ## Storage& GetStorage() const \
	{ \
		return Super::GetStorage<InType>(); \
	} \
	FORCEINLINE InThisType ## Storage& GetMutableStorage() const \
	{ \
		return Super::GetMutableStorage<InType>(); \
	} \
	FORCEINLINE const InType& GetConstant() const \
	{ \
		return GetStorage().GetConstant(); \
	} \
	\
	FORCEINLINE const InType& operator[](const int32 Index) const \
	{ \
		return GetStorage()[Index]; \
	} \
	FORCEINLINE const InType* GetData(const FVoxelBufferIterator& Iterator) const \
	{ \
		return GetStorage().GetData(Iterator); \
	} \
	FORCEINLINE TConstVoxelArrayView<InType> GetRawView_NotConstant(const FVoxelBufferIterator& Iterator) const \
	{ \
		return GetStorage().GetRawView_NotConstant(Iterator); \
	} \
	FORCEINLINE auto begin() const \
	{ \
		return GetStorage().begin(); \
	} \
	FORCEINLINE auto end() const \
	{ \
		return GetStorage().end(); \
	} \
	\
	FORCEINLINE InThisType() \
	{ \
		checkStatic(std::is_final_v<VOXEL_THIS_TYPE>); \
		checkStatic(std::is_same_v<Super, FVoxelSimpleTerminalBuffer>); \
		Initialize<InType>(); \
	} \
	FORCEINLINE InThisType(const InType Constant) \
		: InThisType() \
	{ \
		SetConstant(Constant); \
	}