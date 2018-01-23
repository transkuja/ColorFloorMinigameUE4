// Fill out your copyright notice in the Description page of Project Settings.

#include "ColorFloorComponent.h"


// Sets default values for this component's properties
UColorFloorComponent::UColorFloorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UColorFloorComponent::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpule, const FHitResult & Hit)
{
}

// Called when the game starts
void UColorFloorComponent::BeginPlay()
{
	Super::BeginPlay();
	floorMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();

	//floorMesh->GetMaterial(0)->SetVectorParameterValue(FName(TEXT("Color")), FLinearColor(1.0f, 0.0f, 1.0f));
	
	auto material = UMaterialInstanceDynamic::Create(floorMesh->GetMaterial(0), NULL);
	material->SetVectorParameterValue(FName(TEXT("Color")), FLinearColor(1.0f, 0.0f, 1.0f));
	floorMesh->SetMaterial(0, material);
	
}


// Called every frame
void UColorFloorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

