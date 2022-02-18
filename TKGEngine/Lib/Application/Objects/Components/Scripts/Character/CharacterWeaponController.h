#pragma once
#include "Components/inc/CMonoBehaviour.h"

namespace TKGEngine
{
	class IWeaponBase;
	class CameraController;
	class Animator;

	class CharacterWeaponController
		: public MonoBehaviour
	{
	public:
		CharacterWeaponController() = default;
		virtual ~CharacterWeaponController() override = default;
		CharacterWeaponController(const CharacterWeaponController&) = delete;
		CharacterWeaponController& operator=(const CharacterWeaponController&) = delete;

	public:
		// 武器の種類一覧
		enum class WeaponType
		{
			HandGun,

			Max_WeaponType
		};

		// エイム状態
		enum class AimState
		{
			Hold,	// 構え
			Low,	// 武器保持

			Max_AimState
		};

		//////////////////////////////////
		// functions
		//////////////////////////////////

		// 所持している武器の登録
		void RegisterWeapon(WeaponType type, const std::weak_ptr<IWeaponBase>& weapon);

		// 入力によるエイム状態の更新
		void ApplyInputAimUpdate();
		// 入力による射撃
		void ApplyInputShotUpdate();
		// 入力によるリロード
		void ApplyInputReloadUpdate();
		// リロードキャンセル
		void CancelReload();


		// エイム状態
		void SetAimState(AimState state) { m_aim_state = state; }
		AimState GetAimState() const { return m_aim_state; }
		// エイム状態の重み
		float GetAimWeight() const { return m_aim_weight; }
		float GetAimUpperWeight() const { return m_aim_upper_weight; }


	private:
		/////////////////////////////////////////////////////////////////////////
		// 使用するものをコメント解除して使用
		/////////////////////////////////////////////////////////////////////////
		//virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		//virtual void LateUpdate() override;
		//virtual void OnEnable() override;
		//virtual void OnDisable() override;
		//virtual void OnDestroyed() override;


		//////////////////////////////////
		// functions
		//////////////////////////////////
		// エイム状態の更新
		void AimUpdate();


		//////////////////////////////////
		// variables
		//////////////////////////////////

		// 参照
		SWPtr<CameraController> m_camera;
		SWPtr<Animator> m_animator;

		// 入力閾値
		float m_left_trigger_threshold = 0.5f;
		float m_right_trigger_threshold = 0.9f;
		// 入力フラグ
		bool m_right_trigger_entered = false;

		// 武器
		WeaponType m_current_weapon_type = WeaponType::HandGun;	// 現在装備中の武器
		std::vector<SWPtr<IWeaponBase>> m_weapons = std::vector<SWPtr<IWeaponBase>>(static_cast<int>(WeaponType::Max_WeaponType));	// 武器の参照

		// エイム用変数
		AimState m_aim_state = AimState::Low;
		float m_aim_weight = 0.0f;
		float m_aim_upper_weight = 0.0f;
		float m_aim_transition_time = 0.2f;	// 何秒で遷移するか


	public:
		//////////////////////////////////
		// アニメーションイベント
		//////////////////////////////////

		// リロード開始
		void OnBeginReload();
		// 弾込め完了
		void OnCompleteReload();
		// リロード終了
		void OnEndReload();


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
CEREAL_CLASS_VERSION(TKGEngine::CharacterWeaponController, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::CharacterWeaponController, "TKGEngine::CharacterWeaponController")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::CharacterWeaponController)
