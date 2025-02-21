// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TargetSystemComponent.h"
#include "Player/PlayerCharacter.h"
#include "../Controller/MotePlayerController.h"
#include "Player/PlayerInfo.h"
#include "../AI/AIMonsterBase.h"
#include "../AI/Monster/TargetAimSystemComponent.h"

UTargetSystemComponent::UTargetSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Debug
	bDrawDebug = false;
	DebugTargetSphereSize = 30.0f;
	DebugTargetSphereSegment = 30.0f;

	// TargetingSystem
	bIsLockOnTarget = false;
	bDynamicLockOnTarget = false;
	StopDynamicCameraLockScreenRatio = 0.25f;
	Target = nullptr;
	MaxSearchTargetableDistance = 2000.0f;
	InterpSpeed = 5.0f;
	LastTimeChangeTarget = 0.0f;
	TimeSinceLastChangeTarget = 0.0f;
	MinMouseValueToChangeTarget = 0.01f; // 마우스 회전 input값
	MinDelayTimeToChangeTarget = 0.3f;
	MinCancelToTarget = 3.f;

}


void UTargetSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializePlayerReference();

	/*APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	
	if (IsValid(Player))
		mPlayer = Player;

	mPlayerController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));*/

}


void UTargetSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	UpdateCamera(DeltaTime);
	//UpdateDrawDebug();
	//UpdateCamera(DeltaTime);

	//if (bDrawDebug)
	//{
	//	UpdateDrawDebug();
	//}
}

#pragma region Debug
bool UTargetSystemComponent::IsDrawDebug() const
{
	return bDrawDebug;
}

void UTargetSystemComponent::UpdateDrawDebug()
{
	//// 탐지하는 범위를 드로우 디버그합니다.
	DrawDebugSphere(GetWorld(), mPlayerOwner->GetActorLocation(), DetectionRange,
		DebugTargetSphereSegment, FColor::Green);

	//// Target을 드로우 디버그합니다.
	//if (IsValid(Target) == true)
	//{
	//	AAIMonsterBase* Monster = Cast<AAIMonsterBase>(Target);
	//	if (Monster)
	//	{
	//		DrawDebugSphere(GetWorld(), Cast<UTargetAimSystemComponent>(Monster->GetTargetAimSystem())->GetComponentLocation(),
	//			DebugTargetSphereSize, DebugTargetSphereSegment, FColor::Red);
	//	}
	//}

	//// Target을 제외한 TargetableActors를 드로우 디버그합니다.
	//TArray<AActor*> TargetableActors = SearchTargetableActors();
	//TargetableActors.Remove(Target);
	//for (AActor* TargetableActor : TargetableActors)
	//{
	//	AAIMonsterBase* Monster = Cast<AAIMonsterBase>(TargetableActor);
	//	if (Monster)
	//	{
	//		DrawDebugSphere(GetWorld(), Monster->GetTargetAimLocation(),
	//			DebugTargetSphereSize, DebugTargetSphereSegment, FColor::Green);
	//	}
	//}
}

void UTargetSystemComponent::SetDrawDebug(bool bFlag)
{
	bDrawDebug = bFlag;
}
#pragma endregion 

#pragma region PlayerReference
void UTargetSystemComponent::InitializePlayerReference()
{
	APlayerCharacter* NewPlayerOwner = Cast<APlayerCharacter>(GetOwner());
	if (IsValid(NewPlayerOwner))
	{
		mPlayerOwner = NewPlayerOwner;
	}

	mPlayerController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	mPlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
}
#pragma endregion

#pragma region TargetingSystem
bool UTargetSystemComponent::IsLockOnTarget() const
{
	return bIsLockOnTarget && IsValid(Target);
}

bool UTargetSystemComponent::IsDynamicLockOnTarget() const
{
	return bDynamicLockOnTarget;
}

void UTargetSystemComponent::ExecuteLockOnTarget()
{
	SetTarget(FindTarget());
	if (IsValid(Target))
	{
		EnableCameraLock();
	}
	else
	{
		CancelLockOnTarget();
	}
}

void UTargetSystemComponent::CancelLockOnTarget()
{
	DisableCameraLock();
	if (bDynamicLockOnTarget) bDynamicLockOnTarget = false;
	Target = nullptr;
}

void UTargetSystemComponent::HoldTargetLockOn()
{
	EnableCameraLock();
}

void UTargetSystemComponent::HoldTargetLockOff()
{
	DisableCameraLock();
}

