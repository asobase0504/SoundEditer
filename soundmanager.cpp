//------------------------------------------------------------------------------
//
//�T�E���h�}�l�[�W���[
//Author::TAKANO
//
//------------------------------------------------------------------------------
//--------------------------------------
//�C���N���[�h�t�@�C��
//--------------------------------------
#include "application.h"
#include <assert.h>
#include "soundmanager.h"
#include "sound.h"

// �t�@�C���Ǘ�
#include "nlohmann/json.hpp"
#include <fstream>
#include "letter.h"
#include "file.h"

//--------------------------------------
//�萔��`
//--------------------------------------
const float BAN_BGM = 0.0007f;
const float TRACK_PITH = 0.001f;
const float MAX_RATE = 11.0f;
const float UP_DOWN_TRACK = 0.01f;

//--------------------------------------
//�R���X�g���N�^
//--------------------------------------
CSoundManager::CSoundManager()
{
	m_pXAudio2 = nullptr;				// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	m_pMasteringVoice = nullptr;		// �}�X�^�[�{�C�X
	m_apSourceVoice.clear();			// �\�[�X�{�C�X
	m_apDataAudio.clear();				// �I�[�f�B�I�f�[�^
	m_aSizeAudio.clear();				// �I�[�f�B�I�f�[�^�T�C�Y

	m_nCount = 0;
	m_Fade_Ok = false;
	m_Fadetype = TYPE_NONE;
}

//--------------------------------------
//�f�X�g���N�^
//--------------------------------------
CSoundManager::~CSoundManager()
{
}

//--------------------------------------
// �쐬
//--------------------------------------
CSound * CSoundManager::SoundCreate(std::string inName)
{
	return SoundCreate(m_pathName[inName]);
}

//--------------------------------------
// �쐬
//--------------------------------------
CSound * CSoundManager::SoundCreate(int inName)
{
	if (m_pSound[inName] != nullptr)
	{
		Init();
		m_pSound[inName] = new CSound;
		LoadPath();
		m_pSound[inName]->Init();
	}

	return m_pSound[inName];
}

//--------------------------------------
//�I��
//--------------------------------------
CSoundManager * CSoundManager::ManagerCreate()
{
	CSoundManager* pManager = new CSoundManager;

	if (pManager != nullptr)
	{
		pManager->Init();
	}

	return pManager;
}

//--------------------------------------
// ������
// �T�v�F�@�\�Ƃ��Ă�����̂��T���v���Ƃ���
//--------------------------------------
HRESULT CSoundManager::Init()
{
	LoadPath();		//�����̓Ǎ�

	//�m�F�\
	HRESULT hr;		//HRESULT�^�̕ϐ�

	//--------------------------------------
	// COM���C�u�����̏�����
	//--------------------------------------
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// XAudio2�I�u�W�F�N�g�̍쐬
	hr = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(hr))
	{
		// COM���C�u�����̏I������
		CoUninitialize();

		// �v���O�������ً}�Ŏ~�߂�
		assert(false);

		return E_FAIL;
	}

	// �}�X�^�[�{�C�X�̐���
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr))
	{
		if (m_pXAudio2)
		{
			// XAudio2�I�u�W�F�N�g�̊J��
			m_pXAudio2->Release();
			m_pXAudio2 = nullptr;
		}

		// COM���C�u�����̏I������
		CoUninitialize();

		// �v���O�������ً}�Ŏ~�߂�
		assert(false);

		return E_FAIL;
	}

	/*�T�E���h�f�[�^�T�E���h�Ɏ������邱�Ƃ�ڕW��*/

	// �T�E���h�f�[�^�̏�����
	for (int nCntSound = 0; nCntSound < (int)m_path.size(); nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// �o�b�t�@�̃N���A
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// �T�E���h�f�[�^�t�@�C���̐���
		hFile = CreateFile((LPCSTR)GetPath(nCntSound).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return HRESULT_FROM_WIN32(GetLastError());
		}
		if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// �t�@�C���|�C���^��擪�Ɉړ�
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// WAVE�t�@�C���̃`�F�b�N
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if (FAILED(hr))
		{
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return S_FALSE;
		}
		if (dwFiletype != 'EVAW')
		{
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return S_FALSE;
		}

		// �t�H�[�}�b�g�`�F�b�N
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if (FAILED(hr))
		{
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if (FAILED(hr))
		{
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return S_FALSE;
		}

		// �I�[�f�B�I�f�[�^�ǂݍ���
		hr = CheckChunk(hFile, 'atad', &m_aSizeAudio[nCntSound], &dwChunkPosition);
		if (FAILED(hr))
		{
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return S_FALSE;
		}
		m_apDataAudio[nCntSound] = (BYTE*)malloc(m_aSizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, m_apDataAudio[nCntSound], m_aSizeAudio[nCntSound], dwChunkPosition);
		if (FAILED(hr))
		{
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return S_FALSE;
		}

		// �t�@�C�����N���[�Y
		CloseHandle(hFile);

		// �\�[�X�{�C�X�̐���
		hr = m_pXAudio2->CreateSourceVoice(&m_apSourceVoice[nCntSound], &(wfx.Format));
		if (FAILED(hr))
		{
			//�v���O�������ً}�Ŏ~�߂�
			assert(false);
			return S_FALSE;
		}

		// �o�b�t�@�̒l�ݒ�
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_aSizeAudio[nCntSound];
		buffer.pAudioData = m_apDataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = GetLoop(nCntSound);

		// �I�[�f�B�I�o�b�t�@�̓o�^
		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}

	return S_OK;
}

