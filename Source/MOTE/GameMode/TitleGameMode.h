// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/GameModeBase.h"
#include "TitleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API ATitleGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ATitleGameMode();

protected:
	virtual void BeginPlay() override;

};
