#pragma once

#include "Application/Resource/inc/Texture.h"

#include "Utility/inc/myfunc_vector.h"

#include "../../external/Effekseer/EffekseerRendererDX11.h"

#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;


namespace TKGEngine
{
	class ICamera;

	class Effect
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Effect();
		virtual ~Effect() = default;
		Effect(const Effect&) = delete;
		Effect& operator=(const Effect& effect);


		static bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);
		static void Destroy();

		//static void RemoveUnused();

		// �c�ݕ`�掞�̃R�s�[�e�N�X�`���̃T�C�Y��ύX����
		static void OnResizeWindow(int width, int height);

		static void OnFrameUpdate(float unscaled_time, float scaled_time);
		// Deferred Context�p�t���[���I������
		static void OnFrameEnd();
		static void SetCameraParam(const MATRIX& view, const MATRIX& projection);

		static void PauseAll();
		static void ResumeAll();

		bool Load(const std::string& filepath, bool use_unscaled_time);
		void Unload(const std::string& filepath);

		void Render(ID3D11DeviceContext* p_context, const std::shared_ptr<ICamera>& camera);
		void Render(ID3D11DeviceContext* p_context, const std::shared_ptr<ICamera>& camera, const VECTOR3& pos, const Quaternion& rot);

		bool IsExist() const;
		bool IsPlaying() const;

		void Play(const VECTOR3& pos, const Quaternion& rot, float start_time = 0.0f);
		void Stop();
		void StopEmit();
		void Pause();
		void Resume();

		void SetSpeed(const float speed);
		void SetScale(const float x, const float y, const float z);

		void SetTargetPosition(const VECTOR3& pos);
		void SetTargetPosition(const float x, const float y, const float z);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private enum
		// ==============================================
		/// <summary>
		/// �G�t�F�N�g�̍X�V�Ɏg�p���鎞�ԃ^�C�v
		/// </summary>
		enum class EffectUpdateType
		{
			UnscaledTime = 0,
			ScaledTime = 1,

			Max_EffectUpdateType
		};

		// ==============================================
		// private class
		// ==============================================
		/// <summary>
		/// �c�ݕ`��O�ɌĂ΂��R�[���o�b�N
		/// </summary>
		class DistortionEffectCallback
			: public EffekseerRenderer::DistortingCallback
		{
		public:
			bool OnDistorting(EffekseerRenderer::Renderer* renderer) override;
			void SetCopyParameter(ID3D11DeviceContext* p_context, ICamera* p_camera)
			{
				context = p_context;
				camera = p_camera;
			}
			
		private:
			ID3D11DeviceContext* context = nullptr;
			ICamera* camera = nullptr;
		};


		// ==============================================
		// private methods
		// ==============================================
		
		/// <summary>
		/// �n���h���z���active�Ȃ��̂�擪���Ɋ񂹁A�L���Ȗ����̃C���f�b�N�X���擾����
		/// </summary>
		/// <returns> �L���ȃn���h���� </returns>
		int OrganizeHandles();

		/// <summary>
		/// �c�ݕ`�掞�̃R�s�[�e�N�X�`���̍쐬
		/// </summary>
		static void CreateCopyTexture(int width, int height);

		// ==============================================
		// private variables
		// ==============================================
		// ���̃G���e�B�e�B���Ǘ�����G�t�F�N�g�̃n���h���z��
		std::vector<Effekseer::Handle> m_effekseer_handles{};

		// �Ǘ�����Ă���n���h���̖����̃C���f�b�N�X
		int m_last_handle_index = 0;

		// �G���e�B�e�B�Ǘ����X�g���̃C�e���[�^
		std::list<Effect*>::iterator m_entity_ref_cache_iterator;

		// �G�t�F�N�g�f�[�^�Q��
		Effekseer::EffectRef m_effekseer_effect = nullptr;

		// �X�V���ǂ̌o�ߎ��Ԃōs����(���s���̕ύX�s��)
		EffectUpdateType m_update_type = EffectUpdateType::ScaledTime;

		// �c�ݕ`�掞�̃R�s�[�e�N�X�`��
		static Texture m_copy_texture;
		static DistortionEffectCallback* m_distortion_callbacks[2];
		// Effekseer�̃}�l�[�W���[�ƃ����_���[
		static Effekseer::ManagerRef m_effekseer_managers[static_cast<int>(EffectUpdateType::Max_EffectUpdateType)];
		static EffekseerRendererDX11::RendererRef m_effekseer_renderers[static_cast<int>(EffectUpdateType::Max_EffectUpdateType)];
		// Effect�Ǘ��pmap
		static std::unordered_map<std::string, Effekseer::EffectRef> m_caches[static_cast<int>(EffectUpdateType::Max_EffectUpdateType)];

		// �n���h���z����X�V����K�v�̂���G���e�B�e�B���Ǘ����郊�X�g
		static std::list<Effect*> m_entity_ref_caches;
	};


}// namespace TKGEngine