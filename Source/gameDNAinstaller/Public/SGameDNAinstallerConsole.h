// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#pragma once

#include "Runtime/Slate/Public/Widgets/Views/SListView.h"
#include "Runtime/Slate/Public/Widgets/Views/STableRow.h"
#include "Runtime/Slate/Public/Widgets/Views/STableViewBase.h"
#include "Runtime/Slate/Public/Widgets/Input/SMultiLineEditableTextBox.h"
#include "SGameDNAinstallerMarshaller.h"
#include "GameDNAinstallerTypes.h"

class SGameDNAinstallerConsole : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGameDNAinstallerConsole)
	{}
	SLATE_END_ARGS()

	~SGameDNAinstallerConsole();

	/** Construct */
	void Construct(const FArguments& InArgs);

	/** Append Message */
	void AppendMessage(const TCHAR* InText);

private:
	/** Refresh Plugins List */
	void RefreshPluginsList();

	/** Change Plugins List Visibility */
	void ChangePluginsListVisibility(bool bAfterDownload = false);

	/** Download Plugins List */
	void DownloadPluginsList();

	/** On Download Plugins List Completed */
	void OnDownloadPluginsListCompleted(bool bSuccess);

	/** On User Scrolled */
	void OnUserScrolled(float ScrollOffset);

	/** Exten TextBox Menu */
	void ExtendTextBoxMenu(FMenuBuilder& Builder);

	/** Request Force Scroll */
	void RequestForceScroll();

	/** Can Clear Log */
	bool CanClearLog() const;

	/** On Refresh Button Clicked */
	FReply OnRefreshButtonClicked();

	/** On GameDNA Link Clicked */
	void OnGameDNALinkClicked();

	/** On Installation Completed */
	void OnInstallationCompleted(bool bSuccess);

	/** On Generate Plugin Row List */
	TSharedRef<ITableRow> OnGeneratePluginRowList(TSharedPtr<FGameDNAinstallerPlugin> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/** On Clear Log */
	void OnClearLog();

	/** On Plugin Button Clicked */
	void OnPluginButtonClicked(TSharedRef<FGameDNAinstallerPlugin> PluginEntry);

	/** Disable All Buttons */
	void DisableAllButtons();

	/** Core */
	FGameDNAinstallerCorePtr Core;

	/** Plugins List View */
	TSharedPtr<SListView<TSharedPtr<FGameDNAinstallerPlugin>>> PluginsListView;

	/** Messages Text Marshaller */
	TSharedPtr<FGameDNAinstallerTextLayoutMarshaller> MessagesTextMarshaller;

	/** Messages Text Box */
	TSharedPtr<SMultiLineEditableTextBox> MessagesTextBox;

	/** Loading Box */
	TSharedPtr<SHorizontalBox> LoadingBox;

	/** Downloading Messaging */
	TSharedPtr<STextBlock> DownloadingMessaging;

	/** Refresh Button */
	TSharedPtr<SButton> RefreshButton;

	/** Is User Scrolled */
	bool bIsUserScrolled;
};
