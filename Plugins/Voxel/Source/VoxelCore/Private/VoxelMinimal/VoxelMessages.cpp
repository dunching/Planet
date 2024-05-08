// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "Misc/UObjectToken.h"
#include "Logging/MessageLog.h"
#include "HAL/ThreadSingleton.h"
#include "Framework/Application/SlateApplication.h"

#if WITH_EDITOR
#include "Kismet2/KismetDebugUtilities.h"
#include "Kismet2/KismetEditorUtilities.h"
#endif

FVoxelMessages::FOnMessageLogged FVoxelMessages::OnMessageLogged;
FVoxelMessages::FOnFocusNode FVoxelMessages::OnFocusNode;
FVoxelMessages::FOnNodeMessageLogged FVoxelMessages::OnNodeMessageLogged;
TArray<FVoxelMessages::FGatherCallstack> FVoxelMessages::GatherCallstacks;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelMessagesThreadSingleton : public TThreadSingleton<FVoxelMessagesThreadSingleton>
{
public:
	// Weak ptr because messages are built on the game thread
	TArray<TWeakPtr<IVoxelMessageConsumer>> MessageConsumers;

	TWeakPtr<IVoxelMessageConsumer> GetTop() const
	{
		if (MessageConsumers.Num() == 0)
		{
			return nullptr;
		}
		return MessageConsumers.Last();
	}
};

FVoxelScopedMessageConsumer::FVoxelScopedMessageConsumer(TWeakPtr<IVoxelMessageConsumer> MessageConsumer)
{
	FVoxelMessagesThreadSingleton::Get().MessageConsumers.Add(MessageConsumer);
}

FVoxelScopedMessageConsumer::FVoxelScopedMessageConsumer(TFunction<void(const TSharedRef<FTokenizedMessage>&)> LogMessage)
{
	class FMessageConsumer : public IVoxelMessageConsumer
	{
	public:
		TFunction<void(const TSharedRef<FTokenizedMessage>&)> LogMessageLambda;

		virtual void LogMessage(const TSharedRef<FTokenizedMessage>& Message) override
		{
			LogMessageLambda(Message);
		}
	};

	const TSharedRef<FMessageConsumer> Consumer = MakeVoxelShared<FMessageConsumer>();
	Consumer->LogMessageLambda = LogMessage;

	TempConsumer = Consumer;
	FVoxelMessagesThreadSingleton::Get().MessageConsumers.Add(Consumer);
}

FVoxelScopedMessageConsumer::~FVoxelScopedMessageConsumer()
{
	FVoxelMessagesThreadSingleton::Get().MessageConsumers.Pop(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMessages::LogMessage(const TSharedRef<FTokenizedMessage>& Message)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(IsInGameThread()))
	{
		return;
	}

	{
		VOXEL_SCOPE_COUNTER("Check recent messages");

		struct FRecentMessage
		{
			FString Text;
			double Time = 0;
		};
		static TArray<FRecentMessage> RecentMessages;

		const FString Text = Message->ToText().ToString();
		const double Time = FPlatformTime::Seconds();

		RecentMessages.RemoveAllSwap([&](const FRecentMessage& RecentMessage)
		{
			return RecentMessage.Time + 0.5 < Time;
		});

		if (RecentMessages.FindByPredicate([&](const FRecentMessage& RecentMessage)
			{
				return RecentMessage.Text.Equals(Text);
			}))
		{
			// A message with the same text was displayed recently, don't log this one again
			return;
		}

		RecentMessages.Add(FRecentMessage
		{
			Text,
			Time
		});
	}

#if WITH_EDITOR
	INLINE_LAMBDA
	{
		const TArrayView<const FFrame* const> ScriptStack = FBlueprintContextTracker::Get().GetCurrentScriptStack();

		if (ScriptStack.Num() == 0 ||
			!ensure(ScriptStack.Last()))
		{
			return;
		}
		const FFrame& Frame = *ScriptStack.Last();

		const UClass* Class = FKismetDebugUtilities::FindClassForNode(nullptr, Frame.Node);
		if (!Class)
		{
			return;
		}

		const UBlueprint* Blueprint = Cast<UBlueprint>(Class->ClassGeneratedBy);
		if (!Blueprint)
		{
			return;
		}

		const UBlueprintGeneratedClass* GeneratedClass = Cast<UBlueprintGeneratedClass>(Class);
		if (!GeneratedClass ||
			!GeneratedClass->DebugData.IsValid())
		{
			return;
		}

		const int32 CodeOffset = Frame.Code - Frame.Node->Script.GetData() - 1;
		const UEdGraphNode* BlueprintNode = GeneratedClass->DebugData.FindSourceNodeFromCodeLocation(Frame.Node, CodeOffset, true);
		if (!BlueprintNode)
		{
			return;
		}

		const TSharedRef<IMessageToken> Token = FUObjectToken::Create(BlueprintNode, BlueprintNode->GetNodeTitle(ENodeTitleType::ListView))
			->OnMessageTokenActivated(MakeLambdaDelegate([](const TSharedRef<IMessageToken>& InToken)
			{
				check(InToken->GetType() == EMessageToken::Object);

				const UObject* Object = StaticCastSharedRef<FUObjectToken>(InToken)->GetObject().Get();
				if (!Object)
				{
					return;
				}

				FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Object);
			}));

		ConstCast(Message->GetMessageTokens()).Insert(Token, 1);
	};
