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
	
};