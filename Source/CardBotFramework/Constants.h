#pragma once

#include "Constants.generated.h"


/**
 * Action types to define routing policies by "channels"
 */
UENUM(BlueprintType, meta=(Bitflags) )
enum class EActionType : uint8
{
    Move        = 0x01,
    Attack      ,
    Defense     ,
    Morph       ,
    Special     ,
    System      ,
    Custom1     ,
    Custom2     ,
    Custom3     ,
    Custom4     ,
    Custom5     ,
    Custom6     ,
    Custom7     ,
    Custom8     ,
    Custom9
};


/**
 * Sensors types to define routing policies by "channels"
 */
UENUM(BlueprintType, meta=(Bitflags) )
enum class ESensorType : uint8
{
    Damage        = 0x01,
    Collision     ,
    Sonar         ,
    System        ,
    Custom1       ,
    Custom2       ,
    Custom3       ,
    Custom4       ,
    Custom5       ,
    Custom6       ,
    Custom7       ,
    Custom8       ,
    Custom9
};


/**
 * FName forced indexes for optim purpose
 */
enum EFNameIndex
{
    OnPartHit           = 0x186A0,
    OnPartBeginOverlap  ,
    OnPartEndOverlap    ,
    OnPartDamage        ,
    OnPartBroken
};
