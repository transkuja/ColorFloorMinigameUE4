#pragma once

#include "GameFramework/Actor.h"
#include "EmptyActor.generated.h"

UCLASS()
class PROJETPERSO_API AEmptyActor : public AActor
{
	GENERATED_BODY()
public:
	AEmptyActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	class USceneComponent* Scene;
};