//--------------------------------------
//�I��
//--------------------------------------
void CSoundManager::Uninit()
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < (int)m_apSourceVoice.size(); nCntSound++)
	{
		if (m_apSourceVoice[nCntSound])
		{
			// �ꎞ��~
			m_apSourceVoice[nCntSound]->Stop(0);

			// �\�[�X�{�C�X�̔j��
			m_apSourceVoice[nCntSound]->DestroyVoice();
			m_apSourceVoice[nCntSound] = nullptr;

			// �I�[�f�B�I�f�[�^�̊J��
			free(m_apDataAudio[nCntSound]);
			m_apDataAudio[nCntSound] = nullptr;
		}
	}

	// �}�X�^�[�{�C�X�̔j��
	m_pMasteringVoice->DestroyVoice();
	m_pMasteringVoice = nullptr;

	if (m_pXAudio2)
	{
		// XAudio2�I�u�W�F�N�g�̊J��
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}

	// COM���C�u�����̏I������
	CoUninitialize();
}

//--------------------------------------
// �X�V
// �T�v:�@�\�Ƃ��Ă�����̂��T���v���Ƃ��Ċm�F�\
//--------------------------------------
void CSoundManager::Update()
{
	//���ʕύX
	//SoundVolume(m_type);

	//�T�E���h�t�F�[�h�C��
	//FadeInSound(m_type);

	//�T�E���h�t�F�[�h�A�E�g
	//FadeOutSound(m_type);

	//�s�b�`����
	//ShiftRate(m_type);
}

//--------------------------------------
//�S�ẴT�E���h��~
//--------------------------------------
void CSoundManager::Stop()
{
	for (int nCntSound = 0; nCntSound < (int)m_pSound.size(); nCntSound++)
	{
		if (m_pSound[nCntSound]->GetSourceVoice() == nullptr)
		{
			continue;
		}

		// �ꎞ��~
		m_pSound[nCntSound]->GetSourceVoice()->Stop(0);
	}
}

//--------------------------------------
// ���ʂ̐ݒ�
//--------------------------------------
void CSoundManager::SetVolume(std::string type, float fVolume)
{
	SetVolume(m_pathName[type], fVolume);
}

//--------------------------------------
// ���ʂ̐ݒ�
//--------------------------------------
void CSoundManager::SetVolume(int type, float fVolume)
{
	m_pSound[type]->GetSourceVoice()->SetVolume(fVolume);
}

//--------------------------------------
// �s�b�`�̐ݒ�
//--------------------------------------
void CSoundManager::SetRate(std::string type, float rate)
{
	SetRate(m_pathName[type], rate);
}

//--------------------------------------
// �s�b�`�̐ݒ�
//--------------------------------------
void CSoundManager::SetRate(int type, float rate)
{
	m_apSourceVoice[type]->SetFrequencyRatio(rate);
}

//--------------------------------------
// �t�F�[�h�C��
//--------------------------------------
void CSoundManager::FadeInSound(std::string type)
{
	FadeInSound(m_pathName[type]);
}

//--------------------------------------
// �t�F�[�h�C��
//--------------------------------------
void CSoundManager::FadeInSound(int type)
{
	if (m_pSound[type]->Getvol() <= 1.0f &&
		!m_Fade_Ok &&
		m_Fadetype == TYPE_IN)
	{
		m_pSound[type]->volUp(BAN_BGM);
		SetVolume(m_type, m_pSound[type]->Getvol());
	}
	else
	{
		if (m_Fadetype == TYPE_IN)
		{
			m_Fade_Ok = true;
			m_Fadetype = TYPE_NONE;
		}
	}
}

//--------------------------------------
// �t�F�[�h�A�E�g
//--------------------------------------
void CSoundManager::FadeOutSound(std::string type)
{
	FadeOutSound(m_pathName[type]);
}

//--------------------------------------
// �t�F�[�h�A�E�g
//--------------------------------------
void CSoundManager::FadeOutSound(int type)
{
	if (m_pSound[type]->Getvol() >= 0.0f &&
		!m_Fade_Ok &&
		m_Fadetype == TYPE_OUT)
	{
		m_pSound[type]->volDown(BAN_BGM);
		SetVolume(m_type, m_pSound[type]->Getvol());
	}
	else
	{
		if (m_Fadetype == TYPE_OUT)
		{
			m_Fade_Ok = true;
			m_Fadetype = TYPE_NONE;
		}
	}
}

