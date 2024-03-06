#pragma once

#include <cstdint>
#include <string>
#include <functional>

#define mvOffsetOf(c, mv) ((size_t) & (static_cast<c*>(nullptr)->mv))

enum class EPrimitiveType
{
	EPT_Int,
	EPT_String,
	EPT_Float,
	EPT_Bool,
};

class MemberVariable
{
public:
	using ReplCallback = std::function<void(void* instance)>;

public:
	MemberVariable(const char* inName, EPrimitiveType inPrimitiveType, uint32_t inOffset)
		: mName(inName), mPrimitiveType(inPrimitiveType), mOffset(inOffset) {}

	MemberVariable(const MemberVariable& other)
	{
		mName = other.mName;
		mPrimitiveType = other.mPrimitiveType;
		mOffset = other.mOffset;
	}

	template <typename T>
	void SetReplicationCallback(void (T::* method)())
	{
		mReplCallback = ([=](void* instance) {
			(reinterpret_cast<T*>(instance)->*method)();
		});
	}

	void DoReplicatedCallback(IReplicationObject* owner) const
	{
		if (mReplCallback)
		{
			mReplCallback(owner);
		}
	}

	EPrimitiveType GetPrimitiveType() const { return mPrimitiveType; }
	uint32_t GetOffset() const { return mOffset; }

private:
	std::string mName;
	EPrimitiveType mPrimitiveType;
	uint32_t mOffset;

	ReplCallback mReplCallback = nullptr;
};
