// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "Containers/VoxelArray.h"

class UEdGraph;
class UEdGraphPin;
class UEdGraphNode;
class IMessageToken;
class FTokenizedMessage;

enum class EVoxelMessageSeverity
{
	Info,
	Warning,
	Error
};

class IVoxelMessageConsumer
{
public:
	virtual ~IVoxelMessageConsumer() = default;

	virtual void LogMessage(const TSharedRef<FTokenizedMessage>& Message) = 0;
};

class FVoxelMessageSinkConsumer : public IVoxelMessageConsumer
{
public:
	virtual void LogMessage(const TSharedRef<FTokenizedMessage>& Message) override
	{
	}
};

class VOXELCORE_API FVoxelScopedMessageConsumer
{
public:
	explicit FVoxelScopedMessageConsumer(TWeakPtr<IVoxelMessageConsumer> MessageConsumer);
	explicit FVoxelScopedMessageConsumer(TFunction<void(const TSharedRef<FTokenizedMessage>&)> LogMessage);
	~FVoxelScopedMessageConsumer();

private:
	TSharedPtr<IVoxelMessageConsumer> TempConsumer;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_UNIQUE_VOXEL_ID(FVoxelMessageBuilderId);

class VOXELCORE_API FVoxelMessageBuilder
{
public:
	using FArg = TFunction<void(FTokenizedMessage& Message, TSet<const UEdGraph*>& OutGraphs)>;

	const FVoxelMessageBuilderId BuilderId = FVoxelMessageBuilderId::New();
	const EVoxelMessageSeverity Severity;
	const FString Format;
	const TSharedPtr<IVoxelMessageConsumer> MessageConsumer;

	explicit FVoxelMessageBuilder(EVoxelMessageSeverity Severity, const FString& Format);

	void AddArg(FArg Arg)
	{
		Args.Add(Arg);
	}

	void Silence()
	{
		bIsSilenced = true;
	}
	bool IsSilenced() const
	{
		return bIsSilenced;
	}

	void AppendTo(FTokenizedMessage& Message, TSet<const UEdGraph*>& OutGraphs) const;
	TSharedRef<FTokenizedMessage> BuildMessage(TSet<const UEdGraph*>& OutGraphs) const;

private:
	TVoxelArray<FArg> Args;
	TVoxelArray<TSharedPtr<FVoxelMessageBuilder>> Callstacks;
	bool bIsSilenced = false;

	friend struct FVoxelMessages;
};

template<typename T, typename = void>
struct TVoxelMessageArgProcessor;

template<typename T>
using TVoxelMessageArgProcessorType = TVoxelMessageArgProcessor<std::remove_const_t<typename TRemovePointer<typename TDecay<T>::Type>::Type>>;

struct VOXELCORE_API FVoxelMessageArgProcessor
{
	template<typename T>
	static void ProcessArg(FVoxelMessageBuilder& Builder, const T& Value)
	{
		TVoxelMessageArgProcessorType<T>::ProcessArg(Builder, Value);
	}

	static void ProcessArgs(FVoxelMessageBuilder& Builder) {}

	template<typename ArgType, typename... ArgTypes>
	static void ProcessArgs(FVoxelMessageBuilder& Builder, ArgType&& Arg, ArgTypes&&... Args)
	{
		FVoxelMessageArgProcessor::ProcessArg(Builder, Arg);
		FVoxelMessageArgProcessor::ProcessArgs(Builder, Forward<ArgTypes>(Args)...);
	}

	static void ProcessTextArg(FVoxelMessageBuilder& Builder, const FText& Text);
	static void ProcessPinArg(FVoxelMessageBuilder& Builder, const UEdGraphPin* Pin);
	static void ProcessObjectArg(FVoxelMessageBuilder& Builder, TWeakObjectPtr<const UObject> Object);
	static void ProcessTokenArg(FVoxelMessageBuilder& Builder, const TSharedRef<IMessageToken>& Token);
	static void ProcessChildArg(FVoxelMessageBuilder& Builder, const TSharedRef<FVoxelMessageBuilder>& ChildBuilder);
};

template<>
struct TVoxelMessageArgProcessor<TSharedRef<IMessageToken>>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TSharedRef<IMessageToken>& Token)
	{
		FVoxelMessageArgProcessor::ProcessTokenArg(Builder, Token);
	}
};

template<>
struct TVoxelMessageArgProcessor<TSharedRef<FVoxelMessageBuilder>>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TSharedRef<FVoxelMessageBuilder>& ChildBuilder)
	{
		FVoxelMessageArgProcessor::ProcessChildArg(Builder, ChildBuilder);
	}
};

