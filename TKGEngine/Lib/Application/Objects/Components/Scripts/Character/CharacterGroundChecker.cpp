#include "CharacterGroundChecker.h"

REGISTERCOMPONENT(TKGEngine::CharacterGroundChecker);

namespace TKGEngine
{
	void CharacterGroundChecker::Awake()
	{
		// 更新をキャラクターより先に行うようにする
		Priority(-10);
	}

	void CharacterGroundChecker::Update()
	{
		// レイを飛ばして接地判定を行う
		const VECTOR3 origin = GetTransform()->Position() + VECTOR3::Down * m_ray_offset;
		m_is_ground = Physics::Raycast(origin, VECTOR3::Down, m_ray_distance, false, static_cast<int>(Layer::Stage));
	}

	bool CharacterGroundChecker::IsGround() const
	{
		return m_is_ground;
	}

	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void CharacterGroundChecker::OnGUI()
	{
	}
#endif
}
