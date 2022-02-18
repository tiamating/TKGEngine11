#pragma once

namespace TKGEngine::SystemSetting
{
	// 起動時に新規シーンを開くか
	static constexpr bool NEW_SCENE_ON_BEGIN = false;
	// 起動時にロードするシーンファイル
	static constexpr const char* START_SCENE_FILEPATH = "./Asset/Scenes/PlayerScene.scene";

	// ウィンドウサイズ
	static constexpr unsigned WINDOW_WIDTH = 1280;
	static constexpr unsigned WINDOW_HEIGHT = 720;
	// フルスクリーンモードにするか
	static constexpr bool WINDOW_FULLSCREEN_MODE = false;
	// ウィンドウを最大化して表示するか
	static constexpr bool WINDOW_SHOW_MAXIMIZE = false;

}// namespace TKGEngine::SystemSetting