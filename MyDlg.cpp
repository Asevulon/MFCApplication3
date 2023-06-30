
// MyDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication3.h"
#include "Rocket.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно MyDlg


MyDlg::MyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION3_DIALOG, pParent)
	, x(0)
	, y(0)
	, z(0)
	, X(0)
	, Y(0)
	, STOP(false)
	, RecY(0)
	, V0abs(0)
	, RecA(0)
	, PutS(0)
	, ShirotaB(0)
	, dolgotaB(0)
	, ShirotaCur(0)
	, DolgotaCur(0)
	, CurrentH(0)
	, NeedSopr(FALSE)
	, NeedGConst(FALSE)
	, Delay(0)
	, dt(0)
	, Plotnost(0)
	, DalnostPoleta(0)
	, FindLeft(FALSE)
	, FindRight(FALSE)
	, FindTop(FALSE)
	, FindBot(FALSE)
	, LeftRight(_T("Слева"))
	, TopBot(_T("Снизу"))
	, CURRENT_TIME(0)
	, Cd(0.5)
	, Diametr(3)
	, H0(20000)
	, HELP_ADVICE(_T("Жду приказаний"))
	, Distanse_btw_curpos_b(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void MyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, x);
	DDX_Text(pDX, IDC_EDIT2, y);
	DDX_Text(pDX, IDC_EDIT3, z);
	//  DDX_Control(pDX, IDC_EDIT1, x3d);
	//  DDX_Control(pDX, IDC_EDIT2, y3d);
	//  DDX_Control(pDX, IDC_EDIT3, z3d);
	DDX_Control(pDX, IDC_EDIT1, x3d);
	DDX_Control(pDX, IDC_EDIT2, y3d);
	DDX_Control(pDX, IDC_EDIT3, z3d);
	DDX_Text(pDX, IDC_EDIT6, ShirotaA);
	DDX_Text(pDX, IDC_EDIT7, DolgotaA);
	DDX_Control(pDX, IDC_EDIT6, ShirotaACntrl);
	DDX_Control(pDX, IDC_EDIT7, DolgotaACntrl);
	DDX_Text(pDX, IDC_EDIT10, RecY);
	DDX_Text(pDX, IDC_EDIT8, V0abs);
	DDX_Text(pDX, IDC_EDIT9, RecA);
	DDX_Control(pDX, IDC_EDIT8, V0Cntrl);
	DDX_Control(pDX, IDC_EDIT9, RecACntrl);
	DDX_Control(pDX, IDC_EDIT10, RecYCntrl);
	DDX_Control(pDX, IDC_BUTTON1, PAUSECTRL);
	DDX_Text(pDX, IDC_EDIT11, PutS);
	DDX_Text(pDX, IDC_EDIT12, g);
	DDX_Text(pDX, IDC_EDIT13, ShirotaB);
	DDX_Text(pDX, IDC_EDIT14, dolgotaB);
	DDX_Control(pDX, IDC_EDIT13, ShirotaBCntrl);
	DDX_Control(pDX, IDC_EDIT14, DolgotaBCntrl);
	DDX_Text(pDX, IDC_EDIT16, ShirotaCur);
	DDX_Text(pDX, IDC_EDIT15, DolgotaCur);
	DDX_Text(pDX, IDC_EDIT17, CurrentH);
	DDX_Check(pDX, IDC_CHECK2, NeedSopr);
	DDX_Check(pDX, IDC_CHECK1, NeedGConst);
	DDX_Control(pDX, IDC_CHECK1, GconstCntrl);
	DDX_Control(pDX, IDC_CHECK2, NeedSoprCntrl);
	DDX_Control(pDX, IDC_EDIT20, DelayControl);
	DDX_Text(pDX, IDC_EDIT20, Delay);
	DDX_Control(pDX, IDC_EDIT21, dtcontrol);
	DDX_Text(pDX, IDC_EDIT21, dt);
	DDX_Text(pDX, IDC_EDIT22, Plotnost);
	DDX_Text(pDX, IDC_EDIT23, DalnostPoleta);
	DDX_Text(pDX, IDC_EDIT18, CURRENT_TIME);
	DDX_Text(pDX, IDC_EDIT19, Cd);
	DDX_Control(pDX, IDC_EDIT19, CdControl);
	DDX_Text(pDX, IDC_EDIT24, Diametr);
	DDX_Control(pDX, IDC_EDIT24, DiametrControl);
	DDX_Text(pDX, IDC_EDIT25, H0);
	DDX_Control(pDX, IDC_EDIT25, H0Control);
	DDX_Control(pDX, IDC_BUTTON3, HELP_CONTROL);
	DDX_Text(pDX, IDC_EDIT27, Distanse_btw_curpos_b);
}

BEGIN_MESSAGE_MAP(MyDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &MyDlg::OnBnClickedPrint)
	//ON_STN_CLICKED(IDC_PICTURE, &MyDlg::OnStnClickedPicture)
	ON_BN_CLICKED(IDC_PRINT,&MyDlg::OnBnClickedPrint)
	ON_BN_CLICKED(IDC_BUTTON2, &MyDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &MyDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_OUT, &MyDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON3,&MyDlg::OnBnClickedHelp)
	ON_BN_CLICKED(IDC_CHECK1, &MyDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &MyDlg::OnBnClickedCheck2)
	//ON_BN_CLICKED(IDC_BUTTON4,&MyDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// Обработчики сообщений MyDlg

