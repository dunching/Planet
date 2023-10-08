// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelArray.h"
#include "VoxelMinimal/Containers/VoxelChunkedArray.h"
#include "VoxelMinimal/Utilities/VoxelIntVectorUtilities.h"

template<typename Allocator = FVoxelAllocator>
struct TVoxelAddOnlyMapArrayType
{
	template<typename Type>
	using TArray = TVoxelArray<Type, Allocator>;

	template<typename Type>
	static void SetNumFast(TArray<Type>& Array, const int32 Num)
	{
		FVoxelUtilities::SetNumFast(Array, Num);
	}
	template<typename Type>
	static void Memset(TArray<Type>& Array, const uint8 Value)
	{
		FVoxelUtilities::Memset(Array, Value);
	}
};

// Smaller footprint than TMap
// Much faster to Reserve as no sparse array/free list
template<typename KeyType, typename ValueType, typename ArrayType = TVoxelAddOnlyMapArrayType<>>
class TVoxelAddOnlyMap
{
public:
	template<typename Type>
	using TArray = typename ArrayType::template TArray<Type>;

	struct FElement
	{
		const KeyType Key;
		ValueType Value;

		FORCEINLINE explicit FElement(const KeyType& Key)
			: Key(Key)
		{
		}

	private:
		int32 NextElementIndex;

		friend class TVoxelAddOnlyMap;
	};

	TVoxelAddOnlyMap() = default;

	FORCEINLINE int32 Num() const
	{
		return Elements.Num();
	}
	FORCEINLINE int64 GetAllocatedSize() const
	{
		return HashTable.GetAllocatedSize() + Elements.GetAllocatedSize();
	}
	FORCEINLINE void Reset()
	{
		HashSize = 0;
		Elements.Reset();
		HashTable.Reset();
	}
	FORCEINLINE void Empty()
	{
		HashSize = 0;
		Elements.Empty();
		HashTable.Empty();
	}
	FORCEINLINE void Reserve(const int32 Number)
	{
		if (Number <= Elements.Num())
		{
			return;
		}

		Elements.Reserve(Number);

		const int32 NewHashSize = TSetAllocator<>::GetNumberOfHashBuckets(Number);

		if (HashSize < NewHashSize)
		{
			HashSize = NewHashSize;
			Rehash();
		}
	}

public:
	FORCEINLINE ValueType* Find(const KeyType& Key)
	{
		return this->FindHashed(HashValue(Key), Key);
	}
	FORCEINLINE ValueType* FindHashed(const uint32 Hash, const KeyType& Key)
	{
		checkVoxelSlow(HashValue(Key) == Hash);

		if (HashSize == 0)
		{
			return nullptr;
		}

		int32 ElementIndex = this->GetElementIndex(Hash);
		while (true)
		{
			if (ElementIndex == -1)
			{
				return nullptr;
			}

			FElement& Element = Elements[ElementIndex];
			if (Element.Key == Key)
			{
				return &Element.Value;
			}
			ElementIndex = Element.NextElementIndex;
		}
	}
	FORCEINLINE const ValueType* Find(const KeyType& Key) const
	{
		return ConstCast(this)->Find(Key);
	}

	FORCEINLINE ValueType FindRef(const KeyType& Key) const
	{
		checkStatic(TIsTriviallyDestructible<ValueType>::Value);
		if (const ValueType* Value = this->Find(Key))
		{
			return *Value;
		}
		return ValueType();
	}
	FORCEINLINE auto* FindSharedPtr(const KeyType& Key) const
	{
		if (const ValueType* Value = this->Find(Key))
		{
			return Value->Get();
		}
		return nullptr;
	}

	FORCEINLINE ValueType& FindChecked(const KeyType& Key)
	{
		int32 ElementIndex = this->GetElementIndex(HashValue(Key));
		while (true)
		{
			checkVoxelSlow(ElementIndex != -1);
			FElement& Element = Elements[ElementIndex];
			if (Element.Key == Key)
			{
				return Element.Value;
			}
			ElementIndex = Element.NextElementIndex;
		}
	}
	FORCEINLINE const ValueType& FindChecked(const KeyType& Key) const
	{
		return ConstCast(this)->FindChecked(Key);
	}

