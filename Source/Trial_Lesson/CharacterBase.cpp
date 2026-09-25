#include "CharacterBase.h"
#include "Projectile_Base.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h" 
#include "Kismet/KismetMathLibrary.h"

void ACharacterBase::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	if ( bIsTurningToTarget && IsValid( LockedTargetActor ) ) {

		bUseControllerRotationYaw = true;

		const FVector TargetLocation = LockedTargetActor->GetActorLocation( );
		const FVector SelfLocation = GetActorLocation( );

		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation( SelfLocation, TargetLocation );
		FRotator CurrentRotation = GetControlRotation( );

		float InterpSpeed = 5.0f;
		FRotator SmoothRotation = FMath::RInterpTo( CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed );

		SmoothRotation.Pitch = CurrentRotation.Pitch;
		SmoothRotation.Roll = CurrentRotation.Roll;

		if ( Controller ) {
			Controller->SetControlRotation( SmoothRotation );
		}

	} else {
		if ( bIsTurningToTarget ) {
			bIsTurningToTarget = false;
			LockedTargetActor = nullptr;

			bUseControllerRotationYaw = true;
		}

		if ( bFollowMouse ) {
			UpdateMouseFacing( );
		} else {
			if ( Controller && !LookAxisValue.IsNearlyZero( ) ) {
				AddControllerYawInput( LookAxisValue.X * BaseTurnRate * DeltaTime );
				AddControllerPitchInput( LookAxisValue.Y * BaseLookUpRate * DeltaTime );
			}
		}
	}

	if ( now_hp <= 0 && !isDead ) {
		isDead = true;
		UE_LOG( LogTemp, Log, TEXT( "Dead!!" ) );
		OnDeath( );
		if ( isRespawn ) {
			RespawnCharacter( );
		}
	}

	UCharacterMovementComponent* CMC = GetCharacterMovement( );
	if ( CMC ) {
		bIsInAir = CMC->IsFalling( );

		FVector CurrentVelocity = GetVelocity( );
		FVector ActorForward = GetActorForwardVector( );
		FVector ActorRight = GetActorRightVector( );

		CurrentForwardSpeed = FVector::DotProduct( CurrentVelocity, ActorForward );
		CurrentRightSpeed = FVector::DotProduct( CurrentVelocity, ActorRight );

		const bool bHasMovementInput = CurrentMovementInput.SizeSquared( ) > KINDA_SMALL_NUMBER;

		const bool bCurrentBackwardCondition =
			( CurrentForwardSpeed < -1.0f ) &&
			bHasMovementInput;

		if ( bCurrentBackwardCondition != bIsMovingBackward ) {
			bIsMovingBackward = bCurrentBackwardCondition;

			OnMovementBackwardChanged( bIsMovingBackward );
		}
	}
}


void ACharacterBase::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent ) {
	Super::SetupPlayerInputComponent( PlayerInputComponent );

	if ( UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>( PlayerInputComponent ) ) {
		EIC->BindAction( MoveAction, ETriggerEvent::Triggered, this, &ACharacterBase::Move );
		EIC->BindAction( MoveAction, ETriggerEvent::Completed, this, &ACharacterBase::ResetMoveBools );

		EIC->BindAction( JumpAction, ETriggerEvent::Started, this, &ACharacterBase::StartJump );
		EIC->BindAction( JumpAction, ETriggerEvent::Completed, this, &ACharacterBase::StopJump );

		EIC->BindAction( LookAction, ETriggerEvent::Triggered, this, &ACharacterBase::Look );

		EIC->BindAction( SlideAction, ETriggerEvent::Triggered, this, &ACharacterBase::StartSlide );
	}
}


