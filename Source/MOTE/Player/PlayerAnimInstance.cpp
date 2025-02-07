// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "Player/PlayerTrailFX.h"
#include "Player/MotePlayerState.h"
#include "Controller/MotePlayerController.h"
#include "../UI/Main/MainWidget.h"


void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	mPlayer = Cast<APlayerCharacter>(TryGetPawnOwner());

	if(mPlayer)
		mMovement = mPlayer->GetCharacterMovement();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds); 

	if(!mIsStartPose)
		mIsStagger ? mIsKeyLock = true : mIsKeyLock = false;
		
	if (mMovement)
	{
		mMoveSpeed = mMovement->Velocity.Length();

		mAcceleration = mMovement->GetCurrentAcceleration().Length() > 0.f;

		// �ö󰡴� ���߿��� true������
		mIsAir = mMovement->IsFalling();

		// ���Ͻ� Jump false
		if (mMovement->Velocity.Z < 0)
			mIsJump = false;

	}
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UPlayerAnimInstance::PlayDashMontage()
{
	if (!IsValid(mDashMontage))
		return;

	if (mIsAir || mIsJump)
	{
		if (!mIsDash)
		{
			if (!Montage_IsPlaying(mDashMontage))
			{
				if (mIsFloat) // �÷��� �뽬
				{
					Montage_SetPosition(mDashMontage, 0.f);
					Montage_Play(mDashMontage);
					Montage_JumpToSection(mDashFloatSection);
				}

				else // ���� �뽬
				{
					Montage_SetPosition(mDashMontage, 0.f);
					Montage_Play(mDashMontage);
					Montage_JumpToSection(mDashAirSection);
				}
			}
		}
	}

	else
	{
		if (!mIsDash) // �׶��� �뽬
		{
			if (!Montage_IsPlaying(mDashMontage))
			{
				// ���� ������ 0
				mMovement->BrakingFrictionFactor = 0.f;
				Montage_SetPosition(mDashMontage, 0.f);
				Montage_Play(mDashMontage);
				Montage_JumpToSection(mDashGroundSection);
			}
		}
	}
}

void UPlayerAnimInstance::AnimNotify_DashStart()
{
	mIsDash = true; 

	if (mIsFloat)
	{
		FVector Dir = mTargetDir;

		if (mIsAim && mTargetDir != FVector::Zero()) // ���ػ����̰� ���Ӱ��ɻ���
		{
			mPlayer->LaunchCharacter(Dir * 50000.f, true, true);
			mMovement->GravityScale = 0.f;
		}
		else // ���ӻ��°� �ƴϰų� ���ӺҰ��϶� ĳ���� �������� �Ϲ� �뽬
		{
			if(mIsAim)
				mPlayer->ZoomOut();

			mPlayer->LaunchCharacter(mPlayer->GetActorForwardVector() * 2500.f, true, true);
			mMovement->GravityScale = 0.f;
		}

		mPlayer->GetWorldSettings()->SetTimeDilation(1.f);
	}
	else
	{
		mPlayer->LaunchCharacter(mPlayer->GetActorForwardVector() * 2000.f, true, true);

		if (mIsJump)
		{
			mDashEnable = false;
			//mMovement->GravityScale = 0.f;
		}
	}
}

void UPlayerAnimInstance::AnimNotify_DashEnd()
{
	mIsDash = false;
	// �÷��� �뽬 �߷� ������
	mMovement->GravityScale = 3.f;
	// �׶��� �뽬 ������ ������
	mMovement->BrakingFrictionFactor = 2.f;

	if (Montage_IsPlaying(mDashAttackMontage) && mAcceleration)
	{
		Montage_Stop(0.1f, mDashMontage);
	}
}

void UPlayerAnimInstance::PlayAttackMontage()
{
	if (!IsValid(mAttackMontage) || !IsValid(mJumpAttackMontage))
		return;
	

	if (!mAttackState && !Montage_IsPlaying(mAttackMontage) && !Montage_IsPlaying(mJumpAttackMontage))
	{
		if (mMovement->IsMovingOnGround())
		{
			Montage_Play(mAttackMontage);
			Montage_JumpToSection(mAttackSectionArray[mCurrentAttackSection]);
			mAttackState = true;
		}
		else
		{
			Montage_Play(mJumpAttackMontage);
			Montage_JumpToSection(mJumpAttackSectionArray[mCurrentJumpAttackSection]);
			mAttackState = true;
		}
	}
	else
	{
		//if (mMovement->IsMovingOnGround())
			mAttackCombo = true;
	}
}

void UPlayerAnimInstance::PlayDashAttackMontage()
{
	if (mAttackState)
		return;

	Montage_SetPosition(mDashAttackMontage, 0.f);
	Montage_Play(mDashAttackMontage);

	if (mPlayer->GetCharacterMovement()->IsMovingOnGround())
	{
		Montage_JumpToSection(mDashAttackV2);
	}
	else
	{
		Montage_JumpToSection(mDashAttackV2);
	}

	mAttackState = true;
}

