// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "Containers/DynamicRHIResourceArray.h"

template<typename Type, Type InValue>
struct TVoxelIntegralConstant
{
    static constexpr Type Value = InValue;

    constexpr operator Type() const
	{
        return Value;
    }

    constexpr Type operator()() const
	{
        return Value;
    }
};

template<typename Type>
struct TVoxelTypeInstance
{
    constexpr Type operator()() const
	{
        return {};
    }
};

using FVoxelTrueType = TVoxelIntegralConstant<bool, true>;
using FVoxelFalseType = TVoxelIntegralConstant<bool, false>;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<int32 Bits, typename = void>
struct TVoxelTypeForBits;

template<int32 Bits>
struct TVoxelTypeForBits<Bits, typename TEnableIf<0 < Bits && Bits <= 8>::Type>
{
	using Type = uint8;
};
template<int32 Bits>
struct TVoxelTypeForBits<Bits, typename TEnableIf<8 < Bits && Bits <= 16>::Type>
{
	using Type = uint16;
};
template<int32 Bits>
struct TVoxelTypeForBits<Bits, typename TEnableIf<16 < Bits && Bits <= 32>::Type>
{
	using Type = uint32;
};
template<int32 Bits>
struct TVoxelTypeForBits<Bits, typename TEnableIf<32 < Bits && Bits <= 64>::Type>
{
	using Type = uint64;
};

template<int32 Bytes>
struct TVoxelTypeForBytes : TVoxelTypeForBits<Bytes * 8>
{
};

