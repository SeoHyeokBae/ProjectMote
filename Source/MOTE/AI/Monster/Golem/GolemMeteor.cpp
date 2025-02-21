// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemMeteor.h"
#include "../../../Player/PlayerCharacter.h"
#include "../../../GameMode/MainGameMode.h"
#include "../../../Controller/MotePlayerController.h"
#include "NiagaraFunctionLibrary.h"
#include "../../../Sound/SoundSubsystem.h"

// Sets default values
AGolemMeteor::AGolemMeteor()
{
	PrimaryActorTick.bCanEverTick = true;

	mMeteorCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MeteorCapsule"));
	mMeteorMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MeteorMovement"));
	mAimDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("AimDecal"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		NiagaraAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/AI/Monster/Boss/Meteor/NS_Meteor.NS_Meteor'"));

	if (NiagaraAsset.Succeeded())
		mMeteorFXSystem = NiagaraAsset.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		ExplosionFireNiagaraAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/AI/Monster/Boss/Meteor/NS_MeteorExplosionFX.NS_MeteorExplosionFX'"));

	if (ExplosionFireNiagaraAsset.Succeeded())
		mExplosionFireFX = ExplosionFireNiagaraAsset.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		ExplosionGroundNiagaraAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/AI/Monster/Boss/Meteor/NS_MeteorGroundFX.NS_MeteorGroundFX'"));

	if (ExplosionGroundNiagaraAsset.Succeeded())
		mExplosionGroundFX = ExplosionGroundNiagaraAsset.Object;

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCamera(TEXT("/Script/Engine.Blueprint'/Game/AI/Monster/Boss/Blueprint/BP_GolemDefaultShake.BP_GolemDefaultShake_C'"));
	if (ShakeCamera.Succeeded())
		mCameraShake = ShakeCamera.Class;

	{	// 데칼
		mAimMtrlInst = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr
			, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/UI/Monster/MTI_MonsterAim.MTI_MonsterAim'")));

		mAimMtrlInstDynamic = UMaterialInstanceDynamic::Create(mAimMtrlInst, this);
	}

	SetRootComponent(mMeteorCapsule);

	mMeteorCapsule->SetCapsuleSize(250.f, 250.f);
	mMeteorCapsule->SetHiddenInGame(false);
	mMeteorCapsule->SetCollisionProfileName(TEXT("GolemMeteor"));
	
	mMeteorMovement->Bounciness = 0;
	mMeteorMovement->Friction = 0;
	mMeteorMovement->ProjectileGravityScale = 0.f;
	mMeteorMovement->MaxSpeed = mMaxSpeed;

	mAimDecalComponent->SetWorldScale3D(FVector(0.0, 0.0, 0.0));
	
	mAttackDmg = 10.f;
}

// Called when the game starts or when spawned
void AGolemMeteor::BeginPlay()
{
	Super::BeginPlay();

	AMotePlayerController* PlayerController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(PlayerController))
	{
		FVector Dir = ((PlayerController->GetCharacter()->GetActorLocation()) - GetActorLocation()).GetSafeNormal();
		FVector Offset = GetActorForwardVector() * -100.f;
		SetActorRotation(Dir.Rotation());
		
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			mMeteorFXSystem, mMeteorCapsule, NAME_None, Offset, FRotator(-90.0, 0.0, 0.0), EAttachLocation::KeepRelativeOffset, true);

		mMeteorMovement->Velocity = Dir * mMinSpeed;
	}

	mAimDecalComponent->SetVisibility(true);
	mAimDecalComponent->SetMaterial(0, mAimMtrlInstDynamic);

	// 장판
	DrawDecal();
	mPrevLength = mMaxLength;

	mMeteorCapsule->OnComponentHit.AddDynamic(this, &AGolemMeteor::OnHit);
}

void AGolemMeteor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	mMeteorMovement->Velocity += mMeteorMovement->Velocity.GetSafeNormal() * mAccSpeed;

	UpdateDecalSize();
}