void UTargetSystemComponent::ChangeLockOnTargetForTurnValue(float TurnValue)
{
	AAIMonsterBase* Monster = Cast<AAIMonsterBase>(Target);
	if (IsValid(Monster)&& Monster->GetAIType() != EAIType::Death)
	{
		Monster->SelectedTarget(false);
	}
	
	// 마우스 회전이 크면 종료
	if (FMath::Abs(TurnValue) > MinCancelToTarget)
	{
		CancelLockOnTarget();
		return;
	}

	TimeSinceLastChangeTarget = GetWorld()->GetRealTimeSeconds() - LastTimeChangeTarget;
	if (FMath::Abs(TurnValue) > MinMouseValueToChangeTarget && TimeSinceLastChangeTarget > MinDelayTimeToChangeTarget)
	{
		if (TurnValue < 0.0f)
		{
			SetTarget(FindDirectionalTarget(ETargetDirection::TargetDirection_Left));
		}
		else
		{
			SetTarget(FindDirectionalTarget(ETargetDirection::TargetDirection_Right));
		}

		LastTimeChangeTarget = GetWorld()->GetRealTimeSeconds();
	}

	EnableCameraLock();
}

void UTargetSystemComponent::UpdateCamera(float DeltaTime)
{
	// DynamicLockOn일 경우
	if (bDynamicLockOnTarget)
	{
		UpdateDynamicCameraLock(DeltaTime);
	}
	// LockOn일 경우
	else
	{
		UpdateCameraLock();
	}
}

void UTargetSystemComponent::UpdateCameraLock()
{
	if (bIsLockOnTarget)
	{
		if (IsValid(Target)&& mPlayerOwner->GetDistanceTo(Target) <= DetectionRange)
		{
			// Target이 살아있는 동안 카메라가 Target을 바라보도록 고정
			AAIMonsterBase* Monster = Cast<AAIMonsterBase>(Target);
			if (IsValid(Monster)&& Monster->GetAIType() !=  EAIType::Death && mPlayerController)
			{
				const FRotator InterpToTarget = CalculateInterpToTarget(Monster);
				mPlayerController->SetControlRotation(InterpToTarget);
				mPlayerOwner->GetComponentByClass<USpringArmComponent>()->SetRelativeRotation(InterpToTarget);
			}
			else
			{
				// Target이 사망했을 경우 다음 Target
				CancelLockOnTarget();
				ExecuteLockOnTarget();
			}
		}
		else
		{
			CancelLockOnTarget();
		}
	}
}

void UTargetSystemComponent::UpdateDynamicCameraLock(float DeltaTime)
{
	if (bIsLockOnTarget)
	{
		if (IsValid(Target) && mPlayerOwner->GetDistanceTo(Target) <= DetectionRange)
		{
			// Target이 살아있는 동안 카메라가 Target을 바라보도록 고정합니다.
			AAIMonsterBase* Monster = Cast<AAIMonsterBase>(Target);
			if (IsValid(Monster) && Monster->GetAIType() != EAIType::Death && mPlayerController)
			{
				if (!mPlayerOwner->IsMoveCamera())
				{
					if (IsInViewport(GetScreenPositionOfActor(Monster).Get<0>(), StopDynamicCameraLockScreenRatio) == false)
					{
						const FRotator CurrentRotator = mPlayerOwner->GetCameraRotator();
						const FRotator TargetRotator = UKismetMathLibrary::FindLookAtRotation(mPlayerOwner->GetActorLocation(), Target->GetActorLocation());
						const FRotator InterpRotator = UKismetMathLibrary::RInterpTo(CurrentRotator, TargetRotator, DeltaTime, 0.3f);

						mPlayerController->SetControlRotation(InterpRotator);
						mPlayerOwner->GetComponentByClass<USpringArmComponent>()->SetRelativeRotation(InterpRotator);
						mPlayerOwner->SetCameraRotator(InterpRotator);
					}
				}
			}
			else
			{
				// Target이 사망했을 경우 다음 Target을 찾습니다.
				CancelLockOnTarget();
				ExecuteLockOnTarget();
			}
		}
		else
		{
			CancelLockOnTarget();
		}
	}
}

void UTargetSystemComponent::EnableCameraLock()
{
	AAIMonsterBase* Monster = Cast<AAIMonsterBase>(Target);
	if (IsValid(Monster))
	{
		bIsLockOnTarget = true;
		Monster->SelectedTarget(true);
	}
}