void ACharacterBase::BeginPlay( ) {
	Super::BeginPlay( );

	if ( APlayerController* PC = Cast<APlayerController>( GetController( ) ) ) {
		if ( UEnhancedInputLocalPlayerSubsystem* Subsystem =
			 ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( PC->GetLocalPlayer( ) ) ) {
			Subsystem->AddMappingContext( DefaultMappingContext, 0 );
		}
	}
	if ( !PlayerCamera ) {
		PlayerCamera = FindComponentByClass<UCameraComponent>( );
		if ( PlayerCamera ) {
			UE_LOG( LogTemp, Log, TEXT( "Camera Find" ) );
		}
	}

	if ( UCharacterMovementComponent* MoveComp = GetCharacterMovement( ) ) {
		DefaultGroundFriction = MoveComp->GroundFriction;
	}
	if ( UCapsuleComponent* CapComp = GetCapsuleComponent( ) ) {
		DefaultCapsuleHalfHeight = CapComp->GetUnscaledCapsuleHalfHeight( );
	}
}

void ACharacterBase::PossessedBy( AController* NewController ) {
	Super::PossessedBy( NewController );

	if ( APlayerController* PC = Cast<APlayerController>( NewController ) ) {
		if ( UEnhancedInputLocalPlayerSubsystem* Subsystem =
			 ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( PC->GetLocalPlayer( ) ) ) {
			Subsystem->AddMappingContext( DefaultMappingContext, 0 );
			UE_LOG( LogTemp, Warning, TEXT( "IMC Added via PossessedBy" ) );
		}
	}
}

bool ACharacterBase::CanSlide( ) const {
	if ( !GetCharacterMovement( ) ) {
		return false;
	}

	return !bIsSliding && !GetCharacterMovement( )->IsFalling( );
}

ACharacterBase::ACharacterBase( ) {
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>( TEXT( "SpringArm" ) );
	SpringArmComp->SetupAttachment( RootComponent );
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>( TEXT( "PlayerCamera" ) );
	PlayerCamera->SetupAttachment( SpringArmComp );
	PrimaryActorTick.bCanEverTick = true;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bUseControllerRotationYaw = true;
	if ( GetCharacterMovement( ) ) {
		GetCharacterMovement( )->bOrientRotationToMovement = false;
	}

	bFrontMoveActionExist = false;
	bBackMoveActionExist = false;
	bLeftMoveActionExist = false;
	bRightMoveActionExist = false;
	bJumpActionExist = false;

	ItemCount = 0;
	isHorizontalMovementActive = true;
	max_hp = 1;
	now_hp = max_hp;
	max_mp = 0;
	now_mp = max_mp;
	isDead = false;
	lockDir = false;
	bFollowMouse = false;
	bIsTurningToTarget = false;
	SetMouseCursorVisible( false );
}


void ACharacterBase::AddItemCount( ) {
	ItemCount++;

}

void ACharacterBase::RespawnCharacter( ) {
	//if ( isRespawn ) {
		DisableInput( Cast<APlayerController>( GetController( ) ) );
		SetActorHiddenInGame( true );
		SetActorEnableCollision( false );

		FTimerHandle RespawnHandle;
		GetWorldTimerManager( ).SetTimer(
			RespawnHandle,
			[ this ] ( ) {
			SetActorLocation( RespawnLocation );
			SetActorRotation( RespawnRotaiton );

			SetActorHiddenInGame( false );
			SetActorEnableCollision( true );
			EnableInput( Cast<APlayerController>( GetController( ) ) );
			SetHp( max_hp );
			isDead = false;
		},
			3.0f,
			false
		);
	//}
}

void ACharacterBase::Look( const FInputActionValue& Value ) {
	LookAxisValue = Value.Get<FVector2D>( );

	if ( !LookAxisValue.IsNearlyZero( ) ) {
		//UE_LOG(LogTemp, Warning, TEXT("LookAxisValue: X=%f, Y=%f"), LookAxisValue.X, LookAxisValue.Y);
	}
}

void ACharacterBase::SetNoDamage( bool invincible ) {
	bNoDamage = invincible;
}

