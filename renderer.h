//--------------------------------------------------------------------------------------
//
// レンダリング
// Author::TAKANO
//
//--------------------------------------------------------------------------------------
#ifndef _RENDERER_H_
#define _RENDERER_H_

//----------------------------------------
// ライブラリーリンク
//----------------------------------------
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")

//----------------------------------------
//インクルードファイル
//----------------------------------------
#include <d3dx9.h>
#include <Windows.h>
#include <tchar.h> // _T
//カットandペーストでクラス指定

//----------------------------------------
//クラス定義
//レンダリング
//----------------------------------------
class CRenderer
{
public:
	CRenderer();									//コンストラクタ
	~CRenderer();									//デストラクタ
	HRESULT Init(HWND hWnd, bool bWindow);			//初期化
	void Uninit();									//終了
	void Update();									//更新
	void Draw();									//描画
	void DrawFPS();									//FPSの描画
	LPDIRECT3DDEVICE9 GetDevice();					//デバイス情報の取得

private:
	LPDIRECT3D9 m_pD3D = nullptr;					//ダイレクト3Dのポインター
	LPDIRECT3DDEVICE9 m_pD3DDevice = nullptr;		//デバイスのポインター
	LPD3DXFONT m_pFont = nullptr;					//デバックで使用するのフォントポインター
	int m_nCountFPS;								//FPSカウント用の変数
};

#endif // !_RENDERER_H_
