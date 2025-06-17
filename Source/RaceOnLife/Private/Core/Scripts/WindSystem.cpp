#include "Core/Scripts/WindScript.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "FoliageInstancedStaticMeshComponent.h"

AWindScript::AWindScript()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWindScript::BeginPlay()
{
    Super::BeginPlay();
    ScheduleNextGust();
}

void AWindScript::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    ApplyWindForce();
}

void AWindScript::ScheduleNextGust()
{
    float Interval = FMath::RandRange(MinGustInterval, MaxGustInterval);
    GetWorld()->GetTimerManager().SetTimer(GustTimerHandle, this, &AWindScript::StartGust, Interval, false);
}

void AWindScript::StartGust()
{
    CurrentGustStrength = FMath::RandRange(MinGustStrength, MaxGustStrength);
    SpawnMovingGustFX();

    GetWorld()->GetTimerManager().SetTimer(GustTimerHandle, this, &AWindScript::EndGust, GustDuration, false);
}

void AWindScript::EndGust()
{
    CurrentGustStrength = 0.0f;
    ScheduleNextGust();
}

void AWindScript::ApplyWindForce()
{
    if (CurrentGustStrength <= 0.0f) return;

    FVector Origin = GetActorLocation();
    FVector Direction = WindDirection.GetSafeNormal();
    float RadiusSqr = FMath::Square(WindRadius);

    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;
        if (!Actor || Actor == this) continue;

        FVector ToActor = Actor->GetActorLocation() - Origin;
        if (ToActor.SizeSquared() > RadiusSqr) continue;

        TArray<UPrimitiveComponent*> Components;
        Actor->GetComponents<UPrimitiveComponent>(Components);

        for (UPrimitiveComponent* Comp : Components)
        {
            if (Comp && Comp->IsSimulatingPhysics())
            {
                float Mass = Comp->GetMass();
                if (Mass > 200.0f) continue; // слишком тяжёлый объект

                float ForceStrength = WindStrength * CurrentGustStrength / FMath::Clamp(Mass, 0.1f, 200.0f);
                FVector Force = Direction * ForceStrength;
                Comp->AddForce(Force);
            }
        }
    }

    if (bAffectFoliage)
    {
        for (TObjectIterator<UFoliageInstancedStaticMeshComponent> It; It; ++It)
        {
            if (It->GetWorld() != GetWorld()) continue;

            for (int32 Index = 0; Index < It->GetInstanceCount(); ++Index)
            {
                FTransform InstanceTransform;
                if (It->GetInstanceTransform(Index, InstanceTransform, true))
                {
                    FVector Offset = Direction * FMath::Sin(GetWorld()->TimeSeconds * 2.0f) * 2.0f;
                    InstanceTransform.AddToTranslation(Offset);
                    It->UpdateInstanceTransform(Index, InstanceTransform, true, false, true);
                }
            }
        }
    }
}

void AWindScript::SpawnMovingGustFX()
{
    if (!WindGustSystem) return;

    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = WindDirection.Rotation();

    UNiagaraComponent* GustFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        WindGustSystem,
        SpawnLocation,
        SpawnRotation,
        FVector(1.0f),
        true,
        true,
        ENCPoolMethod::AutoRelease
    );

    if (GustFX)
    {
        GustFX->SetVectorParameter(FName("WindDirection"), WindDirection.GetSafeNormal());

        FTimerHandle MoveTimer;
        float ElapsedTime = 0.0f;

        FTimerDelegate MoveDelegate;
        MoveDelegate.BindLambda([=]() mutable {
            if (GustFX)
            {
                ElapsedTime += 0.05f;
                FVector NewLocation = GustFX->GetComponentLocation() + WindDirection.GetSafeNormal() * GustEffectSpeed * 0.05f;
                GustFX->SetWorldLocation(NewLocation);

                if (ElapsedTime >= GustEffectLifetime)
                {
                    GustFX->Deactivate();
                }
            }
        });

        GetWorld()->GetTimerManager().SetTimer(MoveTimer, MoveDelegate, 0.05f, true);
    }
}
