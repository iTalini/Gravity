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



	bool ReadyForGravState;

	bool check_grav;// Для анимации
	bool Gravit_line_trace;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;


protected:
	UPROPERTY(EditAnywhere, Category = "ButtonPress")
		bool ReadyPress;

	UPROPERTY(EditAnywhere, Category = "ButtonPress")
		bool WasPress;

	UPROPERTY(ReplicatedUsing = OnRep_Score)
		float Score;

	UFUNCTION()
		virtual void OnRep_Score();

	UFUNCTION(Client, Reliable)
	void PrintScore();

	bool jumping;

	float f_force;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
		void ControlGrav(float outvalue);

	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetF_Force() const { return f_force; }

	UFUNCTION(BlueprintCallable)
		void SetF_Force(float input);

	UFUNCTION(BlueprintCallable)
		void AddToScore(float add);

	UFUNCTION(BlueprintCallable)
		void Set_CheckGrav(bool input);

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool Get_CheckGrav() const { return check_grav; }

	UFUNCTION(BlueprintCallable)
		void Set_GravLineTrace(bool input);

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool Get_GravLineTrace() const { return ReadyForGravState; }

	UFUNCTION(BlueprintCallable)
		void Set_ReadyPress(bool input);
	
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool Get_ReadyPress() const { return ReadyPress; }

	UFUNCTION(BlueprintCallable)
		void Set_WasPress(bool input);

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool Get_WasPress() const { return WasPress; }

	UFUNCTION(BlueprintCallable)
		void Up();

	UFUNCTION(BlueprintCallable)
		void Down();

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
