// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/IndirectArray.h"
#include "Components/ActorComponent.h"
#include "SceneInterface.h"

/** Destroys render state for a component and then recreates it when this object is destroyed */
class FComponentRecreateRenderStateContext
{
private:
	/** Pointer to component we are recreating render state for */
	UActorComponent* Component;

	TSet<FSceneInterface*>* ScenesToUpdateAllPrimitiveSceneInfos;

public:
	FComponentRecreateRenderStateContext(UActorComponent* InComponent, TSet<FSceneInterface*>* InScenesToUpdateAllPrimitiveSceneInfos = nullptr)
		: ScenesToUpdateAllPrimitiveSceneInfos(InScenesToUpdateAllPrimitiveSceneInfos)
	{
		check(InComponent);
		checkf(!InComponent->IsUnreachable(), TEXT("%s"), *InComponent->GetFullName());

		if (InComponent->IsRegistered() && InComponent->IsRenderStateCreated())
		{
			InComponent->DestroyRenderState_Concurrent();
			Component = InComponent;

			UpdateAllPrimitiveSceneInfosForSingleComponent(InComponent, ScenesToUpdateAllPrimitiveSceneInfos);
		}
		else
		{
			Component = nullptr;
		}
	}

	~FComponentRecreateRenderStateContext()
	{
		if (Component && !Component->IsRenderStateCreated() && Component->IsRegistered())
		{
			Component->CreateRenderState_Concurrent();

			UpdateAllPrimitiveSceneInfosForSingleComponent(Component, ScenesToUpdateAllPrimitiveSceneInfos);
		}
	}
};

/** Destroys render states for all components and then recreates them when this object is destroyed */
class FGlobalComponentRecreateRenderStateContext
{
public:
	/** 
	* Initialization constructor. 
	*/
	ENGINE_API FGlobalComponentRecreateRenderStateContext();

	/** Destructor */
	ENGINE_API ~FGlobalComponentRecreateRenderStateContext();

private:
	/** The recreate contexts for the individual components. */
	TIndirectArray<FComponentRecreateRenderStateContext> ComponentContexts;

	TSet<FSceneInterface*> ScenesToUpdateAllPrimitiveSceneInfos;

	void UpdateAllPrimitiveSceneInfos();
};