BOOL MyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	drv.Create(GetDlgItem(IDC_PICTURE)->GetSafeHwnd());
	drv2.Create(GetDlgItem(IDC_PICTURE2)->GetSafeHwnd());
	graphEk.Create(GetDlgItem(IDC_GRAPH_EK)->GetSafeHwnd());
	graphEp.Create(GetDlgItem(IDC_GRAPH_EP)->GetSafeHwnd());
	STOP = false;
	PAUSE = FALSE;
	STOP_HELP = false;
	NeedGConst = FALSE;
	NeedSopr = TRUE;
	ShirotaA = 0;
	DolgotaA = 0;
	V0abs = 7900;
	RecA = 90;
	RecY = 90;
	Delay = 0;
	dt = 1;
	FindLeft = FALSE;
	FindRight = TRUE;
	FindTop = FALSE;
	FindBot = TRUE;
	IsHelpNeeded = FALSE;
	UpdateData(FALSE);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void MyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
		
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR MyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void MyDlg::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialogEx::OnOK();
}


void MyDlg::OnStnClickedPicture()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void MyDlg::OnBnClickedPrint()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData(TRUE);
	if (PAUSE == true)OnBnClickedButton1();
	if (ShirotaA > 180 || ShirotaA < -180) return;
	if (DolgotaA > 180 || DolgotaA < -180)return;
	if (V0abs <= 0)return;
	if (RecA > 180 || RecA < -180)return;
	if (RecY > 180 || RecY < 0)return;
	OnBnClickedButton2();

	H0Control.EnableWindow(FALSE);
	DiametrControl.EnableWindow(FALSE);
	CdControl.EnableWindow(FALSE);
	TopBotControl.EnableWindow(FALSE);
	LeftRightControl.EnableWindow(FALSE);
	dtcontrol.EnableWindow(FALSE);
	DelayControl.EnableWindow(FALSE);
	GconstCntrl.EnableWindow(FALSE);
	NeedSoprCntrl.EnableWindow(FALSE);
	ShirotaBCntrl.EnableWindow(FALSE);
	DolgotaBCntrl.EnableWindow(FALSE);
	ShirotaACntrl.EnableWindow(FALSE);
	DolgotaACntrl.EnableWindow(FALSE);
	V0Cntrl.EnableWindow(FALSE);
	RecACntrl.EnableWindow(FALSE);
	RecYCntrl.EnableWindow(FALSE);
	HELP_CONTROL.EnableWindow(FALSE);
	Rocket rocket;
	rocket.SetA(ShirotaA, DolgotaA, H0);
	rocket.SetV0(V0abs, RecA, RecY);
	rocket.StartRocket(*this, drv, NeedGConst, NeedSopr, Delay, dt);
	if (IsHelpNeeded)rocket.HELP_ADVICE(*this);
	ShirotaACntrl.EnableWindow(TRUE);
	DolgotaACntrl.EnableWindow(TRUE);
	V0Cntrl.EnableWindow(TRUE);
	RecACntrl.EnableWindow(TRUE);
	RecYCntrl.EnableWindow(TRUE);
	ShirotaBCntrl.EnableWindow(TRUE);
	DolgotaBCntrl.EnableWindow(TRUE);
	GconstCntrl.EnableWindow(TRUE);
	NeedSoprCntrl.EnableWindow(TRUE);
	DelayControl.EnableWindow(TRUE);
	dtcontrol.EnableWindow(TRUE);
	TopBotControl.EnableWindow(TRUE);
	LeftRightControl.EnableWindow(TRUE);
	CdControl.EnableWindow(TRUE);
	DiametrControl.EnableWindow(TRUE);
	H0Control.EnableWindow(TRUE);
	HELP_CONTROL.EnableWindow(TRUE);
}


void MyDlg::OnBnClickedButton1()
{
	// TODO: добавьте свой код обработчика уведомлений
	if (STOP == true)return;
	if (PAUSE == false)
	{
		PAUSE = true;
		PAUSECTRL.SetWindowText(L"Возобновить");
	}
	else
	{
		PAUSE = false;
		PAUSECTRL.SetWindowText(L"Пауза");
	}
}


void MyDlg::OnBnClickedButton2()
{
	if (PAUSE == TRUE)OnBnClickedButton1();
	STOP = true;
	
	// TODO: добавьте свой код обработчика уведомлений
}


void MyDlg::OnUpdateOffer()
{
	UpdateData(FALSE);
}



void MyDlg::OnBnClickedButton3()
{
	STOP_HELP = true;
	STOP = true;
	CDialogEx::OnOK();
}


void MyDlg::OnBnClickedButton4()
{
	// TODO: добавьте свой код обработчика уведомлений
	ShirotaA = 56.3287;
	DolgotaA = 44.002;
	ShirotaB = 38.8951;
	dolgotaB = -77.0364;
	V0abs = 7840;
	RecY = 20;
	RecA = -50;
	if (IsHelpNeeded == FALSE)OnBnClickedHelp();
	UpdateData(FALSE);
}

void MyDlg::OnBnClickedHelp()
{
	if (IsHelpNeeded)
	{
		IsHelpNeeded = FALSE;
		HELP_CONTROL.SetWindowTextW(L"Отображать");
	}
	else
	{
		HELP_CONTROL.SetWindowTextW(L"Отмена");
		IsHelpNeeded = TRUE;
	}
}

void MyDlg::OnBnClickedCheck1()
{
	// TODO: добавьте свой код обработчика уведомлений
	/*
	if (NeedGConst == FALSE)
	{
		NeedGConst = TRUE;
	}
	else
	{
		NeedGConst = FALSE;
	}
	UpdateData(FALSE);
	*/
}


void MyDlg::OnBnClickedCheck2()
{
	// TODO: добавьте свой код обработчика уведомлений
	/*if (NeedSopr == TRUE)
	{
		NeedSopr = FALSE;
	}
	else
	{
		NeedSopr = TRUE;
	}
	UpdateData(FALSE);
	*/
}

