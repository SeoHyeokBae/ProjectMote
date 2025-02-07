// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NotifyPlayerAttackCollision.h"
#include "Player/PlayerCharacter.h"
#include "Player/PlayerAnimInstance.h"
#include "Player/MotePlayerState.h"
#include "../AI/AIMonsterBase.h"
#include "../UI/ETC/DmgTextActor.h"
#include "../Sound/SoundSubsystem.h"

UNotifyPlayerAttackCollision::UNotifyPlayerAttackCollision()
{
	// 공격할떄마다 로드하기때문에 성능에 영향이 갈 수 있음
	UNiagaraSystem* NiagaraHitAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/Player/FX/NS_PlayerAttackCollisionFX.NS_PlayerAttackCollisionFX'"));

	if (NiagaraHitAsset)
		mHitEffect = NiagaraHitAsset;

	UNiagaraSystem* NiagaraSlashHitAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/SlashHitAndStabHit/Particles/Niagara/NS_ky_slashHit08.NS_ky_slashHit08'"));

	if (NiagaraSlashHitAsset)
		mSlashHitEffect = NiagaraSlashHitAsset;
}

void UNotifyPlayerAttackCollision::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		int32 CurrentCombo = 0;
		CurrentCombo = Cast<UPlayerAnimInstance>(MeshComp->GetAnimInstance())->GetCurrentAttackSection();
		switch (CurrentCombo)
		{
		case 0:
			CreateCollision(Player);
			break;
		case 1:
			CreateCollision(Player);
			break;
		case 2:
			CreateCollision(Player);
			break;
		case 3:
			CreateCollision(Player);
			break;
		case 4:
			CreateCollision(Player);
			break;
		}
	}
}

