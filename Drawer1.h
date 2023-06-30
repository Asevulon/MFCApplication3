#pragma once

#include <afxwin.h>
#include <vector>

using namespace std;

class Drawer
{
	// Прямоугольник области рисования.
	CRect frame;
	// Указатель на объект окна, обалсти рисования.
	CWnd * wnd;
	// Контекст рисования, привязанный к области рисования.
	CDC * dc;
	// Контекст рисования, привязанный к памяти.
	CDC memDC;
	// Память для контекста рисования memDC.
	CBitmap bmp;
	// Флаг для отслеживания состояния инициализации класса.
	bool init;
public:
	// Проинициализировать объект класса на основе HWND.
	void Create(HWND hWnd)
	{
		// Получаем указатель на окно.
		wnd = CWnd::FromHandle(hWnd);
		// Получаем прямоугольник окна.
		wnd->GetClientRect(frame);
		// Получаем контекст для рисования в этом окне.
		dc = wnd->GetDC();

		// Создаем буфер-контекст.
		memDC.CreateCompatibleDC(dc);
		// Создаем растр для контекста рисования.
		bmp.CreateCompatibleBitmap(dc, frame.Width(), frame.Height());
		// Выбираем растр для использования буфер-контекстом.
		memDC.SelectObject(&bmp);
		init = true;
	}
	// Нарисовать график по переданным данным.
	void Draw(vector<double> &data,vector<double>&data2, vector<double> &keys = vector<double>())
	{
		if (!init) return;

		CPen subgrid_pen(PS_DOT, 1, RGB(200, 200, 200));
		CPen grid_pen(PS_SOLID, 1, RGB(0, 0, 0));
		CPen data_pen(PS_SOLID, 2, RGB(255, 0, 0));
		CPen data2_pen(PS_SOLID, 2, RGB(0, 255, 0));
		CFont font;
		font.CreateFontW(18, 0, 0, 0,
			FW_DONTCARE,
			FALSE,				// Курсив
			FALSE,				// Подчеркнутый
			FALSE,				// Перечеркнутый
			DEFAULT_CHARSET,	// Набор символов
			OUT_OUTLINE_PRECIS,	// Точность соответствия.	
			CLIP_DEFAULT_PRECIS,//  
			CLEARTYPE_QUALITY,	// Качество
			VARIABLE_PITCH,		//
			TEXT("Times New Roman")		//
		);

		int padding = 20;
		int left_keys_padding = 20;
		int bottom_keys_padding = 10;

		int actual_width = frame.Width() - 2 * padding - left_keys_padding;
		int actual_height = frame.Height() - 2 * padding - bottom_keys_padding;

		int actual_top = padding;
		int actual_bottom = actual_top + actual_height;
		int actual_left = padding + left_keys_padding;
		int actual_right = actual_left + actual_width;
		
		// Белый фон.
		memDC.FillSolidRect(frame, RGB(255,255,255));

		// Рисуем сетку и подсетку.
		unsigned int grid_size = 10;

		memDC.SelectObject(&grid_pen);

		
		/*for (double i = 0.5; i < grid_size; i += 1.0)
		{
			memDC.MoveTo(actual_left + i * actual_width / grid_size, actual_top);
			memDC.LineTo(actual_left + i * actual_width / grid_size, actual_bottom);
			memDC.MoveTo(actual_left,   actual_top + i * actual_height / grid_size);
			memDC.LineTo(actual_right, actual_top + i * actual_height / grid_size);
		}*/

		memDC.SelectObject(&subgrid_pen);

		/*for (double i = 0.0; i < grid_size + 1; i += 1.0)
		{
			memDC.MoveTo(actual_left + i * actual_width / grid_size, actual_top);
			memDC.LineTo(actual_left + i * actual_width / grid_size, actual_bottom);
			memDC.MoveTo(actual_left, actual_top + i * actual_height / grid_size);
			memDC.LineTo(actual_right, actual_top + i * actual_height / grid_size);
		}*/
	
		// Рисуем график.
		if (data.empty()) return;

		if (keys.size() != data.size())
		{
			keys.resize(data.size());
			for (int i = 0; i < keys.size(); i++)
			{
				keys[i] = i;
			}
		}

		memDC.SelectObject(&data_pen);

		double data_y_max(2), data_y_min(0);
		double data_x_max(100), data_x_min(0);

		vector<double> y = convert_range(data, actual_top, actual_bottom, data_y_max, data_y_min);
		vector<double> x = convert_range(keys, actual_right, actual_left, data_x_max, data_x_min);
				
		memDC.MoveTo(x[0], y[0]);
		for (unsigned int i = 0; i < y.size(); i++)
		{
			memDC.LineTo(x[i], y[i]);
		}

		memDC.SelectObject(&data2_pen);

		y = convert_range(data2, actual_top, actual_bottom, data_y_max, data_y_min);
		 x = convert_range(keys, actual_right, actual_left, data_x_max, data_x_min);

		memDC.MoveTo(x[0], y[0]);
		for (unsigned int i = 0; i < y.size(); i++)
		{
			memDC.LineTo(x[i], y[i]);
		}

		memDC.SelectObject(&font);
		memDC.SetTextColor(RGB(0, 0, 0));
		for (int i = 0; i < grid_size/2 + 1; i++)
		{
			CString str;
			str.Format(L"%.1f", data_x_min + i*(data_x_max - data_x_min) /(grid_size / 2));
			memDC.TextOutW(actual_left + (double)i * actual_width / (grid_size / 2) - bottom_keys_padding, actual_bottom + bottom_keys_padding/2, str);

			str.Format(L"%.1f", data_y_min + i*(data_y_max - data_y_min) / (grid_size / 2));
			memDC.TextOutW(actual_left - 1.5*left_keys_padding, actual_bottom - (double)i * actual_height / (grid_size / 2) - bottom_keys_padding, str);
		}
		
		dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
	}

