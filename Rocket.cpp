#include"pch.h"
#include"Rocket.h"
using namespace std;
Rocket::Rocket()
{
	pi = 4 * atan(1);
	A.s = DegToRad(56.3287);
	A.d = DegToRad(44.002);
	REarth = 6371000;
	w = 2 * pi / (24 * 60 * 60);
	dt = 60. * 90. * 0.0001;
	a = GeoToDek(A);
	CurPos = a;
	m = 20000;
	G = 6.674*pow(10.,-11);
	M = 6*pow(10.,24);
	S = pi * 9. / 4.;
	Cd = 0.04;
	Calc_g();
	RGoal = 20000;
	pl = 0;
}
//поворот вектора скорости для задания н.у.
void Rocket::RotateV()
{
	double x(v.x), y(v.y), z(v.z);
	double AA(DegToRad(90.)-A.s),//поворот по х
		B(DegToRad(90.)+A.d);//Поворот по z
	v.x = x * cos(B) - (y * cos(AA) - z * sin(AA)) * sin(B);
	v.y = cos(B) * (y * cos(AA) - z * sin(AA)) + x * sin(B);
	v.z = z * cos(AA) + y * sin(AA);
}

double Rocket::DegToRad(double Deg)
{
	return Deg * pi / 180.;
}
double Rocket::RadToDeg(double Rad)
{
	return Rad * 180. / pi;
}

void  Rocket::SetA(double s, double d, double h0)
{
	A.s = DegToRad(s);
	A.d = DegToRad(d);
	a = GeoToDek_For_A(A, h0);
}//устанавливает точку А
Rocket::GeoC Rocket::GetA()
{
	return A;
}

void  Rocket::SetB(double s, double d)
{
	B.s = DegToRad(s);
	B.d = DegToRad(d);
	b = GeoToDek(B);
}
Rocket::GeoC Rocket::GetB()
{
	return B;
}

void  Rocket::SetV0(double val, double RecA, double RecY) //сразу перерасчет v c поворотом сис. коорд.
{
	v0.val = val;
	v0.RecA = DegToRad(RecA);
	v0.RecY = DegToRad(RecY);

	v.x = v0.val * cos(v0.RecY) * sin(v0.RecA);
	v.y = v0.val * cos(v0.RecY) * cos(v0.RecA);
	v.z = v0.val * sin(v0.RecY);
	RotateV();
}
Rocket::V0 Rocket::GetV0()
{
	return v0;
}

void  Rocket::SetV(double x, double y, double z)
{
	v.x = x;
	v.y = y;
	v.z = z;
}
Rocket::Coord Rocket::GetV()
{
	return v;
}

Rocket::Coord Rocket::GeoToDek(GeoC X)
{
	Coord res;
	res.x = REarth * cos(X.s) * cos(X.d);
	res.y = REarth * cos(X.s) * sin(X.d);
	res.z = REarth * sin(X.s);
	return res;
}

Rocket::Coord Rocket::GeoToDek_For_A(GeoC X, double h0)
{
	Coord res;
	res.x = (REarth + h0) * cos(X.s) * cos(X.d);
	res.y = (REarth + h0) * cos(X.s) * sin(X.d);
	res.z = (REarth + h0) * sin(X.s);
	return res;
}

void Rocket::CalcVw(Coord& vw)
{
	vw.x = v.y * w;
	vw.y = v.x * w;
}

