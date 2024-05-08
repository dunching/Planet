// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreEditorMinimal.h"
#include "SVoxelDetailWidgets.h"
#include "VoxelEditorDetailsUtilities.h"

template<typename T>
class SVoxelDetailComboBox
	: public SCompoundWidget
	, public FSelfRegisteringEditorUndoClient
{
public:
	DECLARE_DELEGATE_OneParam(FOnSelection, T);
	DECLARE_DELEGATE_RetVal_OneParam(bool, FIsOptionValid, T);
	DECLARE_DELEGATE_RetVal_OneParam(FString, FGetOptionText, T);
	DECLARE_DELEGATE_RetVal_OneParam(FText, FGetOptionToolTip, T);
	DECLARE_DELEGATE_RetVal_OneParam(T, FOnMakeOptionFromText, FString);
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnGenerate, T);

	VOXEL_SLATE_ARGS()
	{
		FArguments()
		{
			_CurrentOption = T{};
			_CanEnterCustomOption = false;

			if constexpr (std::is_same_v<T, FName> || std::is_same_v<T, FString>)
			{
				_OptionText = MakeLambdaDelegate([](T Value)
				{
					return LexToString(Value);
				});
				_OnMakeOptionFromText = MakeLambdaDelegate([](FString String)
				{
					T Value;
					LexFromString(Value, *String);
					return Value;
				});
			};
		}

		SLATE_ARGUMENT(FSimpleDelegate, RefreshDelegate);

		bool _NoRefreshDelegate = false;
		WidgetArgsType& NoRefreshDelegate()
		{
			_NoRefreshDelegate = true;
			return *this;
		}

		template<typename ArgType>
		WidgetArgsType& RefreshDelegate(const TSharedPtr<IPropertyHandle>& PropertyHandle, const ArgType& Arg)
		{
			_RefreshDelegate = FVoxelEditorUtilities::MakeRefreshDelegate(PropertyHandle, Arg);
			return *this;
		}

		SLATE_ATTRIBUTE(TArray<T>, Options);
		SLATE_ARGUMENT(T, CurrentOption);

		SLATE_ARGUMENT(bool, CanEnterCustomOption);
		SLATE_EVENT(FOnMakeOptionFromText, OnMakeOptionFromText);

		SLATE_EVENT(FGetOptionText, OptionText)
		SLATE_EVENT(FGetOptionToolTip, OptionToolTip);
		SLATE_EVENT(FIsOptionValid, IsOptionValid);

		SLATE_EVENT(FOnSelection, OnSelection);
		SLATE_EVENT(FOnGenerate, OnGenerate);
	};

	void Construct(const FArguments& Args)
	{
		OptionsAttribute = Args._Options;
		for (T Option : OptionsAttribute.Get())
		{
			Options.Add(MakeVoxelShared<T>(Option));
		}

		for (const TSharedPtr<T>& Option : Options)
		{
			if (*Option == Args._CurrentOption)
			{
				CurrentOption = Option;
			}
		}
		if (!CurrentOption)
		{
			CurrentOption = MakeVoxelShared<T>(Args._CurrentOption);
		}

		bCanEnterCustomOption = Args._CanEnterCustomOption;
		if (bCanEnterCustomOption)
		{
			ensure(Args._OnMakeOptionFromText.IsBound());
		}

		OnMakeOptionFromTextDelegate = Args._OnMakeOptionFromText;
		GetOptionTextDelegate = Args._OptionText;
		GetOptionToolTipDelegate = Args._OptionToolTip;
		IsOptionValidDelegate = Args._IsOptionValid;
		OnSelectionDelegate = Args._OnSelection;
		OnGenerateDelegate = Args._OnGenerate;

		ensure(Args._NoRefreshDelegate || Args._RefreshDelegate.IsBound());
		RefreshDelegate = Args._RefreshDelegate;

		TSharedPtr<SWidget> ComboboxContent;
		if (bCanEnterCustomOption)
		{
			SAssignNew(CustomOptionTextBox, SEditableTextBox)
			.Text(this, &SVoxelDetailComboBox::GetCurrentOptionText)
			.OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type)
			{
				if (!ensure(OnMakeOptionFromTextDelegate.IsBound()))
				{
					return;
				}

				T NewOption = OnMakeOptionFromTextDelegate.Execute(Text.ToString());
				CurrentOption = nullptr;
				for (const TSharedPtr<T>& Option : Options)
				{
					if (NewOption == *Option)
					{
						CurrentOption = Option;
						break;
					}
				}

				if (!CurrentOption)
				{
					CurrentOption = MakeVoxelShared<T>(NewOption);
				}

				ComboBox->SetSelectedItem(CurrentOption);
			})
			.SelectAllTextWhenFocused(true)
			.RevertTextOnEscape(true)
			.Font(FVoxelEditorUtilities::Font());

			ComboboxContent = CustomOptionTextBox;
		}
		else
		{
			SAssignNew(SelectedItemBox, SBox)
			[
				GenerateWidget(CurrentOption)
			];

			ComboboxContent = SelectedItemBox;
		}

		ChildSlot
		[
			SAssignNew(ComboBox, SComboBox<TSharedPtr<T>>)
			.OptionsSource(&Options)
			.OnSelectionChanged_Lambda([this](TSharedPtr<T> NewOption, ESelectInfo::Type SelectInfo)
			{
				if (!NewOption)
				{
					// Will happen when doing SetSelectedItem(nullptr) below
					return;
				}

				CurrentOption = NewOption;

				const TWeakPtr<SWidget> WeakThis = AsWeak();
				check(WeakThis.IsValid());

				OnSelectionDelegate.ExecuteIfBound(*NewOption);

				if (!WeakThis.IsValid())
				{
					// OnSelectionDelegate destroyed our widget
					return;
				}

				RefreshOptionsList();

				if (!bCanEnterCustomOption)
				{
					const TSharedRef<SWidget> Widget = GenerateWidget(CurrentOption);
					SelectedItemBox->SetContent(Widget);
				}
			})
			.OnGenerateWidget_Lambda([this](const TSharedPtr<T>& Option) -> TSharedRef<SWidget>
			{
				return GenerateWidget(Option);
			})
			.InitiallySelectedItem(CurrentOption)
			[
				ComboboxContent.ToSharedRef()
			]
		];
	}

