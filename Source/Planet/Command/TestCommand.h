// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AHumanCharacter;

namespace TestCommand
{
	void AddPlayerCharacterTestDataImp(AHumanCharacter* CharacterPtr);

	void AddAICharacterTestDataImp(AHumanCharacter* CharacterPtr);

	void AddCahracterTestData();

	static FAutoConsoleCommand CMDAddCahracterTestData(
		//CMD ���֣��ڿ���̨�����������
		TEXT("AddCahracterTestData"),
		//����̨������Ϣ��ѡ����������ʱ��ῴ��
		TEXT("this is a CMD test."),
		//������̬ί�У������������������õ�����ĺ���
		FConsoleCommandDelegate::CreateStatic(AddCahracterTestData),
		//��ѡ��־λ����
		EConsoleVariableFlags::ECVF_Default
	);
};