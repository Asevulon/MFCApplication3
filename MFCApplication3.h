
// MFCApplication3.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// MyApp:
// Сведения о реализации этого класса: MFCApplication3.cpp
//

class MyApp : public CWinApp
{
public:
	MyApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern MyApp theApp;
