// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility)

UFPGameplayAbility::UFPGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	
	ActivationPolicy = EAbilityActivationPolicy::Manual;
}
