// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#include "GameDNAinstallerCore.h"
#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"
#include "GameDNAinstallerSave.h"

#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"

#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

#include "Misc/MonitoredProcess.h"

#define LOCTEXT_NAMESPACE "gameDNAinstallerModule"

FGameDNAinstallerCore::FGameDNAinstallerCore() 
	: bIsInstallationInProgress(false)
	, bIsDownloadingInProgress(false)
{
}

FGameDNAinstallerCore::~FGameDNAinstallerCore()
{
}

bool FGameDNAinstallerCore::InstallPlugin(TSharedRef<FGameDNAinstallerPlugin> PluginEntry, const FOnGameDNAinstallerInstallationProgress& InInstallProgressDelegate, const FOnGameDNAinstallerInstallationCompleted& InInstallCompletedDelegate)
{
	if (!bIsInstallationInProgress)
	{
		InstallProgressDelegate = InInstallProgressDelegate;
		InstallCompletedDelegate = InInstallCompletedDelegate;

		bIsInstallationInProgress = true;

		TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginEntry->Id);
		if (Plugin.IsValid())
		{
			FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("GameDNAinstaller"))->GetBaseDir();
			FString EngineDir = FPaths::EngineDir();

			FString ProcessPath = FPaths::ConvertRelativePathToFull(PluginDir + TEXT("/Extras/Core/GameDNAinstaller.exe"));
			FString CommandLine = FString("\"") + PluginEntry->Id + FString("\" \"") + PluginEntry->Name + FString("\" \"") + Plugin->GetDescriptor().VersionName + FString("\" \"") + FPaths::ConvertRelativePathToFull(Plugin->GetBaseDir()) + FString("\" \"") + FPaths::ConvertRelativePathToFull(EngineDir) + FString("\" ") + FString::FromInt(PluginEntry->Revision);
			
#if PLATFORM_MAC
			FString CmdExe = TEXT("/bin/sh");
			FString ScriptPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Build/BatchFiles/Mac/RunMono.sh"));
			FString FullCommandLine = FString::Printf(TEXT("\"%s\" \"%s\" %s"), *ScriptPath, *ProcessPath, *CommandLine);
#elif PLATFORM_LINUX
			FString CmdExe = TEXT("/bin/bash");
			FString ScriptPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Build/BatchFiles/Linux/RunMono.sh"));
			FString FullCommandLine = FString::Printf(TEXT("\"%s\" \"%s\" %s"), *ScriptPath, *ProcessPath, *CommandLine);
#else
			FString CmdExe = ProcessPath;
			FString FullCommandLine = CommandLine;
#endif

			FMonitoredProcess* InstallerProcess = new FMonitoredProcess(CmdExe, FullCommandLine, true);
			InstallerProcess->OnOutput().BindRaw(this, &FGameDNAinstallerCore::InstallationProgress);
			InstallerProcess->OnCompleted().BindRaw(this, &FGameDNAinstallerCore::InstallationCompleted, PluginEntry->Id, PluginEntry->Revision);
			InstallerProcess->OnCanceled().BindRaw(this, &FGameDNAinstallerCore::InstallationCanceled);
			bool bSuccess = InstallerProcess->Launch();

			if (!bSuccess)
			{
				bIsInstallationInProgress = false;
				SafeInstallationCompleted(false);
			}

			return bSuccess;
		}
	}
	
	return false;
}

bool FGameDNAinstallerCore::DownloadPluginsList(const FOnGameDNAinstallerDownloadPluginsListCompleted& InDownloadPluginsListCompletedDelegate)
{
	if (!bIsDownloadingInProgress)
	{
		bIsDownloadingInProgress = true;

		DownloadPluginsListCompletedDelegate = InDownloadPluginsListCompletedDelegate;

		TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetURL(TEXT("https://gamednastudio.com/plugins/config/plugins.php"));
		HttpRequest->OnProcessRequestComplete().BindRaw(this, &FGameDNAinstallerCore::OnDownloadPluginsList);
		HttpRequest->ProcessRequest();

		return true;
	}

	return false;
}

