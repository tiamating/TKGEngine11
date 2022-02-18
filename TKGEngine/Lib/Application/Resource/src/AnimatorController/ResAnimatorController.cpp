
#include "IResAnimatorController.h"

#include "Animator_State.h"
#include "Animator_ControllerLayer.h"

#include "Systems/inc/LogSystem.h"
#include "Systems/inc/AssetSystem.h"
#include "Utility/inc/myfunc_file.h"

#include <list>
#include <unordered_set>
#include <unordered_map>

#include <Windows.h>
#include <cassert>

namespace TKGEngine
{
	class ResAnimatorController
		: public IResAnimatorController
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResAnimatorController(const ResAnimatorController&) = delete;
		ResAnimatorController& operator=(const ResAnimatorController&) = delete;

		ResAnimatorController();
		virtual ~ResAnimatorController() = default;

		std::shared_ptr<Animations::AnimatorState> AddMotion(const std::string& motion_filepath) override;

		int GetParameterCount() const override;
		void AddParameter(const std::string& param_name, Animations::AnimatorControllerParameter::Type type) override;
		void RemoveParameter(const std::string& param_name) override;
		std::shared_ptr<Animations::AnimatorControllerParameter> GetParameter(const std::string& param_name) override;

		std::shared_ptr<Animations::AnimatorControllerLayer> AddLayer(const std::string& layer_name) override;
		void RemoveLayer(const std::string& layer_name) override;
		void RemoveLayer(const int index) override;
		std::shared_ptr<Animations::AnimatorControllerLayer> GetLayer(const std::string& layer_name) override;
		std::shared_ptr<Animations::AnimatorControllerLayer> GetLayer(const int index) override;

		void SetBool(const std::string& param_name, bool param) override;
		void SetInteger(const std::string& param_name, int param) override;
		void SetFloat(const std::string& param_name, float param) override;
		void SetTrigger(const std::string& param_name) override;

		bool GetBool(const std::string& param_name) override;
		int GetInteger(const std::string& param_name) override;
		float GetFloat(const std::string& param_name) override;
		void ResetTrigger(const std::string& param_name) override;

		void ApplyAnimationTransform(
			const GameObjectID goid,
			const float elapsed_time,
			const std::unordered_map<std::string, int>* node_index,
			const std::vector<int>& enable_nodes,
			std::vector<Animations::KeyData>& data
		) override;

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
					cereal::base_class<IResAnimatorController>(this),
					CEREAL_NVP(m_layers),
					CEREAL_NVP(m_parameters)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override {}
		// �Ăяo�����֐�
		void OnGUI(const GameObjectID goid, const std::vector<std::string>* bone_names) override;
		// �p�����[�^�ݒ�p
		void OnGUIParameter();
		// ���C���[�I��p
		void OnGUILayerSelect();
		// ���C���[���X�e�[�g�}�V���\���p
		void OnGUILayerStateMachine(const GameObjectID goid);
		// ���C���[���}�X�N�ݒ�p
		void OnGUILayerMask(const std::vector<std::string>* bone_names);
#endif // USE_IMGUI

		void SetAsyncOnLoad() override;
		void OnLoad() override;

		// use GUI
		bool Save();
		// ~use GUI


		// ==============================================
		// private variables
		// ==============================================
#ifdef USE_IMGUI
		int m_selecting_layer = 0;
#endif // USE_IMGUI

		std::vector<std::shared_ptr<Animations::AnimatorControllerLayer>> m_layers;
		std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>> m_parameters;

	};


	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////

	static constexpr const char* DEFAULT_NAME = "AnimController";


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	// IResAnimatorController
	std::shared_ptr<IResAnimatorController> IResAnimatorController::CreateInterface()
	{
		return std::static_pointer_cast<IResAnimatorController>(std::make_shared<ResAnimatorController>());
	}

	std::shared_ptr<IResAnimatorController> IResAnimatorController::Create()
	{
		static int idx = 0;
		return Create(std::string(DEFAULT_NAME + std::to_string(idx++)));
	}

	std::shared_ptr<IResAnimatorController> IResAnimatorController::Create(const std::string& controller_name)
	{
		std::shared_ptr<IResAnimatorController> s_ptr = CreateInterface();
		s_ptr->m_has_resource = true;
		s_ptr->SetName(controller_name);

		return s_ptr;
	}

	std::shared_ptr<IResAnimatorController> IResAnimatorController::LoadAsync(const std::string& filename)
	{
		// Create new Controller
		std::shared_ptr<IResAnimatorController> res_new(CreateInterface());
		res_new->SetFilePath(filename);

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResAnimatorController::SetAsyncOnLoad, res_new));

		return res_new;
	}

	std::shared_ptr<IResAnimatorController> IResAnimatorController::Load(const std::string& filename)
	{
		// Create new Controller
		std::shared_ptr<IResAnimatorController> res_new(CreateInterface());
		res_new->SetFilePath(filename);

		// Load Resource
		res_new->m_load_state_mutex.lock();
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnLoad();
		if (!res_new->HasResource())
		{
			res_new->Release();
			res_new->m_load_state_mutex.unlock();
			res_new.reset();
			return std::shared_ptr<IResAnimatorController>();
		}
		res_new->m_load_state_mutex.unlock();
		return res_new;
	}

	void IResAnimatorController::Release()
	{
		/* nothing */
	}
	// ~IResAnimatorController

	// ResAnimatorController
	ResAnimatorController::ResAnimatorController()
	{
		// ���炩����root���C���[���쐬����
		m_layers.emplace_back(std::make_shared<Animations::AnimatorControllerLayer>());
		m_layers.back()->SetName("Root");
	}