	// Нарисовать график по переданным данным.
	void DrawPoints(/*vector<double>& data, vector<double>& data2, vector<double>& keys2 = vector<double>(), vector<double>& keys = vector<double>()*/)
	{
		if (!init) return;
		CPen subgrid_pen(PS_DOT, 1, RGB(200, 200, 200));
		CPen grid_pen1(PS_SOLID, 3, RGB(0, 150, 150));
		CPen grid_pen2(PS_SOLID, 3, RGB(0, 0, 0));
		CPen data_pen(PS_SOLID, 3, RGB(255, 0, 0));
		CPen data2_pen(PS_SOLID, 3, RGB(0, 255, 0));
		CPen axes_penx(PS_SOLID, 3, RGB(200, 0, 0));
		CPen axes_peny(PS_SOLID, 3, RGB(0, 200, 0));
		CPen axes_penz(PS_SOLID, 3, RGB(0, 0, 200));
		CPen subround_pen1(PS_SOLID, 2, RGB(0, 150, 150));
		CPen subround_pen2(PS_SOLID, 2, RGB(0, 0, 0));
		CFont font;
		font.CreateFontW(18, 0, 0, 0,
			FW_DONTCARE,
			FALSE,				// Курсив
			FALSE,				// Подчеркнутый
			FALSE,				// Перечеркнутый
			DEFAULT_CHARSET,	// Набор символов
			OUT_OUTLINE_PRECIS,	// Точность соответствия.	
			CLIP_DEFAULT_PRECIS,//  
			CLEARTYPE_QUALITY,	// Качество
			VARIABLE_PITCH,		//
			TEXT("Times New Roman")		//
		);

		int padding = 0;
		int left_keys_padding = 0;
		int bottom_keys_padding = 0;

		int actual_width = frame.Width();
		int actual_height = frame.Height();

		int actual_top = 0;
		int actual_bottom = actual_top + actual_height;
		int actual_left =0;
		int actual_right = actual_left + actual_width;

		double data_y_max(9000), data_y_min(-9000);
		double data_x_max(9000), data_x_min(-9000);

		// Белый фон.
		memDC.FillSolidRect(frame, RGB(255, 255, 255));

		// Рисуем сетку и подсетку.
		unsigned int grid_size = 10;

		//Рисуем оси координат

		vector<double> axes[3];
		axes[0].push_back(0);
		axes[0].push_back(7000000);
		axes[0].push_back(0);
		axes[0].push_back(0);

		axes[1].push_back(0);
		axes[1].push_back(0);
		axes[1].push_back(7000000);
		axes[1].push_back(0);

		axes[2].push_back(0);
		axes[2].push_back(0);
		axes[2].push_back(0);
		axes[2].push_back(7000000);

		vector<double>* axes2d = convert_to_2D_v2(axes[0], axes[1], axes[2]);
		axes2d[0] = convert_range(axes2d[0], actual_right, actual_left, data_x_max, data_x_min);
		axes2d[1] = convert_range(axes2d[1], actual_top, actual_bottom, data_y_max, data_y_min);
		
		memDC.SelectObject(&axes_penx);
			memDC.MoveTo(axes2d[0][0], axes2d[1][0]);
			memDC.LineTo(axes2d[0][1], axes2d[1][1]);
		
			memDC.SelectObject(&axes_peny);
			memDC.MoveTo(axes2d[0][0], axes2d[1][0]);
			memDC.LineTo(axes2d[0][2], axes2d[1][2]);

			memDC.SelectObject(&axes_penz);
			memDC.MoveTo(axes2d[0][0], axes2d[1][0]);
			memDC.LineTo(axes2d[0][3], axes2d[1][3]);
		//Рисуем вертикальный круг

		memDC.SelectObject(&grid_pen1);

		vector<double>round[3];
		double pi = 4 * atan(1);
		for (int i = 0; i < 360; i++)
		{
			round[0].push_back(0);
			round[1].push_back(6371000 * cos(i * pi / 180.));
			round[2].push_back(6371000 * sin(i * pi / 180.));
		}
		vector<double>* round2d = convert_to_2D_v2(round[0], round[1], round[2]);
		round2d[0] = convert_range(round2d[0], actual_right, actual_left, data_x_max, data_x_min);
		round2d[1] = convert_range(round2d[1], actual_top, actual_bottom, data_y_max, data_y_min);

		memDC.MoveTo(round2d[0][0], round2d[1][0]);
		for (unsigned int i = 0; i < round2d[0].size(); i++)
		{
			memDC.LineTo(round2d[0][i], round2d[1][i]);
		}


		//Рисуем горизонтальный круг
		memDC.SelectObject(&grid_pen2);
		vector<double>gor_round[3];
		for (int i = 0; i < 360; i++)
		{
			gor_round[0].push_back(6371000 * cos(i * pi / 180.));
			gor_round[1].push_back(6371000 * sin(i * pi / 180.));
			gor_round[2].push_back(0);
		}
		round2d = convert_to_2D_v2(gor_round[0], gor_round[1], gor_round[2]);
		round2d[0] = convert_range(round2d[0], actual_right, actual_left, data_x_max, data_x_min);
		round2d[1] = convert_range(round2d[1], actual_top, actual_bottom, data_y_max, data_y_min);
		memDC.MoveTo(round2d[0][0], round2d[1][0]);
		for (unsigned int i = 0; i < round2d[0].size(); i++)
		{
			memDC.LineTo(round2d[0][i], round2d[1][i]);
		}

		//рисуем вертикальные каркасные круги

		memDC.SelectObject(subround_pen1);

		for (int i = 0; i < 2; i++)
		{
			transform_round(round,pi/3.);
			round2d = convert_to_2D_v2(round[0], round[1], round[2]);
			round2d[0] = convert_range(round2d[0], actual_right, actual_left, data_x_max, data_x_min);
			round2d[1] = convert_range(round2d[1], actual_top, actual_bottom, data_y_max, data_y_min);
			memDC.MoveTo(round2d[0][0], round2d[1][0]);
			for (unsigned int i = 0; i < round2d[0].size(); i++)
			{
				memDC.LineTo(round2d[0][i], round2d[1][i]);
			}
		}
		
		//рисуем горизонтальные каркасные круги
		memDC.SelectObject(subround_pen2);
		int numofgorrounds = 6;
		for (int g = 1; g < numofgorrounds; g++)
		{
			for (int i = 0; i < 3; i++)
			{
				gor_round[i].clear();
			}
			if (90 - g * 180. / (double)numofgorrounds == 0)continue;
			double CurR = 6371000 * cos((90 - g * 180. / (double)numofgorrounds) * pi / 180.);
			for (int i = 0; i < 360; i++)
			{
				gor_round[0].push_back(CurR * cos(i * pi / 180.));
				gor_round[1].push_back(CurR * sin(i * pi / 180.));
				gor_round[2].push_back(CurR * tan((90 - g * 180. / (double)numofgorrounds) * pi / 180.));
			}
			round2d = convert_to_2D_v2(gor_round[0], gor_round[1], gor_round[2]);
			round2d[0] = convert_range(round2d[0], actual_right, actual_left, data_x_max, data_x_min);
			round2d[1] = convert_range(round2d[1], actual_top, actual_bottom, data_y_max, data_y_min);
			memDC.MoveTo(round2d[0][0], round2d[1][0]);
			for (unsigned int i = 0; i < round2d[0].size(); i++)
			{
				memDC.LineTo(round2d[0][i], round2d[1][i]);
			}
		}
	
		dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
	}
	vector<double> convert_range(vector <double> & data, double outmax, double outmin, double inmax, double inmin)
	{
		vector<double> output = data;
		double k = (outmax - outmin) / (inmax - inmin);
		for (auto & item : output)
		{
			item = (item/1000. - inmin) * k + outmin;
		}

		return output;
	}
	vector<double>* convert_to_2D(vector<double> y, vector<double> z, vector<double> x)
	{
		vector <double>* output = new vector <double>[2];
		int n = x.size();
		double pi = 4*atan(1);
		double A(asin(tan(pi / 6.))), B(pi / 4.);
		for (int i = 0; i < n; i++)
		{
			output[0].push_back(
				//cos(C)*(x[i]*cos(B)-z[i]*sin(B)) - sin(C)*(z[i]*cos(A)*cos(B)-y[i]*sin(A)+x[i]*cos(A)*sin(B))
			x[i] * cos(B) - z[i] * sin(B)
				//x[i] * cos(A) * cos(B) - y[i] * sin(A) - z[i] * cos(A) * sin(B)
			);//0 - x
			output[1].push_back(
					y[i]*cos(A)+z[i]*cos(B)*sin(A)+x[i]*sin(A)*sin(B)
				//y[i] * cos(A) + x[i] * cos(B) * sin(A) - z[i] * sin(A) * sin(B)
			);//1 - y
		}
		return output;
	}
	void transform_round(vector<double>* round, double A)
	{
		for (int i = 0; i < round[0].size(); i++)
		{
			double x(round[0][i]), y(round[1][i]), z(round[2][i]);
			round[0][i] = x * cos(A) - y * sin(A);
			round[1][i] = y * cos(A) + x * sin(A);
			round[2][i] = z;
		}
	}