template<typename ElementType, uint32 Alignment>
struct TIsContiguousContainer<TResourceArray<ElementType, Alignment>> : TIsContiguousContainer<TArray<ElementType, TMemoryImageAllocator<Alignment>>>
{
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FORCEINLINE FName operator+(const FString& A, const FName B)
{
	return *(A + B.ToString());
}
template<typename CharType>
FORCEINLINE FString operator+(const FString& A, const CharType* B)
{
	return A + FString(B);
}
template<typename CharType>
FORCEINLINE FString operator+(FString&& A, const CharType* B)
{
	return A + FString(B);
}

FORCEINLINE FName operator+(const FName A, const FString& B)
{
	return *(A.ToString() + B);
}
template<typename CharType>
FORCEINLINE FString operator+(const CharType* A, const FString& B)
{
	return FString(A) + B;
}
template<typename CharType>
FORCEINLINE FString operator+(const CharType* A, FString&& B)
{
	return FString(A) + B;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace FVoxelUtilities
{
	/** Static lookup table used for FColor -> FLinearColor conversion. sRGB */
	VOXELCORE_API extern float sRGBToLinearTable[256];

	template<typename T>
	FORCEINLINE constexpr T PositiveMod(T X, T Y)
	{
		return ((X % Y) + Y) % Y;
	}

	// Unlike DivideAndRoundDown, actually floors the result even if Dividend < 0
	template<typename T>
	FORCEINLINE constexpr T DivideFloor(T Dividend, T Divisor)
	{
		const T Q = Dividend / Divisor;
		const T R = Dividend % Divisor;
		return R ? (Q - ((Dividend < 0) ^ (Divisor < 0))) : Q;
	}
	template<typename T>
	FORCEINLINE T DivideFloor_Positive(T Dividend, T Divisor)
	{
		checkVoxelSlow(Dividend >= 0);
		checkVoxelSlow(Divisor > 0);
		return Dividend / Divisor;
	}
	// ~2x faster than DivideFloor
	FORCEINLINE int32 DivideFloor_FastLog2(int32 Dividend, int32 DivisorLog2)
	{
		checkVoxelSlow(DivisorLog2 >= 0);
		ensureVoxelSlow(DivisorLog2 < 32);
		constexpr int32 Offset = 1 << 30;
		checkVoxelSlow(Dividend < Offset);
		checkVoxelSlow(Dividend > -Offset);
		// Make the dividend positive to be able to use bit shifts
		const int32 Result = ((Dividend + Offset) >> DivisorLog2) - (Offset >> DivisorLog2);
		checkVoxelSlow(Result == DivideFloor(Dividend, 1 << DivisorLog2));
		return Result;
	}

	FORCEINLINE constexpr int32 DivideCeil(int32 Dividend, int32 Divisor)
	{
		return (Dividend > 0) ? 1 + (Dividend - 1) / Divisor : (Dividend / Divisor);
	}
	FORCEINLINE constexpr int64 DivideCeil64(int64 Dividend, int64 Divisor)
	{
		return (Dividend > 0) ? 1 + (Dividend - 1) / Divisor : (Dividend / Divisor);
	}
	template<typename T>
	FORCEINLINE T DivideCeil_Positive(T Dividend, T Divisor)
	{
		checkVoxelSlow(Dividend >= 0);
		checkVoxelSlow(Divisor > 0);
		return (Dividend + Divisor - 1) / Divisor;
	}

	FORCEINLINE constexpr int32 DivideRound(int32 Dividend, int32 Divisor)
	{
		const int32 R = PositiveMod(Dividend, Divisor);
		if (R < Divisor / 2)
		{
			return DivideFloor(Dividend, Divisor);
		}
		else
		{
			return DivideCeil(Dividend, Divisor);
		}
	}

	template<int32 InX>
	FORCEINLINE constexpr int32 FloorLog2()
	{
		int32 X = InX;

		int32 Exp = -1;
		while (X)
		{
			X >>= 1;
			++Exp;
		}

		if (1 << Exp == InX)
		{
			return Exp;
		}
		else
		{
			return Exp - 1;
		}
	}
	template<int32 InX>
	FORCEINLINE constexpr int32 CeilLog2()
	{
		int32 X = InX;

		int32 Exp = -1;
		while (X)
		{
			X >>= 1;
			++Exp;
		}

		if (1 << Exp == InX)
		{
			return Exp;
		}
		else
		{
			return Exp + 1;
		}
	}
	template<int32 InX>
	FORCEINLINE constexpr int32 ExactLog2()
	{
		checkStatic(FMath::IsPowerOfTwo(InX));

		int32 X = InX;
		int32 Exp = -1;
		while (X)
		{
			X >>= 1;
			++Exp;
		}
		return Exp;
	}
	template<int32 A, int32 B>
	FORCEINLINE constexpr int32 ExactDivide()
	{
		checkStatic(A % B == 0);
		return A / B;
	}

	FORCEINLINE int32 ExactLog2(int32 X)
	{
		const int32 Log2 = FMath::FloorLog2(X);
		checkVoxelSlow((1 << Log2) == X);
		return Log2;
	}

	FORCEINLINE bool IsMultipleOfPowerOfTwo(int32 Number, int32 PowerOfTwo)
	{
		checkVoxelSlow(FMath::IsPowerOfTwo(PowerOfTwo));
		return (Number & (PowerOfTwo - 1)) == 0;
	}

	FORCEINLINE constexpr uint32 MakeLowerMask(int32 NumBits)
	{
		return (1 << NumBits) - 1;
	}

	FORCEINLINE int64 IntPow(int64 Value, int64 Exponent)
	{
		checkVoxelSlow(Exponent >= 0);
		ensureVoxelSlow(Exponent < 1024);

		int64 Result = 1;
		for (int32 Index = 0; Index < Exponent; Index++)
		{
			Result *= Value;
		}
		return Result;
	}

	template<int32 NumPerChunk>
	FORCEINLINE int32 GetChunkIndex(const int32 Index)
	{
		// Make sure to not use unsigned division as it would add a sign check
		checkVoxelSlow(Index >= 0);
		constexpr int32 Log2 = ExactLog2<NumPerChunk>();
		return Index >> Log2;
	}
	template<int32 NumPerChunk>
	FORCEINLINE int32 GetChunkOffset(const int32 Index)
	{
		checkVoxelSlow(Index >= 0);
		checkStatic(FMath::IsPowerOfTwo(NumPerChunk));
		return Index & (NumPerChunk - 1);
	}

	VOXELCORE_API FText ConvertToTimeText(double Value, int32 NumExtraDigits = 0);
	VOXELCORE_API FText ConvertToNumberText(double Value);

	VOXELCORE_API bool IsInt(const FStringView& Text);
	VOXELCORE_API bool IsFloat(const FStringView& Text);

	FORCEINLINE int32 Atoi(const FStringView& Text)
	{
#if VOXEL_DEBUG
		ensure(IsInt(Text));
#endif
		return FCString::Atoi(Text.GetData());
	}
	FORCEINLINE float Atof(const FStringView& Text)
	{
#if VOXEL_DEBUG
		ensure(IsFloat(Text));
#endif
		return FCString::Atof(Text.GetData());
	}

	VOXELCORE_API FName AppendName(const FStringView& Base, FName Name);

	FORCEINLINE uint8 CastToUINT8(int32 Value)
	{
		ensureMsgfVoxelSlowNoSideEffects(0 <= Value && Value < 256, TEXT("Invalid uint8 value: %d"), Value);
		return Value;
	}
	FORCEINLINE constexpr int8 ClampToINT8(int32 Value)
	{
		return FMath::Clamp<int32>(Value, MIN_int8, MAX_int8);
	}
	FORCEINLINE constexpr uint8 ClampToUINT8(int32 Value)
	{
		return FMath::Clamp<int32>(Value, MIN_uint8, MAX_uint8);
	}
	FORCEINLINE constexpr int8 ClampToINT16(int32 Value)
	{
		return FMath::Clamp<int32>(Value, MIN_int16, MAX_int16);
	}
	FORCEINLINE constexpr uint16 ClampToUINT16(int32 Value)
	{
		return FMath::Clamp<int32>(Value, MIN_uint16, MAX_uint16);
	}

	FORCEINLINE uint8 FloatToUINT8(float Float)
	{
		return ClampToUINT8(FMath::FloorToInt(Float * 255.999f));
	}
	FORCEINLINE constexpr float UINT8ToFloat(uint8 Int)
	{
		return Int / 255.f;
	}

	// Round up if the new value is higher than the previous one, to avoid being stuck
	FORCEINLINE uint8 FloatToUINT8_ForLerp(float NewValue, float OldValue)
	{
		return ClampToUINT8(NewValue > OldValue
			? FMath::CeilToInt(NewValue * 255.999f)
			: FMath::FloorToInt(NewValue * 255.999f));
	}
	FORCEINLINE uint8 LerpUINT8(uint8 A, uint8 B, float Alpha)
	{
		const float Result = FMath::Lerp<float>(A, B, Alpha);
		// Do special rounding to not get stuck, eg Lerp(251, 255, 0.1) = 251 should be 252 instead
		// and Lerp(255, 251, 0.1) should be 254
		const int32 RoundedResult = (Alpha > 0) == (A < B) ? FMath::CeilToInt(Result) : FMath::FloorToInt(Result);
		return ClampToUINT8(RoundedResult);
	}

	FORCEINLINE FColor FloatToUINT8(FLinearColor Float)
	{
		return
		FColor
		{
			FloatToUINT8(Float.R),
			FloatToUINT8(Float.G),
			FloatToUINT8(Float.B),
			FloatToUINT8(Float.A)
		};
	}
	FORCEINLINE FLinearColor UINT8ToFloat(FColor Int)
	{
		return
		FLinearColor
		{
			UINT8ToFloat(Int.R),
			UINT8ToFloat(Int.G),
			UINT8ToFloat(Int.B),
			UINT8ToFloat(Int.A)
		};
	}

	FORCEINLINE uint16 FloatToUINT16(float Float)
	{
		return ClampToUINT16(FMath::FloorToInt(Float * 65535.999f));
	}
	FORCEINLINE constexpr float UINT16ToFloat(uint16 Int)
	{
		return Int / 65535.f;
	}

	FORCEINLINE bool IsValidUINT8(int32 Value)
	{
		return 0 <= Value && Value < MAX_uint8;
	}
	FORCEINLINE bool IsValidUINT16(int32 Value)
	{
		return 0 <= Value && Value < MAX_uint16;
	}

	FORCEINLINE float& FloatBits(uint32& Value)
	{
		return reinterpret_cast<float&>(Value);
	}
	FORCEINLINE uint32& IntBits(float& Value)
	{
		return reinterpret_cast<uint32&>(Value);
	}
	FORCEINLINE const float& FloatBits(const uint32& Value)
	{
		return reinterpret_cast<const float&>(Value);
	}
	FORCEINLINE const uint32& IntBits(const float& Value)
	{
		return reinterpret_cast<const uint32&>(Value);
	}

	FORCEINLINE bool SignBit(const float& Value)
	{
		return IntBits(Value) >> 31;
	}
	template<typename T>
	bool SignBit(const T&) = delete;

	FORCEINLINE int32 MakeFastDivide16Magic(const int32 Divisor)
	{
		checkVoxelSlow(0 < Divisor);
		checkVoxelSlow(1 <= Divisor && Divisor <= MAX_uint8);
		return (1 << 16) / Divisor + 1;
	}
	FORCEINLINE int64 MakeFastDivide32Magic(const int32 Divisor)
	{
		checkVoxelSlow(0 < Divisor);
		checkVoxelSlow(1 <= Divisor && Divisor <= MAX_uint16);
		return (int64(1) << 32) / Divisor + 1;
	}

	FORCEINLINE int32 FastDivide16(const int32 Dividend, const int32 Divisor, const int32 Magic)
	{
		checkVoxelSlow(0 <= Dividend && Dividend <= MAX_uint8);
		checkVoxelSlow(Magic == MakeFastDivide16Magic(Divisor));
		const int32 Result = (Dividend * Magic) >> 16;
		checkVoxelSlow(Result == (Dividend / Divisor));
		return Result;
	}
	FORCEINLINE int32 FastDivide32(const int32 Dividend, const int32 Divisor, const int64 Magic)
	{
		checkVoxelSlow(0 <= Dividend && Dividend <= MAX_uint16);
		checkVoxelSlow(Magic == MakeFastDivide32Magic(Divisor));
		const int32 Result = (Dividend * Magic) >> 32;
		checkVoxelSlow(Result == (Dividend / Divisor));
		return Result;
	}

	// Returns in [0, 1)
	FORCEINLINE float GetFraction(const uint32 Seed)
	{
		const float Result = FloatBits(0x3F800000U | (Seed >> 9)) - 1.0f;
		checkVoxelSlow(0.f <= Result && Result < 1.f);
		return Result;
	}
	// Returns in [Min, Max]
	FORCEINLINE int32 RandRange(const uint32 Seed, const int32 Min, const int32 Max)
	{
		checkVoxelSlow(Min <= Max);
		const int32 Result = Min + FMath::FloorToInt(GetFraction(Seed) * float((Max - Min) + 1));
		checkVoxelSlow(Min <= Result && Result <= Max);
		return Result;
	}

	FORCEINLINE constexpr uint32 MurmurHash32(uint32 Hash)
	{
		Hash ^= Hash >> 16;
		Hash *= 0x85ebca6b;
		Hash ^= Hash >> 13;
		Hash *= 0xc2b2ae35;
		Hash ^= Hash >> 16;
		return Hash;
	}
	FORCEINLINE constexpr uint64 MurmurHash64(uint64 Hash)
	{
		Hash ^= Hash >> 33;
		Hash *= 0xff51afd7ed558ccd;
		Hash ^= Hash >> 33;
		Hash *= 0xc4ceb9fe1a85ec53;
		Hash ^= Hash >> 33;
		return Hash;
	}

	FORCEINLINE uint32 MurmurHash32xN(uint32 const* RESTRICT Hash, int32 Size, uint32 Seed = 0)
	{
		uint32 H = 1831214719 * (1460481823 + Seed);
		for (int32 Index = 0; Index < Size; Index++)
		{
			uint32 K = Hash[Index];
			K *= 0xcc9e2d51;
			K = (K << 15) | (K >> 17);
			K *= 0x1b873593;
			H ^= K;
			H = (H << 13) | (H >> 19);
			H = H * 5 + 0xe6546b64;
		}

		H ^= uint32(Size);
		H ^= H >> 16;
		H *= 0x85ebca6b;
		H ^= H >> 13;
		H *= 0xc2b2ae35;
		H ^= H >> 16;
		return H;
	}
	template<typename T, typename = typename TEnableIf<TIsTriviallyDestructible<T>::Value && (
		sizeof(T) == sizeof(uint8) ||
		sizeof(T) == sizeof(uint16) ||
		sizeof(T) % sizeof(uint32) == 0)>::Type>
	FORCEINLINE uint64 MurmurHash(const T& Value, const uint32 Seed = 0)
	{
		checkStatic(!TIsTArrayView<T>::Value);

		uint32 H = 1831214719 * (1460481823 + Seed);

		if constexpr (std::is_same_v<T, bool>)
		{
			return MurmurHash64(H ^ uint64(Value ? 3732917 : 2654653));
		}
		if constexpr (sizeof(T) == sizeof(uint8))
		{
			return MurmurHash64(H ^ uint64(ReinterpretCastRef<uint8>(Value)));
		}
		if constexpr (sizeof(T) == sizeof(uint16))
		{
			return MurmurHash64(H ^ uint64(ReinterpretCastRef<uint16>(Value)));
		}
		if constexpr (sizeof(T) == sizeof(uint32))
		{
			return MurmurHash64(H ^ uint64(ReinterpretCastRef<uint32>(Value)));
		}
		if constexpr (sizeof(T) == sizeof(uint64))
		{
			return MurmurHash64(H ^ uint64(ReinterpretCastRef<uint64>(Value)));
		}

		constexpr int32 Size = sizeof(T) / sizeof(uint32);
		const uint32* RESTRICT Hash = reinterpret_cast<const uint32*>(&Value);

		for (int32 Index = 0; Index < Size; Index++)
		{
			uint32 K = Hash[Index];
			K *= 0xcc9e2d51;
			K = (K << 15) | (K >> 17);
			K *= 0x1b873593;
			H ^= K;
			H = (H << 13) | (H >> 19);
			H = H * 5 + 0xe6546b64;
		}
		return MurmurHash64(H ^ uint32(Size));
	}
	FORCEINLINE uint64 MurmurHashBytes(const TConstArrayView<uint8>& Bytes, const uint32 Seed = 0)
	{
		constexpr int32 WordSize = sizeof(uint32);
		const int32 Size = Bytes.Num() / WordSize;
		const uint32* RESTRICT Hash = reinterpret_cast<const uint32*>(Bytes.GetData());

		uint32 H = 1831214719 * (1460481823 + Seed);
		for (int32 Index = 0; Index < Size; Index++)
		{
			uint32 K = Hash[Index];
			K *= 0xcc9e2d51;
			K = (K << 15) | (K >> 17);
			K *= 0x1b873593;
			H ^= K;
			H = (H << 13) | (H >> 19);
			H = H * 5 + 0xe6546b64;
		}

		uint32 Tail = 0;
		for (int32 Index = Size * WordSize; Index < Bytes.Num(); Index++)
		{
			Tail = (Tail << 8) | Bytes[Index];
		}

		return MurmurHash64(H ^ Tail ^ uint32(Size));
	}

	template<typename T>
	FORCEINLINE uint64 MurmurHashMultiImpl(uint32 Seed, const T& Arg)
	{
		return FVoxelUtilities::MurmurHash(Arg, Seed);
	}
	template<typename T, typename... ArgTypes>
	FORCEINLINE uint64 MurmurHashMultiImpl(uint32 Seed, const T& Arg, const ArgTypes&... Args)
	{
		return FVoxelUtilities::MurmurHash(Arg, Seed) ^ FVoxelUtilities::MurmurHashMultiImpl(Seed + 1, Args...);
	}

	template<typename... ArgTypes>
	FORCEINLINE uint64 MurmurHashMulti(const ArgTypes&... Args)
	{
		return FVoxelUtilities::MurmurHashMultiImpl(0, Args...);
	}

	template<uint32 Base>
	FORCEINLINE float Halton(uint32 Index)
	{
		float Result = 0.0f;
		const float InvBase = 1.0f / Base;
		float Fraction = InvBase;
		while (Index > 0)
		{
			Result += (Index % Base) * Fraction;
			Index /= Base;
			Fraction *= InvBase;
		}
		return Result;
	}

	/**
	 * Y
	 * ^ C - D
	 * | |   |
	 * | A - B
	 *  -----> X
	 */
	template<typename T, typename U = float>
	FORCEINLINE T BilinearInterpolation(T A, T B, T C, T D, U X, U Y)
	{
		T AB = FMath::Lerp(A, B, X);
		T CD = FMath::Lerp(C, D, X);
		return FMath::Lerp(AB, CD, Y);
	}

	/**
	 * Y
	 * ^ C - D
	 * | |   |
	 * | A - B
	 * 0-----> X
	 * Y
	 * ^ G - H
	 * | |   |
	 * | E - F
	 * 1-----> X
	 */
	template<typename T, typename U = float>
	FORCEINLINE T TrilinearInterpolation(
		T A, T B, T C, T D,
		T E, T F, T G, T H,
		U X, U Y, U Z)
	{
		const T ABCD = BilinearInterpolation<T, U>(A, B, C, D, X, Y);
		const T EFGH = BilinearInterpolation<T, U>(E, F, G, H, X, Y);
		return FMath::Lerp(ABCD, EFGH, Z);
	}

	template<typename T>
	FORCEINLINE T&& VariadicMin(T&& Val)
	{
		return Forward<T>(Val);
	}
	template<typename T0, typename T1, typename... Ts>
	FORCEINLINE auto VariadicMin(T0&& Val1, T1&& Val2, Ts&&... Vals)
	{
		return (Val1 < Val2) ?
			VariadicMin(Val1, Forward<Ts>(Vals)...) :
			VariadicMin(Val2, Forward<Ts>(Vals)...);
	}

	template<typename T>
	FORCEINLINE T&& VariadicMax(T&& Val)
	{
		return Forward<T>(Val);
	}
	template<typename T0, typename T1, typename... Ts>
	FORCEINLINE auto VariadicMax(T0&& Val1, T1&& Val2, Ts&&... Vals)
	{
		return (Val1 > Val2) ?
			VariadicMax(Val1, Forward<Ts>(Vals)...) :
			VariadicMax(Val2, Forward<Ts>(Vals)...);
	}

	FORCEINLINE int32 CountBits(uint32 Bits)
	{
#if PLATFORM_WINDOWS && !PLATFORM_COMPILER_CLANG
		// Force use the intrinsic
		return _mm_popcnt_u32(Bits);
#else
		return FMath::CountBits(Bits);
#endif
	}

	FORCEINLINE int32 CountBits(uint64 Bits)
	{
#if PLATFORM_WINDOWS && !PLATFORM_COMPILER_CLANG
		// Force use the intrinsic
		return _mm_popcnt_u64(Bits);
#else
		return FMath::CountBits(Bits);
#endif
	}

	struct CForceInitializable
	{
		template<typename T>
		auto Requires() -> decltype(T(ForceInit));
	};

	template<typename T>
	typename TEnableIf<!TModels<CForceInitializable, T>::Value, T>::Type MakeSafe()
	{
		return T();
	}
	template<typename T>
	typename TEnableIf<TModels<CForceInitializable, T>::Value, T>::Type MakeSafe()
	{
		return T(ForceInit);
	}

	template<typename T>
	T MakeZeroed()
	{
		T Value;
		FMemory::Memzero(&Value, sizeof(T));
		return Value;
	}

	FORCEINLINE bool MemoryEqual(const void* Buf1, const void* Buf2, SIZE_T Count)
	{
		return FPlatformMemory::Memcmp(Buf1, Buf2, Count) == 0;
	}
	template<typename ArrayType>
	void Memzero(ArrayType&& Array)
	{
		FMemory::Memzero(GetData(Array), GetNum(Array) * sizeof(decltype(*GetData(Array))));
	}
	template<typename ArrayType>
	void Memset(ArrayType&& Array, const uint8 Value)
	{
		FMemory::Memset(GetData(Array), Value, GetNum(Array) * sizeof(decltype(*GetData(Array))));
	}
	template<typename ArrayType>
	void LargeMemzero(ArrayType&& Array)
	{
		VOXEL_FUNCTION_COUNTER_NUM(GetNum(Array), 4096);
		FVoxelUtilities::Memzero(Array);
	}
	template<typename ArrayType>
	void LargeMemset(ArrayType&& Array, const uint8 Value)
	{
		VOXEL_FUNCTION_COUNTER_NUM(GetNum(Array), 4096);
		FVoxelUtilities::Memset(Array, Value);
	}
	template<typename DstArrayType, typename SrcArrayType, typename = typename TEnableIf<
		!TIsConst<typename TRemoveReference<decltype(*GetData(DeclVal<DstArrayType>()))>::Type>::Value &&
		std::is_same_v<
			VOXEL_GET_TYPE(*GetData(DeclVal<DstArrayType>())),
			VOXEL_GET_TYPE(*GetData(DeclVal<SrcArrayType>()))
		>>::Type>
	void Memcpy(DstArrayType&& Dest, SrcArrayType&& Src)
	{
		checkVoxelSlow(GetNum(Dest) == GetNum(Src));
		FMemory::Memcpy(GetData(Dest), GetData(Src), GetNum(Dest) * sizeof(decltype(*GetData(Dest))));
	}

	template<int32 StaticNum, typename DstArrayType, typename SrcArrayType, typename = typename TEnableIf<
		!TIsConst<typename TRemoveReference<decltype(*GetData(DeclVal<DstArrayType>()))>::Type>::Value &&
		std::is_same_v<
			VOXEL_GET_TYPE(*GetData(DeclVal<DstArrayType>())),
			VOXEL_GET_TYPE(*GetData(DeclVal<SrcArrayType>()))
		>>::Type>
	void StaticMemcpy(DstArrayType&& Dest, SrcArrayType&& Src)
	{
		checkVoxelSlow(GetNum(Src) == StaticNum);
		checkVoxelSlow(GetNum(Dest) == StaticNum);
		FMemory::Memcpy(GetData(Dest), GetData(Src), StaticNum * sizeof(decltype(*GetData(Dest))));
	}
	template<int32 StaticNum, typename ArrayType>
	void StaticMemzero(ArrayType&& Array)
	{
		checkVoxelSlow(GetNum(Array) == StaticNum);
		FMemory::Memzero(GetData(Array), StaticNum * sizeof(decltype(*GetData(Array))));
	}

	template<typename ArrayTypeA, typename ArrayTypeB, typename = typename TEnableIf<
		std::is_same_v<
			VOXEL_GET_TYPE(*GetData(DeclVal<ArrayTypeA>())),
			VOXEL_GET_TYPE(*GetData(DeclVal<ArrayTypeB>()))
		> &&
		TIsTriviallyDestructible<VOXEL_GET_TYPE(*GetData(DeclVal<ArrayTypeA>()))>::Value
	>::Type>
	bool MemoryEqual(ArrayTypeA&& A, ArrayTypeB&& B)
	{
		checkVoxelSlow(GetNum(A) == GetNum(B));
		return FVoxelUtilities::MemoryEqual(GetData(A), GetData(B), GetNum(A) * sizeof(decltype(*GetData(A))));
	}

	template<typename ArrayType, typename NumType>
	void SetNum(ArrayType& Array, NumType Num)
	{
		Array.Reserve(Num);
		Array.SetNum(Num);
	}
	template<typename ArrayType>
	void SetNum(ArrayType& Array, const FIntVector& Size)
	{
		FVoxelUtilities::SetNum(Array, int64(Size.X) * int64(Size.Y) * int64(Size.Z));
	}

	template<typename ArrayType, typename NumType, typename = typename TEnableIf<TIsTriviallyDestructible<VOXEL_GET_TYPE(*::GetData(DeclVal<ArrayType>()))>::Value>::Type>
	void SetNumFast(ArrayType& Array, NumType Num)
	{
		Array.Reserve(Num);
		Array.SetNumUninitialized(Num, false);
	}
	template<typename ArrayType, typename = typename TEnableIf<TIsTriviallyDestructible<VOXEL_GET_TYPE(*::GetData(DeclVal<ArrayType>()))>::Value>::Type>
	void SetNumFast(ArrayType& Array, const FIntVector& Size)
	{
		FVoxelUtilities::SetNumFast(Array, int64(Size.X) * int64(Size.Y) * int64(Size.Z));
	}

	template<typename ArrayType, typename NumType>
	void SetNumZeroed(ArrayType& Array, NumType Num)
	{
		Array.Reserve(Num);
		Array.SetNumZeroed(Num);
	}
	template<typename ArrayType>
	void SetNumZeroed(ArrayType& Array, const FIntVector& Size)
	{
		FVoxelUtilities::SetNumZeroed(Array, int64(Size.X) * int64(Size.Y) * int64(Size.Z));
	}

	template<typename ArrayType, typename = typename TEnableIf<!TIsConst<typename TRemoveReference<decltype(*GetData(DeclVal<ArrayType>()))>::Type>::Value>::Type>
	FORCENOINLINE void SetAll(ArrayType&& Array, const VOXEL_GET_TYPE(*GetData(DeclVal<ArrayType>())) Value)
	{
		using T = VOXEL_GET_TYPE(*GetData(DeclVal<ArrayType>()));

		T* RESTRICT Start = GetData(Array);
		const T* RESTRICT End = Start + GetNum(Array);
		while (Start != End)
		{
			*Start = Value;
			Start++;
		}
	}

	template<typename ArrayType, typename IndicesArrayType>
	void RemoveAt(ArrayType& Array, const IndicesArrayType& SortedIndicesToRemove)
	{
		using T = VOXEL_GET_TYPE(*GetData(DeclVal<ArrayType>()));
		checkStatic(TIsPODType<T>::Value);

		if (SortedIndicesToRemove.Num() == 0)
		{
			return;
		}

#if VOXEL_DEBUG
		for (int64 Index = 0; Index < SortedIndicesToRemove.Num() - 1; Index++)
		{
			check(SortedIndicesToRemove[Index] < SortedIndicesToRemove[Index + 1]);
			check(Array.IsValidIndex(SortedIndicesToRemove[Index]));
			check(Array.IsValidIndex(SortedIndicesToRemove[Index + 1]));
		}
#endif

		if (SortedIndicesToRemove.Num() == Array.Num())
		{
			Array.Reset();
			return;
		}

		for (int64 Index = 0; Index < SortedIndicesToRemove.Num() - 1; Index++)
		{
			const int64 IndexToRemove = SortedIndicesToRemove[Index];
			const int64 NextIndexToRemove = SortedIndicesToRemove[Index + 1];

			// -1: don't count the element we're removing
			const int64 Count = NextIndexToRemove - IndexToRemove - 1;
			checkVoxelSlow(Count >= 0);

			if (Count == 0)
			{
				continue;
			}

			FMemory::Memmove(
				&Array[IndexToRemove - Index],
				&Array[IndexToRemove + 1],
				Count * sizeof(T));
		}

		{
			const int64 Index = SortedIndicesToRemove.Num() - 1;
			const int64 IndexToRemove = SortedIndicesToRemove[Index];
			const int64 NextIndexToRemove = Array.Num();

			// -1: don't count the element we're removing
			const int64 Count = NextIndexToRemove - IndexToRemove - 1;
			checkVoxelSlow(Count >= 0);

			if (Count != 0)
			{
				FMemory::Memmove(
					&Array[IndexToRemove - Index],
					&Array[IndexToRemove + 1],
					Count * sizeof(T));
			}
		}

		Array.SetNum(Array.Num() - SortedIndicesToRemove.Num(), false);
	}

	template<typename ArrayType, typename ElementType>
	FORCEINLINE bool IsInArrayAllocation(ArrayType& Array, const ElementType& Element)
	{
		const ElementType* Pointer = &Element;
		return
			Array.GetData() <= Pointer &&
			Pointer < Array.GetData() + Array.Num();
	}

	VOXELCORE_API bool AllEqual(uint8 Value, TConstArrayView<uint8> Data);
	VOXELCORE_API bool AllEqual(uint16 Value, TConstArrayView<uint16> Data);
	VOXELCORE_API bool AllEqual(uint32 Value, TConstArrayView<uint32> Data);
	VOXELCORE_API bool AllEqual(uint64 Value, TConstArrayView<uint64> Data);

	VOXELCORE_API float GetMin(TConstArrayView<float> Data);
	VOXELCORE_API float GetMax(TConstArrayView<float> Data);
	VOXELCORE_API FFloatInterval GetMinMax(TConstArrayView<float> Data);
	// Will return { MAX_flt, -MAX_flt } if no values are valid
	VOXELCORE_API FFloatInterval GetMinMaxSafe(TConstArrayView<float> Data);

	FORCEINLINE FLinearColor GetDistanceFieldColor(float Value)
	{
		// Credit for this snippet goes to Inigo Quilez

		FLinearColor Color = FLinearColor::White - FMath::Sign(Value) * FLinearColor(0.1f, 0.4f, 0.7f, 0.f);
		Color *= 1.0f - FMath::Exp(-3.f * FMath::Abs(Value));
		Color *= 0.8f + 0.2f * FMath::Cos(150.f * Value);
		Color = FMath::Lerp(Color, FLinearColor::White, 1.0 - FMath::SmoothStep(0.0f, 0.01f, FMath::Abs(Value)));
		Color.A = 1.f;
		return Color;
	}
}