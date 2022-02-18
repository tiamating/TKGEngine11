#pragma once

#include "CBehaviour.h"
#include "Managers/MonoBehaviourManager.h"
#include "Managers/SceneManager.h"

#include "Application/Objects/inc/IGameObject.h"
#include "Components/inc/CTransform.h"
#include "Components/inc/CCollider.h"

#include "Systems/inc/LogSystem.h"
#include "Systems/inc/IWindow.h"
#include "Systems/inc/ITime.h"
#include "Systems/inc/IInput.h"
#include "Systems/inc/IAudio.h"
#include "Systems/inc/IGUI.h"

#include "Utility/inc/myfunc_easing.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/Physics_Raycast.h"
#include "Utility/inc/random.h"
#include "Utility/inc/template_SWPtr.h"


namespace TKGEngine
{
	//////////////////////////////////////////////////////
	///
	/// 派生先.cppでREGISTERCOMPONENT(TYPE_NAME)をすること
	///
	//////////////////////////////////////////////////////
	
	/// <summary>
	/// This component manage event function.
	/// </summary>
	class MonoBehaviour
		: public Behaviour
	{
	public:
		/// <summary>
		/// イベント関数で実行しているイベントの種類
		/// </summary>
		enum class MonoBehaviourEventType
		{
			OnAwake,
			OnEnable,
			OnStart
		};
		// ==============================================
		// public methods
		// ==============================================
		MonoBehaviour(const MonoBehaviour&) = delete;
		MonoBehaviour& operator=(const MonoBehaviour&) = delete;

		MonoBehaviour();
		virtual ~MonoBehaviour();


		//////////////////////////////////////////
		// 派生先ではprivateで宣言する
		//////////////////////////////////////////
#ifdef USE_IMGUI
		virtual void OnGUI() override;
#endif // USE_IMGUI

		virtual void Awake() {}
		virtual void Start() {}
		virtual void Update() {}
		virtual void LateUpdate() {}

		virtual void OnEnable() {}
		virtual void OnDisable() {}

		virtual void OnDestroyed() {}

		// 衝突時
		virtual void OnCollisionEnter(std::shared_ptr<Collider>& collider) {}
		virtual void OnCollisionStay(std::shared_ptr<Collider>& collider) {}
		virtual void OnCollisionExit(std::shared_ptr<Collider>& collider) {}

		virtual void OnTriggerEnter(std::shared_ptr<Collider>& collider) {}
		virtual void OnTriggerStay(std::shared_ptr<Collider>& collider) {}
		virtual void OnTriggerExit(std::shared_ptr<Collider>& collider) {}
		
		// https://qiita.com/Hirai0827/items/4946ee4b8b52d6f1da27
		//virtual void OnRenderImage(ColorTarget& src, const ColorTarget& dst) {}

		//////////////////////////////////////////

		inline int Priority() const;
		inline void Priority(int num);

		void CheckMonoBehaviourState(MonoBehaviourEventType type);
		inline bool IsCalledAwake() const;
		inline bool CanCallEveryFrameUpdate() const;

		inline void SetStoreId(int id);
		inline int GetStoreId() const;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	protected:
		// ==============================================
		// protected variables
		// ==============================================
		// 毎フレーム更新関数を呼び出すかどうか
		bool m_can_call_every_frame_update = true;


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_can_call_every_frame_update),
					CEREAL_NVP(m_priority)
				);
			}
		}

		/// <summary>
		/// イベント関数で実行するのを待機している場所を保持する
		/// </summary>
		enum class MonoBehaviourStandByState
		{
			BeforeAwake = 0,
			BeforeEnable,
			BeforeStart,
			AllReady
		};

		// ==============================================
		// private methods
		// ==============================================
		void OnCreate(const std::shared_ptr<Object>& p_self) override;
		void OnDestroy() override;

		// Behaviour
		void OnEnabled() final;
		void OnDisabled() final;
		// ~Behaviour


		// ==============================================
		// private variables
		// ==============================================
		int m_priority = 0;	//!< 実行優先度(小さいほど先)

		int m_store_list_id = 0;
		MonoBehaviourStandByState m_state = MonoBehaviourStandByState::BeforeAwake;
	};

	// ------------------------------------------------
	// inline 
	// ------------------------------------------------
	inline int MonoBehaviour::Priority() const
	{
		return m_priority;
	}

	inline void MonoBehaviour::Priority(int num)
	{
		m_priority = num;
	}

	inline bool MonoBehaviour::IsCalledAwake() const
	{
		return m_state == MonoBehaviourStandByState::BeforeAwake ? false : true;
	}

	inline bool MonoBehaviour::CanCallEveryFrameUpdate() const
	{
		return m_can_call_every_frame_update;
	}

	inline void MonoBehaviour::SetStoreId(int id)
	{
		m_store_list_id = id;
	}

	inline int MonoBehaviour::GetStoreId() const
	{
		return m_store_list_id;
	}

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::MonoBehaviour, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::MonoBehaviour, "TKGEngine::MonoBehaviour")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Behaviour, TKGEngine::MonoBehaviour)