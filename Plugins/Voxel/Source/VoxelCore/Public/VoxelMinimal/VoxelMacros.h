// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/ScopeExit.h"
#include "UObject/SoftObjectPtr.h"
#include "VoxelEngineVersionHelpers.h"

#ifndef INTELLISENSE_PARSER
#if defined(__INTELLISENSE__) || defined(__RSCPP_VERSION)
#define INTELLISENSE_PARSER 1
#else
#define INTELLISENSE_PARSER 0
#endif
#endif

#if INTELLISENSE_PARSER
#define VOXEL_DEBUG 1
#define UE_NODISCARD [[nodiscard]]
#define RHI_RAYTRACING 1
#define INTELLISENSE_ONLY(...) __VA_ARGS__
#define INTELLISENSE_SKIP(...)

// Needed for Resharper to detect the printf hidden in the lambda
#undef UE_LOG
#define UE_LOG(CategoryName, Verbosity, Format, ...) \
	{ \
		(void)ELogVerbosity::Verbosity; \
		(void)(FLogCategoryBase*)&CategoryName; \
		FString::Printf(Format, ##__VA_ARGS__); \
	}

// Syntax highlighting is wrong otherwise
#undef TEXTVIEW
#define TEXTVIEW(String) FStringView(TEXT(String))

#error "Compiler defined as parser"
#else
#define INTELLISENSE_ONLY(...)
#define INTELLISENSE_SKIP(...) __VA_ARGS__
#endif

// This is defined in the generated.h. It lets you use GetOuterASomeOuter. Resharper/intellisense are confused when it's used, so define it for them
#define INTELLISENSE_DECLARE_WITHIN(Name) INTELLISENSE_ONLY(DECLARE_WITHIN(Name))

#define INTELLISENSE_PRINTF(Format, ...) INTELLISENSE_ONLY((void)FString::Printf(TEXT(Format), __VA_ARGS__);)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern VOXELCORE_API bool GVoxelDisableSlowChecks;

#if VOXEL_DEBUG
#define checkVoxelSlow(x) check((x) || GVoxelDisableSlowChecks)
#define checkfVoxelSlow(x, ...) checkf((x) || GVoxelDisableSlowChecks, ##__VA_ARGS__)
#define ensureVoxelSlow(x) ensure((x) || GVoxelDisableSlowChecks)
#define ensureMsgfVoxelSlow(x, ...) ensureMsgf((x) || GVoxelDisableSlowChecks, ##__VA_ARGS__)
#define ensureVoxelSlowNoSideEffects(x) ensure((x) || GVoxelDisableSlowChecks)
#define ensureMsgfVoxelSlowNoSideEffects(x, ...) ensureMsgf((x) || GVoxelDisableSlowChecks, ##__VA_ARGS__)
#define VOXEL_ASSUME(...) check(__VA_ARGS__)
#undef FORCEINLINE
#define FORCEINLINE FORCEINLINE_DEBUGGABLE_ACTUAL
#if PLATFORM_WINDOWS
#define FORCEINLINE_ACTUAL __forceinline
#else
#define FORCEINLINE_ACTUAL
#endif
#define VOXEL_DEBUG_ONLY(...) __VA_ARGS__
#else
#define checkVoxelSlow(x)
#define checkfVoxelSlow(x, ...)
#define ensureVoxelSlow(x) (!!(x))
#define ensureMsgfVoxelSlow(x, ...) (!!(x))
#define ensureVoxelSlowNoSideEffects(x)
#define ensureMsgfVoxelSlowNoSideEffects(...)
#define VOXEL_ASSUME(...) UE_ASSUME(__VA_ARGS__)
#define VOXEL_DEBUG_ONLY(...)
#define FORCEINLINE_ACTUAL FORCEINLINE
#endif

#if VOXEL_DEBUG
#define ensureThreadSafe(...) ensure(__VA_ARGS__)
#else
#define ensureThreadSafe(...)
#endif

#define checkStatic(...) static_assert(__VA_ARGS__, "Static assert failed")
#define checkfStatic(Expr, Error) static_assert(Expr, Error)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VOXELCORE_API uint64 VoxelHashString(const FStringView& Name);

#if VOXEL_DEBUG
#define VOXEL_STATIC_HELPER_CHECK() ON_SCOPE_EXIT { ensure(StaticRawValue != 0); }
#else
#define VOXEL_STATIC_HELPER_CHECK()
#endif

// Zero initialize static and check if it's 0
// That way the static TLS logic is optimized out and this is compiled to a single mov + test
// This works only if the initializer is deterministic & can be called multiple times
#define VOXEL_STATIC_HELPER(InType) \
	static TVoxelTypeForBytes<sizeof(InType)>::Type StaticRawValue = 0; \
	VOXEL_STATIC_HELPER_CHECK(); \
	InType& StaticValue = ReinterpretCastRef<InType>(StaticRawValue); \
	if (!StaticRawValue)

template<typename T>
inline static FName GVoxelStaticName{ static_cast<T*>(nullptr)->operator()() };

template<typename T>
FORCEINLINE const FName& VoxelStaticName(const T&)
{
    return GVoxelStaticName<T>;
}

#define STATIC_FNAME(Name) VoxelStaticName([]{ return FName(Name); })

#define STATIC_FSTRING(String) \
	([]() -> const FString& \
	{ \
		static const FString StaticString = String; \
		return StaticString; \
	}())

#define STATIC_HASH(Name) \
	([]() -> uint64 \
	{ \
		VOXEL_STATIC_HELPER(uint64) \
		{ \
			StaticValue = VoxelHashString(TEXT(Name)); \
		} \
		return StaticValue; \
	}())

#define GET_MEMBER_NAME_STATIC(ClassName, MemberName) STATIC_FNAME(GET_MEMBER_NAME_STRING_CHECKED(ClassName, MemberName))
#define GET_OWN_MEMBER_NAME(MemberName) GET_MEMBER_NAME_CHECKED(TDecay<decltype(*this)>::Type, MemberName)

#define VOXEL_GET_TYPE(Value) typename TDecay<decltype(Value)>::Type
#define VOXEL_THIS_TYPE VOXEL_GET_TYPE(*this)
// This is needed in classes, where just doing class Name would fwd declare it in the class scope
#define VOXEL_FWD_DECLARE_CLASS(Name) void PREPROCESSOR_JOIN(__VoxelDeclareDummy_, __LINE__)(class Name*);

// This makes the macro parameter show up as a class in Resharper
#if INTELLISENSE_PARSER
#define VOXEL_FWD_DECLARE_CLASS_INTELLISENSE(Name) VOXEL_FWD_DECLARE_CLASS(Name)
#else
#define VOXEL_FWD_DECLARE_CLASS_INTELLISENSE(Name)
#endif

#define VOXEL_CONSOLE_VARIABLE(Api, Type, Name, Default, Command, Description) \
	Api Type Name = Default; \
	static FAutoConsoleVariableRef CVar_ ## Name( \
		TEXT(Command),  \
		Name,  \
		TEXT(Description))

