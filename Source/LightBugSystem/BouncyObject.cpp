#include "BouncyObject.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"  // For GEngine

UBouncyObject::UBouncyObject()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBouncyObject::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner() == nullptr) return;

    InitialLocation = GetOwner()->GetActorLocation();
    TargetLocation = InitialLocation + FVector(0, 0, -25);

    BoxComponent = GetOwner()->FindComponentByClass<UBoxComponent>();

    bShouldMoveDown = false;
    bShouldMoveUp = false;
    bHasMoved = false; // New flag to prevent repeated movement
    TotalMoveTime = 0.6f; // Total time to move down and up
    CurrentMoveTime = 0.0f;


    if (BoxComponent != nullptr)
    {
        
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &UBouncyObject::OnOverlapBegin);
        BoxComponent->OnComponentEndOverlap.AddDynamic(this, &UBouncyObject::OnOverlapEnd);
    }

}

void UBouncyObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bHasMoved) return;

    if (bShouldMoveDown)
    {

        CurrentMoveTime += DeltaTime;
        float LerpValue = FMath::Clamp(CurrentMoveTime / (TotalMoveTime / 2), 0.0f, 1.0f);
        FVector NewLocation = FMath::Lerp(InitialLocation, TargetLocation, LerpValue);
        GetOwner()->SetActorLocation(NewLocation);

        if (LerpValue >= 1.0f)
        {
            bShouldMoveDown = false;
            bShouldMoveUp = true;
            CurrentMoveTime = 0.0f;
        }
    }
    else if (bShouldMoveUp)
    {
        
        CurrentMoveTime += DeltaTime;
        float LerpValue = FMath::Clamp(CurrentMoveTime / (TotalMoveTime / 2), 0.0f, 1.0f);
        FVector NewLocation = FMath::Lerp(TargetLocation, InitialLocation, LerpValue);
        GetOwner()->SetActorLocation(NewLocation);

        if (LerpValue >= 1.0f)
        {

            bShouldMoveUp = false;
            bHasMoved = true; // Set flag to prevent further movement
        }
    }
}

void UBouncyObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (GetOwner() == nullptr) return;

    if (!bHasMoved) // Only move if it hasn't moved yet
    {
        if (OtherActor && (OtherActor != GetOwner()))
        {
            ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
            if (PlayerCharacter && OtherActor == PlayerCharacter)
            {
                StartMovement();
            }
        }
    }
}

void UBouncyObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

    if (GetOwner() == nullptr) return;

    if (OtherActor && (OtherActor != GetOwner()))
    {
        ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter && OtherActor == PlayerCharacter)
        {
            bHasMoved = false;
        }
    }
}

void UBouncyObject::StartMovement()
{
    bShouldMoveDown = true;
    CurrentMoveTime = 0.0f;
}
