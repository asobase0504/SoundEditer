//--------------------------------------------------------------------------------------
//
// �����_�����O
// Author::TAKANO
//
//--------------------------------------------------------------------------------------
#ifndef _RENDERER_H_
#define _RENDERER_H_

//----------------------------------------
// ���C�u�����[�����N
//----------------------------------------
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")

//----------------------------------------
//�C���N���[�h�t�@�C��
//----------------------------------------
#include <d3dx9.h>
#include <Windows.h>
#include <tchar.h> // _T
//�J�b�gand�y�[�X�g�ŃN���X�w��

//----------------------------------------
//�N���X��`
//�����_�����O
//----------------------------------------
class CRenderer
{
public:
	CRenderer();									//�R���X�g���N�^
	~CRenderer();									//�f�X�g���N�^
	HRESULT Init(HWND hWnd, bool bWindow);			//������
	void Uninit();									//�I��
	void Update();									//�X�V
	void Draw();									//�`��
	void DrawFPS();									//FPS�̕`��
	LPDIRECT3DDEVICE9 GetDevice();					//�f�o�C�X���̎擾

private:
	LPDIRECT3D9 m_pD3D = nullptr;					//�_�C���N�g3D�̃|�C���^�[
	LPDIRECT3DDEVICE9 m_pD3DDevice = nullptr;		//�f�o�C�X�̃|�C���^�[
	LPD3DXFONT m_pFont = nullptr;					//�f�o�b�N�Ŏg�p����̃t�H���g�|�C���^�[
	int m_nCountFPS;								//FPS�J�E���g�p�̕ϐ�
};

#endif // !_RENDERER_H_
