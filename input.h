//-----------------------------------------------------------------------------------------------------------
//
//キーボード、ゲームパッド処理
//Author::TAKANO
//
//-----------------------------------------------------------------------------------------------------------
#ifndef _INPUT_H_
#define _INPUT_H_

//-----------------------------------------------------------
//インクルードファイル
//-----------------------------------------------------------
#include <d3dx9.h>
#include <dxdiag.h>
#define DIRECTINPUT_VERSION		(0X0800)
#include "dinput.h"
#include "xinput.h"

//-----------------------------------------------------------
//ライブラリのリンク
//-----------------------------------------------------------
#pragma comment(lib,"d3d9.lib")			// 描画処理に使用するライブラリ
#pragma comment(lib,"d3dx9.lib")		// d3d9.libの拡張用のライブラリ
#pragma comment(lib,"dxguid.lib")		// DirectXコンポーネント使用になライブラリ
#pragma comment(lib,"winmm.lib")		// システム時刻取得に使用するライブラリ
#pragma comment(lib,"dinput8.lib")		// DInputに使用するライブラリ
#pragma comment(lib,"xinput.lib")		// XInputに使用するライブラリ

//-----------------------------------------------------------
//マクロ定義
//-----------------------------------------------------------
#define NUM_KEY_MAX	(256)		//キーの最大数

//-----------------------------------------------------------
//クラス定義
//キーボード系統の入力処理
//-----------------------------------------------------------
class CInput
{
public:

	enum XINPUT_TYPE		//キーの種類
	{
		JOYKEY_UP = 0,		//十字キー上
		JOYKEY_DOWN,		//十字キー下
		JOYKEY_LEFT,		//十字キー左
		JOYKEY_RIGHT,		//十字キー右
		JOYKEY_STRTE,		//スタ-ト
		JOYKEY_BACK,		//バック
		JOYKEY_LPUSH,		//Lステック押込
		JOYKEY_RPUSH,		//Rステック押込
		JOYKEY_L1,			//L1
		JOYKEY_R1,			//R1
		JOYKEY_L2,			//L2
		JOYKEY_R2,			//R2
		JOYKEY_A,			//Aボタン
		JOYKEY_B,			//Bボタン
		JOYKEY_X,			//Xボタン
		JOYKEY_Y,			//Yボタン
		MAX_KEY
	};

	CInput();													//コンストラクタ
	~CInput();													//デストラクタ
	HRESULT Initkeyboard(HINSTANCE hInstance, HWND hWnd);		//キーボードの初期化
	void Uninitkeyboard(void);									//キーボードの終了
	void Updatekeyboard(void);									//キーボードの更新
	bool GetkeyboardPress(int nkey);							//キーボードの入力情報の取得
	bool GetkeyboardTrigger(int nkey);							//キーボードの押込情報の取得
	HRESULT InitJoypad(void);									//コントローラーの初期化
	void UninitJoypad(void);									//コントローラーの終了
	void UpdateJoypad(void);									//コントローラーの更新
	bool GetJoypadPress(XINPUT_TYPE key);						//コントローラーの入力情報の取得
	bool GetJoypadTrigger(XINPUT_TYPE key);						//コントローラーの押込情報の取得

private:
	LPDIRECTINPUT8 m_pInput = NULL;								//ダイレクトインプットオブジェクトのポインタ
	LPDIRECTINPUTDEVICE8 m_pDevkeyboard = NULL;					//入力デバイス（キーボード）のポインタ
	BYTE m_akeyState[NUM_KEY_MAX];								//キーボードのプレス情報
	BYTE m_akeyStateTrigger[NUM_KEY_MAX];						//キーボードのトリガー情報
	XINPUT_STATE m_JoykeyState;									//ゲームパッドのプレス情報
	XINPUT_STATE m_JoykeyStateTrigger;							//ゲームパッドのトリガー情報

};

#endif // !_INPUT_H_

