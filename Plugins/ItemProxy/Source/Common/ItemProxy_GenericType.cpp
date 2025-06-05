
#include "ItemProxy_GenericType.h"

inline FPerLevelValue_Float::FPerLevelValue_Float()
{
}

inline FPerLevelValue_Float::FPerLevelValue_Float(
	std::initializer_list<float> InitList
	):
	 PerLevelValue(InitList)
{
}

FPerLevelValue_Int::FPerLevelValue_Int()
{
}

FPerLevelValue_Int::FPerLevelValue_Int(
	std::initializer_list<int32> InitList
	):
	 PerLevelValue(InitList)
{
}
