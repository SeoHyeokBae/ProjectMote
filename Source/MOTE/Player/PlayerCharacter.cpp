// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "PlayerAnimInstance.h"
#include "MotePlayerState.h"
#include "Camera/CameraShakeBase.h"
#include "BaseWeapon.h"

#include "../Controller/InputData.h"
#include "../Controller/MotePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Components/TimelineComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/DecalComponent.h"
#include "Interface/CustomGravityComponent.h"

#include "PlayerGhostTrail.h"
#include "PlayerTrailFX.h"
#include "Player/TargetSystemComponent.h"
#include "../UI/Main/PlayerStaminaWidget.h"
#include "../UI/ETC/DmgTextActor.h"

#include "Controller/MotePlayerController.h"
#include "../Sound/SoundSubsystem.h"

#include "../UI/Main/MainWidget.h"
#include "../UI/Main/PlayerKeyGuideWidget.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Create();
	Load();
	Init();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// AnimInstance
	mAnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	// Player State
	mState = GetPlayerState<AMotePlayerState>();
	
	mStaminaWidget = Cast<UPlayerStaminaWidget>(mStaminaComponent->GetWidget());

	checkf(IsValid(mState), TEXT("Not Exist Player State"));

	if (IsValid(mState))
		mState->AddHPDelegate<APlayerCharacter>(this, &APlayerCharacter::LowHelthIndicate);

	// PlayerCamera Edit
	mCameraRotator = mArm->GetRelativeRotation();

	if (mCameraZoomCurve)
	{
		mTargetArmLength = mArm->TargetArmLength;
		mSocketOffset = mArm->SocketOffset;
		mFieldOfView = mCamera->FieldOfView;

		// ZoomIn Deligated Bind and Apply Curve
		mCameraZoomHandler.BindDynamic(this, &ThisClass::ZoomInByAiming);
		mZoomComponent->AddInterpFloat(mCameraZoomCurve, mCameraZoomHandler);

		mAimDecalComponent->SetVisibility(false);
	}

	// PlayerController Edit
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem 
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetController<APlayerController>()->GetLocalPlayer());

		//get CDO
		const UDefaultInputData* InputData = GetDefault <UDefaultInputData>();

		Subsystem->AddMappingContext(InputData->mDefaultContext, 0);
	}

	// Weapon Spawn
	if (!mWeapon)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		mWeapon = GetWorld()->SpawnActor<ABaseWeapon>(ABaseWeapon::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName("WeaponCaseSocket2");
		
		if (WeaponSocket && mWeapon)
			WeaponSocket->AttachActor(mWeapon, GetMesh());
	}

	// DashTrail Actor Spawn
	if (!IsValid(mTrailFX))
	{
		mTrailFX = GetWorld()->SpawnActor<APlayerTrailFX>(APlayerTrailFX::StaticClass());
		mTrailFX->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		mTrailFX->SetOwner(this);
	}

	// Collision Event
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APlayerCharacter::DashOnHit);

	mScreenEffect = Cast<APostProcessVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass()));
	if (mScreenEffect)
	{
		mScreenEffect->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(0.f, mScreenDamageDynamic));
		mScreenEffect->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(0.f, mScreenDamageFireDynamic));
		mScreenEffect->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(0.f, mScreenDashAttackDynamic));
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mSpawnParticle, GetActorLocation() - GetActorForwardVector() *10.f, FRotator::ZeroRotator, FVector(1.5, 1.5, 1.5), true);

	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
	{
#if WITH_EDITOR
		FString MapName = GetWorld()->GetMapName();  // 에디터 모드에서 실행 중일 때
		if (MapName == "UEDPIE_0_Tutorial")
#else
		FString MapName = GetWorld()->GetMapName();  // 패키징된 게임에서 실행 중일 때
		if (MapName == "Tutorial")
#endif
		{
			// 튜토리얼 레벨
			AMotePlayerController * Control = GetController<AMotePlayerController>();
			UMainWidget* MainWidget = Control->GetMainWidget();
			if (MainWidget)
			{
				mGuide = true;
				MainWidget->ShowGuide();
			}

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, SoundSubsystem]()
				{
					SoundSubsystem->PlayVFXSound(TEXT("SpawnTutorial"), 0.5f, GetActorLocation());
				}, 1.0f, false);
		}
#if WITH_EDITOR
		else if (MapName == "UEDPIE_0_BossPlanet")
#else
		else if (MapName == "BossPlanet")