#define VOXEL_CONSOLE_COMMAND(Name, Command, Description) \
	INTELLISENSE_ONLY(void Name()); \
	static void Cmd ## Name(); \
	static FAutoConsoleCommand AutoCmd ## Name( \
	    TEXT(Command), \
	    TEXT(Description), \
		MakeLambdaDelegate([] \
		{ \
			VOXEL_SCOPE_COUNTER(#Name); \
			Cmd ## Name(); \
		})); \
	\
	static void Cmd ## Name()

#define VOXEL_CONSOLE_WORLD_COMMAND(Name, Command, Description) \
	INTELLISENSE_ONLY(void Name()); \
	static void Cmd ## Name(const TArray<FString>&, UWorld*); \
	static FAutoConsoleCommand AutoCmd ## Name( \
	    TEXT(Command), \
	    TEXT(Description), \
		MakeLambdaDelegate([](const TArray<FString>& Args, UWorld* World, FOutputDevice&) \
		{ \
			VOXEL_SCOPE_COUNTER(#Name); \
			Cmd ## Name(Args, World); \
		})); \
	\
	static void Cmd ## Name(const TArray<FString>& Args, UWorld* World)

struct VOXELCORE_API FVoxelConsoleSinkHelper
{
	explicit FVoxelConsoleSinkHelper(TFunction<void()> Lambda);
};

