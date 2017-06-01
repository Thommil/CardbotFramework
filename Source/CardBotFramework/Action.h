#pragma once

#include "Action.generated.h"


/**
 * Action types to define routing policies by "channels"
 */
UENUM(BlueprintType)
enum class EActionType : uint8
{
    Move        = 0x01  ,
    Attack      = 0x02  ,
    Custom1     = 0x04  ,
    Custom2     = 0x08
};


/**
 * Action types to define routing capabilities by "channels"
 */
UENUM(BlueprintType, meta=(Bitflags))
enum class EActionCapability : uint8
{
    Move        = 0x01  ,
    Attack      = 0x02  ,
    Custom1     = 0x04  ,
    Custom2     = 0x08
};
ENUM_CLASS_FLAGS(EActionCapability)

/**
 * Action flags to define global and custom routing policies
 */
UENUM(BlueprintType, meta=(Bitflags))
enum class EActionFlag : uint8
{
    User1       = 0x01  ,
    User2       = 0x02  ,
    User3       = 0x04  ,
    User4       = 0x08
};
ENUM_CLASS_FLAGS(EActionFlag)

