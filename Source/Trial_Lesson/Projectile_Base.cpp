// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Base.h"
#include "Components/BoxComponent.h" 
#include "Components/StaticMeshComponent.h"
#include "Engine/EngineTypes.h"

// Sets default values
AProjectile_Base::AProjectile_Base( ) {
    
    CollisionBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "CollisionBox" ) );
    RootComponent = CollisionBox;

    CollisionBox->SetBoxExtent( FVector( 15.0f, 15.0f, 15.0f ) );
    CollisionBox->ComponentTags.Add( FName( "atk" ) );


    Mesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Mesh" ) );

    Mesh->SetupAttachment( RootComponent );

    Mesh->SetCollisionEnabled( ECollisionEnabled::NoCollision ); // Mesh自体は衝突判定に使用しない
    PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AProjectile_Base::BeginPlay()
{
	Super::BeginPlay();

    if ( Mesh ) {
        // Meshの相対トランスフォームを強制的にゼロに設定し、RootComponentと同期させる
        Mesh->SetRelativeLocation( FVector::ZeroVector );
        Mesh->SetRelativeRotation( FRotator::ZeroRotator );
        Mesh->SetRelativeScale3D( FVector::OneVector );
    }
}

// Called every frame
void AProjectile_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile_Base::SetAttack(int atk ) {
    attack = atk;
}

int AProjectile_Base::GetDamage( ) const {
	return attack;
}
