//-----------------------------------------------------
// A.M. Tykvinsky, 29.09.2020
//-----------------------------------------------------
//ГЕНЕРАТОР СИГНАЛОВ
//-----------------------------------------------------

#include "signal_gen.hpp"

//ГЕНЕРАТОР ОПОРНОГО СИГНАЛА
// ------------------------------------------------------------
int signal_gen::GenInit(double Fs, int FramesPerCycle)
{
	// Заполнение полей:
	m_FramesPerCycle = FramesPerCycle;
	m_Fs             = Fs;
	m_cnt            = 0;
	m_buff           = NULL;

	m_Time = 0;
	m_Time_cnt = 0;
	
	return 0;
}
// ------------------------------------------------------------
// конструктор по умолчанию
signal_gen::signal_gen()
{
		// Заполнение полей:
	m_FramesPerCycle = 16;
	m_Fs             = 4000;
	m_cnt            = 0;
	m_buff           = NULL;

	m_Time = 0;
	m_Time_cnt = 0;
}
// ------------------------------------------------------------
// выделение памяти
void signal_gen::allocate()
{
	// выделяем память:
	m_buff = new double[m_FramesPerCycle];

	// заполняем нулями:
	for (int n = 0; n < m_FramesPerCycle; n++) m_buff[n] = 0;

}
// ------------------------------------------------------------
// освобождение памяти:
void signal_gen::deallocate()
{
	if( m_buff != NULL )
	{
		delete[] m_buff;
		m_buff = NULL;
	}
}
// ------------------------------------------------------------
// деструктор:
signal_gen::~signal_gen() {} // никогда не вызывайте deallocate в деструкторе !!!