#endif

	const auto LogMessage = [=]
	{
		if (OnMessageLogged.IsBound())
		{
			OnMessageLogged.Broadcast(Message);
		}
		else
		{
			FMessageLog("PIE").AddMessage(Message);
		}
	};

	if (FSlateApplication::IsInitialized() &&
		FSlateApplication::Get().GetActiveModalWindow())
	{
		// DelayedCall would only run once the modal is closed
		LogMessage();
		return;
	}

	FVoxelSystemUtilities::DelayedCall(LogMessage);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_UNIQUE_VOXEL_ID(FVoxelMessageBuilderId);

FVoxelMessageBuilder::FVoxelMessageBuilder(const EVoxelMessageSeverity Severity, const FString& Format)
	: Severity(Severity)
	, Format(Format)
	, MessageConsumer(FVoxelMessagesThreadSingleton::Get().GetTop().Pin())
{
}

void FVoxelMessageBuilder::AppendTo(FTokenizedMessage& Message, TSet<const UEdGraph*>& OutGraphs) const
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	const FString FixedFormat = Format.Replace(TEXT("\n"), TEXT("{N}"));
	const TCHAR* FormatPtr = *FixedFormat;

	TArray<bool> UsedArgs;
	UsedArgs.SetNumZeroed(Args.Num());

	while (FormatPtr)
	{
		// Read to next "{":
		const TCHAR* Delimiter = FCString::Strstr(FormatPtr, TEXT("{"));
		if (!Delimiter)
		{
			// Add the remaining text
			const FString RemainingText(FormatPtr);
			if (!RemainingText.IsEmpty())
			{
				Message.AddToken(FTextToken::Create(FText::FromString(RemainingText)));
			}
			break;
		}

		// Add the text left of the {
		const FString TextBefore = FString(Delimiter - FormatPtr, FormatPtr);
		if (!TextBefore.IsEmpty())
		{
			Message.AddToken(FTextToken::Create(FText::FromString(TextBefore)));
		}

		FormatPtr = Delimiter + FCString::Strlen(TEXT("{"));

		// Read to next "}":
		Delimiter = FCString::Strstr(FormatPtr, TEXT("}"));
		if (!ensureMsgf(Delimiter, TEXT("Missing }")))
		{
			break;
		}

		const FString IndexString(Delimiter - FormatPtr, FormatPtr);

		if (IndexString == "N")
		{
			Message.AddToken(FTextToken::Create(FText::FromString("\n")));
			FormatPtr = Delimiter + FCString::Strlen(TEXT("}"));
			continue;
		}

		int32 Index = -1;
		if (IndexString.IsNumeric())
		{
			Index = FCString::Atoi(*IndexString);
		}
		else
		{
			ensure(false);
			break;
		}

		if (!ensureMsgf(Args.IsValidIndex(Index), TEXT("Out of bound index: {%d}"), Index))
		{
			break;
		}

		UsedArgs[Index] = true;
		if (ensure(Args[Index]))
		{
			Args[Index](Message, OutGraphs);
		}

		FormatPtr = Delimiter + FCString::Strlen(TEXT("}"));
	}

	for (int32 Index = 0; Index < Args.Num(); Index++)
	{
		ensureMsgf(UsedArgs[Index], TEXT("Unused arg: %d"), Index);
	}
}

