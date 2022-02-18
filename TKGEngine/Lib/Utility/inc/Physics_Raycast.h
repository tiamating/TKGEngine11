#pragma once

#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/myfunc_math.h"

#include "Application/inc/ProjectSetting.h"

#include <vector>
#include <memory>


// ==============================================
// 前方宣言
// ==============================================
class btConvexShape;
class btCollisionObject;

namespace TKGEngine
{
	class Collider;
	class RigidBody;
}

namespace TKGEngine
{
	/// <summary>
	/// 衝突判定用関数群
	/// </summary>
	class Physics
	{
	public:
		// レイキャストによる情報を得るための構造体
		struct RaycastHit
		{
			// 衝突したワールド座標
			VECTOR3 position = VECTOR3::Zero;
			// 衝突したワールド法線
			VECTOR3 normal = VECTOR3::Zero;
			// 衝突位置までの距離
			float distance = 0.0f;
			float hit_fraction = 0.0f;
			// 衝突した相手
			std::shared_ptr<Collider> collider = nullptr;
			// 剛体なら取得可能
			std::shared_ptr<RigidBody> rigidbody = nullptr;
		};

#pragma region Raycast
		/// <summary>
		/// 始めにレイと衝突したオブジェクトを得る
		/// </summary>
		/// <param name="origin">ワールド座標でのレイの始点</param>
		/// <param name="direction">レイの方向</param>
		/// <param name="max_distance">レイの衝突を検知する最大距離</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool Raycast(
			const VECTOR3& origin,
			const VECTOR3& direction,
			const float max_distance,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// 始めにレイと衝突したオブジェクトを得る
		/// </summary>
		/// <param name="origin">ワールド座標でのレイの始点</param>
		/// <param name="direction">レイの方向</param>
		/// <param name="hit_info">戻り値trueの時に、衝突したコライダーの情報</param>
		/// <param name="max_distance">レイの衝突を検知する最大距離</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool Raycast(
			const VECTOR3& origin,
			const VECTOR3& direction,
			RaycastHit& hit_info,
			const float max_distance,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// レイと衝突したすべてのオブジェクトを距離ソートした情報の配列を得る
		/// </summary>
		/// <param name="origin">ワールド座標でのレイの始点</param>
		/// <param name="direction">レイの方向</param>
		/// <param name="hit_infos">戻り値trueの時に、衝突したコライダーの情報</param>
		/// <param name="max_distance">レイの衝突を検知する最大距離</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool RaycastAll(
			const VECTOR3& origin,
			const VECTOR3& direction,
			std::vector<RaycastHit>& hit_infos,
			const float max_distance,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Raycast

#pragma region Line
		/// <summary>
		/// 線分と衝突するか
		/// </summary>
		/// <param name="start">ワールド座標でのレイの始点</param>
		/// <param name="end">ワールド座標でのレイの終点</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool Linecast(
			const VECTOR3& start,
			const VECTOR3& end,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// 線分と衝突するか
		/// </summary>
		/// <param name="start">ワールド座標でのレイの始点</param>
		/// <param name="end">ワールド座標でのレイの終点</param>
		/// <param name="hit_info">戻り値trueの時に、衝突したコライダーの情報</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool Linecast(
			const VECTOR3& start,
			const VECTOR3& end,
			RaycastHit& hit_info,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Line

#pragma region Sphere
		/// <summary>
		/// 球を移動させた時に衝突するか調べる
		/// </summary>
		/// <param name="origin">ワールド座標での球の始点</param>
		/// <param name="radius">球の半径</param>
		/// <param name="direction">球の方向</param>
		/// <param name="max_distance">スイープの最大の長さ</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="ccd_penetration">衝突したことにならないめり込み量</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool SphereCast(
			const VECTOR3& origin,
			const float radius,
			const VECTOR3& direction,
			const float max_distance,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// 球を移動させた時に衝突するか調べる
		/// </summary>
		/// <param name="origin">ワールド座標での球の始点</param>
		/// <param name="radius">球の半径</param>
		/// <param name="direction">球の方向</param>
		/// <param name="max_distance">スイープの最大の長さ</param>
		/// <param name="hit_info">戻り値trueの時に、衝突したコライダーの情報</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="ccd_penetration">衝突したことにならないめり込み量</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool SphereCast(
			const VECTOR3& origin,
			const float radius,
			const VECTOR3& direction,
			const float max_distance,
			RaycastHit& hit_info,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// 指定した位置に球を生成してヒットするか調べる
		/// </summary>
		/// <param name="origin">ワールド座標での球の始点</param>
		/// <param name="radius">球の半径</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="threshold">衝突した判定をとる距離の閾値</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool CheckSphere(
			const VECTOR3& origin,
			const float radius,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// 球と重なっているコライダーを取得する
		/// </summary>
		/// <param name="origin">ワールド座標での球の始点</param>
		/// <param name="radius">球の半径</param>
		/// <param name="hit_infos">戻り値trueの時に、衝突したコライダーの情報</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="threshold">衝突した判定をとる距離の閾値</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool OverlapSphere(
			const VECTOR3& origin,
			const float radius,
			std::vector<RaycastHit>& hit_infos,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Sphere

#pragma region Box
		/// <summary>
		/// Boxを移動させた時に衝突するか調べる
		/// </summary>
		/// <param name="start_center">ボックスの始点中心</param>
		/// <param name="start_rot">ボックスの始点姿勢</param>
		/// <param name="end_center">ボックスの終点中心</param>
		/// <param name="end_rot">ボックスの終点姿勢</param>
		/// <param name="half_extents">ボックスの半分範囲</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="ccd_penetration">衝突したことにならないめり込み量</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool BoxCast(
			const VECTOR3& start_center,
			const Quaternion& start_rot,
			const VECTOR3& end_center,
			const Quaternion& end_rot,
			const VECTOR3& half_extents,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// Boxを移動させた時に衝突するか調べる
		/// </summary>
		/// <param name="start_center">ボックスの始点中心</param>
		/// <param name="start_rot">ボックスの始点姿勢</param>
		/// <param name="end_center">ボックスの終点中心</param>
		/// <param name="end_rot">ボックスの終点姿勢</param>
		/// <param name="half_extents">ボックスの半分範囲</param>
		/// <param name="hit_info">戻り値trueの時に、衝突したコライダーの情報</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="ccd_penetration">衝突したことにならないめり込み量</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool BoxCast(
			const VECTOR3& start_center,
			const Quaternion& start_rot,
			const VECTOR3& end_center,
			const Quaternion& end_rot,
			const VECTOR3& half_extents,
			RaycastHit& hit_info,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// 指定した位置にBoxを生成してヒットするか調べる
		/// </summary>
		/// <param name="center">ボックスの中心</param>
		/// <param name="rot">ボックスの姿勢</param>
		/// <param name="half_extents">ボックスの半分範囲</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="threshold">衝突した判定をとる距離の閾値</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool CheckBox(
			const VECTOR3& center,
			const Quaternion& rot,
			const VECTOR3& half_extents,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// Boxと重なっているコライダーを取得する
		/// </summary>
		/// <param name="center">ボックスの中心</param>
		/// <param name="rot">ボックスの姿勢</param>
		/// <param name="half_extents">ボックスの半分範囲</param>
		/// <param name="hit_infos">戻り値trueの時に、衝突したコライダーの情報</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="threshold">衝突した判定をとる距離の閾値</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool OverlapBox(
			const VECTOR3& center,
			const Quaternion& rot,
			const VECTOR3& half_extents,
			std::vector<RaycastHit>& hit_infos,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Box

#pragma region Capsule
		/// <summary>
		/// Capsuleを移動させた時に衝突するか調べる
		/// </summary>
		/// <param name="point_1">カプセルにある球体の中心</param>
		/// <param name="point_2">カプセルにある球体の中心</param>
		/// <param name="radius">カプセルの半径</param>
		/// <param name="direction">カプセルの通過方向</param>
		/// <param name="max_distance">スイープの最大の長さ</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="ccd_penetration">衝突したことにならないめり込み量</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool CapsuleCast(
			const VECTOR3& point_1,
			const VECTOR3& point_2,
			const float radius,
			const VECTOR3& direction,
			const float max_distance,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// Capsuleを移動させた時に衝突するか調べる
		/// </summary>
		/// <param name="point_1">カプセルにある球体の中心</param>
		/// <param name="point_2">カプセルにある球体の中心</param>
		/// <param name="radius">カプセルの半径</param>
		/// <param name="direction">カプセルの通過方向</param>
		/// <param name="max_distance">スイープの最大の長さ</param>
		/// <param name="hit_info">戻り値trueの時に、衝突したコライダーの情報</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="ccd_penetration">衝突したことにならないめり込み量</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool CapsuleCast(
			const VECTOR3& point_1,
			const VECTOR3& point_2,
			const float radius,
			const VECTOR3& direction,
			const float max_distance,
			RaycastHit& hit_info,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// 指定した位置にCapsuleを生成してヒットするか調べる
		/// </summary>
		/// <param name="point_1">カプセルにある球体の中心</param>
		/// <param name="point_2">カプセルにある球体の中心</param>
		/// <param name="radius">カプセルの半径</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="threshold">衝突した判定をとる距離の閾値</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool CheckCapsule(
			const VECTOR3& point_1,
			const VECTOR3& point_2,
			const float radius,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// Capsuleと重なっているコライダーを取得する
		/// </summary>
		/// <param name="point_1">カプセルにある球体の中心</param>
		/// <param name="point_2">カプセルにある球体の中心</param>
		/// <param name="radius">カプセルの半径</param>
		/// <param name="hit_infos">戻り値trueの時に、衝突したコライダーの情報</param>
		/// <param name="is_hit_trigger">トリガーも検索するか</param>
		/// <param name="threshold">衝突した判定をとる距離の閾値</param>
		/// <param name="layer_mask">衝突するレイヤー</param>
		/// <returns>collide : true</returns>
		static bool OverlapCapsule(
			const VECTOR3& point_1,
			const VECTOR3& point_2,
			const float radius,
			std::vector<RaycastHit>& hit_infos,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Capsule

		private:
			// 最近傍の凸包衝突情報を取得する
			static bool ClosestConvexCast(
				const btConvexShape* shape,
				const VECTOR3& start_pos,
				const Quaternion& start_rot,
				const VECTOR3& end_pos,
				const Quaternion& end_rot,
				const bool is_hit_trigger,
				const float ccd_penetration,
				const int layer_mask
			);
			// 最近傍の凸包衝突情報を取得する
			static bool ClosestConvexCast(
				RaycastHit& hit_info,
				const btConvexShape* shape,
				const VECTOR3& start_pos,
				const Quaternion& start_rot,
				const VECTOR3& end_pos,
				const Quaternion& end_rot, 
				const bool is_hit_trigger,
				const float ccd_penetration,
				const int layer_mask
			);

			// 凸包が衝突するかどうか
			static bool CheckConvex(
				btCollisionObject* col_obj,
				const bool is_hit_trigger,
				const float threshold,
				const int layer_mask
			);

			// 重なるオブジェクトの情報を返す
			static bool OverlapConvex(
				std::vector<RaycastHit>& hit_infos,
				btCollisionObject* col_obj,
				const bool is_hit_trigger,
				const float threshold,
				const int layer_mask
			);

			// 衝突位置にデバッグ描画を行うときに呼ぶ関数
			static void RenderHitShape(btConvexShape* shape, const VECTOR3& pos, const Quaternion& rot);

			// Debug描画色
			static const VECTOR3 DEBUG_COLOR;
			// 衝突時Debug描画色
			static const VECTOR3 DEBUG_HIT_COLOR;
			// 衝突点Debug描画色
			static const VECTOR3 DEBUG_CONTACT_POINT_COLOR;
	};


}// namespace TKGEngine