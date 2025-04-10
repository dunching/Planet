// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AHumanCharacter;

namespace TestCommand
{
	void AddPlayerCharacterTestDataImp(AHumanCharacter* CharacterPtr);

	void AddAICharacterTestDataImp(AHumanCharacter* CharacterPtr);

	void AddCahracterTestData();

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
	
	void ActiveBrandTest();

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
	
	void ActiveBrandTest1();

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
	
	void ActiveBrandTest2();

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
	
	void ChallengeTest();

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
};