#endif
		{
			// 보스레벨
			SoundSubsystem->PlayVFXSound(TEXT("PlayerSpawn"), 0.75f, GetActorLocation());

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					AMotePlayerController* Control = GetController<AMotePlayerController>();
					UMainWidget* MainWidget = Control->GetMainWidget();
					if(MainWidget) 
						MainWidget->AppearWidget();

					ExecuteLockOnAction();
				}, 17.5f, false);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateStamina(DeltaTime);
	UpdateCameraView(DeltaTime);
	
	if (GetMesh()->IsSimulatingPhysics())
	{
		FVector NewLocation = GetMesh()->GetComponentLocation();
		GetCapsuleComponent()->SetWorldLocation(NewLocation + GetMesh()->GetUpVector() * 93.f);
		GetMesh()->SetEnableGravity(false);
		GetMesh()->AddForce(GetGravityDirection() * 100000.f);
	}


}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputCom = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputCom)
	{
		const UDefaultInputData* InputData = GetDefault<UDefaultInputData>();

		// Move
		EnhancedInputCom->BindAction(InputData->mMove, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveAction);
		EnhancedInputCom->BindAction(InputData->mRotation, ETriggerEvent::Triggered, this, &APlayerCharacter::RotationAction);

		// Act
		EnhancedInputCom->BindAction(InputData->mAttack, ETriggerEvent::Started, this, &APlayerCharacter::AttackAction);
		EnhancedInputCom->BindAction(InputData->mDash, ETriggerEvent::Started, this, &APlayerCharacter::DashAction);

		// Jump & Float
		EnhancedInputCom->BindAction(InputData->mJump, ETriggerEvent::Ongoing, this, &APlayerCharacter::JumpStartAction);
		EnhancedInputCom->BindAction(InputData->mJump, ETriggerEvent::Completed, this, &APlayerCharacter::FloatAction);
		EnhancedInputCom->BindAction(InputData->mJump, ETriggerEvent::Canceled, this, &APlayerCharacter::JumpAction);

		// Aim
		EnhancedInputCom->BindAction(InputData->mAim, ETriggerEvent::Ongoing, this, &APlayerCharacter::AimAction);
		EnhancedInputCom->BindAction(InputData->mAim, ETriggerEvent::Canceled, this, &APlayerCharacter::AimActionCanceled);
		EnhancedInputCom->BindAction(InputData->mAim, ETriggerEvent::Completed, this, &APlayerCharacter::AimActionCompleted);

		// Detected
		EnhancedInputCom->BindAction(InputData->mDetected, ETriggerEvent::Started, this, &APlayerCharacter::ExecuteLockOnAction);

		// 
		EnhancedInputCom->BindAction(InputData->mBossKill, ETriggerEvent::Started, this, &APlayerCharacter::BossKill);

		// Show Guide
		EnhancedInputCom->BindAction(InputData->mGuide, ETriggerEvent::Started, this, &APlayerCharacter::ShowGuide);

		// Interaction
		EnhancedInputCom->BindAction(InputData->mInteraction, ETriggerEvent::Started, this, &APlayerCharacter::PressInteraction);
		
		// Pause
		EnhancedInputCom->BindAction(InputData->mPause, ETriggerEvent::Started, this, &APlayerCharacter::PressPause);

		// Respawn
		EnhancedInputCom->BindAction(InputData->mRespawn, ETriggerEvent::Started, this, &APlayerCharacter::Respawn);
	}
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (mGravityComponent)
	{
		mGravityComponent->mPlayer = this;
	}
}

void APlayerCharacter::MoveAction(const FInputActionValue& Value)
{
	// 시작 포즈중엔 키입력x
	if (mAnimInstance->GetIsKeyLock()) return;
	
	FVector Axis = Value.Get<FVector>();

	/*FVector Forward = UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0));
	FVector Right = UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0));

	AddMovementInput(Forward, Axis.Y);
	AddMovementInput(Right, Axis.X);*/

	FRotator GravityRelativeRotation = AMotePlayerController::GetGravityRelativeRotation(GetControlRotation(), GetCharacterMovement()->GetGravityDirection());

	FVector Right = UKismetMathLibrary::GetRightVector(
		Cast<AMotePlayerController>(GetController())->GetGravityWorldRotation(FRotator(0, GravityRelativeRotation.Yaw, GravityRelativeRotation.Roll)
			, GetCharacterMovement()->GetGravityDirection()));

	FVector Forward = UKismetMathLibrary::GetForwardVector(
		Cast<AMotePlayerController>(GetController())->GetGravityWorldRotation(FRotator(0, GravityRelativeRotation.Yaw, 0)
			, GetCharacterMovement()->GetGravityDirection()));

	if ((GetCharacterMovement()->GetGravityDirection()).GetSafeNormal().Z > 0)
		Axis *= -1;

	AddMovementInput(Right, Axis.X);
	AddMovementInput(Forward, Axis.Y);
}

void APlayerCharacter::RotationAction(const FInputActionValue& Value)
{
	FVector Axis = Value.Get<FVector>();

	abs(Axis.Y) > 3.f ? mIsLookUpCamera = true : mIsLookUpCamera = false;
	abs(Axis.X) > 3.f ? mIsTrunCamera = true : mIsTrunCamera = false;

	float PitchDelta = Axis.Y * 45.0 * GetWorld()->GetDeltaSeconds();
	float YawDelta = Axis.X * 45.0 * GetWorld()->GetDeltaSeconds();

	mCameraRotator.Pitch += PitchDelta;
	mCameraRotator.Yaw += YawDelta;

	
	if (mCameraRotator.Pitch < -85.f)
		mCameraRotator.Pitch = -85.f;
	
	else if (mCameraRotator.Pitch > 85.f)
		mCameraRotator.Pitch = 85.f;

	if(abs(Axis.X) > 0.01)
		mLastTargetSystemDir = Axis.X;

	// Lock-On 중일떄 카메라 고정, 아래 들어오면 안됨
	if (!mTargetingSystem->IsLockOnTarget() || mTargetingSystem->IsDynamicLockOnTarget())
	{
		mArm->SetRelativeRotation(mCameraRotator);
		GetController()->SetControlRotation(FRotator(0, mCameraRotator.Yaw, 0));
	}

		
}