void ACharacterBase::Move( const FInputActionValue& Value ) {
	FVector2D MovementVector = Value.Get<FVector2D>( );

	CurrentMovementInput = MovementVector;

	if ( Controller ) {
		FVector ForwardDir;
		FVector RightDir;

		if ( lockDir ) {
			// ワールド依存移動：キャラの向きではなく、ワールド座標を基準に
			ForwardDir = FVector::ForwardVector; // (1,0,0)
			RightDir = FVector::RightVector;   // (0,1,0)
		} else {
			// 通常：キャラの向き依存
			ForwardDir = GetActorForwardVector( );
			RightDir = GetActorRightVector( );
		}

		if ( isHorizontalMovementActive )

		{

			AddMovementInput( ForwardDir, MovementVector.Y );

		}

		AddMovementInput( RightDir, MovementVector.X );
	}

	bFrontMoveActionExist = ( MovementVector.Y > 0.1f );
	bBackMoveActionExist = ( MovementVector.Y < -0.1f );
	bRightMoveActionExist = ( MovementVector.X > 0.1f );
	bLeftMoveActionExist = ( MovementVector.X < -0.1f );

}


void ACharacterBase::StartJump( ) {
	bJumpActionExist = true;
	Jump( );
}


void ACharacterBase::StopJump( ) {
	bJumpActionExist = false;
	StopJumping( );
}


void ACharacterBase::ResetMoveBools( ) {
	bFrontMoveActionExist = false;
	bBackMoveActionExist = false;
	bLeftMoveActionExist = false;
	bRightMoveActionExist = false;
}


void ACharacterBase::StartSlide( ) {
	if ( CanSlide( ) ) {
		bIsSliding = true;

		if ( UCharacterMovementComponent* MoveComp = GetCharacterMovement( ) ) {
			MoveComp->GroundFriction = SlidingGroundFriction;
			MoveComp->bWantsToCrouch = true;
		}

		/*if (UCapsuleComponent* CapComp = GetCapsuleComponent( ))
		{
			CapComp->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight / 2.0f);
		}*/

		LaunchCharacter( GetActorForwardVector( ) * SlideImpulseForce, true, true );

		GetWorld( )->GetTimerManager( ).SetTimer(
			SlideTimerHandle, this, &ACharacterBase::StopSlide, SlideDuration, false );
	}
}


void ACharacterBase::StopSlide( ) {
	bIsSliding = false;

	if ( UCharacterMovementComponent* MoveComp = GetCharacterMovement( ) ) {
		MoveComp->GroundFriction = DefaultGroundFriction;
		MoveComp->bWantsToCrouch = false;
	}

	GetWorld( )->GetTimerManager( ).ClearTimer( SlideTimerHandle );
}

void ACharacterBase::SetCameraLocation( FVector NewLocation ) {
	if ( PlayerCamera ) {
		PlayerCamera->SetWorldLocation( NewLocation );
	} else {
		UE_LOG( LogTemp, Warning, TEXT( "PlayerCamera not L" ) );
	}
}


void ACharacterBase::SetCameraRotation( FRotator NewRotation ) {
	if ( PlayerCamera ) {
		PlayerCamera->SetWorldRotation( NewRotation );
	} else {
		UE_LOG( LogTemp, Warning, TEXT( "PlayerCamera not R" ) );
	}
}


void ACharacterBase::SetCameraDetached( bool bDetach ) {
	if ( bDetach ) {
		PlayerCamera->DetachFromComponent( FDetachmentTransformRules::KeepWorldTransform );
		UE_LOG( LogTemp, Warning, TEXT( "Camera Detach" ) );
	} else {
		if ( SpringArmComp ) {
			PlayerCamera->AttachToComponent( SpringArmComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale );
			UE_LOG( LogTemp, Warning, TEXT( "Camera Attach" ) );
		}
	}
}


void ACharacterBase::EnableMouseLook( bool bEnable ) {
	if ( !bEnable ) {
		bUseControllerRotationYaw = false;
		isHorizontalMovementActive = false;
	} else {
		bUseControllerRotationYaw = true;
		isHorizontalMovementActive = true;
	}
}

void ACharacterBase::SetMaxHp( int hp, bool toMax ) {
	max_hp = hp;
	if ( toMax ) {
		now_hp = max_hp;
	} else {
		now_hp = FMath::Clamp( now_hp, 0, max_hp );
	}
}

void ACharacterBase::SetMaxMp( int mp, bool toMax ) {
	max_mp = mp;
	if ( toMax ) {
		now_mp = max_mp;
	} else {
		now_mp = FMath::Clamp( now_mp, 0, max_mp );
	}
}

