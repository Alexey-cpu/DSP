//-----------------------------------------------------
// A.M. Tykvinsky, 29.09.2020
//-----------------------------------------------------
//ГЕНЕРАТОР СИГНАЛОВ
//-----------------------------------------------------

#ifndef _SIGNAL_GEN_HPP
#define _SIGNAL_GEN_HPP

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "math_const.hpp"

class signal_gen
{

private:

	int       m_cnt;             // внутренний счетчик
	int       m_FramesPerCycle; // количество точек в буффере с опорным синусом
	double    m_Fs;            // частота дискретизации

	double    m_Time;

	double    m_Time_cnt;

public:

	double    *m_buff;   // указатель на массив с выборкой
	double     m_out;   // текущее значение синуса
	double     m_time; // текущее значение времени

	// конструктор:
	signal_gen();

	// деструктор:
	~signal_gen();

	// освобождение памяти:
	void allocate();

	// выделение памяти
	void deallocate();

	// инициализация:
	int GenInit(double Fs, int FramesPerCycle);

	// функции генерирования сигналов:
	int sin_gen(double Fn, double Ph, double Amp);
	int sin_exp_gen(double Fn, double Ph, double Amp, double Tau, double t_start, double d_Amb, double d_pH, int exp_sgin);


	// функции для генерирования сигналов на устройстве типа ТПА-01
	int sin_gen      (double Fn, double Ph, double Amp, double T_delay);                                                                                                                                // установившийся режим
	int sin_step_gen (double Fn, double Ph, double Amp, double d_Amb, double d_pH, double d_F, double T_start, double T_delay);                                                                         // скачок фазы, амплитуды, частоты
	int sin_exp_gen  (double Fn, double Ph, double Amp, double d_Amb, double d_pH, double exp_Tau, int exp_sgin, double t_start, double T_delay);                                                       // КЗ с апериодикой
	int sin_exp_gen  (double Fn, double Ph, double Amp, double d_Amb, double d_pH, double exp_Tau, double d_exp_Amp , int exp_sgin, double t_start, double T_delay);                                    // КЗ с апериодикой, превышающей периодику
	int out_of_step  (double Un, double Fn, double d_Delta, double d_F, double d_U, double Ktt, double R1, double X1, double R2, double X2, double RL, double XL, double T_start, double T_delay);  // простейшая модель асинхронного хода
};

#endif