public:
	void SetCurrentItem(T NewSelection)
	{
		if (*CurrentOption == NewSelection)
		{
			return;
		}

		TSharedPtr<T> NewOption = nullptr;
		for (const TSharedPtr<T>& Option : Options)
		{
			if (*Option == NewSelection)
			{
				NewOption = Option;
			}
		}

		if (NewOption)
		{
			CurrentOption = NewOption;
		}
		else if (bCanEnterCustomOption)
		{
			CurrentOption = MakeVoxelShared<T>(NewSelection);
		}
		else
		{
			return;
		}

		ComboBox->SetSelectedItem(CurrentOption);
	}

private:
	TSharedPtr<SComboBox<TSharedPtr<T>>> ComboBox;
	TSharedPtr<SBox> SelectedItemBox;
	TSharedPtr<SEditableTextBox> CustomOptionTextBox;

	TAttribute<TArray<T>> OptionsAttribute;
	TArray<TSharedPtr<T>> Options;
	TSharedPtr<T> CurrentOption;

	bool bCanEnterCustomOption = false;

	FOnMakeOptionFromText OnMakeOptionFromTextDelegate;
	FGetOptionText GetOptionTextDelegate;
	FGetOptionToolTip GetOptionToolTipDelegate;
	FIsOptionValid IsOptionValidDelegate;
	FOnSelection OnSelectionDelegate;
	FOnGenerate OnGenerateDelegate;

	TSharedRef<SWidget> GenerateWidget(const TSharedPtr<T>& Option) const
	{
		if (OnGenerateDelegate.IsBound())
		{
			const TSharedRef<SWidget> Widget = OnGenerateDelegate.Execute(*Option);
			Widget->SetClipping(EWidgetClipping::ClipToBounds);
			return Widget;
		}

		return SNew(SVoxelDetailText)
			.Clipping(EWidgetClipping::ClipToBounds)
			.Text(this, &SVoxelDetailComboBox::GetOptionText, Option)
			.ToolTipText(this, &SVoxelDetailComboBox::GetOptionToolTip, Option)
			.ColorAndOpacity(this, &SVoxelDetailComboBox::GetOptionColor, Option);
	}

public:
	void RefreshOptionsList()
	{
		VOXEL_FUNCTION_COUNTER();

		TArray<T> NewOptions = OptionsAttribute.Get();

		// Remove non existing options
		Options.RemoveAll([&](const TSharedPtr<T>& Option)
		{
			if (NewOptions.Contains(*Option))
			{
				NewOptions.Remove(*Option);
				return false;
			}
			return true;
		});

		// Add new options
		for (const T& Option : NewOptions)
		{
			if (Option == *CurrentOption)
			{
				Options.Add(CurrentOption);
			}
			else
			{
				Options.Add(MakeVoxelShared<T>(Option));
			}
		}
		ComboBox->RefreshOptions();
	}

private:
	FText GetOptionText(TSharedPtr<T> Option) const
	{
		if (!GetOptionTextDelegate.IsBound())
		{
			return {};
		}

		return FText::FromString(GetOptionTextDelegate.Execute(*Option));
	}
	FText GetOptionToolTip(TSharedPtr<T> Option) const
	{
		if (!GetOptionToolTipDelegate.IsBound())
		{
			return {};
		}

		return GetOptionToolTipDelegate.Execute(*Option);
	}
	FSlateColor GetOptionColor(TSharedPtr<T> Option) const
	{
		if (!Options.Contains(Option))
		{
			return FLinearColor::Red;
		}
		if (IsOptionValidDelegate.IsBound() && !IsOptionValidDelegate.Execute(*Option))
		{
			return FLinearColor::Red;
		}

		return FSlateColor::UseForeground();
	}

private:
	FText GetCurrentOptionText() const
	{
		return this->GetOptionText(CurrentOption);
	}
	FText GetCurrentOptionToolTip() const
	{
		return this->GetOptionToolTip(CurrentOption);
	}
	FSlateColor GetCurrentOptionColor() const
	{
		return this->GetOptionColor(CurrentOption);
	}

private:
	FSimpleDelegate RefreshDelegate;

	//~ Begin Interface FSelfRegisteringEditorUndoClient
	virtual void PostUndo(bool bSuccess) override
	{
		RefreshDelegate.ExecuteIfBound();
	}
	virtual void PostRedo(bool bSuccess) override
	{
		RefreshDelegate.ExecuteIfBound();
	}
	//~ End Interface FSelfRegisteringEditorUndoClient
};