void APlayerCharacter::AttackAction()
{
	if (mAnimInstance->GetIsKeyLock() || mAnimInstance->GetIsStagger()) return;

	if (mAnimInstance->GetIsDash())
	{
		DashAttack();
	}

	else
	{
		NormalAttack();	
	}

}

void APlayerCharacter::DashAction()
{
	if (mAnimInstance->GetIsKeyLock() || mAnimInstance->GetIsStagger()) return;

	if (!mAnimInstance->GetDashEnable())
		return;

	if (mAnimInstance->GetAttackState())
		mAnimInstance->ZeroAttack();
	
	mIsZoom = false;
	mAnimInstance->PlayDashMontage();

	// OffAim 때문에 플레이 몽타주 보다 아래
	if (mAnimInstance->GetIsFloat())
	{
		GetWorldSettings()->SetTimeDilation(1.f);
		mAnimInstance->OffAim();
	}

	// GhostTrail spawn
	GetWorld()->SpawnActor<APlayerGhostTrail>(APlayerGhostTrail::StaticClass()
		, GetActorLocation(), GetActorRotation())->Init(GetMesh());

	// TrailFX Active
	if (mAnimInstance->GetIsFloat())
		mTrailFX->OnFloatActive();
	else
		mTrailFX->OnDashActive();

	mState->SetStamina(DashValue);
}

void APlayerCharacter::JumpStartAction()
{
	if (mAnimInstance->GetIsKeyLock() || mAnimInstance->GetIsStagger()) return;

	// OnGoing
}

void APlayerCharacter::JumpAction()
{
	if (mAnimInstance->GetIsKeyLock() || mAnimInstance->GetIsStagger()) return;

	// 부유 중이 아닐 때
	if (!(mAnimInstance->GetIsFloat()) && !(mAnimInstance->GetAttackState()))
	{
		// 점프 중이 아닐 때 점프
		if (!(mAnimInstance->GetIsJump()) && !GetCharacterMovement()->IsFalling())
		{
			LaunchCharacter(GetActorUpVector() * 1450.f, false, true);
			mAnimInstance->OnJump();
		}
	}
}

void APlayerCharacter::FloatAction()
{
	if (mAnimInstance->GetIsKeyLock() || mAnimInstance->GetIsStagger() || mAnimInstance->GetIsDamaged() || !mAnimInstance->GetFloatEnable()) return;

	// 부유 중이 아닐 때
	if (!(mAnimInstance->GetIsFloat()))
	{
		mState->SetStamina(FloatValue);
		LaunchCharacter(GetActorUpVector() * 5000.f, false, true);
		mAnimInstance->OnFloat();
		mTrailFX->OnFloatActive();
	}
}

void APlayerCharacter::AimAction()
{
	if (mAnimInstance->GetIsStagger() || mAnimInstance->GetIsDamaged() || !mAnimInstance->GetIsFloat() || !mAnimInstance->GetDashEnable()) return;

	if (!IsZoomSound)
	{
		IsZoomSound = true;
		USoundSubsystem* SkillSFX = GetGameInstance()->GetSubsystem<USoundSubsystem>();
		if (SkillSFX)
		{
			SkillSFX->PlayVFXSound(TEXT("SkillSFX"), 0.7f, GetActorLocation());
		}
	}

	FRotator NewRotation
		= FMath::RInterpTo(GetActorRotation(), FRotator(0, mCameraRotator.Yaw, 0), GetWorld()->DeltaTimeSeconds, 20.f);
	SetActorRotation(NewRotation);

	if (mIsZoom && !mAnimInstance->GetIsDash())
	{
		
		// 조준시 블러
		mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::DashAttack].Weight = 1.f;

		FHitResult HitResult;

		bool bHit = GetController<APlayerController>()->GetHitResultUnderCursorByChannel
		(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel5), true, HitResult);

		if (bHit)
		{
			mAimDecalComponent->SetVisibility(true);
			mAimDecalComponent->SetMaterial(0, mAimMtrlInstDynamic);

			// 조준한 곳에 몬스터가 위치할시 Red색상
			AAIMonsterBase* HitActor = Cast<AAIMonsterBase>(HitResult.GetActor());
			HitActor != nullptr ? mAimMtrlInstDynamic->SetVectorParameterValue(TEXT("Color"), FVector4(100.0, 1.0, 1.0, 1.0))
								: mAimMtrlInstDynamic->SetVectorParameterValue(TEXT("Color"), FVector4(1.0, 1.0, 100.0, 1.0));

			FVector CursorLocation = HitResult.Location;
			FRotator CursorRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();

			mAimDecalComponent->SetWorldLocationAndRotation(CursorLocation, CursorRotation);

			mAnimInstance->SetTargetDir((CursorLocation - GetActorLocation()).GetSafeNormal());
		}
		else
		{
			mAnimInstance->SetTargetDir(FVector::Zero());
		}
	}

	// Aim
	if (!(mAnimInstance->GetIsAim()))
	{
		ZoomIn();
		GetWorldSettings()->SetTimeDilation(0.1f);
		mAnimInstance->OnAim();
	}

}

void APlayerCharacter::AimActionCanceled()
{
	if (!mAnimInstance->GetIsFloat())
		return;
	
	mAimDecalComponent->SetVisibility(false);
	
	ZoomOut();
	GetWorldSettings()->SetTimeDilation(1.f);
	mAnimInstance->OffAim();

	// 블러해제
	mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::DashAttack].Weight = 0.f;
	IsZoomSound = false;
}