	FORCEINLINE bool Contains(const KeyType& Key) const
	{
		return this->Find(Key) != nullptr;
	}

	FORCEINLINE ValueType& operator[](const KeyType& Key)
	{
		return this->FindChecked(Key);
	}
	FORCEINLINE const ValueType& operator[](const KeyType& Key) const
	{
		return this->FindChecked(Key);
	}

public:
	FORCEINLINE ValueType& FindOrAdd(const KeyType& Key)
	{
		const uint32 Hash = HashValue(Key);

		if (HashSize > 0)
		{
			int32 ElementIndex = this->GetElementIndex(Hash);
			while (true)
			{
				if (ElementIndex == -1)
				{
					break;
				}

				FElement& Element = Elements[ElementIndex];
				if (Element.Key == Key)
				{
					return Element.Value;
				}
				ElementIndex = Element.NextElementIndex;
			}
		}

		const int32 NewElementIndex = Elements.Add(FElement(Key));
		FElement& Element = Elements[NewElementIndex];

		const int32 DesiredHashSize = TSetAllocator<>::GetNumberOfHashBuckets(Elements.Num());
		if (HashSize < DesiredHashSize)
		{
			HashSize = DesiredHashSize;
			Rehash();
		}
		else
		{
			int32& ElementIndex = GetElementIndex(Hash);
			Element.NextElementIndex = ElementIndex;
			ElementIndex = NewElementIndex;
		}

		return Element.Value;
	}

public:
	FORCEINLINE ValueType& Add_CheckNew(const KeyType& Key)
	{
		checkVoxelSlow(!this->Find(Key));

		const int32 NewElementIndex = Elements.Add(FElement(Key));
		FElement& Element = Elements[NewElementIndex];

		const int32 DesiredHashSize = TSetAllocator<>::GetNumberOfHashBuckets(Elements.Num());
		if (HashSize < DesiredHashSize)
		{
			HashSize = DesiredHashSize;
			Rehash();
		}
		else
		{
			int32& ElementIndex = this->GetElementIndex(HashValue(Key));
			Element.NextElementIndex = ElementIndex;
			ElementIndex = NewElementIndex;
		}

		return Element.Value;
	}
	FORCEINLINE ValueType& Add_CheckNew(const KeyType& Key, const ValueType& Value)
	{
		ValueType& ValueRef = this->Add_CheckNew(Key);
		ValueRef = Value;
		return ValueRef;
	}
	FORCEINLINE ValueType& Add_CheckNew(const KeyType& Key, ValueType&& Value)
	{
		ValueType& ValueRef = this->Add_CheckNew(Key);
		ValueRef = MoveTemp(Value);
		return ValueRef;
	}

public:
	FORCEINLINE ValueType& Add_CheckNew_NoRehash(const KeyType& Key)
	{
		return this->AddHashed_CheckNew_NoRehash(HashValue(Key), Key);
	}
	FORCEINLINE ValueType& Add_CheckNew_NoRehash(const KeyType& Key, const ValueType& Value)
	{
		ValueType& ValueRef = this->Add_CheckNew_NoRehash(Key);
		ValueRef = Value;
		return ValueRef;
	}
	FORCEINLINE ValueType& Add_CheckNew_NoRehash(const KeyType& Key, ValueType&& Value)
	{
		ValueType& ValueRef = this->Add_CheckNew_NoRehash(Key);
		ValueRef = MoveTemp(Value);
		return ValueRef;
	}

public:
	FORCEINLINE ValueType& AddHashed_CheckNew_NoRehash(const uint32 Hash, const KeyType& Key)
	{
		checkVoxelSlow(!this->Find(Key));
		checkVoxelSlow(HashValue(Key) == Hash);

		const int32 NewElementIndex = Elements.Add_NoGrow(FElement(Key));
		FElement& Element = Elements[NewElementIndex];

		const int32 DesiredHashSize = TSetAllocator<>::GetNumberOfHashBuckets(Elements.Num());
		checkVoxelSlow(HashSize >= DesiredHashSize);

		int32& ElementIndex = this->GetElementIndex(Hash);
		Element.NextElementIndex = ElementIndex;
		ElementIndex = NewElementIndex;

		return Element.Value;
	}

public:
	template<typename ElementType>
	struct TIterator
	{
		typename TArray<ElementType>::RangedForIteratorType It;

