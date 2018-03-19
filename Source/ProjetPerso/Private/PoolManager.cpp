// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolManager.h"
#include "PoolChild.h"

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
		AEmptyActor* poolParent = GetWorld()->SpawnActor<AEmptyActor>(AEmptyActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, spawnParam);
		poolParent->Rename(*(GetPoolNameAsString(leader.m_poolName).ToString()));
		poolParent->SetActorLabel(*(GetPoolNameAsString(leader.m_poolName).ToString()));

		FAttachmentTransformRules rules = { EAttachmentRule::KeepRelative, false };
		poolParent->AttachToActor(GetOwner(), rules);
		leader.SetPoolParent(poolParent);
		leader.SetWorld(GetWorld());
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

FPool::~FPool()
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
	if (m_spawnableBlueprints[blueprintIndex] == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("nullptr"));

	AActor* item = m_worldRef->SpawnActor<AActor>(m_spawnableBlueprints[blueprintIndex]->GetClass(), FVector::ZeroVector, FRotator::ZeroRotator);
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

	/*
	UPoolChild* poolChildComponent = ConstructObject<UPoolChild>(UPoolChild::StaticClass(), item, TEXT("PoolChild"));

	poolChildComponent->RegisterComponent();
	poolChildComponent->OnComponentCreated(); // Might need this line, might not.
	poolChildComponent->AttachTo(item->GetRootComponent(), NAME_None);
	*/

	//UE_LOG(LogTemp, Warning, TEXT("Sub pool size %d."), SubPools().Num());
	
	//SubPools(_subpoolIndex)->ItemPool()->Emplace(item);
	SubPools()[_subpoolIndex]->ItemPool()->Emplace(item);
	return item;
}
