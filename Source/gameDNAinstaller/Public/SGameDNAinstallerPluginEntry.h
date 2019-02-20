// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#pragma once

#include "GameDNAinstallerTypes.h"

class SGameDNAinstallerPluginEntry : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGameDNAinstallerPluginEntry)
		: _PluginEntry()
		, _ButtonClickedDelegate()
		{}

		SLATE_ATTRIBUTE(TSharedPtr<FGameDNAinstallerPlugin>, PluginEntry)

		SLATE_EVENT(FOnGameDNAinstallerPluginButtonClicked, ButtonClickedDelegate)

	SLATE_END_ARGS()

	~SGameDNAinstallerPluginEntry();

	/** Construct */
	void Construct(const FArguments& InArgs);

private:
	/** On Plugin Button Clicked */
	FReply OnPluginButtonClicked();

	/** Button Clicked Delegate */
	FOnGameDNAinstallerPluginButtonClicked ButtonClickedDelegate;

	/** Plugin Entry */
	TSharedPtr<FGameDNAinstallerPlugin> PluginEntry;
};
