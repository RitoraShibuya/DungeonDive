#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CountDownWidget.generated.h"

class UTextBlock;

UCLASS( )
class TRIAL_LESSON_API UCountDownWidget : public UUserWidget {
    GENERATED_BODY( )

public:
    // BPのクラスを渡して生成するように変更
    UFUNCTION( BlueprintCallable, Category = "Countdown" )
    static UCountDownWidget* ShowCountdown( UObject* WorldContextObject, TSubclassOf<UCountDownWidget> WidgetClass );

    UFUNCTION( BlueprintCallable, Category = "Countdown" )
    void StartCountdown( );

    UFUNCTION( BlueprintCallable, Category = "Countdown" )
    void UpdateCountdown( );

protected:
    virtual void NativeConstruct( ) override;

    // BP側で作ったTextBlockをバインド
    UPROPERTY( meta = ( BindWidget ) )
    UTextBlock* CountdownText;


    int32 cressent_count;

    FTimerHandle countdowntimer;
};
