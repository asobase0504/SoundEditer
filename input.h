//-----------------------------------------------------------------------------------------------------------
//
//�L�[�{�[�h�A�Q�[���p�b�h����
//Author::TAKANO
//
//-----------------------------------------------------------------------------------------------------------
#ifndef _INPUT_H_
#define _INPUT_H_

//-----------------------------------------------------------
//�C���N���[�h�t�@�C��
//-----------------------------------------------------------
#include <d3dx9.h>
#include <dxdiag.h>
#define DIRECTINPUT_VERSION		(0X0800)
#include "dinput.h"
#include "xinput.h"

//-----------------------------------------------------------
//���C�u�����̃����N
//-----------------------------------------------------------
#pragma comment(lib,"d3d9.lib")			// �`�揈���Ɏg�p���郉�C�u����
#pragma comment(lib,"d3dx9.lib")		// d3d9.lib�̊g���p�̃��C�u����
#pragma comment(lib,"dxguid.lib")		// DirectX�R���|�[�l���g�g�p�ɂȃ��C�u����
#pragma comment(lib,"winmm.lib")		// �V�X�e�������擾�Ɏg�p���郉�C�u����
#pragma comment(lib,"dinput8.lib")		// DInput�Ɏg�p���郉�C�u����
#pragma comment(lib,"xinput.lib")		// XInput�Ɏg�p���郉�C�u����

//-----------------------------------------------------------
//�}�N����`
//-----------------------------------------------------------
#define NUM_KEY_MAX	(256)		//�L�[�̍ő吔

//-----------------------------------------------------------
//�N���X��`
//�L�[�{�[�h�n���̓��͏���
//-----------------------------------------------------------
class CInput
{
public:

	enum XINPUT_TYPE		//�L�[�̎��
	{
		JOYKEY_UP = 0,		//�\���L�[��
		JOYKEY_DOWN,		//�\���L�[��
		JOYKEY_LEFT,		//�\���L�[��
		JOYKEY_RIGHT,		//�\���L�[�E
		JOYKEY_STRTE,		//�X�^-�g
		JOYKEY_BACK,		//�o�b�N
		JOYKEY_LPUSH,		//L�X�e�b�N����
		JOYKEY_RPUSH,		//R�X�e�b�N����
		JOYKEY_L1,			//L1
		JOYKEY_R1,			//R1
		JOYKEY_L2,			//L2
		JOYKEY_R2,			//R2
		JOYKEY_A,			//A�{�^��
		JOYKEY_B,			//B�{�^��
		JOYKEY_X,			//X�{�^��
		JOYKEY_Y,			//Y�{�^��
		MAX_KEY
	};

	CInput();													//�R���X�g���N�^
	~CInput();													//�f�X�g���N�^
	HRESULT Initkeyboard(HINSTANCE hInstance, HWND hWnd);		//�L�[�{�[�h�̏�����
	void Uninitkeyboard(void);									//�L�[�{�[�h�̏I��
	void Updatekeyboard(void);									//�L�[�{�[�h�̍X�V
	bool GetkeyboardPress(int nkey);							//�L�[�{�[�h�̓��͏��̎擾
	bool GetkeyboardTrigger(int nkey);							//�L�[�{�[�h�̉������̎擾
	HRESULT InitJoypad(void);									//�R���g���[���[�̏�����
	void UninitJoypad(void);									//�R���g���[���[�̏I��
	void UpdateJoypad(void);									//�R���g���[���[�̍X�V
	bool GetJoypadPress(XINPUT_TYPE key);						//�R���g���[���[�̓��͏��̎擾
	bool GetJoypadTrigger(XINPUT_TYPE key);						//�R���g���[���[�̉������̎擾

private:
	LPDIRECTINPUT8 m_pInput = NULL;								//�_�C���N�g�C���v�b�g�I�u�W�F�N�g�̃|�C���^
	LPDIRECTINPUTDEVICE8 m_pDevkeyboard = NULL;					//���̓f�o�C�X�i�L�[�{�[�h�j�̃|�C���^
	BYTE m_akeyState[NUM_KEY_MAX];								//�L�[�{�[�h�̃v���X���
	BYTE m_akeyStateTrigger[NUM_KEY_MAX];						//�L�[�{�[�h�̃g���K�[���
	XINPUT_STATE m_JoykeyState;									//�Q�[���p�b�h�̃v���X���
	XINPUT_STATE m_JoykeyStateTrigger;							//�Q�[���p�b�h�̃g���K�[���

};

#endif // !_INPUT_H_

