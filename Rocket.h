#pragma once
#include<cmath>
#include<fstream>
#include<iostream>
#include<iomanip>
#include"Drawer1.h"
#include"MyDlg.h"
#include<windows.h>

using namespace std;

class Rocket
{
private:
	struct V0
	{
		double val = 0;
		double RecA = 0;
		double RecY = 0;
	};
	struct Coord
	{
		long double x = 0;
		long double y = 0;
		long double z = 0;
	};
	struct GeoC
	{
		double s = 0;
		double d = 0;
	};
	double pi; //����� ��
	GeoC A; // ��������� �����
	GeoC B; //�������� �����
	V0 v0; //��������� �������� ����� �� ������, ������ � ���� ����������
	Coord v;// �������� � ����������
	Coord a;//��������� ����� � ��
	Coord b;//�������� ����� � ��
	Coord CurPos;//������� ���������
	double REarth; //������ �����
	double w; //������� �������� �������� �����
	double dt;//��� �� �������
	double m;//�����
	double g;//��������� ���������� �������
	double G;//�������������� ����������
	double M;//����� �����
	double t;//�����
	double S;//������� ����������� ������� ������ 
	double Cd;//�������������� �����������
	Coord Fd;//���� �������������
	double Fd_abs;
	double RGoal;//���� ���������
	double pl;
protected:
	double RadToDeg(double rad);
	double DegToRad(double Deg);
	double Calc_Distanse_btw_A_B(GeoC AA, GeoC BB);
	Coord GeoToDek(GeoC X);
	GeoC DekToGeo(Coord X);
	void RotateV();
	void CalcVw(Coord& vw);
	double f_no_res(double y_,double x,double y,double z);
	double g_no_res(double x_,double x,double y,double z);
	double h_no_res(double x,double y,double z);
	void Calc_g();
	double Calc_pl();
	void Calc_Fd();
	double fx(double x_, double y_, double z_, double x, double y, double z);
	double gy(double x_, double y_, double z_, double x, double y, double z);
	double hz(double x_, double y_, double z_, double x, double y, double z);
	void Calc_Azimut();
	double Calc_abs_ab(Coord BB);
	double Calc_abs_ab(Coord AA, Coord BB);
	double* convert_to_2D(double x, double y, double z);
	void Update(MyDlg& dlg);
	bool StartRocket_for_hepl();
	
public:
	Rocket();

	void  SetA(double s, double d,double h0);
	GeoC GetA();
	Rocket::Coord Rocket::GeoToDek_For_A(GeoC X, double h0);
	void  SetB(double s, double d);
	GeoC GetB();

	void  SetV0(double val, double RecA, double RecY);
	V0 GetV0();

	void  SetV(double x, double y, double z);
	Coord GetV();
	void Rocket::HELP_ADVICE(MyDlg& dlg);
	bool StartRocket_no_res();
	bool help(MyDlg& dlg);
	bool StartRocket(MyDlg& dlg, Drawer& drv, BOOL gconst, BOOL Fsopr, int Delay, double DT);
	bool help_v2(MyDlg& dlg);
	bool StartRocket_For_Help(MyDlg& dlg, BOOL gconst, BOOL Fsopr, double DT);
};