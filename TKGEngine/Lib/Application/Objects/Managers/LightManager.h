#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Application/Resource/inc/ConstantBuffer.h"
#include "Application/Resource/inc/VertexBuffer.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/template_thread.h"

#include <list>
#include <unordered_map>
#include <memory>


// ==============================================
// �O���錾
// ==============================================
namespace TKGEngine
{
	struct MainData;
	class ICamera;
	class Light;
	class VertexBuffer;
}

namespace TKGEngine
{
	/// <summary>
	/// ���C�g�R���|�[�l���g�̊Ǘ��A�p�C�v���C���ɃZ�b�g����}�l�[�W���[�N���X(�V���O���g��)
	/// </summary>
	class LightManager
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static bool Create();
		static void Destroy();

		static LightManager* GetInstance();

		// ���L����GameObjectID���L�[�ɂ���map�ɓo�^����
		static void RegisterManager(const std::shared_ptr<Light>& light);
		static void UnregisterManager(const GameObjectID owner_id);

		// �V�[�����̃��C�g���\�[�g����
		static void SortSceneLight();

		// ���C�g���ƂɍX�V�̌v�Z������
		static void CalculateSceneLight(const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera);

		// ���C�g���Ƃɉe��`�悷��
		static void RenderShadow(ID3D11DeviceContext* context);

		// ���C�g���p�C�v���C���ɃZ�b�g����
		static void SetPipeline(ID3D11DeviceContext* context);


		// ==============================================
		// public methods
		// ==============================================
		/* nothing */
		

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private struct
		// ==============================================
		struct DirectionalLight
		{
			MATRIX cascade_LVPs[MAX_CASCADE];		// �������Ƃ�LVP
			MATRIX cascade_light_views[MAX_CASCADE];		// �������Ƃ�LightView
			MATRIX cascade_light_projections[MAX_CASCADE];	// �������Ƃ�LightProj
			VECTOR4 color = VECTOR4::Zero;			// ���C�g�F
			VECTOR3 direction = VECTOR3::Forward;	// ���C�g����
			float strength = 0.4f;					// �e�F�̋���
			float shadow_size[4];				// �V���h�E�}�b�v�̃T�C�Y
			float inv_shadow_size[4];				// �V���h�E�}�b�v�̃T�C�Y�̋t��
			float bias = 0.01f;						// �V���h�E�o�C�A�X
			float normal_bias = 0.01f;				// �@�������̃V���h�E�o�C�A�X
			BOOL use_shadow_map = FALSE;			// �V���h�E�}�b�v���g�p���邩
			unsigned cascade_num = MAX_CASCADE;		// ������
			BOOL has_caster[MAX_CASCADE] = { FALSE };	// �L���X�^�[�������Ă��邩
			float filter_radius_uv = 1.0f;			// �T���v�����O�t�B���^��UV���a
			unsigned sampling_count = 1;			// �T���v�����O��(�ő�FMAX_POISSON)
			float dummy1;
			float dummy2;

			void operator=(const std::shared_ptr<Light>& light);
		};
		struct SpotLight
		{
			MATRIX LVP = MATRIX::Identity;			// ���C�g�r���[�v���W�F�N�V����
			MATRIX light_view = MATRIX::Identity;	// ���C�g�r���[
			MATRIX light_projection = MATRIX::Identity;	// ���C�g�v���W�F�N�V����
			VECTOR4 pos = VECTOR4::Zero;			// ���C�g�ʒu
			VECTOR4 color = VECTOR4::Zero;			// ���C�g�F
			VECTOR3 direction = VECTOR3::Forward;	// ���C�g����
			float range = 0.0f;						// �͈�(0�ȉ��Ŗ����ȃ��C�g)
			VECTOR3 attenuation = VECTOR3::One;		// �����W��(x:�萔 y:�ꎟ z:��)
			float falloff = 1.0f;					// �����W��(inner --> outer�ւ̌�������`)
			float inner_cone_cos = 1.0f;			// cos(��/2) (��:�����~���̍L����p)
			float outer_cone_cos = 1.0f;			// cos(��/2) (��:�O���~���̍L����p)
			BOOL use_light_mask = FALSE;			// ���C�g�}�X�N�e�N�X�`�����g�p���邩
			BOOL use_shadow_map = FALSE;			// �V���h�E�}�b�v���g�p���邩
			float strength = 0.4f;					// �e�F�̋���
			float bias = 0.01f;						// �V���h�E�o�C�A�X
			float normal_bias = 0.01f;				// �@�������̃V���h�E�o�C�A�X
			float shadow_size;						// �V���h�E�}�b�v�̃T�C�Y
			float inv_shadow_size;					// �V���h�E�}�b�v�̃T�C�Y�̋t��
			float dummy1;
			float dummy2;
			float dummy3;

