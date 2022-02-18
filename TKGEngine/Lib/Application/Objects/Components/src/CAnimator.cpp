
#include "../inc/CAnimator.h"

#include "Components/inc/CTransform.h"
#include "Application/Objects/inc/IGameObject.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Managers/GameObjectManager.h"
#include "Managers/SceneManager.h"
#include "Managers/AnimatorManager.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/ITime.h"
#ifdef USE_IMGUI
#include "Systems/inc/IGUI.h"
#endif // USE_IMGUI

#include "Utility/inc/myfunc_file.h"

#include <functional>
#include <cassert>

REGISTERCOMPONENT(TKGEngine::Animator);

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	ConstantBuffer Animator::m_cb_default_model;
	bool Animator::m_is_initialized = false;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Animator::Animator()
		: Behaviour(COMPONENT_TYPE::COMPONENT_GENERAL)
	{
		/* nothing */
	}

	Animator::~Animator()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void Animator::OnGUI()
	{
		ImGui::PushID(this);
		// Behaviour::OnGUIの実行
		Behaviour::OnGUI();

		// Controller
		ImGui::Text("Animator Controller");
		ImGui::Indent(ImGui::INDENT_VALUE);
		{
			// コントローラ名
			{
				ImGui::PushID("Set Controller Name");
				ImGui::Text("Name:");
				ImGui::SameLine();
				constexpr size_t str_len = 256;
				static char str[str_len] = "\0";
				if (m_controller.HasController())
				{
					strncpy_s(str, m_controller.GetName(), str_len);
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - ImGui::DEFAULT_SPACE_SIZE - ImGui::INDENT_VALUE);
					if (ImGui::InputText("##Controller name", str, str_len, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						m_controller.SetName(str);
					}
				}
				else
				{
					ImGui::TextColored(ImGui::ERROR_FONT_COLOR, "No Controller!");
				}
				ImGui::PopID();
			}
			// Path
			ImGui::Text("Path");
			ImGui::Text("\"%s\"", m_controller_filedata.filepath.c_str());
			// Create
			if (ImGui::Button("Create##AnimController"))
			{
				m_controller.Create();
			}
			// Load
			ImGui::SameLine();
			if (ImGui::Button("Load##AnimController"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Animator Controller(*.controller)\0*.controller\0"), TEXT("Load Animator Controller"), TEXT(".\\Asset\\Animations")))
				{
					SetController(filepath);
				}
			}
			// コントローラとアバターを持っているなら表示
			if (m_controller.HasController() && m_avatar.HasAvatar())
			{
				// Open
				if (ImGui::Button("Open Controller##Controller"))
				{
					IGUI::Get().SetDrawController(GetOwnerID(), m_controller, m_avatar);
				}
			}
		}
		ImGui::Separator();
		ImGui::Unindent(ImGui::INDENT_VALUE);

		// Avatar
		ImGui::Text("Avatar");
		ImGui::Indent(ImGui::INDENT_VALUE);
		{
			// Path
			ImGui::Text("Path");
			ImGui::Text("\"%s\"", m_avatar_filedata.filepath.c_str());
			// Load
			if (ImGui::Button("Load##Avatar"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Avatar(*.avatar)\0*.avatar\0"), TEXT("Load Avatar")))
				{
					SetAvatar(filepath);
				}
			}
		}
		ImGui::Unindent(ImGui::INDENT_VALUE);
		
		ImGui::PopID();
	}
#endif // USE_IMGUI

	void Animator::SetCB(ID3D11DeviceContext* p_context, int slot, bool set_cs)
	{
		// アニメーションに必要なリソースデータがなければデフォルトモーションをセット
		if (!m_avatar_filedata.HasData() || !m_controller.HasController())
		{
			SetDefaultCB(p_context, slot, set_cs);
		}
		else
		{
			if (set_cs)
				m_cb_model.SetCS(p_context, slot);
			else
				m_cb_model.SetVS(p_context, slot);
		}
	}

	void Animator::SetDefaultCB(ID3D11DeviceContext* p_context, int slot, bool set_cs)
	{
		if (!m_is_initialized)
		{
			m_is_initialized = true;
			OnInitOnce();
		}

		if (set_cs)
			m_cb_default_model.SetCS(p_context, slot);
		else
			m_cb_default_model.SetVS(p_context, slot);
	}

	AnimatorController& Animator::GetController()
	{
		return m_controller;
	}

	void Animator::SetController(const std::string& filepath)
	{
		m_controller.Load(filepath);
		// リソース状態チェック
		if (m_controller.HasController())
		{
			m_controller_filedata.Set(filepath);
		}
		else
		{
			m_controller_filedata.Clear();
		}
	}

	void Animator::SetAvatar(const std::string& filepath)
	{
		// 既に持っていたら先に削除
		if (m_avatar_filedata.HasData())
		{
			OnRemoveAvatar();
		}
		m_avatar.Load(filepath);
		// リソース状態チェック
		if (m_avatar.HasAvatar())
		{
			m_avatar_filedata.Set(filepath);
			OnAddAvatar();
		}
		else
		{
			m_avatar_filedata.Clear();
		}
	}

	void Animator::SetAvatar(const Avatar& avatar)
	{
		// 既に持っていたら先に削除
		if (m_avatar_filedata.HasData())
		{
			OnRemoveAvatar();
		}
		m_avatar = avatar;
		// リソース状態チェック
		if (m_avatar.HasAvatar())
		{
			m_avatar_filedata.Set(m_avatar.GetFilePath());
			OnAddAvatar();
		}
		else
		{
			m_avatar_filedata.Clear();
		}
	}
	
	std::vector<std::shared_ptr<Transform>>& Animator::GetBoneReferences()
	{
		return m_bone_references;
	}

	int Animator::GetParameterCount() const
	{
		return m_controller.GetParameterCount();
	}

	std::shared_ptr<Animations::AnimatorControllerParameter> Animator::GetParameter(const std::string& param_name)
	{
		return m_controller.GetParameter(param_name);
	}

	void Animator::SetBool(const std::string& param_name, bool param)
	{
		m_controller.SetBool(param_name, param);
	}

	void Animator::SetInteger(const std::string& param_name, int param)
	{
		m_controller.SetInteger(param_name, param);
	}

	void Animator::SetFloat(const std::string& param_name, float param)
	{
		m_controller.SetFloat(param_name, param);
	}

	void Animator::SetTrigger(const std::string& param_name)
	{
		m_controller.SetTrigger(param_name);
	}

	bool Animator::GetBool(const std::string& param_name)
	{
		return m_controller.GetBool(param_name);
	}

	int Animator::GetInteger(const std::string& param_name)
	{
		return m_controller.GetInteger(param_name);
	}

	float Animator::GetFloat(const std::string& param_name)
	{
		return m_controller.GetFloat(param_name);
	}

	void Animator::ResetTrigger(const std::string& param_name)
	{
		return m_controller.ResetTrigger(param_name);
	}

	void Animator::SetUpdateMode(UpdateMode mode)
	{
		m_update_mode = mode;
	}

	void Animator::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		const std::shared_ptr<Animator> anim = std::static_pointer_cast<Animator>(p_self);
		m_anim_id = AnimatorManager::RegisterManager(anim);

		// デシリアライズ時にAvatar,AnimatorControllerを持っていたらロードする
		if (m_avatar_filedata.HasData())
		{
			// Avatarのロード
			m_avatar.Load(m_avatar_filedata.filepath);

			// 配列のリサイズ
			const int enabled_bone_cnt = m_avatar.GetEnabledBoneCount();
			m_current_key_data.resize(enabled_bone_cnt);
			m_cb_matrix_data.resize(enabled_bone_cnt);
			// CBufferの作成
			m_cb_model.Create(static_cast<int>(sizeof(MATRIX)) * MAX_BONES, true);
		}
		if (m_controller_filedata.HasData())
		{
			// controllerのロード
			m_controller.Load(m_controller_filedata.filepath);
		}
	}

	void Animator::OnDestroy()
	{
		AnimatorManager::UnregisterManager(m_anim_id);
		m_bone_references.clear();
	}

	void Animator::OnInitOnce()
	{
		const MATRIX default_bones[MAX_BONES] = { MATRIX::Identity };
		if (!m_cb_default_model.Create(static_cast<int>(sizeof(default_bones)), default_bones, false))
		{
			assert(0 && "failed Animator::OnInitOnce()");
			return;
		}
	}

	void Animator::OnAddAvatar()
	{
		const int root_cnt = m_avatar.GetRootCount();
		const int bone_cnt = m_avatar.GetBoneCount();
		const auto go = GetGameObject();

		m_bone_references.resize(bone_cnt);
		m_node_references.resize(bone_cnt);
		const int enabled_bone_cnt = m_avatar.GetEnabledBoneCount();
		m_current_key_data.resize(enabled_bone_cnt);
		m_cb_matrix_data.resize(enabled_bone_cnt);
		
		// Root Boneから順にボーンをGameObjectとして作成
		{
			int counter = 0;
			for (int i = 0; i < root_cnt; ++i)
			{
				const int root_idx = m_avatar.GetRootIndex(i);
				const Node* root = m_avatar.GetNode(root_idx);
				// BoneをGameObjectにする再帰関数
				RecurseCreateFromBone(root, root_idx, go, true, counter);
			}
		}

		// Bone数からCBを作成
		// TODO : 警告回避のために過剰なバッファサイズを確保している
		m_cb_model.Create(static_cast<int>(sizeof(MATRIX)) * MAX_BONES, true);
	}

	void Animator::OnRemoveAvatar()
	{
		m_avatar_filedata.Clear();
		m_cb_model.Release();
		m_bone_references.clear();
		m_bone_references.shrink_to_fit();
		m_node_references.clear();
		m_node_references.shrink_to_fit();
		m_current_key_data.clear();
		m_current_key_data.shrink_to_fit();
		const int root_cnt = static_cast<int>(m_root_bones.size());
		for (int i = 0; i < root_cnt; ++i)
		{
			m_root_bones.at(i)->Destroy(-1.0f);
		}
		m_root_bones.clear();
		m_root_bones.shrink_to_fit();
		m_root_index.clear();
		m_root_index.shrink_to_fit();
	}

	void Animator::RecurseCreateFromBone(const Node* node, const int self_idx, const std::shared_ptr<IGameObject>& parent, const bool is_root, int& counter)
	{
		const int child_cnt = static_cast<int>(node->children_idx.size());
		const auto child_go = GameObjectManager::CreateChild(parent, node->bone_name.c_str());
		const auto transform = child_go->GetTransform();

		// Transformの参照を取得する
		m_bone_references.at(counter) = transform;
		m_node_references.at(counter) = node->bone_idx;
		++counter;

		// ルートオブジェクトは参照を保持しておく
		if (is_root)
		{
			m_root_bones.emplace_back(child_go);
			m_root_index.emplace_back(self_idx);
		}

		// Nodeの初期位置を指定
		transform->LocalPosition(node->init_translate);
		transform->LocalRotation(node->init_rotate);
		transform->LocalScale(node->init_scale);

		const auto& child_node_indices = node->children_idx;
		for (int i = 0; i < child_cnt; ++i)
		{
			const int child_idx = child_node_indices.at(i);
			RecurseCreateFromBone(m_avatar.GetNode(child_idx), child_idx, child_go, false, counter);
		}
	}

	void Animator::RecurseCalculateAnimationMatrix(const Node* node, const int self_idx, const MATRIX& matrix)
	{
		const int child_cnt = static_cast<int>(node->children_idx.size());
		const int matrix_idx = m_node_references.at(self_idx);

		const MATRIX anim_matrix = m_bone_references.at(self_idx)->GetAffineTransform() * matrix;

		if (matrix_idx != -1)
		{
			m_cb_matrix_data.at(matrix_idx) = node->inverse_transform * anim_matrix;
		}

		const auto& child_node_indices = node->children_idx;
		for (int i = 0; i < child_cnt; ++i)
		{
			const int child_idx = child_node_indices.at(i);
			RecurseCalculateAnimationMatrix(m_avatar.GetNode(child_idx), child_idx, anim_matrix);
		}
	}

	void Animator::ApplyAnimationTransform()
	{
		// 早期リターン
		if (!m_avatar_filedata.HasData() || !m_controller.HasController())
			return;

		// UpdateModeによる経過時間の取得
		float elapsed_time = 0.0f;
		switch (m_update_mode)
		{
			case UpdateMode::Normal:
				elapsed_time = ITime::Get().DeltaTime();
				break;
			case UpdateMode::UnscaledTime:
				elapsed_time = ITime::Get().UnscaledDeltaTime();
				break;
		}
		// KeyFrameの情報を取得
		m_controller.ApplyAnimationTransform(
			GetOwnerID(),
			elapsed_time,
			m_avatar.GetNodeIndex(),
			m_node_references,
			m_current_key_data
		);

		// Transformの更新
		const int tr_cnt = static_cast<int>(m_bone_references.size());
		for (int i = 0; i < tr_cnt; ++i)
		{
			const int bone_idx = m_node_references.at(i);
			// アニメーションしないボーン
			if (bone_idx == -1)
			{
				//const Node* node = m_avatar.GetNode(i);
				//m_bone_references.at(i)->LocalPosition(node->init_translate);
				//m_bone_references.at(i)->LocalRotation(node->init_rotate);
				//m_bone_references.at(i)->LocalScale(node->init_scale);
				continue;
			}

			// ボーンのTransformをセット
			m_bone_references.at(i)->LocalPosition(m_current_key_data.at(bone_idx).translate);
			m_bone_references.at(i)->LocalRotation(m_current_key_data.at(bone_idx).rotate);
			m_bone_references.at(i)->LocalScale(m_current_key_data.at(bone_idx).scale);
		}
	}

	void Animator::UpdateAnimationMatrix()
	{
		// 早期リターン
		if (!m_avatar_filedata.HasData())
			return;
		
		// Rootから更新していく
		const int root_cnt = static_cast<int>(m_root_index.size());
		for (int i = 0; i < root_cnt; ++i)
		{
			const int root_idx = m_root_index.at(i);
			const Node* root = m_avatar.GetNode(root_idx);
			// Animation Matrixの再帰関数
			RecurseCalculateAnimationMatrix(root, root_idx, MATRIX::Identity);
		}

		// CBufferに送る
		const auto context = IGraphics::Get().IC();
		MATRIX* dst = static_cast<MATRIX*>(m_cb_model.Map(context));
		memcpy(dst, m_cb_matrix_data.data(), m_cb_matrix_data.size() * sizeof(MATRIX));
		m_cb_model.Unmap(context);
	}

	bool Animator::IsActiveAnimator()
	{
		return IsActiveBehaviour();
	}

}// namespace TKGEngine