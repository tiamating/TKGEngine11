
#include "../inc/CParticleSystem.h"

#include "Application/Objects/inc/IGameObject.h"
#include "Application/Resource/inc/Shader.h"
#include "Utility/inc/myfunc_file.h"
#include "Systems/inc/LogSystem.h"

#include <cassert>

REGISTERCOMPONENT(TKGEngine::ParticleSystem);

namespace TKGEngine
{
	ParticleSystem::ParticleSystem()
		: Renderer(RendererType::Particle)
	{
		/* nothing */
	}

	ParticleSystem::~ParticleSystem()
	{
		/* nothing */
	}

	void ParticleSystem::PauseAll()
	{
		Effect::PauseAll();
	}

	void ParticleSystem::ResumeAll()
	{
		Effect::ResumeAll();
	}

#ifdef USE_IMGUI
	void ParticleSystem::OnGUI()
	{
		ImGui::PushID(this);

		// Renderer::OnGUI
		Renderer::OnGUI();

		// ParticleSystem用GUI
		// Effect
		{
			// ファイル名
			ImGui::Text("Effect file");
			ImGui::Text("\"%s\"", m_effect_filedata.filepath.c_str());
			// Loadボタン
			ImVec2 button_size;
			const float width = ImGui::GetWindowWidth();
			constexpr float button_space = 0.3f;
			button_size.x = width * (1.0f - button_space);
			button_size.y = ImGui::DEFAULT_SPACE_SIZE;
			// ボタンを1/2の位置に置く
			ImGui::SetCursorPosX(width * button_space * 0.5f);
			if (ImGui::Button("Load Effect", button_size))
			{
				// ロードするファイル名を取得
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Effect(*.efk)\0*.efk\0"), TEXT("Load Effect"), TEXT(".\\Asset")))
				{
					SetEffect(filepath);
				}
			}
		}
		// RenderQueue
		{
			ImGui::Text("Render Queue");
			ImGui::SameLine();
			ImGui::HelpMarker("Transparent is over 3000.");
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.4f);
			ImGui::DragInt("##Render Queue", &m_render_queue, 0.1f, 0, 5000, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			if (ImGui::Button("Default Queue"))
			{
				ImGui::OpenPopup("Default Queue Popup");
			}
			if (ImGui::BeginPopup("Default Queue Popup"))
			{
				// Background
				if (ImGui::Selectable("Background"))
				{
					m_render_queue = RenderQueue::Background;
				}
				// Geometry
				if (ImGui::Selectable("Geometry"))
				{
					m_render_queue = RenderQueue::Geometry;
				}
				// Transparent
				if (ImGui::Selectable("Transparent"))
				{
					m_render_queue = RenderQueue::Transparent;
				}
				// Overlay
				if (ImGui::Selectable("Overlay"))
				{
					m_render_queue = RenderQueue::Overlay;
				}
				ImGui::EndPopup();
			}
		}
		// Use unscaled time
		ImGui::Text("Use unscaled time");
		ImGui::SameLine();
		ImGui::HelpMarker("This flag can not change on playing.");
		ImGui::AlignedSameLine(0.1f);
		ImGui::Checkbox("##Use unscaled time", &m_use_unscaled_time);
		// Keep pos and rot
		ImGui::Text("Keep position and rotation");
		ImGui::AlignedSameLine(0.1f);
		ImGui::Checkbox("##Keep position and rotation", &m_keep_pos_and_rot);
		// Speed
		ImGui::Text("Speed");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat("##Speed", &m_speed, 0.005f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
		{
			SetSpeed(m_speed);
		}
		// Scale
		ImGui::Text("Scale");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat3("##Scale", &m_scale.x, 0.005f, MIN_EFFECT_SCALE, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
		{
			m_effect.SetScale(m_scale.x, m_scale.y, m_scale.z);
		}
		// Play and Stop
		{
			if (ImGui::Button("Play##Effect Button"))
			{
				Play();
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop##Effect Button"))
			{
				Stop();
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause##Effect Button"))
			{
				Pause();
			}
			ImGui::SameLine();
			if (ImGui::Button("Resume##Effect Button"))
			{
				Resume();
			}
		}

		ImGui::PopID();
	}
#endif // USE_IMGUI


	bool ParticleSystem::SetEffect(const std::string& filepath)
	{
		if (!m_effect.Load(filepath, m_use_unscaled_time))
		{
#ifdef USE_IMGUI
			const std::string debug_str = "failed load effect file. (" + filepath + ")";
			LOG_ASSERT(debug_str.c_str());
#endif// #ifdef USE_IMGUI
			return false;
		}
		m_effect_filedata.Set(filepath);
		return true;
	}

	void ParticleSystem::RemoveEffect()
	{
		m_effect.Unload(m_effect_filedata.filepath);
		m_effect_filedata.Clear();
		m_keep_pos_and_rot = false;
		m_speed = 1.0f;
	}

	void ParticleSystem::SetKeepPosition(bool is_keep)
	{
		m_keep_pos_and_rot = is_keep;
	}

	void ParticleSystem::SetTargetPosition(const VECTOR3& pos)
	{
		m_effect.SetTargetPosition(pos);
	}

	void ParticleSystem::SetTargetPosition(const float x, const float y, const float z)
	{
		m_effect.SetTargetPosition(x, y, z);
	}

	void ParticleSystem::SetSpeed(float speed)
	{
		if (speed < 0.0f)
			speed = 0.0f;

		m_effect.SetSpeed(speed);
		m_speed = speed;
	}

	float ParticleSystem::GetSpeed() const
	{
		return m_speed;
	}

	void ParticleSystem::SetScale(float x, float y, float z)
	{
		if (x < MIN_EFFECT_SCALE)
			x = MIN_EFFECT_SCALE;
		if (y < MIN_EFFECT_SCALE)
			y = MIN_EFFECT_SCALE;
		if (z < MIN_EFFECT_SCALE)
			z = MIN_EFFECT_SCALE;

		m_effect.SetScale(x, y, z);
		m_scale.x = x;
		m_scale.y = y;
		m_scale.z = z;
	}

	void ParticleSystem::SetScale(const VECTOR3& scale)
	{
		m_scale = scale;

		if (m_scale.x < MIN_EFFECT_SCALE)
			m_scale.x = MIN_EFFECT_SCALE;
		if (m_scale.y < MIN_EFFECT_SCALE)
			m_scale.y = MIN_EFFECT_SCALE;
		if (m_scale.z < MIN_EFFECT_SCALE)
			m_scale.z = MIN_EFFECT_SCALE;

		SetScale(m_scale.x, m_scale.y, m_scale.z);
	}

	bool ParticleSystem::IsExist() const
	{
		return m_effect.IsExist();
	}

	bool ParticleSystem::IsPlaying() const
	{
		return m_effect.IsPlaying();
	}

	void ParticleSystem::Play(float start_time)
	{
		const auto transform = GetTransform();
		m_effect.Play(transform->Position(), transform->Rotation(), start_time);
		m_effect.SetSpeed(m_speed);
		m_effect.SetScale(m_scale.x, m_scale.y, m_scale.z);
	}

	void ParticleSystem::Stop()
	{
		m_effect.Stop();
	}

	void ParticleSystem::StopEmit()
	{
		m_effect.StopEmit();
	}

	void ParticleSystem::Pause()
	{
		m_effect.Pause();
	}

	void ParticleSystem::Resume()
	{
		m_effect.Resume();
	}

	bool ParticleSystem::IsActiveAndEnabled()
	{
		// エフェクトファイルをロードする必要があればロードする
		if (m_need_load_file)
		{
			m_effect.Load(m_effect_filedata.filepath, m_use_unscaled_time);
			m_need_load_file = false;
		}

		// Rendererの有効条件にパーティクル存在が加わる
		return m_effect.IsExist() && Renderer::IsActiveAndEnabled();
	}

	void ParticleSystem::Render(
		ID3D11DeviceContext* p_context,
		int index, int start_index, int instance_count,
		VertexBuffer& instance_buffer,
		const std::shared_ptr<ICamera>& camera,
		bool write_depth
	)
	{
		if (write_depth)
			return;

		Shader::Deactivate(p_context);

		if (m_keep_pos_and_rot)
		{
			m_effect.Render(p_context, camera);
		}
		else
		{
			const auto transform = GetTransform();
			m_effect.Render(p_context, camera, transform->Position(), transform->Rotation());
		}
	}

	void ParticleSystem::OnCreated()
	{
		// ShadowCastingModeはOFF
		SetShadowCastMode(ShadowCastingMode::OFF);

		// effectファイルを持った状態でデシリアライズされていたらロードする
		if (m_effect_filedata.HasData())
		{
			m_need_load_file = true;
		}
		// Scale
		SetScale(m_scale);
		// Speed
		SetSpeed(m_speed);
	}

	void ParticleSystem::OnDestroyed()
	{
		Stop();
	}


}// namespace TKGEngine