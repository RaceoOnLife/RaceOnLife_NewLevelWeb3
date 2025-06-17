#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindScript.generated.h"

class UNiagaraSystem;

UCLASS()
class RACEONLIFE_API AWindScript : public AActor
{
    GENERATED_BODY()

public:
    AWindScript();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // --------- Wind Settings ---------
    UPROPERTY(EditAnywhere, Category = "Wind")
    FVector WindDirection = FVector(1, 0, 0);

    UPROPERTY(EditAnywhere, Category = "Wind")
    float WindStrength = 2000.0f;

    UPROPERTY(EditAnywhere, Category = "Wind")
    float WindRadius = 3000.0f;

    UPROPERTY(EditAnywhere, Category = "Wind")
    bool bAffectFoliage = true;

    // --------- Gust Settings ---------
    UPROPERTY(EditAnywhere, Category = "Wind|Gusts")
    float MinGustInterval = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Wind|Gusts")
    float MaxGustInterval = 15.0f;

    UPROPERTY(EditAnywhere, Category = "Wind|Gusts")
    float GustDuration = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Wind|Gusts")
    float MinGustStrength = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Wind|Gusts")
    float MaxGustStrength = 3.0f;

    // --------- Niagara ---------
    UPROPERTY(EditAnywhere, Category = "Wind|Niagara")
    UNiagaraSystem* WindGustSystem;

    UPROPERTY(EditAnywhere, Category = "Wind|Niagara")
    float GustEffectSpeed = 600.0f;

    UPROPERTY(EditAnywhere, Category = "Wind|Niagara")
    float GustEffectLifetime = 3.0f;

private:
    float CurrentGustStrength = 0.0f;
    FTimerHandle GustTimerHandle;

    void ScheduleNextGust();
    void StartGust();
    void EndGust();
    void ApplyWindForce();
    void SpawnMovingGustFX();
};
