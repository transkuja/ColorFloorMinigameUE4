// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolManager.h"
#include "PoolChild.h"

class AEmptyActor;


FPool::~FPool()
{
	delete m_poolParent;
}

// Sets default values for this component's properties
UPoolManager::UPoolManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...	
}

// Get enum value name
FName GetPoolNameAsString(PoolName EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("PoolName"), true);
	if (!EnumPtr) return FName("Invalid");

	return EnumPtr->GetNameByValue((int64)EnumValue);
}

void UPoolManager::BeginPlay()
{
	Super::BeginPlay();

	if (m_poolLeaders.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There are no pool leaders defined in Pool Manager."));
		return;
	}

	for (int i = 0; i < m_poolLeaders.Num(); ++i)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.Name = GetPoolNameAsString(m_poolLeaders[i].m_poolName);
		AEmptyActor* poolParent = GetWorld()->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, spawnParam);
		poolParent->Rename(*(GetPoolNameAsString(m_poolLeaders[i].m_poolName).ToString()));
		poolParent->SetActorLabel(*(GetPoolNameAsString(m_poolLeaders[i].m_poolName).ToString()));

		FAttachmentTransformRules rules = { EAttachmentRule::KeepRelative, false };
		poolParent->AttachToActor(GetOwner(), rules);
		m_poolLeaders[i].SetPoolParent(poolParent);
		m_poolLeaders[i].SetWorld(GetWorld());
		m_poolLeaders[i].InitializePool();
	}
	// ...
	
}

int UPoolManager::GetPoolByName(PoolName _poolName)
{
	for (int i = 0; i < m_poolLeaders.Num(); ++i)
	{
		if (m_poolLeaders[i].m_poolName == _poolName)
			return i;
	}
	UE_LOG(LogTemp, Warning, TEXT("Leader not found"));
	return 0;
}

// Called every frame
void UPoolManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


AActor* UPoolManager::GetItem(PoolName _poolName, bool _activeObjectOnRetrieval, int _subpoolNumber)
{
	AActor* actorToReturn;
	TArray<AActor*> poolParentChildren;
	int leaderDataIndex = GetPoolByName(_poolName);

	if (m_poolLeaders[leaderDataIndex].m_poolParent == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("No pool parent"));
		return nullptr;
	}

	m_poolLeaders[leaderDataIndex].m_poolParent->GetAttachedActors(poolParentChildren);
	if (poolParentChildren.Num() == 0)
	{
		actorToReturn = m_poolLeaders[leaderDataIndex].CreateRandomPoolItem(_subpoolNumber);
	}
	else
	{
		TArray<AActor*> poolChildren;
		poolParentChildren[_subpoolNumber]->GetAttachedActors(poolChildren);
		actorToReturn = poolChildren[0];
	}

	if (actorToReturn == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor to return null"));
		return nullptr;
	}

	actorToReturn->FindComponentByClass<UPoolChild>()->ResetItemTimer();

	actorToReturn->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	if (_activeObjectOnRetrieval)
	{
		UStaticMeshComponent* staticMesh = actorToReturn->FindComponentByClass<UStaticMeshComponent>();
		if (staticMesh == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("No static mesh attached to blueprint."));
			return nullptr;
		}
		staticMesh->SetVisibility(true);
		staticMesh->SetCollisionProfileName(FName("BlockAll"));
	}

	return actorToReturn;
}

AActor * UPoolManager::GetItemEnhanced(PoolName _poolName, AActor * _newParent, FVector _newPosition, FName _newCollisionProfile, bool _activeObjectOnRetrieval, bool _spawnInWorldspace, int _subpoolNumber)
{
	AActor* actorToReturn;
	TArray<AActor*> poolParentChildren;
	int leaderDataIndex = GetPoolByName(_poolName);
	
	if (m_poolLeaders[leaderDataIndex].m_poolParent == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("No pool parent"));
		return nullptr;
	}

	m_poolLeaders[leaderDataIndex].m_poolParent->GetAttachedActors(poolParentChildren);
	TArray<AActor*> poolChildren;
	poolParentChildren[_subpoolNumber]->GetAttachedActors(poolChildren);

	if (poolChildren.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No more items"));
		actorToReturn = m_poolLeaders[leaderDataIndex].CreateRandomPoolItem(_subpoolNumber);
	}
	else
	{
		actorToReturn = poolChildren[0];
	}

	if (actorToReturn == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor to return null"));
		return nullptr;
	}

	if (actorToReturn->FindComponentByClass<UPoolChild>() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pool child null"));
		return nullptr;
	}

	actorToReturn->FindComponentByClass<UPoolChild>()->ResetItemTimer();

	if (_newParent != nullptr)
		actorToReturn->AttachToActor(_newParent, FAttachmentTransformRules::KeepWorldTransform);
	else
		actorToReturn->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	if (_spawnInWorldspace)
	{
		actorToReturn->SetActorLocation(_newPosition);
		//actorToReturn->SetActorRotation(_newRotation);
	}
	else
	{
		actorToReturn->SetActorRelativeLocation(_newPosition);
	}

	if (_activeObjectOnRetrieval)
	{
		UStaticMeshComponent* staticMesh = actorToReturn->FindComponentByClass<UStaticMeshComponent>();
		if (staticMesh == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("No static mesh attached to blueprint."));
			return nullptr;
		}
		staticMesh->SetVisibility(true);
		staticMesh->SetCollisionProfileName(_newCollisionProfile);
	}

	return actorToReturn;
}

