
#include "../../inc/IInputSystem.h"
#include "../../inc/IInput.h"

#include "Systems/inc/IWindow.h"
#include "Systems/inc/IGUI.h"

#include "../../DirectXTK/Inc/Keyboard.h"
#include "../../DirectXTK/Inc/GamePad.h"
#include "../../DirectXTK/Inc/Mouse.h"
#pragma comment(lib, "DirectXTK.lib")

#include <Windows.h>

#include <memory>

namespace TKGEngine::Input
{
	/// ========================================================
	/// @class	InputSystem
	/// @brief	Input system
	/// 
	/// ========================================================
	class InputSystem
		: public IInputSystem,
		public IInput
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		InputSystem();
		virtual ~InputSystem();
		InputSystem(InputSystem&&) = default;
		InputSystem(const InputSystem&) = delete;
		InputSystem& operator=(const InputSystem&) = delete;

		// ISystem
		bool OnInit() override;
		void OnTerm() override;
		void OnFrameBegin(const FrameEventArgs& args) override;
		void OnFrameRender(const FrameEventArgs& args) override;
		void OnFrameEnd(const FrameEventArgs& args) override;
		void OnResize(const ResizeEventArgs& args) override;
		// ~ISystem


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private structs
		// ==============================================
		// 入力状態用構造体
		struct InputState
		{
			bool STATE;
			bool PRESSED;
			bool RELEASED;
		};

		// GamePadの振動用構造体
		struct PadVibrate
		{
			bool vibration = false;	// 現在振動しているかどうか
			float timer = 0.0f;		// 残り振動時間[s]
		};


		// ==============================================
		// private methods
		// ==============================================
		// IInput
		// KeyBoard
		bool GetKey(Input::Key key) const override;
		bool GetKeyUp(Input::Key key) const override;
		bool GetKeyDown(Input::Key key) const override;
		// ~KeyBoard

		// GamePad
		bool GetPad(int player, Input::GamePad button) const override;
		bool GetPadUp(int player, Input::GamePad button) const override;
		bool GetPadDown(int player, Input::GamePad button) const override;

		float GetPadStickRX(int player) const override;
		float GetPadStickRY(int player) const override;
		float GetPadStickLX(int player) const override;
		float GetPadStickLY(int player) const override;
		float GetPadRTrigger(int player) const override;
		float GetPadLTrigger(int player) const override;
		bool SetVibration(
			int player, float seconds,
			float left_motor, float right_motor,
			float left_trigger = 0, float right_trigger = 0
		) override;
		bool StopVibration(int player) override;
		bool IsConnected(int player) const override;
		// ~GamePad

		// Mouse
		VECTOR2 GetMousePos() const override;
		int GetMousePosX() const override;
		int GetMousePosY() const override;
		int GetMouseWheel() const override;

		void SetMousePos(const VECTOR2& point) override;
		void SetMousePos(int x, int y) override;
		void SetMousePosX(int x) override;
		void SetMousePosY(int y) override;

		bool GetMouseButton(Input::Mouse button) const override;
		bool GetMouseButtonUp(Input::Mouse button) const override;
		bool GetMouseButtonDown(Input::Mouse button) const override;

		bool GetMouseVisible() const override;
		void SetMouseVisible(bool is_visible) override;
		// ~Mouse
		// ~IInput

		bool StartVibration(
			int player,
			float left_motor, float right_motor,
			float left_trigger = 0, float right_trigger = 0
		);


		// ==============================================
		// private variables
		// ==============================================
		HWND m_hwnd = nullptr;

		// キー状態情報
		InputState m_key_input_state[256/*DirectX::Keyboard::Keys*/] = {};
		InputState m_pad_input_state[GAMEPAD_NUM][static_cast<int>(GamePad::NUM)] = {};
		InputState m_mouse_input_state[static_cast<int>(Mouse::NUM)] = {};

		// デバイス入力トラッカー内の各ボタンへのポインタ(initializeでトラッカー作成後に設定)
		const DirectX::GamePad::ButtonStateTracker::ButtonState* m_pad_button_ptrs[GAMEPAD_NUM][static_cast<int>(GamePad::NUM)/*buttons*/] = {};
		const DirectX::Mouse::ButtonStateTracker::ButtonState* m_mouse_button_ptrs[static_cast<int>(Mouse::NUM)/*buttons*/] = {};

		// ゲームパッドの振動を管理する構造体配列
		PadVibrate m_pad_vibrations[GAMEPAD_NUM];

