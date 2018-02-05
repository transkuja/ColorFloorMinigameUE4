// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerIndexAssigner.h"
#include "AssignPlayerIndex.h"

// Sets default values for this component's properties
UPlayerIndexAssigner::UPlayerIndexAssigner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerIndexAssigner::BeginPlay()
{
	Super::BeginPlay();

	playerIndex = GetNextPlayerIndex();
	// ...
	
}


// Called every frame
void UPlayerIndexAssigner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