bool Rocket::StartRocket_no_res()
{
	
	t = 0;
	double kx[4];
	double ky[4];
	double kz[4];
	double x(CurPos.x), y(CurPos.y), z(CurPos.z);
	double x_(v.x), y_(v.y), z_(v.z);
	for (int i = 0; i < 4; i++)
	{
		kx[i] = 0;
		ky[i] = 0;
		kz[i] = 0;
	}
	int i = 0;
	do
	{
		x = CurPos.x;
		y = CurPos.y;
		z = CurPos.z;

		x_ = v.x;
		y_ = v.y;
		z_ = v.z;
		
		//Calc_g();
		t = i * dt;
		kx[0] = f_no_res(y_, x, y, z) * dt;
		ky[0] = g_no_res(x_, x, y, z) * dt;
		kz[0] = h_no_res(x, y, z) * dt;

		kx[1] = f_no_res(y_ + ky[0] / 2., x + x_ * dt / 2., y + y_ * dt / 2., z + z_ * dt / 2.) * dt;
		ky[1] = g_no_res(x_ + kx[0] / 2., x + x_ * dt / 2., y + y_ * dt / 2., z + z_ * dt / 2.) * dt;
		kz[1] = h_no_res(x + x_ * dt / 2., y + y_ * dt / 2., z + z_ * dt / 2.) * dt;

		kx[2] = f_no_res(y_ + ky[1] / 2., x + x_ * dt / 2. + kx[0] * dt / 4., y + y_ * dt / 2. + ky[0] * dt / 4., z + z_ * dt / 2. + kz[0] * dt / 4.) * dt;
		ky[2] = g_no_res(x_ + kx[1] / 2., x + x_ * dt / 2. + kx[0] * dt / 4., y + y_ * dt / 2. + ky[0] * dt / 4., z + z_ * dt / 2. + kz[0] * dt / 4.) * dt;
		kz[2] = h_no_res(x + x_ * dt / 2. + kx[0] * dt / 4., y + y_ * dt / 2. + ky[0] * dt / 4., z + z_ * dt / 2. + kz[0] * dt / 4.) * dt;
		
		kx[3] = f_no_res(y_ + ky[2], x + x_ * dt + kx[1] * dt / 2., y + y_ * dt + ky[1] * dt / 2., z + z_ * dt + kz[1] * dt / 2.) * dt;
		ky[3] = g_no_res(x_ + kx[2], x + x_ * dt + kx[1] * dt / 2., y + y_ * dt + ky[1] * dt / 2., z + z_ * dt + kz[1] * dt / 2.) * dt;
		kz[3] = h_no_res(x + x_ * dt + kx[1] * dt / 2., y + y_ * dt + ky[1] * dt / 2., z + z_ * dt + kz[1] * dt / 2.) * dt;

		CurPos.x = x + x_ * dt + (kx[0] + kx[1] + kx[2]) * dt / 6.;
		CurPos.y = y + y_ * dt + (ky[0] + ky[1] + ky[2]) * dt / 6.;
		CurPos.z = z + z_ * dt + (kz[0] + ky[1] + ky[2]) * dt / 6.;

		v.x = x_ + (kx[0] + 2. * kx[1] + 2. * kx[2] + kx[3]) / 6.;
		v.y = y_ + (ky[0] + 2. * ky[1] + 2. * ky[2] + ky[3]) / 6.;
		v.z = z_ + (kz[0] + 2. * kz[1] + 2. * kz[2] + kz[3]) / 6.;
		
		i++;
		
		if (i > 1000000)return false;

	} while (sqrt(pow(CurPos.x,2)+pow(CurPos.y,2)+pow(CurPos.z,2))>REarth);
	return true;
}