#ifdef USE_IMGUI
	void ResAnimatorController::OnGUI(const GameObjectID goid, const std::vector<std::string>* bone_names)
	{
		ImGui::IDWrapped id(this);

		// Menu Bar
		if (ImGui::BeginMenuBar())
		{
			// Save
			if (ImGui::MenuItem("Save##Controller"))
			{
				Save();
			}

			ImGui::EndMenuBar();
		}
		// Parameters and Layers window
		ImGui::BeginChild("Animator Parameters and Layers", ImVec2(220.0f, 0.0f), true);
		{
			constexpr ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("Param or Layer", tab_bar_flags))
			{
				if (ImGui::BeginTabItem("Parameters"))
				{
					OnGUIParameter();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Layers"))
				{
					OnGUILayerSelect();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();
		// StateMachine window
		ImGui::BeginChild("Animator StateMachine", ImVec2(0.0f, 0.0f));
		{
			constexpr ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("StateMachine and Mask", tab_bar_flags))
			{
				// StateMachine
				if (ImGui::BeginTabItem("StateMachine"))
				{
					OnGUILayerStateMachine(goid);
					ImGui::EndTabItem();
				}
				// Avatar Mask (Root���C���[�̓}�X�N���g�p���Ȃ�)
				if (m_selecting_layer != 0)
				{
					if (ImGui::BeginTabItem("Avatar Mask"))
					{
						OnGUILayerMask(bone_names);
						ImGui::EndTabItem();
					}
				}

				ImGui::EndTabBar();
			}
		}
		ImGui::EndChild();
	}
	void ResAnimatorController::OnGUIParameter()
	{
		// �V�K�쐬
		{
			// ���O����
			ImGui::IDWrapped id("Name of added Parameter");
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.65f);
			static std::string str;
			ImGui::InputText("##New Parameter name", &str);
			ImGui::SameLine();
			// �V�K�쐬�{�^��
			if (ImGui::Button("Add"))
			{
				ImGui::OpenPopup("Add parameter popup");
			}
			ImGui::SameLine();
			ImGui::HelpMarker("First, input new parameter name.\nSecond, push add button.\nFinally, select parameter type.");
			if (ImGui::BeginPopup("Add parameter popup"))
			{
				// Float
				if (ImGui::Selectable("Float"))
				{
					AddParameter(str, Animations::AnimatorControllerParameter::Type::Float);
					str.clear();
				}
				// Int
				if (ImGui::Selectable("Int"))
				{
					AddParameter(str, Animations::AnimatorControllerParameter::Type::Int);
					str.clear();
				}
				// Bool
				if (ImGui::Selectable("Bool"))
				{
					AddParameter(str, Animations::AnimatorControllerParameter::Type::Bool);
					str.clear();
				}
				// Trigger
				if (ImGui::Selectable("Trigger"))
				{
					AddParameter(str, Animations::AnimatorControllerParameter::Type::Trigger);
					str.clear();
				}

				ImGui::EndPopup();
			}
		}
		ImGui::Separator();
		ImGui::Separator();
		// ImGui�̃t�B���^�[�@�\
		static ImGuiTextFilter im_filter;
		ImGui::Text("Filter");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		im_filter.Draw("##parameter filter");
		ImGui::Separator();

		// �p�����[�^�ϐ��̗���
		ImGui::BeginChild("Parameters");
		ImGui::HelpMarker("If would like to change type, \nRight Click on parameter variable.");
		const auto itr_param_end = m_parameters.end();
		auto itr_param = m_parameters.begin();
		for (; itr_param != itr_param_end;)
		{
			// �t�B���^�[��ʉ߂��邩�`�F�b�N
			if (im_filter.PassFilter(itr_param->first.c_str()))
			{
				ImGui::IDWrapped id(itr_param->second.get());
				// �폜�p�{�^��
				if (ImGui::ButtonColorSettable("X##Erase Button", ImVec4(0.75f, 0.2f, 0.2f, 1.0f), ImVec4(0.55f, 0.2f, 0.2f, 1.0f), ImVec4(0.4f, 0.05f, 0.05f, 1.0f)))
				{
					itr_param = m_parameters.erase(itr_param);
					continue;
				}
				// �p�����[�^��
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
				std::string str = itr_param->first;
				if (ImGui::InputText("##Parameter name", &str, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					// �����p�����[�^�`�F�b�N
					auto itr_find = m_parameters.find(str);
					if (itr_find == itr_param_end)
					{
						// �L�[���ύX����邽�ߐV�����쐬����
						std::shared_ptr<Animations::AnimatorControllerParameter> param = itr_param->second;
						itr_param = m_parameters.erase(itr_param);
						m_parameters.emplace(str, std::move(param));
						continue;
					}
				}

				// �p�����[�^���Ƃ�GUI�\��
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				itr_param->second->OnGUI();
				ImGui::Separator();
			}
			// erase����Ă��Ȃ��Ƃ��̓C���N�������g
			++itr_param;
		}
		ImGui::EndChild();
	}
	void ResAnimatorController::OnGUILayerSelect()
	{
		// ���C���[���\���ƕύX
		{
			ImGui::Text("Name");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			std::string str = m_layers.at(m_selecting_layer)->GetName();
			// Root���C���[�͕ύX�s��
			if (m_selecting_layer == 0/*Root*/)
			{
				ImGui::InputText("##layer name", &str, ImGuiInputTextFlags_ReadOnly);
			}
			// �ύX�\�Ȗ��O�\��
			else
			{
				if (ImGui::InputText("##layer name", &str, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					m_layers.at(m_selecting_layer)->SetName(str);
				}
			}
		}
		// �V�K���C���[�ǉ�
		ImGui::Separator();
		if (ImGui::Button("Add Layer##Layer"))
		{
			AddLayer("New Layer");
		}
		// ���C���[���̕ύX
		// Root�͌Œ�
		if (m_selecting_layer != 0)
		{
			// Up
			ImGui::SameLine();
			if (ImGui::Button("Up##layer alignment"))
			{
				// Root�͌Œ�̂���1���ŏ�
				if (m_selecting_layer > 1)
				{
					// ���̃��C���[��swap����
					const auto buf = m_layers.at(m_selecting_layer);
					m_layers[m_selecting_layer] = m_layers[m_selecting_layer - 1];
					m_layers[m_selecting_layer - 1] = buf;
					// �I�𒆂�index�̕ύX
					--m_selecting_layer;
				}
			}
			// Down
			ImGui::SameLine();
			if (ImGui::Button("Down##layer alignment"))
			{
				// �ő僌�C���[�C���f�b�N�X�𒴂����Ȃ�
				const int max_layer_index = static_cast<int>(m_layers.size()) - 1;
				if (m_selecting_layer < max_layer_index)
				{
					// ����̃��C���[��swap����
					const auto buf = m_layers.at(m_selecting_layer);
					m_layers[m_selecting_layer] = m_layers[m_selecting_layer + 1];
					m_layers[m_selecting_layer + 1] = buf;
					// �I�𒆂�index�̕ύX
					++m_selecting_layer;
				}
			}
		}
		ImGui::Separator();
		ImGui::Separator();
		// ���C���[�z������ɕ\��(Root���C���[�͓���)
		const int layer_num = static_cast<int>(m_layers.size());
		for (int i = 0; i < layer_num; ++i)
		{
			auto& layer = m_layers.at(i);
			ImGui::IDWrapped id(layer.get());

			// �폜�p�{�^��
			const ImVec2 button_size = ImVec2(20.0f, 0.0f);
			if (i == 0)
			{
				// Root�͍폜�s��
				ImGui::Dummy(button_size);
			}
			else
			{
				if (ImGui::ButtonColorSettable("X##Erase Button", ImVec4(0.75f, 0.2f, 0.2f, 1.0f), ImVec4(0.55f, 0.2f, 0.2f, 1.0f), ImVec4(0.4f, 0.05f, 0.05f, 1.0f), button_size))
				{
					// �I�𒆂̃��C���[��艺���폜����ꍇ��1����w���悤�ɂ���
					if (m_selecting_layer >= i)
					{
						--m_selecting_layer;
					}
					// ���C���[�̍폜
					m_layers.erase(m_layers.begin() + i);
					break;
				}
			}

			// ���C���[���\���ƃ��C���[�I��
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::Selectable(layer->GetName(), m_selecting_layer == i ? true : false))
			{
				m_selecting_layer = i;
			}
			ImGui::Separator();
		}
	}
	void ResAnimatorController::OnGUILayerStateMachine(const GameObjectID goid)
	{
		// �͈͊O�A�N�Z�X���p
		if (static_cast<int>(m_layers.size()) <= m_selecting_layer)
			m_selecting_layer = 0;

		m_layers.at(m_selecting_layer)->OnGUIStateMachine(goid, m_parameters);
	}
	void ResAnimatorController::OnGUILayerMask(const std::vector<std::string>* bone_name)
	{
		// �͈͊O�A�N�Z�X���p
		if (static_cast<int>(m_layers.size()) <= m_selecting_layer)
			m_selecting_layer = 0;

		m_layers.at(m_selecting_layer)->OnGUIMask(bone_name, m_parameters);
	}
#endif // USE_IMGUI

	std::shared_ptr<Animations::AnimatorState> ResAnimatorController::AddMotion(const std::string& motion_filepath)
	{
		return m_layers.at(0)->AddMotion(motion_filepath);
	}

	int ResAnimatorController::GetParameterCount() const
	{
		return static_cast<int>(m_parameters.size());
	}

	void ResAnimatorController::AddParameter(const std::string& param_name, Animations::AnimatorControllerParameter::Type type)
	{
		auto s_ptr = std::make_shared<Animations::AnimatorControllerParameter>();
		m_parameters.emplace(param_name, s_ptr);
		s_ptr->type = type;
	}

	void ResAnimatorController::RemoveParameter(const std::string& param_name)
	{
		m_parameters.erase(param_name);
	}

	std::shared_ptr<Animations::AnimatorControllerParameter> ResAnimatorController::GetParameter(const std::string& param_name)
	{
		const auto itr = m_parameters.find(param_name);
		if (itr == m_parameters.end())
			return nullptr;
		return itr->second;
	}

	std::shared_ptr<Animations::AnimatorControllerLayer> ResAnimatorController::AddLayer(const std::string& layer_name)
	{
		m_layers.emplace_back(std::make_shared<Animations::AnimatorControllerLayer>());
		std::shared_ptr<Animations::AnimatorControllerLayer> s_ptr = m_layers.back();
		s_ptr->SetName(layer_name);
		return s_ptr;
	}

	void ResAnimatorController::RemoveLayer(const std::string& layer_name)
	{
		// Root�͏����Ȃ�
		if (m_layers.size() == 1)
			return;

		const auto itr_end = m_layers.end();
		for (auto itr = m_layers.begin() + 1; itr != itr_end; ++itr)
		{
			if ((*itr)->GetName() == layer_name)
			{
				m_layers.erase(itr);
				return;
			}
		}
	}

	void ResAnimatorController::RemoveLayer(const int index)
	{
		// Root�͏����Ȃ�
		if (index >= static_cast<int>(m_layers.size()) || index == 0)
			return;

		m_layers.erase(m_layers.begin() + index);
	}

	std::shared_ptr<Animations::AnimatorControllerLayer> ResAnimatorController::GetLayer(const std::string& layer_name)
	{
		const auto itr_end = m_layers.end();
		for (auto itr = m_layers.begin(); itr != itr_end; ++itr)
		{
			if ((*itr)->GetName() == layer_name)
			{
				return *itr;
			}
		}
		return nullptr;
	}

	std::shared_ptr<Animations::AnimatorControllerLayer> ResAnimatorController::GetLayer(const int index)
	{
		if (index >= static_cast<int>(m_layers.size()))
			return nullptr;

		return m_layers.at(index);
	}

	void ResAnimatorController::SetBool(const std::string& param_name, bool param)
	{
		const auto itr = m_parameters.find(param_name);
		if (itr == m_parameters.end())
			return;
		itr->second->default_bool = param;
	}

	void ResAnimatorController::SetInteger(const std::string& param_name, int param)
	{
		const auto itr = m_parameters.find(param_name);
		if (itr == m_parameters.end())
			return;
		itr->second->default_int = param;
	}

	void ResAnimatorController::SetFloat(const std::string& param_name, float param)
	{
		const auto itr = m_parameters.find(param_name);
		if (itr == m_parameters.end())
			return;
		itr->second->default_float = param;
	}

	void ResAnimatorController::SetTrigger(const std::string& param_name)
	{
		const auto itr = m_parameters.find(param_name);
		if (itr == m_parameters.end())
			return;
		itr->second->default_bool = true;
	}

	bool ResAnimatorController::GetBool(const std::string& param_name)
	{
		const auto itr = m_parameters.find(param_name);
		if (itr == m_parameters.end())
		{
			assert(0 && "invalid argument");
			return false;
		}
		return itr->second->default_bool;
	}

	int ResAnimatorController::GetInteger(const std::string& param_name)
	{
		const auto itr = m_parameters.find(param_name);
		if (itr == m_parameters.end())
		{
			assert(0 && "invalid argument");
			return 0;
		}
		return itr->second->default_int;
	}

	float ResAnimatorController::GetFloat(const std::string& param_name)
	{
		const auto itr = m_parameters.find(param_name);
		if (itr == m_parameters.end())
		{
			assert(0 && "invalid argument");
			return 0.0f;
		}
		return itr->second->default_float;
	}

	void ResAnimatorController::ResetTrigger(const std::string& param_name)
	{
		const auto itr = m_parameters.find(param_name);
		if (itr == m_parameters.end())
		{
			assert(0 && "invalid argument");
			return;
		}
		itr->second->default_bool = false;
	}

	void ResAnimatorController::ApplyAnimationTransform(
		const GameObjectID goid,
		const float elapsed_time,
		const std::unordered_map<std::string, int>* node_index,
		const std::vector<int>& enable_nodes,
		std::vector<Animations::KeyData>& data
	)
	{
		const size_t layer_cnt = m_layers.size();
		for (size_t i = 0; i < layer_cnt; ++i)
		{
			m_layers.at(i)->ApplyAnimation(
				goid,
				elapsed_time,
				(i == 0) ? true : false,
				m_parameters,
				node_index,
				enable_nodes,
				data
			);
		}
	}

	void ResAnimatorController::SetAsyncOnLoad()
	{
		// ���[�h����
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}
		// ���[�h�J�n
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);

			if (!ifs.is_open())
			{
				b_result = false;
			}
			else
			{
				cereal::BinaryInputArchive ar(ifs);

				ar(*this);
				b_result = true;
			}
		}
		// ���[�h�I��
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (b_result)
			{
				// ����
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;

				////////// ���[�h��̏���
				/* nothing */


			}
			else
			{
				// ���s
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
		}
	}

	void ResAnimatorController::OnLoad()
	{
		// ���[�h����
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}
		// ���[�h�J�n
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);
			if (!ifs.is_open())
			{
				b_result = false;
			}
			else
			{
				cereal::BinaryInputArchive ar(ifs);

				ar(*this);
				b_result = true;
			}
		}
		// ���[�h�I��
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (b_result)
			{
				// ����
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;

				////////// ���[�h��̏���
				/* nothing */

			}
			else
			{
				// ���s
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
		}
	}

	bool ResAnimatorController::Save()
	{
		// �t�@�C�����擾
		std::string filepath;
		if (!MyFunc::FetchSaveFileName(filepath, TEXT("AnimController(*.controller)\0*.controller\0"), TEXT("Save AnimController"), TEXT(".\\Asset\\Animations")))
		{
			return false;
		}
		// �t�@�C���̏㏑�����͊g���q�����邽�߃`�F�b�N
		if (MyFunc::GetExtension(filepath).empty())
		{
			filepath += ANIMATOR_CONTROLLER_EXTENSION;
		}
		// �V�����t�@�C���p�X���Z�b�g
		SetFilePath(filepath);
		// �V���A���C�Y
		// Binary
		{
			std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. ResAnimatorController::Save()");
				return false;
			}
			cereal::BinaryOutputArchive ar(ofs);
			ar(*this);
		}
		// Json
#ifdef SAVE_JSON
		{
			filepath += "J";
			std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. ResAnimatorController::Save()");
				return false;
			}
			cereal::JSONOutputArchive ar(ofs);
			ar(*this);
		}
#endif// ~SAVE_JSON

		return true;
	}
	// ~ResAnimatorController


}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::ResAnimatorController);
CEREAL_CLASS_VERSION(TKGEngine::ResAnimatorController, 1);
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::IResAnimatorController, TKGEngine::ResAnimatorController)