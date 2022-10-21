//-----------------------------------------------------------------------------------------------------------
//
//キーボード、ゲームパッド入力処理
//Author::TAKANO
//
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------
//インクルードファイル
//-----------------------------------------------------------
#include "input.h"
#include "application.h"

//-----------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------
CInput::CInput()
{
	m_pInput = nullptr;
	m_pDevkeyboard = nullptr;
}

//-----------------------------------------------------------
//デストラクタ
//-----------------------------------------------------------
CInput::~CInput()
{
}

//-----------------------------------------------------------
//キーボードの初期化
//-----------------------------------------------------------
HRESULT CInput::Initkeyboard(HINSTANCE hInstance, HWND hWnd)
{
	//Directinputの生成
	if (FAILED(DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)& m_pInput,NULL)))
	{
		return E_FAIL;
	}

	//入力デバイスの生成
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard,&m_pDevkeyboard,NULL)))
	{
		return E_FAIL;
	}

	//データフォーマットを設定
	if (FAILED(m_pDevkeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//協調モードの設定
	if (FAILED(m_pDevkeyboard->SetCooperativeLevel(hWnd,(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//キーボードのアクセス権を獲得
	m_pDevkeyboard->Acquire();

	return S_OK;
}

//-----------------------------------------------------------
//ゲームパッドの初期化
//-----------------------------------------------------------
HRESULT CInput::InitJoypad(void)
{
	//メモリのクリア
	memset(&m_JoykeyState, 0, sizeof(XINPUT_STATE));

	//Xinputのステートの設定（有効活用）
	XInputEnable(true);

	return S_OK;
}

//-----------------------------------------------------------
//キーボードの終了
//-----------------------------------------------------------
void CInput::Uninitkeyboard(void)
{
	//入力デバイスの破棄
	if (m_pDevkeyboard != NULL)
	{
		m_pDevkeyboard->Unacquire();
		m_pDevkeyboard->Release();
		m_pDevkeyboard = NULL;
	}

	//DirectInputオブジェクトの破棄
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}

//-----------------------------------------------------------
//ゲームパッドの終了
//-----------------------------------------------------------
void CInput::UninitJoypad(void)
{
	//Xinputのステートの設定
	XInputEnable(false);
}

//-----------------------------------------------------------
//キーボードの更新
//-----------------------------------------------------------
void CInput::Updatekeyboard(void)
{
	BYTE akeystate[NUM_KEY_MAX];				//キーボードの入力情報
	int nCntkey;

	//入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevkeyboard->GetDeviceState(sizeof(akeystate), &akeystate[0])))
	{
			for (nCntkey = 0; nCntkey < NUM_KEY_MAX; nCntkey++)
			{
				m_akeyStateTrigger[nCntkey] = (m_akeyState[nCntkey] & akeystate[nCntkey]) ^ akeystate[nCntkey];			//トリガー処理						//キーボードのトリガー情報を取得
				m_akeyState[nCntkey] = akeystate[nCntkey];																//プレス処理						//キーボードのプレス情報を取得
			}
	}
	else
	{
		m_pDevkeyboard->Acquire();					//キーボードのアクセス権を獲得
	}
}

//-----------------------------------------------------------
//ゲームパッド更新
//-----------------------------------------------------------
void CInput::UpdateJoypad(void)
{
	XINPUT_STATE joykeystate;

	//ショイパットの状態を取得
	if (XInputGetState(0,&joykeystate) == ERROR_SUCCESS)
	{
		m_JoykeyStateTrigger.Gamepad.wButtons = (m_JoykeyState.Gamepad.wButtons & joykeystate.Gamepad.wButtons) ^ joykeystate.Gamepad.wButtons;			//トリガー処理	
		m_JoykeyState = joykeystate;
	}
}

//-----------------------------------------------------------
//キーボードの情報を取得(押込)
//-----------------------------------------------------------
bool CInput::GetkeyboardPress(int nkey)
{
	return(m_akeyState[nkey] & 0x80) ? true : false;
}

//-----------------------------------------------------------
//キーボードの情報を取得
//-----------------------------------------------------------
bool CInput::GetkeyboardTrigger(int nkey)
{
	return(m_akeyStateTrigger[nkey] & 0x80) ? true : false;
}

//-----------------------------------------------------------
//ゲームパッドの情報を取得(押込)
//-----------------------------------------------------------
bool CInput::GetJoypadPress(XINPUT_TYPE key)
{
	return (m_JoykeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//-----------------------------------------------------------
//ゲームパッドの情報を取得
//-----------------------------------------------------------
bool CInput::GetJoypadTrigger(XINPUT_TYPE key)
{
	return (m_JoykeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