bool Rocket::help(MyDlg& dlg)
{
	dlg.STOP_HELP = false;//предварительно присваеваем ложь, чтобы не было остановки гарантировано
	double CurRecA,//текущий азимут
		CurRecY,//текущий угол вызвышения
		CurMinRecA,//текущий минимальный угол азимута, с которого начинается итерационный процесс
		CurMaxRecA,//текущий максимальный угол азимута, который ограничивает итерационный процесс
		CurMinRecY(0),//текущий минимальный угол возвышения, с которого начинается итерационный процесс
		CurMaxRecY(90);//текущий максимальный угол возвышения, который ограничивает итерационный процесс

	//если точки А и В совпадают, то ищем азимут слева между -135 и -45
	if (B.s == A.s && B.d == A.d)
	{
		CurMinRecA = -135;
		CurMaxRecA = -45;
	}
	//если точки А и В не совпали, то проверяем следущее условие (долгота А совпадает с долготой В)
	else
	{
		//Если долгота А и В совпадают, то делаем проверку на выбор направления (0 или 180)
		if (A.d == B.d)
		{
			//если широта А больше широты В, то ищем снизу от 135 до 225
				if (A.s > B.s)
				{
					CurMinRecA = 135;
					CurMaxRecA = 225;
				}
				//иначе ищем сверху от -45 до 45
				else
				{
					CurMinRecA = -90;
					CurMaxRecA = 90;
				}
		}
		//если долгота А и долгота В не совпадают, то проверяем следующее условие (широты А и В совпадают)
		else
		{
			//если широты совпали, то делаем проверку:
			//если долготы одинаковые по знаку, то 
			//если долгота А больше долготы Б, то ищем азимут от -135 до -45
			if (A.s = B.s)
			{
				if ((A.d >= 0 && B.d >= 0) || (A.d < 0 && B.d < 0))
				{
					if (A.d > B.d)
					{
						CurMinRecA = -135;
						CurMaxRecA = -45;
					}
					//иначе ищем угол от 45 до 135
					else
					{
						CurMinRecA = 45;
						CurMaxRecA = 135;
					}
				}
				//иначе
				//если долгота А неотрицательная, а долгота В отрицательная, то добавляем к отрицательной 2пи и смотрим какая из дуг короче:левая или правая
				else
				{
					if (A.d >= 0 && B.d < 0)
					{
						if (DegToRad(360) + B.d - A.d < A.d - B.d)
						{
							CurMinRecA = 45;
							CurMaxRecA = 135;
						}
						else
						{
							CurMinRecA = -135;
							CurMaxRecA = -45;
						}
					}
					//иначе остается только случай с А.d<0 и B.d>=0
					else
					{
						if (DegToRad(360) + A.d - B.d < B.d - A.d)
						{
							CurMinRecA = -135;
							CurMaxRecA = -45;
						}
						else
						{
							CurMinRecA = 45;
							CurMaxRecA = 135;
						}
					}
				}
			}

			//после проверки всех частных случаев рассматриваем общий 
			//нужно определить в какой четверти относительно А искать угол азимута
			//точно известно, что точки не совпадают и не лежат на одной широте или долготе
			else
			{
				if ((A.d >= 0 && B.d >= 0) || (A.d < 0 && B.d < 0))
				{
					if (A.d > B.d)
					{
						CurMinRecA = -190;
						CurMaxRecA = 10;
					}
					//иначе ищем угол от 45 до 135
					else
					{
						CurMinRecA = -10;
						CurMaxRecA = 190;
					}
				}
				//иначе
				//если долгота А неотрицательная, а долгота В отрицательная, то добавляем к отрицательной 2пи и смотрим какая из дуг короче:левая или правая
				else
				{
					if (A.d >= 0 && B.d < 0)
					{
						if (DegToRad(360) + B.d - A.d < A.d - B.d)
						{
							CurMinRecA = -10;
							CurMaxRecA = 190;
						}
						else
						{
							CurMinRecA = -190;
							CurMaxRecA = 10;
						}
					}
					//иначе остается только случай с А.d<0 и B.d>=0
					else
					{
						if (DegToRad(360) + A.d - B.d < B.d - A.d)
						{
							CurMinRecA = -190;
							CurMaxRecA = 10;
						}
						else
						{
							CurMinRecA = -10;
							CurMaxRecA = 190;
						}
					}
				}
			}
		}
	}
	
	
	return true;	
}

