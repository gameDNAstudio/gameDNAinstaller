// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2017 gameDNA Ltd. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "BaseTextLayoutMarshaller.h"
#include "GameDNAinstallerTypes.h"
#include "TextLayout.h"

class FGameDNAinstallerTextLayoutMarshaller : public FBaseTextLayoutMarshaller
{
public:
	virtual ~FGameDNAinstallerTextLayoutMarshaller();

	/** Create */
	static TSharedRef<FGameDNAinstallerTextLayoutMarshaller> Create();
	
	/** Set Text */
	virtual void SetText(const FString& SourceString, FTextLayout& TargetTextLayout) override;

	/** Get Text */
	virtual void GetText(FString& TargetString, const FTextLayout& SourceTextLayout) override;

	/** Count Messages */
	void CountMessages();

	/** Get Num Messages*/
	int32 GetNumMessages() const;

	/** Clear Messages */
	void ClearMessages();

	/** Mark Messages Cache As Dirty */
	void MarkMessagesCacheAsDirty();

	/** Append Message */
	bool AppendMessage(const TCHAR* InText);

protected:
	FGameDNAinstallerTextLayoutMarshaller();

private:
	/** Append Message To Text Layout */
	void AppendMessageToTextLayout(const TSharedPtr<FString>& InMessage);

	/** Append Messages To Text Layout */
	void AppendMessagesToTextLayout(const TArray<TSharedPtr<FString>>& InMessages);

	FTextLayout* TextLayout;
	bool bNumMessagesCacheDirty;
	int32 CachedNumMessages;
	TArray<TSharedPtr<FString>> Messages;
};