//--------------------------------------
//�T�E���h�p�X�̃��[�h
//--------------------------------------
void CSoundManager::LoadPath()
{
	std::ifstream ifs("Data/JSON/SoundPath.json");
	nlohmann::json Loadfile;
	ifs >> Loadfile;

	if (ifs)
	{
		for (int i = 0;i < (int)Loadfile["SOUND"].size();i++)
		{
			m_pathName.emplace(UTF8toSjis(Loadfile["SOUND"][i]["NAME"]), i);

			// �p�X�̏�����
			SPath path;
			path.path = UTF8toSjis(Loadfile["SOUND"][i]["PATH"]);	// �p�X����
			path.isLoop = Loadfile["SOUND"][i]["LOOP"];	// �J��Ԃ����ۂ�����
			m_path.emplace(i, path);
		}
	}
}

//--------------------------------------
//�T�E���h���[�h
//--------------------------------------
void CSoundManager::Load()
{
	//std::string inString = "MAIN";

	//std::ifstream ifs("Data/JSON/SoundDate.json");
	//nlohmann::json Loadfile;
	//ifs >> Loadfile;

	//if (ifs)
	//{
	//	if (Loadfile["SOUND"].count(inString) == 0)
	//	{// �ĂԒl�����݂��邩
	//		assert(false);
	//	}

	//	// �l�̏o��
	//	SData soundData;
	//	soundData.volume = Loadfile["SOUND"][inString]["VOLUME"];
	//	soundData.rate = Loadfile["SOUND"][inString]["PITCH"];

	//	data.emplace(inString, soundData);

	//	m_volume = Loadfile["SOUND"]["TYPE1"];
	//	m_frate = Loadfile["PITCH"];
	//	m_type = Loadfile["SOUND_TYPE"];
	//	m_Switch = Loadfile["SOUND_NUM"];

	//	Select();

	//	CSound::SetRate(m_type, m_frate);
	//}
}

//--------------------------------------
// �Z�[�u
//--------------------------------------
void CSoundManager::Save()
{
	nlohmann::json Savefile;

	//Savefile["VOLUME"] = m_volume;
	//Savefile["PITCH"] = m_frate;
	//Savefile["SOUND_TYPE"] = m_type;
	//Savefile["SOUND_NUM"] = m_Switch;

	SaveJson(Savefile, "Data/JSON/SoundData.json");
}

//--------------------------------------
// �Đ�
//--------------------------------------
HRESULT CSoundManager::Play(std::string inName)
{
	return Play(m_pathName[inName]);
}

//--------------------------------------
// �Đ�
//--------------------------------------
HRESULT CSoundManager::Play(int inName)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[inName];
	buffer.pAudioData = m_apDataAudio[inName];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_path[inName].isLoop;

	// ��Ԏ擾
	m_apSourceVoice[inName]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
	 // �ꎞ��~
		m_apSourceVoice[inName]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_apSourceVoice[inName]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_apSourceVoice[inName]->SubmitSourceBuffer(&buffer);

	// �Đ�
	m_apSourceVoice[inName]->Start(0);

	return S_OK;
}

//--------------------------------------
//�T�E���h��~
//--------------------------------------
void CSoundManager::Stop(std::string type)
{
	Stop(m_pathName[type]);
}

//--------------------------------------
//�T�E���h��~
//--------------------------------------
void CSoundManager::Stop(int type)
{
	XAUDIO2_VOICE_STATE xa2state;

	// ��Ԏ擾
	m_apSourceVoice[type]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
	 // �ꎞ��~
		m_apSourceVoice[type]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_apSourceVoice[type]->FlushSourceBuffers();
	}
}

//--------------------------------------
//�T�E���h�A�b�v
//--------------------------------------
void CSoundManager::SetFadeIn(std::string type)
{
	SetFadeIn(m_pathName[type]);
}

//--------------------------------------
//�T�E���h�A�b�v
//--------------------------------------
void CSoundManager::SetFadeIn(int type)
{
	m_pSound[type]->Setvol(0.0f);
	m_Fadetype = TYPE_IN;
	m_Fade_Ok = false;
}

//--------------------------------------
//�T�E���h�_�E��
//--------------------------------------
void CSoundManager::FadeOut()
{
	m_Fadetype = TYPE_OUT;
	m_Fade_Ok = false;
}

//--------------------------------------
//���̊m�F
//--------------------------------------
HRESULT CSoundManager::CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��擪�Ɉړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, nullptr) == 0)
		{// �`�����N�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, nullptr) == 0)
		{// �`�����N�f�[�^�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, nullptr) == 0)
			{// �t�@�C���^�C�v�̓ǂݍ���
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, nullptr, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//--------------------------------------
//���̓ǂݎ��
//--------------------------------------
HRESULT CSoundManager::ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^���w��ʒu�܂ňړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, nullptr) == 0)
	{// �f�[�^�̓ǂݍ���
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
