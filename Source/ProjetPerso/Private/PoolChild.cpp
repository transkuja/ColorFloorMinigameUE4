// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolChild.h"


UPoolChild::UPoolChild()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPoolChild::BeginPlay()
{
	Super::BeginPlay();
}

void UPoolChild::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// If the item has received its Pool data, start the timer
	if (m_isReady)
	{
		// Ignore timer if we don't want it
		if (m_noReturn)
			return;

		// Send the item back to its pool after m_currentTimer expires
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
		// Reset timer for next retrieval
		m_currentTimer = m_pool->m_timerReturnToPool;
		if (m_currentTimer == -1) m_noReturn = true;
	}
}

