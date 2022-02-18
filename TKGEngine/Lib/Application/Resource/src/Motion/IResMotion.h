#pragma once

#include "Application/Resource/inc/AssetDataBase.h"
#include "Application/Resource/src/AnimatorController/Animation_Defined.h"

#include "../ResourceManager.h"
#include "Utility/inc/myfunc_vector.h"
#include "Systems/inc/TKGEngine_Defined.h"

#include <string>
#include <vector>
#include <unordered_map>

#include <memory>
#include <mutex>

#ifdef USE_IMGUI
#include <fbxsdk.h>
#endif// USE_IMGUI

namespace TKGEngine
{
	/// <summary>
	/// Motion Resource interface
	/// </summary>
	class IResMotion
		: public AssetDataBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		IResMotion() = default;
		virtual ~IResMotion() = default;
		IResMotion(const IResMotion&) = delete;
		IResMotion& operator=(const IResMotion&) = delete;

		static std::shared_ptr<IResMotion> Load(const std::string& filename);
		static std::shared_ptr<IResMotion> LoadAsync(const std::string& filename);
#ifdef USE_IMGUI
		static void CreateBinaryFromFBX(
			const std::vector<std::string>& filepathes,
			FbxScene* fbx_scene,
			std::vector<std::string>& motion_names,
			std::unordered_map<std::string, int>& bone_name_index
		);
#endif// USE_IMGUI
		static void RemoveUnused();

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual float GetSampleRate() const = 0;
		virtual float GetMotionLength() const = 0;

		virtual std::vector<Animations::KeyData>* GetKeyFrame(const int frame) = 0;
		virtual std::unordered_map<std::string, int>* GetKeyIndex() = 0;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<AssetDataBase>(this)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		static std::shared_ptr<IResMotion> CreateInterface();

		// ==============================================
		// private variables
		// ==============================================
		static ResourceManager<IResMotion> m_caches;
		static std::mutex m_cache_mutex;
	};

}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::IResMotion)
CEREAL_CLASS_VERSION(TKGEngine::IResMotion, 1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::AssetDataBase, TKGEngine::IResMotion)
