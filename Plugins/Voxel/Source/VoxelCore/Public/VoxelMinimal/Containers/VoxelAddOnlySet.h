// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelArray.h"
#include "VoxelMinimal/Utilities/VoxelIntVectorUtilities.h"

// Smaller footprint than TSet
// Much faster to Reserve as no sparse array/free list
template<typename Type>
class TVoxelAddOnlySet
{
public:
	struct FElement
	{
		Type Value;
		int32 NextElementIndex;
	};

	TVoxelAddOnlySet() = default;

	FORCEINLINE int32 Num() const
	{
		return Elements.Num();
	}
	FORCEINLINE int64 GetAllocatedSize() const
	{
		return HashTable.GetAllocatedSize() + Elements.GetAllocatedSize();
	}
	void Reset()
	{
		HashSize = 0;
		Elements.Reset();
		HashTable.Reset();
	}
	void Empty()
	{
		HashSize = 0;
		Elements.Empty();
		HashTable.Empty();
	}
	void Reserve(const int32 Number)
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
	FORCEINLINE int32 Find(const Type& Value) const
	{
		if (HashSize == 0)
		{
			return -1;
		}

		int32 ElementIndex = this->GetElementIndex(HashValue(Value));
		while (true)
		{
			if (ElementIndex == -1)
			{
				return -1;
			}

			const FElement& Element = Elements[ElementIndex];
			if (Element.Value == Value)
			{
				return ElementIndex;
			}
			ElementIndex = Element.NextElementIndex;
		}
	}

	FORCEINLINE bool Contains(const Type& Value) const
	{
		if (HashSize == 0)
		{
			return false;
		}

		int32 ElementIndex = this->GetElementIndex(HashValue(Value));
		while (true)
		{
			if (ElementIndex == -1)
			{
				return false;
			}

			const FElement& Element = Elements[ElementIndex];
			if (Element.Value == Value)
			{
				return true;
			}
			ElementIndex = Element.NextElementIndex;
		}
	}
	template<typename LambdaType>
	FORCEINLINE bool Contains(const uint32 Hash, LambdaType Matches) const
	{
		if (HashSize == 0)
		{
			return false;
		}

		int32 ElementIndex = this->GetElementIndex(Hash);
		while (true)
		{
			if (ElementIndex == -1)
			{
				return false;
			}

			const FElement& Element = Elements[ElementIndex];
			if (Matches(Element.Value))
			{
				return true;
			}
			ElementIndex = Element.NextElementIndex;
		}
	}

	FORCEINLINE int32 Add(const Type& Value)
	{
		bool bIsInSet = false;
		return this->FindOrAdd(Value, bIsInSet);
	}
	FORCEINLINE int32 FindOrAdd(const Type& Value, bool& bIsInSet)
	{
		const uint32 Hash = HashValue(Value);

		if (HashSize > 0)
		{
			int32 ElementIndex = this->GetElementIndex(Hash);
			while (ElementIndex != -1)
			{
				FElement& Element = Elements[ElementIndex];
				if (Element.Value == Value)
				{
					bIsInSet = true;
					return ElementIndex;
				}
				ElementIndex = Element.NextElementIndex;
			}
		}

		bIsInSet = false;

		const int32 NewElementIndex = Elements.Emplace();
		FElement& Element = Elements[NewElementIndex];
		Element.Value = Value;

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

		return NewElementIndex;
	}
	FORCEINLINE void Add_NoRehash(const Type& Value)
	{
		const uint32 Hash = HashValue(Value);

		if (HashSize > 0)
		{
			int32 ElementIndex = this->GetElementIndex(Hash);
			while (ElementIndex != -1)
			{
				FElement& Element = Elements[ElementIndex];
				if (Element.Value == Value)
				{
					return;
				}
				ElementIndex = Element.NextElementIndex;
			}
		}

		const int32 NewElementIndex = Elements.Emplace();
		FElement& Element = Elements[NewElementIndex];
		Element.Value = Value;

		const int32 DesiredHashSize = TSetAllocator<>::GetNumberOfHashBuckets(Elements.Num());
		checkVoxelSlow(HashSize >= DesiredHashSize);

		int32& ElementIndex = GetElementIndex(Hash);
		Element.NextElementIndex = ElementIndex;
		ElementIndex = NewElementIndex;
	}

	template<typename ArrayType>
	FORCENOINLINE void BulkAdd(const ArrayType& NewElements)
	{
		VOXEL_FUNCTION_COUNTER_NUM(NewElements.Num(), 1024);

		checkVoxelSlow(Num() == 0);
		HashSize = TSetAllocator<>::GetNumberOfHashBuckets(NewElements.Num());

		FVoxelUtilities::SetNumFast(Elements, NewElements.Num());

		for (int32 Index = 0; Index < NewElements.Num(); Index++)
		{
			Elements[Index].Value = NewElements[Index];
		}

		Rehash();
	}

public:
	struct FIterator
	{
		typename TVoxelArray<FElement>::RangedForConstIteratorType It;

		FORCEINLINE explicit FIterator(const typename TVoxelArray<FElement>::RangedForConstIteratorType& It)
			: It(It)
		{
		}

		FORCEINLINE FIterator& operator++()
		{
			++It;
			return *this;
		}
		FORCEINLINE explicit operator bool() const
		{
			return bool(It);
		}
		FORCEINLINE const Type& operator*() const
		{
			return (*It).Value;
		}
		FORCEINLINE bool operator!=(const FIterator& Other) const
		{
			return It != Other.It;
		}
	};

	FORCEINLINE FIterator begin() const
	{
		return FIterator(Elements.begin());
	}
	FORCEINLINE FIterator end() const
	{
		return FIterator(Elements.end());
	}

private:
	int32 HashSize = 0;
	TVoxelArray<int32> HashTable;
	TVoxelArray<FElement> Elements;

	FORCEINLINE static uint32 HashValue(const Type& Value)
	{
		if constexpr (std::is_same_v<Type, FIntVector>)
		{
			return FVoxelUtilities::FastIntVectorHash(Value);
		}
		if constexpr (std::is_same_v<Type, FIntPoint>)
		{
			return FVoxelUtilities::FastIntPointHash(Value);
		}

		return GetTypeHash(Value);
	}

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
		FVoxelUtilities::SetNumFast(HashTable, HashSize);
		FVoxelUtilities::Memset(HashTable, 0xFF);

		for (int32 Index = 0; Index < Elements.Num(); Index++)
		{
			FElement& Element = Elements[Index];

			int32& ElementIndex = this->GetElementIndex(HashValue(Element.Value));
			Element.NextElementIndex = ElementIndex;
			ElementIndex = Index;
		}
	}
};