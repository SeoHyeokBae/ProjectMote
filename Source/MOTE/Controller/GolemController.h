// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AI/AIInfo.h"
#include "AIController.h"
#include "GolemController.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API AGolemController : public AAIController
{
	GENERATED_BODY()

public:
	AGolemController();

protected:
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* mAIPerception;

	UAISenseConfig_Sight* mSightConfing;
	UAISenseConfig_Damage* mDamageConfig;

	UBehaviorTree* mAITree;
	UBlackboardData* mBlackBoard;

	uint8	mTeamID = 10;

public:
	void SetTeamID(uint8 ID)
	{
		mTeamID = ID;
	}

	virtual FGenericTeamId GetGenericTeamId() const;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;
	

private:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn)override;
	virtual void OnUnPossess()override;

public:
	virtual void Tick(float DeltaTime) override;
	
private:
	UFUNCTION()
	void GolemOnTargetDetect(AActor* Target, FAIStimulus Stimulus);

	UFUNCTION()
	void GolemOnTargetForget(AActor* Target);
};
