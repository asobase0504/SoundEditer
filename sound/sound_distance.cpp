#include "sound_distance.h"

const float CSoundDistance::m_Power_0_Distance = 100.0f;
const float CSoundDistance::m_NoneSubtractDistance = 0.0f;
D3DXVECTOR3* CSoundDistance::m_listenerPos = nullptr;


CSoundDistance::CSoundDistance()
{
}

CSoundDistance::~CSoundDistance()
{
}

HRESULT CSoundDistance::Init()
{
	CSound::Init();
	return E_NOTIMPL;
}

void CSoundDistance::Update()
{
	VolumeSubtract();
	CSound::Update();
}

HRESULT CSoundDistance::Play(const D3DXVECTOR3& inPos)
{
	CSound::Play();
	m_pos = inPos;
	VolumeSubtract();
	return S_OK;
}

void CSoundDistance::VolumeSubtract()
{
	D3DXVECTOR3 listenerPos(0.0f, 0.0f, 0.0f);

	if (m_listenerPos != nullptr)
	{
		listenerPos = *m_listenerPos;
	}

	D3DXVECTOR3 distance = m_pos - listenerPos;

	float length = D3DXVec3Length(&distance);

	float power = (m_Power_0_Distance - length + m_NoneSubtractDistance) / m_Power_0_Distance;

	if (power >= 1.0f)
	{
		power = 1.0f;
	}

	SetVolume(power);
}