void APlayerCharacter::AimActionCompleted()
{
	if (!mAnimInstance->GetIsFloat())
		return;
	
	mAimDecalComponent->SetVisibility(false);

	ZoomOut();
	GetWorldSettings()->SetTimeDilation(1.f);
	mAnimInstance->OffAim();

	// 블러해제
	mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::DashAttack].Weight = 0.f;
	IsZoomSound = false;
}

void APlayerCharacter::ExecuteLockOnAction()
{
	if (!mTargetingSystem->IsLockOnTarget())
	{
		if (!mTargetingSystem->GetTarget())
		{
			mTargetingSystem->ExecuteLockOnTarget();

			// 타켓이 골렘일경우 바로 DynamicLock-On
			if (mTargetingSystem->IsLockOnTarget())
			{
				AAIMonsterBase* Monster = Cast<AAIMonsterBase>(mTargetingSystem->GetTarget());
				if (Monster->mMonsterInfoKey == FName(TEXT("Golem")))
					mTargetingSystem->SetDynamicLockOnTarget(true);
			}
		}
		else
		{
			AAIMonsterBase* Monster = Cast<AAIMonsterBase>(mTargetingSystem->GetTarget());
			if (Monster->mMonsterInfoKey == FName(TEXT("Golem")))
				mTargetingSystem->HoldTargetLockOn();
		}
	}
	// 락온된 타켓이 있을 경우
	else
	{
		mIsReadyChangeTarget = true;
		AAIMonsterBase* Monster = Cast<AAIMonsterBase>(mTargetingSystem->GetTarget());
		if (Monster->mMonsterInfoKey == FName(TEXT("Golem")))
		{
			mTargetingSystem->CancelLockOnTarget();
			//mTargetingSystem->HoldTargetLockOff();
		}
		else if (mIsReadyChangeTarget && mLastTargetSystemDir != 0.0f)
		{
				mIsReadyChangeTarget = false;
				mTargetingSystem->ChangeLockOnTargetForTurnValue(mLastTargetSystemDir);
		}
		else
		{
			mTargetingSystem->CancelLockOnTarget();
		}
	}
}

void APlayerCharacter::BossKill()
{
	if (!mTargetingSystem->GetTarget()) return;
	//FDamageEvent	DmgEvent;
	//mTargetingSystem->GetTarget()->TakeDamage(100, DmgEvent, GetController(), this);
	mState->SetHP(10.f);
}

void APlayerCharacter::ShowGuide()
{
	AMotePlayerController* Control = GetController<AMotePlayerController>();
	UMainWidget* MainWidget = Control->GetMainWidget();
	if (MainWidget)
	{
		if (mGuide)
		{
			mGuide = false;
			MainWidget->HideGuide();
		}
		else
		{
			mGuide = true;
			MainWidget->ShowGuide();
		}
	}
}

void APlayerCharacter::PressInteraction()
{
	if (!mCanInteraction) 
	{ 
		return; 
	}

	mPressedInteraction = true;

}

void APlayerCharacter::PressPause()
{
	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	AMotePlayerController* Control = GetController<AMotePlayerController>();
	
	mIsPuase = true;
	if (SoundSubsystem && GetWorldSettings() && Control)
	{
		SoundSubsystem->ResumeBGM(false);
		SoundSubsystem->SetPauseBGM(mIsPuase);
		GetWorldSettings()->SetTimeDilation(0.f);
		Control->VisiblePauseWidget();
	}
}

void APlayerCharacter::Respawn()
{
#if WITH_EDITOR
	FString MapName = GetWorld()->GetMapName();  // 에디터 모드에서 실행 중일 때
	if (MapName == "UEDPIE_0_BossPlanet")
#else
	FString MapName = GetWorld()->GetMapName();  // 패키징된 게임에서 실행 중일 때
	if (MapName == "BossPlanet")
#endif
	{
		USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
		if (mSpawnParticle && SoundSubsystem)
		{
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			SetActorLocation(FVector(-4000.f, 0.f, 29600.f));
			SoundSubsystem->PlayVFXSound(TEXT("PlayerSpawn"), 0.75f, GetActorLocation());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mSpawnParticle, GetActorLocation() - GetActorForwardVector() * 10.f, FRotator::ZeroRotator, FVector(1.5, 1.5, 1.5), true);
		}
	}
}

void APlayerCharacter::LowHelthIndicate(float Percent)
{
	if (Percent <= 0.2f && !IsScreenDamage())
	{
		mIsLowHelth = true;
		SetScreenDamage(true);
	}
	else if (Percent > 0.2f && mIsLowHelth && IsScreenDamage())
	{
		mIsLowHelth = false;
		SetScreenDamage(false);
	}
}

void APlayerCharacter::NormalAttack()
{
	mAnimInstance->PlayAttackMontage();

	if (mAnimInstance->GetAttackState())
	{
		GripWeapon(true);
	}
	/*if (Collision)
		mTrailFX->OnAttackActive();*/
}

void APlayerCharacter::DashAttack()
{
	GripWeapon(true);
	mAnimInstance->PlayDashAttackMontage();
}

void APlayerCharacter::FloatDashAttack()
{
}

