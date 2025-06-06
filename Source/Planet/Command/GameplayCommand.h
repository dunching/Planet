// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AHumanCharacter;

namespace GameplayCommand
{
	void ActiveGuideMainThread();

	static FAutoConsoleCommand ActiveGuideMainThreadCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ActiveGuideMainThread"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(ActiveGuideMainThread),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void ViewAllocationMenu();

	static FAutoConsoleCommand ViewAllocationMenuCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ViewAllocationMenu"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(ViewAllocationMenu),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void EntryActionProcessor();

	static FAutoConsoleCommand EntryActionProcessorCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("EntryActionProcessor"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(EntryActionProcessor),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void ViewGroupmateMenu();

	static FAutoConsoleCommand ViewGroupmateMenuCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ViewGroupmateMenu"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(ViewGroupmateMenu),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void ViewSetting();

	static FAutoConsoleCommand ViewSettingCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ViewSetting"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(ViewSetting),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void EntryChallengeLevel(const TArray< FString >& Args);

	static FAutoConsoleCommand EntryChallengeLevelCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("EntryChallengeLevel"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(EntryChallengeLevel),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void SwitchTeammateOptionToFollow();

	static FAutoConsoleCommand SwitchTeammateOptionToFollowCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SwitchTeammateOptionToFollow"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(SwitchTeammateOptionToFollow),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void SwitchTeammateOptionToAssistance();

	static FAutoConsoleCommand SwitchTeammateOptionToAssistanceCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SwitchTeammateOptionToAssistance"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(SwitchTeammateOptionToAssistance),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void SwitchTeammateOptionToFireTarget();

	static FAutoConsoleCommand SwitchTeammateOptionToFireTargetCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SwitchTeammateOptionToFireTarget"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(SwitchTeammateOptionToFireTarget),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void RespawnPlayerCharacter();

	static FAutoConsoleCommand RespawnPlayerCharacterCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("RespawnPlayerCharacter"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(RespawnPlayerCharacter),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void ServerSpawnCharacterByProxyType(const TArray< FString >& Args);

	static FAutoConsoleCommand ServerSpawnCharacterByProxyTypeCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ServerSpawnCharacterByProxyType"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(ServerSpawnCharacterByProxyType),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
};