#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/TimelineComponent.h"
#include "Math/Vector.h"
#include "MyHUD.h"
#include "GameFramework/Actor.h"
#include "MyCharacter.generated.h"

class AParentButton;


UCLASS(config = Game)
class AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;


	UPROPERTY(VisibleDefaultsOnly, Category = Arrow)
	class UArrowComponent* ControllerDir;

	UPROPERTY(VisibleDefaultsOnly, Category = Arrow)
	UArrowComponent* InputDir;



	//For curve timeline
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)//for second variable
	UTimelineComponent* GraviTimelineComp;

	/*The initial location of the actor*/
	FVector ActorInitialLocation;

	/*The target location which is based on the max bounce height*/
	FVector TargetLocation;

	/*Curve float reference*/
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* CurveFloat;


	UPROPERTY(ReplicatedUsing = OnRep_Set_GravLineTrace)
	bool bReadyForGravState;

	UPROPERTY(ReplicatedUsing = OnRep_Set_CheckGrav)
	bool bCheckGrav;// For animation

	bool bGravit_line_trace;

	bool bIsUp;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
	void SetCurrentButton(AParentButton* NewCurrentButton);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AParentButton* GetCurrentButton() { return CurrentButton; }

protected:

	UPROPERTY(ReplicatedUsing = OnRep_Score)
	float Score;

	UFUNCTION(Client, Reliable)
	void PrintScore();

	void OnInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnInteract();

	bool jumping;

	float f_force;

	UPROPERTY()
	AParentButton* CurrentButton;

	//GravCurve

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curve)
	UCurveFloat* ChangeGravCurve;

	float CurveFloatValue;

	float TimelineValue;

	FTimeline ChangeGravTimeline;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void OnRep_Score();

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetF_Force() const { return f_force; }

	UFUNCTION(BlueprintCallable)
	void SetF_Force(float input);

	UFUNCTION(BlueprintCallable)
	void AddToScore(float add);

	UFUNCTION(BlueprintCallable)
	void OnRep_Set_CheckGrav(bool input);

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool Get_CheckGrav() const { return bCheckGrav; }

	UFUNCTION(BlueprintCallable)
	void OnRep_Set_GravLineTrace(bool input);

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool Get_GravLineTrace() const { return bReadyForGravState; }


	UFUNCTION(BlueprintCallable)
	void Up();

	UFUNCTION(BlueprintCallable)
	void Down();

	UFUNCTION(BlueprintCallable)
	void InitializeGravCurve();

	UFUNCTION(BlueprintCallable)
	void TimelineProgress();

	UFUNCTION(BlueprintCallable)
	void TimelineFinish();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable)
	FVector CalcInputDirForw();

	UFUNCTION(BlueprintCallable)
	FVector CalcInputDirRight();


	void ArrowRot();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Stop_Jumping();
	void Jump_();



	/** Resets HMD orientation in VR. */
	void OnResetVR();


	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

private:
	
	FOnTimelineFloat UpdateFunctionFloat;
};
