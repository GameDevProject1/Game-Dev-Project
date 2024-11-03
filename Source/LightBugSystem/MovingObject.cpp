#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "MovingObject.h"

// Sets default values for this component's properties
UMovingObject::UMovingObject()
{
	PrimaryComponentTick.bCanEverTick = true;
	ElapsedTime = 0.0f;
	bMovingToDestination = true; // Start by moving to the destination
}



// Called when the game starts
void UMovingObject::BeginPlay()
{
	Super::BeginPlay();
	BeginningLocation = GetOwner()->GetActorLocation();

    if (!isAuto) {
        ShouldMove = false;
        BoxComponent = GetOwner()->FindComponentByClass<UBoxComponent>();

        if (BoxComponent != nullptr)
        {

            BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &UMovingObject::OnOverlapBegin);
            BoxComponent->OnComponentEndOverlap.AddDynamic(this, &UMovingObject::OnOverlapEnd);
        }
    } else ShouldMove = true;
}

// Called every frame
void UMovingObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (ShouldMove)
	    MovePlatform(DeltaTime);

	RotatePlatform(DeltaTime);
}

void UMovingObject::MovePlatform(float DeltaTime)
{
    if (ElapsedTime < Duration)
    {
        ElapsedTime += DeltaTime;

        // Calculate the progress based on elapsed time and duration
        float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);

        // Determine the current target location
        FVector StartLocation = bMovingToDestination ? BeginningLocation : Destination;
        FVector EndLocation = bMovingToDestination ? Destination : BeginningLocation;

        // Interpolate between the current location and the target
        FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
        GetOwner()->SetActorLocation(NewLocation);

        // Check if we reached the target
        if (Alpha >= 1.0f)
        {
            if (!isAuto && CharacterOff) 
            {
                ShouldMove = false;
            }

            // Reset for the next movement
            ElapsedTime = 0.0f;
            bMovingToDestination = !bMovingToDestination; // Toggle direction
        }
    }
}

void UMovingObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (GetOwner() == nullptr) return;

    
        if (OtherActor && (OtherActor != GetOwner()))
        {
            ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
            if (PlayerCharacter && OtherActor == PlayerCharacter)
            {
                ShouldMove = true;
                CharacterOff = false;
            }
        }
    
}

void UMovingObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

    if (GetOwner() == nullptr) return;

    if (OtherActor && (OtherActor != GetOwner()))
    {
        ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter && OtherActor == PlayerCharacter)
        {
            CharacterOff = true;
        }
    }
}


void UMovingObject::RotatePlatform(float DeltaTime)
{
	GetOwner()->AddActorLocalRotation(RotationVelocity * DeltaTime);
}