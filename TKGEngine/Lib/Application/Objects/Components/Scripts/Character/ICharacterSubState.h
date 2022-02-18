#pragma once

namespace TKGEngine
{
	/// <summary>
	/// �X�e�[�g���ƂɎ��s����鏈���̃C���^�[�t�F�[�X
	/// </summary>
	class ICharacterSubState
	{
	public:
		ICharacterSubState() = default;
		virtual ~ICharacterSubState() = default;
		ICharacterSubState(const ICharacterSubState&) = delete;
		ICharacterSubState& operator=(const ICharacterSubState&) = delete;
		ICharacterSubState& operator=(ICharacterSubState&&) = default;

		// �e�X�e�[�g���Ƃ̏���
		virtual void Enter() = 0;
		virtual void BeforeExecute() {}
		virtual void Execute() = 0;
		virtual void AfterExecute() {}
		virtual void Exit() = 0;
	};
}