bool Rocket::help_v2(MyDlg& dlg)
{
	dlg.STOP_HELP = FALSE;
	double ResY(0),
		ResA(0),
		MaxA(180),
		MinA(-180),
		MaxY(90),
		MinY(0),
		MinDistanseY(7 * REarth),
		MinDistanseA(7 * REarth),
		MinDistanse(7 * REarth);
	if (dlg.TopBot==_T("Сверху") && dlg.LeftRight==_T("Слева"))
	{
		MinA = -90;
		MaxA = 0;
	}
	else
	{
		if (dlg.TopBot == _T("Сверху") && dlg.LeftRight == _T("Справа"))
		{
			MinA = 0;
			MaxA = 90;
		}
		else
		{
			if (dlg.TopBot == _T("Снизу") && dlg.LeftRight == _T("Слева"))
			{
				MinA = -180;
				MaxA = -90;
			}
			else
			{
				if (dlg.TopBot == _T("Снизу") && dlg.LeftRight == _T("Справа"))
				{
					MinA = 90;
					MaxA = 180;
				}
			}
		}
	}
	do
	{
		double CurMaxA(MaxA),
			CurMinA(MinA);
		double incrementA = (CurMaxA - CurMinA) / 3.;
		
		for (double CurA = CurMinA; CurA < CurMaxA; CurA += incrementA)
		{
			double CurMaxY(MaxY), CurMinY(MinY);
			double PrevDistanse;
			double CurDist = 0;
			MinDistanseY = 7 * REarth;
			do
			{
				PrevDistanse = MinDistanseY;
				double incrementY = (CurMaxY - CurMinY) / 3.;

				for (double CurY = CurMinY; CurY < CurMaxY; CurY += incrementY)
				{
					SetV0(v0.val, CurA, CurY);
					if (StartRocket_For_Help(dlg, dlg.NeedGConst, dlg.NeedSopr, dlg.dt))
					{
						CurDist = Calc_Distanse_btw_A_B(DekToGeo(CurPos), B);
						if (CurDist < MinDistanseY)
						{
							ResA = CurA;
							ResY = CurY;
							MinDistanseY = CurDist;
						}
					}
				}
				CurMinY = ResY - incrementY;
				CurMaxY = ResY + incrementY;

			} while (fabs(MinDistanseY - PrevDistanse) < 1000);

		}
	
		dlg.RecA = ResA;
		dlg.RecY = ResY;
		if (dlg.STOP_HELP = TRUE)return false;
		dlg.UpdateData(FALSE);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (MinDistanse > 20000);

	return true;
}

void Rocket::HELP_ADVICE(MyDlg& dlg)
{

}

double Rocket::f_no_res(double y_, double x, double y, double z)
{
	return -g * x / sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) + pow(w, 2) * x + 2. * y_ * w;
}
double Rocket::g_no_res(double x_, double x, double y, double z)
{
	return pow(w, 2) * y - g * y / sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) - 2. * x_ * w;
}
double Rocket::h_no_res(double x, double y, double z)
{
	return - g * z / sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

void Rocket::Calc_g()
{
	g = G * M / (pow(CurPos.x, 2) + pow(CurPos.y, 2) + pow(CurPos.z, 2));// cout << g << endl;
}//расчет ускорения свободного падения

//расчет плотности
double Rocket::Calc_pl() 
{
	
	double
		P0(1.225),
		Mol(0.029),
		Rgas(8.314),
		Temp(223),
		HHH(sqrt(CurPos.x * CurPos.x + CurPos.y * CurPos.y + CurPos.z * CurPos.z) - REarth),
		plotnost= P0 * exp(-1. * Mol * g * HHH / (Rgas * Temp));
	pl = plotnost;
	return plotnost;

	
}

bool Rocket::StartRocket(MyDlg& dlg, Drawer& drv, BOOL gconst, BOOL Fsopr, int Delay, double DT)
{
	
	if (Fsopr == FALSE)Cd = 0;
	else Cd = dlg.Cd;
	t = 0;
	dt = DT;
	S = dlg.Diametr * dlg.Diametr * pi / 4.;
	double kx[4];
	double ky[4];
	double kz[4];
	CurPos = a;
	double x(CurPos.x), y(CurPos.y), z(CurPos.z);
	double x_(v.x), y_(v.y), z_(v.z);
	for (int i = 0; i < 4; i++)
	{
		kx[i] = 0;
		ky[i] = 0;
		kz[i] = 0;
	}
	int i = 0;
	dlg.STOP = false;
	dlg.PutS = 0;
	drv.DrawPoints();
	if (dlg.IsHelpNeeded)
	{
		SetB(dlg.ShirotaB, dlg.dolgotaB);
		dlg.drv.DrawBlueDot(b.x, b.y, b.z);
	}
	drv.SetStart(a.x, a.y, a.z);
	drv.DrawPinkDot(a.x, a.y, a.z);
	dlg.drv2.DrawGraphScene(0, 600 * dt);
	double Ep(m * g * sqrt(CurPos.x * CurPos.x + CurPos.y * CurPos.y + CurPos.z * CurPos.z) + 0.5 * m * w * w * (CurPos.x * CurPos.x + CurPos.y * CurPos.y)),
		Ek(m * (v.x * v.x + v.y * v.y + v.z * v.z) * 0.5);
	Ep = int(Ep * 1e-10);
	Ek = int(Ek * 1e-10);
	double E = (Ep + Ek);
	dlg.drv2.SetGraphStart(E);
	dlg.graphEk.DrawGraphScene(0, 600 * dt);
	dlg.graphEk.SetGraphStart(Ek);

	dlg.graphEp.DrawGraphScene(0, 600 * dt);
	dlg.graphEp.SetGraphStart(Ep);

	Coord PrevPos = a;
	
	vector<double>xgraph, ygraph, xgraph_Ek, ygraph_Ek, xgraph_Ep, ygraph_Ep;

	do
	{
		PrevPos = CurPos;
		x = CurPos.x;
		y = CurPos.y;
		z = CurPos.z;

		x_ = v.x;
		y_ = v.y;
		z_ = v.z;
		t = i * dt;
		if (gconst == FALSE)Calc_g();
		if (Calc_pl() < 1e-5)dlg.Plotnost = 0;
		else dlg.Plotnost = pl;
		Calc_Fd();
		kx[0] = fx(x_, y_, z_, x, y, z) * dt;
		ky[0] = gy(x_, y_, z_, x, y, z) * dt;
		kz[0] = hz(x_, y_, z_, x, y, z) * dt;

		kx[1] = fx(x_ + kx[0] / 2., y_ + ky[0] / 2., z_ + kz[0] / 2., x + x_ * dt / 2., y + y_ * dt / 2., z + z_ * dt / 2.) * dt;
		ky[1] = gy(x_ + kx[0] / 2., y_ + ky[0] / 2., z_ + kz[0] / 2., x + x_ * dt / 2., y + y_ * dt / 2., z + z_ * dt / 2.) * dt;
		kz[1] = hz(x_ + kx[0] / 2., y_ + ky[0] / 2., z_ + kz[0] / 2., x + x_ * dt / 2., y + y_ * dt / 2., z + z_ * dt / 2.) * dt;

		kx[2] = fx(x_ + kx[1] / 2., y_ + ky[1] / 2., z_ + kz[1] / 2., x + x_ * dt / 2. + kx[0] * dt / 4., y + y_ * dt / 2. + ky[0] * dt / 4., z + z_ * dt / 2. + kz[0] * dt / 4.) * dt;
		ky[2] = gy(x_ + kx[1] / 2., y_ + ky[1] / 2., z_ + kz[1] / 2., x + x_ * dt / 2. + kx[0] * dt / 4., y + y_ * dt / 2. + ky[0] * dt / 4., z + z_ * dt / 2. + kz[0] * dt / 4.) * dt;
		kz[2] = hz(x_ + kx[1] / 2., y_ + ky[1] / 2., z_ + kz[1] / 2., x + x_ * dt / 2. + kx[0] * dt / 4., y + y_ * dt / 2. + ky[0] * dt / 4., z + z_ * dt / 2. + kz[0] * dt / 4.) * dt;

		kx[3] = fx(x_ + kx[2], y_ + ky[2], z_ + kz[2], x + x_ * dt + kx[1] * dt / 2., y + y_ * dt + ky[1] * dt / 2., z + z_ * dt + kz[1] * dt / 2.) * dt;
		ky[3] = gy(x_ + kx[2], y_ + ky[2], z_ + kz[2], x + x_ * dt + kx[1] * dt / 2., y + y_ * dt + ky[1] * dt / 2., z + z_ * dt + kz[1] * dt / 2.) * dt;
		kz[3] = hz(x_ + kx[2], y_ + ky[2], z_ + kz[2], x + x_ * dt + kx[1] * dt / 2., y + y_ * dt + ky[1] * dt / 2., z + z_ * dt + kz[1] * dt / 2.) * dt;

		CurPos.x = x + x_ * dt + (kx[0] + kx[1] + kx[2]) * dt / 6.;
		CurPos.y = y + y_ * dt + (ky[0] + ky[1] + ky[2]) * dt / 6.;
		CurPos.z = z + z_ * dt + (kz[0] + ky[1] + ky[2]) * dt / 6.;

		v.x = x_ + (kx[0] + 2. * kx[1] + 2. * kx[2] + kx[3]) / 6.;
		v.y = y_ + (ky[0] + 2. * ky[1] + 2. * ky[2] + ky[3]) / 6.;
		v.z = z_ + (kz[0] + 2. * kz[1] + 2. * kz[2] + kz[3]) / 6.;

		drv.SetStart(PrevPos.x, PrevPos.y, PrevPos.z);


		dlg.x = (int)(CurPos.x / 1000.);
		dlg.y = (int)(CurPos.y / 1000.);
		dlg.z = (int)(CurPos.z / 1000.);

		dlg.PutS += (int)(sqrt(
			(CurPos.x - PrevPos.x) * (CurPos.x - PrevPos.x) + (CurPos.y - PrevPos.y) * (CurPos.y - PrevPos.y) + (CurPos.z - PrevPos.z) * (CurPos.z - PrevPos.z)
		)/1000.);

		double* mas = drv.convert_to_2D(CurPos.x, CurPos.y, CurPos.z);
		dlg.X = (int)drv.convert_range(mas[0]);
		dlg.Y =  (int)drv.convert_range(mas[1]);

		dlg.g = g;
		dlg.CURRENT_TIME = (int)t;
		drv.PrintLineTo(CurPos.x, CurPos.y, CurPos.z, i, dt);

		Ep = (m * g * sqrt(CurPos.x * CurPos.x + CurPos.y * CurPos.y + CurPos.z * CurPos.z) + 0.5 * m * w * w * (CurPos.x * CurPos.x + CurPos.y * CurPos.y));
		Ek = (m * (v.x * v.x + v.y * v.y + v.z * v.z) * 0.5);
		Ep = int(Ep * 1e-10);
		Ek = int(Ek * 1e-10);
		E = (Ek + Ep);

		dlg.drv2.PrintGraphDot(dt, t, E, xgraph, ygraph);
		dlg.graphEk.PrintGraphDot(dt, t, Ek, xgraph_Ek, ygraph_Ek);
		dlg.graphEp.PrintGraphDot(dt, t, Ep, xgraph_Ep, ygraph_Ep);

		GeoC Cur = DekToGeo(CurPos);
		dlg.ShirotaCur = (int)RadToDeg(Cur.s);
		dlg.DolgotaCur = (int)RadToDeg(Cur.d);
		if(dlg.IsHelpNeeded)dlg.Distanse_btw_curpos_b = (int)(Calc_Distanse_btw_A_B(DekToGeo(CurPos), B)/1000.);
		dlg.CurrentH = (int)((sqrt(CurPos.x * CurPos.x + CurPos.y * CurPos.y + CurPos.z * CurPos.z) - REarth)/1000.);
		dlg.DalnostPoleta = (int)(2 * REarth * asin(sqrt(pow(sin(0.5 * (Cur.s - A.s)), 2) + cos(A.s) * cos(Cur.s) * pow(sin(0.5 * (Cur.d - A.d)), 2)))*0.001);
		Update(dlg);
		Sleep(Delay);
		while (dlg.PAUSE == true)
		{
			Sleep(100);
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (dlg.STOP == true)return false;
		
		i++;
	} while (sqrt(pow(CurPos.x, 2) + pow(CurPos.y, 2) + pow(CurPos.z, 2)) > (REarth));
	dlg.drv.DrawDot(CurPos.x, CurPos.y, CurPos.z);
		return true;
}

double Rocket::fx(double x_, double y_, double z_, double x, double y, double z)
{

	return -g * x / sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) + pow(w, 2) * x + 2. * y_ * w - Fd_abs * x_ * sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}
double Rocket::gy(double x_, double y_, double z_, double x, double y, double z)
{
	return pow(w, 2) * y - g * y / sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) - 2. * x_ * w - Fd_abs * y_ * sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}