TSharedRef<FTokenizedMessage> FVoxelMessageBuilder::BuildMessage(TSet<const UEdGraph*>& OutGraphs) const
{
	const EMessageSeverity::Type MessageSeverity = INLINE_LAMBDA
	{
		switch (Severity)
		{
		default: ensure(false);
		case EVoxelMessageSeverity::Info: return EMessageSeverity::Info;
		case EVoxelMessageSeverity::Warning: return EMessageSeverity::Warning;
		case EVoxelMessageSeverity::Error: return EMessageSeverity::Error;
		}
	};
	const TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create(MessageSeverity);

	AppendTo(*Message, OutGraphs);

	// Merge text tokens as otherwise they are rendered with an ugly space in between them

	TArray<TSharedRef<IMessageToken>> Tokens = Message->GetMessageTokens();
	if (ensure(Tokens[0]->GetType() == EMessageToken::Severity))
	{
		// First token is just the severity, no need to duplicate
		Tokens.RemoveAt(0);
	}

	for (int32 Index = 1; Index < Tokens.Num(); Index++)
	{
		IMessageToken& PreviousToken = *Tokens[Index - 1];
		IMessageToken& CurrentToken = *Tokens[Index];
		if (PreviousToken.GetType() == EMessageToken::Text &&
			CurrentToken.GetType() == EMessageToken::Text)
		{
			const FText NewText = FText::Format(INVTEXT("{0}{1}"), PreviousToken.ToText(), CurrentToken.ToText());
			Tokens.RemoveAt(Index - 1, 2);
			Tokens.Insert(FTextToken::Create(NewText), Index - 1);
			Index--;
		}
	}

	const TSharedRef<FTokenizedMessage> FinalMessage = FTokenizedMessage::Create(MessageSeverity);
	for (const TSharedRef<IMessageToken>& Token : Tokens)
	{
		FinalMessage->AddToken(Token);
	}

	for (const TSharedPtr<FVoxelMessageBuilder>& Callstack : Callstacks)
	{
		FinalMessage->AddToken(FTextToken::Create(FText::FromString("\n")));
		Callstack->AppendTo(*FinalMessage, OutGraphs);
	}

	return FinalMessage;
}

