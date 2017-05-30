#pragma once

#include "BaseEvent.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EEventType : uint8
{
    EET_Move             UMETA(DisplayName="Move"),
    EET_Transform        UMETA(DisplayName="Transform"),
    EET_Attack           UMETA(DisplayName="Attack"),
    EET_User1            UMETA(DisplayName="User1"),
    EET_User2            UMETA(DisplayName="User2"),
    EET_User3            UMETA(DisplayName="User3"),
    EET_User4            UMETA(DisplayName="User4"),
    EET_User5            UMETA(DisplayName="User5"),
    EET_User6            UMETA(DisplayName="User6"),
    EET_User7            UMETA(DisplayName="User7"),
    EET_User8            UMETA(DisplayName="User8"),
    EET_User9            UMETA(DisplayName="User9")
};

/**
 *
 */
UENUM(BlueprintType)
enum class EEventFlag : uint8
{
    EEF_User1             UMETA(DisplayName="User1"),
    EEF_User2             UMETA(DisplayName="User2"),
    EEF_User3             UMETA(DisplayName="User3"),
    EEF_User4             UMETA(DisplayName="User4"),
    EEF_User5             UMETA(DisplayName="User5"),
    EEF_User6             UMETA(DisplayName="User6"),
    EEF_User7             UMETA(DisplayName="User7"),
    EEF_User8             UMETA(DisplayName="User8"),
    EEF_User9             UMETA(DisplayName="User9")
};

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class CARDBOTFRAMEWORK_API UBaseEvent : public UObject
{
    GENERATED_BODY()
    
public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    EEventType Type;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot", meta=(Bitmask))
    EEventFlag Flags;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    UStruct* data;

};
