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
#include "input.h"
#include <assert.h>
#include "soundmanager.h"
#include "sound.h"
#include <fstream>
#include "nlohmann/json.hpp"
#include "letter.h"
#include "file.h"
#include <unordered_map>

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
	m_apSourceVoice[SOUND_MAX] = {};	// �\�[�X�{�C�X
	m_apDataAudio[SOUND_MAX] = {};		// �I�[�f�B�I�f�[�^
	m_aSizeAudio[SOUND_MAX] = {};		// �I�[�f�B�I�f�[�^�T�C�Y

	m_Switch = 0;
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
//������
//--------------------------------------
//--------------------------------------
CSound * CSoundManager::SoundCreate(SOUND_TYPE type)
{
	if (m_pSound[type] != nullptr)
	{
		Init();
		m_pSound[type] = new CSound;
		LoadPath();
		m_pSound[type]->Init();
	}

	return m_pSound[type];
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
//�X�V�@�@�ȉ��T�v
//�@�\�Ƃ��Ă�����̂��T���v���Ƃ���
HRESULT CSoundManager::Init()
{
	m_Switch = 0;		//�Z���N�g���Ɏg�p����ϐ��̏�����
	LoadPath();			//�����̓Ǎ�
//�m�F�\
	HRESULT hr;			//HRESULT�^�̕ϐ�
//--------------------------------------
	// COM���C�u�����̏�����
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

	CSoundManager* manager = CApplication::GetSoundManager();

	// �T�E���h�f�[�^�̏�����
	for (int nCntSound = 0; nCntSound < SOUND_MAX; nCntSound++)
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
		hFile = CreateFile((LPCSTR)manager->GetPath(nCntSound).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
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
		buffer.LoopCount = manager->GetLoop(nCntSound);

		// �I�[�f�B�I�o�b�t�@�̓o�^
		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}

	return S_OK;
}

//--------------------------------------
//�X�V�@�@�ȉ��T�v
//�@�\�Ƃ��Ă�����̂��T���v���Ƃ���
//�m�F�\
//--------------------------------------
void CSoundManager::Update()
{
	//�ȑւ�
	ChangeSound();

	//���ʕύX
	SoundVolume(m_type);

	//�T�E���h�t�F�[�h�C��
	FadeInSound(m_type);

	//�T�E���h�t�F�[�h�A�E�g
	FadeOutSound(m_type);

	//�s�b�`����
	ShiftRate(m_type);

	//�T�E���h�̃p�����[�^�[�ۑ�
	SoundParameter();
}

//--------------------------------------
//���y�g�p�Ǘ�
//--------------------------------------
HRESULT CSoundManager::Play(SOUND_TYPE type)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[type];
	buffer.pAudioData = m_apDataAudio[type];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_path[type].isLoop;

	// ��Ԏ擾
	m_apSourceVoice[type]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
	 // �ꎞ��~
		m_apSourceVoice[type]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		m_apSourceVoice[type]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	m_apSourceVoice[type]->SubmitSourceBuffer(&buffer);

	// �Đ�
	m_apSourceVoice[type]->Start(0);

	return S_OK;
}


//--------------------------------------
//�T�E���h��~
//--------------------------------------
void CSoundManager::Stop(SOUND_TYPE type)
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
//�S�ẴT�E���h��~
//--------------------------------------
void CSoundManager::Stop()
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_MAX; nCntSound++)
	{
		if (m_apSourceVoice[nCntSound])
		{
			// �ꎞ��~
			m_apSourceVoice[nCntSound]->Stop(0);
		}
	}
}

//--------------------------------------
//���y�g�p�Ǘ�
//--------------------------------------
void CSoundManager::ChangeSound()
{
	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_L))
	{ //�Ȃ̓���ւ�

		m_Switch = (m_Switch + 1) % 12;

		Select();

		if (m_Switch >= 11)
		{
			m_Switch = 0;
		}
	}
}

//--------------------------------------
//���ʊǗ�
//--------------------------------------
void CSoundManager::SoundVolume(SOUND_TYPE type)
{
	if (CApplication::GetInput()->GetkeyboardPress(DIK_UP))
	{
		m_pSound[type]->volUp(UP_DOWN_TRACK);
		SetVolume(m_type, m_pSound[type]->Getvol());
	}

	if (CApplication::GetInput()->GetkeyboardPress(DIK_DOWN))
	{
		m_pSound[type]->volDown(UP_DOWN_TRACK);
		SetVolume(m_type, m_pSound[type]->Getvol());
	}

	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_LEFT))
	{
		m_Fadetype = TYPE_IN;
		m_Fade_Ok = false;
	}

	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_RIGHT))
	{
		m_Fadetype = TYPE_OUT;
		m_Fade_Ok = false;
	}
}

