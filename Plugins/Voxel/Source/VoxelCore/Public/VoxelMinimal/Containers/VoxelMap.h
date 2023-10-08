// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelSparseArray.h"
#include "VoxelMinimal/Utilities/VoxelBaseUtilities.h"

// ~30-50% faster than TMap in dev builds for reads
// up to 4x faster for adds if using Add_CheckNew
template<typename InKeyType, typename InValueType, typename SetAllocator = FVoxelSetAllocator, typename KeyFuncs = TDefaultMapHashableKeyFuncs<InKeyType, InValueType, false>>
class TVoxelMap : public TMap<InKeyType, InValueType, SetAllocator, KeyFuncs>
{
public:
	using Super = TMap<InKeyType, InValueType, SetAllocator, KeyFuncs>;
	using typename Super::ValueType;
	using typename Super::KeyType;
	using typename Super::KeyConstPointerType;
	using typename Super::ElementType;
	using Super::Pairs;
	using Super::Super;

	using SetElementType = TSetElement<ElementType>;
	using ElementArrayType = TVoxelSparseArray<SetElementType, int32, typename SetAllocator::SparseArrayAllocator>;
	using HashType = typename SetAllocator::HashAllocator::template ForElementType<int32>;

public:
	// Hide "Might not be initialized" warnings
#if INTELLISENSE_PARSER
	TVoxelMap() {}
#endif

	FORCEINLINE void Reserve(int32 Number)
	{
		GetSet().Reserve(Number);
	}

public:
	FORCEINLINE InValueType* Find(KeyConstPointerType Key)
	{
		ElementType* Pair = GetSet().Find(Key);
		if (!Pair)
		{
			return nullptr;
		}
		return &Pair->Value;
	}
	FORCEINLINE const InValueType* Find(KeyConstPointerType Key) const
	{
		return ConstCast(this)->Find(Key);
	}

	FORCEINLINE InValueType& FindChecked(const KeyType& Key)
	{
		return GetSet().FindChecked(Key).Value;
	}
	FORCEINLINE const InValueType& FindChecked(const KeyType& Key) const
	{
		return ConstCast(this)->FindChecked(Key);
	}

	FORCEINLINE InValueType& operator[](const KeyType& Key)
	{
		return this->FindChecked(Key);
	}
	FORCEINLINE const InValueType& operator[](const KeyType& Key) const
	{
		return this->FindChecked(Key);
	}

public:
	FORCEINLINE InValueType& FindOrAdd(const KeyType& Key)
	{
		const uint32 KeyHash = KeyFuncs::GetKeyHash(Key);
		if (ElementType* Pair = GetSet().FindByHash(KeyHash, Key))
		{
			return Pair->Value;
		}

		return GetSet().AddByHash_CheckNew(KeyHash, Key).Value;
	}
	FORCEINLINE InValueType& FindOrAdd(KeyType&& Key)
	{
		return this->FindOrAdd(Key);
	}

public:
	FORCEINLINE InValueType& Add(const KeyType& Key)
	{
		return this->FindOrAdd(Key);
	}
	FORCEINLINE InValueType& Add(KeyType&& Key)
	{
		return this->FindOrAdd(Key);
	}

	FORCEINLINE InValueType& Add(const KeyType& Key, const ValueType& Value)
	{
		ValueType& ValueRef = this->Add(Key);
		ValueRef = Value;
		return ValueRef;
	}
	FORCEINLINE InValueType& Add(KeyType&& Key, const ValueType& Value)
	{
		ValueType& ValueRef = this->Add(Key);
		ValueRef = Value;
		return ValueRef;
	}

	FORCEINLINE InValueType& Add(const KeyType& Key, ValueType&& Value)
	{
		ValueType& ValueRef = this->Add(Key);
		ValueRef = MoveTemp(Value);
		return ValueRef;
	}
	FORCEINLINE InValueType& Add(KeyType&& Key, ValueType&& Value)
	{
		ValueType& ValueRef = this->Add(Key);
		ValueRef = MoveTemp(Value);
		return ValueRef;
	}

public:
	FORCEINLINE InValueType& Add_CheckNew(const KeyType& Key)
	{
		const uint32 KeyHash = KeyFuncs::GetKeyHash(Key);
		return GetSet().AddByHash_CheckNew(KeyHash, Key).Value;
	}

	FORCEINLINE InValueType& Add_CheckNew(const KeyType& Key, const ValueType& Value)
	{
		ValueType& ValueRef = this->Add_CheckNew(Key);
		ValueRef = Value;
		return ValueRef;
	}
	FORCEINLINE InValueType& Add_CheckNew(const KeyType& Key, ValueType&& Value)
	{
		ValueType& ValueRef = this->Add_CheckNew(Key);
		ValueRef = MoveTemp(Value);
		return ValueRef;
	}

private:
	using Super::Add;
	using Super::Find;
	using Super::FindOrAdd;

	struct FSet
	{
	public:
		ElementArrayType Elements;
		mutable HashType Hash;
		mutable int32 HashSize;

		FORCEINLINE int32& GetTypedHash(const int32 HashIndex) const
		{
			checkStatic(sizeof(FSetElementId) == sizeof(int32));
			return static_cast<int32*>(Hash.GetAllocation())[HashIndex & (HashSize - 1)];
		}