	bool SetStart(double x, double y, double z)
	{
		vector<double> X, Y, Z;
		X.push_back(x);
		Y.push_back(y);
		Z.push_back(z);
		int padding = 0;
		int left_keys_padding = 0;
		int bottom_keys_padding = 0;

		int actual_width = frame.Width();
		int actual_height = frame.Height();

		int actual_top = 0;
		int actual_bottom = actual_top + actual_height;
		int actual_left = 0;
		int actual_right = actual_left + actual_width;

		double data_y_max(9000), data_y_min(-9000);
		double data_x_max(9000), data_x_min(-9000);
		vector<double>* round2d = convert_to_2D_v2(X, Y, Z);
		round2d[0] = convert_range(round2d[0], actual_right, actual_left, data_x_max, data_x_min);
		round2d[1] = convert_range(round2d[1], actual_top, actual_bottom, data_y_max, data_y_min);
		memDC.MoveTo(round2d[0][0], round2d[1][0]);
		return true;
	}

	double* convert_to_2D(double y, double z, double x)
	{
		double* res = new double[2];
		double pi = 4 * atan(1);
		double A(asin(tan(pi / 6.))), B(pi / 4.);
		
		res[0] = x * cos(B) - z * sin(B);//0 - x
		res[1] = y * cos(A) + z * cos(B) * sin(A) + x * sin(A) * sin(B);//1 - y
		
		return res;
	}
	bool PrintLineTo(double x, double y, double z, int i, double dt)
	{
		//CPen traectory_pen(PS_SOLID, 4, RGB(255*tan(i/10.), 255*tan(i/500.), 255*tan(i/250.)));
		CPen traectory_pen(PS_SOLID, 4, RGB(255 * fabs(cos(i * dt / 500.)), 0, 255 * fabs(sin(i * dt / 500.))));;
		memDC.SelectObject(&traectory_pen);
		
		int padding = 0;
		int left_keys_padding = 0;
		int bottom_keys_padding = 0;

		int actual_width = frame.Width();
		int actual_height = frame.Height();

		int actual_top = 0;
		int actual_bottom = actual_top + actual_height;
		int actual_left = 0;
		int actual_right = actual_left + actual_width;

		double data_y_max(9000), data_y_min(-9000);
		double data_x_max(9000), data_x_min(-9000);

		vector<double> X, Y, Z;

		X.push_back(x);
		Y.push_back(y);
		Z.push_back(z);

		vector<double>*round2d = convert_to_2D_v2(X, Y, Z);
		round2d[0] = convert_range(round2d[0], actual_right, actual_left, data_x_max, data_x_min);
		round2d[1] = convert_range(round2d[1], actual_top, actual_bottom, data_y_max, data_y_min);

		memDC.LineTo(round2d[0][0], round2d[1][0]);

		dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
		return true;
	}

