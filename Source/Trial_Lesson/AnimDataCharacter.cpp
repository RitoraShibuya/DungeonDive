// AAnimDataCharacter.cpp

#include "AnimDataCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h" // •âŠÔ‚É•K—v‚Å‚ ‚ê‚Î’Ç‰Á

// ... (‘¼‚ÌŠÖ”’è‹`) ...

void AAnimDataCharacter::Tick( float DeltaTime ) {
    Super::Tick( DeltaTime );

    UCharacterMovementComponent* CMC = GetCharacterMovement( );
    if ( CMC ) {
        bIsInAir = CMC->IsFalling( );

        FVector CurrentVelocity = GetVelocity( );
        FVector ActorForward = GetActorForwardVector( );
        FVector ActorRight = GetActorRightVector( );

        CurrentForwardSpeed = FVector::DotProduct( CurrentVelocity, ActorForward );
        CurrentRightSpeed = FVector::DotProduct( CurrentVelocity, ActorRight );
    }
}