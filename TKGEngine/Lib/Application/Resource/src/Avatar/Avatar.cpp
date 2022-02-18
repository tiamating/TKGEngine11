
#include "../../inc/Avatar.h"

#include "IResAvatar.h"


namespace TKGEngine
{
	// ==================================================
	// class methods
	// ==================================================
	Avatar& Avatar::operator=(const Avatar& ava)
	{
		m_res_avatar = ava.m_res_avatar;
		return *this;
	}

	void Avatar::RemoveUnused()
	{
		IResAvatar::RemoveUnused();
	}

	void Avatar::Load(const std::string& filename)
	{
		m_res_avatar = IResAvatar::Load(filename);
	}

	void Avatar::LoadAsync(const std::string& filename)
	{
		m_res_avatar = IResAvatar::LoadAsync(filename);
	}

	void Avatar::Release()
	{
		if (m_res_avatar != nullptr)
		{
			m_res_avatar->Release();
		}
		m_res_avatar.reset();
	}

	void Avatar::Reset()
	{
		m_res_avatar.reset();
	}

	int Avatar::GetBoneCount() const
	{
		return m_res_avatar == nullptr ? 0 : m_res_avatar->GetBoneCount();
	}

	int Avatar::GetBoneIndex(const std::string& bone_name)
	{
		return m_res_avatar == nullptr ? -1 : m_res_avatar->GetBoneIndex(bone_name);
	}

	int Avatar::GetEnabledBoneCount() const
	{
		return m_res_avatar == nullptr ? -1 : m_res_avatar->GetEnabledBoneCount();
	}

	int Avatar::GetRootCount() const
	{
		return m_res_avatar == nullptr ? 0 : m_res_avatar->GetRootCount();
	}

	const char* Avatar::GetRootName(int index)
	{
		return m_res_avatar == nullptr ? nullptr : m_res_avatar->GetRootName(index);
	}

	int Avatar::GetRootIndex(int index)
	{
		return m_res_avatar == nullptr ? -1 : m_res_avatar->GetRootIndex(index);
	}

	const Node* Avatar::GetNode(int index)
	{
		return m_res_avatar == nullptr ? nullptr : m_res_avatar->GetNode(index);
	}

	const Node* Avatar::GetNode(const std::string& bone_name)
	{
		return m_res_avatar == nullptr ? nullptr : m_res_avatar->GetNode(bone_name);
	}

	const std::unordered_map<std::string, int>* Avatar::GetNodeIndex() const
	{
		return m_res_avatar == nullptr ? nullptr : m_res_avatar->GetNodeIndex();
	}

	const std::vector<std::string>* Avatar::GetAlignedBoneNames() const
	{
		return m_res_avatar == nullptr ? nullptr : m_res_avatar->GetAlignedBoneNames();
	}

	bool Avatar::IsLoaded() const
	{
		return m_res_avatar != nullptr ? m_res_avatar->IsLoaded() : false;
	}

	bool Avatar::HasAvatar() const
	{
		return m_res_avatar == nullptr ? false : m_res_avatar->HasResource();
	}

	const char* Avatar::GetName() const
	{
		return m_res_avatar == nullptr ? nullptr : m_res_avatar->GetName();
	}

	const char* Avatar::GetFilePath() const
	{
		return m_res_avatar ? m_res_avatar->GetFilePath() : "";
	}

	unsigned Avatar::GetHash() const
	{
		return m_res_avatar == nullptr ? 0 : m_res_avatar->GetHash();
	}

}// namespace TKGEngine