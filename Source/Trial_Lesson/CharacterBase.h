#pragma once

#include "AnimDataCharacter.h"
#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "ItemBase.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_DELEGATE( FOnMPConsumed );
DECLARE_DYNAMIC_DELEGATE( FOnMPInsufficient );

class UInputMappingContext;
class UInputAction;

UCLASS( Blueprintable )
class TRIAL_LESSON_API ACharacterBase : public AAnimDataCharacter {
	GENERATED_BODY( )

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( AllowPrivateAccess = "true" ) )
	class UCameraComponent* PlayerCamera;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera" )
	class USpringArmComponent* SpringArmComp;

public:
	virtual void Tick( float DeltaTime ) override;
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;

protected:
	virtual void BeginPlay( ) override;
	virtual void PossessedBy( AController* NewController ) override;

protected:
	bool CanSlide( ) const;

	UPROPERTY( )
	float CurrentHorizontalInputValue = 0.0f;

public:
	ACharacterBase( );
	void AddItemCount( );

private:
	void RespawnCharacter( );


	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetNoDamage( bool invincible );

protected:
	void Look( const FInputActionValue& Value );
	void Move( const FInputActionValue& Value );
	void ResetMoveBools( );
	void StartJump( );
	void StopJump( );
	void StartSlide( );
	void StopSlide( );

public:
	UFUNCTION( BlueprintCallable, Category = "Camera" )
	void SetCameraLocation( FVector NewLocation );

	UFUNCTION( BlueprintCallable, Category = "Camera" )
	void SetCameraRotation( FRotator NewRotation );

	UFUNCTION( BlueprintCallable, Category = "Camera" )
	void SetCameraDetached( bool bDetach );

	UFUNCTION( BlueprintCallable, Category = "Camera" )
	void EnableMouseLook( bool bEnable );

	UFUNCTION( BlueprintCallable, Category = "Camera" )
	void UpdateMouseFacing( );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetMaxHp( int hp, bool toMax );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetMaxMp( int mp, bool toMax );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetHp( int hp );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetMp( int mp );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	void SetAttack( int atk );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	bool TryConsumeMP( int32 Cost );

	UFUNCTION( BlueprintCallable, Category = "Status" )
	int GetHp( ) const;

	UFUNCTION( BlueprintCallable, Category = "Status" )
	int GetMaxHp( ) const;

	UFUNCTION( BlueprintCallable, Category = "Status" )
	int GetMp( ) const;

	UFUNCTION( BlueprintCallable, Category = "Status" )
	int GetMaxMp( )const;

	UFUNCTION( BlueprintCallable, Category = "Status" )
	int GetDamage( ) const;

	UFUNCTION( BlueprintCallable, Category = "Respawn" )
	void Respawn( bool state );

	UFUNCTION( BlueprintCallable, Category = "Respawn" )
	void RespawnPointSet( FVector  NewLocation );

	UFUNCTION( BlueprintCallable, Category = "Movement" )
	void SetLockDir( bool bNewLock );

	UFUNCTION( BlueprintCallable, Category = "UI" )
	void SetMouseCursorVisible( bool bVisible );

	UFUNCTION( BlueprintCallable, Category = "Mouse" )
	void SetFollowMouse( bool bFllow );

	UFUNCTION( BlueprintCallable, Category = "Damage" )
	bool ApplyDamageFromComponent( AActor* OtherActor, UPrimitiveComponent* OtherComp );

	UFUNCTION( BlueprintCallable, Category = "Damage" )
	bool HandleDamageAndInvincibility( int32 ActualDamage );

	UFUNCTION( BlueprintCallable, Category = "Targeting" )
	void TurnToActorClass( TSubclassOf<AActor> TargetClass );

	UFUNCTION( BlueprintCallable, Category = "Targeting" )
	void ToggleTurnToActorClass( TSubclassOf<AActor> TargetClass );

	UFUNCTION( BlueprintCallable, Category = "Block" )
	void setBlock( bool bNewBlockState );

	UFUNCTION( BlueprintImplementableEvent, Category = "Item" )
	void OnItemPickedUp( AItemBase* Item );


protected:
	UFUNCTION( BlueprintCallable, Category = "Inventory" )
	void PickupItem( AItemBase* Item );

	UFUNCTION( BlueprintImplementableEvent, Category = "Movement" )
	void OnMovementBackwardChanged( bool bIsBackward );

	UFUNCTION( BlueprintImplementableEvent, Category = "Status" )
	void OnDeath( );

public:
	UFUNCTION( BlueprintCallable, Category = "Spawn" )
	void SpawnObject( TSubclassOf<AActor> SpawnActorClass, FTransform OffsetTransform );

private:
	FVector2D LookAxisValue;
	FVector RespawnLocation;
	FRotator RespawnRotaiton;

protected:

	UPROPERTY( BlueprintReadWrite, Category = "Inventory" )
	TMap<FString, int32> ItemCounts;


	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Input" )
	UInputAction* MoveAction;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Input" )
	UInputAction* JumpAction;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Input" )
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Input" )
	UInputAction* LookAction;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Input" )
	TObjectPtr<class UInputAction> SlideAction;

	UPROPERTY( )
	bool bIsMovingBackward = false;

public:
	UPROPERTY( BlueprintReadOnly, Category = "Items" )
	int32 ItemCount;

private:
	bool isHorizontalMovementActive;

	bool lockDir;

	bool bFollowMouse;

	bool isDead;

	bool isRespawn;

	bool bNoDamage;

	bool bIsTurningToTarget;

	UPROPERTY( EditDefaultsOnly, Category = "Sliding" )
	float SlideDuration = 1.5f;

	UPROPERTY( EditDefaultsOnly, Category = "Sliding" )
	float SlidingGroundFriction = 0.1f;

	UPROPERTY( EditDefaultsOnly, Category = "Sliding" )
	float SlideImpulseForce = 1500.0f;

	UPROPERTY( EditDefaultsOnly, Category = "Status" )
	float NoDamageDuration = 1.0f;

	FTimerHandle SlideTimerHandle;

	FTimerHandle TimerHandle_NoDamage;

	float DefaultGroundFriction;

	float DefaultCapsuleHalfHeight;

	int now_hp;

	int max_hp;

	int now_mp;

	int max_mp;

	int attack;

protected:
	UPROPERTY( BlueprintReadOnly, Category = "Sliding" )
	bool bIsSliding;


	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera" )
	float BaseTurnRate;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera" )
	float BaseLookUpRate;

	UPROPERTY( )
	AActor* LockedTargetActor = nullptr;

	UPROPERTY( )
	FVector2D CurrentMovementInput = FVector2D::ZeroVector;
};
