
#include "../../inc/AnimatorController.h"

#include "IResAnimatorController.h"

namespace TKGEngine
{
	// ==================================================
	// class methods
	// ==================================================
#ifdef USE_IMGUI
	void AnimatorController::OnGUI(const GameObjectID goid, const std::vector<std::string>* bone_names)
	{
		if (m_res_controller)
			m_res_controller->OnGUI(goid, bone_names);
	}
#endif // USE_IMGUI

	AnimatorController& AnimatorController::operator=(const AnimatorController& anim_controller)
	{
		m_res_controller = anim_controller.m_res_controller;
		return *this;
	}

	void AnimatorController::Create()
	{
		m_res_controller = IResAnimatorController::Create();
	}

	void AnimatorController::LoadAsync(const std::string& filename)
	{
		m_res_controller = IResAnimatorController::LoadAsync(filename);
	}

	void AnimatorController::Load(const std::string& filename)
	{
		m_res_controller = IResAnimatorController::Load(filename);
	}

	void AnimatorController::Release()
	{
		if (m_res_controller)
		{
			m_res_controller->Release();
		}
		m_res_controller.reset();
	}

	std::shared_ptr<Animations::AnimatorState> AnimatorController::AddMotion(const std::string& motion_filepath)
	{
		return m_res_controller == nullptr ? nullptr : m_res_controller->AddMotion(motion_filepath);
	}

	int AnimatorController::GetParameterCount() const
	{
		return m_res_controller == nullptr ? -1 : m_res_controller->GetParameterCount();
	}

	void AnimatorController::AddParameter(const std::string& param_name, Animations::AnimatorControllerParameter::Type type)
	{
		if (m_res_controller)
			m_res_controller->AddParameter(param_name, type);
	}

	void AnimatorController::RemoveParameter(const std::string& param_name)
	{
		if (m_res_controller)
			m_res_controller->RemoveParameter(param_name);
	}

	std::shared_ptr<Animations::AnimatorControllerParameter> AnimatorController::GetParameter(const std::string& param_name)
	{
		return m_res_controller == nullptr ? nullptr : m_res_controller->GetParameter(param_name);
	}

	std::shared_ptr<Animations::AnimatorControllerLayer> AnimatorController::AddLayer(const std::string& layer_name)
	{
		return m_res_controller == nullptr ? nullptr : m_res_controller->AddLayer(layer_name);
	}

	void AnimatorController::RemoveLayer(const std::string& layer_name)
	{
		if (m_res_controller)
			m_res_controller->RemoveLayer(layer_name);
	}

	void AnimatorController::RemoveLayer(const int index)
	{
		if (m_res_controller)
			m_res_controller->RemoveLayer(index);
	}

	std::shared_ptr<Animations::AnimatorControllerLayer> AnimatorController::GetLayer(const std::string& layer_name)
	{
		return m_res_controller == nullptr ? nullptr : m_res_controller->GetLayer(layer_name);
	}

	std::shared_ptr<Animations::AnimatorControllerLayer> AnimatorController::GetLayer(const int index)
	{
		return m_res_controller == nullptr ? nullptr : m_res_controller->GetLayer(index);
	}

	void AnimatorController::SetBool(const std::string& param_name, bool param)
	{
		if (m_res_controller)
			m_res_controller->SetBool(param_name, param);
	}

	void AnimatorController::SetInteger(const std::string& param_name, int param)
	{
		if (m_res_controller)
			m_res_controller->SetInteger(param_name, param);
	}

	void AnimatorController::SetFloat(const std::string& param_name, float param)
	{
		if (m_res_controller)
			m_res_controller->SetFloat(param_name, param);
	}

	void AnimatorController::SetTrigger(const std::string& param_name)
	{
		if (m_res_controller)
			m_res_controller->SetTrigger(param_name);
	}

	bool AnimatorController::GetBool(const std::string& param_name)
	{
		return m_res_controller == nullptr ? false : m_res_controller->GetBool(param_name);
	}

	int AnimatorController::GetInteger(const std::string& param_name)
	{
		return m_res_controller == nullptr ? 0 : m_res_controller->GetInteger(param_name);
	}

	float AnimatorController::GetFloat(const std::string& param_name)
	{
		return m_res_controller == nullptr ? 0.f : m_res_controller->GetFloat(param_name);
	}

	void AnimatorController::ResetTrigger(const std::string& param_name)
	{
		if (m_res_controller)
			m_res_controller->ResetTrigger(param_name);
	}

	bool AnimatorController::IsLoaded() const
	{
		return m_res_controller != nullptr ? m_res_controller->IsLoaded() : false;
	}

	bool AnimatorController::HasController() const
	{
		if (!m_res_controller)
			return false;

		return m_res_controller->HasResource();
	}

	const char* AnimatorController::GetName() const
	{
		return m_res_controller == nullptr ? nullptr : m_res_controller->GetName();
	}

	void AnimatorController::SetName(const std::string& name)
	{
		if (m_res_controller)
			m_res_controller->SetName(name);
	}

	unsigned AnimatorController::GetHash() const
	{
		return m_res_controller == nullptr ? 0 : m_res_controller->GetHash();
	}

	void AnimatorController::ApplyAnimationTransform(
		const GameObjectID goid,
		const float elapsed_time,
		const std::unordered_map<std::string, int>* node_index,
		const std::vector<int>& enable_nodes,
		std::vector<Animations::KeyData>& data
	)
	{
		// ‘ŠúƒŠƒ^[ƒ“
		if (!node_index)
			return;

		if (m_res_controller)
			m_res_controller->ApplyAnimationTransform(goid, elapsed_time, node_index, enable_nodes, data);
	}

}// namespace TKGEngine