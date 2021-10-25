// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
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
	GetCharacterMovement()->AirControl = 0.2f;//0.2f стояло до, попробовать!!!!!!!!!!!!!!!!!можно удалить комент
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

	ReadyForGravState = false;
	Gravit_line_trace = false;
	check_grav = false;

	//параметр влияющий на притяжение персонажа
	f_force = 0;

	jumping = false;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle ExplosionTimer;
	
	Set_ReadyPress(false);

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ArrowRot();

	if (check_grav)
	{
		FVector force(0.0f, 0.0f, f_force);

		GetCharacterMovement()->AddForce(force);
	}
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



void AMyCharacter::ControlGrav(float outvalue)
{
	FQuat NewRotate = FQuat(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, outvalue));
	SetActorRotation(NewRotate);
	
	//f_force = 11.0f * -25000.0f;
	f_force = 337000.0f;
	
	
	//rotate camera with person
	/*FollowCamera->GetComponentRotation();
	FQuat NewCameraRotation = FQuat(FRotator(FollowCamera->GetComponentRotation().Pitch, FollowCamera->GetComponentRotation().Yaw, outvalue));
	
	FollowCamera->SetWorldRotation(NewCameraRotation);*/
}



void AMyCharacter::ArrowRot()
{
	float newZ = GetMesh()->GetAnimInstance()->CalculateDirection(GetVelocity(), GetActorRotation());
	InputDir->SetRelativeRotation(FRotator(0.0f, newZ, 0.0f));
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
		if (check_grav)
			AddMovementInput(Direction, -Value);
		else
			AddMovementInput(Direction, Value);
		
	}
}

void AMyCharacter::Jump_()
{
	if (check_grav)
	{
		LaunchCharacter(FVector(0.0f, 0.0f, -600.0f), false, false);
	}
	else
	{
		ACharacter::Jump();
	}

	Set_GravLineTrace(true);
}


void AMyCharacter::Stop_Jumping()
{
	ACharacter::StopJumping();
	Set_GravLineTrace(false);
}


void AMyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
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

void AMyCharacter::Set_CheckGrav(bool input) 
{
	check_grav = input;
}

bool AMyCharacter::Get_CheckGrav()
{
	return check_grav;
}

void AMyCharacter::Set_GravLineTrace(bool input)
{
	ReadyForGravState = input;
}

bool AMyCharacter::Get_GravLineTrace()
{
	return ReadyForGravState;
}

void AMyCharacter::Set_ReadyPress(bool input)
{
	ReadyPress = input;
}

bool AMyCharacter::Get_ReadyPress()
{
	return ReadyPress;
}

void AMyCharacter::Set_WasPress(bool input)
{
	WasPress = input;
}

bool AMyCharacter::Get_WasPress()
{
	return WasPress;
}

void AMyCharacter::Up()
{
	Set_WasPress(true);
	Set_CheckGrav(true);
	Set_GravLineTrace(true);
}

void AMyCharacter::Down()
{
	Set_WasPress(true);
	Set_CheckGrav(false);
}


