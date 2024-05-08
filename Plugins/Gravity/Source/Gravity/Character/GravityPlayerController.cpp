
#include "GravityPlayerController.h"

#include <Engine/Engine.h>
#include <GameFramework/Character.h>
#include <IXRTrackingSystem.h>
#include <IXRCamera.h>
#include <Kismet/KismetMathLibrary.h>

#include "GravityCharacter.h"
#include "GravityMovementComponent.h"

void AGravityPlayerController::UpdateRotation(float DeltaTime)
{
    APawn* const P = GetPawnOrSpectator();
    if (P)
    {
        FRotator ViewRotation = ControlRotationWithoutGravityTrans;

        // Calculate Delta to be applied on ViewRotation
        FRotator DeltaRot(RotationInput);

        if (PlayerCameraManager)
        {
            PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
        }

        AActor* ViewTarget = GetViewTarget();
        if (!PlayerCameraManager || !ViewTarget || !ViewTarget->HasActiveCameraComponent() || ViewTarget->HasActivePawnControlCameraComponent())
        {
            if (IsLocalPlayerController() && GEngine->XRSystem.IsValid() && GetWorld() != nullptr && GEngine->XRSystem->IsHeadTrackingAllowedForWorld(*GetWorld()))
            {
                auto XRCamera = GEngine->XRSystem->GetXRCamera();
                if (XRCamera.IsValid())
                {
                    XRCamera->ApplyHMDRotation(this, ViewRotation);
                }
            }
        }

        ControlRotationWithoutGravityTrans = ViewRotation;

        ViewRotation = (P->GetGravityTransform() * ViewRotation.Quaternion()).Rotator();

        SetControlRotation(ViewRotation);

#if WITH_EDITOR
        RootComponent->SetWorldLocation(P->GetActorLocation());
#endif

        P->FaceRotation(ViewRotation, DeltaTime);
    }
}