void FVoxelMessages::InternalLogMessage(const TSharedRef<FVoxelMessageBuilder>& Builder)
{
	if (Builder->IsSilenced())
	{
		return;
	}

	for (const FGatherCallstack& GatherCallstack : GatherCallstacks)
	{
		GatherCallstack(Builder->Callstacks);
	}

	FVoxelUtilities::RunOnGameThread([=]
	{
		TSet<const UEdGraph*> Graphs;
		const TSharedRef<FTokenizedMessage> Message = Builder->BuildMessage(Graphs);
		Graphs.Remove(nullptr);

		for (const UEdGraph* Graph : Graphs)
		{
			OnNodeMessageLogged.Broadcast(Graph, MakeSharedCopy(*Message));
		}

		for (const UEdGraph* Graph : Graphs)
		{
			UObject* Outer = Graph->GetOuter();
			while (Outer && !Outer->HasAllFlags(RF_Public))
			{
				Outer = Outer->GetOuter();
			}

			if (Outer)
			{
				ConstCast(Message->GetMessageTokens()).Insert(FVoxelObjectUtilities::MakeSafeObjectToken(Outer), 1);
				ConstCast(Message->GetMessageTokens()).Insert(FTextToken::Create(INVTEXT(": ")), 2);
			}
		}

		if (Builder->MessageConsumer)
		{
			Builder->MessageConsumer->LogMessage(Message);
		}
		else
		{
			LogMessage(Message);
		}
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMessageArgProcessor::ProcessTextArg(FVoxelMessageBuilder& Builder, const FText& Text)
{
	ensure(!Text.ToString().Contains("%d"));
	ensure(!Text.ToString().Contains("%l"));
	ensure(!Text.ToString().Contains("%f"));
	ensure(!Text.ToString().Contains("%s"));

	Builder.AddArg([=](FTokenizedMessage& Message, TSet<const UEdGraph*>& OutGraphs)
	{
		Message.AddToken(FTextToken::Create(Text));
	});
}

void FVoxelMessageArgProcessor::ProcessPinArg(FVoxelMessageBuilder& Builder, const UEdGraphPin* Pin)
{
	ensure(IsInGameThread());

	if (!Pin)
	{
		ProcessArg(Builder, "Null");
		return;
	}

	const TSharedRef<FVoxelMessageBuilder> ChildBuilder = MakeVoxelShared<FVoxelMessageBuilder>(Builder.Severity, "{0}.{1}");
	ProcessArg(*ChildBuilder, Pin->GetOwningNode());
#if WITH_EDITOR
	ProcessArg(*ChildBuilder, Pin->GetDisplayName());
#else
	ProcessArg(*ChildBuilder, Pin->GetName());
#endif
	ProcessArg(Builder, ChildBuilder);
}

void FVoxelMessageArgProcessor::ProcessObjectArg(FVoxelMessageBuilder& Builder, const TWeakObjectPtr<const UObject> WeakObject)
{
	Builder.AddArg([=](FTokenizedMessage& Message, TSet<const UEdGraph*>& OutGraphs)
	{
		check(IsInGameThread());

		const UObject* Object = WeakObject.Get();
		if (!Object)
		{
			Message.AddToken(FUObjectToken::Create(nullptr));
			return;
		}

#if WITH_EDITOR
		if (Object->IsA<UEdGraphNode>())
		{
			const UEdGraphNode* Node = CastChecked<UEdGraphNode>(Object);
			OutGraphs.Add(Node->GetGraph());

			FString Title = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
			if (Title.IsEmpty() ||
				Title == " ")
			{
				Title = "<empty>";
			}

			Message.AddToken(FActionToken::Create(
				FText::FromString(Title),
				Node->GetNodeTitle(ENodeTitleType::FullTitle),
				MakeLambdaDelegate([WeakNode = MakeWeakObjectPtr(Node)]
				{
					const UEdGraphNode* LocalNode = WeakNode.Get();
					if (!ensure(LocalNode))
					{
						return;
					}
					FVoxelMessages::OnFocusNode.Broadcast(*LocalNode);
				})));

			return;
		}
#endif

		Message.AddToken(FVoxelObjectUtilities::MakeSafeObjectToken(ConstCast(Object)));
	});
}

void FVoxelMessageArgProcessor::ProcessTokenArg(FVoxelMessageBuilder& Builder, const TSharedRef<IMessageToken>& Token)
{
	Builder.AddArg([=](FTokenizedMessage& Message, TSet<const UEdGraph*>& OutGraphs)
	{
		Message.AddToken(Token);
	});
}

void FVoxelMessageArgProcessor::ProcessChildArg(FVoxelMessageBuilder& Builder, const TSharedRef<FVoxelMessageBuilder>& ChildBuilder)
{
	Builder.AddArg([=](FTokenizedMessage& Message, TSet<const UEdGraph*>& OutGraphs)
	{
		const TSharedRef<FTokenizedMessage> ChildMessage = ChildBuilder->BuildMessage(OutGraphs);
		for (const TSharedRef<IMessageToken>& Token : ChildMessage->GetMessageTokens())
		{
			if (Token->GetType() == EMessageToken::Severity)
			{
				continue;
			}

			Message.AddToken(Token);
		}
	});
}