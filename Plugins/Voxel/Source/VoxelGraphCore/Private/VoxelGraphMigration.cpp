// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphMigration.h"

FVoxelGraphMigration* GVoxelGraphMigration = MakeVoxelSingleton(FVoxelGraphMigration);

UFunction* FVoxelGraphMigration::FindNewFunction(const FName CachedName) const
{
	const FNodeMigration* Migration = NodeMigrations.Find(CachedName);
	if (!Migration ||
		!ensure(Migration->NewFunction))
	{
		LOG_VOXEL(Warning, "No redirector for '%s'", *CachedName.ToString());
		return nullptr;
	}

	LOG_VOXEL(Log, "Redirecting '%s' to %s", *CachedName.ToString(), *Migration->NewFunction->GetName());
	return Migration->NewFunction;
}

FName FVoxelGraphMigration::FindNewPinName(UObject* Outer, const FName OldName) const
{
	if (OldName.ToString().StartsWith("ORPHANED_"))
	{
		return {};
	}

	const FPinMigration* Migration = PinMigrations.Find({ Outer, OldName });
	if (!Migration)
	{
		LOG_VOXEL(Warning, "No redirector for %s.%s", *Outer->GetName(), *OldName.ToString());
		return {};
	}

	LOG_VOXEL(Log, "Redirecting %s.%s to %s",
		*Outer->GetName(),
		*OldName.ToString(),
		*Migration->NewName.ToString());
	return Migration->NewName;
}

void FVoxelGraphMigration::RegisterNodeMigration(const FNodeMigration& NodeMigration)
{
	ensure(!NodeMigrations.Contains(NodeMigration.DisplayName));
	NodeMigrations.Add(NodeMigration.DisplayName, NodeMigration);
}

void FVoxelGraphMigration::RegisterPinMigration(const FPinMigration& PinMigration)
{
	if (const UFunction* Function = Cast<UFunction>(PinMigration.Outer))
	{
		ensure(Function->FindPropertyByName(PinMigration.NewName));
	}

	const TPair<UObject*, FName> Key{ PinMigration.Outer, PinMigration.OldName };
	ensure(!PinMigrations.Contains(Key));
	PinMigrations.Add(Key, PinMigration);
}