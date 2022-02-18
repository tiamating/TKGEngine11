#pragma once

#include "../../inc/AssetDataBase.h"

#include "Animation_Defined.h"
#include "../ResourceManager.h"
#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine::Animations
{
	class AnimatorState;
	class AnimatorControllerLayer;
}

namespace TKGEngine
{
	/// <summary>
	/// Animator Controller Resource interface
	/// </summary>
	class IResAnimatorController
		: public AssetDataBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		IResAnimatorController(const IResAnimatorController&) = delete;
		IResAnimatorController& operator=(const IResAnimatorController&) = delete;

		IResAnimatorController() = default;
		virtual ~IResAnimatorController() = default;

		static std::shared_ptr<IResAnimatorController> Create();
		static std::shared_ptr<IResAnimatorController> Create(const std::string& controller_name);
		static std::shared_ptr<IResAnimatorController> LoadAsync(const std::string& filename);
		static std::shared_ptr<IResAnimatorController> Load(const std::string& filename);

#ifdef USE_IMGUI
		virtual void OnGUI(const GameObjectID goid, const std::vector<std::string>* bone_names) = 0;
#endif // USE_IMGUI
		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual std::shared_ptr<Animations::AnimatorState> AddMotion(const std::string& motion_filepath) = 0;

		virtual int GetParameterCount() const = 0;
		virtual void AddParameter(const std::string& param_name, Animations::AnimatorControllerParameter::Type type) = 0;
		virtual void RemoveParameter(const std::string& param_name) = 0;
		virtual std::shared_ptr<Animations::AnimatorControllerParameter> GetParameter(const std::string& param_name) = 0;

		virtual std::shared_ptr<Animations::AnimatorControllerLayer> AddLayer(const std::string& layer_name) = 0;
		virtual void RemoveLayer(const std::string& layer_name) = 0;
		virtual void RemoveLayer(const int index) = 0;
		virtual std::shared_ptr<Animations::AnimatorControllerLayer> GetLayer(const std::string& layer_name) = 0;
		virtual std::shared_ptr<Animations::AnimatorControllerLayer> GetLayer(const int index) = 0;

		virtual void SetBool(const std::string& param_name, bool param) = 0;
		virtual void SetInteger(const std::string& param_name, int param) = 0;
		virtual void SetFloat(const std::string& param_name, float param) = 0;
		virtual void SetTrigger(const std::string& param_name) = 0;

		virtual bool GetBool(const std::string& param_name) = 0;
		virtual int GetInteger(const std::string& param_name) = 0;
		virtual float GetFloat(const std::string& param_name) = 0;
		virtual void ResetTrigger(const std::string& param_name) = 0;

		virtual void ApplyAnimationTransform(
			GameObjectID goid,
			const float elapsed_time,
			const std::unordered_map<std::string, int>* node_index,
			const std::vector<int>& enable_nodes,
			std::vector<Animations::KeyData>& data
		) = 0;

		// ==============================================
		// public variables
		// ==============================================


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


		static std::shared_ptr<IResAnimatorController> CreateInterface();

		// ==============================================
		// private variables
		// ==============================================

	};

}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::IResAnimatorController);
CEREAL_CLASS_VERSION(TKGEngine::IResAnimatorController, 1);
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::AssetDataBase, TKGEngine::IResAnimatorController)