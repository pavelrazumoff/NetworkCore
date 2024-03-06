#pragma once

#include "MemberVariable.h"
#include <vector>

class DataType
{
public:
	DataType() {}
	DataType(const std::initializer_list<MemberVariable>& inMVs)
		: mMemberVariables(inMVs) {}

	MemberVariable& AddMemberVariable(const MemberVariable& mv)
	{
		mMemberVariables.push_back(mv);
		return mMemberVariables.back();
	}

	const std::vector<MemberVariable>& GetMemberVariables() const { return mMemberVariables; }

	MemberVariable* GetMemberByOffset(uint32_t offset)
	{
		for (size_t i = 0; i < mMemberVariables.size(); ++i)
		{
			if (mMemberVariables[i].GetOffset() == offset)
			{
				return &mMemberVariables[i];
			}
		}
		return nullptr;
	}

	int GetMemberIndexByOffset(uint32_t offset) const
	{
		for (int i = 0, c = (int)mMemberVariables.size(); i < c; ++i)
		{
			if (mMemberVariables[i].GetOffset() == offset)
			{
				return i;
			}
		}
		return -1;
	}

private:
	std::vector<MemberVariable> mMemberVariables;
};
