// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AHumanCharacter;

namespace TestCommand
{
	void CopyID2RowName();

	static FAutoConsoleCommand CopyID2RowNameCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("CopyID2RowName"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(CopyID2RowName),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void TestPlayerCharacterMoveTo(const TArray< FString >& Args);

	static FAutoConsoleCommand TestPlayerCharacterMoveToCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestPlayerCharacterMoveTo"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(TestPlayerCharacterMoveTo),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void TestCameraManager(const TArray< FString >& Args);

	static FAutoConsoleCommand TestCameraManagerCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestCameraManager"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(TestCameraManager),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void TestSectorCollision();

	static FAutoConsoleCommand TestSectorCollisionCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestSectorCollision"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(TestSectorCollision),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void TestAsyncAssetLoad();

	static FAutoConsoleCommand CMDTestAsyncAssetLoadt(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestAsyncAssetLoad"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(TestAsyncAssetLoad),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);

	void TestContainer();

	static FAutoConsoleCommand TestContainerCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestContainer"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(TestContainer),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);

	void TestSpline();

	static FAutoConsoleCommand CMDTestSpline(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestSpline"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(TestSpline),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);

	void TestDisplacementSkill();

	static FAutoConsoleCommand CMDTestDisplacementSkill(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestDisplacementSkill"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(TestDisplacementSkill),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);

	void TestCooldown();

	static FAutoConsoleCommand CMDTestCooldown(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestCooldown"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(TestCooldown),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);

	void TestGAEventModify();

	static FAutoConsoleCommand TestGAEventModifyCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestGAEventModify"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(TestGAEventModify),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);

	void SpawnHumanCharacter(const TArray< FString >& Args);

	static FAutoConsoleCommand SpawnHumanCharacterCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SpawnHumanCharacter"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(SpawnHumanCharacter),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void SpawnHorseCharacter(const TArray< FString >& Args);

	static FAutoConsoleCommand SpawnHorseCharacterCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SpawnHorseCharacter"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(SpawnHorseCharacter),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);

	void RecruitCharacter();

	static FAutoConsoleCommand RecruitCharacterCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("RecruitCharacter"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(RecruitCharacter),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void ModifyWuXingProperty(const TArray< FString >& Args);

	static FAutoConsoleCommand ModifyWuXingPropertyCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ModifyWuXingProperty"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(ModifyWuXingProperty),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void TestGAState2Self(const TArray< FString >& Args);

	static FAutoConsoleCommand TestGAState2SelfCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestGAState2Self"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(TestGAState2Self),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void TestGATagState2Target(const TArray< FString >& Args);

	static FAutoConsoleCommand TestGATagState2TargetCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestGATagState2Target"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(TestGATagState2Target),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);

};