// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameDNAinstaller.h"
#include "GameFramework/SaveGame.h"
#include "GameDNAinstallerSave.generated.h"

UCLASS()
class UGameDNAinstallerSave : public USaveGame
{
public:
	GENERATED_UCLASS_BODY()

	/** Save Slot Name */
	UPROPERTY()
	FString SaveSlotName;

	/** User Index */
	UPROPERTY()
	uint32 UserIndex;

	/** Plugins Revisions */
	UPROPERTY()
	TMap<FString, int32> PluginsRevisions;
};
