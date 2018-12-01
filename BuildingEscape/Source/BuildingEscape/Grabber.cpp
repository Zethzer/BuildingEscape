// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }

	// If physics handle have a component grabbed
	if (PhysicsHandle->GrabbedComponent)
	{
		// Move it
		PhysicsHandle->SetTargetLocation(GetReachLineTracePoints().v2);
	}
}

void UGrabber::Grab()
{
	// Debug
	//UE_LOG(LogTemp, Warning, TEXT("Grab pressed"))

	if (!PhysicsHandle) { return; }

	// Get result of the hit
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	// If we hit an actor
	if (ActorHit)
	{
		// Grab it
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			FRotator(0.f)
		);
	}
}

void UGrabber::Release()
{
	// Debug
	//UE_LOG(LogTemp, Warning, TEXT("Grab release"))

	if (!PhysicsHandle) { return; }

	// If physics handle have a component grabbed
	if (PhysicsHandle->GrabbedComponent)
	{
		// Release it
		PhysicsHandle->ReleaseComponent();
	}
}

void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName())
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName())
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	FCollisionQueryParams DefaultTraceParameters(FName(TEXT("")), false, GetOwner());
	FHitResult HitResult;
	FTwoVectors TracePoints = GetReachLineTracePoints();

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		TracePoints.v1,
		TracePoints.v2,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		DefaultTraceParameters
	);

	// Debug
	/*UE_LOG(LogTemp, Warning, TEXT("VP_L : %s | VP_R : %s"),
		*PlayerViewPointLocation.ToString(),
		*PlayerViewPointRotation.ToString()
	)
	// Draw a red trace (Debug)
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor().Red,
		false,
		0.f,
		0.f,
		10.f
	);
	// See what we hit
	AActor* HitActor = LineTraceHit.GetActor();
	if (HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit on %s"), *HitActor->GetName())
	}*/

	return HitResult;
}

FTwoVectors UGrabber::GetReachLineTracePoints()
{
	// Get the player viewpoint
	FVector StartLocation;
	FRotator PlayerViewPointRotation;
	
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT StartLocation,
		OUT PlayerViewPointRotation
	);

	FVector EndLocation = StartLocation + PlayerViewPointRotation.Vector() * Reach;
	
	return FTwoVectors(StartLocation, EndLocation);
}