//--------------------------------------
//�T�E���h�t�F�[�h�C��
//--------------------------------------
void CSoundManager::FadeInSound(SOUND_TYPE type)
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
//�T�E���h�t�F�[�h�A�E�g
//--------------------------------------
void CSoundManager::FadeOutSound(SOUND_TYPE type)
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
//�s�b�`����
//--------------------------------------
void CSoundManager::ShiftRate(SOUND_TYPE type)
{
	if (CApplication::GetInput()->GetkeyboardPress(DIK_O))
	{
		m_pSound[type]->PitchUp(TRACK_PITH);
		SetRate(m_type, m_pSound[type]->Getrate());
	}

	if (CApplication::GetInput()->GetkeyboardPress(DIK_P))
	{
		m_pSound[type]->PitchDown(TRACK_PITH);
		SetRate(m_type, m_pSound[type]->Getrate());
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
//�T�E���h�Z�[�u
//--------------------------------------
void CSoundManager::Save()
{
	nlohmann::json Savefile;

	Savefile["VOLUME"] = m_volume;
	Savefile["PITCH"] = m_frate;
	Savefile["SOUND_TYPE"] = m_type;
	Savefile["SOUND_NUM"] = m_Switch;

	SaveJson(Savefile, "Data/JSON/SoundData.json");
}

//--------------------------------------
//�T�E���h�Z�[�u
//--------------------------------------
void CSoundManager::SoundParameter()
{
	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_N))
	{
		Save();
	}

	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_M))
	{
		Load();
	}
}

//--------------------------------------
//�T�E���h�I��
//--------------------------------------
void CSoundManager::Select()
{
	switch (m_Switch)
	{
	case 0:

		/*�ύX�Ȃ�*/

		break;

	case 1:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_BATTLEBAN);
		m_type = SOUND_BGM_BATTLEBAN;
		SoundVolume(m_type);
		break;

	case 2:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_LAST_MEETING);
		m_type = SOUND_BGM_LAST_MEETING;
		SoundVolume(m_type);
		break;

	case 3:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_RESULT_BGM);
		m_type = SOUND_BGM_RESULT_BGM;
		SoundVolume(m_type);
		break;

	case 4:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_VIRTUAL);
		m_type = SOUND_BGM_VIRTUAL;
		SoundVolume(m_type);
		break;

	case 5:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_BATTLEMEETING_VER_2);
		m_type = SOUND_BGM_BATTLEMEETING_VER_2;
		SoundVolume(m_type);

		break;

	case 6:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_NO_NAME);
		m_type = SOUND_BGM_NO_NAME;
		SoundVolume(m_type);
		break;

	case 7:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_CHALLENGE_TO_TOMORROW);
		m_type = SOUND_BGM_CHALLENGE_TO_TOMORROW;
		SoundVolume(m_type);
		break;

	case 8:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_BATTLEMEETING);
		m_type = SOUND_BGM_BATTLEMEETING;
		SoundVolume(m_type);
		break;

	case 9:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_HEART);
		m_type = SOUND_BGM_HEART;
		SoundVolume(m_type);
		break;

	case 10:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_DRUM_VOICE);
		m_type = SOUND_BGM_DRUM_VOICE;
		SoundVolume(m_type);
		break;

	case 11:
		FadeIn(m_type);
		CSoundManager::Stop();
		CSoundManager::Play(SOUND_BGM_RANKING);
		m_type = SOUND_BGM_RANKING;
		SoundVolume(m_type);
		break;

	default:
		assert(false);
		break;
	}
}

//--------------------------------------
//�T�E���h�A�b�v
//--------------------------------------
void CSoundManager::FadeIn(SOUND_TYPE type)
{
	m_pSound[type]->Setvol(0.0f);
	m_Fadetype = TYPE_IN;
	m_Fade_Ok = false;
}

//--------------------------------------
//�T�E���h�_�E��
//--------------------------------------
void CSoundManager::FadeOut(SOUND_TYPE type)
{
	m_Fadetype = TYPE_OUT;
	m_Fade_Ok = false;
}

//--------------------------------------
//�I��
//--------------------------------------
void CSoundManager::Uninit()
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_MAX; nCntSound++)
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
//���ʐݒ�
//--------------------------------------
void CSoundManager::SetVolume(SOUND_TYPE type, float fVolume)
{
	m_apSourceVoice[type]->SetVolume(fVolume);
}

//--------------------------------------
//�s�b�`����
//--------------------------------------
void CSoundManager::SetRate(SOUND_TYPE type, float rate)
{
	m_apSourceVoice[type]->SetFrequencyRatio(rate);
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