FPoolLeader::FPoolLeader(FPoolLeader* _toCopy)
{
	this->m_poolName = _toCopy->m_poolName;
	this->m_poolSize = _toCopy->m_poolSize;
	this->m_separateSpawnablesIntoDifferentPools = _toCopy->m_separateSpawnablesIntoDifferentPools;
	this->m_timerReturnToPool = _toCopy->m_timerReturnToPool;
	this->m_poolParent = _toCopy->m_poolParent;
	this->m_worldRef = _toCopy->m_worldRef;
	this->m_subPools = _toCopy->m_subPools;
	this->m_spawnableBlueprints = _toCopy->m_spawnableBlueprints;
}

void FPoolLeader::InitializePool()
{
	if (m_spawnableBlueprints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot initialize pool %s because no blueprints are linked."), *GetNameSafe(m_poolParent));
		return;
	}

	if (m_poolSize <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot initialize pool because pool size is null or negative."));
		return;
	}

	TArray<AActor*> children;
	for (int i = 0; i < ((m_separateSpawnablesIntoDifferentPools) ? m_spawnableBlueprints.Num() : 1); i++)
	{
		m_poolParent->GetAttachedActors(children);
		AEmptyActor* poolContainer = NULL;

		if (children.Num() <= i)
		{
			if (m_worldRef == NULL)
			{
				UE_LOG(LogTemp, Warning, TEXT("Woops"));
				return;
			}

			poolContainer = m_worldRef->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
			FString poolContainerName = FString("Pool Container ");
			poolContainerName.AppendInt(i);
			poolContainer->Rename(*poolContainerName);
			poolContainer->SetActorLabel(*poolContainerName);

			FAttachmentTransformRules rules = { EAttachmentRule::KeepRelative, false };
			poolContainer->AttachToActor(m_poolParent, rules);
		}

		if (poolContainer == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Null pool container, retrieve it from children..."));
			m_poolParent->GetAttachedActors(children);
			poolContainer = (AEmptyActor*)children[i];

			if (poolContainer == NULL)
			{
				UE_LOG(LogTemp, Warning, TEXT("Still null pool container, no pool for you."));
				return;
			}
		}

		FPool* newPool = new FPool(poolContainer, m_timerReturnToPool);
		if (newPool == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Fail to create pool."));
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("Pool created."));

		SubPools().Emplace(newPool);
		UE_LOG(LogTemp, Warning, TEXT("Sub pool size %d."), SubPools().Num());

		for (int j = 0; j < m_poolSize / ((m_separateSpawnablesIntoDifferentPools) ? m_spawnableBlueprints.Num() : 1); j++)
		{
			CreateRandomPoolItem(i);
		}
	}
}

AActor* FPoolLeader::CreateRandomPoolItem(int _subpoolIndex)
{
	int blueprintIndex = (m_separateSpawnablesIntoDifferentPools) ? _subpoolIndex : rand()% m_spawnableBlueprints.Num();
	UE_LOG(LogTemp, Warning, TEXT("Index %d"), blueprintIndex);

	AActor* item = m_worldRef->SpawnActor<AActor>(m_spawnableBlueprints[blueprintIndex], FVector::ZeroVector, FRotator::ZeroRotator);
	if (item == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("nullptr item"));

	UStaticMeshComponent* staticMesh = item->FindComponentByClass<UStaticMeshComponent>();
	if (staticMesh == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("No static mesh attached to blueprint."));
		return nullptr;
	}

	staticMesh->SetVisibility(false);
	staticMesh->SetCollisionProfileName(FName("NoCollision"));
	staticMesh->SetMobility(EComponentMobility::Movable);

	TArray<AActor*> children;
	m_poolParent->GetAttachedActors(children);
	FAttachmentTransformRules rules = { EAttachmentRule::KeepRelative, false };
	item->AttachToActor(children[_subpoolIndex], rules);
	item->FindComponentByClass<UPoolChild>()->SetPool(SubPools(_subpoolIndex));

	// Check this later
	// UPoolChild* poolChildComponent = item->CreateDefaultSubobject<UPoolChild>(TEXT("PoolChild")); // Only use this in constructors

	SubPools()[_subpoolIndex]->ItemPool()->Emplace(item);
	return item;
}
