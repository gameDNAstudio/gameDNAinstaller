// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#include "SGameDNAinstallerConsole.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "Editor.h"
#include "Runtime/Core/Public/Internationalization/Text.h"
#include "Engine/StreamableManager.h"
#include "GameDNAinstallerStyle.h"
#include "Runtime/SlateCore/Public/Types/SlateStructs.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "SGameDNAinstallerPluginEntry.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Runtime/Slate/Public/Widgets/Input/SHyperlink.h"

void SGameDNAinstallerConsole::Construct(const FArguments& InArgs)
{
	Core = FGameDNAinstallerModule::Get().GetCore();

	MessagesTextMarshaller = FGameDNAinstallerTextLayoutMarshaller::Create();

	MessagesTextBox = SNew(SMultiLineEditableTextBox)
		.Style(FEditorStyle::Get(), "Log.TextBox")
		.TextStyle(FEditorStyle::Get(), "Log.Normal")
		.AutoWrapText(true)
		.ForegroundColor(FLinearColor::Gray)
		.Marshaller(MessagesTextMarshaller)
		.IsReadOnly(true)
		.AlwaysShowScrollbars(true)
		.OnVScrollBarUserScrolled(this, &SGameDNAinstallerConsole::OnUserScrolled)
		.ContextMenuExtender(this, &SGameDNAinstallerConsole::ExtendTextBoxMenu);

	ChildSlot
	[
		// Header
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(10, 10, 10, 10))
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Top)
			[
				SNew(SImage)
				.Image(FGameDNAinstallerStyle::Get().GetBrush("GameDNAinstaller.Logo"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1)
			.MaxWidth(400)
			.Padding(30, 0, 0, 0)
			[
				// Plugins List
				SNew(SOverlay)
				+SOverlay::Slot()
				[
					SAssignNew(PluginsListView, SListView<TSharedPtr<FGameDNAinstallerPlugin>>)
					.ItemHeight(35)
					.SelectionMode(ESelectionMode::None)
					.AllowOverscroll(EAllowOverscroll::No)
					.ConsumeMouseWheel(EConsumeMouseWheel::Never)
					.ListItemsSource(&Core->GetPluginsList())
					.OnGenerateRow(this, &SGameDNAinstallerConsole::OnGeneratePluginRowList)
				]
				+SOverlay::Slot()
				[
					SAssignNew(LoadingBox, SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(DownloadingMessaging, STextBlock)
					]
				]
			]
		]
		// Output log
		+SVerticalBox::Slot()
		.FillHeight(1)
		[
			MessagesTextBox.ToSharedRef()
		]
		// Footer
		+SVerticalBox::Slot()
		.Padding(FMargin(10, 10, 10, 10))
		.VAlign(VAlign_Fill)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(FMargin(0, 7, 0, 0))
			.HAlign(EHorizontalAlignment::HAlign_Left)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Copyright (c) 2014-2019 "))
					.TextStyle(FGameDNAinstallerStyle::Get(), "gameDNAinstaller.MainTextStyle")
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SHyperlink)
					.Text(FText::FromString("gameDNA Ltd"))
					.TextStyle(FGameDNAinstallerStyle::Get(), "gameDNAinstaller.MainTextStyle")
					.OnNavigate(this, &SGameDNAinstallerConsole::OnGameDNALinkClicked)
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(" All Rights Reserved."))
					.TextStyle(FGameDNAinstallerStyle::Get(), "gameDNAinstaller.MainTextStyle")
				]
			]
			+SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.Padding(FMargin(0, 2, 0, 0))
			.AutoWidth()
			[
				SAssignNew(RefreshButton, SButton)
				.Text(FText::FromString("Refresh"))
				.TextStyle(FGameDNAinstallerStyle::Get(), "gameDNAinstaller.PluginButtonTextStyle")
				.ButtonStyle(FGameDNAinstallerStyle::Get(), "gameDNAinstaller.PluginButtonStyle")
				.OnClicked(this, &SGameDNAinstallerConsole::OnRefreshButtonClicked)
			]
		]
	];

	PluginsListView->SetItemWidth(300);

	if(Core->GetPluginsList().Num() > 0)
	{
		ChangePluginsListVisibility();
	}
	else
	{
		DownloadPluginsList();
	}

	bIsUserScrolled = false;
	RequestForceScroll();
}

SGameDNAinstallerConsole::~SGameDNAinstallerConsole()
{
}

TSharedRef<ITableRow> SGameDNAinstallerConsole::OnGeneratePluginRowList(TSharedPtr<FGameDNAinstallerPlugin> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FGameDNAinstallerPlugin>>, OwnerTable)
	.Padding(5)
	[
		SNew(SGameDNAinstallerPluginEntry)
		.PluginEntry(InItem)
		.ButtonClickedDelegate(this, &SGameDNAinstallerConsole::OnPluginButtonClicked)
	];	
}

