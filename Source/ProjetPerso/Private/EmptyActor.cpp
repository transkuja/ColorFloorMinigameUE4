#include "EmptyActor.h"
#include "ProjetPerso.h"

AEmptyActor::AEmptyActor(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene"));
	Scene->SetMobility(EComponentMobility::Movable);
	SetRootComponent(Scene);
}