double Rocket::hz(double x_, double y_, double z_, double x, double y, double z)
{
	return -g * z / sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) - Fd_abs * z_ * sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}

void Rocket::Calc_Fd()
{
	Fd_abs = Cd * pl * S / (2. * m);
}

void Rocket::Calc_Azimut()
{
	Coord ab;
	ab.x = b.x - a.x;
	ab.y = b.y - a.y;
	ab.z = b.z - a.z;
	Coord buf = ab;
	ab.x = -buf.x * sin(A.d) - cos(A.d) * (buf.z * cos(A.s) + buf.y * sin(A.s));
	ab.y = buf.x * cos(A.d) - sin(A.d) * (buf.z * cos(A.s) + buf.y * sin(A.s));
	v0.RecA = acos(ab.y / sqrt(pow(ab.x, 2) + pow(ab.y, 2)));
}

//возвращает модуль вектора из точки а в точку в, а - точка старта
double Rocket::Calc_abs_ab(Coord BB)
{
	Coord ab;
	ab.x = BB.x - a.x;
	ab.y = BB.y - a.y;
	ab.z = BB.z - a.z;
	return sqrt(pow(ab.x, 2) + pow(ab.y, 2) + pow(ab.z, 2));
}

Rocket::GeoC Rocket::DekToGeo(Coord X)
{
	GeoC res;
	res.d = acos(X.x / sqrt(pow(X.x, 2) + pow(X.y, 2)));
	res.s = asin(X.z / sqrt(pow(X.x, 2) + pow(X.y, 2) + pow(X.z, 2)));
	if (X.y < 0)res.d = -res.d;
	//if (X.z < 0)res.s = -res.s;
	return res;
}