#define VOXEL_CONSOLE_SINK(UniqueId) \
	INTELLISENSE_ONLY(void UniqueId();) \
	void VOXEL_APPEND_LINE(PREPROCESSOR_JOIN(VoxelConsoleSink, UniqueId))(); \
	static const FVoxelConsoleSinkHelper PREPROCESSOR_JOIN(VoxelConsoleSinkHelper, UniqueId)([] \
	{ \
		VOXEL_SCOPE_COUNTER(#UniqueId); \
		VOXEL_APPEND_LINE(PREPROCESSOR_JOIN(VoxelConsoleSink, UniqueId))(); \
	}); \
	void VOXEL_APPEND_LINE(PREPROCESSOR_JOIN(VoxelConsoleSink, UniqueId))()

#define VOXEL_EXPAND(X) X

#define VOXEL_APPEND_LINE(X) PREPROCESSOR_JOIN(X, __LINE__)

#define ON_SCOPE_EXIT_IMPL(Suffix) const auto PREPROCESSOR_JOIN(PREPROCESSOR_JOIN(ScopeGuard_, __LINE__), Suffix) = ::ScopeExitSupport::FScopeGuardSyntaxSupport() + [&]()

// Unlike GENERATE_MEMBER_FUNCTION_CHECK, this supports inheritance
// However, it doesn't do any signature check
#define VOXEL_GENERATE_MEMBER_FUNCTION_CHECK(MemberName)		            \
template <typename T>														\
class THasMemberFunction_##MemberName										\
{																			\
	template <typename U> static char MemberTest(decltype(&U::MemberName));	\
	template <typename U> static int32 MemberTest(...);						\
public:																		\
	enum { Value = sizeof(MemberTest<T>(nullptr)) == sizeof(char) };		\
};

#define VOXEL_FOLD_EXPRESSION(...) \
	{ \
		int32 Temp[] = { 0, ((__VA_ARGS__), 0)... }; \
		(void)Temp; \
	}

#if !IS_MONOLITHIC
#define VOXEL_ISPC_ASSERT() \
	extern "C" void VoxelISPC_Assert(const int32 Line) \
	{ \
		ensureAlwaysMsgf(false, TEXT("ISPC LINE: %d"), Line); \
	} \
	extern "C" void VoxelISPC_UnsupportedTargetWidth(const int32 Width) \
	{ \
		LOG_VOXEL(Fatal, "Unsupported ISPC target width: %d", Width); \
	}
#else
#define VOXEL_ISPC_ASSERT()
#endif

#define VOXEL_DEFAULT_MODULE(Name) \
	IMPLEMENT_MODULE(FDefaultModuleImpl, Name) \
	VOXEL_ISPC_ASSERT()

#define VOXEL_PURE_VIRTUAL(...) { ensureMsgf(false, TEXT("Pure virtual %s called"), *FString(__FUNCTION__)); return __VA_ARGS__; }

#define VOXEL_USE_VARIABLE(Name) ensure(&Name != reinterpret_cast<void*>(0x1234))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern VOXELCORE_API bool GIsVoxelCoreModuleLoaded;
extern VOXELCORE_API FSimpleMulticastDelegate GOnVoxelModuleUnloaded_DoCleanup;
extern VOXELCORE_API FSimpleMulticastDelegate GOnVoxelModuleUnloaded;

enum class EVoxelRunOnStartupPhase
{
	Game,
	Editor,
	EditorCook,
	FirstTick
};
struct VOXELCORE_API FVoxelRunOnStartupPhaseHelper
{
	FVoxelRunOnStartupPhaseHelper(EVoxelRunOnStartupPhase Phase, int32 Priority, TFunction<void()> Lambda);
};

#define VOXEL_RUN_ON_STARTUP(UniqueId, Phase, Priority) \
	INTELLISENSE_ONLY(void UniqueId();) \
	void VOXEL_APPEND_LINE(PREPROCESSOR_JOIN(VoxelStartupFunction, UniqueId))(); \
	static const FVoxelRunOnStartupPhaseHelper PREPROCESSOR_JOIN(VoxelRunOnStartupPhaseHelper, UniqueId)(EVoxelRunOnStartupPhase::Phase, Priority, [] \
	{ \
		VOXEL_APPEND_LINE(PREPROCESSOR_JOIN(VoxelStartupFunction, UniqueId))(); \
	}); \
	void VOXEL_APPEND_LINE(PREPROCESSOR_JOIN(VoxelStartupFunction, UniqueId))()

#define VOXEL_RUN_ON_STARTUP_GAME(UniqueId) VOXEL_RUN_ON_STARTUP(UniqueId, Game, 0)
// Will only run if GIsEditor is true
#define VOXEL_RUN_ON_STARTUP_EDITOR(UniqueId) VOXEL_RUN_ON_STARTUP(UniqueId, Editor, 0)
// Will run before any package PostLoad to work for cooker
#define VOXEL_RUN_ON_STARTUP_EDITOR_COOK(UniqueId) VOXEL_RUN_ON_STARTUP(UniqueId, EditorCook, 0)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace VoxelInternal
{
	struct FVoxelOnConstruct
	{
		template<typename FuncType>
		FVoxelOnConstruct operator+(FuncType&& InFunc)
		{
			InFunc();
			return {};
		}
	};
}

#define VOXEL_ON_CONSTRUCT() VoxelInternal::FVoxelOnConstruct VOXEL_APPEND_LINE(__OnConstruct) = VoxelInternal::FVoxelOnConstruct() + [this]

#define VOXEL_SLATE_ARGS() \
	struct FArguments; \
	using WidgetArgsType = FArguments; \
	struct FDummyArguments \
	{ \
		using FArguments = FArguments; \
	}; \
	\
	struct FArguments : public TSlateBaseNamedArgs<FDummyArguments>

// Useful for templates
#define VOXEL_WRAP(...) __VA_ARGS__

#define UE_NONCOPYABLE_MOVEABLE(TypeName) \
	TypeName(TypeName&&) = default; \
	TypeName& operator=(TypeName&&) = default; \
	TypeName(const TypeName&) = delete; \
	TypeName& operator=(const TypeName&) = delete;

#define UE_NONCOPYABLE_STRUCT(TypeName) \
	TypeName(TypeName&&) = default; \
	TypeName& operator=(TypeName&&) = default; \
	TypeName(const TypeName&) \
	{ \
		ensureVoxelSlow(false); \
	} \
	TypeName& operator=(const TypeName&) \
	{ \
		ensureVoxelSlow(false); \
		return *this; \
	}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelLambdaCaller
{
	template<typename T>
	FORCEINLINE auto operator+(T&& Lambda) -> decltype(auto)
	{
		return Lambda();
	}
};

#define INLINE_LAMBDA FVoxelLambdaCaller() + [&]()

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// UniqueClass: to forbid copying ids from different classes
template<typename UniqueClass>
class TVoxelUniqueId
{
public:
	TVoxelUniqueId() = default;

	FORCEINLINE bool IsValid() const { return Id != 0; }

	FORCEINLINE bool operator==(const TVoxelUniqueId& Other) const { return Id == Other.Id; }
	FORCEINLINE bool operator!=(const TVoxelUniqueId& Other) const { return Id != Other.Id; }

	FORCEINLINE bool operator<(const TVoxelUniqueId& Other) const { return Id < Other.Id; }
	FORCEINLINE bool operator>(const TVoxelUniqueId& Other) const { return Id > Other.Id; }

	FORCEINLINE bool operator<=(const TVoxelUniqueId& Other) const { return Id <= Other.Id; }
	FORCEINLINE bool operator>=(const TVoxelUniqueId& Other) const { return Id >= Other.Id; }

	FORCEINLINE friend uint32 GetTypeHash(TVoxelUniqueId UniqueId)
	{
	    return uint32(UniqueId.Id);
	}

	FORCEINLINE uint64 GetId() const
	{
		return Id;
	}

	FORCEINLINE static TVoxelUniqueId New()
	{
		return TVoxelUniqueId(TVoxelUniqueId_MakeNew(static_cast<UniqueClass*>(nullptr)));
	}

private:
	FORCEINLINE TVoxelUniqueId(uint64 Id)
		: Id(Id)
	{
		ensureVoxelSlow(IsValid());
	}

	uint64 Id = 0;
};

#define DECLARE_UNIQUE_VOXEL_ID_EXPORT(Api, Name) \
	Api uint64 TVoxelUniqueId_MakeNew(class __ ## Name ##_Unique*); \
	using Name = TVoxelUniqueId<class __ ## Name ##_Unique>;

#define DECLARE_UNIQUE_VOXEL_ID(Name) DECLARE_UNIQUE_VOXEL_ID_EXPORT(,Name)

#define DEFINE_UNIQUE_VOXEL_ID(Name) \
	INTELLISENSE_ONLY(void VOXEL_APPEND_LINE(Dummy)(Name);) \
	uint64 TVoxelUniqueId_MakeNew(class __ ## Name ##_Unique*) \
	{ \
		static FThreadSafeCounter64 Counter; \
		return Counter.Increment(); \
	}

template<typename T>
class TVoxelIndex
{
public:
	TVoxelIndex() = default;

	FORCEINLINE bool IsValid() const
	{
		return Index != -1;
	}
	FORCEINLINE operator bool() const
	{
		return IsValid();
	}

	FORCEINLINE bool operator==(const TVoxelIndex& Other) const
	{
		return Index == Other.Index;
	}
	FORCEINLINE bool operator!=(const TVoxelIndex& Other) const
	{
		return Index != Other.Index;
	}

	FORCEINLINE friend uint32 GetTypeHash(TVoxelIndex InIndex)
	{
	    return InIndex.Index;
	}

protected:
	int32 Index = -1;

	FORCEINLINE TVoxelIndex(int32 Index)
		: Index(Index)
	{
	}

	FORCEINLINE operator int32() const
	{
		return Index;
	}

	friend T;
};
 static_assert(sizeof(TVoxelIndex<class FVoxelIndexDummy>) == sizeof(int32), "");

#define DECLARE_VOXEL_INDEX(Name, FriendClass) using Name = TVoxelIndex<class FriendClass>;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename ToType, typename FromType, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE ToType* ReinterpretCastPtr(FromType* From)
{
	return reinterpret_cast<ToType*>(From);
}
template<typename ToType, typename FromType, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE const ToType* ReinterpretCastPtr(const FromType* From)
{
	return reinterpret_cast<const ToType*>(From);
}

template<typename ToType, typename FromType, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE ToType& ReinterpretCastRef(FromType& From)
{
	return reinterpret_cast<ToType&>(From);
}
template<typename ToType, typename FromType, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE const ToType& ReinterpretCastRef(const FromType& From)
{
	return reinterpret_cast<const ToType&>(From);
}

template<typename ToType, typename FromType, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType) && !TIsReferenceType<FromType>::Value>::Type>
FORCEINLINE ToType&& ReinterpretCastRef(FromType&& From)
{
	return reinterpret_cast<ToType&&>(From);
}

template<typename ToType, typename FromType, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType) && std::is_const_v<FromType> == std::is_const_v<ToType>>::Type>
FORCEINLINE TSharedPtr<ToType>& ReinterpretCastSharedPtr(TSharedPtr<FromType>& From)
{
	return ReinterpretCastRef<TSharedPtr<ToType>>(From);
}
template<typename ToType, typename FromType, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType) && std::is_const_v<FromType> == std::is_const_v<ToType>>::Type>
FORCEINLINE TSharedRef<ToType>& ReinterpretCastSharedPtr(TSharedRef<FromType>& From)
{
	return ReinterpretCastRef<TSharedRef<ToType>>(From);
}

