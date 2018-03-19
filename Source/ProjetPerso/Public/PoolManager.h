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

	UPROPERTY(EditAnywhere)
	PoolName m_poolName;
	UPROPERTY(EditAnywhere)
	int m_poolSize = 50;
	UPROPERTY(EditAnywhere)
	TArray<AActor*> m_spawnableBlueprints;
	UPROPERTY(EditAnywhere)
	bool m_separateSpawnablesIntoDifferentPools = false;
	UPROPERTY(EditAnywhere)
	float m_timerReturnToPool = -1;

	FPoolLeader() { }
	FPoolLeader(FPoolLeader* _toCopy);

	inline TArray<FPool*>& SubPools() { return m_subPools; }
	inline FPool* SubPools(int _index) { return m_subPools[_index]; }

	inline void SetPoolParent(AActor* _poolParent) { m_poolParent = _poolParent; }
	inline void SetWorld(UWorld* _worldRef) { m_worldRef = _worldRef; }

	void InitializePool();
	AActor* CreateRandomPoolItem(int _subpoolIndex);
};

//UCLASS(BlueprintType)
//class PROJETPERSO_API UPoolLeaderClass : public UObject
//{
//	GENERATED_BODY()
//
//public:
//
//	UFUNCTION(BlueprintCallable)
//	AActor* GetItem(FPoolLeader _poolLeaderData, bool _activeObjectOnRetrieval = false, int _subpoolNumber = 0);
//	UFUNCTION(BlueprintCallable)
//	AActor* GetItem2(AActor* _newParent, FVector _newPosition, bool _activeObjectOnRetrieval = false, bool _spawnInWorldspace = false, int _subpoolNumber = 0);
//};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJETPERSO_API UPoolManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPoolManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPoolLeader> m_poolLeaders;

	//UPROPERTY(BlueprintReadOnly)
	//UPoolLeaderClass* leaderClass;
	//UPROPERTY(EditAnywhere)
	//	TArray<int> test;
	//UPROPERTY(EditAnywhere)
	//	float test2;
	//UFUNCTION(BlueprintCallable)
	int GetPoolByName(PoolName _poolName);

	UFUNCTION(BlueprintCallable)
	AActor* GetItem(PoolName _poolName, bool _activeObjectOnRetrieval = false, int _subpoolNumber = 0);
	UFUNCTION(BlueprintCallable)
	AActor* GetItemEnhanced(PoolName _poolName, AActor* _newParent, FVector _newPosition, FName _newCollisionProfile = FName("BlockAll"), bool _activeObjectOnRetrieval = false, bool _spawnInWorldspace = false, int _subpoolNumber = 0);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



};