void AGolemMeteor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(PlayerController))
		PlayerController->PlayerCameraManager->StartCameraShake(mCameraShake);

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player && Player->GetCharacterMovement()->IsFalling())
	{
		// 플레이어를 공중에서 타격한 경우
		FVector LaunchDirection = (Player->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal();
		Player->LaunchCharacter(LaunchDirection * 2500, true, true);
		// 공중에서 폭발처리

		FDamageEvent DmgEvent;
		Player->TakeDamage(mAttackDmg, DmgEvent, Player->GetController(), this);

		// 이펙트 스폰
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mExplosionFireFX, Hit.ImpactPoint, LaunchDirection.Rotation(), FVector(6.f, 6.f, 6.f));

		// 데미지 처리
		Destroy();
	}
	else
	{
		TArray<FHitResult> HitResults;
		TSet<AActor*> DamagedActors;
		FVector ExplosionOrigin = GetActorLocation() /*+ GetActorForwardVector() * (mMeteorCapsule->GetScaledCapsuleRadius() / 2)*/;
		FCollisionShape ExplosionSphere = FCollisionShape::MakeSphere(mMeteorCapsule->GetScaledCapsuleRadius() + 200.f);

		// 구 모양의 충돌 테스트
		bool bHit = GetWorld()->SweepMultiByChannel(
			HitResults,
			ExplosionOrigin,
			ExplosionOrigin + FVector(0.1f), // 작은 오프셋
			FQuat::Identity,
			ECC_GameTraceChannel8, // 캐릭터가 속한 충돌 채널
			ExplosionSphere);

		if (bHit)
		{
			for (auto& HitResult : HitResults)
			{
				Player = Cast<APlayerCharacter>(HitResult.GetActor());
				if (Player && !DamagedActors.Contains(Player))
				{
					DamagedActors.Add(Player);
					FVector LaunchDirection;

					if (Player->GetGravityDirection() != FVector(0.0f, 0.0f, -1.0f))
					{
						float PlayerHeight = abs(FVector::DotProduct(Player->GetActorLocation(), Player->GetGravityDirection()));
						float ImpactHeight = abs(FVector::DotProduct(Hit.ImpactPoint, Player->GetGravityDirection()));

						if (PlayerHeight < ImpactHeight)
						{
							float DeltaHeight = ImpactHeight - PlayerHeight;
							LaunchDirection = (Player->GetActorLocation() - (DeltaHeight + 15.f) * Player->GetGravityDirection() - Hit.ImpactPoint).GetSafeNormal();
						}
						else
							LaunchDirection = (Player->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal();
					}
					else
					{
						if (Player->GetActorLocation().Z < Hit.ImpactPoint.Z)
						{
							LaunchDirection = (FVector(Player->GetActorLocation().X - Hit.ImpactPoint.X,
								Player->GetActorLocation().Y - Hit.ImpactPoint.Y, 15.f)).GetSafeNormal();
						}
						else
							LaunchDirection = (Player->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal();
					}
					Player->LaunchCharacter(LaunchDirection * 2500, true, true);

					FDamageEvent DmgEvent;
					Player->TakeDamage(mAttackDmg, DmgEvent, Player->GetController(), this);
				}
			}
		}

		AMotePlayerController* MoteController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		FVector Dir = Cast<APlayerCharacter>(MoteController->GetCharacter())->GetGravityDirection();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mExplosionFireFX, Hit.ImpactPoint - Dir * 10, Dir.Rotation() - FRotator(90.0, 0.0, 0.0), FVector(6.f, 6.f, 6.f));
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mExplosionGroundFX, Hit.ImpactPoint - Dir * 10, Dir.Rotation() - FRotator(90.0, 0.0, 0.0), FVector(6.f, 6.f, 6.f));

		USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
		if (SoundSubsystem)
			SoundSubsystem->PlayVFXSound(TEXT("MeteorExplosion"),1.0f, Hit.ImpactPoint);

		Destroy();
	}
}

void AGolemMeteor::DrawDecal()
{
	// 라인트레이스로 그려줄 위치 정함
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * 50000.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel10, QueryParams);

	if (bHit)
	{
		FVector		AimLocation = HitResult.Location;
		FRotator	AimRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();

		mArrivalPoint = AimLocation;
		mMaxLength = (Start - AimLocation).Length();

		mAimDecalComponent->SetWorldLocationAndRotation(AimLocation, AimRotation);

		DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Red, false, 1.0f, 0, 1.0f);
	}
}

void AGolemMeteor::UpdateDecalSize()
{
	float CurrentLength = (mArrivalPoint - GetActorLocation()).Length();
	mDeltaLength += mPrevLength - CurrentLength;
	mPrevLength = CurrentLength;

	float NewScale = (mDeltaLength / mMaxLength) * 1.2;

	mAimDecalComponent->SetWorldScale3D(FVector(NewScale, NewScale, NewScale));
}
