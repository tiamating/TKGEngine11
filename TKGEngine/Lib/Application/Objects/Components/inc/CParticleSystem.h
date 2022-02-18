#pragma once

#include "CRenderer.h"

#include "Application/Resource/inc/Mesh.h"
#include "Application/Resource/inc/Effect.h"
#include "Application/Resource/inc/Material_Defined.h"


namespace TKGEngine
{
	class IGameObject;

	/// <summary>
	/// Rendering Particle Management module.
	/// </summary>
	class ParticleSystem
		: public Renderer
	{
	public:
		// ==============================================
		// public declaration
		// ==============================================


		// ==============================================
		// public methods
		// ==============================================
		ParticleSystem(const ParticleSystem&) = delete;
		ParticleSystem& operator=(const ParticleSystem&) = delete;

		ParticleSystem();
		virtual ~ParticleSystem();


		static void PauseAll();
		static void ResumeAll();

		bool SetEffect(const std::string& filepath);
		void RemoveEffect();

		int GetRenderQueue() const;
		void SetRenderQueue(int queue);

		// 更新をどの経過時間で行うか(ロード後は実行中の変更不可)
		void SetUseUnscaledTime(bool use_unscaled);

		void SetKeepPosition(bool is_keep);

		void SetTargetPosition(const VECTOR3& pos);
		void SetTargetPosition(const float x, const float y, const float z);

		void SetSpeed(float speed);
		float GetSpeed() const;

		void SetScale(float x, float y, float z);
		void SetScale(const VECTOR3& scale);

		bool IsExist() const;
		bool IsPlaying() const;

		void Play(float start_time = 0.0f);
		void Stop();
		void StopEmit();
		void Pause();
		void Resume();


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
					cereal::base_class<Renderer>(this),
					CEREAL_NVP(m_render_queue),
					CEREAL_NVP(m_use_unscaled_time),
					CEREAL_NVP(m_effect_filedata),
					CEREAL_NVP(m_keep_pos_and_rot),
					CEREAL_NVP(m_speed),
					CEREAL_NVP(m_scale)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI

		// Renderer
		void OnCreated() override;
		void OnDestroyed() override;
		// ~Renderer

		// IRenderer
		void AddMesh(const std::string& filepath, bool set_global = true) override;
		void AddMesh(const Mesh& mesh, bool set_global = true) override;

		void Render(
			ID3D11DeviceContext* p_context,
			int index, int start_index, int instance_count,
			VertexBuffer& instance_buffer,
			const std::shared_ptr<ICamera>& camera,
			bool write_depth
		) override;

		// パーティクルシステムが描画中かチェック
		bool IsActiveAndEnabled() override;

		int GetSubsetCount() const override;
		unsigned GetHashMesh() const override;
		unsigned GetHashMaterial(int index) const override;
		int GetRenderQueue(int index) const override;
		inline bool IsUsedTarget(int index) const override;
		ShadowCastingMode GetShadowCastMode() const override;
		void CalculateRenderParameter(const std::shared_ptr<ICamera>& camera) override;
		unsigned GetHashTexture() const override;	// UI用
		int GetDepth() const override;		// UI用
		// ~IRenderer

		// ==============================================
		// private variables
		// ==============================================
		// スケールの最小値
		static constexpr float MIN_EFFECT_SCALE = 0.0001f;

		int m_render_queue = RenderQueue::Transparent;

		// 更新をどの経過時間で行うか(ロード後は実行中の変更不可)
		bool m_use_unscaled_time = false;

		FileLoadStateData m_effect_filedata;
		Effect m_effect;
		// エフェクトファイルをロードする必要があるか
		bool m_need_load_file = false;

		bool m_keep_pos_and_rot = false;
		float m_speed = 1.0f;
		VECTOR3 m_scale = VECTOR3::One;
	};

	// -----------------------------------
	// inline
	// -----------------------------------
	inline void ParticleSystem::AddMesh(const std::string& filepath, bool set_global)
	{
		/* nothing */
	}

	inline void ParticleSystem::AddMesh(const Mesh& mesh, bool set_global)
	{
		/* nothing */
	}

	inline int ParticleSystem::GetSubsetCount() const
	{
		return 1;
	}

	inline unsigned ParticleSystem::GetHashMesh() const
	{
		return 0;
	}

	inline unsigned ParticleSystem::GetHashMaterial(int index) const
	{
		return 0;
	}

	inline int ParticleSystem::GetRenderQueue() const
	{
		return m_render_queue;
	}

	inline ShadowCastingMode ParticleSystem::GetShadowCastMode() const
	{
		return ShadowCastingMode::OFF;
	}

	inline void ParticleSystem::CalculateRenderParameter(const std::shared_ptr<ICamera>& camera)
	{
		/* nothing */
	}

	inline void ParticleSystem::SetRenderQueue(int queue)
	{
		m_render_queue = queue;
	}

	inline void ParticleSystem::SetUseUnscaledTime(bool use_unscaled)
	{
		m_use_unscaled_time = use_unscaled;
	}

	inline unsigned ParticleSystem::GetHashTexture() const
	{
		return 0;
	}

	inline int ParticleSystem::GetRenderQueue(int index) const
	{
		return m_render_queue;
	}

	inline bool ParticleSystem::IsUsedTarget(int index) const
	{
		return false;
	}

	inline int ParticleSystem::GetDepth() const
	{
		return 0;
	}
}// namespaace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::ParticleSystem, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::ParticleSystem, "TKGEngine::ParticleSystem")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Renderer, TKGEngine::ParticleSystem)