void APlayerCharacter::UpdateStamina(float DeltaTime)
{
	// None State: Recover Stamina
	if (!(mAnimInstance->GetIsDash() || mAnimInstance->GetIsFloat()) && !mState->GetMaxStamina())
	{
		mState->SetStamina(RecoveryStaminaValue * DeltaTime, true);
	}

	 //Float State: Reduce Stamina
	if (mAnimInstance->GetIsFloat())
	{
		mState->SetStamina(FloatLoopValue * DeltaTime);
	}

	// Disable Dash
	if (mState->GetStamina() < DashValue)
	{
		mAnimInstance->OffDashEnable();
	}

	// Keep the Limit to one JumpDash
	else if(!mAnimInstance->GetDashEnable() && mState->GetStamina() >= DashValue 
		&& !mAnimInstance->GetIsFloat() && !GetCharacterMovement()->IsFalling())
	{
		mAnimInstance->OnDashEnable();
	}

	// Disable Float
	if (mState->GetStamina() < FloatValue)
	{
		// Zoom Cancled
		if (mIsZoom)
			AimActionCanceled();

		mAnimInstance->OffFloatEnable();
	}
	else
	{
		mAnimInstance->OnFloatEnable();
	}
}


void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (mAnimInstance->GetStartPose())
	{
		mArm->CameraLagSpeed = 10.0f;
		GetController<APlayerController>()->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EShakeType::FloatDash]);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Hit]()
			{
				USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
				if (SoundSubsystem)
					SoundSubsystem->PlayVFXSound(TEXT("SpawnTutorialAfter"), 0.25f, Hit.ImpactPoint);
			}, 1.15f, false);
	}

	if (mIsZoom)
	{
		ZoomOut();
		if (mAnimInstance->GetIsDash())
		{
			GetCharacterMovement()->StopMovementImmediately();

			// Create Collision
			TArray<FHitResult> result;

			FVector Start = GetActorLocation();
			FVector End = Start;

			FCollisionShape CollisionShape;
			CollisionShape.SetSphere(300.f);

			FCollisionQueryParams param;
			param.AddIgnoredActor(this);

			TArray<FHitResult> HitResults;
			TSet<AAIMonsterBase*> DamagedActors;
			bool bCollision = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel6, CollisionShape, param);

//#if ENABLE_DRAW_DEBUG
//			FColor DrawColor = bCollision ? FColor::Red : FColor::Green;
//
//			DrawDebugSphere(GetWorld(), Start, 300.f, 12, DrawColor, false, 1.f);
//#endif
			bool bGround = true;

			//충돌처리 결과
			for (const FHitResult& Hits : HitResults)
			{
				//충돌된 객체 처리
				AAIMonsterBase* HitActor = Cast<AAIMonsterBase>(Hits.GetActor());
				if (HitActor && !DamagedActors.Contains(HitActor))
				{
					bGround = false;
					HitStopStart();

					DamagedActors.Add(HitActor);

					FDamageEvent	DmgEvent;
					FName BoneName = Hits.BoneName; // 타격이 발생한 본을 확인
					float Dmg = 100.f;
					bool IsHeadShot = false;
					if (BoneName == "head")
					{
						IsHeadShot = true;
						Dmg *= 1.5f;
						HitActor->TakeDamage(Dmg, DmgEvent, GetController(), this);
					}
					else
					{
						HitActor->TakeDamage(Dmg, DmgEvent, GetController(), this);
					}

					FActorSpawnParameters SpawnParams;
					ADmgTextActor* DmgText = GetWorld()->SpawnActor<ADmgTextActor>(ADmgTextActor::StaticClass(),
						Hit.ImpactPoint + HitActor->GetActorUpVector() * 200.f + HitActor->GetActorRightVector() * -200.f, FRotator::ZeroRotator, SpawnParams);
					
					IsHeadShot ? DmgText->InputDamage(Dmg, true) : DmgText->InputDamage(Dmg);
				}
			}
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mDashExplosionEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), true);

			// Camera Shake
			GetController<APlayerController>()->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EShakeType::FloatDash]);

			USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
			if (SoundSubsystem)
				SoundSubsystem->PlayVFXSound(TEXT("DashSkillAttack"), 1.0f, Hit.ImpactPoint);

		}
		mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::DashAttack].Weight = 0.f;
	}
	else if (mAnimInstance->GetIsStagger())
	{
		GetController<APlayerController>()->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EShakeType::StrongAttack]);
		// 보스한테서 Hit됐을때 Landed 처리 예외
		return;
	}

	mAnimInstance->OffAim();
	mAnimInstance->OnDashEnable();
	mAnimInstance->OffFloat();			
	GetWorldSettings()->SetTimeDilation(1.f); 
}

void APlayerCharacter::GripWeapon(bool Grip)
{
	// 공격시 장착 비전투시 해제
	
	if (Grip) // 전투시
	{
		const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName("GripWeapon2");
		if (WeaponSocket && mWeapon)
			WeaponSocket->AttachActor(mWeapon, GetMesh());
	}
	else // 비전투시
	{
		const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName("WeaponCaseSocket2");
		if (WeaponSocket && mWeapon)
			WeaponSocket->AttachActor(mWeapon, GetMesh());
	}
}

void APlayerCharacter::ZoomIn()
{
	mIsZoom = true;
	
	GetController<APlayerController>()->bShowMouseCursor = true;

	mZoomComponent->SetPlayRate(1 / mBlendTimeOnZoomIn);
	mArm->bEnableCameraLag = false;
	mZoomComponent->Play();
}

