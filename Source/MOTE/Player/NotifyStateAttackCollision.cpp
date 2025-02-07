

#include "Player/NotifyStateAttackCollision.h"
#include "PlayerCharacter.h"
#include "BaseWeapon.h"
#include "Components/ArrowComponent.h"
#include "AI/AIMonsterBase.h"
#include "Player/MotePlayerState.h"
#include "Player/PlayerAnimInstance.h"

void UNotifyStateAttackCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UNiagaraSystem* NiagaraAttackAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/Player/FX/NS_PlayerAttackCollisionFX.NS_PlayerAttackCollisionFX'"));

	if (NiagaraAttackAsset)
		mHitEffect = NiagaraAttackAsset;

	mPlayer = Cast<APlayerCharacter>(MeshComp->GetOwner());
}

void UNotifyStateAttackCollision::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if(mPlayer)
	{
		if (mPlayer->GetWeapon())
		{
			ABaseWeapon* Weapon = mPlayer->GetWeapon();
			FVector Start = Weapon->GetBasePoint()->GetComponentLocation();
			FVector End = Weapon->GetTipPoint()->GetComponentLocation();

			FCollisionQueryParams param;
			param.AddIgnoredActor(mPlayer);
			TArray<FHitResult> HitResults;

			bool Collision = mPlayer->GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel6, FCollisionShape::MakeSphere(30.f), param);

#if ENABLE_DRAW_DEBUG
			//FColor DrawColor = Collision ? FColor::Red : FColor::Green;
			//DrawDebugCapsule(mPlayer->GetWorld(), (Start + End) / 2.f, 125.f, 30.f, FRotationMatrix::MakeFromZ(End - Start).ToQuat(), DrawColor, false, 0.2f);
#endif
			// 충돌처리 결과
			for (const FHitResult& Hit : HitResults)
			{
				UNiagaraComponent* EffectComp =  UNiagaraFunctionLibrary::SpawnSystemAtLocation(mPlayer->GetWorld(), mHitEffect, Hit.ImpactPoint);


				AAIMonsterBase* HitActor = Cast<AAIMonsterBase>(Hit.GetActor());
				if (HitActor)
				{
					mPlayer->HitStopStart();

					// 한 콤보에 데미지 한번만 적용
					// 대쉬, 공중 콤보는 Default로 적용
					if (!DamagedActors.Contains(HitActor))
					{
						DamagedActors.Add(HitActor);
						
						FDamageEvent	DmgEvent;

						int32 CurrentCombo = 0;
						CurrentCombo = Cast<UPlayerAnimInstance>(MeshComp->GetAnimInstance())->GetCurrentAttackSection();
						switch (CurrentCombo)
						{
						case 0:
							HitActor->TakeDamage(Cast<AMotePlayerState>(mPlayer->GetPlayerState())->mAttack, DmgEvent, mPlayer->GetController(), mPlayer);
							Cast<AMotePlayerState>(mPlayer->GetPlayerState())->SetHP(1, true);
							break;
						case 1:
							HitActor->TakeDamage(Cast<AMotePlayerState>(mPlayer->GetPlayerState())->mAttack + 2, DmgEvent, mPlayer->GetController(), mPlayer);
							Cast<AMotePlayerState>(mPlayer->GetPlayerState())->SetHP(1, true);
							break;
						case 2:
							HitActor->TakeDamage(Cast<AMotePlayerState>(mPlayer->GetPlayerState())->mAttack + 5, DmgEvent, mPlayer->GetController(), mPlayer);
							Cast<AMotePlayerState>(mPlayer->GetPlayerState())->SetHP(3, true);
							break;
						case 3:
							HitActor->TakeDamage(Cast<AMotePlayerState>(mPlayer->GetPlayerState())->mAttack + 10, DmgEvent, mPlayer->GetController(), mPlayer);
							Cast<AMotePlayerState>(mPlayer->GetPlayerState())->SetHP(5, true);
							break;
						case 4:
							HitActor->TakeDamage(Cast<AMotePlayerState>(mPlayer->GetPlayerState())->mAttack + 20, DmgEvent, mPlayer->GetController(), mPlayer);
							Cast<AMotePlayerState>(mPlayer->GetPlayerState())->SetHP(10, true);
							break;
						}
					}
				}
			}
		}
	}
}

void UNotifyStateAttackCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if(mPlayer)
		mPlayer->OffCameraShake();

	if (!DamagedActors.IsEmpty())
		DamagedActors.Empty();
}