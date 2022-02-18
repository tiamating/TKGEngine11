#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Application/Objects/inc/Light_Defined.h"

struct ID3D11DeviceContext;

namespace TKGEngine
{
	struct MainData;
	class ICamera;
	class Frustum;

	/// <summary>
	/// Light��ނɉ������V���h�E�̌v�Z������񋟂���x�[�X�N���X
	/// </summary>
	class LightShadowBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		LightShadowBase() = default;
		virtual ~LightShadowBase() = default;
		LightShadowBase(const LightShadowBase&) = delete;
		LightShadowBase& operator=(const LightShadowBase&) = delete;

		/// <summary>
		/// �t���X�^�����ɉe�𗎂Ƃ��I�u�W�F�N�g������
		/// </summary>
		/// <param name="scene_objects">�e��`�悷��I�u�W�F�N�g���X�g</param>
		/// <param name="object_num">�I�u�W�F�N�g���X�g���̃I�u�W�F�N�g��</param>
		/// <param name="camera">�`�撆�̃J�����̎Q��</param>
		/// <param name="data">�e���o�����C�g�̏��</param>
		/// <param name="frustum">�t���X�^��</param>
		/// <param name="test_accurate">�e�X�g�̐��m��</param>
		/// <param name="casters">�e�𗎂Ƃ��I�u�W�F�N�g���X�g</param>
		virtual void FindCasters(
			const std::vector<MainData>& scene_objects,
			const int object_num,
			const std::shared_ptr<ICamera>& camera,
			const LightShadowData& data,
			const Frustum& frustum,
			bool test_accurate,
			std::vector<MainData>& casters
		) = 0;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

		
	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		/* nothing */


	};




}
