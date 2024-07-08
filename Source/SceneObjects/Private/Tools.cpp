
#include "Tools.h"

UWorld* GetWorldImp()
{
	#if WITH_EDITOR
		return GEditor->GetEditorWorldContext().World();
	#else 
		return GEngine->GetWorld();
	#endif
}
