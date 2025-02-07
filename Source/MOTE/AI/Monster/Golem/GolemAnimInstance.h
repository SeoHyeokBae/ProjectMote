// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "Animation/AnimInstance.h"
#include "GolemAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EGolemAnim : uint8
{
	None,
	Starting,
	Idle,
	Attack,
	Skill_Crash,
	Skill_Crarge,
	Skill_Lazer,
	Skill_Lazer_Charge,
	Skill_Meteor,
	Casting,
	Stagger,
	Death
};

UCLASS()
class MOTE_API UGolemAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	UGolemAnimInstance();

protected:
	UPROPERTY(VisibleAnywhere)
	class AGolem* mOwner;

	UPROPERTY(VisibleAnywhere)
	UCharacterMovementComponent* mMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<class UCameraShakeBase >> mCameraShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator mLookDir = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EGolemAnim	mGolemAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool mIsStarting = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool mIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool mIsDeadStart = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool mIsDeathEnd = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool mCanSpawnMeteor = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> mStaggerMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USoundSubsystem* mBGM;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USoundSubsystem* mDeathBGM;

public:
	UFUNCTION(BlueprintCallable)
	bool GetIsDeathEnd() { return mIsDeathEnd; }
	UFUNCTION(BlueprintCallable)
	void SetIsDeathEnd() { mIsDeathEnd = false; }

	UFUNCTION(BlueprintCallable)
	bool GetIsDeathStart() { return mIsDeadStart; }
	UFUNCTION(BlueprintCallable)
	void SetIsDeathStart() { mIsDeadStart = false; }

	bool GetCanSpawnMeteor() { return mCanSpawnMeteor; }
	void SetSpawnMeteor() { mCanSpawnMeteor = false; }

public:
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);

	virtual void NativeBeginPlay();

public:
	void ChangeAnim(EGolemAnim Anim)
	{
		mGolemAnim = Anim;
	}

	const TSubclassOf<class UCameraShakeBase> GetGolemShakeBase(int32 ShakeType) const { return mCameraShake[ShakeType]; }

	void PlayStaggerMontage(bool Play = true);
public:
	UFUNCTION()
	void AnimNotify_Shake();
	UFUNCTION()
	void AnimNotify_Starting();
	UFUNCTION()
	void AnimNotify_StartingEnd();
	UFUNCTION()
	void AnimNotify_StateStart();
	UFUNCTION()
	void AnimNotify_StateEnd();
	UFUNCTION()
	void AnimNotify_AttackEnd();
	UFUNCTION()
	void AnimNotify_SpawnMeteor();
	UFUNCTION()
	void AnimNotify_GolemDeathEnd();
	UFUNCTION()
	void AnimNotify_GolemDeathStart();
	UFUNCTION()
	void AnimNotify_OnBGM();
};
