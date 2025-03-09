// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Golem/GolemAnimNotifyStateLazer.h"
#include "../../AICharacterBase.h"
#include "../../Monster/Golem/Golem.h"
#include "AIController.h"
#include "../../../Player/PlayerCharacter.h"

void UGolemAnimNotifyStateLazer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOwner())
	{
		AGolem* Golem = Cast<AGolem>(MeshComp->GetOwner());
		if (Golem)
		{
			AAIController* Control = Golem->GetController<AAIController>();
			if(Control)
			{
				bool IsStagger = Control->GetBlackboardComponent()->GetValueAsBool(TEXT("Stagger"));
				if (IsStagger) return;
			}

			if (mCanBurn) return;
			mCanBurn = true;

			if (!mChargeEffect)
			{
				UNiagaraSystem* ChargeAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/MegaMagicVFXBundle/VFX/MagicalExplosionsVFX/VFX/ChaosExplosion/Systems/N_ChaosExplosionCharged_Edit.N_ChaosExplosionCharged_Edit'"));
				if (ChargeAsset)
					mChargeEffect = ChargeAsset;
			}

			if (!mBeamEffect)
			{
				UNiagaraSystem* AttackAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/EnergyBeam/NS/NS_FireTongueLick.NS_FireTongueLick'"));
				if (AttackAsset)
					mBeamEffect = AttackAsset;
			}

			if (!mGroundEffect)
			{
				UNiagaraSystem* GroundAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/EnergyBeam/NS/NS_Hit_Constant_Fire.NS_Hit_Constant_Fire'"));
				if (GroundAsset)
					mGroundEffect = GroundAsset;
			}


			mBeamComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(mBeamEffect, MeshComp, "LazerSocket", FVector(0, 0, 0), FRotator(0, 0, 0), EAttachLocation::SnapToTarget, true);
			if (mBeamComponent.IsValid())
			{
				mPrevRot = mBeamComponent->GetComponentRotation();

				mBeamComponent->SetNiagaraVariableFloat(TEXT("User._Length"), 4.5f);
				mBeamComponent->SetNiagaraVariableFloat(TEXT("User._Size"), 1.8f);
				mBeamComponent->SetNiagaraVariableFloat(TEXT("User._Width"), 1.5f);
				mBeamComponent->SetNiagaraVariableFloat(TEXT("User.AirDistortStrength"), 0.06f);
			}
		}
	}

}

void UGolemAnimNotifyStateLazer::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	AGolem* Golem = Cast<AGolem>(MeshComp->GetOwner());
	if (Golem && mBeamComponent.IsValid())
	{
		if (mCanBurn)
		{
			FVector PlayerLoc;
			APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(Golem->GetWorld(), 0));
			if (IsValid(PlayerController))
			{
				APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
				if (Player)
				{
					PlayerLoc = Player->GetActorLocation() - Player->GetActorUpVector() * 50.f;
				}
			}

			FVector Loc;
			FRotator Rot;
			MeshComp->GetSocketWorldLocationAndRotation("LazerSocket", Loc, Rot);
			FVector Start = Loc;

			// 빔 방향 변경
			FRotator PrevRot = mPrevRot;
			FRotator TargetRot = (PlayerLoc - Start).GetSafeNormal().Rotation();

			const FRotator InterpToRotator = FMath::RInterpTo(PrevRot, TargetRot, FrameDeltaTime, 3.5f);
			
			mBeamComponent->SetWorldRotation(InterpToRotator);
			mPrevRot = InterpToRotator;

			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(Golem);

			FVector End = Start + InterpToRotator.Vector().GetSafeNormal() * 15000.f;
			bool bHit = MeshComp->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel8, CollisionParams);

			if (bHit)
			{
				LastSpawnTime += FrameDeltaTime;
				if (LastSpawnTime > SpawnInterval)
				{
					LastSpawnTime = 0.f;
					APlayerCharacter* Player = Cast<APlayerCharacter>(HitResult.GetActor());
					if (Player)
					{
						Player->LaunchCharacter(InterpToRotator.Vector().GetSafeNormal() * 500.f, true, true);

						FDamageEvent DmgEvent;

						UNiagaraComponent* BurnPlayer = UNiagaraFunctionLibrary::SpawnSystemAttached(
							mGroundEffect, 
							Player->GetMesh(),"root", 
							FVector(0, -70, 0), 
							FRotator(0, 0, -90), 
							EAttachLocation::KeepRelativeOffset, 
							true);

						BurnPlayer->SetNiagaraVariableFloat(TEXT("User._Size"), 2.7f);

						// 상태이상 동안 0.5f동안 데미지
						Player->TakeDamage(1.f, DmgEvent, Golem->GetController(), Golem);
						Player->SetScreenFireDamage(true);
						Player->SetScreenDamage(true);

						if (!Player->IsDead())
						{
							Golem->GetWorld()->GetTimerManager().SetTimer(StatusEffectTimerHandle, [this, Golem, Player, DmgEvent]()
								{
									Player->TakeDamage(1.f, DmgEvent, Golem->GetController(), Golem);
								}, 0.5f, true);

							// 상태이상 종료되면 비활성화
							Golem->GetWorld()->GetTimerManager().SetTimer(StatusEffectEndTimerHandle, [this, Golem, Player]()
								{
									Player->SetScreenFireDamage(false);

									if (!Player->GetIsLowHelth())
										Player->SetScreenDamage(false);
									Golem->GetWorld()->GetTimerManager().ClearTimer(StatusEffectTimerHandle);
								}, Duration, false);
						}
					}
					else
					{
						UNiagaraComponent* Ground = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
							Golem->GetWorld(), 
							mGroundEffect, 
							HitResult.ImpactPoint, 
							FRotator::ZeroRotator);

						Ground->SetNiagaraVariableFloat(TEXT("User._Size"), 2.7f);
					}
				}

			}
		}
	}

}

void UGolemAnimNotifyStateLazer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (mBeamComponent.IsValid())
	{
		// 컴포넌트 비활성화 또는 제거
		mBeamComponent->Deactivate();
	}

	// 변수초기화
	mCanBurn = false;
	LastSpawnTime = 0.f;
}
