// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Golem/GolemAnimNotifyAttackCollision.h"
#include "Golem.h"
#include "../../../Player/PlayerCharacter.h"
#include "../../../Sound/SoundSubsystem.h"

void UGolemAnimNotifyAttackCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// �ǰ� ���̾ư���
	UNiagaraSystem* NiagaraAttackAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/MegaMagicVFXBundle/VFX/MagicalExplosionsVFX/VFX/FlameBlast/Systems/N_FlameBlast.N_FlameBlast'"));

	if (NiagaraAttackAsset)
		mHitEffect = NiagaraAttackAsset;

	mGolem = Cast<AGolem>(MeshComp->GetOwner());
	if(mGolem)
	{
		mGolem->AddToRoot();
	}
}

void UGolemAnimNotifyAttackCollision::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (mGolem && !IsAlreadyHit)
	{
		FVector BasePoint = MeshComp->GetSocketLocation(FName("hand_r"));
		FVector Start = BasePoint + mGolem->GetActorForwardVector() * 500.f;
		FVector End = BasePoint - mGolem->GetActorForwardVector() * 1400.f;

		FCollisionQueryParams param;
		param.AddIgnoredActor(mGolem);
		TArray<FHitResult> HitResults;

		bool Collision = mGolem->GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel8, FCollisionShape::MakeSphere(600.f), param);