		// 入力デバイスに関するポインタ
		std::unique_ptr<DirectX::Keyboard> m_keyboard;
		std::unique_ptr<DirectX::GamePad> m_game_pad;
		std::unique_ptr<DirectX::Mouse> m_mouse;
		std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> m_kb_tracker;
		std::unique_ptr<DirectX::GamePad::ButtonStateTracker> m_pad_tracker[GAMEPAD_NUM];
		std::unique_ptr<DirectX::Mouse::ButtonStateTracker> m_mouse_tracker;
		// 入力状態が格納される
		DirectX::Keyboard::State m_kb_state;
		DirectX::GamePad::State m_pad_state[GAMEPAD_NUM];
		DirectX::Mouse::State m_mouse_state;

		// マウスポインタの表示状態
		bool m_is_mouse_visible = true;
	};

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	std::unique_ptr<IInputSystem> IInputSystem::CreateInterface()
	{
		return std::make_unique<InputSystem>();
	}

	InputSystem::InputSystem()
	{
		/* nothing */
	}

	InputSystem::~InputSystem()
	{
		/* nothing */
	}


	bool InputSystem::OnInit()
	{
		m_keyboard = std::make_unique<DirectX::Keyboard>();
		m_game_pad = std::make_unique<DirectX::GamePad>();
		m_mouse = std::make_unique<DirectX::Mouse>();

		m_kb_tracker = std::make_unique<DirectX::Keyboard::KeyboardStateTracker>();
		for (int i = 0; i < GAMEPAD_NUM; ++i)
		{
			m_pad_tracker[i] = std::make_unique<DirectX::GamePad::ButtonStateTracker>();
		}
		m_mouse_tracker = std::make_unique<DirectX::Mouse::ButtonStateTracker>();

		m_hwnd = IWindow::Get().GetHwnd();
		m_mouse->SetWindow(m_hwnd);

		// トラッカー内の各ボタンへのポインタを設定
		// GamePad
		for (int i = 0; i < GAMEPAD_NUM; ++i)
		{
			m_pad_button_ptrs[i][static_cast<int>(GamePad::UP)/*0*/] = &m_pad_tracker[i].get()->dpadUp;				// Up
			m_pad_button_ptrs[i][static_cast<int>(GamePad::DOWN)/*1*/] = &m_pad_tracker[i].get()->dpadDown;			// Down
			m_pad_button_ptrs[i][static_cast<int>(GamePad::RIGHT)/*2*/] = &m_pad_tracker[i].get()->dpadRight;		// Right
			m_pad_button_ptrs[i][static_cast<int>(GamePad::LEFT)/*3*/] = &m_pad_tracker[i].get()->dpadLeft;			// Left
			m_pad_button_ptrs[i][static_cast<int>(GamePad::A)/*4*/] = &m_pad_tracker[i].get()->a;					// A
			m_pad_button_ptrs[i][static_cast<int>(GamePad::B)/*5*/] = &m_pad_tracker[i].get()->b;					// B
			m_pad_button_ptrs[i][static_cast<int>(GamePad::X)/*6*/] = &m_pad_tracker[i].get()->x;					// X
			m_pad_button_ptrs[i][static_cast<int>(GamePad::Y)/*7*/] = &m_pad_tracker[i].get()->y;					// Y
			m_pad_button_ptrs[i][static_cast<int>(GamePad::LSTICK)/*8*/] = &m_pad_tracker[i].get()->leftStick;		// LStick
			m_pad_button_ptrs[i][static_cast<int>(GamePad::RSTICK)/*9*/] = &m_pad_tracker[i].get()->rightStick;		// RStick
			m_pad_button_ptrs[i][static_cast<int>(GamePad::LSHOULDER)/*10*/] = &m_pad_tracker[i].get()->leftShoulder;	// LShoulder
			m_pad_button_ptrs[i][static_cast<int>(GamePad::RSHOULDER)/*11*/] = &m_pad_tracker[i].get()->rightShoulder;	// RShoulder
			m_pad_button_ptrs[i][static_cast<int>(GamePad::BACK)/*12*/] = &m_pad_tracker[i].get()->back;				// Back
			m_pad_button_ptrs[i][static_cast<int>(GamePad::START)/*13*/] = &m_pad_tracker[i].get()->start;				// Start
		}
		// Mouse
		m_mouse_button_ptrs[static_cast<int>(Mouse::LBUTTON)/*0*/] = &m_mouse_tracker.get()->leftButton;	// LButton
		m_mouse_button_ptrs[static_cast<int>(Mouse::MBUTTON)/*1*/] = &m_mouse_tracker.get()->middleButton;	// MButton
		m_mouse_button_ptrs[static_cast<int>(Mouse::RBUTTON)/*2*/] = &m_mouse_tracker.get()->rightButton;	// RButton
		m_mouse_button_ptrs[static_cast<int>(Mouse::X1BUTTON)/*3*/] = &m_mouse_tracker.get()->xButton1;		// XButton1
		m_mouse_button_ptrs[static_cast<int>(Mouse::X2BUTTON)/*4*/] = &m_mouse_tracker.get()->xButton2;		// XButton2

		IInput::Init(this);

		return true;
	}

	void InputSystem::OnTerm()
	{
		IInput::Release();

	}

	void InputSystem::OnFrameBegin(const FrameEventArgs& args)
	{
		// ***********************************************
		// キーボードの更新
		// ***********************************************
		// キーボードの入力状態を取得
		{
			// 入力デバイスの現状態を取得
			m_kb_state = m_keyboard.get()->GetState();
			// トラッカーの更新
			m_kb_tracker.get()->Update(m_kb_state);

			for (int i = 0; i < 256; ++i)
			{
				// 参照先へのポインタとして置いておく
				const auto s = &m_key_input_state[i];
				// 存在しないキーコードは適当な値が入ってしまうが外部で呼び出せないので構わない
				const DirectX::Keyboard::Keys key = static_cast<DirectX::Keyboard::Keys>(i);
				s->STATE = m_kb_state.IsKeyDown(key);
				s->PRESSED = m_kb_tracker->IsKeyPressed(key);
				s->RELEASED = m_kb_tracker->IsKeyReleased(key);
			}
		}

		// ***********************************************
		// マウスの更新
		// ***********************************************
		// マウスの入力状態を取得
		{
			// 入力デバイスの現状態を取得
			m_mouse_state = m_mouse.get()->GetState();
			// トラッカーの更新
			m_mouse_tracker.get()->Update(m_mouse_state);

			for (int i = 0; i < static_cast<int>(Mouse::NUM); ++i)
			{
				// 参照先へのポインタとして置いておく
				const auto s = &m_mouse_input_state[i];
				// トラッカーから状態を取得
				s->STATE = (*m_mouse_button_ptrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::HELD);
				s->PRESSED = (*m_mouse_button_ptrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED);
				s->RELEASED = (*m_mouse_button_ptrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED);
			}
			// マウスホイールをリセットする
			m_mouse.get()->ResetScrollWheelValue();
		}

		// ***********************************************
		// ゲームパッドの更新
		// ***********************************************
		for (int i = 0; i < GAMEPAD_NUM; ++i)
		{
			// 入力デバイスの現状態を取得(gamepad)
			m_pad_state[i] = m_game_pad->GetState(i, DirectX::GamePad::DEAD_ZONE_CIRCULAR);

			// 接続されていなければ処理を飛ばす
			if (!m_pad_state[i].IsConnected())continue;

			// トラッカーの更新(gamepad)
			m_pad_tracker[i].get()->Update(m_pad_state[i]);

			// 振動の時間による更新
			if (m_pad_vibrations[i].vibration == true)
			{
				m_pad_vibrations[i].timer -= args.unscaled_delta_time;
				if (m_pad_vibrations[i].timer <= 0.0f)
				{
					StopVibration(i);
				}
			}

			// ゲームパッドの入力状態を取得
			for (int j = 0; j < static_cast<int>(GamePad::NUM); ++j)
			{
				// 参照先へのポインタとして置いておく
				const auto state = &m_pad_input_state[i][j];

				// トラッカーから状態を取得
				state->STATE = (*m_pad_button_ptrs[i][j] == DirectX::GamePad::ButtonStateTracker::ButtonState::HELD);
				state->PRESSED = (*m_pad_button_ptrs[i][j] == DirectX::GamePad::ButtonStateTracker::ButtonState::PRESSED);
				state->RELEASED = (*m_pad_button_ptrs[i][j] == DirectX::GamePad::ButtonStateTracker::ButtonState::RELEASED);
			}
		}
	}

	void InputSystem::OnFrameRender(const FrameEventArgs& args)
	{
		/* nothing */
	}

	void InputSystem::OnFrameEnd(const FrameEventArgs& args)
	{
		/* nothing*/
	}

	void InputSystem::OnResize(const ResizeEventArgs& args)
	{
		/* nothing */
	}

	// IInput
