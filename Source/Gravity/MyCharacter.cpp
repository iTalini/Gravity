// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "ParentButton.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "ScoreSpheres.h"
#include "Net/UnrealNetwork.h"
#include "MyHUD.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.0f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 100.f;
	GetCharacterMovement()->FallingLateralFriction = 100.f;
	GetCharacterMovement()->GravityScale = 1.5f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ControllerDir = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowContrDir"));
	ControllerDir->SetArrowColor(FLinearColor::Blue);
	ControllerDir->SetupAttachment(GetCapsuleComponent());


	InputDir = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowInpDir"));
	InputDir->SetArrowColor(FLinearColor::Yellow);
	InputDir->SetupAttachment(GetCapsuleComponent());

	Score = 0;

	bReadyForGravState = false;
	bGravit_line_trace = false;
	bCheckGrav = false;

	CurrentButton = nullptr;

	f_force = 0;

	jumping = false;

	bIsUp = false;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle ExplosionTimer;

	auto richCurve = new FRichCurve();
	auto key = richCurve->AddKey(0.0f, 0.f);
	richCurve->AddKey(0.5f, 180.f);
	richCurve->SetKeyInterpMode(key, RCIM_Linear);

	ChangeGravCurve = NewObject<UCurveFloat>();

	ChangeGravCurve->FloatCurve = *richCurve;

	InitializeGravCurve();
}


void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacter, Score);
}

void AMyCharacter::SetCurrentButton(AParentButton* NewCurrentButton)
{
	CurrentButton = NewCurrentButton;
}

void AMyCharacter::OnRep_Score()
{
	PrintScore_Implementation();
}

void AMyCharacter::PrintScore_Implementation()
{
	if (IsLocallyControlled())
	{
		AMyHUD* InGameHUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		if (InGameHUD)
			InGameHUD->UpdateScore(Score);
	}
}

void AMyCharacter::Up()
{
	OnRep_Set_CheckGrav(true);
	OnRep_Set_GravLineTrace(true);
	ChangeGravTimeline.PlayFromStart();
	bIsUp = true;
}

void AMyCharacter::Down()
{
	OnRep_Set_CheckGrav(false);
	ChangeGravTimeline.Reverse();
	bIsUp = false;
}

void AMyCharacter::OnInteract()
{
	if (HasAuthority())
	{
		if (CurrentButton && CurrentButton->Get_ReadyStat())
		{
			if (bIsUp)
			{
				Down();
			}
			else
			{
				Up();
			}
			CurrentButton->TryPressButton();
		}
	}
	else
	{
		Server_OnInteract();
	}
}

bool AMyCharacter::Server_OnInteract_Validate()
{
	return true;
}

void AMyCharacter::Server_OnInteract_Implementation()
{
	OnInteract();
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ArrowRot();

	if (bCheckGrav)
	{
		FVector force(0.0f, 0.0f, f_force);

		GetCharacterMovement()->AddForce(force);
	}

	ChangeGravTimeline.TickTimeline(DeltaTime);
}

//Diraction for walking forward
FVector AMyCharacter::CalcInputDirForw()
{
	/*GetForwardVector
		FRotationMatrix(FRotator  InRot).GetScaledAxis( EAxis::X );*/

	FVector DirForw = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::X);
	DirForw.Normalize();

	return DirForw;
}

//Diraction for walking left/right
FVector AMyCharacter::CalcInputDirRight()
{
	/*GetRightVector
		FRotationMatrix(FRotator  InRot).GetScaledAxis( EAxis::Y );*/

	FVector DirRight = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::Y);
	DirRight.Normalize();
	return DirRight;
}


void AMyCharacter::TimelineProgress()
{
	TimelineValue = ChangeGravTimeline.GetPlaybackPosition();

	FQuat NewRotate = FQuat(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, ChangeGravCurve->GetFloatValue(TimelineValue)));
	SetActorRotation(NewRotate);

	//f_force = 11.0f * -25000.0f;337000
	SetF_Force(337000.0f);

	//rotate character's camera too
	/*FollowCamera->GetComponentRotation();
	FQuat NewCameraRotation = FQuat(FRotator(FollowCamera->GetComponentRotation().Pitch, FollowCamera->GetComponentRotation().Yaw, ChangeGravCurve->GetFloatValue(TimelineValue)));

	FollowCamera->SetWorldRotation(NewCameraRotation);*/
}

void AMyCharacter::TimelineFinish()
{
	OnRep_Set_GravLineTrace(false);
}

void AMyCharacter::SetF_Force(float input)
{
	f_force = input;
}

void AMyCharacter::AddToScore(float add)
{
	Score += add;
	OnRep_Score();
}

void AMyCharacter::OnRep_Set_CheckGrav(bool input)
{
	bCheckGrav = input;
}

void AMyCharacter::OnRep_Set_GravLineTrace(bool input)
{
	bReadyForGravState = input;
}


void AMyCharacter::ArrowRot()
{
	float newZ = GetMesh()->GetAnimInstance()->CalculateDirection(GetVelocity(), GetActorRotation());
	InputDir->SetWorldRotation(FRotator(0.0f, newZ, 0.0f));
	ControllerDir->SetWorldRotation(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
}

void AMyCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);


		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		if (bCheckGrav)
			AddMovementInput(Direction, -Value);
		else
			AddMovementInput(Direction, Value);
		
	}
}

void AMyCharacter::Jump_()
{
	if (bCheckGrav)
	{
		LaunchCharacter(FVector(0.0f, 0.0f, -600.0f), false, false);
	}
	else
	{
		ACharacter::Jump();
	}

	OnRep_Set_GravLineTrace(true);
}


void AMyCharacter::Stop_Jumping()
{
	ACharacter::StopJumping();
	OnRep_Set_GravLineTrace(false);
}


void AMyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyCharacter::OnInteract);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::Jump_);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::Stop_Jumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyCharacter::OnResetVR);
}


void AMyCharacter::OnResetVR()
{
	// If Gravity is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in Gravity.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}


void AMyCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump_();
}

void AMyCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	Stop_Jumping();
}

void AMyCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::InitializeGravCurve()
{
	if (ChangeGravCurve)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		ChangeGravTimeline.AddInterpFloat(ChangeGravCurve, TimelineProgress);

		FOnTimelineEventStatic FinishTimeline;
		FinishTimeline.BindUFunction(this, FName("TimelineFinish"));
		ChangeGravTimeline.SetTimelineFinishedFunc(FinishTimeline);
	}
}
