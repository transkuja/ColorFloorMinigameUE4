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

class PROJETPERSO_API Pool
{
	TArray<AActor*> m_itemPool;

public:
	float m_timerReturnToPool = -1;
	AActor* m_poolParent;

	Pool(AActor* _poolParent, float _timerReturnToPool) : m_poolParent(_poolParent), m_timerReturnToPool(_timerReturnToPool) {};
	~Pool();
	TArray<AActor*> ItemPool() { return m_itemPool; }
};

USTRUCT(BlueprintType)
struct PROJETPERSO_API FPoolLeader
{
	GENERATED_BODY()
	TArray<Pool*> m_subPools;

protected:
	UPROPERTY(EditDefaultsOnly)
	int m_poolSize = 50;
	

public:
	UPROPERTY(EditAnywhere)
	PoolName m_poolName;
	UPROPERTY(EditAnywhere)
	AActor* m_poolParent;
	UPROPERTY(EditAnywhere)
	TArray<AActor*> m_spawnableBlueprints;
	UPROPERTY(EditAnywhere)
	bool m_separateSpawnablesIntoDifferentPools = false;
	UPROPERTY(EditAnywhere)
	float m_timerReturnToPool = -1;

	FPoolLeader() { }

	inline TArray<Pool*> SubPools() { return m_subPools; }

	AActor* GetItem(bool _activeObjectOnRetrieval = false, int _subpoolNumber = 0);
	AActor* GetItem(AActor* _newParent, FVector _newPosition, bool _activeObjectOnRetrieval = false, bool _spawnInWorldspace = false, int _subpoolNumber = 0);
	void InitializePool();
	AActor* CreateRandomPoolItem(int _subpoolIndex);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJETPERSO_API UPoolManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPoolManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPoolLeader> m_poolLeaders;
	//UPROPERTY(EditAnywhere)
	//	TArray<int> test;
	//UPROPERTY(EditAnywhere)
	//	float test2;
	FPoolLeader GetPoolByName(PoolName _poolName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



};






