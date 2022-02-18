#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterSubState.h"

namespace TKGEngine
{
	class PlayerActionState;
	class PlayerController;
	class CharacterMoveController;
	class CameraController;
	class Animator;


	class PlayerSlideSubState
		: public MonoBehaviour
		, public ICharacterSubState
	{
	public:
		PlayerSlideSubState() = default;
		virtual ~PlayerSlideSubState() override = default;
		PlayerSlideSubState(const PlayerSlideSubState&) = delete;
		PlayerSlideSubState& operator=(const PlayerSlideSubState&) = delete;

	private:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void OnTriggerEnter(std::shared_ptr<Collider>& collider) override;
		virtual void OnTriggerExit(std::shared_ptr<Collider>& collider) override;

		virtual void Enter() override;
		virtual void BeforeExecute() override;
		virtual void AfterExecute() override;
		virtual void Execute() override;
		virtual void Exit() override;

		//////////////////////////////////
		// functions
		//////////////////////////////////
		// 速度の更新
		void UpdateVelocity();
		// アニメーションコントローラにセット
		void ApplyToParameter();

		// カバーに遷移可能かチェックして可能なら遷移を実行
		bool CheckCoverTransition();
		// カバーに遷移
		void TransitionCover(const Physics::RaycastHit& ray_data, const VECTOR3& ray_direction);

		//////////////////////////////////
		// variables
		//////////////////////////////////
		// 参照
		SWPtr<PlayerActionState> m_parent_state;
		SWPtr<PlayerController> m_player;
		SWPtr<CharacterMoveController> m_mover;
		SWPtr<CameraController> m_camera;
		SWPtr<Animator> m_animator;
		
		// 初速度
		float m_initial_speed = 6.0f;
		// 減速値
		float m_brake = 4.0f;
		// スライディング終了閾値
		float m_slide_threshold_speed = 2.5f;
		// 許容する入力と進行方向の差分角度
		float m_allow_different_angle = 100.0f;

		// 現在の速度
		float m_current_speed = 0.0f;
		// スライディング実行中フラグ
		bool m_on_sliding = false;
		// スライディングから起き上がったフラグ
		bool m_on_end_slide_end = false;
		// カバーに遷移する
		bool m_on_transition_cover = false;
		
		// カバー判定
		bool m_can_cover = false;
		float m_max_cover_different_angle = 70.0f;
		VECTOR3 m_block_position = VECTOR3::Zero;

	public:
		//////////////////////////////////
		// アニメーションイベント
		//////////////////////////////////
		// スライディングの起き上がり時の処理
		void OnEndSlideEnd();


	private:
		/////////////////////////////////////////////////////////////////////////
		// Serialize設定
		/////////////////////////////////////////////////////////////////////////
		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<TKGEngine::MonoBehaviour>(this)
				);
			}
		}

		/////////////////////////////////////////////////////////////////////////
		// GUI設定
		/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI
	};
}

/////////////////////////////////////////////////////////////////////////
// Serialize登録
/////////////////////////////////////////////////////////////////////////
CEREAL_CLASS_VERSION(TKGEngine::PlayerSlideSubState, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::PlayerSlideSubState, "TKGEngine::PlayerSlideSubState")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::PlayerSlideSubState)