void UTargetSystemComponent::DisableCameraLock()
{
	bIsLockOnTarget = false;

	AAIMonsterBase* Monster = Cast<AAIMonsterBase>(Target);
	if (IsValid(Monster) )
	{
		Monster->SelectedTarget(false);
	}
}

void UTargetSystemComponent::SetTarget(AActor* NewTarget)
{
	if (IsValid(NewTarget) )
	{
		Target = NewTarget;
	}
	else
	{
		Target = nullptr;
	}
}

AActor* UTargetSystemComponent::FindTarget() const
{
	TArray<AActor*> TargetableActors = SearchTargetableActors();
	AActor* NewTarget = nullptr;
	float DistanceFromCenterOfViewport = 0.0f;
	const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();

	// 화면의 중앙에 가까운 액터를 찾아 NewTarget으로 설정
	int32 ArrayIndex = 0;
	for (AActor* TargetableActor : TargetableActors)
	{
		const float ActorScreenPosition = UKismetMathLibrary::Abs(GetScreenPositionOfActor(TargetableActor).Get<0>().X
			- ViewportSize.X / 2.0f);

		if (ArrayIndex == 0)
		{
			DistanceFromCenterOfViewport = ActorScreenPosition;
		}
		else
		{
			if (ActorScreenPosition < DistanceFromCenterOfViewport)
			{
				DistanceFromCenterOfViewport = ActorScreenPosition;
			}
			else
			{
				continue;
			}
		}

		NewTarget = TargetableActor;
		ArrayIndex++;
	}

	return NewTarget;
}

TArray<AActor*> UTargetSystemComponent::SearchTargetableActors(bool bInScreenPosition) const
{
	const FVector PlayerLocation = mPlayerOwner->GetActorLocation();

	TArray<FHitResult> HitResults;
	TArray<AActor*> TargetableActors;

	// ECollisionChannel::ECC_GameTraceChannel2는 TraceChannels의 Targeting을 나타냅니다.
	bool bIsHit = GetWorld()->SweepMultiByChannel(HitResults, PlayerLocation, PlayerLocation, FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel7, FCollisionShape::MakeSphere(DetectionRange));
	if (bIsHit)
	{
		for (FHitResult Hit : HitResults)
		{
			if (IsValid(Hit.GetActor()) )
			{
				AAIMonsterBase* HitedAI = Cast<AAIMonsterBase>(Hit.GetActor());
				if (IsValid(HitedAI)  && HitedAI->GetAIType() != EAIType::Death)
				{
					if (bInScreenPosition)
					{
						TTuple<FVector2D, bool> ActorScreenPosition = GetScreenPositionOfActor(HitedAI);
						if (IsInViewport(ActorScreenPosition.Get<0>()) && ActorScreenPosition.Get<1>())
						{
							// 중복되지 않게 TargetableActors Array에 추가
							TargetableActors.AddUnique(Cast<AActor>(Hit.GetActor()));
						}
					}
					else
					{
						// 중복되지 않게 TargetableActors Array에 추가
						TargetableActors.AddUnique(Cast<AActor>(Hit.GetActor()));
					}
				}
			}
		}
	}

	return TargetableActors;
}

