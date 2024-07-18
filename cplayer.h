//音乐播放器的头文件
//32位程序版本 
#ifndef __CPLAYER_H__
#define __CPLAYER_H__
#define DebugLog(...)
class CPlayer
{
public:
	CPlayer();
	~CPlayer();
public:
	BOOL Open(LPCTSTR lpFileName);//打开文件  
	BOOL Play(DWORD dwPos = 0);//播放  
	BOOL Close();//关闭  
	BOOL Stop();//停止  
	BOOL Pause();//暂停  
	BOOL Continue();//继续  
	BOOL SetVolume(DWORD dSize);
	BOOL SetPos(DWORD dwPos);
	BOOL SetAudio(bool AudioOff);
	BOOL SetSpeed(DWORD Speed);
	DWORD GetLength();//获取歌曲长度  
	BOOL GetError(LPTSTR pszText, UINT cchText);
	DWORD GetPos();
	void SetWindowHwnd(HWND hWnd);//设置主窗口句柄  
private:
	MCI_OPEN_PARMS m_ciOpen;//打开设备参数  
	HWND m_hWnd;//主窗口句柄  
	DWORD m_dwFrom;//播放起始点
	MCIERROR m_dwError;
};



//#include "stdafx.h"
//#include "Player.h"
#include <digitalv.h>

#pragma comment(lib, "Winmm.lib")
#define _USER_MCI_COMMAND


CPlayer::CPlayer()
	: m_hWnd(nullptr)
	, m_dwFrom(0)
	, m_dwError(0)
{
	ZeroMemory(&m_ciOpen, sizeof(MCI_OPEN_PARMS));
}


CPlayer::~CPlayer()
{
	if (m_ciOpen.wDeviceID)///如果有打开的MCI设备就关闭  
		Close();
}

/// 打开设备
BOOL CPlayer::Open(LPCTSTR lpFileName)
{
	DebugLog(__func__);
	TCHAR lpszShortPath[MAX_PATH] = { 0 };
	GetShortPathName(lpFileName, lpszShortPath, _countof(lpszShortPath));// 获得短路经
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)///如果有打开的MCI设备就关闭  
		Close();
	m_ciOpen.dwCallback = (DWORD) &(m_hWnd);
	DebugLog(__func__);
	m_ciOpen.lpstrDeviceType = NULL;
	m_ciOpen.lpstrElementName = lpszShortPath;///播放文件路径
	m_ciOpen.lpstrAlias = _T("MusicAlias");
	if (m_dwError = mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD) &(m_ciOpen)))///打开设备时，设备号为0
	{
		DebugLog(__func__);
		return FALSE;
	}
	m_dwFrom = 0;
	return TRUE;
#else
	mciSendString(_T("Stop MusicAlias"), nullptr, 0, m_hWnd);
	TCHAR lpstrCommand[MAX_PATH * 2] = {};
	_stprintf_s(lpstrCommand, _T("Open %s Alias MusicAlias"), lpszShortPath);// MusicAlias 是设备别名
	return !(m_dwError = mciSendString(lpstrCommand, nullptr, 0, m_hWnd));
#endif
}

/// 播放音乐  
BOOL CPlayer::Play(DWORD dwPos)
{
	DebugLog(__func__);
	m_dwFrom = dwPos;
#ifdef _USER_MCI_COMMAND
	MCI_PLAY_PARMS mciplayparms;///播放参数结构  	
	DWORD cdlen = GetLength();///得到文件大小  
	mciplayparms.dwCallback = (DWORD) &(m_hWnd);
	mciplayparms.dwFrom = m_dwFrom;///设置起始位置  
	mciplayparms.dwTo = cdlen;///设置终止位置  
	if (m_ciOpen.wDeviceID != 0)///判断是否打开文件  
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_PLAY, MCI_TO | MCI_FROM, (DWORD) &(mciplayparms)));
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Play MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

/// 执行MCI_CLOSE操作，关闭MCI设备  
BOOL CPlayer::Close()
{
	DebugLog(__func__);
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_CLOSE, 0, 0));
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Close MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

/// 执行MCI_STOP操作，停止播放音乐  
BOOL CPlayer::Stop()
{
	DebugLog(__func__);
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
	{
		if (0 == mciSendCommand(m_ciOpen.wDeviceID, MCI_STOP, 0, 0))
			return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, 0));///把播放位置设定为音乐文件的开头（下一次播放操作从文件开头位置开始）  
	}
	m_dwFrom = 0;
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Stop MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

/// 执行MCI_PAUSE操作，暂停播放音乐 
BOOL CPlayer::Pause()
{
	DebugLog(__func__);
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
	{
		DWORD dwsf = GetPos();
		m_dwFrom = MCI_MAKE_MSF(MCI_MSF_MINUTE(dwsf), MCI_MSF_SECOND(dwsf), MCI_MSF_FRAME(dwsf));
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_PAUSE, 0, 0));
	}
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Pause MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

BOOL CPlayer::Continue()
{
	DebugLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_PLAY_PARMS mciplayparms;///播放参数结构  	
	DWORD cdlen = GetLength();///得到文件大小  
	mciplayparms.dwCallback = (DWORD) &(m_hWnd);
	mciplayparms.dwTo = cdlen;///设置终止位置  
	if (m_ciOpen.wDeviceID != 0)///判断是否打开文件  
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_PLAY, MCI_TO, (DWORD) &(mciplayparms)));
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Play MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

