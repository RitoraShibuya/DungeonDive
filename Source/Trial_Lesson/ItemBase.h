#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

// ëOï˚êÈåæ
class ACharacterBase;
class USphereComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class TRIAL_LESSON_API AItemBase : public AActor
{
	GENERATED_BODY()

public:
	AItemBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bCanPickup;

public:
	virtual void Tick(float DeltaTime) override;

	FString GetItemName() const {
		return ItemName;
	}

	// èEÇÌÇÍÇΩéûÇÃèàóù
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void OnPickup(ACharacterBase* PickupInstigator);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintCallable)
	void MoveVertical( int z);

private:
	bool moveup = true;
	int move_amount = 0;
};