AActor* UTargetSystemComponent::FindDirectionalTarget(ETargetDirection NewTargetDirection)
{
	AActor* NewTarget = nullptr;
	TArray<AActor*> TargetableActors = SearchTargetableActors();
	const int32 CurrentTargetIndex = TargetableActors.Find(Target);

	// 현재 Target이 TargetableActors 배열에 존재할 경우 배열에서 제거
	if (CurrentTargetIndex != INDEX_NONE)
	{
		TargetableActors.Remove(Target);
	}

	// 현재 Target을 기준으로 좌우에 있는 Target을 탐색
	TArray<AActor*> LeftTargetableActors;
	TArray<AActor*> RightTargetableActors;

	for (AActor* TargetableActor : TargetableActors)
	{
		AAICharacterBase* Monster = Cast<AAICharacterBase>(TargetableActor);
		if (IsValid(Monster)  && Monster->GetAIType() != EAIType::Death)
		{
			switch (WhichSideOfTarget(Monster))
			{
			case ETargetDirection::TargetDirection_Left:
				LeftTargetableActors.AddUnique(Monster);
				break;
			case ETargetDirection::TargetDirection_Right:
				RightTargetableActors.AddUnique(Monster);
				break;
			default:
				break;
			}
		}
	}

	// DynamicLockOn일 경우
	if (bDynamicLockOnTarget)
	{
		switch (NewTargetDirection)
		{
		case ETargetDirection::TargetDirection_Left:
			if (LeftTargetableActors.Num() > 0)
			{
				// 왼쪽에서 내적이 가장 큰 Target
				NewTarget = GetTargetByDotProduct(LeftTargetableActors, true);
			}
			else
			{
				// 오른쪽에서 내적이 가장 큰 Target
				NewTarget = GetTargetByDotProduct(RightTargetableActors, true);
			}
			break;
		case ETargetDirection::TargetDirection_Right:
			if (RightTargetableActors.Num() > 0)
			{
				// 오른쪽에서 내적이 가장 작은 Target
				NewTarget = GetTargetByDotProduct(RightTargetableActors, false);
			}
			else
			{
				// 왼쪽에서 내적이 가장 작은 Target
				NewTarget = GetTargetByDotProduct(LeftTargetableActors, false);
			}
			break;
		default:
			break;
		}
	}
	// LockOn일 경우
	else
	{
		// 두 벡터가 이루는 각이 클수록 두 벡터의 내적은 작습니다.
		switch (NewTargetDirection)
		{
		case ETargetDirection::TargetDirection_Left:
			if (LeftTargetableActors.Num() > 0)
			{
				// 왼쪽에서 내적이 가장 큰 Target
				NewTarget = GetTargetByDotProduct(LeftTargetableActors, true);
			}
			else
			{
				// 오른쪽에서 내적이 가장 작은 Target
				NewTarget = GetTargetByDotProduct(RightTargetableActors, false);
			}
			break;
		case ETargetDirection::TargetDirection_Right:
			if (RightTargetableActors.Num() > 0)
			{
				// 오른쪽에서 내적이 가장 큰 Target
				NewTarget = GetTargetByDotProduct(RightTargetableActors, true);
			}
			else
			{
				// 왼쪽에서 내적이 가장 작은 Target
				NewTarget = GetTargetByDotProduct(LeftTargetableActors, false);
			}
			break;
		default:
			break;
		}
	}

	if (IsValid(NewTarget))
	{
		return NewTarget;
	}

	return nullptr;
}

ETargetDirection UTargetSystemComponent::WhichSideOfTarget(AActor* NewTargetableActor) const
{
	// DynamicLockOn일 경우
	// 화면을 기준으로 구분합니다.
	if (bDynamicLockOnTarget)
	{
		const float TargetDot = CalculateDotProductToTarget(Target);
		const float NewTargetDot = CalculateDotProductToTarget(NewTargetableActor);

		if (TargetDot >= NewTargetDot)
		{
			return ETargetDirection::TargetDirection_Left;
		}

		return ETargetDirection::TargetDirection_Right;
	}

	// LockOn일 경우
	// Target과 플레이어의 내적을 기준으로 구분합니다.
	const FVector TargetActorDirection = 
		UKismetMathLibrary::GetDirectionUnitVector(mPlayerCameraManager->GetCameraLocation(), Target->GetActorLocation());
	const FVector NewTargetableActorDirection = 
		UKismetMathLibrary::GetDirectionUnitVector(mPlayerCameraManager->GetCameraLocation(),NewTargetableActor->GetActorLocation());
	const FVector NewDirection = UKismetMathLibrary::Cross_VectorVector(TargetActorDirection, NewTargetableActorDirection);
	const float NewDot = UKismetMathLibrary::Dot_VectorVector(NewDirection, GetOwner()->GetActorUpVector());

	if (NewDot >= 0.0f)
	{
		return ETargetDirection::TargetDirection_Right;
	}

	return ETargetDirection::TargetDirection_Left;
}

AActor* UTargetSystemComponent::GetTargetByDotProduct(TArray<AActor*> TargetableActors, bool bIsLargestDot)
{
	float DotProduct = 0.0f;
	AActor* NewTarget = nullptr;
	int32 ArrayIndex = 0;

	if (TargetableActors.Num() == 0)
	{
		return nullptr;
	}

	for (AActor* TargetableActor : TargetableActors)
	{
		if (ArrayIndex == 0)
		{
			DotProduct = CalculateDotProductToTarget(TargetableActor);
			NewTarget = TargetableActor;
		}
		else
		{
			if (bIsLargestDot)
			{
				if (CalculateDotProductToTarget(TargetableActor) > DotProduct)
				{
					DotProduct = CalculateDotProductToTarget(TargetableActor);
					NewTarget = TargetableActor;
				}
			}
			else
			{
				if (CalculateDotProductToTarget(TargetableActor) < DotProduct)
				{
					DotProduct = CalculateDotProductToTarget(TargetableActor);
					NewTarget = TargetableActor;
				}
			}
		}

		ArrayIndex++;
	}

	return NewTarget;
}