void UNotifyPlayerAttackCollision::CreateCollision(class APlayerCharacter* Player, float Angle)
{
	float Distance = 400.f;

	//DamagedActors.Empty();
	// 각 Linetrace마다 담아지는 Results가 초기화되기때문에 매 LineTrace마다 담아두엇다가 HitResults에 추가
	TArray<FHitResult> HitResults;
	TArray<FHitResult> TempHitResults; 
	//HitResults.Empty();
	//TempHitResults.Empty();

	// 라인 갯수 초기화
	mCoord.SetNum(13);
	mCoordDown.SetNum(13);
	
	// 정점세팅
	mCoord[0] = Player->GetActorLocation();
	mCoordDown[0] = Player->GetActorLocation() 
					- FVector(0.0,0.0,Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() -1);

	float firstAngle = - (Angle / 2);
	for (int32 i = 0; i < mCoord.Num() - 1; ++i)
	{
		float AngleDegrees = firstAngle + Angle / (mCoord.Num() - 1) * i;
		float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
		FVector Direction = Player->GetActorForwardVector() * FMath::Cos(AngleRadians) 
							+ Player->GetActorRightVector() * FMath::Sin(AngleRadians);
		mCoord[i + 1] = mCoord[0] + Direction * Distance;
		mCoordDown[i + 1] = mCoordDown[0] + Direction * Distance;
	}

	// 정점연결 및 충돌 검사
	for (int32 i = 0; i < mCoord.Num() - 1; ++i)
	{
		FVector Start = mCoord[0];
		FVector End = mCoord[i + 1];

		FVector StartDown = mCoordDown[0];
		FVector EndDown = mCoordDown[i + 1];

		// 외곽 연결
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(Player);


		if (i != 0)
		{
			FVector Prev = mCoord[i]; // 1일때 [i + 1 = 2] 와 [i = 1] 연결 

			bool bHitArc = Player->GetWorld()->LineTraceMultiByChannel(
				TempHitResults,
				Prev,
				End,
				ECollisionChannel::ECC_GameTraceChannel6,
				TraceParams);

			HitResults.Append(TempHitResults);

//#if ENABLE_DRAW_DEBUG
//			FColor DrawColor = bHitArc ? FColor::Red : FColor::Green;
//			DrawDebugLine(Player->GetWorld(), Prev, End, DrawColor, false, 1.0f, 0, 5.0f);
//#endif

			 //아래
			FVector PrevDown = mCoordDown[i]; // 1일때 [i + 1 = 2] 와 [i = 1] 연결 

			bool bHitArcDown = Player->GetWorld()->LineTraceMultiByChannel(
				TempHitResults,
				PrevDown,
				EndDown,
				ECollisionChannel::ECC_GameTraceChannel6,
				TraceParams);

			HitResults.Append(TempHitResults);

//#if ENABLE_DRAW_DEBUG
//			FColor DrawColor2 = bHitArcDown ? FColor::Red : FColor::Green;
//			DrawDebugLine(Player->GetWorld(), PrevDown, EndDown, DrawColor2, false, 1.0f, 0, 5.0f);
//#endif
		}

		bool bHit = Player->GetWorld()->LineTraceMultiByChannel(
			TempHitResults,
			Start,
			End,
			ECollisionChannel::ECC_GameTraceChannel6,
			TraceParams);

		HitResults.Append(TempHitResults);

//#if ENABLE_DRAW_DEBUG
//		FColor DrawColor = bHit ? FColor::Red : FColor::Green;
//		DrawDebugLine(Player->GetWorld(), Start, End, DrawColor, false, 1.0f, 0, 5.0f);
//#endif
		 //아래
		bool bHitDown = Player->GetWorld()->LineTraceMultiByChannel(
			TempHitResults,
			StartDown,
			EndDown,
			ECollisionChannel::ECC_GameTraceChannel6,
			TraceParams);

		HitResults.Append(TempHitResults);
		TempHitResults.Empty(); //02.03 크래쉬 추가
//#if ENABLE_DRAW_DEBUG
//		FColor DrawColor2 = bHitDown ? FColor::Red : FColor::Green;
//		DrawDebugLine(Player->GetWorld(), StartDown, EndDown, DrawColor2, false, 1.0f, 0, 5.0f);
//#endif
		// 다중타격시 Rand각도로 각 다르게 Hit effect
		int count = 0;
		for (const FHitResult& Hit : HitResults)
		{
			AAIMonsterBase* HitActor = Cast<AAIMonsterBase>(Hit.GetActor());
			if (HitActor)
			{
				if (!DamagedActors.Contains(HitActor))
				{
					Player->HitStopStart();
					DamagedActors.Add(HitActor);

					FDamageEvent	DmgEvent;
					FRotator RotOffset = FRotator::ZeroRotator;
					float RandAgle = 0.f;
					float RandDmg = RandAgle = FMath::RandRange(-10, 5);
					if (count != 0) RandAgle = FMath::RandRange(-(count + 1) * 5, 45);
					int32 CurrentCombo = 0;
					CurrentCombo = Cast<UPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance())->GetCurrentAttackSection();

					float Damage = Cast<AMotePlayerState>(Player->GetPlayerState())->mAttack + RandDmg;
					float AddDmg = 0.f;
					float AddHealth = 0.f;

					bool IsHeadShot = false;
					FName BoneName = Hit.BoneName; // 타격이 발생한 본을 확인
					if (BoneName == "head")
					{
						IsHeadShot = true;
						Damage *= 2.f;
					}
					
					// 사운드 호출
					USoundSubsystem* SoundSubsystem = Player->GetGameInstance()->GetSubsystem<USoundSubsystem>();
					switch (CurrentCombo)
					{
					case 0:
					{
						AddDmg = 8.f;
						AddHealth = 1.f;
						if (Player->GetCharacterMovement()->IsFalling())
							HitActor->TakeDamage(Damage + AddDmg, DmgEvent, Player->GetController(), Player);
						else
							HitActor->TakeDamage(Damage, DmgEvent, Player->GetController(), Player);
						Cast<AMotePlayerState>(Player->GetPlayerState())->SetHP(AddHealth, true);
						RotOffset = FRotator(0.0, 10.0 + RandAgle, 0.0);

						if (SoundSubsystem)
							SoundSubsystem->PlayVFXSound(TEXT("Hit0"),3.f);
						break;
					}
					case 1:
					{
						AddDmg = 2.f;
						AddHealth = 1.f;
						HitActor->TakeDamage(Damage + AddDmg, DmgEvent, Player->GetController(), Player);
						Cast<AMotePlayerState>(Player->GetPlayerState())->SetHP(AddHealth, true);
						RotOffset = FRotator(-180.0, -15.0 - RandAgle, 0.0);

						if (SoundSubsystem)
							SoundSubsystem->PlayVFXSound(TEXT("Hit1"), 3.f);
						break;
					}
					case 2:
					{
						AddDmg = 5.f;
						AddHealth = 3.f;
						HitActor->TakeDamage(Damage + AddDmg, DmgEvent, Player->GetController(), Player);
						Cast<AMotePlayerState>(Player->GetPlayerState())->SetHP(AddHealth, true);
						RotOffset = FRotator(0.0, 0.0 + RandAgle, 0.0);

						if (SoundSubsystem)
							SoundSubsystem->PlayVFXSound(TEXT("Hit2"), 3.f);
						break;
					}
					case 3:
					{
						AddDmg = 10.f;
						AddHealth = 5.f;
						HitActor->TakeDamage(Damage + AddDmg, DmgEvent, Player->GetController(), Player);
						Cast<AMotePlayerState>(Player->GetPlayerState())->SetHP(AddHealth, true);
						RotOffset = FRotator(-180.0, 35.0 + RandAgle, 0.0);

						if (SoundSubsystem)
							SoundSubsystem->PlayVFXSound(TEXT("Hit3"), 3.f);
						break;
					}
					case 4:
					{
						AddDmg = 20.f;
						AddHealth = 10.f;
						HitActor->TakeDamage(Damage + AddDmg, DmgEvent, Player->GetController(), Player);
						Cast<AMotePlayerState>(Player->GetPlayerState())->SetHP(AddHealth, true);
						RotOffset = FRotator(-180.0, -15.0 - RandAgle, 0.0);

						if (SoundSubsystem)
							SoundSubsystem->PlayVFXSound(TEXT("Hit4"), 3.f);
						break;
					}
					}

					FActorSpawnParameters SpawnParams;
					ADmgTextActor* DmgText = Player->GetWorld()->SpawnActor<ADmgTextActor>(ADmgTextActor::StaticClass(), 
						Hit.ImpactPoint + HitActor->GetActorUpVector() * 300.f + HitActor->GetActorRightVector() * - 50.f, FRotator::ZeroRotator, SpawnParams);
					IsHeadShot ? DmgText->InputDamage(Damage + AddDmg, true) : DmgText->InputDamage(Damage + AddDmg);


					FRotator Rot = Player->GetCameraRotator() + FRotator(0.0,-90.0,0.0);
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(Player->GetWorld(), mSlashHitEffect, Hit.ImpactPoint, Rot + RotOffset, FVector(2.0,2.0,2.0));
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(Player->GetWorld(), mHitEffect, Hit.ImpactPoint);
					Player->OffCameraShake();
				}
			}
			else
			{
				AActor* HitEnvironment = Hit.GetActor();
				if (!DamagedEnvActors.Contains(HitEnvironment))
				{
					DamagedEnvActors.Add(HitEnvironment);

					FRotator RotOffset = FRotator::ZeroRotator;
					int32 CurrentCombo = 0;
					CurrentCombo = Cast<UPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance())->GetCurrentAttackSection();

					USoundSubsystem* SoundSubsystem = Player->GetGameInstance()->GetSubsystem<USoundSubsystem>();
					if (SoundSubsystem)
					{
						switch (CurrentCombo)
						{
						case 0:
							SoundSubsystem->PlayVFXSound(TEXT("Hit0"), 3.f);
							RotOffset = FRotator(0.0, 10.0, 0.0);
							break;
						case 1:
							SoundSubsystem->PlayVFXSound(TEXT("Hit1"), 3.f);
							RotOffset = FRotator(-180.0, -15.0, 0.0);
							break;
						case 2:
							SoundSubsystem->PlayVFXSound(TEXT("Hit2"), 3.f);
							RotOffset = FRotator(0.0, 0.0, 0.0);
							break;
						case 3:
							SoundSubsystem->PlayVFXSound(TEXT("Hit3"), 3.f);
							RotOffset = FRotator(-180.0, 35.0, 0.0);
							break;
						case 4:
							SoundSubsystem->PlayVFXSound(TEXT("Hit4"), 3.f);
							RotOffset = FRotator(-180.0, -15.0, 0.0);
							break;
						}

						FRotator Rot = Player->GetCameraRotator() + FRotator(0.0, -90.0, 0.0);
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(Player->GetWorld(), mHitEffect, Hit.ImpactPoint, Rot + RotOffset);
						Player->OffCameraShake();
					}
				}
			}
			count++;
		}
	}
	HitResults.Empty();
	DamagedActors.Empty();
	DamagedEnvActors.Empty();
}