#pragma region KeyBoard
	bool InputSystem::GetKey(Input::Key key) const
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return false;
#endif // USE_IMGUI
		return m_key_input_state[static_cast<int>(key)].STATE;
	}
	bool InputSystem::GetKeyUp(Input::Key key) const
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return false;
#endif // USE_IMGUI
		return m_key_input_state[static_cast<int>(key)].RELEASED;
	}
	bool InputSystem::GetKeyDown(Input::Key key) const
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return false;
#endif // USE_IMGUI
		return m_key_input_state[static_cast<int>(key)].PRESSED;
	}
#pragma endregion

#pragma region GamePad
	bool InputSystem::GetPad(int player, Input::GamePad button) const
	{
		return m_pad_input_state[player][static_cast<int>(button)].STATE;
	}
	bool InputSystem::GetPadUp(int player, Input::GamePad button) const
	{
		return m_pad_input_state[player][static_cast<int>(button)].RELEASED;
	}
	bool InputSystem::GetPadDown(int player, Input::GamePad button) const
	{
		return m_pad_input_state[player][static_cast<int>(button)].PRESSED;
	}
	float InputSystem::GetPadStickRX(int player) const
	{
		return m_pad_state[player].thumbSticks.rightX;
	}
	float InputSystem::GetPadStickRY(int player) const
	{
		return m_pad_state[player].thumbSticks.rightY;
	}
	float InputSystem::GetPadStickLX(int player) const
	{
		return m_pad_state[player].thumbSticks.leftX;
	}
	float InputSystem::GetPadStickLY(int player) const
	{
		return m_pad_state[player].thumbSticks.leftY;
	}
	float InputSystem::GetPadRTrigger(int player) const
	{
		return m_pad_state[player].triggers.right;
	}
	float InputSystem::GetPadLTrigger(int player) const
	{
		return m_pad_state[player].triggers.left;
	}
	bool InputSystem::SetVibration(
		int player, float seconds,
		float left_motor, float right_motor,
		float left_trigger, float right_trigger
	)
	{
		m_pad_vibrations[player].timer = seconds;
		m_pad_vibrations[player].vibration = true;
		return StartVibration(player, left_motor, right_motor, left_trigger, right_trigger);
	}
	bool InputSystem::StopVibration(int player)
	{
		m_pad_vibrations[player].vibration = false;
		return m_game_pad.get()->SetVibration(player, 0.0f, 0.0f);
	}
	bool InputSystem::IsConnected(int player) const
	{
		return m_pad_state[player].IsConnected();
	}
