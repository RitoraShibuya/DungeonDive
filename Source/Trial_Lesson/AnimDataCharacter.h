#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AnimDataCharacter.generated.h"

UCLASS( )
class TRIAL_LESSON_API AAnimDataCharacter : public ACharacter {
    GENERATED_BODY( )

public:
    virtual void Tick( float DeltaTime ) override;

    UFUNCTION( BlueprintPure, Category = "AnimData|Speed" )
    float GetCurrentForwardSpeed( ) const { return CurrentForwardSpeed; }

    UFUNCTION( BlueprintPure, Category = "AnimData|Speed" )
    float GetCurrentRightSpeed( ) const { return CurrentRightSpeed; }

    UFUNCTION( BlueprintPure, Category = "AnimData|State" )
    bool IsInAir( ) const { return bIsInAir; }

    UFUNCTION( BlueprintPure, Category = "AnimData|Input" )
    bool HasFrontMoveAction( ) const { return bFrontMoveActionExist; }

    UFUNCTION( BlueprintPure, Category = "AnimData|Input" )
    bool HasRightMoveAction( ) const { return bRightMoveActionExist; }

    UFUNCTION( BlueprintPure, Category = "AnimData|Input" )
    bool HasBackMoveAction( ) const { return bBackMoveActionExist; }

    UFUNCTION( BlueprintPure, Category = "AnimData|Input" )
    bool HasLeftMoveAction( ) const { return bLeftMoveActionExist; }

    UFUNCTION( BlueprintPure, Category = "AnimData|Input" )
    bool IsJumpingAction( ) const { return bJumpActionExist; }

    UFUNCTION( BlueprintPure, Category = "AnimData|Block" )
    bool GetActiveBlock( ) const { return active_block; }

    UFUNCTION( BlueprintPure, Category = "AnimData|Block" )
    bool GetIsBlock( ) const { return is_block; }

protected:
    UPROPERTY( BlueprintReadOnly, Category = "AnimData|Speed" )
    float CurrentForwardSpeed = 0.0f;

    UPROPERTY( BlueprintReadOnly, Category = "AnimData|Speed" )
    float CurrentRightSpeed = 0.0f;

    UPROPERTY( BlueprintReadOnly, Category = "AnimData|State" )
    bool bIsInAir = false;

    UPROPERTY( BlueprintReadOnly, Category = "AnimData|Input" )
    bool bFrontMoveActionExist = false;

    UPROPERTY( BlueprintReadOnly, Category = "AnimData|Input" )
    bool bRightMoveActionExist = false;

    UPROPERTY( BlueprintReadOnly, Category = "AnimData|Input" )
    bool bBackMoveActionExist = false;

    UPROPERTY( BlueprintReadOnly, Category = "AnimData|Input" )
    bool bLeftMoveActionExist = false;

    UPROPERTY( BlueprintReadOnly, Category = "AnimData|Input" )
    bool bJumpActionExist = false;

    UPROPERTY( BlueprintReadOnly, Category = "AnimData|Block" )
    bool active_block = false;

    UPROPERTY( BlueprintReadOnly, Category = "AnimData|Block" )
    bool is_block = false;
};