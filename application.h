//--------------------------------------------------------------------------------------
//
// �}�l�[�W���[�A�v���P�[�V����
// Author::TAKANO
//
//--------------------------------------------------------------------------------------
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <d3dx9.h>

//----------------------------------------
//�O���錾
//----------------------------------------
class CRenderer;
class CTexture;
class CTitle;
class CGame;
class CResult;
class CRanking;
class CInput;
class CSoundManager;

//----------------------------------------
//�N���X��`
//�}�l�[�W���[
//----------------------------------------
class CApplication
{
public:
	CApplication();														//�R���X�g���N�^
	~CApplication();													//�f�X�g���N�^
	HRESULT Init(HINSTANCE hInctance, HWND hWnd, bool bWindow);			//������
	void Uninit();														//�I��
	void Update();														//�X�V
	void Draw();														//�`��
	void ReleseDustCheck();												//����R������Ă��Ȃ����m�F�p�֐�
	static CRenderer* GetRenderer();									//�`����̎擾
	static CInput* GetInput();
	static CSoundManager* GetSoundManager();							//���y�}�l�[�W���[���̎擾

private:
	static CRenderer* m_aRenderer;										//�����_���[�̃|�C���^�[
	static CInput* m_pInput;
	static CSoundManager* m_pSoundManager;								//���y�}�l�[�W���[�̃|�C���^�[
};

#endif // !_APPLICATION_H_