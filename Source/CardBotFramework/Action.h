#pragma once

#include "Action.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EActionType : uint8
{
    EAT_Move             UMETA(DisplayName="Move"),
    EAT_Transform        UMETA(DisplayName="Transform"),
    EAT_Attack           UMETA(DisplayName="Attack"),
    EAT_User1            UMETA(DisplayName="User1"),
    EAT_User2            UMETA(DisplayName="User2"),
    EAT_User3            UMETA(DisplayName="User3"),
    EAT_User4            UMETA(DisplayName="User4"),
    EAT_User5            UMETA(DisplayName="User5"),
    EAT_User6            UMETA(DisplayName="User6"),
    EAT_User7            UMETA(DisplayName="User7"),
    EAT_User8            UMETA(DisplayName="User8"),
    EAT_User9            UMETA(DisplayName="User9")
};

/**
 *
 */
UENUM(BlueprintType)
enum class EActionFlag : uint8
{
    EAF_User1             UMETA(DisplayName="User1"),
    EAF_User2             UMETA(DisplayName="User2"),
    EAF_User3             UMETA(DisplayName="User3"),
    EAF_User4             UMETA(DisplayName="User4"),
    EAF_User5             UMETA(DisplayName="User5"),
    EAF_User6             UMETA(DisplayName="User6"),
    EAF_User7             UMETA(DisplayName="User7"),
    EAF_User8             UMETA(DisplayName="User8"),
    EAF_User9             UMETA(DisplayName="User9")
};
