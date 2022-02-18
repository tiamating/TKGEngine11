#pragma once
#include "Systems/inc/TKGEngine_Defined.h"

#include "Application/Resource/inc/Asset_Defined.h"
#include "Application/Resource/inc/Avatar_Defined.h"

#include <string>

#include <memory>


namespace TKGEngine
{
	class IResAvatar;

	class Avatar
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Avatar(const Avatar&) = delete;

		Avatar() = default;
		virtual ~Avatar() = default;
		Avatar& operator=(const Avatar&);

		static void RemoveUnused();

		void Load(const std::string& filename);
		void LoadAsync(const std::string& filename);

		void Release();
		void Reset();

		int GetBoneCount() const;
		int GetBoneIndex(const std::string& bone_name);
		int GetEnabledBoneCount() const;
		int GetRootCount() const;
		const char* GetRootName(int index);
		int GetRootIndex(int index);
		const Node* GetNode(int index);
		const Node* GetNode(const std::string& bone_name);
		const std::unordered_map<std::string, int>* GetNodeIndex() const;
		const std::vector<std::string>* GetAlignedBoneNames() const;

		bool IsLoaded() const;
		bool HasAvatar() const;
		const char* GetName() const;
		const char* GetFilePath() const;

		unsigned GetHash() const;

		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================


		// ==============================================
		// private variables
		// ==============================================
		std::shared_ptr<IResAvatar> m_res_avatar = nullptr;
	};


}	// namespace TKGEngine