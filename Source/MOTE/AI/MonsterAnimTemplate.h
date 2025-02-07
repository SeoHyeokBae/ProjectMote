// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIInfo.h"
#include "../GameInfo.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimTemplate.generated.h"

UENUM(BlueprintType)
enum class EMonsterAnimState : uint8
{
	Idle,
	Walk,
	Run,
	Attack,
	Death,
	Max
};

UCLASS()
class MOTE_API UMonsterAnimTemplate : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UMonsterAnimTemplate();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMonsterAnimState mMonsterAnim = EMonsterAnimState::Max;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UAnimSequence*>	mSequenceMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UBlendSpace*>		mBlendSpaceMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, UAnimMontage*>	mMontageMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float	mHitAlpha = 0.f;


	bool mIsDeathEnd = false;

public:
	void ChangeAnim(EMonsterAnimState Anim)
	{
		mMonsterAnim = Anim;
	}

	void SetAnimData(const FName& Key);

	UFUNCTION(BlueprintCallable)
	bool GetIsDeathEnd() { return mIsDeathEnd; }
	UFUNCTION(BlueprintCallable)
	void SetIsDeathEnd() { mIsDeathEnd = false; }

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void NativeBeginPlay();

	void PlayMontage(const FString& Name);
public:
	UFUNCTION()
	void AnimNotify_Attack();

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_HitEnd();

	UFUNCTION()
	void AnimNotify_DeathEnd();
};
