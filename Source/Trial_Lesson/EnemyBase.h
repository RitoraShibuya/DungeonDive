// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AnimDataCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.h"
#include "EnemyBase.generated.h"

UCLASS( )
class TRIAL_LESSON_API AEnemyBase : public AAnimDataCharacter {
	GENERATED_BODY( )

public:
	// Sets default values for this character's properties

	AEnemyBase( );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay( ) override;

	UFUNCTION( BlueprintImplementableEvent, Category = "Status" )
	void OnDeath( );

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;

public:

	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetMaxHp( int hp, bool toMax );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetHp( int hp );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetAttack( int atk );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	int GetHp( ) const;

	UFUNCTION( BlueprintCallable, Category = "Status" )
	int GetDamage( ) const;

	UFUNCTION( BlueprintCallable, Category = "AI|Targeting" )
	void SetTurningEnabled( bool bEnable );

	UFUNCTION( BlueprintCallable, Category = "Spawn" )
	void SpawnObject( TSubclassOf<AActor> SpawnActorClass, FTransform SpawnTransform );

	UFUNCTION( BlueprintCallable, Category = "Damage" )
	bool ApplyDamageFromComponent( AActor* OtherActor, UPrimitiveComponent* OtherComp );

	UFUNCTION( BlueprintCallable, Category = "Damage" )
	bool HandleDamageAndInvincibility( int32 ActualDamage );

private:

	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetNoDamage( bool invincible );

private:
	bool dead = false;

	bool bNoDamage;

	int max_hp;

	int now_hp;

	int attack;

	bool bShouldTurnToPlayer = false;

	UPROPERTY( EditDefaultsOnly, Category = "Status" )
	float NoDamageDuration = 1.0f;

	FTimerHandle TimerHandle_NoDamage;
};
