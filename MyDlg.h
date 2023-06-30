
// MyDlg.h: файл заголовка
//

#pragma once
#include"Drawer1.h"


// Диалоговое окно MyDlg
class MyDlg : public CDialogEx
{
// Создание
public:
	MyDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION3_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	Drawer drv;
	Drawer drv2;
	Drawer graphEk;
	Drawer graphEp;
	afx_msg void OnBnClickedOk();
	afx_msg void OnStnClickedPicture();
	afx_msg void OnBnClickedPrint();
	// Текущая координата х в 3D
	double x;
	// Текущее значение координаты y в 3D
	double y;
	// Текущее значение z в 3D
	double z;
	// Значение Х в 2D
	double X;
	// Значение у в 2D
	double Y;

	bool STOP;
	bool PAUSE;
	afx_msg void OnBnClickedButton1();
	CEdit x3d;
	CEdit y3d;
	CEdit z3d;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnUpdateOffer();
	HANDLE th;
	/* Широта точки А */
	// Широта точки А
	double ShirotaA;
	// Долгота точки А
	double DolgotaA;
	// Управление полем широты А
	CEdit ShirotaACntrl;
	// Упраление полем долготы А
	CEdit DolgotaACntrl;
	// Высота подъема
	double RecY;
	// Модуль начальной скорости
	double V0abs;
	// Азимут
	double RecA;
	// Управление Начальной скоростью
	CEdit V0Cntrl;
	// Управление Азимутом
	CEdit RecACntrl;
	// Управление угол подъема
	CEdit RecYCntrl;
	// Управление кнопкой паузы
	CButton PAUSECTRL;
	afx_msg void OnBnClickedButton3();
	// Пройденный путь
	double PutS;
	double g;
	// Широта В
	double ShirotaB;
	// Долгота В
	double dolgotaB;
	// Контроль широты В
	CEdit ShirotaBCntrl;
	// Контроль широты В
	CEdit DolgotaBCntrl;
	bool STOP_HELP;
	afx_msg void OnBnClickedButton4();
	void OnBnClickedHelp();
	// Текущая географическая координата широты
	double ShirotaCur;
	// Текущее значение долготы в Гео координатах
	double DolgotaCur;
	// Текущая высота от земли
	double CurrentH;
	// модуль вектора из точки попадания в точку В
//	double ABS_CURPOS_B;
//	CString WhatHelp;
	// //TRUE если сопротивление нужно
	BOOL NeedSopr;
	// TRUE если g = const
	BOOL NeedGConst;
	CButton GconstCntrl;
	CButton NeedSoprCntrl;
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	CEdit DelayControl;
	int Delay;
	CEdit dtcontrol;
	double dt;
	double Plotnost;
	double DalnostPoleta;
//	BOOL FindLeft;
	CButton FindLeftControl;
//	BOOL FindRight;
	CButton FindRightControl;
//	BOOL FindTop;
	CButton FindTopControl;
	CButton FindBotControl;
//	BOOL FindBot;
	BOOL FindLeft;
	BOOL FindRight;
	BOOL FindTop;
	BOOL FindBot;
	afx_msg void OnBnClickedRadio1();
	CString LeftRight;
	CComboBox LeftRightControl;
	CString TopBot;
	CComboBox TopBotControl;
	double CURRENT_TIME;
	double Cd;
	CEdit CdControl;
	double Diametr;
	CEdit DiametrControl;
	double H0;
	CEdit H0Control;
	CString HELP_ADVICE;
	CButton HELP_CONTROL;
	BOOL IsHelpNeeded;
	double Distanse_btw_curpos_b;
};
