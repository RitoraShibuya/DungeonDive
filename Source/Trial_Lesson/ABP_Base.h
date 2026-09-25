// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimDataCharacter.h"
#include "ABP_Base.generated.h"


UCLASS( )
class TRIAL_LESSON_API UABP_Base : public UAnimInstance {
	GENERATED_BODY( )

protected:
	virtual void NativeInitializeAnimation( ) override;
	virtual void NativeUpdateAnimation( float DeltaSeconds ) override;

	TWeakObjectPtr<AAnimDataCharacter> OwningCharacter;

	UPROPERTY( EditDefaultsOnly, Category = "MovementSettings" )
	float MovementThreshold = 3.0f;

	UPROPERTY( BlueprintReadOnly, Category = "MovementState" )
	float _current_forward_speed = 0.0f;

	UPROPERTY( BlueprintReadOnly, Category = "MovementState" )
	float _current_right_speed = 0.0f;

	UPROPERTY( BlueprintReadOnly, Category = "MovementState" )
	bool _is_in_air = false;

	UPROPERTY( BlueprintReadOnly, Category = "MovementState" )
	bool _is_moving_forward_real = false;

	UPROPERTY( BlueprintReadOnly, Category = "MovementState" )
	bool _is_moving_backward_real = false;

	UPROPERTY( BlueprintReadOnly, Category = "MovementState" )
	bool _is_moving_right_real = false;

	UPROPERTY( BlueprintReadOnly, Category = "MovementState" )
	bool _is_moving_left_real = false;

	UPROPERTY( BlueprintReadOnly, Category = "InputState" )
	bool _is_moving_forward = false;

	UPROPERTY( BlueprintReadOnly, Category = "InputState" )
	bool _is_moving_right = false;

	UPROPERTY( BlueprintReadOnly, Category = "InputState" )
	bool _is_moving_backward = false;

	UPROPERTY( BlueprintReadOnly, Category = "InputState" )
	bool _is_moving_left = false;

	UPROPERTY( BlueprintReadOnly, Category = "InputState" )
	bool _is_jumping = false;

	UPROPERTY( BlueprintReadOnly, Category = "Block" )
	bool _active_block = false;

	UPROPERTY( BlueprintReadOnly, Category = "Block" )
	bool _is_block = false;
};
