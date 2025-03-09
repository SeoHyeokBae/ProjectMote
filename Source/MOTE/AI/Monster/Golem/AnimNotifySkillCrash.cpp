// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Golem/AnimNotifySkillCrash.h"
#include "Golem.h"
#include "../../../Player/PlayerCharacter.h"
#include "GolemAnimInstance.h"
#include "../../../UI/ETC/Telegraph.h"
#include "../../../Sound/SoundSubsystem.h"

void UAnimNotifySkillCrash::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	{
		//UParticleSystem* FistEffectAsset = LoadObject<UParticleSystem>(nullptr, TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/SpecialAttack3/FX/P_Prime_Ground_Fist_Impact_Eidt.P_Prime_Ground_Fist_Impact_Eidt'"));
		UParticleSystem* FistEffectAsset = LoadObject<UParticleSystem>(nullptr, TEXT("/Script/Engine.ParticleSystem'/Game/FX/InfinityBladeEffects/Effects/FX_Skill_Leap/P_Skill_Leap_Fire_Impact_Suction.P_Skill_Leap_Fire_Impact_Suction'"));

		if (FistEffectAsset)
			mFistEffect = FistEffectAsset;

		UParticleSystem* AttackEffectAsset = LoadObject<UParticleSystem>(nullptr, TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/SpecialAttack3/FX/P_Prime_SpecialAttack4.P_Prime_SpecialAttack4'"));
		if (AttackEffectAsset)
			mAttackEffect = AttackEffectAsset;

		UNiagaraSystem* HitEffectAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/SlashHitAndStabHit/Particles/Niagara/NS_ky_stabHit02.NS_ky_stabHit02'"));
		if (HitEffectAsset)
			mHitEffect = HitEffectAsset;
	}

	AGolem* Golem = Cast<AGolem>(MeshComp->GetOwner());
	if (Golem)
	{
		FVector CrashPos = MeshComp->GetSocketLocation("SkillCrash");
		// 텔레그레프가 찍힌 지점
		if (Golem->GetCrashHitPos() != FVector::Zero())
		{
			CrashPos = Golem->GetCrashHitPos() + FVector(0.f,0.f,1.f);
		}

		FRotator ActorRot = Golem->GetActorRightVector().Rotation() + FRotator(0, 45, 0);

		// Fist collision
		UGameplayStatics::SpawnEmitterAtLocation(Golem->GetWorld(), mFistEffect, CrashPos, ActorRot , FVector(5, 5, 5));

		TArray<FHitResult> HitResults;
		TSet<APlayerCharacter*> DamagedActors;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Golem);

		float Radius = 1500.f;

		bool bHit = MeshComp->GetWorld()->SweepMultiByChannel(HitResults, CrashPos, CrashPos, FQuat::Identity
			, ECC_GameTraceChannel8, FCollisionShape::MakeSphere(Radius), CollisionParams);

		if (bHit)
		{
			for (const FHitResult& Hit : HitResults)
			{
				APlayerCharacter* player = Cast<APlayerCharacter>(Hit.GetActor());
				if (player && !DamagedActors.Contains(player))
				{
					DamagedActors.Add(player);



					// interaction 
					FVector Direction = (Hit.GetActor()->GetActorLocation() - CrashPos).GetSafeNormal();
					float Distance = FVector::Dist(CrashPos, Hit.GetActor()->GetActorLocation());
					float MaxDistance = Radius;
					float ExplosionStrength = 5000.0f;
					float StrengthMultiplier = FMath::Clamp(1.0f - (Distance / MaxDistance), 0.0f, 1.0f);
					float FinalStrength = ExplosionStrength * StrengthMultiplier;
					
					// Hit Effect
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(Golem->GetWorld(), mHitEffect, player->GetActorLocation(), FRotator::ZeroRotator, FVector(2.f, 2.f, 2.f));

					player->LaunchCharacter(Direction * FinalStrength, true, true);

					// damage
					Golem->HitStopStart();
					FDamageEvent DmgEvent;
					player->TakeDamage(Golem->mAttack, DmgEvent, Golem->GetController(), Golem);
				}

//#if ENABLE_DRAW_DEBUG
//				FColor DrawColor = player ? FColor::Red : FColor::Green;
//				DrawDebugSphere(MeshComp->GetWorld(), CrashPos, Radius, 24, DrawColor, false, 1.f);
//#endif
			}
		}

		APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(Golem->GetWorld(), 0));

		if (IsValid(PlayerController))
		{
			PlayerController->PlayerCameraManager->StartCameraShake(Cast<UGolemAnimInstance>(MeshComp->GetAnimInstance())->GetGolemShakeBase(int32(EMonsterShakeType::StrongAttack)));
			APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
			if (Player)
			{
				DrawTelegraph(MeshComp, Player, Golem);
			}
		}
		HitResults.Empty();
		DamagedActors.Empty();
	}
}

