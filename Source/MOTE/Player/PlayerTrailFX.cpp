

#include "PlayerTrailFX.h"
#include "Components/WidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"

APlayerTrailFX::APlayerTrailFX()
{
	PrimaryActorTick.bCanEverTick = true;

	mTrailComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trail"));
	SetRootComponent(mTrailComponent);
	mTrailComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Dash FX
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		NiagaraDashAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Player/FX/NS_PlayerDashFX.NS_PlayerDashFX'"));

	if (NiagaraDashAsset.Succeeded())
		mDashFXSystem = NiagaraDashAsset.Object;

	// Float FX
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		NiagaraFloatAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Player/FX/NS_PlayerFloatFX.NS_PlayerFloatFX'"));

	if (NiagaraFloatAsset.Succeeded())
		mFloatFXSystem = NiagaraFloatAsset.Object;

	// Shield FX
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		NiagaraShieldAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/MegaMagicVFXBundle/VFX/MagicShieldsVFX/VFX/DefaultVersions/LightningShield/Systems/N_LightningShield.N_LightningShield'"));

	if (NiagaraShieldAsset.Succeeded())
		mShieldFXSystem = NiagaraShieldAsset.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		AfterExplosionAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Player/FX/NS_PlayerExplosionFX.NS_PlayerExplosionFX'"));

	if (AfterExplosionAsset.Succeeded())
		mAfterExplosionEffect = AfterExplosionAsset.Object;
}


void APlayerTrailFX::OnDashActive()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player)
	{
		return;
	}

	if (!IsValid(mDashFXComponent))
	{
		mDashFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			mDashFXSystem, Player->GetArrow(), NAME_None, Player->GetArrow()->GetRelativeLocation()
			, FRotator(0.f, 0.f, 0.f), EAttachLocation::KeepRelativeOffset, true);
	}
	else
	{
		mDashFXComponent->Activate();
	}

	mDashLifeTime = 0.f;
}

void APlayerTrailFX::OnFloatActive()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player)
		return;

	if (!IsValid(mFloatFXComponent))
	{
		mFloatFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			mFloatFXSystem, Player->GetArrow(), NAME_None, Player->GetArrow()->GetRelativeLocation()
			, FRotator(0.f, 0.f, 0.f), EAttachLocation::KeepRelativeOffset, true);
	}
	else
	{
		mFloatFXComponent->Activate();
	}
	
	mFloatLifeTime = 0.f;
}

void APlayerTrailFX::OnShieldActive()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player)
		return;

	if (!IsValid(mShieldFXComponent))
	{
		FVector ShieldLocation = Player->GetArrow()->GetRelativeLocation() + Player->GetActorUpVector() * 50.f;
		mShieldFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			mShieldFXSystem, Player->GetArrow(), NAME_None, ShieldLocation
			, FRotator(0.f, 0.f, 0.f), EAttachLocation::KeepRelativeOffset, true);
	}
	else
	{
		mShieldFXComponent->Activate();
	}

	mShieldLifeTime = 0.f;
}

void APlayerTrailFX::OffShieldActive()
{
	if (IsValid(mShieldFXComponent))
	{
		mShieldFXComponent->DeactivateImmediate();

		APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
		if(Player)
		{
			FRotator Rot = Player->GetGravityDirection().Rotation() - FRotator(90.0, 0.0, 0.0);
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				Player->GetWorld(), mAfterExplosionEffect, Player->GetActorLocation(), Rot, FVector(14.0, 14.0, 14.0));
		}
	}
}

void APlayerTrailFX::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerTrailFX::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(mDashFXComponent))
	{
		if (mDashFXComponent->IsActive())
		{
			mDashLifeTime += DeltaTime;
		}

		if (mDashLifeTime > 0.4f)
		{
			mDashFXComponent->Deactivate();
			mDashLifeTime = 0.f;
		}
	}
}


void APlayerTrailFX::DestroyComponent(UNiagaraComponent* Component)
{
	if (!IsValid(Component))
		return;

	Component->Deactivate();
	Component->DestroyComponent();
}