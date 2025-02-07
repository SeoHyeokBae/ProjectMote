// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Golem/GolemAnimNotifyLazer.h"
#include "Golem.h"
#include "../../../Player/PlayerCharacter.h"
#include "GolemAnimInstance.h"
#include "../../../Sound/SoundSubsystem.h"

void UGolemAnimNotifyLazer::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	UParticleSystem* MuzzleEffectAsset = LoadObject<UParticleSystem>(nullptr, TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/PrimaryAttack/FX/P_PrimeMuzzle.P_PrimeMuzzle'"));

	if (MuzzleEffectAsset)
		mMuzzleEffect = MuzzleEffectAsset;

	UParticleSystem* AttackEffectAsset = LoadObject<UParticleSystem>(nullptr, TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/PrimaryAttack/FX/P_PrimeHelix_Attack.P_PrimeHelix_Attack'"));
	if (AttackEffectAsset)
		mAttackEffect = AttackEffectAsset;

	UNiagaraSystem* ExplosionEffectAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/ExplosionPack/Niagara/NS_Explosion_System11.NS_Explosion_System11'"));
	if (ExplosionEffectAsset)
		mExplosionEffect = ExplosionEffectAsset;

	UNiagaraSystem* HitEffectAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/ExplosionPack/Niagara/NS_Explosion_System8.NS_Explosion_System8'"));
	if (HitEffectAsset)
		mHitEffect = HitEffectAsset;

	AGolem* Golem = Cast<AGolem>(MeshComp->GetOwner());
	if (Golem)
	{
		float Radius = 300.0f; // Ä¸½¶ ¹ÝÁö¸§
		float HalfHeight = 10000.0f; // Ä¸½¶ ³ôÀÌ

		FVector MuzzlePos = MeshComp->GetSocketLocation("LazerSocket");
		FVector EffectScale = FVector(12, 12, 12);
		FRotator AttackRot = Golem->GetLaserAimRot();

		USoundSubsystem* SoundSubsystem = Golem->GetGameInstance()->GetSubsystem<USoundSubsystem>();
		if (SoundSubsystem)
			SoundSubsystem->PlayVFXSound(TEXT("Laser"), 1.0f, MuzzlePos);

		// ·¹ÀÌÁ®ºö ÀÌÆåÆ®
		UParticleSystemComponent* MuzzleParticle = UGameplayStatics::SpawnEmitterAtLocation(Golem->GetWorld(), mMuzzleEffect, MuzzlePos, AttackRot, EffectScale);
		UParticleSystemComponent* AttackParticle = UGameplayStatics::SpawnEmitterAtLocation(Golem->GetWorld(), mAttackEffect, MuzzlePos, AttackRot, EffectScale);

		FVector Start = MuzzlePos;
		FVector End = MuzzlePos + AttackRot.Vector().GetSafeNormal() * HalfHeight;

		TArray<FHitResult> HitResults;
		TSet<AActor*> DamagedActors;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Golem);

		bool bHit = MeshComp->GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, ECC_GameTraceChannel11, CollisionParams);
		//DrawDebugLine(Golem->GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 1.0f);

		for (const FHitResult& Hit : HitResults)
		{
			APlayerCharacter* Player = Cast<APlayerCharacter>(Hit.GetActor());
			if (Player && !DamagedActors.Contains(Player))
			{
				DamagedActors.Add(Player);

				// damage
				Golem->HitStopStart();

				FDamageEvent DmgEvent;
				Player->TakeDamage(Golem->mAttack, DmgEvent, Golem->GetController(), Golem);

				mDamaged = true;

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(Golem->GetWorld(), mHitEffect, Hit.ImpactPoint, FRotator::ZeroRotator, FVector(2.f, 2.f, 2.f));
				Player->LaunchCharacter(AttackRot.Vector().GetSafeNormal() * 3500.f, true, true);
			}
			else
			{
				AActor* NonePlayer = Hit.GetActor();

				if (!DamagedActors.Contains(NonePlayer))
				{
					DamagedActors.Add(NonePlayer);
					// 0.3ÃÊ ÈÄ¿¡ ½ºÅ³½ºÆù ¹× Collision
					FTimerHandle TimerHandle;
					MeshComp->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, MeshComp, Player, Hit, Golem]()
						{
							USoundSubsystem* SoundSubsystem = Golem->GetGameInstance()->GetSubsystem<USoundSubsystem>();
							if (SoundSubsystem)
								SoundSubsystem->PlayVFXSound(TEXT("LaserExplosion"), 1.0f, Hit.ImpactPoint);

							UNiagaraFunctionLibrary::SpawnSystemAtLocation(Golem->GetWorld(), mExplosionEffect, Hit.ImpactPoint, FRotator::ZeroRotator, FVector(3.f, 3.f, 3.f));

							TArray<FHitResult> ExplosionHitResults;
							TSet<APlayerCharacter*> DamagedPlayer;
							float Radius = 350.f;
							FCollisionQueryParams CollisionParams;
							bool bHit = MeshComp->GetWorld()->SweepMultiByChannel(ExplosionHitResults, Hit.ImpactPoint, Hit.ImpactPoint, FQuat::Identity
								, ECC_GameTraceChannel8, FCollisionShape::MakeSphere(Radius), CollisionParams);

							if (bHit)
							{
								for (const FHitResult& ExPlosionHit : ExplosionHitResults)
								{
									APlayerCharacter* player = Cast<APlayerCharacter>(ExPlosionHit.GetActor());
									if (player && !DamagedPlayer.Contains(player))
									{
										DamagedPlayer.Add(player);

										// damage
										Golem->HitStopStart();

										FDamageEvent DmgEvent;
										player->TakeDamage(Golem->mAttack, DmgEvent, Golem->GetController(), Golem);

										// interaction 
										FVector Direction = (ExPlosionHit.GetActor()->GetActorLocation() - ExPlosionHit.ImpactPoint).GetSafeNormal();
										//Direction.Normalize();
										float Distance = FVector::Dist(ExPlosionHit.ImpactPoint, ExPlosionHit.GetActor()->GetActorLocation());
										float MaxDistance = Radius;
										float ExplosionStrength = 3000.0f;
										float StrengthMultiplier = FMath::Clamp(1.0f - (Distance / MaxDistance), 0.0f, 1.0f);
										float FinalStrength = ExplosionStrength * StrengthMultiplier;

										// Hit Effect
										UNiagaraFunctionLibrary::SpawnSystemAtLocation(Golem->GetWorld(), mHitEffect, player->GetActorLocation(), FRotator::ZeroRotator, FVector(2.f, 2.f, 2.f));

										player->LaunchCharacter(Direction * FinalStrength, true, true);

									}
								}
							}
						}, 0.3f, false);
				}

			}
		}
		DamagedActors.Empty();
		HitResults.Empty();
	}
}

