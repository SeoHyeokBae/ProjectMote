#include "Interface/CustomGravityComponent.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "../Player/PlayerCharacter.h"

UCustomGravityComponent::UCustomGravityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCustomGravityComponent::BeginPlay()
{
	Super::BeginPlay();
	for (TActorIterator<AStaticMeshActor> it(GetWorld()); it; ++it)
	{
		if (it->GetName().Contains(TEXT("Planet")))
		{
			mPlanet = *it; 
			break;
		}
	}
}


void UCustomGravityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (mPlayer && IsValid(mPlanet))
	{
		TArray<UStaticMeshComponent*> PlanetComponent;
		mPlanet->GetComponents<UStaticMeshComponent>(PlanetComponent); // StaticMesh 타입이 아닌 컴포넌트는 걸러짐
		TObjectPtr<UStaticMesh> PlanetStaticMesh = PlanetComponent[0]->GetStaticMesh();
		float PlanetRadius = PlanetStaticMesh->GetBounds().SphereRadius;

		FVector GravityDir = (FVector(0, 0, PlanetRadius) - mPlayer->GetActorLocation()).GetSafeNormal();
		mPlayer->GetCharacterMovement()->SetGravityDirection(GravityDir);
		mPlayer->SetPlanetGravityDir(GravityDir);
	}
}