void APlayerCharacter::ZoomOut()
{
	mIsZoom = false;
	mAimDecalComponent->SetVisibility(false);

	FInputModeGameOnly InputMode; 
	GetController<APlayerController>()->bShowMouseCursor = false;
	GetController<APlayerController>()->SetInputMode(InputMode);

	mZoomComponent->SetPlayRate(1);
	mArm->bEnableCameraLag = true;
	mZoomComponent->Reverse();
}


void APlayerCharacter::ZoomInByAiming(float Alpha)
{
	mArm->TargetArmLength = FMath::Lerp(mTargetArmLength, mTargetArmLenghtOnZoomIn, Alpha);
	mArm->SocketOffset = FMath::Lerp(mSocketOffset, mSocketOffsetOnZoomIn, Alpha);
	mCamera->SetFieldOfView(FMath::Lerp(mFieldOfView, mFieldOfViewOnZoomIn, Alpha));
}

void APlayerCharacter::UpdateCameraView(float DeltaTime)
{
	if (mIsZoom) return;

	if (!mTargetingSystem->GetTarget())
	{
		FVector NewLoc
			= FMath::VInterpTo(mArm->GetRelativeLocation(), DefaultArmLoc, DeltaTime, 1.f);
		mArm->SetRelativeLocation(NewLoc);

		mArm->SetRelativeLocation(FVector(0.0, 0.0, 300.0));
		
		float NewLength = FMath::FInterpTo(mArm->TargetArmLength, DefaultArmLength, DeltaTime, 1.f);
		mArm->TargetArmLength = NewLength;
	}
	else if(mTargetingSystem->IsDynamicLockOnTarget())
	{
		float Distance = FVector::Dist(GetActorLocation(), mTargetingSystem->GetTarget()->GetActorLocation());
		float Ratio = FMath::Clamp(Distance / MaxCameraDist,  MinCameraDist / MaxCameraDist, 1.0f);
		if (Ratio >= 1.f)
		{
			mArm->SetRelativeLocation(DefaultArmLoc);
			mArm->TargetArmLength = DefaultArmLength;
		}
		else
		{
			FVector InterpLocation = FMath::Lerp(MaxArmLoc, DefaultArmLoc, Ratio);
			mArm->SetRelativeLocation(InterpLocation);
			float InterpLenth = FMath::Lerp(MaxArmLength, DefaultArmLength, Ratio);
			mArm->TargetArmLength = InterpLenth;
		}
	}

	mTargetArmLength = mArm->TargetArmLength;
}

void APlayerCharacter::DashOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Hit일때 대쉬공격이면 폭발
	if (mAnimInstance->GetIsDash())
	{
		Landed(Hit);
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (mIsDead)
		return 0.f;

	if (mState->SetHP(DamageAmount))
	{
		// 죽을 경우 처리
		mIsDead = true;

		USoundSubsystem* SkillSFX = GetGameInstance()->GetSubsystem<USoundSubsystem>();
		if (SkillSFX)
		{
			SkillSFX->PlayVFXSound(TEXT("SkillSFX"), 0.7f, GetActorLocation());
		}

		// 래그돌
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				GetMesh()->SetSimulatePhysics(true);
				GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}, 0.35f, false);

		// Screen Effect 제거 
		SetScreenDamage(false);
		SetScreenFireDamage(false);

		AMotePlayerController* Control = GetController<AMotePlayerController>();
		if (Control)
		{
			// MainUI제거 
			Control->InVisibleMainWidget();
			//입력 불가
			DisableInput(Control);
			// Death로고 UI
			Control->VisibleDeathWidget();
		}
	}
	else
	{
		TakeDamageInteraction(DamageAmount);
	}

	return DamageAmount;
}

void APlayerCharacter::TakeDamageInteraction(float DamageAmount)
{
	if (DamageAmount >= 6.f)
	{
		if(mAnimInstance->GetAttackState())
			mAnimInstance->ZeroAttack();

		mAnimInstance->OffFloat();
		mAnimInstance->OnDamaged();

		if(!IsScreenDamage() && !mIsDead)
			SetScreenDamage(true);
		
		if (!mIsLowHelth)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					SetScreenDamage(false);
				}, 0.2f, false);
		}

	}

	if (DamageAmount < 6.f && !mAnimInstance->GetAttackState() && !mAnimInstance->GetIsJump() && !mAnimInstance->GetIsDash())
	{
		mAnimInstance->PlayDamageMontage();
	}
}
void APlayerCharacter::SetScreenDamage(bool Play)
{
	if (mIsDead) Play = false;
	Play ? mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::Damage].Weight = 1.f
		: mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::Damage].Weight = 0.f;
}

void APlayerCharacter::SetScreenFireDamage(bool Play)
{

	if (mIsDead) Play = false;
	Play ? mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::Fire].Weight = 0.8f
		: mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::Fire].Weight = 0.f;
}

bool APlayerCharacter::IsScreenDamage()
{
	return mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::Damage].Weight > 0.0f;
}

bool APlayerCharacter::IsScreenFireDamage()
{
	return mScreenEffect->Settings.WeightedBlendables.Array[(int8)EScreenEffct::Fire].Weight > 0.0f;
}

void APlayerCharacter::HitInteraction()
{
}

