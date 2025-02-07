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

		if (bHit)
		{
			CrashPos = HitResult.Location;
			
			transform.SetLocation(CrashPos);
			transform.SetRotation(FRotator::ZeroRotator.Quaternion());
			transform.SetScale3D(FVector(3.f, 5.5f, 5.5f));

			// µ¥Ä® ¿¢ÅÍ ½ºÆù
			ATelegraph* Telegraph = Cast<ATelegraph>(MeshComp->GetWorld()->SpawnActor<ATelegraph>(ATelegraph::StaticClass(), transform, SpawnParams));
			if (Telegraph)
			{
				Telegraph->SetShape(ETelegraphShape::Circle);
				Telegraph->SetLifeTime(0.7f);
			}
		}
	}

}