	double convert_range(double data)
	{

		int actual_width = frame.Width();
		int actual_height = frame.Height();

		int actual_top = 0;
		int actual_bottom = actual_top + actual_height;
		int outmin = 0;
		int outmax = outmin + actual_width;

		double inmax(9000), inmin(-9000);
		double k = ((double)outmax - (double)outmin) / (inmax - inmin);
		
		return (data / 1000. - inmin) * k + outmin;
	}
	bool DrawPinkDot(double x, double y, double z)
	{
		CPen dot_pen(PS_SOLID, 7, RGB(200, 100, 200));
		memDC.SelectObject(&dot_pen);
		int padding = 0;
		int left_keys_padding = 0;
		int bottom_keys_padding = 0;

		int actual_width = frame.Width();
		int actual_height = frame.Height();

		int actual_top = 0;
		int actual_bottom = actual_top + actual_height;
		int actual_left = 0;
		int actual_right = actual_left + actual_width;

		double data_y_max(9000), data_y_min(-9000);
		double data_x_max(9000), data_x_min(-9000);

		vector<double> X, Y, Z;

		X.push_back(x);
		Y.push_back(y);
		Z.push_back(z);

		vector<double>* round2d = convert_to_2D_v2(X, Y, Z);
		round2d[0] = convert_range(round2d[0], actual_right, actual_left, data_x_max, data_x_min);
		round2d[1] = convert_range(round2d[1], actual_top, actual_bottom, data_y_max, data_y_min);
		memDC.Ellipse(round2d[0][0]-4,round2d[1][0]-4,round2d[0][0]+4,round2d[1][0]+4);
		dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
		return true;
	}
	bool DrawBlueDot(double x, double y, double z)
	{
		CPen dot_pen(PS_SOLID, 7, RGB(50, 50, 200));
		memDC.SelectObject(&dot_pen);
		int padding = 0;
		int left_keys_padding = 0;
		int bottom_keys_padding = 0;

		int actual_width = frame.Width();
		int actual_height = frame.Height();

		int actual_top = 0;
		int actual_bottom = actual_top + actual_height;
		int actual_left = 0;
		int actual_right = actual_left + actual_width;

		double data_y_max(9000), data_y_min(-9000);
		double data_x_max(9000), data_x_min(-9000);

		vector<double> X, Y, Z;

		X.push_back(x);
		Y.push_back(y);
		Z.push_back(z);

		vector<double>* round2d = convert_to_2D_v2(X, Y, Z);
		round2d[0] = convert_range(round2d[0], actual_right, actual_left, data_x_max, data_x_min);
		round2d[1] = convert_range(round2d[1], actual_top, actual_bottom, data_y_max, data_y_min);
		memDC.MoveTo(round2d[0][0], round2d[1][0]);
		memDC.Ellipse(round2d[0][0] - 4, round2d[1][0] - 4, round2d[0][0] + 4, round2d[1][0] + 4);
		dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
		return true;
	}
	bool DrawDot(double x, double y, double z)
	{

		CPen dot_pen(PS_SOLID, 7, RGB(100, 200, 100));
		memDC.SelectObject(&dot_pen);
		int padding = 0;
		int left_keys_padding = 0;
		int bottom_keys_padding = 0;

		int actual_width = frame.Width();
		int actual_height = frame.Height();

		int actual_top = 0;
		int actual_bottom = actual_top + actual_height;
		int actual_left = 0;
		int actual_right = actual_left + actual_width;

		double data_y_max(9000), data_y_min(-9000);
		double data_x_max(9000), data_x_min(-9000);

		vector<double> X, Y, Z;

		X.push_back(x);
		Y.push_back(y);
		Z.push_back(z);

		vector<double>* round2d = convert_to_2D_v2(X, Y, Z);
		round2d[0] = convert_range(round2d[0], actual_right, actual_left, data_x_max, data_x_min);
		round2d[1] = convert_range(round2d[1], actual_top, actual_bottom, data_y_max, data_y_min);
		memDC.Ellipse(round2d[0][0] - 4, round2d[1][0] - 4, round2d[0][0] + 4, round2d[1][0] + 4);
		dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
		return true;
	}
	vector<double>* convert_to_2D_v2(vector<double> x, vector<double> y, vector<double> z)
	{
		vector <double>* output = new vector <double>[2];
		int n = x.size();
		double pi = 4 * atan(1);
		double A(asin(tan(pi/6.))), B(-pi/4.);
		for (int i = 0; i < n; i++)
		{
			output[1].push_back(
				z[i]*cos(A)-x[i]*cos(B)*sin(A)+y[i]*sin(A)*sin(B)
				//x[i] * cos(A) * cos(B) + z[i] * sin(A) - y[i] * sin(B) * cos(A)
			);//0 - x
			output[0].push_back(
				(y[i]*cos(B)+x[i]*sin(B))
			);//1 - y
		}
		return output;
	}
	void DrawGraphScene(double data_x_min, double data_x_max)
	{
		if (!init) return;

		CPen subgrid_pen(PS_DOT, 1, RGB(200, 200, 200));
		CPen grid_pen(PS_SOLID, 1, RGB(0, 0, 0));
		CPen data_pen(PS_SOLID, 2, RGB(255, 0, 0));
		CPen data2_pen(PS_SOLID, 2, RGB(0, 255, 0));
		CFont font;
		font.CreateFontW(18, 0, 0, 0,
			FW_DONTCARE,
			FALSE,				// Курсив
			FALSE,				// Подчеркнутый
			FALSE,				// Перечеркнутый
			DEFAULT_CHARSET,	// Набор символов
			OUT_OUTLINE_PRECIS,	// Точность соответствия.	
			CLIP_DEFAULT_PRECIS,//  
			CLEARTYPE_QUALITY,	// Качество
			VARIABLE_PITCH,		//
			TEXT("Times New Roman")		//
		);

		int padding = 20;
		int left_keys_padding = 20;
		int bottom_keys_padding = 10;

		int actual_width = frame.Width() - 2 * padding - left_keys_padding;
		int actual_height = frame.Height() - 2 * padding - bottom_keys_padding;

		int actual_top = padding;
		int actual_bottom = actual_top + actual_height;
		int actual_left = padding + left_keys_padding;
		int actual_right = actual_left + actual_width;



		// Белый фон.
		memDC.FillSolidRect(frame, RGB(255, 255, 255));

		// Рисуем сетку и подсетку.
		unsigned int grid_size = 10;

		memDC.SelectObject(&subgrid_pen);

		for (double i = 0.5; i < grid_size; i += 1.0)
		{
			memDC.MoveTo(actual_left + i * actual_width / grid_size, actual_top);
			memDC.LineTo(actual_left + i * actual_width / grid_size, actual_bottom);
			memDC.MoveTo(actual_left, actual_top + i * actual_height / grid_size);
			memDC.LineTo(actual_right, actual_top + i * actual_height / grid_size);
		}

		memDC.SelectObject(&grid_pen);

		for (double i = 0.0; i < grid_size + 1; i += 1.0)
		{
			memDC.MoveTo(actual_left + i * actual_width / grid_size, actual_top);
			memDC.LineTo(actual_left + i * actual_width / grid_size, actual_bottom);
			memDC.MoveTo(actual_left, actual_top + i * actual_height / grid_size);
			memDC.LineTo(actual_right, actual_top + i * actual_height / grid_size);
		}

		double data_y_min(0); double data_y_max(300);
		
		memDC.SelectObject(&font);
		memDC.SetTextColor(RGB(0, 0, 0));
		for (int i = 0; i < grid_size / 2 + 1; i++)
		{
			CString str;
			str.Format(L"%.1f", data_x_min + i * (data_x_max - data_x_min) / (grid_size / 2));
			memDC.TextOutW(actual_left + (double)i * actual_width / (grid_size / 2) - bottom_keys_padding, actual_bottom + bottom_keys_padding / 2, str);

			str.Format(L"%.1f", data_y_min + i * (data_y_max - data_y_min) / (grid_size / 2));
			memDC.TextOutW(actual_left - 1.5 * left_keys_padding, actual_bottom - (double)i * actual_height / (grid_size / 2) - bottom_keys_padding, str);
		}

		dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
	}
	void PrepareGraphScene(double data_x_min, double data_x_max)
	{
		if (!init) return;

		CPen subgrid_pen(PS_DOT, 1, RGB(200, 200, 200));
		CPen grid_pen(PS_SOLID, 1, RGB(0, 0, 0));
		CPen data_pen(PS_SOLID, 2, RGB(255, 0, 0));
		CPen data2_pen(PS_SOLID, 2, RGB(0, 255, 0));
		CFont font;
		font.CreateFontW(18, 0, 0, 0,
			FW_DONTCARE,
			FALSE,				// Курсив
			FALSE,				// Подчеркнутый
			FALSE,				// Перечеркнутый
			DEFAULT_CHARSET,	// Набор символов
			OUT_OUTLINE_PRECIS,	// Точность соответствия.	
			CLIP_DEFAULT_PRECIS,//  
			CLEARTYPE_QUALITY,	// Качество
			VARIABLE_PITCH,		//
			TEXT("Times New Roman")		//
		);

		int padding = 20;
		int left_keys_padding = 20;
		int bottom_keys_padding = 10;

		int actual_width = frame.Width() - 2 * padding - left_keys_padding;
		int actual_height = frame.Height() - 2 * padding - bottom_keys_padding;

		int actual_top = padding;
		int actual_bottom = actual_top + actual_height;
		int actual_left = padding + left_keys_padding;
		int actual_right = actual_left + actual_width;



		// Белый фон.
		memDC.FillSolidRect(frame, RGB(255, 255, 255));

		// Рисуем сетку и подсетку.
		unsigned int grid_size = 10;

		memDC.SelectObject(&subgrid_pen);

		for (double i = 0.5; i < grid_size; i += 1.0)
		{
			memDC.MoveTo(actual_left + i * actual_width / grid_size, actual_top);
			memDC.LineTo(actual_left + i * actual_width / grid_size, actual_bottom);
			memDC.MoveTo(actual_left, actual_top + i * actual_height / grid_size);
			memDC.LineTo(actual_right, actual_top + i * actual_height / grid_size);
		}

		memDC.SelectObject(&grid_pen);

		for (double i = 0.0; i < grid_size + 1; i += 1.0)
		{
			memDC.MoveTo(actual_left + i * actual_width / grid_size, actual_top);
			memDC.LineTo(actual_left + i * actual_width / grid_size, actual_bottom);
			memDC.MoveTo(actual_left, actual_top + i * actual_height / grid_size);
			memDC.LineTo(actual_right, actual_top + i * actual_height / grid_size);
		}

		double data_y_min(0); double data_y_max(300);

		memDC.SelectObject(&font);
		memDC.SetTextColor(RGB(0, 0, 0));
		for (int i = 0; i < grid_size / 2 + 1; i++)
		{
			CString str;
			str.Format(L"%.1f", data_x_min + i * (data_x_max - data_x_min) / (grid_size / 2));
			memDC.TextOutW(actual_left + (double)i * actual_width / (grid_size / 2) - bottom_keys_padding, actual_bottom + bottom_keys_padding / 2, str);

			str.Format(L"%.1f", data_y_min + i * (data_y_max - data_y_min) / (grid_size / 2));
			memDC.TextOutW(actual_left - 1.5 * left_keys_padding, actual_bottom - (double)i * actual_height / (grid_size / 2) - bottom_keys_padding, str);
		}

		//dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
	}
	void PrintGraphDot(double dt, double x, double y, vector<double>&xgraph, vector<double>& ygraph)
	{
		double data_x_min(0); double data_x_max(600*dt);
		double data_y_min(0); double data_y_max(300);

		CPen data_pen(PS_SOLID, 2, RGB(255, 0, 0));
		
		int padding = 20;
		int left_keys_padding = 20;
		int bottom_keys_padding = 10;

		int actual_width = frame.Width() - 2 * padding - left_keys_padding;
		int actual_height = frame.Height() - 2 * padding - bottom_keys_padding;

		int actual_top = padding;
		int actual_bottom = actual_top + actual_height;
		int actual_left = padding + left_keys_padding;
		int actual_right = actual_left + actual_width;
		
		
		if (x <= 600*dt)
		{
			vector<double> X, Y;
			Y.push_back(y);
			X.push_back(x);
			memDC.SelectObject(&data_pen);
			Y = convert_range_graph(Y, actual_top, actual_bottom, data_y_max, data_y_min);
			X = convert_range_graph(X, actual_right, actual_left, data_x_max, data_x_min);
			xgraph.push_back(x);
			ygraph.push_back(y);
			memDC.LineTo(X[0], Y[0]);
		}
		else
		{
			data_x_min = x - 600*dt;
			data_x_max = x;
			PrepareGraphScene(data_x_min, data_x_max);
			
			update_vector(xgraph);
			update_vector(ygraph);
			xgraph[xgraph.size()-1] = x;
			ygraph[ygraph.size()-1] = y;
			vector<double> X, Y;
			Y = convert_range_graph(create_vector_copy(ygraph), actual_top, actual_bottom, data_y_max, data_y_min);
			X = convert_range_graph(create_vector_copy(xgraph), actual_right, actual_left, data_x_max, data_x_min);
			memDC.SelectObject(&data_pen);
			memDC.MoveTo(X[0], Y[0]);
			for (int i = 0; i < X.size(); i++)
			{
				memDC.LineTo(X[i], Y[i]);
			}
		}
		dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
	}
	void PrintGraphLine(double dt, double x, double y, vector<double>& xgraph, vector<double>& ygraph)
	{
		double data_x_min(0); double data_x_max(600 * dt);
		double data_y_min(0); double data_y_max(300);

		CPen data_pen(PS_SOLID, 5, RGB(255, 0, 0));

		int padding = 20;
		int left_keys_padding = 20;
		int bottom_keys_padding = 10;

		int actual_width = frame.Width() - 2 * padding - left_keys_padding;
		int actual_height = frame.Height() - 2 * padding - bottom_keys_padding;

		int actual_top = padding;
		int actual_bottom = actual_top + actual_height;
		int actual_left = padding + left_keys_padding;
		int actual_right = actual_left + actual_width;


		if (x <= 600 * dt)
		{
			vector<double> X, Y;
			Y.push_back(y);
			X.push_back(x);
			memDC.SelectObject(&data_pen);
			xgraph.push_back(x);
			ygraph.push_back(y);
			Y = convert_range_graph(create_vector_copy(ygraph), actual_top, actual_bottom, data_y_max, data_y_min);
			X = convert_range_graph(create_vector_copy(xgraph), actual_right, actual_left, data_x_max, data_x_min);
			
			if (x !=0)
			{
				memDC.MoveTo(X[X.size() - 2], Y[Y.size() - 2]);
				memDC.LineTo(X[X.size() - 1], Y[Y.size() - 1]);
			}
			else memDC.MoveTo(X[X.size() - 1], Y[Y.size() - 1]);
			//memDC.MoveTo(X[X.size() - 1], Y[Y.size() - 1]);
			memDC.LineTo(X[X.size() - 1], actual_bottom);
			if (x != 0)memDC.LineTo(X[X.size() - 2], actual_bottom);
		}
		else
		{
			data_x_min = x - 600 * dt;
			data_x_max = x;
			PrepareGraphScene(data_x_min, data_x_max);

			update_vector(xgraph);
			update_vector(ygraph);
			xgraph[xgraph.size() - 1] = x;
			ygraph[ygraph.size() - 1] = y;
			vector<double> X, Y;
			Y = convert_range_graph(create_vector_copy(ygraph), actual_top, actual_bottom, data_y_max, data_y_min);
			X = convert_range_graph(create_vector_copy(xgraph), actual_right, actual_left, data_x_max, data_x_min);
			memDC.SelectObject(&data_pen);
			memDC.MoveTo(X[0], Y[0]);
			for (int i = 0; i < X.size(); i++)
			{
				
				if (i!=0)
				{
					memDC.MoveTo(X[i-1], Y[i-1]);
					memDC.LineTo(X[i], Y[i]);
				}
				memDC.LineTo(X[i], actual_bottom);
				if (i != 0)memDC.LineTo(X[i - 1], actual_bottom);
			}
		}
		dc->BitBlt(0, 0, frame.Width(), frame.Height(), &memDC, 0, 0, SRCCOPY);
	}
	void SetGraphStart(double y)
	{
		double data_x_min(0); double data_x_max(600);
		double data_y_min(0); double data_y_max(400);
		int padding = 20;
		int left_keys_padding = 20;
		int bottom_keys_padding = 10;

		int actual_width = frame.Width() - 2 * padding - left_keys_padding;
		int actual_height = frame.Height() - 2 * padding - bottom_keys_padding;

		int actual_top = padding;
		int actual_bottom = actual_top + actual_height;
		int actual_left = padding + left_keys_padding;
		int actual_right = actual_left + actual_width;
		vector<double>Y;
		Y.push_back(y);
		Y = convert_range_graph(Y, actual_top, actual_bottom, data_y_max, data_y_min);
		memDC.MoveTo(actual_left, Y[0]);
	}
	vector<double> convert_range_graph(vector <double>& data, double outmax, double outmin, double inmax, double inmin)
	{
		vector<double> output = data;
		double k = (outmax - outmin) / (inmax - inmin);
		for (auto& item : output)
		{
			item = (item - inmin) * k + outmin;
		}

		return output;
	}
	void update_vector(vector<double> &vect)
	{
		for (int i = 0; i < vect.size()-1; i++)
		{
			vect[i] = vect[i + 1];
		}
	}
	vector<double>create_vector_copy(vector<double> x)
	{
		vector <double> copy;
		for (int i = 0; i < x.size(); i++)
		{
			copy.push_back(x[i]);
		}
		return copy;
	}
};