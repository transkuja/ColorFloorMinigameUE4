// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolManager.h"

#include "PoolChild.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJETPERSO_API UPoolChild : public UActorComponent
{
	GENERATED_BODY()

private:
	FPool* m_pool;
	float m_currentTimer = 0.0f;
	bool m_noReturn = false;
	bool m_isReady = false;

public:	
	// Sets default values for this component's properties
	UPoolChild();
	inline void SetPool(FPool* _pool) { 
		m_pool = _pool;
		m_currentTimer = m_pool->m_timerReturnToPool;
		if (m_currentTimer < 0.0f)
			m_noReturn = true;
		m_isReady = true;
	}

	inline FPool* GetPool() { return m_pool; }

	UFUNCTION(BlueprintCallable)
	void ReturnToPool();

	void ResetItemTimer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