template<>
struct TVoxelMessageArgProcessor<FText>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FText& Text)
	{
		FVoxelMessageArgProcessor::ProcessTextArg(Builder, Text);
	}
};
template<>
struct TVoxelMessageArgProcessor<char>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const char* Text)
	{
		FVoxelMessageArgProcessor::ProcessTextArg(Builder, FText::FromString(Text));
	}
};
template<>
struct TVoxelMessageArgProcessor<TCHAR>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TCHAR* Text)
	{
		FVoxelMessageArgProcessor::ProcessTextArg(Builder, FText::FromString(Text));
	}
};
template<>
struct TVoxelMessageArgProcessor<FName>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, FName Text)
	{
		FVoxelMessageArgProcessor::ProcessTextArg(Builder, FText::FromName(Text));
	}
};
template<>
struct TVoxelMessageArgProcessor<FString>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FString& Text)
	{
		FVoxelMessageArgProcessor::ProcessTextArg(Builder, FText::FromString(Text));
	}
};

#define PROCESS_NUMBER(Type) \
	template<> \
	struct TVoxelMessageArgProcessor<Type> \
	{ \
		static void ProcessArg(FVoxelMessageBuilder& Builder, Type Value) \
		{ \
			FVoxelMessageArgProcessor::ProcessTextArg(Builder, FText::AsNumber(Value, &FNumberFormattingOptions::DefaultNoGrouping())); \
		} \
	};

	PROCESS_NUMBER(int8);
	PROCESS_NUMBER(int16);
	PROCESS_NUMBER(int32);
	PROCESS_NUMBER(int64);

	PROCESS_NUMBER(uint8);
	PROCESS_NUMBER(uint16);
	PROCESS_NUMBER(uint32);
	PROCESS_NUMBER(uint64);

	PROCESS_NUMBER(float);
	PROCESS_NUMBER(double);

#undef PROCESS_NUMBER

template<typename T>
struct TVoxelMessageArgProcessor<TWeakObjectPtr<T>>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TWeakObjectPtr<const T> Object)
	{
		// Don't require including T
		FVoxelMessageArgProcessor::ProcessObjectArg(Builder, ReinterpretCastRef<TWeakObjectPtr<UObject>>(Object));
	}
};

template<typename T>
struct TVoxelMessageArgProcessor<TWeakInterfacePtr<T>>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TWeakInterfacePtr<T> Interface)
	{
		FVoxelMessageArgProcessor::ProcessObjectArg(Builder, Interface.GetWeakObjectPtr());
	}
};

struct CVoxelMessageProcessorHasGetMessageArg
{
	template<typename T>
	auto Requires() -> decltype(DeclVal<T>().GetMessageArg());
};

template<typename T>
struct TVoxelMessageArgProcessor<T, typename TEnableIf<TIsDerivedFrom<T, UObject>::Value>::Type>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const T* Object)
	{
		if (!Object)
		{
			FVoxelMessageArgProcessor::ProcessObjectArg(Builder, nullptr);
			return;
		}

		if constexpr (TModels<CVoxelMessageProcessorHasGetMessageArg, T>::Value)
		{
			FVoxelMessageArgProcessor::ProcessArg(Builder, Object->GetMessageArg());
			return;
		}

		ensure(IsInGameThread());
		FVoxelMessageArgProcessor::ProcessObjectArg(Builder, Object);
	}
	static void ProcessArg(FVoxelMessageBuilder& Builder, const T& Object)
	{
		TVoxelMessageArgProcessor::ProcessArg(Builder, &Object);
	}
};

template<typename T>
struct TVoxelMessageArgProcessor<T, typename TEnableIf<TIsDerivedFrom<T, IInterface>::Value && !TIsDerivedFrom<T, UObject>::Value>::Type>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const T* Object)
	{
		if (!Object)
		{
			FVoxelMessageArgProcessor::ProcessObjectArg(Builder, nullptr);
			return;
		}

		if constexpr (TModels<CVoxelMessageProcessorHasGetMessageArg, T>::Value)
		{
			FVoxelMessageArgProcessor::ProcessArg(Builder, Object->GetMessageArg());
			return;
		}

		ensure(IsInGameThread());
		FVoxelMessageArgProcessor::ProcessObjectArg(Builder, Cast<UObject>(Object));
	}
	static void ProcessArg(FVoxelMessageBuilder& Builder, const T& Object)
	{
		TVoxelMessageArgProcessor::ProcessArg(Builder, &Object);
	}
};

template<typename T>
struct TVoxelMessageArgProcessor<TObjectPtr<T>, typename TEnableIf<TIsDerivedFrom<T, UObject>::Value>::Type>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TObjectPtr<T>& Object)
	{
		ensure(IsInGameThread());
		FVoxelMessageArgProcessor::ProcessObjectArg(Builder, Object.Get());
	}
};

