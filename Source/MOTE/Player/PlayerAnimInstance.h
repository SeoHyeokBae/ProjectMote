// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APlayerCharacter* mPlayer;

	UPROPERTY(VisibleAnywhere)
	UCharacterMovementComponent* mMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float	mMoveSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float	mMoveDir = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mAcceleration = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsAir = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsJump = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsDash = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsFloat = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsAim = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsDamaged = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsStagger = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsKeyLock = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsStartPose = false;

protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool mAttackCombo = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool mAttackState = false;

	bool mDashEnable = true;

	bool mFloatEnable = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool mIsRagDoll = false;

protected:
	// Dash
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> mDashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName mDashGroundSection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName mDashAirSection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName mDashFloatSection;

	// Attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> mAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> mAttackSectionArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 mCurrentAttackSection = 0;

	// JumpAttack
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> mJumpAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> mJumpAttackSectionArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 mCurrentJumpAttackSection = 0;

	// DashAttack
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> mDashAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName mDashAttackGroundSection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName mDashAttackV2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName mDashAttackAirSection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector mTargetDir;

	// Damaged
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> mDamagedMontage;

public:
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);

	virtual void NativeBeginPlay();

public:
	void OnJump(){ mIsJump = true; }
	bool GetIsJump() const { return mIsJump; }

	bool GetAttackState() const{ return mAttackState; }
	void SetAttackState() { mAttackState = false; }

	void OffDash() { mIsDash = false; }
	bool GetIsDash() const { return mIsDash; }

	void OnDashEnable() { mDashEnable = true; }
	void OffDashEnable() { mDashEnable = false; }
	bool GetDashEnable() const { return mDashEnable; }

	void OnFloat() { mIsFloat = true; }
	void OffFloat() { mIsFloat = false; }
	bool GetIsFloat() const { return mIsFloat; }

	void OnFloatEnable() { mFloatEnable = true; }
	void OffFloatEnable() { mFloatEnable = false; }
	bool GetFloatEnable() const { return mFloatEnable; }

	void OnAim() { mIsAim = true; }
	void OffAim() { mIsAim = false; }
	bool GetIsAim() const { return mIsAim; }

	void OnDamaged() { mIsDamaged = true;	mIsStagger = true;}
	void OffDamaged() { mIsDamaged = false; }
	bool GetIsDamaged() const { return mIsDamaged; }

	void OnStagger() { mIsStagger = true;  }
	void OffStagger() { mIsStagger = false; }
	bool GetIsStagger() const { return mIsStagger; }

	void OnKeyLock() { mIsKeyLock = true;  }
	void OffKeyLock() { mIsKeyLock = false; }
	bool GetIsKeyLock() const { return mIsKeyLock; }

	void OnStartPose() { mIsStartPose = true; }
	bool GetStartPose() const { return mIsStartPose; }

	bool RagDollMode() const { return mIsRagDoll; }

	void ZeroAttack();

	void SetTargetDir(FVector Dir) { mTargetDir = Dir; }

	int32 GetCurrentAttackSection() const {return mCurrentAttackSection;}

public:
	void PlayDashMontage();
	void PlayAttackMontage();
	void PlayDashAttackMontage();
	void PlayDamageMontage();

public:
	UFUNCTION()
	void AnimNotify_DashStart();

	UFUNCTION()
	void AnimNotify_DashEnd();

	UFUNCTION()
	void AnimNotify_AttackComboStart();

	UFUNCTION()
	void AnimNotify_AttackComboEnd();

	UFUNCTION()
	void AnimNotify_JumpAttackComboStart();

	UFUNCTION()
	void AnimNotify_JumpAttackComboEnd();

	UFUNCTION()
	void AnimNotify_DashAttackStart();

	UFUNCTION()
	void AnimNotify_DashAttackEnd();

	UFUNCTION()
	void AnimNotify_OffDamage();

	UFUNCTION()
	void AnimNotify_OnRagDoll();

	UFUNCTION()
	void AnimNotify_OffRagDoll();

	UFUNCTION()
	void AnimNotify_GetUp();

	UFUNCTION()
	void AnimNotify_InitDamage();

	UFUNCTION()
	void AnimNotify_StartFallingPose();

	UFUNCTION()
	void AnimNotify_EndStartPose();
};
