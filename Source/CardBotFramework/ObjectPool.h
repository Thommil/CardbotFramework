#pragma once

#include "ObjectPool.generated.h"

/**
 * Generic basic Object Pool to use in Blueprints
 *  - Generate BluePrint Class for pooling
 *  - Override InstanciateObject() to create your own content
 *  - Override Reset() to reset objectc before push back
 *  - Can grow with WARNING
 */
UCLASS(BlueprintType, Blueprintable)
class CARDBOTFRAMEWORK_API UObjectPool : public UObject
{
    GENERATED_BODY()

public:
    UObjectPool()
    {
        ObjectClass = UObject::StaticClass();
        Size = DEFAULT_OBJECT_POOL_SIZE;
    }

    
protected:
    
    TArray<UObject*> InnerQueue;
    
public:
    virtual void PostInitProperties() override
    {
        Super::PostInitProperties();
        InnerQueue.Reserve(Size);
        for(int32 i=0; i < Size; i++)
        {
            InnerQueue.Push(InstanciateObject());
        }
    }
    
    /** Class of inner objects (for auto instanciation) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    TSubclassOf<UObject> ObjectClass;
    
    /** Size of Pool */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardBot")
    int32 Size;
    
    /** Object creator to override for specific constructor implementation */
    UFUNCTION(BlueprintNativeEvent, Category="CardBot")
    UObject* InstanciateObject();
    virtual UObject* InstanciateObject_Implementation()
    {
         return NewObject<UObject>(GetTransientPackage(), ObjectClass);
    }
    
    /** Object reset to override for specific reset implementation */
    UFUNCTION(BlueprintNativeEvent, Category="CardBot")
    void ResetObject(UObject* object);
    virtual void ResetObject_Implementation(UObject* object){}
    
    /** Retrieve an object from the pool */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    UObject* Pop()
    {
        if(InnerQueue.Num() == 0)
        {
            WARNING(TEXT("Extending Depleted Pool -> check Push() call or initial size"));
            for(int32 i=0; i < Size; i++)
            {
                InnerQueue.Push(InstanciateObject());
            }
        }
        return InnerQueue.Pop();
    }
    
    /** Puhs back an object to the pool */
    UFUNCTION(BlueprintCallable, Category="CardBot")
    void Push(UObject* object)
    {
        ResetObject(object);
        InnerQueue.Push(object);
    }
};

/**
 * Raw templated version Object Pool to use in CPP
 *  - Override InstanciateObject() to create your own content
 *  - Override Reset() to reset objectc before push back
 *  - Can grow with WARNING
 */

template<typename T>
class CARDBOTFRAMEWORK_API TObjectPool
{
    
public:
    TObjectPool(int32 size = DEFAULT_OBJECT_POOL_SIZE)
    {
        Size = size;
        InnerQueue.Reserve(Size);
        for(int32 i=0; i < Size; i++)
        {
            InnerQueue.Push(InstanciateObject());
        }
    }
    
    
protected:
    
    TArray<T*> InnerQueue;
    
    /** Object creator to override for specific constructor implementation */
    virtual T* InstanciateObject()
    {
        return NewObject<T>(GetTransientPackage(), T::StaticClass());
    }
    
public:
    
    /** Size of Pool */
    int32 Size;
    
    /** Object reset to override for specific reset implementation */
    virtual void ResetObject(T* object){}
    
    /** Retrieve an object from the pool */
    T* Pop()
    {
        if(InnerQueue.Num() == 0)
        {
            WARNING(TEXT("Extending Depleted Pool -> check Push() call or initial size"));
            for(int32 i=0; i < Size; i++)
            {
                InnerQueue.Push(InstanciateObject());
            }
        }
        return InnerQueue.Pop();
    }
    
    /** Puhs back an object to the pool */
    void Push(T* object)
    {
        ResetObject(object);
        InnerQueue.Push(object);
    }
};