/// 得到文件大小  
DWORD CPlayer::GetLength()
{
	DebugLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback = (DWORD) &(m_hWnd);
	mcistatusparms.dwItem = MCI_STATUS_LENGTH;
	mcistatusparms.dwReturn = 0;
	m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD) &(mcistatusparms));
	return mcistatusparms.dwReturn;/// 单位是毫秒 mm
#else
	TCHAR sPosition[256];
	DWORD lLength;
	mciSendString(_T("Status MusicAlias length"), sPosition, 255, m_hWnd);
	lLength = _tcstol(sPosition, nullptr, 10);
	return lLength;/// 单位是毫秒 mm
#endif
}

/// 得到文件播放位置
DWORD CPlayer::GetPos()
{
	DebugLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback = (DWORD) &(m_hWnd);
	mcistatusparms.dwItem = MCI_STATUS_POSITION;
	mcistatusparms.dwReturn = 0;
	m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD) &(mcistatusparms));
	return mcistatusparms.dwReturn;
#else
	TCHAR sPosition[256];
	DWORD lLength;
	mciSendString(_T("Status MusicAlias position"), sPosition, 255, m_hWnd);
	lLength = _tcstol(sPosition, nullptr, 10);
	return lLength;/// 单位是毫秒 mm
#endif
}

void CPlayer::SetWindowHwnd(HWND hWnd)
{
	DebugLog(__func__);
	m_hWnd = hWnd;
}

/// 设置音量 0-100，百分比
BOOL CPlayer::SetVolume(DWORD dSize)
{
	DebugLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_DGV_SETAUDIO_PARMS mciSetAudioPara;
	mciSetAudioPara.dwCallback = (DWORD) &(m_hWnd);
	mciSetAudioPara.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	mciSetAudioPara.dwValue = dSize * 10; ///此处就是音量大小 (0--1000)
	return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD)&(mciSetAudioPara)));
#else
	TCHAR lpstrCommand[32] = {};
	_stprintf_s(lpstrCommand, _T("setaudio MusicAlias volume to %ld"), dSize);
	return !(m_dwError = mciSendString(lpstrCommand, nullptr, 0, m_hWnd));
#endif
}

/// 设置进度
BOOL CPlayer::SetPos(DWORD dwPos)
{
	DebugLog(__func__);
	m_dwFrom = dwPos;
#ifdef _USER_MCI_COMMAND
	DWORD dwLength = GetLength();///得到文件大小
	MCI_SEEK_PARMS Seek;
	Seek.dwCallback = (DWORD)&(m_hWnd);
	Seek.dwTo = dwPos;
	DWORD dwFlags = MCI_WAIT;
	if (dwPos > 0 && dwPos < dwLength)
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_WAIT | MCI_TO, (DWORD) &(Seek)));
	else if (dwPos >= dwLength)
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_WAIT | MCI_SEEK_TO_END, 0));
	return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_WAIT | MCI_SEEK_TO_START, 0));
#else
	TCHAR lpstrCommand[32] = {};
	_stprintf_s(lpstrCommand, _T("seek MusicAlias to %ld"), dwPos);
	return !(m_dwError = mciSendString(lpstrCommand, nullptr, 0, m_hWnd));
#endif
}

/// 静音True为静音，FALSE为取消静音
BOOL CPlayer::SetAudio(bool AudioOff)
{
	DebugLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_SET_PARMS setAudio;
	setAudio.dwCallback = (DWORD)&(m_hWnd);
	setAudio.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	setAudio.dwAudio = MCI_SET_AUDIO_ALL;
	DWORD dwFlags = MCI_SET_AUDIO;
	if (AudioOff)
		dwFlags |= MCI_SET_ON;
	else
		dwFlags |= MCI_SET_OFF;
	return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SET, dwFlags, (DWORD) &(setAudio)));
#else
	if (AudioOff)
		return !(m_dwError = mciSendString(_T("setaudio MusicAlias off"), nullptr, 0, m_hWnd));
	else
		return !(m_dwError = mciSendString(_T("setaudio MusicAlias on"), nullptr, 0, m_hWnd));
#endif
}

/// 设置播放速度1-100，百分比
BOOL CPlayer::SetSpeed(DWORD Speed)
{
	DebugLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_DGV_SET_PARMS setSpeed;
	setSpeed.dwCallback = (DWORD)&(m_hWnd);
	//setSpeed.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	//setSpeed.dwAudio = MCI_SET_AUDIO_ALL;
	//setSpeed.dwFileFormat = 0;
	setSpeed.dwSpeed = Speed * 10;/// 设置播放速度1-2000,1000为正常速度
	DWORD dwFlags = MCI_DGV_SET_SPEED;
	return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SET, dwFlags, (DWORD) &(setSpeed)));
#else
	TCHAR lpstrCommand[32] = {};
	_stprintf_s(lpstrCommand, _T("set MusicAlias speed %ld"), Speed);
	return !(m_dwError = mciSendString(lpstrCommand, nullptr, 0, m_hWnd));
#endif
}

/// 获得错误描述
BOOL CPlayer::GetError(LPTSTR pszText, UINT cchText)
{
	return mciGetErrorString(m_dwError, pszText, cchText);
}
//from https://blog.csdn.net/u010682281/article/details/66473313
//↑ 请支持作者！ 
#endif
