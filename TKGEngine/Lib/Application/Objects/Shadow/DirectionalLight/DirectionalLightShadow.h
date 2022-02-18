#pragma once

#include "Application/Objects/Shadow/LightShadowBase.h"

namespace TKGEngine
{
	/// <summary>
	/// ���s�����̉e�v�Z�����N���X
	/// </summary>
	class DirectionalLightShadow
		: public LightShadowBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		DirectionalLightShadow() = default;
		virtual ~DirectionalLightShadow() = default;
		DirectionalLightShadow(const DirectionalLightShadow&) = delete;
		DirectionalLightShadow& operator=(const DirectionalLightShadow&) = delete;

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
			const std::vector<MainData>&scene_objects,
			const int object_num,
			const std::shared_ptr<ICamera>&camera,
			const LightShadowData& data,
			const Frustum& frustum,
			bool test_accurate,
			std::vector<MainData>& casters
		) override;
		

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================


		// ==============================================
		// private variables
		// ==============================================
		/* nothing */

	};


}
