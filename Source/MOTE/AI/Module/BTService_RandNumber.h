// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AIInfo.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RandNumber.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UBTService_RandNumber : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_RandNumber();

public:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