TArray<TSharedPtr<FGameDNAinstallerPlugin>>& FGameDNAinstallerCore::GetPluginsList()
{
	return PluginsList;
}

void FGameDNAinstallerCore::DisableAllButtons()
{
	for(auto &Plugin : PluginsList)
	{
		Plugin->ButtonCanBeEnabled = false;
	}
}

void FGameDNAinstallerCore::InstallationCompleted(int32 ReturnCode, const FString PluginId, const int32 PluginRevision)
{
	bIsInstallationInProgress = false;

	if (ReturnCode == 0)
	{
		UGameDNAinstallerSave* SaveGameObject = LoadSaveGameObject();
		if (SaveGameObject->PluginsRevisions.Contains(PluginId))
		{
			SaveGameObject->PluginsRevisions[PluginId] = PluginRevision;
		}
		else
		{
			SaveGameObject->PluginsRevisions.Add(PluginId, PluginRevision);
		}
		UGameplayStatics::SaveGameToSlot(SaveGameObject, FString("GameDNAinstaller"), 0);

		for (auto &Plugin : PluginsList)
		{
			Plugin->State = EGameDNAInstallerPluginState::Installed;
		}

		SafeInstallationCompleted(true);
	}
	else
	{
		SafeInstallationCompleted(false);
	}
}

void FGameDNAinstallerCore::SafeInstallationCompleted(bool bSuccess)
{
	DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.gameDNAinstaller_InstallerCompleted"), STAT_FSimpleDelegateGraphTask_gameDNAinstaller_InstallerCompleted, STATGROUP_TaskGraphTasks);

	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
		FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]() {
			InstallCompletedDelegate.ExecuteIfBound(bSuccess);
		}),
		GET_STATID(STAT_FSimpleDelegateGraphTask_gameDNAinstaller_InstallerCompleted),
		nullptr,
		ENamedThreads::GameThread
	);
}

void FGameDNAinstallerCore::InstallationCanceled()
{
	UE_LOG(LogGameDNAinstaller, Display, TEXT("gameDNA installer - plugin installation canceled"));

	bIsInstallationInProgress = false;
	SafeInstallationCompleted(false);
}

void FGameDNAinstallerCore::InstallationProgress(FString Output)
{
	DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.gameDNAinstaller_InstallerProcessOutput"), STAT_FSimpleDelegateGraphTask_gameDNAinstaller_InstallerProcessOutput, STATGROUP_TaskGraphTasks);

	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
		FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]() {
			InstallProgressDelegate.ExecuteIfBound(*Output);
		}),
		GET_STATID(STAT_FSimpleDelegateGraphTask_gameDNAinstaller_InstallerProcessOutput),
		nullptr,
		ENamedThreads::GameThread
	);
}

void FGameDNAinstallerCore::OnDownloadPluginsList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bIsDownloadingInProgress = false;

	if (bWasSuccessful)
	{
		FString ResponseContent = Response->GetContentAsString();
		TSharedPtr<FJsonObject> ResponseObject = MakeShareable(new FJsonObject());

		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseContent);
		FJsonSerializer::Deserialize(JsonReader, ResponseObject);

		if (ResponseObject.IsValid())
		{
			if (ResponseObject->HasField("plugins"))
			{
				PluginsList.Empty();

				UGameDNAinstallerSave* SaveGameObject = LoadSaveGameObject();

				TArray<TSharedPtr<FJsonValue>> Plugins = ResponseObject->GetArrayField("plugins");
				for (auto Plugin : Plugins)
				{
					TSharedPtr<FGameDNAinstallerPlugin> PluginPtr = MakeShareable(new FGameDNAinstallerPlugin());
					TSharedPtr<FJsonObject> PluginObject = Plugin->AsObject();
					
					if (PluginObject->HasField("id"))
					{
						PluginPtr->Id = PluginObject->GetStringField("id");
					}
					if (PluginObject->HasField("name"))
					{
						PluginPtr->Name = PluginObject->GetStringField("name");
					}
					if (PluginObject->HasField("website"))
					{
						PluginPtr->Website = PluginObject->GetStringField("website");
					}
					if (PluginObject->HasField("versions"))
					{
						TArray<TSharedPtr<FJsonValue>> Versions = PluginObject->GetArrayField("versions");
						PluginPtr->State = ProcessPluginState(PluginPtr->Id, Versions, SaveGameObject, PluginPtr->Revision);
						PluginPtr->ButtonCanBeEnabled = true;
					}

					PluginsList.Add(PluginPtr);
				}

				DownloadPluginsListCompletedDelegate.ExecuteIfBound(true);
				return;
			}
		}
	}

	DownloadPluginsListCompletedDelegate.ExecuteIfBound(false);
	return;
}