		FORCEINLINE explicit TIterator(const typename TArray<ElementType>::RangedForIteratorType& It)
			: It(It)
		{
		}

		FORCEINLINE TIterator& operator++()
		{
			++It;
			return *this;
		}
		FORCEINLINE explicit operator bool() const
		{
			return bool(It);
		}
		FORCEINLINE ElementType& operator*() const
		{
			return *It;
		}
		FORCEINLINE bool operator!=(const TIterator& Other) const
		{
			return It != Other.It;
		}
	};

	FORCEINLINE TIterator<FElement> begin()
	{
		return TIterator<FElement>(Elements.begin());
	}
	FORCEINLINE TIterator<FElement> end()
	{
		return TIterator<FElement>(Elements.end());
	}

	FORCEINLINE TIterator<const FElement> begin() const
	{
		return TIterator<const FElement>(Elements.begin());
	}
	FORCEINLINE TIterator<const FElement> end() const
	{
		return TIterator<const FElement>(Elements.end());
	}

public:
	FORCEINLINE static uint32 HashValue(const KeyType& Key)
	{
		if constexpr (std::is_same_v<KeyType, FIntVector>)
		{
			return FVoxelUtilities::FastIntVectorHash(Key);
		}
		if constexpr (std::is_same_v<KeyType, FIntPoint>)
		{
			return FVoxelUtilities::FastIntPointHash(Key);
		}

		return GetTypeHash(Key);
	}

private:
	int32 HashSize = 0;
	TArray<int32> HashTable;
	TArray<FElement> Elements;

	FORCEINLINE int32& GetElementIndex(const uint32 Hash)
	{
		checkVoxelSlow(HashSize != 0);
		checkVoxelSlow(FMath::IsPowerOfTwo(HashSize));
		return HashTable[Hash & (HashSize - 1)];
	}
	FORCEINLINE const int32& GetElementIndex(const uint32 Hash) const
	{
		checkVoxelSlow(HashSize != 0);
		checkVoxelSlow(FMath::IsPowerOfTwo(HashSize));
		return HashTable[Hash & (HashSize - 1)];
	}

	FORCENOINLINE void Rehash()
	{
		VOXEL_FUNCTION_COUNTER_NUM(HashSize, 1024);

		HashTable.Reset();

		if (HashSize == 0)
		{
			return;
		}

		checkVoxelSlow(FMath::IsPowerOfTwo(HashSize));
		ArrayType::SetNumFast(HashTable, HashSize);
		ArrayType::Memset(HashTable, 0xFF);

		for (int32 Index = 0; Index < Elements.Num(); Index++)
		{
			FElement& Element = Elements[Index];

			int32& ElementIndex = this->GetElementIndex(HashValue(Element.Key));
			Element.NextElementIndex = ElementIndex;
			ElementIndex = Index;
		}
	}
};

template<int32 MaxBytesPerChunk>
struct TVoxelAddOnlyMapChunkedArrayType
{
	template<typename Type>
	using TArray = TVoxelChunkedArray<Type, MaxBytesPerChunk>;

	template<typename Type>
	static void SetNumFast(TArray<Type>& Array, const int32 Num)
	{
		Array.SetNumUninitialized(Num);
	}
	template<typename Type>
	static void Memset(TArray<Type>& Array, const uint8 Value)
	{
		Array.Memset(Value);
	}
};

template<typename KeyType, typename ValueType, int32 MaxBytesPerChunk = GVoxelDefaultAllocationSize>
using TVoxelAddOnlyChunkedMap = TVoxelAddOnlyMap<KeyType, ValueType, TVoxelAddOnlyMapChunkedArrayType<MaxBytesPerChunk>>;