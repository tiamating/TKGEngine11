#pragma once
#include "Systems/inc/TKGEngine_Defined.h"

#include "../src/AnimatorController/Animation_Defined.h"
#include "../src/AnimatorController/Animator_State.h"
#include "../src/AnimatorController/Animator_ControllerLayer.h"

#include <string>
#include <vector>

#include <memory>

namespace TKGEngine
{
	class IResAnimatorController;

	class AnimatorController
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AnimatorController() = default;
		virtual ~AnimatorController() = default;
		AnimatorController(const AnimatorController&) = delete;
		AnimatorController& operator=(const AnimatorController&);

#ifdef USE_IMGUI
		void OnGUI(const GameObjectID goid, const std::vector<std::string>* bone_names);
#endif // USE_IMGUI

		void Create();	// リソースマネージャーにセットされない作成
		void LoadAsync(const std::string& filename);
		void Load(const std::string& filename);

		void Release();

		std::shared_ptr<Animations::AnimatorState> AddMotion(const std::string& motion_filepath);

		int GetParameterCount() const;
		void AddParameter(const std::string& param_name, Animations::AnimatorControllerParameter::Type type);
		void RemoveParameter(const std::string& param_name);
		std::shared_ptr<Animations::AnimatorControllerParameter> GetParameter(const std::string& param_name);

		std::shared_ptr<Animations::AnimatorControllerLayer> AddLayer(const std::string& layer_name);
		void RemoveLayer(const std::string& layer_name);
		void RemoveLayer(const int index);
		std::shared_ptr<Animations::AnimatorControllerLayer> GetLayer(const std::string& layer_name);
		std::shared_ptr<Animations::AnimatorControllerLayer> GetLayer(const int index);

		void SetBool(const std::string& param_name, bool param);
		void SetInteger(const std::string& param_name, int param);
		void SetFloat(const std::string& param_name, float param);
		void SetTrigger(const std::string& param_name);

		bool GetBool(const std::string& param_name);
		int GetInteger(const std::string& param_name);
		float GetFloat(const std::string& param_name);
		void ResetTrigger(const std::string& param_name);

		bool IsLoaded() const;
		bool HasController() const;
		const char* GetName() const;
		void SetName(const std::string& name);

		unsigned GetHash() const;

		void ApplyAnimationTransform(
			const GameObjectID goid,
			const float elapsed_time,
			const std::unordered_map<std::string, int>* node_index,
			const std::vector<int>& enable_nodes,
			std::vector<Animations::KeyData>& data
		);

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
		std::shared_ptr<IResAnimatorController> m_res_controller = nullptr;
	};

}	// namespace TKGEngine