// ------------------------------------------------------------
// генерирование синуса
int signal_gen::sin_gen(double Fn, double Ph, double Amp )
{
	
	for (int n = 0; n < m_FramesPerCycle; n++) 
	{
		m_buff[n] = Amp * sin(PI2* Fn * m_cnt / m_Fs + Ph * PI0 / 180);

		m_cnt++;
		if (m_buff[n] == 0 && m_cnt > m_Fs / Fn) 
		{
			m_cnt = 0;
		}

	}

	return 0;
}
// ------------------------------------------------------------
// генерирование косинуса
int signal_gen::sin_exp_gen(double Fn, double Ph, double Amp, double Tau, double t_start, double d_Amb, double d_pH, int exp_sgin)
{
	// Fn      - номинальная частота синуса
	// Ph	   - начальная фаза синуса
	// Amp     - амплитуда синуса
	// Tau     - постоянная времени затухания экспоненты
	// t_start - момент времени начала возмущения
	// d_Amp   - относительное увеличение амплитуды
	// в_Ph    - приращение фазы

	if (exp_sgin > 0) 
	{
		exp_sgin = 1;
	}
	else 
	{
		exp_sgin = -1;
	}

	if (m_Time < t_start)
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{
			m_buff[n] = Amp * sin(PI2* Fn * m_cnt / m_Fs + Ph * PI0 / 180);

			m_cnt++;
			if (m_buff[n] == 0 && m_cnt > m_Fs / Fn) m_cnt = 0;

			m_Time = m_Time + 1 / m_Fs;

		}
	}
	else 
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{
			m_buff[n] = Amp * (1 + d_Amb) * sin(PI2* Fn * m_cnt / m_Fs + Ph * PI0 / 180 + d_pH * PI0 / 180) + exp_sgin * Amp * (1 + d_Amb) * exp(-m_Time / Tau);

			m_cnt++;
			if (m_buff[n] == 0 && m_cnt > m_Fs / Fn) m_cnt = 0;

			m_Time = m_Time + 1 / m_Fs;

		}
	}

	return 0;
}
// ------------------------------------------------------------
// модель установившегося режима:
int signal_gen::sin_gen(double Fn, double Ph, double Amp, double T_delay)
{

	for (int n = 0; n < m_FramesPerCycle; n++)
	{
		m_Time_cnt = m_Time_cnt + 1 / m_Fs;

		if (m_Time_cnt > T_delay)
		{
			m_buff[n] = Amp * sin(PI2* Fn * m_cnt / m_Fs + Ph * PI0 / 180);

			// периодически обнуляем счетчик во избежание ошибки переполнения !!!
			if (m_cnt > 0.5 * INT_MAX)
			{
				m_cnt = 0;
			}
			else
			{
				m_cnt++;
			}
		}
		else
		{
			m_buff[n] = 0;
		}

	}

	return 0;
}
// ------------------------------------------------------------
int signal_gen::sin_step_gen(double Fn, double Ph, double Amp, double d_Amb, double d_pH, double d_F, double T_start, double T_delay)
{
	// Fn       - номинальная частота синуса
	// Ph	    - начальная фаза синуса
	// Amp      - амплитуда синуса
	// d_Amp    - относительное приращение амплитуды (во сколько раз выросла)
	// в_Ph     - приращение фазы в градусах
	// Tau      - постоянная времени затухания экспоненты
	// exp_sign - знак экспоненты ( 1 - положительный, -1 - отрицательный)
	// T_start  - момент времени начала выдачи сигнала
	// T_delay  - время существования предаварийного режима



	if (m_Time_cnt < T_start + T_delay)
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{
			// обновление счетчика времени работы алгоритма:
			m_Time_cnt = m_Time_cnt + 1 / m_Fs;

			if (m_Time_cnt > T_start) // если время превышает время начала выдачи сигнала, то:
			{
				m_buff[n] = Amp * sin(PI2* Fn * m_cnt / m_Fs + Ph * PI0 / 180);

				// периодически обнуляем счетчик во избежание ошибки переполнения !!!
				if (m_cnt > 0.5 * INT_MAX)
				{
					m_cnt = 0;
				}
				else
				{
					m_cnt++;
				}

			} // если время меньше времени начала выдачи сигнала, то выдаем нули:
			else
			{
				m_buff[n] = 0;
			}
		}
	}
	else
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{

			m_buff[n] = Amp * d_Amb * sin(PI2* (Fn + d_F) * m_cnt / m_Fs + Ph * PI0 / 180 + d_pH * PI0 / 180);

			// периодически обнуляем счетчик во избежание ошибки переполнения !!!
			if (m_cnt > 0.5 * INT_MAX)
			{
				m_cnt = 0;
			}
			else
			{
				m_cnt++;
			}

			m_Time = m_Time + 1 / m_Fs;

		}
	}
	return 0;
}
// ------------------------------------------------------------
// модель КЗ с наложением апериодики:
int signal_gen::sin_exp_gen(double Fn, double Ph, double Amp, double d_Amb, double d_pH, double exp_Tau, int exp_sgin, double T_start, double T_delay)
{
	// Fn       - номинальная частота синуса
	// Ph	    - начальная фаза синуса
	// Amp      - амплитуда синуса
	// d_Amp    - относительное приращение амплитуды (во сколько раз выросла)
	// в_Ph     - приращение фазы в градусах
	// Tau      - постоянная времени затухания экспоненты
	// exp_sign - знак экспоненты ( 1 - положительный, -1 - отрицательный)
	// T_start  - момент времени начала выдачи сигнала
	// T_delay  - время существования предаварийного режима

	if (exp_sgin > 0)
	{
		exp_sgin = 1;
	}
	else
	{
		exp_sgin = -1;
	}

	if (m_Time_cnt < T_start + T_delay)
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{
			// обновление счетчика времени работы алгоритма:
			m_Time_cnt = m_Time_cnt + 1 / m_Fs;

			if (m_Time_cnt > T_start) // если время превышает время начала выдачи сигнала, то:
			{
				m_buff[n] = Amp * sin(PI2* Fn * m_cnt / m_Fs + Ph * PI0 / 180);

				// периодически обнуляем счетчик во избежание ошибки переполнения !!!
				if (m_cnt > 0.5 * INT_MAX) 
				{
					m_cnt = 0;
				}
				else
				{
					m_cnt++;
				}

			} // если время меньше времени начала выдачи сигнала, то выдаем нули:
			else 
			{
				m_buff[n] = 0;
			}
		}
	}
	else
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{

			m_buff[n] = Amp * d_Amb * sin(PI2* Fn * m_cnt / m_Fs + Ph * PI0 / 180 + d_pH * PI0 / 180) + exp_sgin * Amp * d_Amb * exp(-m_Time / exp_Tau);

			// периодически обнуляем счетчик во избежание ошибки переполнения !!!
			if (m_cnt > 0.5 * INT_MAX)
			{
				m_cnt = 0;
			}
			else
			{
				m_cnt++;
			}

			m_Time = m_Time + 1 / m_Fs;

		}
	}

	return 0;
}
// ------------------------------------------------------------
// модель КЗ с наложением апериодики превышающей периодику:
int signal_gen::sin_exp_gen(double Fn, double Ph, double Amp, double d_Amb, double d_pH, double exp_Tau, double d_exp_Amp, int exp_sgin, double T_start, double T_delay)
{
	// Fn        - номинальная частота синуса
	// Ph	     - начальная фаза синуса
	// Amp       - амплитуда синуса
	// d_Amp     - относительное приращение амплитуды (во сколько раз выросла)
	// в_Ph      - приращение фазы в градусах
	// Tau       - постоянная времени затухания экспоненты
	// exp_sign  - знак экспоненты ( 1 - положительный, -1 - отрицательный)
	// d_exp_Amp - во сколько раз амплитуда экспоненты превышает амплитуду периодики
	// T_start   - момент времени начала выдачи сигнала
	// T_delay   - время существования предаварийного режима

	if (exp_sgin > 0)
	{
		exp_sgin = 1;
	}
	else
	{
		exp_sgin = -1;
	}

	if (m_Time_cnt < T_start + T_delay)
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{
			// обновление счетчика времени работы алгоритма:
			m_Time_cnt = m_Time_cnt + 1 / m_Fs;

			if (m_Time_cnt > T_start) // если время превышает время начала выдачи сигнала, то:
			{
				m_buff[n] = Amp * sin(PI2* Fn * m_cnt / m_Fs + Ph * PI0 / 180);

				// периодически обнуляем счетчик во избежание ошибки переполнения !!!
				if (m_cnt > 0.5 * INT_MAX)
				{
					m_cnt = 0;
				}
				else
				{
					m_cnt++;
				}

			} // если время меньше времени начала выдачи сигнала, то выдаем нули:
			else
			{
				m_buff[n] = 0;
			}
		}
	}
	else
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{

			m_buff[n] = Amp * (1 + d_Amb) * sin(PI2* Fn * m_cnt / m_Fs + Ph * PI0 / 180 + d_pH * PI0 / 180) + d_exp_Amp * exp_sgin * Amp * d_Amb * exp(-m_Time / exp_Tau);

			// периодически обнуляем счетчик во избежание ошибки переполнения !!!
			if (m_cnt > 0.5 * INT_MAX)
			{
				m_cnt = 0;
			}
			else
			{
				m_cnt++;
			}

			m_Time = m_Time + 1 / m_Fs;

		}
	}

	return 0;
}
int signal_gen::out_of_step(double Un, double Fn, double d_Delta, double d_F, double d_U, double Ktt, double R1, double X1, double R2, double X2, double RL, double XL, double T_start, double T_delay)
{
	// Un       - номинальное напряжение сети                 , кВ
	// Fn       - номинальная частота    сети                 , Гц
	// d_Delta  - разница фаз ЭДС эквивалентов                , град
	// d_F      - разница частот ЭДС эквивалентов             , Гц
	// d_U      - разница уровней напряжений ЭДС эквивалентов , %
	// Ktt      - коэффициент трансформации ТТ                , о.е.
	// R1       - активное сопротивление эквивалента 1        , Ом
	// X1       - реактивное сопротивление эквивалента 1      , Ом
	// R2       - активное сопротивление эквивалента 2        , Ом
	// X2       - реактивное сопротивление эквивалента 2      , Ом
	// RL       - активное сопротивление линии связи          , Ом
	// XL       - реактивное сопротивление линии связи        , Ом
	// T_start  - время начала выдачи сигналов                , секунды
	// T_delay  - время начала переходного процесса           , секунды

	// Вспомогательные переменные:

	double Re_Zeq = R1 + R2 + RL; // активное   сопротивление эквивалентной линии связи
	double Im_Zeq = X1 + X2 + XL; // реактивное сопротивление эквивалентной линии связи
	double Re_E1 = 0;             // проекция эквивалентной ЭДС 1 на ось X
	double Im_E1 = 0;             // проекция эквивалентной ЭДС 1 на ось Y
	double Re_E2 = 0;             // проекция эквивалентной ЭДС 2 на ось X
	double Im_E2 = 0;             // проекция эквивалентной ЭДС 1 на ось Y
	double Re_dU = 0;             // проекция падения напряжения на линии связи на ось X
	double Im_dU = 0;             // проекция падения напряжения на линии связи на ось Y
	double Re_dI = 0;             // проекция тока качаний на ось X
	double Im_dI = 0;             // проекция тока качаний на ось Y


	if (m_Time_cnt < T_start + T_delay) // установившийся ( предаварийный ) режим
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{
			// обновление счетчика времени работы алгоритма:
			m_Time_cnt = m_Time_cnt + 1 / m_Fs;

			if (m_Time_cnt > T_start) // если время превышает время начала выдачи сигнала, то:
			{

				// расчетная ЭДС эквивалента 1:
				Re_E1 = 1000 * (1 + d_U / 100) * Un * cos(PI2 * Fn * m_cnt / m_Fs + d_Delta * PI0 / 180);
				Im_E1 = 1000 * (1 + d_U / 100) * Un * sin(PI2 * Fn * m_cnt / m_Fs + d_Delta * PI0 / 180);

				// расчетная ЭДС эквивалента 1:
				Re_E1 = 1000 * Un * cos(PI2 * Fn * m_cnt / m_Fs);
				Im_E1 = 1000 * Un * sin(PI2 * Fn * m_cnt / m_Fs);

				// Расчет разностного напряжения :
				Re_dU = Re_E1 - Re_E2;
				Im_dU = Im_E1 - Im_E2;

				// Расчет вектора тока качаний :
				Re_dI     = (Re_dU * Re_Zeq + Im_dU * Im_Zeq) / (Re_Zeq * Re_Zeq + Im_Zeq * Im_Zeq);
				Im_dI     = (Im_dU * Re_Zeq - Re_dU * Im_Zeq) / (Re_Zeq * Re_Zeq + Im_Zeq * Im_Zeq);

				// расчет мгновенных значений тока качаний:
				m_buff[n] = sqrt( Re_dI * Re_dI + Im_dI * Im_dI ) * sin( atan2( Im_dI , Re_dI ) ) / Ktt;

				// периодически обнуляем счетчик во избежание ошибки переполнения !!!
				if (m_cnt > 0.5 * INT_MAX)
				{
					m_cnt = 0;
				}
				else
				{
					m_cnt++;
				}

			} // если время меньше времени начала выдачи сигнала, то выдаем нули:
			else
			{
				m_buff[n] = 0;
			}
		}
	}
	else // расчет переходного режима:
	{
		for (int n = 0; n < m_FramesPerCycle; n++)
		{

			// расчетная ЭДС эквивалента 1:
			Re_E1 = 1000 * (1 + d_U / 100) * Un * cos(PI2 * (Fn + d_F) * m_cnt / m_Fs + d_Delta * PI0 / 180);
			Im_E1 = 1000 * (1 + d_U / 100) * Un * sin(PI2 * (Fn + d_F) * m_cnt / m_Fs + d_Delta * PI0 / 180);

			// расчетная ЭДС эквивалента 1:
			Re_E1 = 1000 * Un * cos(PI2 * Fn * m_cnt / m_Fs);
			Im_E1 = 1000 * Un * sin(PI2 * Fn * m_cnt / m_Fs);

			// Расчет разностного напряжения :
			Re_dU = Re_E1 - Re_E2;
			Im_dU = Im_E1 - Im_E2;

			// Расчет вектора тока качаний :
			Re_dI = (Re_dU * Re_Zeq + Im_dU * Im_Zeq) / (Re_Zeq * Re_Zeq + Im_Zeq * Im_Zeq);
			Im_dI = (Im_dU * Re_Zeq - Re_dU * Im_Zeq) / (Re_Zeq * Re_Zeq + Im_Zeq * Im_Zeq);

			// расчет мгновенных значений тока качаний:
			m_buff[n] = sqrt(Re_dI * Re_dI + Im_dI * Im_dI) * sin(atan2(Im_dI, Re_dI)) / Ktt;

			// периодически обнуляем счетчик во избежание ошибки переполнения !!!
			if (m_cnt > 0.5 * INT_MAX)
			{
				m_cnt = 0;
			}
			else
			{
				m_cnt++;
			}

			m_Time = m_Time + 1 / m_Fs;

		}
	}

	return 0;
}