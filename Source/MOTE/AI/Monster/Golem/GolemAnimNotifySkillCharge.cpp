// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Golem/GolemAnimNotifySkillCharge.h"
#include "Golem.h"
#include "../../../Player/PlayerCharacter.h"
#include "GolemAnimInstance.h"


void UGolemAnimNotifySkillCharge::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	UParticleSystem* ExplosionEffectAsset = LoadObject<UParticleSystem>(nullptr, TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/SpecialAttack2/FX/P_PH_Shockwave_V2_Edit.P_PH_Shockwave_V2_Edit'"));
	if (ExplosionEffectAsset)
		mExplosionEffect = ExplosionEffectAsset;

	UNiagaraSystem* HitEffectAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/SlashHitAndStabHit/Particles/Niagara/NS_ky_stabHit02.NS_ky_stabHit02'"));
	if (HitEffectAsset)
		mHitEffect = HitEffectAsset;

	AGolem* Golem = Cast<AGolem>(MeshComp->GetOwner());
	if (Golem)
	{
		FDamageEvent Heal;
		Golem->TakeDamage(-100.f, Heal, Golem->GetController(), Golem);

		FVector Pos = MeshComp->GetSocketLocation("spine_03");

		UGameplayStatics::SpawnEmitterAtLocation(Golem->GetWorld(), mExplosionEffect, Pos, FRotator::ZeroRotator, FVector(8, 8, 8));

		TArray<FHitResult> HitResults;
		TSet<APlayerCharacter*> DamagedActor;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Golem);

		float Radius = 3100.f;

		bool bHit = MeshComp->GetWorld()->SweepMultiByChannel(HitResults, Pos, Pos, FQuat::Identity
			, ECC_GameTraceChannel8, FCollisionShape::MakeSphere(Radius), CollisionParams);

		APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(Golem->GetWorld(), 0));

		if (IsValid(PlayerController))
		{
			PlayerController->PlayerCameraManager->StartCameraShake(Cast<UGolemAnimInstance>(MeshComp->GetAnimInstance())->GetGolemShakeBase(int32(EMonsterShakeType::StrongAttack)));
		}

		if (bHit)
		{
			for (const FHitResult& Hit : HitResults)
			{
				APlayerCharacter* player = Cast<APlayerCharacter>(Hit.GetActor());
				if (player && !DamagedActor.Contains(player))
				{
					DamagedActor.Add(player);
					// damage
					Golem->HitStopStart();

					FDamageEvent DmgEvent;
					player->TakeDamage(Golem->mAttack, DmgEvent, Golem->GetController(), Golem);

					// interaction 
					FVector Direction = (Hit.GetActor()->GetActorLocation() - Pos).GetSafeNormal();
					float ExplosionStrength = 3500.0f;

					FVector LaunchDirection = Direction;
					if (player->GetGravityDirection() != FVector(0.0f, 0.0f, -1.0f))
					{
						float PlayerHeight = abs(FVector::DotProduct(player->GetActorLocation(), player->GetGravityDirection()));
						float ImpactHeight = abs(FVector::DotProduct(Hit.ImpactPoint, player->GetGravityDirection()));

						if (PlayerHeight < ImpactHeight)
						{
							ExplosionStrength = 2500.f;
							float DeltaHeight = ImpactHeight - PlayerHeight;
							LaunchDirection = (player->GetActorLocation() - (DeltaHeight + 25.f) * player->GetGravityDirection() - Hit.ImpactPoint).GetSafeNormal();
							LaunchDirection += Golem->GetActorForwardVector() * 0.5;
						}
					}
					else if(player->GetActorLocation().Z < Hit.ImpactPoint.Z)
					{
						ExplosionStrength = 2500.f;
						LaunchDirection = (FVector(player->GetActorLocation().X - Hit.ImpactPoint.X,
							player->GetActorLocation().Y - Hit.ImpactPoint.Y, 15.f)).GetSafeNormal();

						LaunchDirection += Golem->GetActorForwardVector() * 0.5;
					}
					//Direction.Normalize();
					float Distance = FVector::Dist(Pos, Hit.GetActor()->GetActorLocation());
					float MaxDistance = Radius;
					float StrengthMultiplier = FMath::Clamp(1.0f - (Distance / MaxDistance), 0.7f, 1.0f);
					float FinalStrength = ExplosionStrength * StrengthMultiplier;

					// Hit Effect
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(Golem->GetWorld(), mHitEffect, player->GetActorLocation(), FRotator::ZeroRotator, FVector(2.f, 2.f, 2.f));

					player->LaunchCharacter(LaunchDirection * FinalStrength, true, true);
				}

//#if ENABLE_DRAW_DEBUG
//				FColor DrawColor = player ? FColor::Red : FColor::Green;
//				DrawDebugSphere(MeshComp->GetWorld(), Pos, Radius, 24, DrawColor, false, 1.f);
//#endif
			}
		}
		HitResults.Empty();
		DamagedActor.Empty();
	}
}
