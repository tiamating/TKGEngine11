#pragma once
#include "Components/inc/CMonoBehaviour.h"

namespace TKGEngine
{
	class GoalManager
		: public TKGEngine::MonoBehaviour
	{
	public:
		GoalManager() = default;
		virtual ~GoalManager() override = default;
		GoalManager(const GoalManager&) = delete;
		GoalManager& operator=(const GoalManager&) = delete;

	private:
		/////////////////////////////////////////////////////////////////////////
		// 使用するものをコメント解除して使用
		/////////////////////////////////////////////////////////////////////////
		//virtual void Awake() override;
		//virtual void Start() override;
		virtual void Update() override;
		//virtual void LateUpdate() override;
		//virtual void OnEnable() override;
		//virtual void OnDisable() override;
		//virtual void OnDestroyed() override;

		/////////////////////////////////////////////////////////////////////////
		// コリジョン用メッセージ
		/////////////////////////////////////////////////////////////////////////
		// 物体同士の衝突時
		//virtual void OnCollisionEnter(std::shared_ptr<TKGEngine::Collider>& collider) override;
		//virtual void OnCollisionStay(std::shared_ptr<TKGEngine::Collider>& collider) override;
		//virtual void OnCollisionExit(std::shared_ptr<TKGEngine::Collider>& collider) override;
		// トリガーオブジェクトの衝突時
		virtual void OnTriggerEnter(std::shared_ptr<TKGEngine::Collider>& collider) override;
		//virtual void OnTriggerStay(std::shared_ptr<TKGEngine::Collider>& collider) override;
		//virtual void OnTriggerExit(std::shared_ptr<TKGEngine::Collider>& collider) override;

	private:
		//////////////////////////////////
		// functions
		//////////////////////////////////



		//////////////////////////////////
		// variables
		//////////////////////////////////
		bool m_begin_loading = false;

		// ゴールに触れてからロード開始までの時間
		float m_stop_time = 1.0f;
		float m_stop_timer = 0.0f;

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
CEREAL_CLASS_VERSION(TKGEngine::GoalManager, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::GoalManager, "TKGEngine::GoalManager")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::GoalManager)
