#include "ItemBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "CharacterBase.h"

// コンストラクタ
AItemBase::AItemBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // 当たり判定用のSphere
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->InitSphereRadius(50.f);
    CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = CollisionSphere;

    // メッシュ
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    ItemMesh->SetupAttachment(RootComponent);

    // デフォルト設定
    ItemName = TEXT("DefaultItem");
    bCanPickup = true;
}

// ゲーム開始時
void AItemBase::BeginPlay()
{
    Super::BeginPlay();

    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);
}

// Tick
void AItemBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// アイテムを拾った処理
void AItemBase::OnPickup(ACharacterBase* PickupInstigator)
{
    if (bCanPickup && PickupInstigator)
    {
        // キャラ側でカウント増やす
        PickupInstigator->AddItemCount();

        PickupInstigator->OnItemPickedUp( this );

        //UE_LOG(LogTemp, Log, TEXT("%s picked up by %s"), *ItemName, *PickupInstigator->GetName());

        // 破壊
        Destroy();
    }
}

// 衝突検知
void AItemBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        // キャラクターだったら拾わせる
        if (ACharacterBase* Char = Cast<ACharacterBase>(OtherActor))
        {
            OnPickup(Char);
        }
    }
}

void AItemBase::MoveVertical(int z) 
{
    move_amount = z;

    FVector Location = GetActorLocation();
    if (moveup) 
    {
        Location.Z += move_amount;
    }
    else 
    {
        Location.Z -= move_amount;
    }

    SetActorLocation(Location);

    moveup = !moveup;
}