void UAnimNotifySkillCrash::DrawTelegraph(USkeletalMeshComponent* MeshComp, class APlayerCharacter* Player, class AGolem* Golem)
{
	if (!Player || !Golem)
		return;

	FVector AttackLoc;
	FVector AttackDir;
	FVector AttackNormal;
	FRotator AttackRot;
	FTransform transform;
	
	// telegraph 데칼 그릴 위치 선점
	FVector Start = Player->GetActorLocation();
	FVector End = Start + (Player->GetActorUpVector() * -30000.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Player);

	FHitResult HitResult;
	bool bHit = MeshComp->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel10, QueryParams);

	if (bHit)
	{
		AttackNormal = HitResult.ImpactNormal;
		AttackLoc = HitResult.ImpactPoint;
		AttackDir = Player->GetActorForwardVector();
		AttackRot = AttackDir.Rotation();
	}

	transform.SetLocation(AttackLoc);
	transform.SetRotation(AttackRot.Quaternion());
	transform.SetScale3D(FVector(3.f, 0.8f, 2.5f));
	
	FTimerHandle TimerHandle;
	FActorSpawnParameters SpawnParams;

	// 데칼 엑터 스폰
	AActor* Telegraph = MeshComp->GetWorld()->SpawnActor<ATelegraph>(ATelegraph::StaticClass(), transform, SpawnParams);

	// 0.5초 후에 스킬스폰 및 Collision
	if (Player->IsDead()) return;
	MeshComp->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, MeshComp, Player, Golem, AttackDir, Telegraph, AttackLoc, AttackRot]()
		{
			UGameplayStatics::SpawnEmitterAtLocation(MeshComp->GetWorld(),mAttackEffect, AttackLoc, AttackRot, FVector(8, 8, 8));
			APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
			if(PlayerController)
				PlayerController->PlayerCameraManager->StartCameraShake(Cast<UGolemAnimInstance>(MeshComp->GetAnimInstance())->GetGolemShakeBase(int32(EMonsterShakeType::StrongAttack)));

			USoundSubsystem* ExPlosionSFX = Golem->GetGameInstance()->GetSubsystem<USoundSubsystem>();
			if (ExPlosionSFX)
			{
				ExPlosionSFX->PlayVFXSound(TEXT("CrashExplosion"), 0.4f, Telegraph->GetActorLocation());
				ExPlosionSFX->PlayVFXSound(TEXT("CrashExplosion2"), 0.7f, Telegraph->GetActorLocation());
			}
			// 충돌체 생성
			FVector Start = AttackLoc - AttackDir * 500.f;
			FVector End = Start + Telegraph->GetActorUpVector() * 1000.f;
			FVector BoxExtent(500.f, 200.0f, 1400.0f);
			// 충돌체 기울이기
			//FQuat TiltRotation = FQuat(AttackDir.YAxisVector, FMath::DegreesToRadians(25.0f));
			//FQuat SweepRotation = AttackRot.Quaternion();
			//FQuat FinalRotation = SweepRotation * TiltRotation;

			FRotator FinalRotation = AttackDir.Rotation() + FRotator(25.f, 0.f, 0.f);

			TArray<FHitResult> HitResults;
			TSet<APlayerCharacter*> DamagedActors;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(MeshComp->GetOwner());

			bool bHit = MeshComp->GetWorld()->SweepMultiByChannel(HitResults, Start, End, FinalRotation.Quaternion()
				, ECC_GameTraceChannel8, FCollisionShape::MakeBox(BoxExtent),CollisionParams);
			
			if (bHit)
			{
				for (const FHitResult& Hit : HitResults)
				{
					APlayerCharacter* player = Cast<APlayerCharacter>(Hit.GetActor());
					if (player && !DamagedActors.Contains(player))
					{
						DamagedActors.Add(player);

						// damage
						Golem->HitStopStart();

						FDamageEvent DmgEvent;
						player->TakeDamage(Golem->mAttack, DmgEvent, Golem->GetController(), Golem);

						// interaction 
						float Tilt = 0.28f;
						FVector Direction = ((1- Tilt) * player->GetActorUpVector() + Tilt * AttackDir).GetSafeNormal();
						float Distance = FVector::Dist(AttackLoc, Hit.Location);
						float MaxDistance = 1400.0f;
						float ExplosionStrength = 5500.0f;
						float StrengthMultiplier = FMath::Clamp(1.0f - (Distance / MaxDistance), 0.0f, 1.0f);
						float FinalStrength = ExplosionStrength * StrengthMultiplier;

						player->LaunchCharacter(Direction * FinalStrength, true, true);

						// player hit effect
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(Golem->GetWorld(), mHitEffect, Player->GetActorLocation(), FRotator::ZeroRotator, FVector(2.f, 2.f, 2.f));
					}

//#if ENABLE_DRAW_DEBUG
//					FColor DrawColor = player ? FColor::Red : FColor::Green;
//					DrawDebugBox(MeshComp->GetWorld(), (Start + End) / 2, BoxExtent, (FinalRotation - FRotator(50.f, 0.f, 0.f)).Quaternion(), DrawColor, false, 1.f);
//#endif
				}
			}
			HitResults.Empty();
			DamagedActors.Empty();

		}, 0.5f, false);
}




