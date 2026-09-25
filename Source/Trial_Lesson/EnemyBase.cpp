// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "Projectile_Base.h"
#include "CharacterBase.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyBase::AEnemyBase( ) {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	max_hp = 1;
	now_hp = max_hp;
	attack = 0;
	bNoDamage = false;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay( ) {
	Super::BeginPlay( );

}

// Called every frame
void AEnemyBase::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	if ( now_hp <= 0 && !dead) {
		OnDeath( );
		dead = true;
	}

	if ( bShouldTurnToPlayer ) {

		if ( APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn( GetWorld( ), 0 ) ) {
			const FVector TargetLocation = PlayerPawn->GetActorLocation( );
			const FVector SelfLocation = GetActorLocation( );

			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation( SelfLocation, TargetLocation );

			FRotator CurrentRotation = GetActorRotation( );

			float InterpSpeed = 8.0f;
			FRotator SmoothRotation = FMath::RInterpTo( CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed );

			CurrentRotation.Yaw = SmoothRotation.Yaw;

			SetActorRotation( CurrentRotation );
		}
	}
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent ) {
	Super::SetupPlayerInputComponent( PlayerInputComponent );

}

void AEnemyBase::SetMaxHp( int hp, bool toMax ) {
	max_hp = hp;
	if ( toMax ) {
		now_hp = max_hp;
	} else {
		now_hp = FMath::Clamp( now_hp, 0, max_hp );
	}
}

void AEnemyBase::SetHp( int hp ) {
	now_hp = FMath::Clamp( hp, 0, max_hp );
}

void AEnemyBase::SetAttack( int atk ) {
	attack = atk;
}

int AEnemyBase::GetHp( ) const {
	return now_hp;
}

int AEnemyBase::GetDamage( ) const {
	return attack;
}

void AEnemyBase::SpawnObject( TSubclassOf<AActor> SpawnActorClass, FTransform SpawnTransform ) {
	if ( !SpawnActorClass || !GetWorld( ) ) {
		return;
	}

	const FTransform BaseTransform = GetActorTransform( );

	const FTransform FinalSpawnTransform = SpawnTransform * BaseTransform;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector FinalLocation = FinalSpawnTransform.GetLocation( );
	const FRotator FinalRotation = FinalSpawnTransform.GetRotation( ).Rotator( );

	AActor* SpawnedActor = GetWorld( )->SpawnActor<AActor>(
		SpawnActorClass,
		FinalLocation,
		FinalRotation,
		Params
	);

	if ( SpawnedActor ) {
		SpawnedActor->Tags.Add( FName( "Enemy" ) );
		SpawnedActor->SetActorScale3D( FinalSpawnTransform.GetScale3D( ) );
	}
}

void AEnemyBase::SetNoDamage( bool invincible ) {
	bNoDamage = invincible;
}

bool AEnemyBase::ApplyDamageFromComponent( AActor* OtherActor, UPrimitiveComponent* OtherComp ) {
	if ( !OtherActor || !OtherComp ) {
		return false;
	}

	if ( !OtherComp->ComponentHasTag( FName( "atk" ) ) ) {
		return false;
	}

	if ( ACharacterBase* Player = Cast<ACharacterBase>( OtherActor ) ) {

		const int ActualDamage = Player->GetDamage( );

		return HandleDamageAndInvincibility( ActualDamage );
	}

	if ( AProjectile_Base* Projectile = Cast<AProjectile_Base>( OtherActor ) ) {

		if ( Projectile->GetOwner( ) == this ) {
			return false;
		}

		if ( Projectile->ActorHasTag( FName( "Enemy" ) ) ) {
			return false;
		}
		const int ActualDamage = Projectile->GetDamage( );

		return HandleDamageAndInvincibility( ActualDamage );
	}

	return false;
}


bool AEnemyBase::HandleDamageAndInvincibility( int32 ActualDamage ) {
	if ( ActualDamage > 0 && !bNoDamage ) {

		SetNoDamage( true );
		now_hp -= ActualDamage;
		if ( now_hp < 0 ) {
			now_hp = 0;
		}

		// 2. 無敵タイマーの設定
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction( this, FName( "SetNoDamage" ), false );
		GetWorld( )->GetTimerManager( ).SetTimer(
			TimerHandle_NoDamage,
			TimerDel,
			NoDamageDuration,
			false
		);

		return true;
	}
	return false;
}

void AEnemyBase::SetTurningEnabled( bool bEnable ) {
	// 引数のbool値でフラグを直接切り替える
	bShouldTurnToPlayer = bEnable;
}