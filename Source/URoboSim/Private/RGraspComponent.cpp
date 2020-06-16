#include "RGraspComponent.h"
#include "Physics/RModel.h"
#include "Physics/RLink.h"

URGraspComponent::URGraspComponent()
{
  InitSphereRadius(10.f);
  SetGenerateOverlapEvents(true);
  SetEnableGravity(false);
  Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Constraint"));
  Constraint->SetupAttachment(this);
  Constraint->ConstraintInstance.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
  Constraint->ConstraintInstance.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
  Constraint->ConstraintInstance.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 2);
}

void URGraspComponent::Init(URStaticMeshComponent* InGripper)
{
  Gripper = InGripper;
  bObjectGrasped = false;

  OnComponentBeginOverlap.AddDynamic(this, &URGraspComponent::OnFixationGraspAreaBeginOverlap);
  OnComponentEndOverlap.AddDynamic(this, &URGraspComponent::OnFixationGraspAreaEndOverlap);

}

void URGraspComponent::BeginPlay()
{
  Super::BeginPlay();
}

void URGraspComponent::OnFixationGraspAreaBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
                                                       class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
  if (ARModel* SMA = Cast<ARModel>(OtherActor))
    {
      return;
    }
  if (AStaticMeshActor* OtherSMA = Cast<AStaticMeshActor>(OtherActor))
    {
      ObjectsInReach.Emplace(OtherSMA);
      // UE_LOG(LogTemp, Warning, TEXT("InReach %s"), *OtherSMA->GetName());
    }
}

void URGraspComponent::OnFixationGraspAreaEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
                                                     class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  // Remove actor from array (if present)
  if (AStaticMeshActor* SMA = Cast<AStaticMeshActor>(OtherActor))
    {
      ObjectsInReach.Remove(SMA);
    }
}

// Try to fixate object to hand
bool URGraspComponent::TryToFixate()
{
  bool bSuccess = false;

  // if(bObjectGrasped)
  if(!bObjectGrasped && ObjectsInReach.Num() > 0)
    {
      // Pop a SMA
      AStaticMeshActor* SMA = ObjectsInReach[0];

      // Check if the actor is graspable
      FixateObject(SMA);
    }

  //   {
  //     bSuccess = true;
  //   }

  // return bSuccess;
  return bObjectGrasped;
}

// Fixate object to hand
void URGraspComponent::FixateObject(AStaticMeshActor* InSMA)
{
  // AStaticMeshActor* ConstrainedActor = Cast<AStaticMeshActor>(InSMA->GetAttachParentActor());
  AStaticMeshActor* ConstrainedActor = InSMA;

  //If the grasped object is attached to another object (door handle), connecting via constraints moves
  // the gripper to the root object
  bool bParentFound = false;
  while(!bParentFound)
    {
      AStaticMeshActor* TempActor = Cast<AStaticMeshActor>(ConstrainedActor->GetAttachParentActor());
      if(TempActor)
        {
          ConstrainedActor = TempActor;
        }
      else
        {
          bParentFound = true;
        }
    }

  UStaticMeshComponent* SMC = nullptr;
  SMC = ConstrainedActor->GetStaticMeshComponent();

  if(!SMC)
    {
      UE_LOG(LogTemp, Error, TEXT("RootComponent of InSMA has no static mesh"));
      return;
    }


  // if(InSMA == ConstrainedActor)
  //   {
  //     // Set the fixated object
  //     FixatedObject = ConstrainedActor;
  //     ConstrainedActor->GetStaticMeshComponent()->SetSimulatePhysics(false);
  //     ConstrainedActor->AttachToComponent(Gripper, FAttachmentTransformRules::KeepWorldTransform);
  //   }
  // else
  //   {
  //     Constraint->SetConstrainedComponents(Gripper, NAME_None, SMC, NAME_None);
  //   }

  FixatedObject = ConstrainedActor;
  Constraint->SetConstrainedComponents(Gripper, NAME_None, SMC, NAME_None);
  bGraspObjectGravity = SMC->IsGravityEnabled();
  bObjectGrasped = true;
  SMC->SetEnableGravity(false);


}

// Detach fixation
void URGraspComponent::TryToDetach()
{
  // if(FixatedObject)
  //   {
  //     FixatedObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
  //     FixatedObject->GetStaticMeshComponent()->SetSimulatePhysics(true);
  //     FixatedObject = nullptr;
  //   }
  // else
  //   {
  //     Constraint->BreakConstraint();
  //   }
  Constraint->BreakConstraint();
  if(FixatedObject)
    {
      FixatedObject->GetStaticMeshComponent()->SetEnableGravity(bGraspObjectGravity);
      FixatedObject = nullptr;
    }
  bObjectGrasped = false;
}