double* Rocket::convert_to_2D(double x, double y, double z)
{
	double* res = new double[2];
	double pi = 4 * atan(1);
	double A(-asin(tan(pi / 6.))), B(pi / 4.), C(pi / 2.);
	res[0] = x * cos(B) - z * sin(B);
	res[1] = y * cos(A) + z * cos(B) * sin(A) + x * sin(A) * sin(B);
	return res;
}
void Rocket::Update(MyDlg& dlg)
{
	dlg.UpdateData(FALSE);
}



double Rocket::Calc_abs_ab(Coord AA, Coord BB)
{
	Coord ab;
	ab.x = BB.x - AA.x;
	ab.y = BB.y - AA.y;
	ab.z = BB.z - AA.z;
	return sqrt(pow(ab.x, 2) + pow(ab.y, 2) + pow(ab.z, 2));
}

double Rocket::Calc_Distanse_btw_A_B(GeoC AA, GeoC BB)
{
	return (2 * REarth * asin(sqrt(pow(sin(0.5 * (BB.s - AA.s)), 2) + cos(AA.s) * cos(BB.s) * pow(sin(0.5 * (BB.d - AA.d)), 2))));
}

bool Rocket::StartRocket_For_Help(MyDlg &dlg, BOOL gconst, BOOL Fsopr, double DT)
{
	if (Fsopr == FALSE)Cd = 0;
	else Cd = dlg.Cd;
	t = 0;
	dt = DT;
	double kx[4];
	double ky[4];
	double kz[4];
	CurPos = a;
	double x(CurPos.x), y(CurPos.y), z(CurPos.z);
	double x_(v.x), y_(v.y), z_(v.z);
	for (int i = 0; i < 4; i++)
	{
		kx[i] = 0;
		ky[i] = 0;
		kz[i] = 0;
	}
	int i = 0;
	Coord PrevPos = a;

	do
	{
		PrevPos = CurPos;
		x = CurPos.x;
		y = CurPos.y;
		z = CurPos.z;

		x_ = v.x;
		y_ = v.y;
		z_ = v.z;
		t = i * dt;
		if (gconst == FALSE)Calc_g();
		Calc_pl();
		Calc_Fd();
		kx[0] = fx(x_, y_, z_, x, y, z) * dt;
		ky[0] = gy(x_, y_, z_, x, y, z) * dt;
		kz[0] = hz(x_, y_, z_, x, y, z) * dt;

		kx[1] = fx(x_ + kx[0] / 2., y_ + ky[0] / 2., z_ + kz[0] / 2., x + x_ * dt / 2., y + y_ * dt / 2., z + z_ * dt / 2.) * dt;
		ky[1] = gy(x_ + kx[0] / 2., y_ + ky[0] / 2., z_ + kz[0] / 2., x + x_ * dt / 2., y + y_ * dt / 2., z + z_ * dt / 2.) * dt;
		kz[1] = hz(x_ + kx[0] / 2., y_ + ky[0] / 2., z_ + kz[0] / 2., x + x_ * dt / 2., y + y_ * dt / 2., z + z_ * dt / 2.) * dt;

		kx[2] = fx(x_ + kx[1] / 2., y_ + ky[1] / 2., z_ + kz[1] / 2., x + x_ * dt / 2. + kx[0] * dt / 4., y + y_ * dt / 2. + ky[0] * dt / 4., z + z_ * dt / 2. + kz[0] * dt / 4.) * dt;
		ky[2] = gy(x_ + kx[1] / 2., y_ + ky[1] / 2., z_ + kz[1] / 2., x + x_ * dt / 2. + kx[0] * dt / 4., y + y_ * dt / 2. + ky[0] * dt / 4., z + z_ * dt / 2. + kz[0] * dt / 4.) * dt;
		kz[2] = hz(x_ + kx[1] / 2., y_ + ky[1] / 2., z_ + kz[1] / 2., x + x_ * dt / 2. + kx[0] * dt / 4., y + y_ * dt / 2. + ky[0] * dt / 4., z + z_ * dt / 2. + kz[0] * dt / 4.) * dt;

		kx[3] = fx(x_ + kx[2], y_ + ky[2], z_ + kz[2], x + x_ * dt + kx[1] * dt / 2., y + y_ * dt + ky[1] * dt / 2., z + z_ * dt + kz[1] * dt / 2.) * dt;
		ky[3] = gy(x_ + kx[2], y_ + ky[2], z_ + kz[2], x + x_ * dt + kx[1] * dt / 2., y + y_ * dt + ky[1] * dt / 2., z + z_ * dt + kz[1] * dt / 2.) * dt;
		kz[3] = hz(x_ + kx[2], y_ + ky[2], z_ + kz[2], x + x_ * dt + kx[1] * dt / 2., y + y_ * dt + ky[1] * dt / 2., z + z_ * dt + kz[1] * dt / 2.) * dt;

		CurPos.x = x + x_ * dt + (kx[0] + kx[1] + kx[2]) * dt / 6.;
		CurPos.y = y + y_ * dt + (ky[0] + ky[1] + ky[2]) * dt / 6.;
		CurPos.z = z + z_ * dt + (kz[0] + ky[1] + ky[2]) * dt / 6.;

		v.x = x_ + (kx[0] + 2. * kx[1] + 2. * kx[2] + kx[3]) / 6.;
		v.y = y_ + (ky[0] + 2. * ky[1] + 2. * ky[2] + ky[3]) / 6.;
		v.z = z_ + (kz[0] + 2. * kz[1] + 2. * kz[2] + kz[3]) / 6.;

		
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (dlg.STOP == true)return false;

		i++;
	} while (sqrt(pow(CurPos.x, 2) + pow(CurPos.y, 2) + pow(CurPos.z, 2)) > (REarth));
	return true;
}