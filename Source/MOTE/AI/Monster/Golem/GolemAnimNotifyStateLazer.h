// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GolemAnimNotifyStateLazer.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UGolemAnimNotifyStateLazer : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	UNiagaraSystem* mChargeEffect;
	UPROPERTY()
	UNiagaraSystem* mBeamEffect;
	UPROPERTY()
	UNiagaraSystem* mGroundEffect;

	TWeakObjectPtr<UNiagaraComponent> mBeamComponent;

	FRotator mPrevRot;

	FTimerHandle StatusEffectTimerHandle;
	FTimerHandle StatusEffectEndTimerHandle;

	bool mCanBurn = false;
	float Duration = 2.6f; // 상태이상 지속시간
	float LastSpawnTime = 0.f;
	float SpawnInterval = 0.25f;

private:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
