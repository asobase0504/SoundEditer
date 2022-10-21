//--------------------------------------------------------------------------------------
//
// ���C��
// Author::TAKANO
//
//--------------------------------------------------------------------------------------

//--------------------------------------
// ���C�u�����[�����N
//--------------------------------------
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")

//--------------------------------------
// �C���N���[�h
//--------------------------------------
#include <Windows.h>
#include <tchar.h> // _T
#include <d3dx9.h>
#include "main.h"
#include "application.h"

//--------------------------------------
// �萔��`
//--------------------------------------
namespace
{
	// �E�C���h�E�̃N���X��
	LPCTSTR CLASS_NAME = _T("Sound Editer_TYPE2");
	// �E�C���h�E�̃L���v�V������
	LPCTSTR WINDOW_NAME = _T("Sound Editer");

	// �X�N���[���̕�
	const int SCREEN_WIDTH = 1280;
	// �X�N���[���̍���
	const int SCREEN_HEIGHT = 720;
}

//--------------------------------------
// �v���g�^�C�v�錾
//--------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hinstace,HWND hWnd, bool bWindow);
void Uninit();
void Update();
void Draw();
int GetCount();

//--------------------------------------
// �O���[�o���ϐ�
//--------------------------------------
// FPS�J�E���^
int g_nCountFPS;

CApplication* pApplecation;

//--------------------------------------
// ���C���֐�
//--------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};

	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	// �w�肵���N���C�A���g�̈���m�ۂ��邽�߂ɕK�v�ȃE�B���h�E���W���v�Z
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// �E�B���h�E�̍쐬
	HWND hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(rect.right - rect.left),
		(rect.bottom - rect.top),
		NULL,
		NULL,
		hInstance,
		NULL);

	pApplecation = new CApplication;

	// ����������
	if (FAILED(Init(hInstance,hWnd, TRUE)))
	{//�����������s�����ꍇ
		return -1;
	}

	// ����\��ݒ�
	timeBeginPeriod(1);

	// �t���[���J�E���g������
	DWORD dwCurrentTime = 0;
	DWORD dwExecLastTime = timeGetTime();
#ifdef _DEBUG
	DWORD dwFrameCount = 0;
	DWORD dwFPSLastTime = dwExecLastTime;
#endif // _DEBUG

	// �E�C���h�E�̕\��
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	// ���b�Z�[�W���[�v
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{ // PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();	// ���݂̎��Ԃ��擾
#ifdef _DEBUG
			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{// 0.5�b���ƂɎ��s
				// FPS���Z�o
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;	// ���݂̎��Ԃ�ۑ�
				dwFrameCount = 0;
			}
#endif // _DEBUG

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{ // 1/60�b�o��
				// ���݂̎��Ԃ�ۑ�
				dwExecLastTime = dwCurrentTime;

				// �X�V����
				Update();

				// �`�揈��
				Draw();

#ifdef _DEBUG
				dwFrameCount++;
#endif // _DEBUG
			}
		}
	}

	// �I������
	Uninit();

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// ����\��߂�
	timeEndPeriod(1);

	return (int)msg.wParam;
}

//--------------------------------------
// �E�C���h�E�v���V�[�W��
//--------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE: // [ESC]�L�[�������ꂽ
			// �E�B���h�E��j��
			DestroyWindow(hWnd);
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//--------------------------------------
// ������
//--------------------------------------
HRESULT Init(HINSTANCE hinstace, HWND hWnd, bool bWindow)
{
	pApplecation->Init(hinstace, hWnd, bWindow);
	return S_OK;
}

//--------------------------------------
// �I��
//--------------------------------------
void Uninit()
{
	if (pApplecation != nullptr)
	{
		pApplecation->Uninit();
		delete pApplecation;
		pApplecation = nullptr;
	}
}

//--------------------------------------
// �X�V
//--------------------------------------
void Update()
{
	pApplecation->Update();
}

//--------------------------------------
// �`��
//--------------------------------------
void Draw()
{
	pApplecation->Draw();
}

//--------------------------------------
// FPS�̃J�E���g�̎擾
//--------------------------------------
int GetCount()
{
	return g_nCountFPS;
}
