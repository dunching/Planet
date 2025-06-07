// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AHumanCharacter;

namespace TestCommand
{
	PLANET_API void AddPlayerCharacterTestDataImp(AHumanCharacter* CharacterPtr);

	PLANET_API void AddAICharacterTestDataImp(AHumanCharacter* CharacterPtr);

	PLANET_API void AddCahracterTestData();

	static FAutoConsoleCommand AddCahracterTestData111CMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("AddCahracterTestData"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(AddCahracterTestData),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void ActiveBrandTest();

	static FAutoConsoleCommand ActiveBrandTestCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ActiveBrandTest"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(ActiveBrandTest),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void ActiveBrandTest1();

	static FAutoConsoleCommand ActiveBrandTest1CMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ActiveBrandTest1"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(ActiveBrandTest1),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void ActiveBrandTest2();

	static FAutoConsoleCommand ActiveBrandTest2CMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ActiveBrandTest2"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(ActiveBrandTest2),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void ChallengeTest();

	static FAutoConsoleCommand ChallengeTestCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ChallengeTest"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(ChallengeTest),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void TestHasBeenFlyAway(const TArray< FString >& Args);

	static FAutoConsoleCommand TestHasBeenFlyAwayCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestHasBeenFlyAway"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(TestHasBeenFlyAway),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void ReplyHP(const TArray< FString >& Args);

	static FAutoConsoleCommand ReplyHPCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ReplyHP"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(ReplyHP),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void IncreaseCD(const TArray< FString >& Args);

	static FAutoConsoleCommand IncreaseCDCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("IncreaseCD"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(IncreaseCD),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void AddProxy(const TArray< FString >& Args);

	static FAutoConsoleCommand AddProxyCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("AddProxy"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(AddProxy),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void RemoveProxy(const TArray< FString >& Args);

	static FAutoConsoleCommand RemoveProxyCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("RemoveProxy"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(RemoveProxy),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void SwitchPlayerInput(const TArray< FString >& Args);

	static FAutoConsoleCommand SwitchPlayerInputCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SwitchPlayerInput"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(SwitchPlayerInput),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void AddOrRemoveState(const TArray< FString >& Args);

	static FAutoConsoleCommand AddOrRemoveStateCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("AddOrRemoveState"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(AddOrRemoveState),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void SetBGMVolume(const TArray< FString >& Args);

	static FAutoConsoleCommand SetBGMVolumeCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SetBGMVolume"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(SetBGMVolume),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void AddExperience(const TArray< FString >& Args);

	static FAutoConsoleCommand AddExperienceCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("AddExperience"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(AddExperience),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	PLANET_API void SetCharacterAttributeValue(const TArray< FString >& Args);

	static FAutoConsoleCommand SetCharacterAttributeValueCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SetCharacterAttributeValue"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(SetCharacterAttributeValue),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
};