#pragma endregion

#pragma region Mouse
	VECTOR2 InputSystem::GetMousePos() const
	{
		return VECTOR2(static_cast<float>(m_mouse_state.x), static_cast<float>(m_mouse_state.y));
	}
	int InputSystem::GetMousePosX() const
	{
		return m_mouse_state.x;
	}
	int InputSystem::GetMousePosY() const
	{
		return m_mouse_state.y;
	}
	int InputSystem::GetMouseWheel() const
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return 0;
#endif // USE_IMGUI
		return m_mouse_state.scrollWheelValue;
	}
	void InputSystem::SetMousePos(const VECTOR2& point)
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return;
#endif // USE_IMGUI
		POINT p{ static_cast<LONG>(point.x), static_cast<LONG>(point.y) };
		ClientToScreen(m_hwnd, &p);
		SetCursorPos(p.x, p.y);
	}
	void InputSystem::SetMousePos(int x, int y)
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return;
#endif // USE_IMGUI
		POINT p = { x,y };
		ClientToScreen(m_hwnd, &p);
		SetCursorPos(p.x, p.y);
	}
	void InputSystem::SetMousePosX(int x)
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return;
#endif // USE_IMGUI
		POINT p = { x,m_mouse_state.y };
		ClientToScreen(m_hwnd, &p);
		SetCursorPos(p.x, p.y);
	}
	void InputSystem::SetMousePosY(int y)
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return;
#endif // USE_IMGUI
		POINT p = { m_mouse_state.x, y };
		ClientToScreen(m_hwnd, &p);
		SetCursorPos(p.x, p.y);
	}
	bool InputSystem::GetMouseButton(Input::Mouse button) const
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return false;
#endif // USE_IMGUI
		return m_mouse_input_state[static_cast<int>(button)].STATE;
	}
	bool InputSystem::GetMouseButtonUp(Input::Mouse button) const
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return false;
#endif // USE_IMGUI
		return m_mouse_input_state[static_cast<int>(button)].RELEASED;
	}
	bool InputSystem::GetMouseButtonDown(Input::Mouse button) const
	{
#ifdef USE_IMGUI
		if (!IGUI::Get().IsActiveInputScene()) return false;
#endif // USE_IMGUI
		return m_mouse_input_state[static_cast<int>(button)].PRESSED;
	}
	bool InputSystem::GetMouseVisible() const
	{
		return m_is_mouse_visible;
	}
	void InputSystem::SetMouseVisible(bool is_visible)
	{
		m_is_mouse_visible = is_visible;
		m_mouse.get()->SetVisible(is_visible);
	}
#pragma endregion

	// ~IInput

	bool InputSystem::StartVibration(
		int player,
		float left_motor, float right_motor,
		float left_trigger, float right_trigger
	)
	{
		return m_game_pad.get()->SetVibration(player, left_motor, right_motor, left_trigger, right_trigger);
	}

}// namespace TKGEngine::Input