void SGameDNAinstallerConsole::OnPluginButtonClicked(TSharedRef<FGameDNAinstallerPlugin> PluginEntry)
{
	bool bNeedInstallation = false;

	switch (PluginEntry->State)
	{
	case EGameDNAInstallerPluginState::NotInstalled:
		UKismetSystemLibrary::LaunchURL(PluginEntry->Website);
		break;
	case EGameDNAInstallerPluginState::Installed:
		bNeedInstallation = true;
		break;
	case EGameDNAInstallerPluginState::InstallSDK:
		bNeedInstallation = true;
		break;
	case EGameDNAInstallerPluginState::UpdateSDK:
		bNeedInstallation = true;
		break;
	default:
		break;
	}

	if (bNeedInstallation)
	{
		FOnGameDNAinstallerInstallationProgress InstallationProgressDelegate = FOnGameDNAinstallerInstallationProgress::CreateRaw(this, &SGameDNAinstallerConsole::AppendMessage);
		FOnGameDNAinstallerInstallationCompleted InstallationCompletedDelegate = FOnGameDNAinstallerInstallationCompleted::CreateRaw(this, &SGameDNAinstallerConsole::OnInstallationCompleted);
		Core->InstallPlugin(PluginEntry, InstallationProgressDelegate, InstallationCompletedDelegate);

		DisableAllButtons();
	}
}

void SGameDNAinstallerConsole::DisableAllButtons()
{
	Core->DisableAllButtons();
	RefreshPluginsList();
	RefreshButton->SetEnabled(false);
}

void SGameDNAinstallerConsole::RequestForceScroll()
{
	if (MessagesTextMarshaller->GetNumMessages() > 0)
	{
		MessagesTextBox->ScrollTo(FTextLocation(MessagesTextMarshaller->GetNumMessages() - 1));
		bIsUserScrolled = false;
	}
}

void SGameDNAinstallerConsole::AppendMessage(const TCHAR* InText)
{
	if (MessagesTextMarshaller->AppendMessage(InText) && !bIsUserScrolled)
	{
		RequestForceScroll();
	}

	UE_LOG(LogGameDNAinstaller, Display, TEXT("%s"), InText);
}

void SGameDNAinstallerConsole::OnDownloadPluginsListCompleted(bool bSuccess)
{
	RefreshPluginsList();
	ChangePluginsListVisibility(true);

	if (bSuccess)
	{
		AppendMessage(*FString("Plugins list refreshed successfully"));
	}
	else
	{
		AppendMessage(*FString("Plugins list refresh failed"));
	}
}

void SGameDNAinstallerConsole::RefreshPluginsList()
{
	PluginsListView->RebuildList();
}

void SGameDNAinstallerConsole::ChangePluginsListVisibility(bool bAfterDownload)
{
	if (Core->GetPluginsList().Num() > 0)
	{
		PluginsListView->SetVisibility(EVisibility::Visible);
		LoadingBox->SetVisibility(EVisibility::Collapsed);
	}
	else
	{
		PluginsListView->SetVisibility(EVisibility::Collapsed);
		LoadingBox->SetVisibility(EVisibility::Visible);

		if (bAfterDownload)
		{
			DownloadingMessaging->SetText(FString("Connection error..."));
			AppendMessage(*FString("Connection error..."));
		}
		else 
		{
			DownloadingMessaging->SetText(FString("Getting info..."));
			AppendMessage(*FString("Getting info..."));
		}
	}
}

void SGameDNAinstallerConsole::DownloadPluginsList()
{
	FOnGameDNAinstallerDownloadPluginsListCompleted DownloadPluginsListCompleted = FOnGameDNAinstallerDownloadPluginsListCompleted::CreateRaw(this, &SGameDNAinstallerConsole::OnDownloadPluginsListCompleted);
	Core->DownloadPluginsList(DownloadPluginsListCompleted);
	ChangePluginsListVisibility();
}

void SGameDNAinstallerConsole::OnUserScrolled(float ScrollOffset)
{
	bIsUserScrolled = ScrollOffset < 1.0 && !FMath::IsNearlyEqual(ScrollOffset, 1.0f);
}

void SGameDNAinstallerConsole::ExtendTextBoxMenu(FMenuBuilder& Builder)
{
	FUIAction ClearOutputLogAction(
		FExecuteAction::CreateRaw(this, &SGameDNAinstallerConsole::OnClearLog),
		FCanExecuteAction::CreateSP(this, &SGameDNAinstallerConsole::CanClearLog)
	);

	Builder.AddMenuEntry(
		NSLOCTEXT("gameDNAinstaller", "ClearLogLabel", "Clear Log"),
		NSLOCTEXT("gameDNAinstaller", "ClearLogTooltip", "Clears all log messages"),
		FSlateIcon(),
		ClearOutputLogAction
	);
}

bool SGameDNAinstallerConsole::CanClearLog() const
{
	return MessagesTextMarshaller->GetNumMessages() > 0;
}

FReply SGameDNAinstallerConsole::OnRefreshButtonClicked()
{
	DownloadPluginsList();
	return FReply::Handled();
}

void SGameDNAinstallerConsole::OnGameDNALinkClicked()
{
	UKismetSystemLibrary::LaunchURL(FString("https://www.gamednastudio.com"));
}

void SGameDNAinstallerConsole::OnInstallationCompleted(bool bSuccess)
{
	RefreshButton->SetEnabled(true);
	DownloadPluginsList();

	if (bSuccess)
	{
		AppendMessage(*FString("Plugin installed successfully"));
	}
	else
	{
		AppendMessage(*FString("Plugin installation error"));
	}
}

void SGameDNAinstallerConsole::OnClearLog()
{
	MessagesTextBox->GoTo(FTextLocation(0));
	MessagesTextMarshaller->ClearMessages();
	MessagesTextBox->Refresh();
	bIsUserScrolled = false;
}
