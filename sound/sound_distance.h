//*****************************************************************************
//
// �T�E���h
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
// Object�̔h���ɂ��Ė��t���[��Update��ʂ��ĉ������B
// Sound�p��priority������ƊǗ��y����
*/

//=============================================================================
// �N���X��`
//=============================================================================
class CSoundDistance : public CSound
{
private:
	static const float m_Power_0_Distance;
	static const float m_NoneSubtractDistance;

	static D3DXVECTOR3* m_listenerPos;
public:
	CSoundDistance();	// �R���X�g���N�^
	~CSoundDistance();	// �f�X�g���N�^

	static void SetListenerPos(D3DXVECTOR3* inPos) { m_listenerPos = inPos; }

	HRESULT Init();	// ������
	void Update();	// �X�V

	// �Đ�
	HRESULT Play(const D3DXVECTOR3& inPos);

	void SetPos(const D3DXVECTOR3& inPos) { m_pos = inPos; }

private:
	void VolumeSubtract();

private:
	D3DXVECTOR3 m_pos;
};

#endif // !_SOUND_H_
