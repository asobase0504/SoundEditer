//*****************************************************************************
//
// サウンド
// Author YudaKaito
//
//*****************************************************************************
#ifndef _SOUND_DISTANCE_H_
#define _SOUND_DISTANCE_H_

//=============================================================================
// include
//=============================================================================
#include "sound.h"
#include <d3dx9.h>

/*
// Objectの派生にして毎フレームUpdateを通して下さい。
// Sound用のpriorityがあると管理楽かも
*/

//=============================================================================
// クラス定義
//=============================================================================
class CSoundDistance : public CSound
{
private:
	static const float m_Power_0_Distance;
	static const float m_NoneSubtractDistance;

	static D3DXVECTOR3* m_listenerPos;
public:
	CSoundDistance();	// コンストラクタ
	~CSoundDistance();	// デストラクタ

	static void SetListenerPos(D3DXVECTOR3* inPos) { m_listenerPos = inPos; }

	HRESULT Init();	// 初期化
	void Update();	// 更新

	// 再生
	HRESULT Play(const D3DXVECTOR3& inPos);

	void SetPos(const D3DXVECTOR3& inPos) { m_pos = inPos; }

private:
	void VolumeSubtract();

private:
	D3DXVECTOR3 m_pos;
};

#endif // !_SOUND_H_