	public:
		FORCEINLINE int32 FindId(KeyConstPointerType Key) const
		{
			if (Elements.Num() == 0)
			{
				return -1;
			}

			for (
				int32 ElementId = GetTypedHash(KeyFuncs::GetKeyHash(Key));
				ElementId != -1;
				ElementId = ReinterpretCastRef<int32>(Elements[ElementId].HashNextId))
			{
				if (KeyFuncs::Matches(KeyFuncs::GetSetKey(Elements[ElementId].Value), Key))
				{
					return ElementId;
				}
			}

			return -1;
		}
		template<typename ComparableKey>
		FORCEINLINE int32 FindIdByHash(uint32 KeyHash, const ComparableKey& Key) const
		{
			checkVoxelSlow(KeyHash == KeyFuncs::GetKeyHash(Key));

			if (Elements.Num() == 0)
			{
				return -1;
			}

			for (
				int32 ElementId = GetTypedHash(KeyHash);
				ElementId != -1;
				ElementId = ReinterpretCastRef<int32>(Elements[ElementId].HashNextId))
			{
				if (KeyFuncs::Matches(KeyFuncs::GetSetKey(Elements[ElementId].Value), Key))
				{
					return ElementId;
				}
			}

			return -1;
		}
		FORCEINLINE int32 FindIdChecked(KeyConstPointerType Key) const
		{
			checkVoxelSlow(Elements.Num() > 0);

			int32 ElementId = this->GetTypedHash(KeyFuncs::GetKeyHash(Key));
			while (true)
			{
				checkVoxelSlow(ElementId != -1);

				const SetElementType& Element = Elements[ElementId];
				if (KeyFuncs::Matches(KeyFuncs::GetSetKey(Element.Value), Key))
				{
					return ElementId;
				}
				ElementId = ReinterpretCastRef<int32>(Element.HashNextId);
			}
		}

	public:
		FORCEINLINE ElementType* Find(KeyConstPointerType Key)
		{
			const int32 ElementId = this->FindId(Key);
			if (ElementId != -1)
			{
				return &Elements[ElementId].Value;
			}
			else
			{
				return nullptr;
			}
		}
		template<typename ComparableKey>
		FORCEINLINE ElementType* FindByHash(uint32 KeyHash, const ComparableKey& Key)
		{
			const int32 ElementId = this->FindIdByHash(KeyHash, Key);
			if (ElementId != -1)
			{
				return &Elements[ElementId].Value;
			}
			else
			{
				return nullptr;
			}
		}
		FORCEINLINE ElementType& FindChecked(KeyConstPointerType Key)
		{
			const int32 ElementId = this->FindIdChecked(Key);
			checkVoxelSlow(ElementId != -1);
			return Elements[ElementId].Value;
		}

	public:
		FORCEINLINE ElementType& AddByHash_CheckNew(uint32 KeyHash, const KeyType& Key)
		{
			checkVoxelSlow(this->FindIdByHash(KeyHash, Key) == -1);

			FSparseArrayAllocationInfo ElementAllocation = Elements.AddUninitialized();

			SetElementType& Element = *new (ElementAllocation.Pointer) SetElementType(TKeyInitializer<KeyType>(Key));
			this->RehashOrLink(KeyHash, Element, ElementAllocation.Index);
			return Element.Value;
		}
		FORCEINLINE void RehashOrLink(const uint32 KeyHash, const SetElementType& Element, const int32 ElementId)
		{
			const int32 DesiredHashSize = SetAllocator::GetNumberOfHashBuckets(Elements.Num());

			if (Elements.Num() > 0 &&
				HashSize < DesiredHashSize)
			{
				HashSize = DesiredHashSize;
				Rehash();
				return;
			}

			this->LinkElement(ElementId, Element, KeyHash);
		}
		FORCENOINLINE void Rehash() const
		{
			VOXEL_FUNCTION_COUNTER_NUM(HashSize, 1024);

			// Free the old hash.
			Hash.ResizeAllocation(0, 0, sizeof(FSetElementId));

			if (HashSize == 0)
			{
				return;
			}

			// Allocate the new hash.
			checkVoxelSlow(FMath::IsPowerOfTwo(HashSize));
			Hash.ResizeAllocation(0, HashSize, sizeof(FSetElementId));

			FVoxelUtilities::SetAll(MakeArrayView(Hash.GetAllocation(), HashSize), -1);

			// Add the existing elements to the new hash.
			for (typename ElementArrayType::TConstIterator ElementIt(Elements); ElementIt; ++ElementIt)
			{
				this->LinkElement(ElementIt.GetIndex(), *ElementIt, KeyFuncs::GetKeyHash(KeyFuncs::GetSetKey(ElementIt->Value)));
			}
		}
		FORCEINLINE void LinkElement(const int32 ElementId, const SetElementType& Element, const uint32 KeyHash) const
		{
			Element.HashIndex = KeyHash & (HashSize - 1);
			ReinterpretCastRef<int32>(Element.HashNextId) = this->GetTypedHash(Element.HashIndex);
			this->GetTypedHash(Element.HashIndex) = ElementId;
		}

	public:
		FORCENOINLINE void Reserve(int32 Number)
		{
			if (Number <= Elements.Num())
			{
				return;
			}

			Elements.Reserve(Number);

			const int32 NewHashSize = SetAllocator::GetNumberOfHashBuckets(Number);

			if (HashSize < NewHashSize)
			{
				HashSize = NewHashSize;
				Rehash();
			}
		}
	};

	FORCEINLINE FSet& GetSet()
	{
		return ReinterpretCastRef<FSet>(this->Pairs);
	}
	FORCEINLINE const FSet& GetSet() const
	{
		return ReinterpretCastRef<FSet>(this->Pairs);
	}
};