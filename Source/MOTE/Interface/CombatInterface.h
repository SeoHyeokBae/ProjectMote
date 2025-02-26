// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOTE_API ICombatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void HitInteraction() = 0;
	UFUNCTION()
	virtual void HitStopStart() = 0;
	UFUNCTION()
	virtual void HitStopEnd() = 0;
};
