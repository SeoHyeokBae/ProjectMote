// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "../AI/AIInfo.h"
#include "AIController.h"
#include "MonsterController.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API AMonsterController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterController();

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

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn);
	virtual void OnUnPossess();

public:
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnTargetDetect(AActor* Target, FAIStimulus Stimulus);

	UFUNCTION()
	void OnTargetForget(AActor* Target);
};