void APlayerCharacter::HitStopStart()
{
	FTimerHandle TimerHandle;

	CustomTimeDilation = 0.0f;
	
	if (mAnimInstance->GetIsAim()) // 대쉬공격일때
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::HitStopEnd, 0.12f, false);
	}
	else if (mAnimInstance->GetIsDash())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::HitStopEnd, 0.08f, false);
		if (!IsCameraShake)
		{
			IsCameraShake = true;
			GetController<APlayerController>()->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EShakeType::DashAttack]);
		}
	}
	else // 대쉬공격 아닐때
	{
		switch (mAnimInstance->GetCurrentAttackSection())
		{
		case 0:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::HitStopEnd, 0.12f, false);
			if (!IsCameraShake)
			{
				IsCameraShake = true;
				GetController<APlayerController>()->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EShakeType::BaseAttack]);
			}
			break;
		case 1:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::HitStopEnd, 0.12f, false);
			if (!IsCameraShake)
			{
				IsCameraShake = true;
				GetController<APlayerController>()->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EShakeType::BaseAttack]);
			}
			break;
		case 2:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::HitStopEnd, 0.24f, false);
			if (!IsCameraShake)
			{
				IsCameraShake = true;
				GetController<APlayerController>()->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EShakeType::StrongAttack]);
			}
			break;
		case 3:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::HitStopEnd, 0.25f, false);
			if (!IsCameraShake)
			{
				IsCameraShake = true;
				GetController<APlayerController>()->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EShakeType::StrongAttack]);
			}
			break;
		case 4:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::HitStopEnd, 0.26f, false);
			if (!IsCameraShake)
			{
				IsCameraShake = true;
				GetController<APlayerController>()->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EShakeType::StrongAttack_4th]);
			}
			break;
		}
	}
}

void APlayerCharacter::OnShield()
{
	mTrailFX->OnShieldActive();
}

void APlayerCharacter::OffShield()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, mTrailFX, &APlayerTrailFX::OffShieldActive, 1.f, false);
	//mTrailFX->OffShieldActive();
}

FGenericTeamId APlayerCharacter::GetGenericTeamId() const
{
	return FGenericTeamId(mTeamID);
}

void APlayerCharacter::Create()
{
	mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("CharacterLight"));
	mArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DashArrow"));

	mCloak_R = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Cloak_Right"));
	mCloak_L = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Cloak_Left"));
	mCloak_R->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Cloak"));
	mCloak_L->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Cloak"));

	mZoomComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("ZoomTimeline"));
	
	mStaminaComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StaminaWidget"));
	mAimDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("TargetAim"));

	mTargetingSystem = CreateDefaultSubobject<UTargetSystemComponent>(TEXT("TargetingSystem"));

	mGravityComponent = CreateDefaultSubobject<UCustomGravityComponent>(TEXT("CustomGravityComponent"));
}

