// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#include "SGameDNAinstallerPluginEntry.h"
#include "SGameDNAinstallerConsole.h"
#include "EditorStyleSet.h"
#include "Editor.h"
#include "Runtime/Core/Public/Internationalization/Text.h"
#include "GameDNAinstallerStyle.h"
#include "Runtime/SlateCore/Public/Types/SlateStructs.h"

void SGameDNAinstallerPluginEntry::Construct(const FArguments& InArgs)
{		
	TSharedPtr<SButton> PluginButton;

	PluginEntry = InArgs._PluginEntry.Get();
	ButtonClickedDelegate = InArgs._ButtonClickedDelegate;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(FMargin(0, 4, 0, 0))
		[
			SNew(STextBlock)
			.Text(FText::FromString(PluginEntry->Name))
			.TextStyle(FGameDNAinstallerStyle::Get(), "gameDNAinstaller.MainTextStyle")
		]
		+SHorizontalBox::Slot()
		[
			SAssignNew(PluginButton, SButton)
			.Text(FText::FromString("Install SDKs"))
			.TextStyle(FGameDNAinstallerStyle::Get(), "gameDNAinstaller.PluginButtonTextStyle")
			.ButtonStyle(FGameDNAinstallerStyle::Get(), "gameDNAinstaller.PluginButtonStyle")
			.OnClicked(this, &SGameDNAinstallerPluginEntry::OnPluginButtonClicked)
		]
	];

	TSharedRef<STextBlock> PluginButtonText = StaticCastSharedRef<STextBlock>(PluginButton->GetContent());
	PluginButtonText->SetJustification(ETextJustify::Center);
	switch (PluginEntry->State)
	{
		case EGameDNAInstallerPluginState::NotInstalled:
			PluginButtonText->SetText(FString("Get it now"));
			break;
		case EGameDNAInstallerPluginState::Installed:
			PluginButtonText->SetText(FString("Reinstall"));
			break;
		case EGameDNAInstallerPluginState::InstallSDK:
			PluginButtonText->SetText(FString("Install SDK"));
			PluginButton->SetButtonStyle(&FGameDNAinstallerStyle::Get().GetWidgetStyle<FButtonStyle>("gameDNAinstaller.PluginInstallButtonStyle"));
			break;
		case EGameDNAInstallerPluginState::UpdateSDK:
			PluginButtonText->SetText(FString("Update SDK"));
			PluginButton->SetButtonStyle(&FGameDNAinstallerStyle::Get().GetWidgetStyle<FButtonStyle>("gameDNAinstaller.PluginInstallButtonStyle"));
			break;
		default:
			PluginButtonText->SetText(FString("Get it now"));
			break;
	}

	PluginButton->SetEnabled(PluginEntry->ButtonCanBeEnabled);
}

SGameDNAinstallerPluginEntry::~SGameDNAinstallerPluginEntry()
{
}

FReply SGameDNAinstallerPluginEntry::OnPluginButtonClicked()
{
	ButtonClickedDelegate.ExecuteIfBound(PluginEntry.ToSharedRef());

	return FReply::Handled();
}
