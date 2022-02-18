
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
		// ���͏�ԗp�\����
		struct InputState
		{
			bool STATE;
			bool PRESSED;
			bool RELEASED;
		};

		// GamePad�̐U���p�\����
		struct PadVibrate
		{
			bool vibration = false;	// ���ݐU�����Ă��邩�ǂ���
			float timer = 0.0f;		// �c��U������[s]
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

		// �L�[��ԏ��
		InputState m_key_input_state[256/*DirectX::Keyboard::Keys*/] = {};
		InputState m_pad_input_state[GAMEPAD_NUM][static_cast<int>(GamePad::NUM)] = {};
		InputState m_mouse_input_state[static_cast<int>(Mouse::NUM)] = {};

		// �f�o�C�X���̓g���b�J�[���̊e�{�^���ւ̃|�C���^(initialize�Ńg���b�J�[�쐬��ɐݒ�)
		const DirectX::GamePad::ButtonStateTracker::ButtonState* m_pad_button_ptrs[GAMEPAD_NUM][static_cast<int>(GamePad::NUM)/*buttons*/] = {};
		const DirectX::Mouse::ButtonStateTracker::ButtonState* m_mouse_button_ptrs[static_cast<int>(Mouse::NUM)/*buttons*/] = {};

		// �Q�[���p�b�h�̐U�����Ǘ�����\���̔z��
		PadVibrate m_pad_vibrations[GAMEPAD_NUM];

		// ���̓f�o�C�X�Ɋւ���|�C���^
		std::unique_ptr<DirectX::Keyboard> m_keyboard;
		std::unique_ptr<DirectX::GamePad> m_game_pad;
		std::unique_ptr<DirectX::Mouse> m_mouse;
		std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> m_kb_tracker;
		std::unique_ptr<DirectX::GamePad::ButtonStateTracker> m_pad_tracker[GAMEPAD_NUM];
		std::unique_ptr<DirectX::Mouse::ButtonStateTracker> m_mouse_tracker;
		// ���͏�Ԃ��i�[�����
		DirectX::Keyboard::State m_kb_state;
		DirectX::GamePad::State m_pad_state[GAMEPAD_NUM];
		DirectX::Mouse::State m_mouse_state;

		// �}�E�X�|�C���^�̕\�����
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

		// �g���b�J�[���̊e�{�^���ւ̃|�C���^��ݒ�
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
		// �L�[�{�[�h�̍X�V
		// ***********************************************
		// �L�[�{�[�h�̓��͏�Ԃ��擾
		{
			// ���̓f�o�C�X�̌���Ԃ��擾
			m_kb_state = m_keyboard.get()->GetState();
			// �g���b�J�[�̍X�V
			m_kb_tracker.get()->Update(m_kb_state);

			for (int i = 0; i < 256; ++i)
			{
				// �Q�Ɛ�ւ̃|�C���^�Ƃ��Ēu���Ă���
				const auto s = &m_key_input_state[i];
				// ���݂��Ȃ��L�[�R�[�h�͓K���Ȓl�������Ă��܂����O���ŌĂяo���Ȃ��̂ō\��Ȃ�
				const DirectX::Keyboard::Keys key = static_cast<DirectX::Keyboard::Keys>(i);
				s->STATE = m_kb_state.IsKeyDown(key);
				s->PRESSED = m_kb_tracker->IsKeyPressed(key);
				s->RELEASED = m_kb_tracker->IsKeyReleased(key);
			}
		}

		// ***********************************************
		// �}�E�X�̍X�V
		// ***********************************************
		// �}�E�X�̓��͏�Ԃ��擾
		{
			// ���̓f�o�C�X�̌���Ԃ��擾
			m_mouse_state = m_mouse.get()->GetState();
			// �g���b�J�[�̍X�V
			m_mouse_tracker.get()->Update(m_mouse_state);

			for (int i = 0; i < static_cast<int>(Mouse::NUM); ++i)
			{
				// �Q�Ɛ�ւ̃|�C���^�Ƃ��Ēu���Ă���
				const auto s = &m_mouse_input_state[i];
				// �g���b�J�[�����Ԃ��擾
				s->STATE = (*m_mouse_button_ptrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::HELD);
				s->PRESSED = (*m_mouse_button_ptrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED);
				s->RELEASED = (*m_mouse_button_ptrs[i] == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED);
			}
			// �}�E�X�z�C�[�������Z�b�g����
			m_mouse.get()->ResetScrollWheelValue();
		}

		// ***********************************************
		// �Q�[���p�b�h�̍X�V
		// ***********************************************
		for (int i = 0; i < GAMEPAD_NUM; ++i)
		{
			// ���̓f�o�C�X�̌���Ԃ��擾(gamepad)
			m_pad_state[i] = m_game_pad->GetState(i, DirectX::GamePad::DEAD_ZONE_CIRCULAR);

			// �ڑ�����Ă��Ȃ���Ώ������΂�
			if (!m_pad_state[i].IsConnected())continue;

			// �g���b�J�[�̍X�V(gamepad)
			m_pad_tracker[i].get()->Update(m_pad_state[i]);

			// �U���̎��Ԃɂ��X�V
			if (m_pad_vibrations[i].vibration == true)
			{
				m_pad_vibrations[i].timer -= args.unscaled_delta_time;
				if (m_pad_vibrations[i].timer <= 0.0f)
				{
					StopVibration(i);
				}
			}

			// �Q�[���p�b�h�̓��͏�Ԃ��擾
			for (int j = 0; j < static_cast<int>(GamePad::NUM); ++j)
			{
				// �Q�Ɛ�ւ̃|�C���^�Ƃ��Ēu���Ă���
				const auto state = &m_pad_input_state[i][j];

				// �g���b�J�[�����Ԃ��擾
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