			void operator=(const std::shared_ptr<Light>& light);
		};
		struct PointLight
		{
			MATRIX LVPs[2] = { MATRIX::Identity };	// ���C�g�r���[�v���W�F�N�V����
			MATRIX light_views[2] = { MATRIX::Identity };		// ���C�g�r���[
			MATRIX light_projections[2] = { MATRIX::Identity };	// ���C�g�v���W�F�N�V����
			VECTOR4 pos = VECTOR4::Zero;			// ���C�g�ʒu
			VECTOR4 color = VECTOR4::Zero;			// ���C�g�F
			VECTOR3 attenuation = VECTOR3::One;		// �����W��(x:�萔 y:�ꎟ z:��)
			float range = 0.0f;						// �͈�(0�ȉ��Ŗ����ȃ��C�g)
			BOOL use_shadow_map = FALSE;			// �V���h�E�}�b�v���g�p���邩
			float strength = 0.4f;					// �e�F�̋���
			float bias = 0.01f;						// �V���h�E�o�C�A�X
			float normal_bias = 0.01f;				// �@�������̃V���h�E�o�C�A�X
			float shadow_size[2];					// �V���h�E�}�b�v�̃T�C�Y
			float inv_shadow_size[2];				// �V���h�E�}�b�v�̃T�C�Y�̋t��

			void operator=(const std::shared_ptr<Light>& light);
		};

		// CBuffer�f�[�^�p�\����
		struct CB_Light
		{
			DirectionalLight directional;
			SpotLight spots[MAX_SPOTLIGHT];
			PointLight points[MAX_POINTLIGHT];
		};

	private:
		// ==============================================
		// private methods
		// ==============================================
		LightManager() = default;
		virtual ~LightManager() = default;

		bool Initialize();
		void Finalize();

		// ���݃A�N�e�B�u�ȃ��C�g���^�C�v���ƂɃ\�[�g����
		// TODO : �L���ȃ��C�g�����ő吔�𒴂��Ă���ꍇ�̓J��������̋����ɂ���đI������
		void SortLightType();

		// �J�������󂯎���ċ�������L���ȓ��I���C�g�̑I�����s��
		void OnApplyParameter();

		// ���C�g���ƂɍX�V�̌v�Z������
		void OnCalculateSceneLight(const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera);
		// �e��`�悷��I�u�W�F�N�g�̌v�Z�����C�g���ƂɎ��s����
		void OnCalculateCaster(const std::list<std::shared_ptr<Light>>& light_list, const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera);
		// ���C�g���Ƃɉe��`�悷��
		void OnRenderShadow(ID3D11DeviceContext* context);
		// ���C�g���Ƃɉe��`�悷�邩�`�F�b�N���A���s����
		void OnRenderShadowCheckAndRender(const std::list<std::shared_ptr<Light>>& light_list, ID3D11DeviceContext* context);
		// �p�C�v���C���ɃZ�b�g����
		void OnSetPipeline(ID3D11DeviceContext* context);
		// �p�C�v���C���ɃZ�b�g���Ƀ��C�g�}�X�N�ƃV���h�E�}�b�v���Z�b�g����
		void SetTexture(ID3D11DeviceContext* context, const std::list<std::shared_ptr<Light>>& light_list);
		void OnSetTextureDirectional(ID3D11DeviceContext* context, const std::shared_ptr<Light>& light, int index);
		void OnSetTextureSpot(ID3D11DeviceContext* context, const std::shared_ptr<Light>& light, int index);
		void OnSetTexturePoint(ID3D11DeviceContext* context, const std::shared_ptr<Light>& light, int index);

		// ==============================================
		// private variables
		// ==============================================
		// Singleton�C���X�^���X
		static LightManager* m_instance;

		// ���݂��郉�C�g���X�g
		std::unordered_map<GameObjectID, std::shared_ptr<Light>> m_light_list;
		// ���C�g�^�C�v���Ƃ̃\�[�g���X�g
		std::list<std::shared_ptr<Light>> m_directional_list;
		std::list<std::shared_ptr<Light>> m_spot_list;
		std::list<std::shared_ptr<Light>> m_point_list;

		// Light CBuffer
		ConstantBuffer m_cb_light;
		// Shadow�C���X�^���X�o�b�t�@
		VertexBuffer m_instance_buffer;
		int m_current_buffer_size = 0;

		// �V���h�E�L���X�^�[�v�Z�p�X���b�h
		static constexpr int MAX_THREAD_NUM = 1 + MAX_POINTLIGHT + MAX_SPOTLIGHT;
		ThreadPool m_calculate_shadow_caster_thread = ThreadPool(MAX_THREAD_NUM);
		std::vector<std::future<int>> m_thread_result = std::vector< std::future<int>>();
		
	};

}// namespace TKGEngine