float UTargetSystemComponent::CalculateDotProductToTarget(AActor* NewTargetableActor) const
{
	if (!NewTargetableActor)
	{
		return 0.0f;
	}

	// DynamicLockOn일 경우
	if (bDynamicLockOnTarget)
	{
		if (mPlayerCameraManager)
		{
			const FVector NewTargetableActorDirection = UKismetMathLibrary::GetDirectionUnitVector(mPlayerCameraManager->GetCameraLocation(),
				NewTargetableActor->GetActorLocation());
			const FVector CrossProduct = UKismetMathLibrary::Cross_VectorVector(mPlayerCameraManager->GetActorForwardVector(), NewTargetableActorDirection);

			return UKismetMathLibrary::Dot_VectorVector(mPlayerCameraManager->GetActorUpVector(), CrossProduct);
		}
	}
	// LockOn일 경우
	else
	{
		// UKismetMathLibrary::GetDirectionUnitVector(start,end) 방향단위벡터
		const FVector TargetActorDirection = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(),
			Target->GetActorLocation());
		const FVector NewTargetableActorDirection = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(),
			NewTargetableActor->GetActorLocation());

		return UKismetMathLibrary::Dot_VectorVector(TargetActorDirection, NewTargetableActorDirection);
	}

	return 0.0f;
}

TTuple<FVector2D, bool> UTargetSystemComponent::GetScreenPositionOfActor(AActor* SearchActor) const
{
	FVector2D ScreenLocation = FVector2D::ZeroVector;
	bool bResult = UGameplayStatics::ProjectWorldToScreen(mPlayerController, SearchActor->GetActorLocation(),
		ScreenLocation);

	return MakeTuple(ScreenLocation, bResult);
}

bool UTargetSystemComponent::IsInViewport(FVector2D ActorScreenPosition, float ScreenRatio) const
{
	FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();

	// 화면 전체를 사용할 경우
	if (ScreenRatio == 0.0f || UKismetMathLibrary::Abs(ScreenRatio) > 1.0f || (ScreenRatio == (1.0f - ScreenRatio)))
	{
		return ActorScreenPosition.X >= 0.0f && ActorScreenPosition.Y >= 0.0f
			&& ActorScreenPosition.X <= ViewportSize.X && ActorScreenPosition.Y <= ViewportSize.Y;
	}

	float LargeScreenRatio;
	float SmallScreenRatio;
	if (ScreenRatio < (1.0f - ScreenRatio))
	{
		LargeScreenRatio = 1.0f - ScreenRatio;
		SmallScreenRatio = ScreenRatio;
	}
	else
	{
		LargeScreenRatio = ScreenRatio;
		SmallScreenRatio = 1.0f - ScreenRatio;
	}

	return ActorScreenPosition.X >= ViewportSize.X * SmallScreenRatio && ActorScreenPosition.X <= ViewportSize.X * LargeScreenRatio
		&& ActorScreenPosition.Y >= ViewportSize.Y * SmallScreenRatio && ActorScreenPosition.Y <= ViewportSize.Y * LargeScreenRatio;
}

FRotator UTargetSystemComponent::CalculateInterpToTarget(AActor* InterpToTarget) const
{
	if (IsValid(InterpToTarget) )
	{
		const FRotator PlayerControllerRotator = mPlayerController->GetControlRotation();
		const FVector PlayerLocation = mPlayerOwner->GetActorLocation() + /* offset */FVector(0.f, 0.f, 150.f);
		const FVector TargetLocation = InterpToTarget->GetActorLocation();

		// 2개의 위치 벡터를 입력하여 2번 째 인자의 위치 벡터를 바라보는 방향정보를 반환
		const FRotator LookAtTarget = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TargetLocation);
		// 1번 째 인자에서 2번 째 인자로 회전 보간
		const FRotator RInterpToRotator = FMath::RInterpTo(PlayerControllerRotator, LookAtTarget,
			GetWorld()->GetDeltaSeconds(), InterpSpeed);

		return RInterpToRotator;
	}

	return FRotator::ZeroRotator;
}

void UTargetSystemComponent::SetDynamicLockOnTarget(bool bFlag)
{
	bDynamicLockOnTarget = bFlag;
}

AActor* UTargetSystemComponent::GetTarget() const
{
	return Target;
}
#pragma endregion 

