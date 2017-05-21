#include "CardBotFramework.h"
#include "Bot.h"

ABot::ABot()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABot::PreInitializeComponents()
{
    Super::PreInitializeComponents();
    
    //Assemble parts
    this->assemble();
}

UActorComponent* ABot::GetComponentByName(FName name)
{
    TSet<UActorComponent*> components = this->GetComponents();
    for (UActorComponent* component : components)
    {
        if(component->GetFName() == name){
            return component;
        }
    }
    return NULL;
}

void ABot::addPart(ABotPart* part)
{
    
}


void ABot::removePart(FName name)
{
    
}


void ABot::assemble()
{
    this->Parts.Empty();
    
    /*// Check and set RootPart
     UActorComponent* rootPartComponent = this->GetComponentByName(this->RootPartName);
     if(rootPartComponent == NULL)
     {
     UE_LOG(LogTemp, Warning, TEXT("Root part not found, cannot assemble Bot"));
     return;
     }
     if(!rootPartComponent->IsA(UChildActorComponent::StaticClass()))
     {
     UE_LOG(LogTemp, Warning, TEXT("Root part is not a UChildActorComponent, cannot assemble Bot"));
     return;
     }
     if(!((UChildActorComponent*)rootPartComponent)->GetChildActor()->IsA(ABodyPart::StaticClass()))
     {
     UE_LOG(LogTemp, Warning, TEXT("Root part child Actor is not a ABodyPart, cannot assemble Bot"));
     return;
     }
     this->RootPart = (ABodyPart*)((UChildActorComponent*)rootPartComponent)->GetChildActor();
     
     // Set All Parts
     TArray<AActor*> childActors;
     this->GetAllChildActors(childActors, false);
     for(AActor* childActor : childActors)
     {
     if(childActor->IsA(ABodyPart::StaticClass()))
     {
     this->Parts.Add((ABodyPart*)childActor);
     }
     }
     
     // Assemble parts starting from RootPart
     this->assemblePart(this->RootPart);*/
}

void ABot::disassemble()
{
    
}

void ABot::assemblePart(ABotPart& part)
{
    /*
     //Parse sockets
     for(FSocketConnector& socketConnector : part->Sockets)
     {
     //Parse all parts to find Plugs using this socket
     for(ABodyPart* plugPart : this->Parts)
     {
     //Parse plugs
     for(FPlugConnector& plugConnector : plugPart->Plugs)
     {
     //Part owning corresponding Plug found
     if(plugConnector.SocketName == socketConnector.Name)
     {
     UActorComponent* plugComponent = plugPart->GetComponentByName(plugConnector.ComponentName);
     //Plug is a PhysicsConstraintComponent -> Make a HingeJoint to socket parent
     if(plugComponent == NULL)
     {
     UE_LOG(LogTemp, Warning, TEXT("Plug component not found"));
     }
     else if(plugComponent != NULL && plugComponent->IsA(UPhysicsConstraintComponent::StaticClass()))
     {
     //Look at socket position to place plug
     UActorComponent* socketComponent = part->GetComponentByName(socketConnector.ComponentName);
     if(socketComponent == NULL)
     {
     UE_LOG(LogTemp, Warning, TEXT("Socket component not found"));
     }
     else if(socketComponent != NULL && socketComponent->IsA(USceneComponent::StaticClass()))
     {
     FVector plugLocation =  ((USceneComponent*)socketComponent)->GetComponentLocation()
     + plugPart->GetRootComponent()->GetComponentLocation()
     - ((UPhysicsConstraintComponent*)plugComponent)->GetComponentLocation();
     UE_LOG(LogTemp, Warning, TEXT("Joint %s"), *plugLocation.ToString());
     plugPart->SetActorLocation(plugLocation);
     UE_LOG(LogTemp, Warning, TEXT("Joint %s"), *((UPhysicsConstraintComponent*)plugComponent)->GetComponentLocation().ToString());
     ((UPhysicsConstraintComponent*)plugComponent)->SetWorldLocation(((USceneComponent*)socketComponent)->GetComponentLocation());
     UE_LOG(LogTemp, Warning, TEXT("Joint %s"), *((UPhysicsConstraintComponent*)plugComponent)->GetComponentLocation().ToString());
     
     }
     else
     {
     UE_LOG(LogTemp, Warning, TEXT("Socket component not supported, must be a USceneComponent"));
     }
     
     }
     else
     {
     UE_LOG(LogTemp, Warning, TEXT("Plug component not supported, must be a UPhysicsConstraintComponent"));
     }
     }
     }
     }
     }*/
}

void ABot::disassemblePart(ABotPart& part)
{
    
}