void ACharacterBase::SetHp( int hp ) {
	now_hp = FMath::Clamp( hp, 0, max_hp );
}

void ACharacterBase::SetMp( int mp ) {
	now_mp = FMath::Clamp( mp, 0, max_mp );
}

void ACharacterBase::SetAttack( int atk ) {
	attack = atk;
}

bool ACharacterBase::TryConsumeMP( int32 Cost ) {
	if ( now_mp >= Cost ) {
		now_mp -= Cost;
		if ( now_hp < 0 ) {
			now_hp = 0;
		}
		return true;
	}
	return false;
}

int ACharacterBase::GetHp( ) const {
	return now_hp;
}

int ACharacterBase::GetMaxHp( ) const {
	return max_hp;
}

int ACharacterBase::GetMp( ) const {
	return now_mp;
}

int ACharacterBase::GetMaxMp( ) const {
	return max_mp;
}

int ACharacterBase::GetDamage( ) const {
	return attack;
}

void ACharacterBase::Respawn( bool state ) {
	isRespawn = state;
	if ( state ) {
		RespawnLocation = FVector( 0, 0, 100 );
		RespawnRotaiton = FRotator( 0, 0, 0 );
	}
}

void ACharacterBase::RespawnPointSet( FVector NewLocation ) {
	RespawnLocation = NewLocation;
}

void ACharacterBase::SetLockDir( bool bNewLock ) {
	lockDir = bNewLock;
}

void ACharacterBase::SetFollowMouse( bool bFollow ) {
	UCharacterMovementComponent* MovementComp = GetCharacterMovement( );
	if ( !MovementComp ) return;

	if ( bFollow ) {
		bFollowMouse = true;
		SpringArmComp->bInheritYaw = false;
		MovementComp->bOrientRotationToMovement = false;
		MovementComp->bUseControllerDesiredRotation = false;
		bUseControllerRotationYaw = true;

	} else {
		bFollowMouse = false;
		SpringArmComp->bInheritYaw = true;
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->bUseControllerDesiredRotation = true;
		bUseControllerRotationYaw = false;
	}
}

bool ACharacterBase::ApplyDamageFromComponent( AActor* OtherActor, UPrimitiveComponent* OtherComp ) {
	if ( !OtherActor || !OtherComp ) {
		return false;
	}

	if ( !OtherComp->ComponentHasTag( FName( "atk" ) ) ) {
		return false;
	}

	if ( ACharacterBase* OtherCharacter = Cast<ACharacterBase>( OtherActor ) ) {

		if ( OtherCharacter == this ) {
			return false;
		}

		const int ActualDamage = OtherCharacter->GetDamage( );

		return HandleDamageAndInvincibility( ActualDamage );
	}

	if ( AEnemyBase* Enemy = Cast<AEnemyBase>( OtherActor ) ) {

		const int ActualDamage = Enemy->GetDamage( );

		return HandleDamageAndInvincibility( ActualDamage );
	}

	if ( AProjectile_Base* Projectile = Cast<AProjectile_Base>( OtherActor ) ) {

		if ( Projectile->GetOwner( ) == this ) {
			return false;
		}

		const int ActualDamage = Projectile->GetDamage( );

		return HandleDamageAndInvincibility( ActualDamage );
	}

	return false;
}


void ACharacterBase::TurnToActorClass( TSubclassOf<AActor> TargetClass ) {

	if ( !TargetClass || !GetWorld( ) ) {
		return;
	}

	AActor* TargetActor = UGameplayStatics::GetActorOfClass( GetWorld( ), TargetClass );

	if ( TargetActor ) {

		const FVector TargetLocation = TargetActor->GetActorLocation( );
		const FVector SelfLocation = GetActorLocation( );

		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation( SelfLocation, TargetLocation );

		FRotator NewRotation = GetActorRotation( );
		NewRotation.Yaw = LookAtRotation.Yaw;

		SetActorRotation( NewRotation );
	}
}

