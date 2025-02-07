// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifySkillCrash.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UAnimNotifySkillCrash : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UParticleSystem* mFistEffect;

	UPROPERTY()
	UParticleSystem* mAttackEffect;

	UPROPERTY()
	UParticleSystem* mGroundEffect;

	UPROPERTY()
	UNiagaraSystem* mHitEffect;

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

private:
	void DrawTelegraph(USkeletalMeshComponent* MeshComp, class APlayerCharacter* Player, class AGolem* Golem);
};
