#pragma once

#include "Action.generated.h"


/**
 * Action types to define routing policies by "channels"
 */
UENUM(BlueprintType, meta=(Bitflags) )
enum class EActionType : uint8
{
    Move        = 1  ,
    Attack      = 2  ,
    Defense     = 3  ,
    Morph       = 4  ,
    Special     = 5  ,
    System      = 6
};
