// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolChild.h"


// Sets default values for this component's properties
UPoolChild::UPoolChild()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPoolChild::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UPoolChild::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (m_isReady)
	{
		if (m_noReturn)
			return;

		m_currentTimer -= DeltaTime;
		if (m_currentTimer < 0.0f)
			ReturnToPool();
	}
}

void UPoolChild::ReturnToPool() 
{
	AActor* actor = GetOwner();

	// Return to its old parent
	FAttachmentTransformRules rules = { EAttachmentRule::KeepRelative, false };
	actor->AttachToActor(m_pool->m_poolParent, rules);

	// Hide item
	UStaticMeshComponent* staticMesh = actor->FindComponentByClass<UStaticMeshComponent>();
	if (staticMesh == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("No static mesh attached to blueprint."));
		return;
	}
	staticMesh->SetVisibility(false);
	staticMesh->SetCollisionProfileName(FName("NoCollision"));
	staticMesh->SetMobility(EComponentMobility::Movable);
}

void UPoolChild::ResetItemTimer()
{
	if (m_pool != nullptr)
	{
		m_currentTimer = m_pool->m_timerReturnToPool;
		if (m_currentTimer == -1) m_noReturn = true;
	}
}