template<typename ToType, typename FromType, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType) && std::is_const_v<FromType> == std::is_const_v<ToType>>::Type>
FORCEINLINE const TSharedPtr<ToType>& ReinterpretCastSharedPtr(const TSharedPtr<FromType>& From)
{
	return ReinterpretCastRef<TSharedPtr<ToType>>(From);
}
template<typename ToType, typename FromType, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType) && std::is_const_v<FromType> == std::is_const_v<ToType>>::Type>
FORCEINLINE const TSharedRef<ToType>& ReinterpretCastSharedPtr(const TSharedRef<FromType>& From)
{
	return ReinterpretCastRef<TSharedRef<ToType>>(From);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE TArray<ToType, Allocator>& ReinterpretCastArray(TArray<FromType, Allocator>& Array)
{
	return reinterpret_cast<TArray<ToType, Allocator>&>(Array);
}
template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE const TArray<ToType, Allocator>& ReinterpretCastArray(const TArray<FromType, Allocator>& Array)
{
	return reinterpret_cast<const TArray<ToType, Allocator>&>(Array);
}

template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE TArray<ToType, Allocator>&& ReinterpretCastArray(TArray<FromType, Allocator>&& Array)
{
	return reinterpret_cast<TArray<ToType, Allocator>&&>(Array);
}
template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE const TArray<ToType, Allocator>&& ReinterpretCastArray(const TArray<FromType, Allocator>&& Array)
{
	return reinterpret_cast<const TArray<ToType, Allocator>&&>(Array);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename ToType, typename ToAllocator, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) != sizeof(ToType)>::Type>
FORCEINLINE TArray<ToType, ToAllocator> ReinterpretCastArray_Copy(const TArray<FromType, Allocator>& Array)
{
	const int64 NumBytes = Array.Num() * sizeof(FromType);
	check(NumBytes % sizeof(ToType) == 0);
	return TArray<ToType, Allocator>(reinterpret_cast<const ToType*>(Array.GetData()), NumBytes / sizeof(ToType));
}
template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) != sizeof(ToType)>::Type>
FORCEINLINE TArray<ToType, Allocator> ReinterpretCastArray_Copy(const TArray<FromType, Allocator>& Array)
{
	return ReinterpretCastArray_Copy<ToType, Allocator>(Array);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE TSet<ToType, DefaultKeyFuncs<ToType>, Allocator>& ReinterpretCastSet(TSet<FromType, DefaultKeyFuncs<FromType>, Allocator>& Set)
{
	return reinterpret_cast<TSet<ToType, DefaultKeyFuncs<ToType>, Allocator>&>(Set);
}
template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE const TSet<ToType, DefaultKeyFuncs<ToType>, Allocator>& ReinterpretCastSet(const TSet<FromType, DefaultKeyFuncs<FromType>, Allocator>& Set)
{
	return reinterpret_cast<const TSet<ToType, DefaultKeyFuncs<ToType>, Allocator>&>(Set);
}

template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE TSet<ToType, DefaultKeyFuncs<ToType>, Allocator>&& ReinterpretCastSet(TSet<FromType, DefaultKeyFuncs<FromType>, Allocator>&& Set)
{
	return reinterpret_cast<TSet<ToType, DefaultKeyFuncs<ToType>, Allocator>&&>(Set);
}
template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) == sizeof(ToType)>::Type>
FORCEINLINE const TSet<ToType, DefaultKeyFuncs<ToType>, Allocator>&& ReinterpretCastSet(const TSet<FromType, DefaultKeyFuncs<FromType>, Allocator>&& Set)
{
	return reinterpret_cast<const TSet<ToType, DefaultKeyFuncs<ToType>, Allocator>&&>(Set);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T, typename = void>
struct TVoxelConstCast
{
	FORCEINLINE static T& ConstCast(T& Value)
	{
		return Value;
	}
};

template<typename T>
struct TVoxelConstCast<const T*>
{
	FORCEINLINE static T*& ConstCast(const T*& Value)
	{
		return const_cast<T*&>(Value);
	}
};

template<typename T>
struct TVoxelConstCast<const T* const>
{
	FORCEINLINE static T*& ConstCast(const T* const& Value)
	{
		return const_cast<T*&>(Value);
	}
};

template<typename T>
struct TVoxelConstCast<const T>
{
	FORCEINLINE static T& ConstCast(const T& Value)
	{
		return const_cast<T&>(Value);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T>
struct TVoxelConstCast<TSharedPtr<const T>>
{
	FORCEINLINE static TSharedPtr<T>& ConstCast(TSharedPtr<const T>& Value)
	{
		return ReinterpretCastRef<TSharedPtr<T>>(Value);
	}
};
template<typename T>
struct TVoxelConstCast<TSharedRef<const T>>
{
	FORCEINLINE static TSharedRef<T>& ConstCast(TSharedRef<const T>& Value)
	{
		return ReinterpretCastRef<TSharedRef<T>>(Value);
	}
};

template<typename T>
struct TVoxelConstCast<const TSharedPtr<const T>>
{
	FORCEINLINE static const TSharedPtr<T>& ConstCast(const TSharedPtr<const T>& Value)
	{
		return ReinterpretCastRef<TSharedPtr<T>>(Value);
	}
};
template<typename T>
struct TVoxelConstCast<const TSharedRef<const T>>
{
	FORCEINLINE static const TSharedRef<T>& ConstCast(const TSharedRef<const T>& Value)
	{
		return ReinterpretCastRef<TSharedRef<T>>(Value);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T>
struct TVoxelConstCast<TObjectPtr<const T>>
{
	FORCEINLINE static TObjectPtr<T>& ConstCast(TObjectPtr<const T>& Value)
	{
		return ReinterpretCastRef<TObjectPtr<T>>(Value);
	}
};
template<typename T>
struct TVoxelConstCast<const TObjectPtr<const T>>
{
	FORCEINLINE static const TObjectPtr<T>& ConstCast(const TObjectPtr<const T>& Value)
	{
		return ReinterpretCastRef<TObjectPtr<T>>(Value);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T>
FORCEINLINE auto ConstCast(T& Value) -> decltype(auto)
{
	return TVoxelConstCast<T>::ConstCast(Value);
}
template<typename T>
FORCEINLINE auto ConstCast(T&& Value) -> decltype(auto)
{
	return TVoxelConstCast<T>::ConstCast(Value);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T>
FORCEINLINE TSoftObjectPtr<T> MakeSoftObjectPtr(const FString& Path)
{
	return TSoftObjectPtr<T>(FSoftObjectPath(Path));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define __VOXEL_GET_NTH_ARG(Dummy, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, N, ...) VOXEL_EXPAND(N)

#define __VOXEL_FOREACH_IMPL_XX(Prefix, Last, Suffix)
#define __VOXEL_FOREACH_IMPL_00(Prefix, Last, Suffix, X)      Last(X)
#define __VOXEL_FOREACH_IMPL_01(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_00(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_02(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_01(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_03(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_02(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_04(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_03(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_05(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_04(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_06(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_05(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_07(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_06(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_08(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_07(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_09(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_08(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_10(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_09(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_11(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_10(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_12(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_11(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_13(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_12(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_14(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_13(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_15(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_14(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_16(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_15(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_17(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_16(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_18(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_17(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_19(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_18(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_20(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_19(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_21(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_20(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_22(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_21(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_23(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_22(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_24(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_23(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_25(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_24(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_26(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_25(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_27(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_26(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_28(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_27(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_29(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_28(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_30(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_29(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_31(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_30(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)
#define __VOXEL_FOREACH_IMPL_32(Prefix, Last, Suffix, X, ...) Prefix(X) VOXEL_EXPAND(__VOXEL_FOREACH_IMPL_31(Prefix, Last, Suffix, __VA_ARGS__)) Suffix(X)

#define VOXEL_FOREACH_IMPL(Prefix, Last, /* Suffix (passed in VA_ARGS), */ ...) \
	VOXEL_EXPAND(__VOXEL_GET_NTH_ARG(__VA_ARGS__, \
	__VOXEL_FOREACH_IMPL_32, \
	__VOXEL_FOREACH_IMPL_31, \
	__VOXEL_FOREACH_IMPL_30, \
	__VOXEL_FOREACH_IMPL_29, \
	__VOXEL_FOREACH_IMPL_28, \
	__VOXEL_FOREACH_IMPL_27, \
	__VOXEL_FOREACH_IMPL_26, \
	__VOXEL_FOREACH_IMPL_25, \
	__VOXEL_FOREACH_IMPL_24, \
	__VOXEL_FOREACH_IMPL_23, \
	__VOXEL_FOREACH_IMPL_22, \
	__VOXEL_FOREACH_IMPL_21, \
	__VOXEL_FOREACH_IMPL_20, \
	__VOXEL_FOREACH_IMPL_19, \
	__VOXEL_FOREACH_IMPL_18, \
	__VOXEL_FOREACH_IMPL_17, \
	__VOXEL_FOREACH_IMPL_16, \
	__VOXEL_FOREACH_IMPL_15, \
	__VOXEL_FOREACH_IMPL_14, \
	__VOXEL_FOREACH_IMPL_13, \
	__VOXEL_FOREACH_IMPL_12, \
	__VOXEL_FOREACH_IMPL_11, \
	__VOXEL_FOREACH_IMPL_10, \
	__VOXEL_FOREACH_IMPL_09, \
	__VOXEL_FOREACH_IMPL_08, \
	__VOXEL_FOREACH_IMPL_07, \
	__VOXEL_FOREACH_IMPL_06, \
	__VOXEL_FOREACH_IMPL_05, \
	__VOXEL_FOREACH_IMPL_04, \
	__VOXEL_FOREACH_IMPL_03, \
	__VOXEL_FOREACH_IMPL_02, \
	__VOXEL_FOREACH_IMPL_01, \
	__VOXEL_FOREACH_IMPL_00, \
	__VOXEL_FOREACH_IMPL_XX)(Prefix, Last, __VA_ARGS__))

#define VOXEL_VOID_MACRO(...)

#define VOXEL_FOREACH(Macro, ...) VOXEL_FOREACH_IMPL(Macro, Macro, VOXEL_VOID_MACRO, ##__VA_ARGS__)
#define VOXEL_FOREACH_SUFFIX(Macro, ...) VOXEL_FOREACH_IMPL(VOXEL_VOID_MACRO, Macro, Macro, ##__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define __VOXEL_FOREACH_ONE_ARG_XX(Macro, Arg)
#define __VOXEL_FOREACH_ONE_ARG_00(Macro, Arg, X)      Macro(Arg, X)
#define __VOXEL_FOREACH_ONE_ARG_01(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_00(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_02(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_01(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_03(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_02(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_04(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_03(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_05(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_04(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_06(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_05(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_07(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_06(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_08(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_07(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_09(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_08(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_10(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_09(Macro, Arg, __VA_ARGS__))

#define __VOXEL_FOREACH_ONE_ARG_11(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_10(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_12(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_11(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_13(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_12(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_14(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_13(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_15(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_14(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_16(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_15(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_17(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_16(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_18(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_17(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_19(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_18(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_20(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_19(Macro, Arg, __VA_ARGS__))

#define __VOXEL_FOREACH_ONE_ARG_21(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_20(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_22(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_21(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_23(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_22(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_24(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_23(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_25(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_24(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_26(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_25(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_27(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_26(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_28(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_27(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_29(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_28(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_30(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_29(Macro, Arg, __VA_ARGS__))

#define __VOXEL_FOREACH_ONE_ARG_31(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_30(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_32(Macro, Arg, X, ...) Macro(Arg, X) VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_31(Macro, Arg, __VA_ARGS__))

#define VOXEL_FOREACH_ONE_ARG(Macro, /* Arg (passed in VA_ARGS), */ ...) \
	VOXEL_EXPAND(__VOXEL_GET_NTH_ARG(__VA_ARGS__, \
	__VOXEL_FOREACH_ONE_ARG_32, \
	__VOXEL_FOREACH_ONE_ARG_31, \
	__VOXEL_FOREACH_ONE_ARG_30, \
	__VOXEL_FOREACH_ONE_ARG_29, \
	__VOXEL_FOREACH_ONE_ARG_28, \
	__VOXEL_FOREACH_ONE_ARG_27, \
	__VOXEL_FOREACH_ONE_ARG_26, \
	__VOXEL_FOREACH_ONE_ARG_25, \
	__VOXEL_FOREACH_ONE_ARG_24, \
	__VOXEL_FOREACH_ONE_ARG_23, \
	__VOXEL_FOREACH_ONE_ARG_22, \
	__VOXEL_FOREACH_ONE_ARG_21, \
	__VOXEL_FOREACH_ONE_ARG_20, \
	__VOXEL_FOREACH_ONE_ARG_19, \
	__VOXEL_FOREACH_ONE_ARG_18, \
	__VOXEL_FOREACH_ONE_ARG_17, \
	__VOXEL_FOREACH_ONE_ARG_16, \
	__VOXEL_FOREACH_ONE_ARG_15, \
	__VOXEL_FOREACH_ONE_ARG_14, \
	__VOXEL_FOREACH_ONE_ARG_13, \
	__VOXEL_FOREACH_ONE_ARG_12, \
	__VOXEL_FOREACH_ONE_ARG_11, \
	__VOXEL_FOREACH_ONE_ARG_10, \
	__VOXEL_FOREACH_ONE_ARG_09, \
	__VOXEL_FOREACH_ONE_ARG_08, \
	__VOXEL_FOREACH_ONE_ARG_07, \
	__VOXEL_FOREACH_ONE_ARG_06, \
	__VOXEL_FOREACH_ONE_ARG_05, \
	__VOXEL_FOREACH_ONE_ARG_04, \
	__VOXEL_FOREACH_ONE_ARG_03, \
	__VOXEL_FOREACH_ONE_ARG_02, \
	__VOXEL_FOREACH_ONE_ARG_01, \
	__VOXEL_FOREACH_ONE_ARG_00, \
	__VOXEL_FOREACH_ONE_ARG_XX)(Macro, __VA_ARGS__))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define __VOXEL_FOREACH_COMMA_XX(Macro)
#define __VOXEL_FOREACH_COMMA_00(Macro, X)      Macro(X)
#define __VOXEL_FOREACH_COMMA_01(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_00(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_02(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_01(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_03(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_02(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_04(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_03(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_05(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_04(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_06(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_05(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_07(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_06(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_08(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_07(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_09(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_08(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_10(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_09(Macro, __VA_ARGS__))

#define __VOXEL_FOREACH_COMMA_11(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_10(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_12(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_11(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_13(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_12(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_14(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_13(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_15(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_14(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_16(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_15(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_17(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_16(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_18(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_17(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_19(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_18(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_20(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_19(Macro, __VA_ARGS__))

#define __VOXEL_FOREACH_COMMA_21(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_20(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_22(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_21(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_23(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_22(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_24(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_23(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_25(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_24(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_26(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_25(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_27(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_26(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_28(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_27(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_29(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_28(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_30(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_29(Macro, __VA_ARGS__))

#define __VOXEL_FOREACH_COMMA_31(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_30(Macro, __VA_ARGS__))
#define __VOXEL_FOREACH_COMMA_32(Macro, X, ...) Macro(X), VOXEL_EXPAND(__VOXEL_FOREACH_COMMA_31(Macro, __VA_ARGS__))

#define VOXEL_FOREACH_COMMA(/* Macro (passed in VA_ARGS), */ ...) \
	VOXEL_EXPAND(__VOXEL_GET_NTH_ARG(__VA_ARGS__, \
	__VOXEL_FOREACH_COMMA_32, \
	__VOXEL_FOREACH_COMMA_31, \
	__VOXEL_FOREACH_COMMA_30, \
	__VOXEL_FOREACH_COMMA_29, \
	__VOXEL_FOREACH_COMMA_28, \
	__VOXEL_FOREACH_COMMA_27, \
	__VOXEL_FOREACH_COMMA_26, \
	__VOXEL_FOREACH_COMMA_25, \
	__VOXEL_FOREACH_COMMA_24, \
	__VOXEL_FOREACH_COMMA_23, \
	__VOXEL_FOREACH_COMMA_22, \
	__VOXEL_FOREACH_COMMA_21, \
	__VOXEL_FOREACH_COMMA_20, \
	__VOXEL_FOREACH_COMMA_19, \
	__VOXEL_FOREACH_COMMA_18, \
	__VOXEL_FOREACH_COMMA_17, \
	__VOXEL_FOREACH_COMMA_16, \
	__VOXEL_FOREACH_COMMA_15, \
	__VOXEL_FOREACH_COMMA_14, \
	__VOXEL_FOREACH_COMMA_13, \
	__VOXEL_FOREACH_COMMA_12, \
	__VOXEL_FOREACH_COMMA_11, \
	__VOXEL_FOREACH_COMMA_10, \
	__VOXEL_FOREACH_COMMA_09, \
	__VOXEL_FOREACH_COMMA_08, \
	__VOXEL_FOREACH_COMMA_07, \
	__VOXEL_FOREACH_COMMA_06, \
	__VOXEL_FOREACH_COMMA_05, \
	__VOXEL_FOREACH_COMMA_04, \
	__VOXEL_FOREACH_COMMA_03, \
	__VOXEL_FOREACH_COMMA_02, \
	__VOXEL_FOREACH_COMMA_01, \
	__VOXEL_FOREACH_COMMA_00, \
	__VOXEL_FOREACH_COMMA_XX)(__VA_ARGS__))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define __VOXEL_FOREACH_ONE_ARG_COMMA_XX(Macro, Arg)
#define __VOXEL_FOREACH_ONE_ARG_COMMA_00(Macro, Arg, X)      Macro(Arg, X)
#define __VOXEL_FOREACH_ONE_ARG_COMMA_01(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_00(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_02(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_01(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_03(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_02(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_04(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_03(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_05(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_04(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_06(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_05(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_07(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_06(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_08(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_07(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_09(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_08(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_10(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_09(Macro, Arg, __VA_ARGS__))

#define __VOXEL_FOREACH_ONE_ARG_COMMA_11(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_10(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_12(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_11(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_13(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_12(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_14(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_13(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_15(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_14(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_16(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_15(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_17(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_16(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_18(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_17(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_19(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_18(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_20(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_19(Macro, Arg, __VA_ARGS__))

#define __VOXEL_FOREACH_ONE_ARG_COMMA_21(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_20(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_22(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_21(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_23(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_22(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_24(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_23(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_25(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_24(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_26(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_25(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_27(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_26(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_28(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_27(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_29(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_28(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_30(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_29(Macro, Arg, __VA_ARGS__))

#define __VOXEL_FOREACH_ONE_ARG_COMMA_31(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_30(Macro, Arg, __VA_ARGS__))
#define __VOXEL_FOREACH_ONE_ARG_COMMA_32(Macro, Arg, X, ...) Macro(Arg, X), VOXEL_EXPAND(__VOXEL_FOREACH_ONE_ARG_COMMA_31(Macro, Arg, __VA_ARGS__))

#define VOXEL_FOREACH_ONE_ARG_COMMA(Macro, /* Arg (passed in VA_ARGS), */ ...) \
	VOXEL_EXPAND(__VOXEL_GET_NTH_ARG(__VA_ARGS__, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_32, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_31, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_30, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_29, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_28, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_27, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_26, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_25, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_24, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_23, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_22, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_21, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_20, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_19, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_18, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_17, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_16, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_15, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_14, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_13, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_12, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_11, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_10, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_09, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_08, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_07, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_06, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_05, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_04, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_03, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_02, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_01, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_00, \
	__VOXEL_FOREACH_ONE_ARG_COMMA_XX)(Macro, __VA_ARGS__))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define DECLARE_VOXEL_VERSION(...) \
	struct \
	{ \
		enum Type : int32 \
		{ \
			__VA_ARGS__, \
			__VersionPlusOne, \
			LatestVersion = __VersionPlusOne - 1 \
		}; \
	}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T>
struct TVoxelUFunctionOverride
{
	struct FFrame : T
	{
		struct FCode
		{
			FORCEINLINE FCode operator+=(FCode) const { return {}; }
			FORCEINLINE FCode operator!() const { return {}; }
		};
		FCode Code;
	};

	using FNativeFuncPtr = void (*)(UObject* Context, FFrame& Stack, void* Result);

	struct FNameNativePtrPair
	{
		const char* NameUTF8;
		FNativeFuncPtr Pointer;
	};

	struct FNativeFunctionRegistrar
	{
		FNativeFunctionRegistrar(UClass* Class, const ANSICHAR* InName, FNativeFuncPtr InPointer)
		{
			RegisterFunction(Class, InName, InPointer);
		}
		static void RegisterFunction(UClass* Class, const ANSICHAR* InName, FNativeFuncPtr InPointer)
		{
			::FNativeFunctionRegistrar::RegisterFunction(Class, InName, reinterpret_cast<::FNativeFuncPtr>(InPointer));
		}
		static void RegisterFunction(UClass* Class, const WIDECHAR* InName, FNativeFuncPtr InPointer)
		{
			::FNativeFunctionRegistrar::RegisterFunction(Class, InName, reinterpret_cast<::FNativeFuncPtr>(InPointer));
		}
		static void RegisterFunctions(UClass* Class, const FNameNativePtrPair* InArray, int32 NumFunctions)
		{
			::FNativeFunctionRegistrar::RegisterFunctions(Class, reinterpret_cast<const ::FNameNativePtrPair*>(InArray), NumFunctions);
		}
	};
};

#define VOXEL_UFUNCTION_OVERRIDE(NewFrameClass) \
	using FFrame = TVoxelUFunctionOverride<NewFrameClass>::FFrame; \
	using FNativeFuncPtr = TVoxelUFunctionOverride<NewFrameClass>::FNativeFuncPtr; \
	using FNameNativePtrPair = TVoxelUFunctionOverride<NewFrameClass>::FNameNativePtrPair; \
	using FNativeFunctionRegistrar = TVoxelUFunctionOverride<NewFrameClass>::FNativeFunctionRegistrar;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef VOXEL_ALLOC_DEBUG
#define VOXEL_ALLOC_DEBUG VOXEL_DEBUG
#endif

#if VOXEL_ALLOC_DEBUG
extern VOXELCORE_API bool GVoxelMallocDisableChecks;

VOXELCORE_API bool EnterVoxelAllowMallocScope();
VOXELCORE_API void ExitVoxelAllowMallocScope(bool bBackup);

VOXELCORE_API bool EnterVoxelAllowReallocScope();
VOXELCORE_API void ExitVoxelAllowReallocScope(bool bBackup);

#define VOXEL_ALLOW_MALLOC_SCOPE() \
	bool VOXEL_APPEND_LINE(_VoxelMallocIsAllowedBackup); \
	if (!GVoxelMallocDisableChecks) \
	{ \
		VOXEL_APPEND_LINE(_VoxelMallocIsAllowedBackup) = EnterVoxelAllowMallocScope(); \
	} \
	ON_SCOPE_EXIT \
	{ \
		if (!GVoxelMallocDisableChecks) \
		{ \
			ExitVoxelAllowMallocScope(VOXEL_APPEND_LINE(_VoxelMallocIsAllowedBackup)); \
		} \
	}

#define VOXEL_ALLOW_REALLOC_SCOPE() \
	bool VOXEL_APPEND_LINE(_VoxelReallocIsAllowedBackup); \
	if (!GVoxelMallocDisableChecks) \
	{ \
		VOXEL_APPEND_LINE(_VoxelReallocIsAllowedBackup) = EnterVoxelAllowReallocScope(); \
	} \
	ON_SCOPE_EXIT \
	{ \
		if (!GVoxelMallocDisableChecks) \
		{ \
			ExitVoxelAllowReallocScope(VOXEL_APPEND_LINE(_VoxelReallocIsAllowedBackup)); \
		} \
	}

#define VOXEL_ALLOW_MALLOC_INLINE(...) \
	([&]() -> decltype(auto) { VOXEL_ALLOW_MALLOC_SCOPE(); return __VA_ARGS__; }())

#else
#define VOXEL_ALLOW_MALLOC_SCOPE()
#define VOXEL_ALLOW_REALLOC_SCOPE()
#define VOXEL_ALLOW_MALLOC_INLINE(...) (__VA_ARGS__)
#endif

#if VOXEL_DEBUG
VOXELCORE_API void EnterVoxelAllowLeakScope();
VOXELCORE_API void ExitVoxelAllowLeakScope();

#define VOXEL_ALLOW_LEAK_SCOPE() \
	EnterVoxelAllowLeakScope(); \
	ON_SCOPE_EXIT \
	{ \
		ExitVoxelAllowLeakScope(); \
	}
#else
#define VOXEL_ALLOW_LEAK_SCOPE()
#endif