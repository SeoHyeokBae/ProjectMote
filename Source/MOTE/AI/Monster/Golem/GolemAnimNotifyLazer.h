// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GolemAnimNotifyLazer.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UGolemAnimNotifyLazer : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UParticleSystem* mMuzzleEffect;

	UPROPERTY()
	UParticleSystem* mAttackEffect;

	UPROPERTY()
	UNiagaraSystem* mExplosionEffect;

	UPROPERTY()
	UNiagaraSystem* mHitEffect;

	bool mDamaged = false;

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

};
