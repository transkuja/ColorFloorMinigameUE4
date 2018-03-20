// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmptyActor.h"

#include "PoolManager.generated.h"

UENUM(BlueprintType)
enum class PoolName : uint8 { 
	Collectable UMETA(DisplayName = "Collectable")
};

class PROJETPERSO_API FPool
{
	TArray<AActor*> m_itemPool;

public:
	// If > 0, the items of this pool will automatically return to it after time expires.
	float m_timerReturnToPool = -1;
	AActor* m_poolParent;

	FPool(AActor* _poolParent, float _timerReturnToPool) : m_poolParent(_poolParent), m_timerReturnToPool(_timerReturnToPool) {};
	~FPool();
	TArray<AActor*>* ItemPool() { return &m_itemPool; }
};

USTRUCT(BlueprintType)
struct PROJETPERSO_API FPoolLeader
{
	GENERATED_BODY()
	TArray<FPool*> m_subPools;
	UWorld* m_worldRef;

public:
	AActor* m_poolParent;

	// The name of the pool, based on an enum.
	UPROPERTY(EditAnywhere)
	PoolName m_poolName;
	// The size of the pool. Defines the number of items that will be instantiated at launch.
	UPROPERTY(EditAnywhere)
	int m_poolSize = 50;
	// The items to spawn
	UPROPERTY(EditAnywhere)
	TArray<UClass> m_spawnableBlueprints;

	// If true, creates a subpool for each spawnable blueprint. Else, every spawnable blueprints will be attached to a single subpool.
	UPROPERTY(EditAnywhere)
	bool m_separateSpawnablesIntoDifferentPools = false;
	// If > 0, the items of this pool will automatically return to it after time expires.
	UPROPERTY(EditAnywhere)
	float m_timerReturnToPool = -1;

	FPoolLeader() { }
	FPoolLeader(FPoolLeader* _toCopy);

	inline TArray<FPool*>& SubPools() { return m_subPools; }
	inline FPool* SubPools(int _index) { return m_subPools[_index]; }

	inline void SetPoolParent(AActor* _poolParent) { m_poolParent = _poolParent; }
	inline void SetWorld(UWorld* _worldRef) { m_worldRef = _worldRef; }

	/*
		Initialize associated pool with parameters specified in editor. Should be called once for each pool leader declared in PoolManager.
	*/
	void InitializePool();

	/*
		Create a random item and add it to this leader associated Pool.
		@_subpoolIndex, the index of the subpool on which the item should be attached. Should be 0 if m_separateSpawnablesIntoDifferentPools is false for this leader.
	*/
	AActor* CreateRandomPoolItem(int _subpoolIndex);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJETPERSO_API UPoolManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPoolManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPoolLeader> m_poolLeaders;

	/*
		Return an item from pool named _poolName.
		@_poolName, the name of the pool we want the item from
		@_activeObjectOnRetrieval, set the item visible after retrieval. True by default.
		@_subpoolNumber, the subpool index we want the item from. 0 by default, 0 if no subpools have been created.
	*/
	UFUNCTION(BlueprintCallable)
	AActor* GetItem(PoolName _poolName, bool _activeObjectOnRetrieval = true, int _subpoolNumber = 0);

	/*
		Return an item from pool named _poolName.
		@_poolName, the name of the pool we want the item from
		@_newParent, the actor we want the item attached to
		@_newPosition, the item new position
		@_newCollisionProfile, change the item collision profile. BlockAll by default.
		@_activeObjectOnRetrieval, set the item visible after retrieval. True by default.
		@_spawnInWorldSpace, whether the item _newPosition is in relative or world space.
		@_subpoolNumber, the subpool index we want the item from. 0 by default, 0 if no subpools have been created.
	*/
	UFUNCTION(BlueprintCallable)
	AActor* GetItemEnhanced(PoolName _poolName, AActor* _newParent, FVector _newPosition, FName _newCollisionProfile = FName("BlockAll"), bool _activeObjectOnRetrieval = true, bool _spawnInWorldspace = false, int _subpoolNumber = 0);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/*
		Return the index of a pool given the PoolName
	*/
	int GetPoolByName(PoolName _poolName);

};






