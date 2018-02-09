// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolManager.h"

class AEmptyActor;

// Sets default values for this component's properties
UPoolManager::UPoolManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...	
}

FName GetPoolNameAsString(PoolName EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("PoolName"), true);
	if (!EnumPtr) return FName("Invalid");

	return EnumPtr->GetNameByValue((int64)EnumValue);
}

// Called when the game starts
void UPoolManager::BeginPlay()
{
	Super::BeginPlay();
	if (m_poolLeaders.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There are no pool leaders defined in Pool Manager."));
		return;
	}

	for (auto leader : m_poolLeaders)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.Name = GetPoolNameAsString(leader.m_poolName);
		AEmptyActor* poolParent = GetWorld()->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass(), GetOwner()->GetActorLocation(), FRotator::ZeroRotator, spawnParam);
		poolParent->Rename(*(GetPoolNameAsString(leader.m_poolName).ToString()));
		poolParent->SetActorLabel(*(GetPoolNameAsString(leader.m_poolName).ToString()));

		FAttachmentTransformRules rules = { EAttachmentRule::KeepRelative, false };
		poolParent->AttachToActor(GetOwner(), rules);
		leader.SetPoolParent(poolParent);
		leader.InitializePool();
	}
	// ...
	
}

FPoolLeader UPoolManager::GetPoolByName(PoolName _poolName)
{
	for (auto leader : m_poolLeaders)
	{
		if (leader.m_poolName == _poolName)
			return leader;
	}
	return FPoolLeader();
}

// Called every frame
void UPoolManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

Pool::~Pool()
{
	delete m_poolParent;
}

AActor* FPoolLeader::GetItem(bool _activeObjectOnRetrieval, int _subpoolNumber)
{
	return nullptr;
}

AActor* FPoolLeader::GetItem(AActor * _newParent, FVector _newPosition, bool _activeObjectOnRetrieval, bool _spawnInWorldspace, int _subpoolNumber)
{
	return nullptr;
}

void FPoolLeader::InitializePool()
{

}

AActor* FPoolLeader::CreateRandomPoolItem(int _subpoolIndex)
{
	return nullptr;
}