//#if ENABLE_DRAW_DEBUG
//			FColor DrawColor = Collision ? FColor::Red : FColor::Green;
//			DrawDebugCapsule(mGolem->GetWorld(), (Start + End) / 2.f,1400.f, 600.f,FRotationMatrix::MakeFromZ(End - Start).ToQuat(), DrawColor, false, 0.5f);
//#endif
		// �浹ó�� ���
		if (Collision)
		{
			for (const FHitResult& Hit : HitResults)
			{
				APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(Hit.GetActor());
				if (HitPlayer)
				{
					IsAlreadyHit = true;

					USoundSubsystem* ExPlosionSFX = mGolem->GetGameInstance()->GetSubsystem<USoundSubsystem>();
					if (ExPlosionSFX)
					{

						ExPlosionSFX->PlayVFXSound(TEXT("BossHit"), 0.7f, Hit.ImpactPoint);
					}
					// ������ ����
					mGolem->HitStopStart();

					FDamageEvent DmgEvent;
					HitPlayer->TakeDamage(mGolem->mAttack, DmgEvent, mGolem->GetController(), mGolem);

					// ���ݿ��� Launch ����
					if (mGolem->GetGolemState() == EGolemState::Attack)
					{
						FVector LaunchDirection;

						if (HitPlayer->GetGravityDirection() != FVector(0.0f, 0.0f, -1.0f))
						{
							float PlayerHeight = abs(FVector::DotProduct(HitPlayer->GetActorLocation(), HitPlayer->GetGravityDirection()));
							float ImpactHeight = abs(FVector::DotProduct(Hit.ImpactPoint, HitPlayer->GetGravityDirection()));

							if (PlayerHeight < ImpactHeight)
							{
								float DeltaHeight = ImpactHeight - PlayerHeight;
								LaunchDirection = (HitPlayer->GetActorLocation() - (DeltaHeight + 25.f) * HitPlayer->GetGravityDirection() - Hit.ImpactPoint).GetSafeNormal();
								LaunchDirection += mGolem->GetActorForwardVector() * 0.5;
							}
							else
							{
								LaunchDirection = (HitPlayer->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal();
							}
						}
						else
						{
							if (HitPlayer->GetActorLocation().Z < Hit.ImpactPoint.Z)
							{
								LaunchDirection = (FVector(HitPlayer->GetActorLocation().X - Hit.ImpactPoint.X,
									HitPlayer->GetActorLocation().Y - Hit.ImpactPoint.Y, 15.f)).GetSafeNormal();

								LaunchDirection += mGolem->GetActorForwardVector() * 0.5;
							}
							else
							{
								LaunchDirection = (HitPlayer->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal();
							}
						}

						HitPlayer->LaunchCharacter(LaunchDirection * 3000.f, true, true);
					}

					UNiagaraFunctionLibrary::SpawnSystemAtLocation(mGolem->GetWorld(), mHitEffect, Hit.ImpactPoint);
					break;
				}
				//if (HitPlayer && !DamagedActors.Contains(HitPlayer)) // �̹� �������� �� ���ʹ� ����
				//{
				//	USoundSubsystem* ExPlosionSFX = mGolem->GetGameInstance()->GetSubsystem<USoundSubsystem>();
				//	if (ExPlosionSFX)
				//	{

				//		ExPlosionSFX->PlayVFXSound(TEXT("BossHit"), 0.7f, Hit.ImpactPoint);
				//	}


				//	// ������ ����
				//	mGolem->HitStopStart();

				//	FDamageEvent DmgEvent;
				//	HitPlayer->TakeDamage(mGolem->mAttack, DmgEvent, mGolem->GetController(), mGolem);

				//	// ���ݿ��� Launch ����
				//	if (mGolem->GetGolemState() == EGolemState::Attack)
				//	{
				//		FVector LaunchDirection;

				//		if (HitPlayer->GetPlanetGravityDir() != FVector(0.0f, 0.0f, -1.0f))
				//		{
				//			float PlayerHeight = abs(FVector::DotProduct(HitPlayer->GetActorLocation(), HitPlayer->GetGravityDirection()));
				//			float ImpactHeight = abs(FVector::DotProduct(Hit.ImpactPoint, HitPlayer->GetGravityDirection()));

				//			if (PlayerHeight < ImpactHeight)
				//			{
				//				float DeltaHeight = ImpactHeight - PlayerHeight;
				//				LaunchDirection = (HitPlayer->GetActorLocation() - (DeltaHeight + 25.f) * HitPlayer->GetPlanetGravityDir() - Hit.ImpactPoint).GetSafeNormal();
				//				LaunchDirection += mGolem->GetActorForwardVector() * 0.5;
				//			}
				//			else
				//			{
				//				LaunchDirection = (HitPlayer->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal();
				//			}
				//		}
				//		else
				//		{
				//			if (HitPlayer->GetActorLocation().Z < Hit.ImpactPoint.Z)
				//			{
				//				LaunchDirection = (FVector(HitPlayer->GetActorLocation().X - Hit.ImpactPoint.X,
				//					HitPlayer->GetActorLocation().Y - Hit.ImpactPoint.Y, 15.f)).GetSafeNormal();

				//				LaunchDirection += mGolem->GetActorForwardVector() * 0.5;
				//			}
				//			else
				//			{
				//				LaunchDirection = (HitPlayer->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal();
				//			}
				//		}

				//		HitPlayer->LaunchCharacter(LaunchDirection * 3000.f, true, true);
				//	}

				//	// ������ ���� ���� ����
				//	DamagedActors.Add(HitPlayer);

				//	// �� ���Ϳ� �ٽ� �������� �� �� �ֵ��� Ÿ�̸� ����
				//	//mGolem->GetWorld()->GetTimerManager().SetTimer(DamageResetTimerHandle, this, &APlayerCharacter::ResetDamagedActors, DamageResetTime, false);

				//// ����Ʈ ����
				//	UNiagaraFunctionLibrary::SpawnSystemAtLocation(mGolem->GetWorld(), mHitEffect, Hit.ImpactPoint);
				//}
			}
			HitResults.Empty();
		}
		
	}
}

void UGolemAnimNotifyAttackCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	IsAlreadyHit = false;
	//if (!DamagedActors.IsEmpty())
	//{
	//	DamagedActors.Empty();
	//}
	
	
	// ��ü�� �� �̻� ��ȿ���� �ʴٸ� ���� ����
	if (mGolem)
	{
		mGolem->RemoveFromRoot();  // �۾��� ������ ��Ʈ ���տ��� ����
		mGolem = nullptr;
	}
}