void UPlayerAnimInstance::PlayDamageMontage()
{
	Montage_SetPosition(mDamagedMontage, 0.f);
	Montage_Play(mDamagedMontage);
}

void UPlayerAnimInstance::AnimNotify_AttackComboStart()
{
	if (mAttackCombo)
	{
		mCurrentAttackSection = (mCurrentAttackSection + 1) % mAttackSectionArray.Num();

		Montage_Play(mAttackMontage);
		Montage_JumpToSection(mAttackSectionArray[mCurrentAttackSection]);
		mAttackCombo = false;
	}

}

void UPlayerAnimInstance::AnimNotify_AttackComboEnd()
{
	// �������� ���� ����
	mPlayer->GripWeapon(false);

	mAttackCombo = false;
	mAttackState = false;
	mCurrentAttackSection = 0;

	if (Montage_IsPlaying(mAttackMontage) && mAcceleration)
	{
		Montage_Stop(0.1f, mAttackMontage);
	}
}

void UPlayerAnimInstance::AnimNotify_JumpAttackComboStart()
{
	if (mAttackCombo)
	{
		mMovement->SetMovementMode(MOVE_Flying);
		mCurrentJumpAttackSection = (mCurrentJumpAttackSection + 1) % mJumpAttackSectionArray.Num();

		Montage_Play(mJumpAttackMontage);
		Montage_JumpToSection(mJumpAttackSectionArray[mCurrentJumpAttackSection]);
		
		mAttackCombo = false;
	}
}

void UPlayerAnimInstance::AnimNotify_JumpAttackComboEnd()
{	// �������� ���� ����
	mPlayer->GripWeapon(false);
	
	mMovement->SetMovementMode(MOVE_Falling);

	mAttackCombo = false;
	mAttackState = false;
	mCurrentJumpAttackSection = 0;
}

void UPlayerAnimInstance::AnimNotify_DashAttackStart()
{
	if (mIsFloat)
		mMovement->GravityScale = 3.f;
}

void UPlayerAnimInstance::AnimNotify_DashAttackEnd()
{
	mPlayer->GripWeapon(false);
	mIsDash = false;
	mAttackState = false;
	// �÷��� �뽬 �߷� ������
	mMovement->GravityScale = 3.f;
	// �׶��� �뽬 ������ ������
	mMovement->BrakingFrictionFactor = 2.f;

	if (Montage_IsPlaying(mDashAttackMontage) && mAcceleration)
	{
		Montage_Stop(0.15f, mDashAttackMontage);
	}
}

void UPlayerAnimInstance::AnimNotify_OffDamage()
{

	mIsDamaged = false;
}

void UPlayerAnimInstance::AnimNotify_OnRagDoll()
{
	mIsRagDoll = true;
	FVector CurrentLaunchVelocity = mMovement->Velocity;

	mPlayer->GetMesh()->SetSimulatePhysics(true);
}

void UPlayerAnimInstance::AnimNotify_OffRagDoll()
{
	mIsRagDoll = false;


	//mPlayer->GetMesh()->AttachToComponent(mPlayer->GetCapsuleComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	mPlayer->GetMesh()->AttachToComponent(mPlayer->GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
	mPlayer->GetMesh()->SetRelativeLocationAndRotation(FVector(0.0, 0.0, -93.0), FRotator(0.0, -90.0, 0.0), false, nullptr, ETeleportType::TeleportPhysics);
	mPlayer->GetMesh()->SetSimulatePhysics(false);
}

void UPlayerAnimInstance::AnimNotify_GetUp()
{
	// ��������
	mPlayer->SetActorEnableCollision(false);
	mPlayer->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	//mPlayer->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}

void UPlayerAnimInstance::AnimNotify_InitDamage()
{
	if (!mIsStagger && !mIsDamaged) return;
	mPlayer->SetActorEnableCollision(true);
	// mPlayer->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mIsStagger = false;
	mIsDamaged = false;
}

void UPlayerAnimInstance::AnimNotify_StartFallingPose()
{
	mIsKeyLock = true;
	mPlayer->LaunchCharacter(mPlayer->GetActorUpVector() * -150000.f, true, true);
	mPlayer->GetTrailFX()->OnFloatActive();
}

void UPlayerAnimInstance::AnimNotify_EndStartPose()
{
	mIsStartPose = false;
	mIsKeyLock = false;

	// Init Starting HP
	if (mPlayer->GetPlayerState<AMotePlayerState>())
		mPlayer->GetPlayerState<AMotePlayerState>()->SetHP(70.f, false, true);

	AMotePlayerController* Control = mPlayer->GetController<AMotePlayerController>();
	if (Control)
	{
		UMainWidget* MainWidget = Control->GetMainWidget();
		if (MainWidget)
			MainWidget->AppearWidget();
	}
}


void UPlayerAnimInstance::ZeroAttack()
{
	// AnimNotify_AttackComboEnd �� �ߺ��Ǵ°� ����,
	Montage_Stop(0.f, mAttackMontage);
	mPlayer->GripWeapon(false);
	mAttackCombo = false;
	mAttackState = false;
	mCurrentAttackSection = 0;

}
