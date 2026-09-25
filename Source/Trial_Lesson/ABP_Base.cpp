// Fill out your copyright notice in the Description page of Project Settings.


#include "ABP_Base.h"
#include "CharacterBase.h" 
#include "GameFramework/Pawn.h"

void UABP_Base::NativeInitializeAnimation( ) {
    Super::NativeInitializeAnimation( );

    APawn* PawnOwner = TryGetPawnOwner( );
    if ( PawnOwner ) {
        OwningCharacter = Cast<AAnimDataCharacter>( PawnOwner );
    }
}

void UABP_Base::NativeUpdateAnimation( float DeltaSeconds ) {
    Super::NativeUpdateAnimation( DeltaSeconds );

    if ( OwningCharacter.IsValid( ) ) {
        _is_moving_forward = OwningCharacter->HasFrontMoveAction( );
        _is_moving_right = OwningCharacter->HasRightMoveAction( );
        _is_moving_backward = OwningCharacter->HasBackMoveAction( );
        _is_moving_left = OwningCharacter->HasLeftMoveAction( );
        _is_jumping = OwningCharacter->IsJumpingAction( );

        _current_forward_speed = OwningCharacter->GetCurrentForwardSpeed( );
        _current_right_speed = OwningCharacter->GetCurrentRightSpeed( );
        _is_in_air = OwningCharacter->IsInAir( );

        float AbsFwd = FMath::Abs( _current_forward_speed );
        float AbsRight = FMath::Abs( _current_right_speed );

        _is_moving_forward_real = ( _current_forward_speed >= MovementThreshold ) && ( AbsFwd >= AbsRight );
        _is_moving_backward_real = ( _current_forward_speed <= -MovementThreshold ) && ( AbsFwd >= AbsRight );
        _is_moving_right_real = ( _current_right_speed >= MovementThreshold ) && ( AbsRight > AbsFwd );
        _is_moving_left_real = ( _current_right_speed <= -MovementThreshold ) && ( AbsRight > AbsFwd );
        _active_block = OwningCharacter->GetActiveBlock( );
        _is_block = OwningCharacter->GetIsBlock( );
    }
}



