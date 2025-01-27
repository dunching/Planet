// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

namespace WeatherSystemCommand
{
	void AdjustHour(const TArray< FString >& Args);

	static FAutoConsoleCommand AdjustHourCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("AdjustHour"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(AdjustHour),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void AdjustWeather(const TArray< FString >& Args);

	static FAutoConsoleCommand AdjustWeatherCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("AdjustWeather"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(AdjustWeather),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);

}; 