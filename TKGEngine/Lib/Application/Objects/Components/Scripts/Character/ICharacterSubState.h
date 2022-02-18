#pragma once

namespace TKGEngine
{
	/// <summary>
	/// ステートごとに実行される処理のインターフェース
	/// </summary>
	class ICharacterSubState
	{
	public:
		ICharacterSubState() = default;
		virtual ~ICharacterSubState() = default;
		ICharacterSubState(const ICharacterSubState&) = delete;
		ICharacterSubState& operator=(const ICharacterSubState&) = delete;
		ICharacterSubState& operator=(ICharacterSubState&&) = default;

		// 各ステートごとの処理
		virtual void Enter() = 0;
		virtual void BeforeExecute() {}
		virtual void Execute() = 0;
		virtual void AfterExecute() {}
		virtual void Exit() = 0;
	};
}
