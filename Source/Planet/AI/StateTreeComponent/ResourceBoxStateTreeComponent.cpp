#include "ResourceBoxStateTreeComponent.h"

FName UResourceBoxStateTreeComponent::ComponentName = TEXT("NPCStateTreeComponent");

// TSubclassOf<UStateTreeSchema> UResourceBoxStateTreeComponent::GetSchema() const
// {
// 	return UStateTreeAIComponentSchema::StaticClass();
// }
//
// bool UResourceBoxStateTreeComponent::SetContextRequirements(FStateTreeExecutionContext& Context, bool bLogErrors)
// {
// 	Context.SetCollectExternalDataCallback(FOnCollectStateTreeExternalData::CreateUObject(this, &UStateTreeAIComponent::CollectExternalData));
// 	return UStateTreeAIComponentSchema::SetContextRequirements(*this, Context, bLogErrors);
// }
