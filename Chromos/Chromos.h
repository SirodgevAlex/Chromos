﻿
// Chromos.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CChromosApp:
// Сведения о реализации этого класса: Chromos.cpp
//

class CChromosApp : public CWinApp
{
public:
	CChromosApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CChromosApp theApp;