template<typename T>
struct TVoxelMessageArgProcessor<TSoftObjectPtr<T>>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TSoftObjectPtr<const T> Object)
	{
		if (ensure(IsInGameThread()))
		{
			FVoxelMessageArgProcessor::ProcessObjectArg(Builder, ReinterpretCastRef<FSoftObjectPtr>(Object).LoadSynchronous());
		}
		else
		{
			FVoxelMessageArgProcessor::ProcessArg(Builder, "SoftObjectPtr");
		}
	}
};

template<typename T>
struct TVoxelMessageArgProcessor<TSubclassOf<T>>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TSubclassOf<T>& Class)
	{
		ensure(IsInGameThread());
		FVoxelMessageArgProcessor::ProcessObjectArg(Builder, Class.Get());
	}
};

template<>
struct TVoxelMessageArgProcessor<FScriptInterface>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FScriptInterface& ScriptInterface)
	{
		ensure(IsInGameThread());
		FVoxelMessageArgProcessor::ProcessObjectArg(Builder, ScriptInterface.GetObject());
	}
};

template<>
struct TVoxelMessageArgProcessor<UEdGraphPin>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const UEdGraphPin* Pin)
	{
		FVoxelMessageArgProcessor::ProcessPinArg(Builder, Pin);
	}
	static void ProcessArg(FVoxelMessageBuilder& Builder, const UEdGraphPin& Pin)
	{
		FVoxelMessageArgProcessor::ProcessPinArg(Builder, &Pin);
	}
};

template<typename T, typename Allocator>
struct TVoxelMessageArgProcessor<TArray<T, Allocator>>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TArray<T, Allocator>& Array)
	{
		if (Array.Num() == 0)
		{
			FVoxelMessageArgProcessor::ProcessArg(Builder, "Empty");
			return;
		}

		FString Format = "{0}";
		for (int32 Index = 1; Index < Array.Num(); Index++)
		{
			Format += FString::Printf(TEXT(", {%d}"), Index);
		}

		const TSharedRef<FVoxelMessageBuilder> ChildBuilder = MakeVoxelShared<FVoxelMessageBuilder>(Builder.Severity, Format);
		for (const T& Token : Array)
		{
			FVoxelMessageArgProcessor::ProcessArg(*ChildBuilder, Token);
		}
		FVoxelMessageArgProcessor::ProcessArg(Builder, ChildBuilder);
	}
};

template<typename T, typename Allocator>
struct TVoxelMessageArgProcessor<TVoxelArray<T, Allocator>>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TVoxelArray<T, Allocator>& Array)
	{
		TVoxelMessageArgProcessor<TArray<T, Allocator>>::ProcessArg(Builder, Array);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct VOXELCORE_API FVoxelMessages
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMessageLogged, const TSharedRef<FTokenizedMessage>&);
	static FOnMessageLogged OnMessageLogged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnFocusNode, const UEdGraphNode& Node);
	static FOnFocusNode OnFocusNode;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNodeMessageLogged, const UEdGraph* Graph, const TSharedRef<FTokenizedMessage>& Message);
	static FOnNodeMessageLogged OnNodeMessageLogged;

	using FGatherCallstack = TFunction<void(TVoxelArray<TSharedPtr<FVoxelMessageBuilder>>& OutCallstacks)>;
	static TArray<FGatherCallstack> GatherCallstacks;

public:
	static void LogMessage(const TSharedRef<FTokenizedMessage>& Message);

public:
	template<typename... ArgTypes>
	static void LogMessage(EVoxelMessageSeverity Severity, const TCHAR* Format, ArgTypes&&... Args)
	{
		VOXEL_FUNCTION_COUNTER();

		const TSharedRef<FVoxelMessageBuilder> Builder = MakeVoxelShared<FVoxelMessageBuilder>(Severity, Format);
		FVoxelMessageArgProcessor::ProcessArgs(*Builder, Forward<ArgTypes>(Args)...);
		InternalLogMessage(Builder);
	}

	static void InternalLogMessage(const TSharedRef<FVoxelMessageBuilder>& Builder);
};

#define INTERNAL_CHECK_ARG(Name) INTELLISENSE_ONLY((void)((TVoxelMessageArgProcessorType<VOXEL_GET_TYPE(Name)>*)nullptr);)

#define VOXEL_MESSAGE(Severity, Message, ...) \
	FVoxelMessages::LogMessage(EVoxelMessageSeverity::Severity, TEXT(Message), ##__VA_ARGS__); \
	VOXEL_FOREACH(INTERNAL_CHECK_ARG, 0, ##__VA_ARGS__)