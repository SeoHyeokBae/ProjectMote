// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "../Sound/SoundSubsystem.h"

APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = true;

	mPortal = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Portal"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		OverlapPortalAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Map/Planet/Test/NS_OverlapPortal.NS_OverlapPortal'"));

	if (OverlapPortalAsset.Succeeded())
		mOverlapPortalFXSystem = OverlapPortalAsset.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		SmokeAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Map/Planet/Test/NS_SkillMeteorLoad.NS_SkillMeteorLoad'"));

	if (SmokeAsset.Succeeded())
		mSmokeFXSystem = SmokeAsset.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		PortalAsset(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Dragon/FX/P_Dragon_Spawn.P_Dragon_Spawn'"));

	if (PortalAsset.Succeeded())
		mPortalGolem = PortalAsset.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		PortalExplosionAsset(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Dragon/FX/P_Dragon_Spawn_End.P_Dragon_Spawn_End'"));

	if (PortalExplosionAsset.Succeeded())
		mPortalGolemExplosion = PortalExplosionAsset.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		PortalGroundAsset(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Dragon/FX/P_Dragon_Spawn_End_Ground.P_Dragon_Spawn_End_Ground'"));

	if (PortalGroundAsset.Succeeded())
		mPortalGolemGround = PortalGroundAsset.Object;


	SetRootComponent(mPortal);

	mPortal->SetCapsuleSize(900.f, 1600.f);
	mPortal->SetHiddenInGame(true);
	mPortal->SetCollisionProfileName(TEXT("Portal"));

	mPortal->OnComponentBeginOverlap.AddDynamic(this, &APortal::BeginOverlap);
	mPortal->OnComponentEndOverlap.AddDynamic(this, &APortal::EndOverlap);
}

void APortal::BeginPlay()
{
	Super::BeginPlay();

	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
		SoundSubsystem->PlayVFXSound(TEXT("Portal"), 0.75f, GetActorLocation());


	mPortalParticleComp = UGameplayStatics::SpawnEmitterAttached(mPortalGolem, mPortal, NAME_None, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), FVector(1.5, 1.5, 1.5), EAttachLocation::KeepRelativeOffset, true);
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(mOverlapPortalFXComponent))
	{
		if (mOverlapPortalFXComponent->IsActive())
		{
			mOverlapTime += DeltaTime;
		}

		if (mOverlapTime > 3.f && mCanNextLevel)
		{
			mCanNextLevel = false;

			if (mPlayer)
			{
				mPlayer->SetActorHiddenInGame(true);

				// ºÎÂøµÈ ¾×ÅÍ ¸ðµÎ ¼û±è
				TArray<AActor*> AttachedActors;
				mPlayer->GetAttachedActors(AttachedActors);

				for (AActor* AttachedActor : AttachedActors)
				{
					if (AttachedActor)
					{
						AttachedActor->SetActorHiddenInGame(true);
					}
				}
			}

			if (mPortalParticleComp && !mIsPortalEnd)
			{
				mIsPortalEnd = true;
				mPortalParticleComp->Deactivate();
				mSmokeFXComponent->Deactivate();
				UGameplayStatics::SpawnEmitterAttached(mPortalGolemExplosion, mPortal, NAME_None, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), FVector(1.5, 1.5, 1.5), EAttachLocation::KeepRelativeOffset, true);
				UGameplayStatics::SpawnEmitterAttached(mPortalGolemGround, mPortal, NAME_None, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), FVector(1.5, 1.5, 1.5), EAttachLocation::KeepRelativeOffset, true);

				USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
				if (SoundSubsystem)
				{
					SoundSubsystem->StopBGM(1.2f);
					SoundSubsystem->PlayVFXSound(TEXT("PortalEnd"), 0.5f, GetActorLocation());
				}
			}

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					UGameplayStatics::OpenLevel(GetWorld(), TEXT("BossPlanet"));
				}, 1.5f, false);
		}
	}
}

void APortal::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(mOverlapPortalFXComponent))
	{
		mOverlapPortalFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			mOverlapPortalFXSystem, mPortal, NAME_None, FVector(0.0, 0.0, -1600.0), FRotator(0.0, 0.0, 0.0), EAttachLocation::KeepRelativeOffset, true);

		mOverlapPortalFXComponent->SetColorParameter(TEXT("CustomColor"), FLinearColor(20000, 0, 0));
	}
	else
		mOverlapPortalFXComponent->Activate();

	if (!IsValid(mSmokeFXComponent))
	{

		mSmokeFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			mSmokeFXSystem, mPortal, NAME_None, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), EAttachLocation::KeepRelativeOffset, true);
	}
	else
		mSmokeFXComponent->Activate();

	mPlayer = OtherActor;
}

void APortal::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(mOverlapPortalFXComponent))
		return;
	
	mOverlapPortalFXComponent->Deactivate();

	mOverlapTime = 0.0;

	mPlayer = nullptr;
}



