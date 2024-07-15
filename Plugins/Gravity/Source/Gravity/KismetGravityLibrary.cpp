
#include "KismetGravityLibrary.h"

FVector UKismetGravityLibrary::GetGravity(const FVector& Location)
{
#if USECUSTOMEGRAVITY
    return -Location.GetSafeNormal();
#else
    return FVector::DownVector;
#endif
}