void APlayerCharacter::Load()
{
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh>
			MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Player/HollowKnight/Mesh/HollowKnight.HollowKnight'"));

		if (MeshAsset.Succeeded())
			GetMesh()->SetSkeletalMeshAsset(MeshAsset.Object);
	}

	{
		static ConstructorHelpers::FClassFinder<UAnimInstance>
			PlayerAnimClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Player/ABP_Player.ABP_Player_C'"));

		if (PlayerAnimClass.Succeeded())
			GetMesh()->SetAnimInstanceClass(PlayerAnimClass.Class);
	}

	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh>
			Cloak_R(TEXT("/Script/Engine.SkeletalMesh'/Game/Player/HollowKnight/Equip/LeftCoak.LeftCoak'"));

		if (Cloak_R.Succeeded())
			mCloak_R->SetSkeletalMeshAsset(Cloak_R.Object);

		static ConstructorHelpers::FObjectFinder<USkeletalMesh>
			Cloak_L(TEXT("/Script/Engine.SkeletalMesh'/Game/Player/HollowKnight/Equip/RightCoak.RightCoak'"));

		if (Cloak_L.Succeeded())
			mCloak_L->SetSkeletalMeshAsset(Cloak_L.Object);
	}

	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat>
			ZoomCurve(TEXT("/Script/Engine.CurveFloat'/Game/Player/Data/ZoomCurve.ZoomCurve'"));

		if (ZoomCurve.Succeeded())
			mCameraZoomCurve = ZoomCurve.Object;
	}

	// CameraShake Regist
	mCameraShake.SetNum((int32)EShakeType::MAX);
	{
		static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCamera(TEXT("/Script/Engine.Blueprint'/Game/Player/BluePrint/BP_CameraShake.BP_CameraShake_C'"));
		if (ShakeCamera.Succeeded())
			mCameraShake[(int32)EShakeType::FloatDash] = ShakeCamera.Class;

		static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCamera2(TEXT("/Script/Engine.Blueprint'/Game/Player/BluePrint/BP_CameraShake_BaseAttack.BP_CameraShake_BaseAttack_C'"));
		if (ShakeCamera.Succeeded())
			mCameraShake[(int32)EShakeType::BaseAttack] = ShakeCamera2.Class;

		static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCamera3(TEXT("/Script/Engine.Blueprint'/Game/Player/BluePrint/BP_CameraShake_StrongAttack.BP_CameraShake_StrongAttack_C'"));
		if (ShakeCamera.Succeeded())
			mCameraShake[(int32)EShakeType::StrongAttack] = ShakeCamera3.Class;

		static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCamera4(TEXT("/Script/Engine.Blueprint'/Game/Player/BluePrint/BP_CameraShake_StrongAttack_4th.BP_CameraShake_StrongAttack_4th_C'"));
		if (ShakeCamera.Succeeded())
			mCameraShake[(int32)EShakeType::StrongAttack_4th] = ShakeCamera4.Class;

		static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCamera5(TEXT("/Script/Engine.Blueprint'/Game/Player/BluePrint/BP_CameraShake_DashAttack.BP_CameraShake_DashAttack_C'"));
		if (ShakeCamera5.Succeeded())
			mCameraShake[(int32)EShakeType::DashAttack] = ShakeCamera5.Class;
	}

	{
		static ConstructorHelpers::FClassFinder<UPlayerStaminaWidget>
			StaminaWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Player/UI_PlayerStamina.UI_PlayerStamina_C'"));

		if (StaminaWidgetClass.Succeeded())
			mStaminaComponent->SetWidgetClass(StaminaWidgetClass.Class);
	}

	{
		mAimMtrlInst = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr
			, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/UI/Player/MTI_Target_Inst.MTI_Target_Inst'")));

		mAimMtrlInstDynamic = UMaterialInstanceDynamic::Create(mAimMtrlInst, this);
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem>
			ExplosionAsset(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/SpecialAttack2/FX/P_PH_Shockwave_V2.P_PH_Shockwave_V2'"));

		if (ExplosionAsset.Succeeded())
			mDashExplosionEffect = ExplosionAsset.Object;
	}

	{
		mScreenDamage = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr
			, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/ScreenEffects/Materials/MI_BloodDamage.MI_BloodDamage'")));

		mScreenDamageDynamic = UMaterialInstanceDynamic::Create(mScreenDamage, this);
	}

	{
		mScreenDamageFire = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr
			, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/ScreenEffects/Materials/MI_ScreenDamageFire.MI_ScreenDamageFire'")));

		mScreenDamageFireDynamic = UMaterialInstanceDynamic::Create(mScreenDamageFire, this);
	}

	{
		mScreenDashAttack = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr
			, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/ScreenEffects/Materials/MI_RadialBlur.MI_RadialBlur'")));

		mScreenDashAttackDynamic = UMaterialInstanceDynamic::Create(mScreenDashAttack, this);
		mScreenDashAttackDynamic->SetScalarParameterValue(TEXT("Blur"), 0.4f);
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem>
			SpawnAsset(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/SharedGameplay/States/Recall/P_Recall_BigBeamDown_Ghosts.P_Recall_BigBeamDown_Ghosts'"));

		if (SpawnAsset.Succeeded())
			mSpawnParticle = SpawnAsset.Object;
	}
}

void APlayerCharacter::Init()
{
	mArm->SetupAttachment(RootComponent);
	mCamera->SetupAttachment(mArm);
	mLight->SetupAttachment(RootComponent);
	mArrow->SetupAttachment(RootComponent);
	mStaminaComponent->SetupAttachment(GetMesh());

	GetCapsuleComponent()->SetCapsuleHalfHeight(90.f);
	GetCapsuleComponent()->SetCapsuleRadius(35.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));

	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -93.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));


	mArm->SetRelativeLocation(DefaultArmLoc);
	mArm->SetRelativeRotation(FRotator(-5.0, 0.0, 0.0));
	mArm->TargetArmLength = DefaultArmLength;

	// Camera Rotate Set
	bUseControllerRotationYaw = false;
	mArm->bInheritPitch = false;
	mArm->bInheritYaw = false;
	mArm->bInheritRoll = false;
	mArm->bEnableCameraLag = true;


	mLight->SetRelativeLocation(FVector(0.0, 0.0, 3000.0));
	mLight->SetRelativeRotation(FRotator(-90.0, 0.0, 0.0));
	mLight->SetMobility(EComponentMobility::Movable);
	mLight->SetIntensityUnits(ELightUnits::Lumens);
	mLight->SetIntensity(1500.0);
	mLight->SetAttenuationRadius(16384.0);
	mLight->SetInnerConeAngle(80.0);
	mLight->SetOuterConeAngle(80.0);
	mLight->SetSourceRadius(1000.0);
	mLight->SetSoftSourceRadius(1000.0);

	mLight->SetSourceLength(1000.0);

	mArrow->SetRelativeLocation(FVector(0.0, 0.0, -15.0));

	mStaminaComponent->SetRelativeLocation(FVector(0.0, 0.0, 250.0));
	mStaminaComponent->SetPivot(FVector2D(0.5, 1.0));
	mStaminaComponent->SetDrawSize(FVector2D(30.0, 30.0));
	mStaminaComponent->SetWidgetSpace(EWidgetSpace::Screen);
	mStaminaComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mStaminaComponent->bVisibleInReflectionCaptures = false;
	mStaminaComponent->bVisibleInRealTimeSkyCaptures = false;
	mStaminaComponent->bReceivesDecals = false;

	GetMesh()->SetCollisionProfileName(TEXT("RagDoll"));
	GetMesh()->bReceivesDecals = false;
	mCloak_R->bReceivesDecals = false;
	mCloak_L->bReceivesDecals = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->MaxWalkSpeed = 750.f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->GravityScale = 3.0;
	GetCharacterMovement()->AirControl = 1.0;
	GetCharacterMovement()->MaxAcceleration = 3072.f;
}