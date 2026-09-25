// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Projectile_Base.generated.h"

class UStaticMeshComponent;
class UBoxComponent;


UCLASS( )
class TRIAL_LESSON_API AProjectile_Base : public AActor {
	GENERATED_BODY( )

public:
	// Sets default values for this actor's properties
	AProjectile_Base( );

public:
	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetAttack( int atk );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	int GetDamage( ) const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay( ) override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

public:

protected:

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Projectile" )
	UBoxComponent* CollisionBox;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Projectile" )
	UStaticMeshComponent* Mesh;

private:
	int attack;
};