void ACharacterBase::ToggleTurnToActorClass( TSubclassOf<AActor> TargetClass ) {

	if ( bIsTurningToTarget ) {
		bIsTurningToTarget = false;
		LockedTargetActor = nullptr;
		return;
	}

	if ( !TargetClass || !GetWorld( ) ) {
		return;
	}

	AActor* FoundActor = UGameplayStatics::GetActorOfClass( GetWorld( ), TargetClass );

	if ( FoundActor ) {
		LockedTargetActor = FoundActor;
		bIsTurningToTarget = true;
	}
}

void ACharacterBase::setBlock( bool bNewBlockState ) {
	active_block = bNewBlockState;
}

void ACharacterBase::PickupItem( AItemBase* Item ) {
	if ( !Item ) return;

	int32* Count = ItemCounts.Find( Item->GetItemName( ) );
	if ( Count ) {
		( *Count )++;
	} else {
		ItemCounts.Add( Item->GetItemName( ), 1 );
	}
}

void ACharacterBase::SpawnObject( TSubclassOf<AActor> SpawnActorClass, FTransform OffsetTransform ) {
	if ( !SpawnActorClass || !GetWorld( ) ) {
		return;
	}

	AController* PC = GetController( );


	FRotator BaseRotator = GetActorRotation( );
	FVector BaseLocation = GetActorLocation( );

	if ( PC ) {

		FRotator ControlRot = PC->GetControlRotation( );

		BaseRotator = FRotator( 0.0f, ControlRot.Yaw, 0.0f );

	}

	FTransform BaseTransform;
	BaseTransform.SetRotation( BaseRotator.Quaternion( ) );
	BaseTransform.SetLocation( BaseLocation );

	const FTransform FinalSpawnTransform = OffsetTransform * BaseTransform;

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
		// SpawnedActor->Tags.Add( FName( "Player" ) );

		SpawnedActor->SetActorScale3D( FinalSpawnTransform.GetScale3D( ) );
	}
}

void ACharacterBase::SetMouseCursorVisible( bool bVisible ) {
	if ( APlayerController* PC = Cast<APlayerController>( GetController( ) ) ) {
		PC->bShowMouseCursor = bVisible;

		if ( bVisible ) {
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior( EMouseLockMode::DoNotLock );
			InputMode.SetHideCursorDuringCapture( false );
			PC->SetInputMode( InputMode );
		} else {
			FInputModeGameOnly InputMode;
			PC->SetInputMode( InputMode );
			PC->bShowMouseCursor = false;
		}
	}
}

void ACharacterBase::UpdateMouseFacing( ) {
	APlayerController* PC = Cast<APlayerController>( GetController( ) );
	if ( !PC ) return;

	float MouseX, MouseY;
	if ( !PC->GetMousePosition( MouseX, MouseY ) ) return;

	FVector CharacterLocation = GetActorLocation( );
	FVector WorldOrigin, WorldDirection;

	if ( PC->DeprojectScreenPositionToWorld( MouseX, MouseY, WorldOrigin, WorldDirection ) ) {

		if ( FMath::IsNearlyZero( WorldDirection.Z ) ) {
			return;
		}

		float T = ( CharacterLocation.Z - WorldOrigin.Z ) / WorldDirection.Z;

		if ( T < 0.0f ) return;

		FVector MouseWorldPos = WorldOrigin + WorldDirection * T;

		FVector Direction = MouseWorldPos - CharacterLocation;
		Direction.Z = 0;

		if ( !Direction.IsNearlyZero( ) ) {
			float TargetYaw = Direction.Rotation( ).Yaw;
			float CurrentYaw = PC->GetControlRotation( ).Yaw;

			float DeltaYaw = FMath::FindDeltaAngleDegrees( CurrentYaw, TargetYaw );

			const float Sensitivity = 5.0f;
			float YawInput = DeltaYaw * Sensitivity * GetWorld( )->GetDeltaSeconds( );

			AddControllerYawInput( YawInput );
		}
	}
}

bool ACharacterBase::HandleDamageAndInvincibility( int32 ActualDamage ) {
	if ( ActualDamage > 0 && !bNoDamage ) {

		if ( active_block ) {
			ActualDamage = FMath::Max( 1, ActualDamage / 2 );
		}

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