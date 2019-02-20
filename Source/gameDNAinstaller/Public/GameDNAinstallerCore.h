// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameDNAinstallerTypes.h"
#include "HttpModule.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "Dom/JsonValue.h"

class FGameDNAinstallerCore
{
public:
	FGameDNAinstallerCore();
	virtual ~FGameDNAinstallerCore();

	/** Install Plugin */
	bool InstallPlugin(TSharedRef<FGameDNAinstallerPlugin> PluginEntry, const FOnGameDNAinstallerInstallationProgress& InInstallProgressDelegate, const FOnGameDNAinstallerInstallationCompleted& InInstallCompletedDelegate);

	/** Download Plugins List */
	bool DownloadPluginsList(const FOnGameDNAinstallerDownloadPluginsListCompleted& InDownloadPluginsListCompletedDelegate = FOnGameDNAinstallerDownloadPluginsListCompleted());

	/** Get Plugins List */
	TArray<TSharedPtr<FGameDNAinstallerPlugin>>& GetPluginsList();

	/** Disable All Buttons */
	void DisableAllButtons();

private:
	/** Is Installation In Progress */
	bool bIsInstallationInProgress;

	/** Is Downloading In Progress */
	bool bIsDownloadingInProgress;

	/** Installation Completed */
	void InstallationCompleted(int32 ReturnCode, const FString PluginId, const int32 PluginRevision);

	/** Safe Installation Completed */
	void SafeInstallationCompleted(bool bSuccess);

	/** Installation Canceled */
	void InstallationCanceled();

	/** Installation Progress */
	void InstallationProgress(FString Output);

	/** On Download Plugins List */
	void OnDownloadPluginsList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/** Process Plugin State */
	EGameDNAInstallerPluginState ProcessPluginState(const FString& PluginId, const TArray<TSharedPtr<FJsonValue>>& Versions, class UGameDNAinstallerSave* SaveGameObject, int32& OutRevision);

	/** Plugins List */
	TArray<TSharedPtr<FGameDNAinstallerPlugin>> PluginsList;

	/** Install Progress Delegate */
	FOnGameDNAinstallerInstallationProgress InstallProgressDelegate;

	/** Install Completed Delegate */
	FOnGameDNAinstallerInstallationCompleted InstallCompletedDelegate;

	/** Download Plugins List Completed Delegate */
	FOnGameDNAinstallerDownloadPluginsListCompleted DownloadPluginsListCompletedDelegate;

	/** Numeric version from String */
	uint32 NumericVersionFromString(const FString& StringVersion);

	/** Load Save Game Object */
	class UGameDNAinstallerSave* LoadSaveGameObject();
};
