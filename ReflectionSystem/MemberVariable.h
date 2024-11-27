#pragma once

#include <cstdint>
#include <string>
#include <functional>

#include "Replication/ReplicationObject.h"

#define mvOffsetOf(c, mv) ((size_t) & (static_cast<c*>(nullptr)->mv))

// TODO: Move out to Engine folder. Replace here with uint8_t.
enum class EPrimitiveType
{
	EPT_None,

	EPT_Int,
	EPT_String,
	EPT_Float,
	EPT_Bool,
	EPT_Vector2D,
	EPT_Array,
};

class MemberVariable
{
public:
	using ReplCallback = std::function<void(void* instance)>;

public:
	MemberVariable(const char* inName, EPrimitiveType inPrimitiveType, uint32_t inOffset, ReplicationType inReplType = ReplicationType::All)
		: mName(inName), mPrimitiveType(inPrimitiveType), mOffset(inOffset), mReplicationType(inReplType) {}

	MemberVariable(const MemberVariable& other)
	{
		mName = other.mName;
		mPrimitiveType = other.mPrimitiveType;
		mPrimitiveSubType = other.mPrimitiveSubType;
		mOffset = other.mOffset;
		mReplCallback = other.mReplCallback;
		mReplicationType = other.mReplicationType;
	}

	void SetPrimitiveSubType(EPrimitiveType inPrimitiveSubType)
	{
		mPrimitiveSubType = inPrimitiveSubType;
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
	EPrimitiveType GetPrimitiveSubType() const { return mPrimitiveSubType; }
	uint32_t GetOffset() const { return mOffset; }
	ReplicationType GetReplicationType() const { return mReplicationType; }

private:
	std::string mName;
	EPrimitiveType mPrimitiveType;
	EPrimitiveType mPrimitiveSubType = EPrimitiveType::EPT_None;

	uint32_t mOffset;

	ReplCallback mReplCallback = nullptr;
	ReplicationType mReplicationType = ReplicationType::All;
};