EGameDNAInstallerPluginState FGameDNAinstallerCore::ProcessPluginState(const FString& PluginId, const TArray<TSharedPtr<FJsonValue>>& Versions, UGameDNAinstallerSave* SaveGameObject, int32& OutRevision)
{
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginId);

	// Plugin not Installed
	if (!Plugin.IsValid())
	{
		return EGameDNAInstallerPluginState::NotInstalled;
	}
    else
    {
        const FString& PluginVersion = Plugin->GetDescriptor().VersionName;
        int32 PluginVersionNumeric = NumericVersionFromString(PluginVersion);
        
        TMap<int32, int32> VersionsMap;
        
        for (auto Version : Versions)
        {
            TSharedPtr<FJsonObject> VersionObject = Version->AsObject();
            
            if (VersionObject->HasField("minInstalled") && VersionObject->HasField("targetRevision"))
            {
                const FString& MinInstalled = VersionObject->GetStringField("minInstalled");
                const int32 TargetRevision = VersionObject->GetNumberField("targetRevision");
                int32 MinInstalledNumeric = NumericVersionFromString(MinInstalled);
                VersionsMap.Add(MinInstalledNumeric, TargetRevision);
            }
        }
        
        int32 BestFoundVersion = 0;
        for (auto& Version : VersionsMap)
        {
            if (Version.Key <= PluginVersionNumeric && Version.Key >= BestFoundVersion)
            {
                BestFoundVersion = Version.Key;
            }
        }
        
        // Not found compatible version
        if(VersionsMap.Contains(BestFoundVersion))
        {
            OutRevision = VersionsMap[BestFoundVersion];
        }
        else
        {
            return EGameDNAInstallerPluginState::NotInstalled;
        }

        // Plugin installed but without installed SDK
        if (!SaveGameObject->PluginsRevisions.Contains(PluginId))
        {
            return EGameDNAInstallerPluginState::InstallSDK;
        }
        // Plugin installed with installed SDK
        else
        {
            // Update SDK
            if (OutRevision > SaveGameObject->PluginsRevisions[PluginId])
            {
                return EGameDNAInstallerPluginState::UpdateSDK;
            }
            // SDK installed
            else
            {
                return EGameDNAInstallerPluginState::Installed;
            }
        }
    }

	return EGameDNAInstallerPluginState::NotInstalled;
}

uint32 FGameDNAinstallerCore::NumericVersionFromString(const FString& StringVersion)
{
	TArray<FString> Version;
	StringVersion.ParseIntoArray(Version, TEXT("."));
	FString TempVersion;

	for (auto VersionNumber : Version)
	{
		TempVersion += VersionNumber;
	}
	
	if (TempVersion.IsNumeric())
	{
		return FCString::Atoi(*TempVersion);
	}

	return 0;
}

UGameDNAinstallerSave* FGameDNAinstallerCore::LoadSaveGameObject()
{
	UGameDNAinstallerSave* SaveGameObject;

	if (UGameplayStatics::DoesSaveGameExist(FString("GameDNAinstaller"), 0))
	{
		SaveGameObject = Cast<UGameDNAinstallerSave>(UGameplayStatics::LoadGameFromSlot(FString("GameDNAinstaller"), 0));
	}
	else
	{
		SaveGameObject = Cast<UGameDNAinstallerSave>(UGameplayStatics::CreateSaveGameObject(UGameDNAinstallerSave::StaticClass()));
	}

	return SaveGameObject;
}

#undef LOCTEXT_NAMESPACE
