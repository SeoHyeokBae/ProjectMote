// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MoteGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UMoteGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMoteGameInstance();

protected:
	FString		mPlayerName = TEXT("Player");
	

public:
	const	FString& GetPlayerName() const { return mPlayerName; }
	void	SetPlayerName(const FString& Name) { mPlayerName = Name; }

public:
	virtual void Init();
};
