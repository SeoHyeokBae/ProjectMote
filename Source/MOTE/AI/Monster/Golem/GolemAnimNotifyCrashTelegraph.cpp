// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Golem/GolemAnimNotifyCrashTelegraph.h"
#include "GolemAnimNotifyCrashTelegraph.h"
#include "Golem.h"
#include "../../../UI/ETC/Telegraph.h"


void UGolemAnimNotifyCrashTelegraph::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AGolem* Golem = Cast<AGolem>(MeshComp->GetOwner());
	if (Golem)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector CrashPos = MeshComp->GetSocketLocation("SkillCrash");

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Golem);
		FVector Start = CrashPos;
		FVector End = Start + (Golem->GetActorUpVector() * -5000.0f);

		FHitResult HitResult;

		FTransform transform;

		bool bHit = MeshComp->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel10, QueryParams);

//#if ENABLE_DRAW_DEBUG
//		if (bHit)
//		{
//			// 히트한 지점과 시작 지점을 연결하는 디버그 선 그리기
//			DrawDebugLine(MeshComp->GetWorld(), Start, HitResult.Location, FColor::Red, false, 2.0f, 0, 2.0f);
//		}
//		else
//		{
//			// 히트하지 않으면 끝 지점까지의 디버그 선 그리기
//			DrawDebugLine(MeshComp->GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);
//		}
//#endif

		if (bHit)
		{
			CrashPos = HitResult.Location;
			Golem->SetCrashHitPos(CrashPos);
			
			transform.SetLocation(CrashPos);
			transform.SetRotation(FRotator::ZeroRotator.Quaternion());
			transform.SetScale3D(FVector(3.f, 5.5f, 5.5f));

			// 데칼 엑터 스폰
			ATelegraph* Telegraph = Cast<ATelegraph>(MeshComp->GetWorld()->SpawnActor<ATelegraph>(ATelegraph::StaticClass(), transform, SpawnParams));
			if (Telegraph)
			{
				Telegraph->SetShape(ETelegraphShape::Circle);
				Telegraph->SetLifeTime(0.7f);
			}
		}
	}

}
