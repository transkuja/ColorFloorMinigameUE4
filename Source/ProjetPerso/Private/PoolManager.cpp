// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolManager.h"
#include "PoolChild.h"

class AEmptyActor;


FPool::~FPool()
{
	delete m_poolParent;
}

UPoolManager::UPoolManager()
{
	PrimaryComponentTick.bCanEverTick = true;
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

	// For each leader, create tree view then initialize associated Pool
	for (int i = 0; i < m_poolLeaders.Num(); ++i)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.Name = GetPoolNameAsString(m_poolLeaders[i].m_poolName);

		// Create an empty actor for Pool Parent, and rename it with pool name.
		AEmptyActor* poolParent = GetWorld()->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, spawnParam);
		poolParent->Rename(*(GetPoolNameAsString(m_poolLeaders[i].m_poolName).ToString()));
		poolParent->SetActorLabel(*(GetPoolNameAsString(m_poolLeaders[i].m_poolName).ToString()));

		// Attach pool parent to PoolManager.
		FAttachmentTransformRules rules = { EAttachmentRule::KeepRelative, false };
		poolParent->AttachToActor(GetOwner(), rules);

		// Set pool parent to pool leader and initialize pool.
		m_poolLeaders[i].SetPoolParent(poolParent);
		m_poolLeaders[i].SetWorld(GetWorld());
		m_poolLeaders[i].InitializePool();
	}
}


void UPoolManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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

AActor* UPoolManager::GetItem(PoolName _poolName, bool _activeObjectOnRetrieval, int _subpoolNumber)
{
	AActor* actorToReturn;

	// Get the leader index from pool name
	int leaderDataIndex = GetPoolByName(_poolName);

	if (m_poolLeaders[leaderDataIndex].m_poolParent == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("No pool parent"));
		return nullptr;
	}

	// Retrieve subpools
	TArray<AActor*> poolParentChildren;
	m_poolLeaders[leaderDataIndex].m_poolParent->GetAttachedActors(poolParentChildren);

	// Retrieve items from subpool
	TArray<AActor*> poolChildren;
	poolParentChildren[_subpoolNumber]->GetAttachedActors(poolChildren);

	// If there's no more item in pool, create a new one. Else, return the first one.
	if (poolChildren.Num() == 0)
	{
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

	// Initialize item
	actorToReturn->FindComponentByClass<UPoolChild>()->ResetItemTimer();

	// Detach item from pool, then show it and change its collisions
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

	// Get the leader index from pool name
	int leaderDataIndex = GetPoolByName(_poolName);
	
	if (m_poolLeaders[leaderDataIndex].m_poolParent == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("No pool parent"));
		return nullptr;
	}

	// Retrieve subpools
	TArray<AActor*> poolParentChildren;
	m_poolLeaders[leaderDataIndex].m_poolParent->GetAttachedActors(poolParentChildren);

	// Retrieve items from subpool
	TArray<AActor*> poolChildren;
	poolParentChildren[_subpoolNumber]->GetAttachedActors(poolChildren);


	// If there's no more item in pool, create a new one. Else, return the first one.
	if (poolChildren.Num() == 0)
	{
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

	// Initialize item
	actorToReturn->FindComponentByClass<UPoolChild>()->ResetItemTimer();

	// If _newParent is specified, attach the item to the specified actor.
	if (_newParent != nullptr)
		actorToReturn->AttachToActor(_newParent, FAttachmentTransformRules::KeepWorldTransform);
	else
		actorToReturn->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	// If _spawnInWorldSpace, set actor location to _newPosition in world space. Else, set actor relative location to _newPosition.
	if (_spawnInWorldspace)
	{
		actorToReturn->SetActorLocation(_newPosition);
	}
	else
	{
		actorToReturn->SetActorRelativeLocation(_newPosition);
	}

	// If _activeObjectOnRetrieval, change item visibility and collision profile.
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

	// Either fill a subpool with random items from m_spawnableBlueprints, or fill a subpool for each blueprint in m_spawnableBlueprints
	for (int i = 0; i < ((m_separateSpawnablesIntoDifferentPools) ? m_spawnableBlueprints.Num() : 1); i++)
	{
		// Retrieve subpools
		m_poolParent->GetAttachedActors(children);
		AEmptyActor* poolContainer = NULL;

		// Create a Pool Container for each subpools if not already existing
		if (children.Num() <= i)
		{
			if (m_worldRef == NULL)
			{
				UE_LOG(LogTemp, Warning, TEXT("Woops"));
				return;
			}

			// Create the subpool container with proper name
			poolContainer = m_worldRef->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
			FString poolContainerName = FString("Pool Container ");
			poolContainerName.AppendInt(i);
			poolContainer->Rename(*poolContainerName);
			poolContainer->SetActorLabel(*poolContainerName);

			// Attach the subpool to pool parent
			FAttachmentTransformRules rules = { EAttachmentRule::KeepRelative, false };
			poolContainer->AttachToActor(m_poolParent, rules);
		}

		// Retrieve the subpool i if was created before function's call.
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

		// Create a new pool linked to pool container
		FPool* newPool = new FPool(poolContainer, m_timerReturnToPool);
		if (newPool == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Fail to create pool."));
			return;
		}

		// Link pool reference to leader
		SubPools().Emplace(newPool);

		// Fill the current pool
		for (int j = 0; j < m_poolSize / ((m_separateSpawnablesIntoDifferentPools) ? m_spawnableBlueprints.Num() : 1); j++)
		{
			CreateRandomPoolItem(i);
		}
	}
}

AActor* FPoolLeader::CreateRandomPoolItem(int _subpoolIndex)
{
	// If m_separateSpawnablesIntoDifferentPools option, create m_poolSize times the same item. Else, create a random item from m_spawnableBlueprints.
	int blueprintIndex = (m_separateSpawnablesIntoDifferentPools) ? _subpoolIndex : rand()% m_spawnableBlueprints.Num();

	AActor* item = m_worldRef->SpawnActor<AActor>(m_spawnableBlueprints[blueprintIndex], FVector::ZeroVector, FRotator::ZeroRotator);
	if (item == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("nullptr item"));

	// Hide item when created
	UStaticMeshComponent* staticMesh = item->FindComponentByClass<UStaticMeshComponent>();
	if (staticMesh == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("No static mesh attached to blueprint."));
		return nullptr;
	}

	staticMesh->SetVisibility(false);
	staticMesh->SetCollisionProfileName(FName("NoCollision"));

	// Set mobility to movable (important to set the item position later)
	staticMesh->SetMobility(EComponentMobility::Movable);

	// Get subpools to attach the new item to the proper one.
	TArray<AActor*> children;
	m_poolParent->GetAttachedActors(children);

	// Attach the item to subpool
	FAttachmentTransformRules rules = { EAttachmentRule::KeepRelative, false };
	item->AttachToActor(children[_subpoolIndex], rules);
	// TODO: would be better if we add dynamically the pool child component
	// Initialize PoolChild component
	item->FindComponentByClass<UPoolChild>()->SetPool(SubPools(_subpoolIndex));

	
	// Check this later
	// UPoolChild* poolChildComponent = item->CreateDefaultSubobject<UPoolChild>(TEXT("PoolChild")); // Only use this in constructors

	// Save the item reference
	SubPools()[_subpoolIndex]->ItemPool()->Emplace(item);
	return item;
}
