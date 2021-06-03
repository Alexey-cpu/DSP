// Стандартные библиотеки компилятора gcc:

#include <iostream>
#include <fstream>
#include <cassert>
#include <math.h>
#include <cmath>

// Генератор тестовых сигналов:

#include "signal_gen.hpp"
////
// Классы фильтров:

#include "Chebyshev_II.hpp"
#include "Chebyshev_I.hpp"
#include "Butterworth.hpp"
#include "Elliptic.hpp"
#include "FIR_filt.hpp"
#include "simple_IIR.hpp"
#include "aperiodic_filt.hpp"
#include "aperiodic_filt_eq.hpp"

// Специальные математические функции и операции с комплексными числами:

#include "special_functions.hpp"
#include "cplx_operations.hpp"

// ортогональные преобразования:
#include "orto_transformations_3ph.hpp"
#include "orto_transformations_1ph.hpp"

// интеграторы:
#include "integrators.hpp"

// Вычислитель РЗиА:
#include "quad_mltpx.hpp"

// Проверка работоспособности классических БИХ-фильтров:

int test1()
{
	std::cout << "Calculation started !" << "\n";
	//-------------------------------------------------------------------------------
	// ФИЛЬТР
	//-------------------------------------------------------------------------------
	// создание объекта нужного класса:
	//Butterworth  CH1;
	//Chebyshev_I  CH1;
	//Chebyshev_II CH1;
	Elliptic     CH1;

	// Инициализация Эллиптического фильтра:
	//	CH1.LP_Init(4000 , 50 , 1 , 80 , 100 , 6 , false, false);          // ФНЧ
	//	CH1.HP_Init(4000 , 50 , 1 , 80 , 500 , 6 , false, false);          // ФВЧ
	//	CH1.BP_Init(4000 , 50 , 1 , 80 , 150  , 500 , 6 , false, false);   // ПФ
		CH1.BS_Init(4000 , 50 , 1 , 80  , 50 , 800 ,  8 , false, false); // РФ

	// Инициализация фильтров Баттерворта и Чебышева:
	//CH1.LP_Init(4000 , 50 , 1 , 500 , 8 , false , false);                // ФНЧ
	//CH1.HP_Init(4000 , 50 , 1 , 800 , 9 , false , false);                // ФВЧ
	//CH1.BP_Init(4000, 50, 80, 100 , 500, 14, false , false);               // ПФ
	//CH1.BS_Init(4000, 50, 80, 150, 500, 14, false , false);                  // РФ

	// выделение памяти с расчетом коэффициентов:
	CH1.allocate();

	//-------------------------------------------------------------------------------
	// ГЕНЕРАТОР СИГНАЛОВ:
	//-------------------------------------------------------------------------------
	// Генераторы сигналов:
	signal_gen GEN_MAIN;
	GEN_MAIN.GenInit(4000, 16);
	GEN_MAIN.allocate();

	//-------------------------------------------------------------------------------
	// ПОТОКИ ДЛЯ ЗАПИСИ В ФАЙЛ:
	//-------------------------------------------------------------------------------
	// Потоки для записи в файл
	std::ofstream file_out1;
	std::ofstream file_out2;
	std::ofstream file_out3;
	std::ofstream file_out4;
	std::ofstream file_out5;

	// потоки для записи в файл:
	file_out1.open("E:\\coding\\SIGNAL_NOT_FILT.txt");
	file_out2.open("E:\\coding\\SIGNAL_FILT.txt");
	file_out3.open("E:\\coding\\F.txt");
	file_out4.open("E:\\coding\\Km.txt");
	file_out5.open("E:\\coding\\pH.txt");

	for (int n = 0; n < 10; n++)
	{
		// Генерирование основного сигнала:
		GEN_MAIN.sin_gen(50, 0, 1);

		// Эмуляция прохода по сигналу фильтом:
		for (int cnt1 = 0; cnt1 < 16; cnt1++)
		{
			// запись в файл нефильтрованного сигнала:
			file_out1 << GEN_MAIN.m_buff[cnt1] << "\n";
			// фильтрация сигнала:
			CH1.filt(&GEN_MAIN.m_buff[cnt1]);
			// запись в файл отфильтрованного сигнала:
			file_out2 << CH1.m_out << "\n";
		}
	}

	// Построение АЧХ и ФЧХ с выгрузкой в файлы:
	for (int n = 1; n < 2000; n++)
	{
		CH1.m_in_F = n;
		CH1.FreqCharacteristics();

		file_out3 << n << "\n";
		file_out4 << CH1.m_Km << "\n";
		file_out5 << CH1.m_pH << "\n";
	}

	// освобождение памяти:
	CH1		.deallocate();
	GEN_MAIN.deallocate();

	// закрытие потоков для записи в файл:
	file_out1.close();
	file_out2.close();
	file_out3.close();
	file_out4.close();
	file_out5.close();

	std::cout << "Calculation finished !" << "\n";

	return 0;
}

// Пример работы с классом классических КИХ-фильnров: 

int test2() 
{
	//-------------------------------------------------------------------------------
	// ФИЛЬТР:
	//-------------------------------------------------------------------------------
	// Создание объекта нужного класса:
	FIR_filt CH1;

	// инициализация фильтра:
	 CH1.LP_Init(4000, 50, 0.1 , 95, true);          // инициализация ФНЧ
	// CH1.HP_Init(4000, 50, 100, 95, true);        // инициализация ФВЧ
	// CH1.BP_Init(4000, 50, 100, 200, 11, true);   // инициализация ПФ
	// CH1.BS_Init(4000, 50, 100, 200, 13, true);   // инициализация РФ
	// CH1.CF_Init(4000, 50, 80);
	// CH1.SF_Init(4000, 50, 80);
	// CH1.HF_Init(4000, 50, 90 , 80);

	// проектирование сглаживающей оконной функции:
	CH1.m_WIND_FCN.Chebyshev(60);

	// выделение памяти с расчетом коэффициентов:
	CH1.allocate();

	// Смотрим спецификацию фильтра:
	CH1.ShowFiltSpec();

	// Смотрим коэффициенты фильтра:
	CH1.ShowFiltCoeffs();

	//-------------------------------------------------------------------------------
	// ГЕНЕРАТОР СИГНАЛОВ:
	//-------------------------------------------------------------------------------

	// Генератор сигналов:
	signal_gen GEN_MAIN;
	GEN_MAIN.GenInit(4000, 16);
	GEN_MAIN.allocate();

	//-------------------------------------------------------------------------------
	// ПОТОКИ ДЛЯ ЗАПИСИ В ФАЙЛ:
	//-------------------------------------------------------------------------------

	// Потоки для записи в файл
	std::ofstream file_out1;
	std::ofstream file_out2;
	std::ofstream file_out3;
	std::ofstream file_out4;
	std::ofstream file_out5;

	// потоки для записи в файл:
	file_out1.open("E:\\coding\\SIGNAL_NOT_FILT.txt");
	file_out2.open("E:\\coding\\SIGNAL_FILT.txt");
	file_out3.open("E:\\coding\\F.txt");
	file_out4.open("E:\\coding\\Km.txt");
	file_out5.open("E:\\coding\\pH.txt");

	file_out1.clear();
	file_out2.clear();
	file_out3.clear();
	file_out4.clear();
	file_out5.clear();

	for (int n = 0; n < 20; n++)
	{
		// Генерирование основного сигнала:
		GEN_MAIN.sin_gen(100, 0, 1);

		// Эмуляция прохода по сигналу фильтом:
		for (int cnt1 = 0; cnt1 < 16; cnt1++)
		{
			// запись в файл нефильтрованного сигнала:
			file_out1 << GEN_MAIN.m_buff[cnt1] << "\n";
			// фильтрация сигнала:
			CH1.filt(&GEN_MAIN.m_buff[cnt1]);
			// запись в файл отфильтрованного сигнала:
			file_out2 << CH1.m_out << "\n";
		}
	}

	// Построение АЧХ и ФЧХ с выгрузкой в файлы:
	for (int n = 0; n < 2000; n++)
	{
		file_out3 << n << "\n";
		CH1.m_in_F = n;
		CH1.FreqCharacteristics();

		file_out4 << CH1.m_Km << "\n";
		//file_out5 << CH1.m_pH << "\n";
	}

	/*for (int n = 0; n < CH1.m_BUFF_WIND_CX.getBuffSize(); n++)
	{
	   file_out5 << CH1.m_BUFF_WIND_CX.m_buff[n] << "\n";

	   //std::cout<<"c["<<n<<"] = "<< CH1.m_BUFF_WIND_CX.m_buff[n] << "\n";
	}*/

	// Освобождение памяти:
	CH1     .deallocate();
	GEN_MAIN.deallocate();

	// Закрытие потоков для записи в файл:
	file_out1.close();
	file_out2.close();
	file_out3.close();
	file_out4.close();
	file_out5.close();

	return 0;
}

// Пример работы с классом простейших БИХ-фильтров:

int test3() 
{
	//-------------------------------------------------------------------------------
	// ФИЛЬТР:
	//-------------------------------------------------------------------------------

	// Подключение нужного класса:
    simple_IIR CH1;

	// инициализация простейших БИХ-фильтров:
	
	// Инициализация типовых динамических звеньев:
	//CH1.DIF_Init(4000, 50, 0.00001);           // дифференцирующее звено с запаздыванием
	//CH1.EXP_Init(4000, 50, 0.001);           // апериодическое звено
	//CH1.LEG_Init(4000, 50, 0.001 , 0.01);    // фазосдвигающее звено
	//CH1.INT_Init(4000 , 50);                 // интегрирующее звено

	// Инициализация фильтров второго порядка:
	CH1.LP_Init(4000, 50, 0.707, 120);        // ФНЧ второго порядка
	//CH1.HP_Init(4000, 50, 0.707, 120);      // ФВЧ второго порядка
	//CH1.BP_Init(4000, 50, 0.707, 120);      // ПФ  второго порядка
	//CH1.BS1_Init(4000, 50 , 0.707 , 120);   // РФ  второго порядка 1 типа (Билинейное преобразование передаточной функции аналогового фильтра)
	//CH1.BS2_Init(4000, 50, 120, 50);        // РФ  второго порядка 2 типа (синтез режекторного фильтра на основе всепропускающего фильтра    )
	
	// выделеине памяти с расчетом коэффициентов:
	CH1.allocate();

	//-------------------------------------------------------------------------------
	// ГЕНЕРАТОР СИГНАЛОВ:
	//-------------------------------------------------------------------------------

	// Генераторы сигналов:
	signal_gen GEN_MAIN;
	GEN_MAIN.GenInit(4000, 16);
	GEN_MAIN.allocate();

	//-------------------------------------------------------------------------------
	// ПОТОКИ ДЛЯ ЗАПИСИ В ФАЙЛ:
	//-------------------------------------------------------------------------------

	// Потоки для записи в файл
	std::ofstream file_out1;
	std::ofstream file_out2;
	std::ofstream file_out3;
	std::ofstream file_out4;
	std::ofstream file_out5;

	// Директории потоков для записи в файл:
	file_out1.open("E:\\coding\\SIGNAL_NOT_FILT.txt");
	file_out2.open("E:\\coding\\SIGNAL_FILT.txt");
	file_out3.open("E:\\coding\\F.txt");
	file_out4.open("E:\\coding\\Km.txt");
	file_out5.open("E:\\coding\\pH.txt");

	for (int n = 0; n < 5; n++)
	{
		// Генерирование основного сигнала:
		GEN_MAIN.sin_gen(50, 0, 1);

		// Эмуляция прохода по сигналу фильтом:
		for (int cnt1 = 0; cnt1 < 16; cnt1++)
		{
			// запись в файл нефильтрованного сигнала:
			file_out1 << GEN_MAIN.m_buff[cnt1] << "\n";
			// фильтрация сигнала:
			CH1.filt(&GEN_MAIN.m_buff[cnt1]);
			// запись в файл отфильтрованного сигнала:
			file_out2 << CH1.m_out << "\n";
		}
	}

	// Построение АЧХ и ФЧХ с выгрузкой в файлы:
	for (int n = 1; n < 2000; n++)
	{
		CH1.m_in_F = n;
		CH1.FreqCharacteristics();

		file_out3 << n << "\n";
		file_out4 << CH1.m_Km << "\n";
		file_out5 << CH1.m_pH << "\n";
	}

	// освобождение памяти:
	CH1     .deallocate();
	GEN_MAIN.deallocate();

	// закрытие потоков для записи в файл:
	file_out1.close();
	file_out2.close();
	file_out3.close();
	file_out4.close();
	file_out5.close();

	return 0;
}

// Пример работы с классом специальных математических функций:

int test4() 
{
	special_functions f_ellip;

	// обратные эллиптические функции:
	std::cout << "u = " << 0.1 << "		" << "k = " << 0.5 << "\n" << "\n";
	std::cout << " inv_CN = " << f_ellip.inv_cn(f_ellip.cn(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_SN = " << f_ellip.inv_sn(f_ellip.sn(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_DN = " << f_ellip.inv_dn(f_ellip.dn(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_CD = " << f_ellip.inv_cd(f_ellip.cd(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_SD = " << f_ellip.inv_sd(f_ellip.sd(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_ND = " << f_ellip.inv_nd(f_ellip.nd(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_DC = " << f_ellip.inv_dc(f_ellip.dc(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_NC = " << f_ellip.inv_nc(f_ellip.nc(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_SC = " << f_ellip.inv_sc(f_ellip.sc(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_NS = " << f_ellip.inv_ns(f_ellip.ns(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_DS = " << f_ellip.inv_ds(f_ellip.ds(0.5, 0.5), 0.5) << "\n";
	std::cout << " inv_CS = " << f_ellip.inv_cs(f_ellip.cs(0.5, 0.5), 0.5) << "\n";

	// эллиптические интегралы:
	std::cout << " K      = " << f_ellip.ellip_K(0.5) << "\n";
	std::cout << " E      = " << f_ellip.ellip_E(0.5) << "\n";

	return 0;
}

// Пример работы с классом трехфазных ортогональных преобразований:

int test5() 
{
	orto_transformations_3ph TRANSFORMATION_3PH;

	// Генераторы сигналов:
	signal_gen GEN_Ia;
	signal_gen GEN_Ib;
	signal_gen GEN_Ic;

	GEN_Ia.GenInit(4000, 16);
	GEN_Ib.GenInit(4000, 16);
	GEN_Ic.GenInit(4000, 16);
	GEN_Ia.allocate();
	GEN_Ib.allocate();
	GEN_Ic.allocate();

	// Потоки для записи в файл
	std::ofstream file_out1;
	std::ofstream file_out2;
	std::ofstream file_out3;
	std::ofstream file_out4;
	std::ofstream file_out5;
	std::ofstream file_out6;
	std::ofstream file_out7;
	std::ofstream file_out8;
	std::ofstream file_out9;

	// потоки для записи в файл:
	file_out1.open("E:\\coding\\Ia.txt");
	file_out2.open("E:\\coding\\Ib.txt");
	file_out3.open("E:\\coding\\Ic.txt");
	file_out4.open("E:\\coding\\ALPHA.txt");
	file_out5.open("E:\\coding\\BETTA.txt");
	file_out6.open("E:\\coding\\ZERO1.txt");
	file_out7.open("E:\\coding\\DIRECT.txt");
	file_out8.open("E:\\coding\\QUADRATURE.txt");
	file_out9.open("E:\\coding\\ZERO2.txt");

	for (int n = 0; n < 5; n++)
	{
		// Генерирование основного сигнала:
		GEN_Ia.sin_gen(50, 0.00, 1);
		GEN_Ib.sin_gen(50, -120, 1);
		GEN_Ic.sin_gen(50, +120, 1);

		// Эмуляция прохода по сигналу фильтом:
		for (int cnt1 = 0; cnt1 < 16; cnt1++)
		{
			// запись в файл синусоид фаз А, В и С:
			file_out1 << GEN_Ia.m_buff[cnt1] << "\n";
			file_out2 << GEN_Ib.m_buff[cnt1] << "\n";
			file_out3 << GEN_Ic.m_buff[cnt1] << "\n";

			// вычисление преобразований Кларка, Парка:
			TRANSFORMATION_3PH.Clark( &GEN_Ia.m_buff[cnt1], &GEN_Ib.m_buff[cnt1], &GEN_Ic.m_buff[cnt1]);
			TRANSFORMATION_3PH.Park ( &GEN_Ia.m_buff[cnt1], &GEN_Ib.m_buff[cnt1], &GEN_Ic.m_buff[cnt1] , 50 , 4000);
		
			// запись в файл результатов преобразования Кларка:
			file_out4 << TRANSFORMATION_3PH.m_A << "\n";
			file_out5 << TRANSFORMATION_3PH.m_B << "\n";
			file_out6 << TRANSFORMATION_3PH.m_Z  << "\n";

			// запись в файл результатов преобразования Парка:
			file_out7 << TRANSFORMATION_3PH.m_D << "\n";
			file_out8 << TRANSFORMATION_3PH.m_Q << "\n";
			file_out9 << TRANSFORMATION_3PH.m_Z << "\n";
		}
	}

	// освобождение памяти:
	GEN_Ia.deallocate();
	GEN_Ib.deallocate();
	GEN_Ic.deallocate();

	// закрытие потоков для записи в файл:
	file_out1.close();
	file_out2.close();
	file_out3.close();
	file_out4.close();
	file_out5.close();
	file_out6.close();
	file_out7.close();
	file_out8.close();
	file_out9.close();

	return 0;
}

// Пример работы с классом однофазных ортогональных преобразований:

int test6() 
{
	int	HBuffSize  = 20;
	int	NumOfSteps = 32;

	// фильтр ортогональных составляющих:
	orto_transformations_1ph TRANSFORMATION_1PH_MAIN;
	orto_transformations_1ph TRANSFORMATION_1PH_REFF;
	TRANSFORMATION_1PH_MAIN.DFT_Init(4000 , 50 , 120 );
	TRANSFORMATION_1PH_REFF.DFT_Init(4000 , 50 );
	// проектирование оконной функции:
	TRANSFORMATION_1PH_MAIN.m_WIND_FCN.Chebyshev(60);
	TRANSFORMATION_1PH_MAIN.allocate();
	TRANSFORMATION_1PH_REFF.allocate();

	// Генераторы сигналов:
	signal_gen GEN_MAIN;
	signal_gen GEN_REFF;
	GEN_MAIN.GenInit(4000 , HBuffSize);
	GEN_REFF.GenInit(4000 , HBuffSize);
	GEN_MAIN.allocate();
	GEN_REFF.allocate();

	// функциональный блок операций с комплексными числами:
	cplx_operations m_CPLX_OP;

	// Потоки для записи в файл:
	std::ofstream file_out1;
	std::ofstream file_out2;
	std::ofstream file_out3;
	std::ofstream file_out4;
	std::ofstream file_out5;
	std::ofstream file_out6;
	std::ofstream file_out7;

	// потоки для записи в файл:
	file_out1.open("E:\\coding\\SIGNAL_MAIN.txt");
	file_out2.open("E:\\coding\\SIGNAL_REFF.txt");
	file_out3.open("E:\\coding\\RE.txt");
	file_out4.open("E:\\coding\\IM.txt");
	file_out5.open("E:\\coding\\Km.txt");
	file_out6.open("E:\\coding\\pH.txt");
	file_out7.open("E:\\coding\\F.txt");


	//--------------------------------------------------------
	// Проверка работы функции фильтрации:
	for (int n = 0; n < NumOfSteps; n++)
	{
		// Генерирование основного ипорного сигнала:
		GEN_MAIN.sin_gen(45, 0, 1);
		GEN_REFF.sin_gen(50, 0, 1);

		// Эмуляция прохода по сигналу фильтом:
		for (int cnt1 = 0; cnt1 < HBuffSize; cnt1++)
		{
			// запись в файл основного и опорного сигналов:
			file_out1 << GEN_MAIN.m_buff[cnt1] << "\n";
			file_out2 << GEN_REFF.m_buff[cnt1] << "\n";

			// заполнение внутренних буфферов фильтров ортогональных составляющих основного и опорного сигналов:
			TRANSFORMATION_1PH_MAIN.m_BUFF_WIND_SX.fill_buff( &GEN_MAIN.m_buff[cnt1] , 1 );
			TRANSFORMATION_1PH_REFF.m_BUFF_WIND_SX.fill_buff( &GEN_REFF.m_buff[cnt1] , 1 );

			// вызов функций фильтрации фильтров ортогональных составляющих основного и опорного сигналов:
			TRANSFORMATION_1PH_MAIN.filt();
			TRANSFORMATION_1PH_REFF.filt();

			// круговая сверка спектров основого и опорного сигналов:
			m_CPLX_OP.mul(TRANSFORMATION_1PH_MAIN.m_Re, TRANSFORMATION_1PH_MAIN.m_Im, TRANSFORMATION_1PH_REFF.m_Re, -TRANSFORMATION_1PH_REFF.m_Im);

			file_out3 << m_CPLX_OP.m_Re << "\n";
			file_out4 << m_CPLX_OP.m_Im << "\n";
		}

		// запись в файл полученных значений (сделано в цикле, чтобы частота дискретизации орт.составляющих и сигналов была одинаковой):
		/*for (int cnt1 = 0; cnt1 < HBuffSize; cnt1++)
		{
			file_out3 << m_CPLX_OP.m_Re << "\n";
			file_out4 << m_CPLX_OP.m_Im << "\n";
		}*/

	}
	//--------------------------------------------------------
	// Проверка работы функции расчета коэффициентов АЧХ и ФЧХ:
	for (int n = 0; n < 2000; n++)
	{
		// подача на вход функционального блока текущей частоты:
		TRANSFORMATION_1PH_MAIN.m_in_F = n;
		// вызов функции расчета коэффициентов АЧХ и ФЧХ:
		TRANSFORMATION_1PH_MAIN.FreqCharacteristics();

		// Запись результата расчета в файл:
		file_out5 << TRANSFORMATION_1PH_MAIN.m_Km << "\n";
		file_out6 << TRANSFORMATION_1PH_MAIN.m_pH << "\n";
		file_out7 << n                            << "\n";
	}
	//--------------------------------------------------------

	// закрываем потоки для записи в файл:
	file_out1.close();
	file_out2.close();
	file_out3.close();
	file_out4.close();

	// освобождение памяти из-под проверяемых функциональных блоков:
	GEN_MAIN.deallocate();
	GEN_REFF.deallocate();
	TRANSFORMATION_1PH_MAIN.deallocate();
	TRANSFORMATION_1PH_REFF.deallocate();

	return 0;
}

// Пример работы с классом кольцевого буффера:

int test7()
{
	// Создание объекта нужного класса:
	mirror_ring_buff MBUFF;

	// инициализация буффера:
	MBUFF.BuffInit(5);

	// выделеине памяти под буффер:
	MBUFF.allocate(true);

	// создание тестового массива и заполнение его нулями:
	double *a = new double[10];
	for (int n = 0; n < 10; n++) a[n] = n;

	// заполняем буффер:
	MBUFF.fill_buff(a, 10);

	// выводим буффер на экран:
	MBUFF.ShowBuff();

	// очищаем память:
	MBUFF.deallocate();

	// выводим буффер на экран:
	MBUFF.ShowBuff();

	delete[] a;

	return 0;
}

// Пример работы с классом оконных функций:

int test8() 
{

	// создание объекта нужного класса:
	wind_fcn WIND;
	
	// инициализация оконной функции:
	WIND.windInit(120);

	// проектирование оконной функции:
	WIND.Parzen();

	// Потоки для записи в файл
	std::ofstream file_out1;
	file_out1.open("E:\\coding\\WIND_FCN.txt");

	// запсь коэффициентов оконной функции в файл:
	for (int n = 0 ; n < 120; n++) 
	{
		file_out1 << WIND.m_BUFF_WIND[n] << "\n";
	}

	// закрываем записанный файл:
	file_out1.close();

	// освобождаем память:
	WIND.deallocate();

	return 0;
}

// Пример построения АЧХ каскада фильтров:

int test9() 
{
	// создание нужных объектов:

	// дискретное преобразование Фурье:
	FIR_filt F1;
	F1.HF_Init(4000 , 50 , 0 , 80);
	F1.m_WIND_FCN.Rectangular();
	F1.allocate();

	FIR_filt F2;
	F2.CF_Init(4000, 50, 20);
	F2.m_WIND_FCN.Rectangular();
	F2.allocate();

	// фильтр апериодической слагающей:
	aperiodic_filt_eq F3;
	F3.filtInit(4000 , 50 , 5 , 0);
	F3.allocate();

	//
	double Km = 0;
	double pH = 0;

	// потоки для записи в файл:
	std::ofstream file_out1;
	std::ofstream file_out2;
	std::ofstream file_out3;
	std::ofstream file_out4;
	std::ofstream file_out5;

	// потоки для записи в файл:
	file_out1.open("E:\\coding\\SIGNAL_NOT_FILT.txt");
	file_out2.open("E:\\coding\\SIGNAL_FILT.txt");
	file_out3.open("E:\\coding\\F.txt");
	file_out4.open("E:\\coding\\Km.txt");
	file_out5.open("E:\\coding\\pH.txt");

	// Построение АЧХ и ФЧХ каскада цифровых фильтров:
	for (int n = 0; n < 2000; n++)
	{
		// задаем частоту входного сигнала на всех фильтрах каскада:
		F1.m_in_F = n;
		F2.m_in_F = n;
		F3.m_in_F = n;
		
		// расчет АЧХ и ФЧХ всех фильтров каскада:
		F1.FreqCharacteristics();
		F2.FreqCharacteristics();
		F3.FreqCharacteristics();

		// Расчет коэффициентов АЧХ и ФЧХ:
		//Km = F1.m_Km * F3.m_Km;
		//pH = F1.m_pH + F3.m_pH;

		Km = F1.m_Km;
		pH = F1.m_pH;

		// выгрузка АЧХ и ФЧХ каскада в файл:
		file_out3 << n << "\n";
		file_out4 << Km << "\n";
		file_out5 << pH << "\n";
		file_out5 << "Calculation is completed !!!" << "\n";
	}


	return 0;
}

// Орган манипуляции ДФЗ, сделанный в соответствии с методикой ПАО ФСК ЕЭС:

int test10() 
{
	int HBuffSize  = 20;
	int NumOfSteps = 100;

	//-------------------------------------------------------
	// Создаем фильтры Фурье для токов фаз А , В , С:
	orto_transformations_1ph DFT_A;
	orto_transformations_1ph DFT_B;
	orto_transformations_1ph DFT_C;
	
	// инициализация:
	DFT_A.DFT_Init(4000, 50 , 80);
	DFT_B.DFT_Init(4000, 50 , 80);
	DFT_C.DFT_Init(4000, 50 , 80);
	
	// проектирование оконной функции:
	DFT_A.m_WIND_FCN.Rectangular();
	DFT_B.m_WIND_FCN.Rectangular();
	DFT_C.m_WIND_FCN.Rectangular();

	// выделение памяти:
	DFT_A.allocate();
	DFT_B.allocate();
	DFT_C.allocate();
	//-------------------------------------------------------
	// Генераторы сигналов:
	signal_gen GEN_Ia;
	signal_gen GEN_Ib;
	signal_gen GEN_Ic;

	// инициализация:
	GEN_Ia.GenInit(4000, HBuffSize);
	GEN_Ib.GenInit(4000, HBuffSize);
	GEN_Ic.GenInit(4000, HBuffSize);

	// выделение памяти:
	GEN_Ia.allocate();
	GEN_Ib.allocate();
	GEN_Ic.allocate();

	// блок для комплексного перемножения:
	cplx_operations CPLX_OPERATIONS1;
	cplx_operations CPLX_OPERATIONS2;
	cplx_operations CPLX_OPERATIONS3;

	//-------------------------------------------------------
	// Потоки для записи в файл
	std::ofstream file_out1;
	std::ofstream file_out2;
	std::ofstream file_out3;
	std::ofstream file_out4;
	std::ofstream file_out5;
	std::ofstream file_out6;
	std::ofstream file_out7;
	std::ofstream file_out8;
	std::ofstream file_out9;
	std::ofstream file_out10;
	std::ofstream file_out11;
	std::ofstream file_out12;
	std::ofstream file_out13;
	std::ofstream file_out14;

	// инициализация:
	file_out1.open ("E:\\coding\\Ia.txt");
	file_out2.open ("E:\\coding\\Ib.txt");
	file_out3.open ("E:\\coding\\Ic.txt");
	file_out4.open ("E:\\coding\\Re_Ia.txt");
	file_out5.open ("E:\\coding\\Im_Ia.txt");
	file_out6.open ("E:\\coding\\Re_Ib.txt");
	file_out7.open ("E:\\coding\\Im_Ib.txt");
	file_out8.open ("E:\\coding\\Re_Ic.txt");
	file_out9.open ("E:\\coding\\Im_Ic.txt");
	file_out10.open("E:\\coding\\Re_I1.txt");
	file_out11.open("E:\\coding\\Im_I1.txt");
	file_out12.open("E:\\coding\\Re_I2.txt");
	file_out13.open("E:\\coding\\Im_I2.txt");
	file_out14.open("E:\\coding\\Iman.txt");

	// ПРомежуточне переменные:
	double I1;
	double I2;
	double Iman;

	//-------------------------------------------------------
	// Моделирование:
	for (int n = 0; n < NumOfSteps; n++)
	{
		// Генерирование основного сигнала:
		//GEN_Ia.sin_gen(45 , 0.00, 1);
		//GEN_Ib.sin_gen(50 , +120, 1);
		//GEN_Ic.sin_gen(45 , -120, 0.1);

		//GEN_Ia.sin_exp_gen(45, 0.00, 0.1, 0.04, 0.12 , 19 , 180 , -1);
		//GEN_Ib.sin_exp_gen(45, +120, 0.1, 0.1, 0.12 , 19 , 0 , 1);
		//GEN_Ic.sin_exp_gen(50, -120, 1, 0.1, 0.04 , 19 , 0);

		// генерим синусы по методике ПАО ФСК ЕЭС:
		if ( n < 2 ) 
		{
			GEN_Ia.sin_gen(50, 0.00, 0.00);
			GEN_Ib.sin_gen(50, 0.00, 1.00);
			GEN_Ic.sin_gen(50, 180, 1.00);
		}
		else
		{
			GEN_Ia.sin_gen(50, 0.00, 0.00);
			GEN_Ib.sin_gen(50, 0.00, 0.00);
			GEN_Ic.sin_gen(50, 180, 0.00);
		}

		// Эмуляция прохода по сигналу фильтом:
		for (int cnt1 = 0; cnt1 < HBuffSize; cnt1++)
		{

			// запись в файл синусоид фаз А, В и С:
			file_out1 << GEN_Ia.m_buff[cnt1] << "\n";
			file_out2 << GEN_Ib.m_buff[cnt1] << "\n";
			file_out3 << GEN_Ic.m_buff[cnt1] << "\n";

			// выполнение ДПФ синусоид фаз А , В , С:
			DFT_A.filt(&GEN_Ia.m_buff[cnt1]);
			DFT_B.filt(&GEN_Ib.m_buff[cnt1]);
			DFT_C.filt(&GEN_Ic.m_buff[cnt1]);

			// запись результов в файлы:
			file_out4 << DFT_A.m_Re << "\n";
			file_out5 << DFT_A.m_Im << "\n";
			file_out6 << DFT_B.m_Re << "\n";
			file_out7 << DFT_B.m_Im << "\n";
			file_out8 << DFT_C.m_Re << "\n";
			file_out9 << DFT_C.m_Im << "\n";

			// выполнение расчета тока прямой последовательности:
			CPLX_OPERATIONS1.mul(DFT_B.m_Re, DFT_B.m_Im, cos(+120 * PI0 / 180), sin(+120 * PI0 / 180));
			CPLX_OPERATIONS2.mul(DFT_C.m_Re, DFT_C.m_Im, cos(-120 * PI0 / 180), sin(-120 * PI0 / 180));
			CPLX_OPERATIONS3.add(CPLX_OPERATIONS1.m_Re, CPLX_OPERATIONS1.m_Im, CPLX_OPERATIONS2.m_Re, CPLX_OPERATIONS2.m_Im);
			CPLX_OPERATIONS3.add(DFT_A.m_Re, DFT_A.m_Im, CPLX_OPERATIONS3.m_Re, CPLX_OPERATIONS3.m_Im);
			CPLX_OPERATIONS3.m_Re = CPLX_OPERATIONS3.m_Re / 3;
			CPLX_OPERATIONS3.m_Im = CPLX_OPERATIONS3.m_Im / 3;

			I1 = CPLX_OPERATIONS3.m_Re;

			// запись результатов в файлы:
			file_out10 << CPLX_OPERATIONS3.m_Re << "\n";
			file_out11 << CPLX_OPERATIONS3.m_Im << "\n";

			// выполнение расчета тока обратной последовательности:
			CPLX_OPERATIONS1.mul(DFT_B.m_Re, DFT_B.m_Im, cos(-120 * PI0 / 180), sin(-120 * PI0 / 180));
			CPLX_OPERATIONS2.mul(DFT_C.m_Re, DFT_C.m_Im, cos(+120 * PI0 / 180), sin(+120 * PI0 / 180));
			CPLX_OPERATIONS3.add(CPLX_OPERATIONS1.m_Re, CPLX_OPERATIONS1.m_Im, CPLX_OPERATIONS2.m_Re, CPLX_OPERATIONS2.m_Im);
			CPLX_OPERATIONS3.add(DFT_A.m_Re, DFT_A.m_Im, CPLX_OPERATIONS3.m_Re, CPLX_OPERATIONS3.m_Im);
			CPLX_OPERATIONS3.m_Re = CPLX_OPERATIONS3.m_Re / 3;
			CPLX_OPERATIONS3.m_Im = CPLX_OPERATIONS3.m_Im / 3;

			I2 = CPLX_OPERATIONS3.m_Re;

			// запись результатов в файлы:
			file_out12 << CPLX_OPERATIONS3.m_Re << "\n";
			file_out13 << CPLX_OPERATIONS3.m_Im << "\n";

			// расчет тока манипуляции:
			Iman = I1 + I2 / 5;

			// запись результата в файл:
			file_out14 << Iman << "\n";
		}
	}

	return 0;
}

// Орган манипуляции ДФЗ, сделанный в соответствии с методикой компании ABB:

int test11()
{
	int HBuffSize = 20;
	int NumOfSteps = 8;

	// КИХ-фильтры фазных токов:
	FIR_filt FIR_A;
	FIR_filt FIR_B;
	FIR_filt FIR_C;

	FIR_A.LP_Init(4000, 50, 50, 80 , true);
	FIR_B.LP_Init(4000, 50, 50, 80 , true);
	FIR_C.LP_Init(4000, 50, 50, 80 , true);

	FIR_A.m_WIND_FCN.Chebyshev(60);
	FIR_B.m_WIND_FCN.Chebyshev(60);
	FIR_C.m_WIND_FCN.Chebyshev(60);

	FIR_A.allocate();
	FIR_B.allocate();
	FIR_C.allocate();


	//-------------------------------------------------------
	// Создаем блок преобразования Кларка:
	orto_transformations_3ph AB0_DQ0;

	//-------------------------------------------------------
	// Буфферы:
	mirror_ring_buff BUFF_ALPHA;
	mirror_ring_buff BUFF_BETTA;
	BUFF_ALPHA.BuffInit(2);
	BUFF_BETTA.BuffInit(2);
	BUFF_ALPHA.allocate(true);
	BUFF_BETTA.allocate(true);

	//-------------------------------------------------------
	// Генераторы сигналов:
	signal_gen GEN_Ia;
	signal_gen GEN_Ib;
	signal_gen GEN_Ic;

	// инициализация:
	GEN_Ia.GenInit(4000, HBuffSize);
	GEN_Ib.GenInit(4000, HBuffSize);
	GEN_Ic.GenInit(4000, HBuffSize);

	// выделение памяти:
	GEN_Ia.allocate();
	GEN_Ib.allocate();
	GEN_Ic.allocate();

	//-------------------------------------------------------
	// Потоки для записи в файл
	std::ofstream file_out1;
	std::ofstream file_out2;
	std::ofstream file_out3;
	std::ofstream file_out4;
	std::ofstream file_out5;
	std::ofstream file_out6;
	std::ofstream file_out7;
	std::ofstream file_out8;

	// инициализация:
	file_out1.open("E:\\coding\\Ia.txt");
	file_out2.open("E:\\coding\\Ib.txt");
	file_out3.open("E:\\coding\\Ic.txt");
	file_out4.open("E:\\coding\\Ialpha.txt");
	file_out5.open("E:\\coding\\Ibetta.txt");
	file_out6.open("E:\\coding\\I1.txt");
	file_out7.open("E:\\coding\\I2.txt");
	file_out8.open("E:\\coding\\Iman.txt");

	// ПРомежуточне переменные:
	double I1;
	double I2;
	double Iman;

	//-------------------------------------------------------
	// Моделирование:
	for (int n = 0; n < NumOfSteps; n++)
	{
		// Генерирование основного сигнала:
		/*GEN_Ia.sin_gen(50 , 0.00, 1);
		GEN_Ib.sin_gen(50 , +120, 1);
		GEN_Ic.sin_gen(50 , -120, 1);*/

		// генерим синусы по методике ПАО ФСК ЕЭС:
		if (n < 2)
		{
			GEN_Ia.sin_gen(50, 0.00, 0.00);
			GEN_Ib.sin_gen(50, 0.00, 1.00);
			GEN_Ic.sin_gen(50, 180, 1.00);
		}
		else
		{
			GEN_Ia.sin_gen(50, 0.00, 0.00);
			GEN_Ib.sin_gen(50, 0.00, 0.00);
			GEN_Ic.sin_gen(50, 180, 0.00);
		}

		// Эмуляция прохода по сигналу фильтом:
		for (int cnt1 = 0; cnt1 < HBuffSize; cnt1++)
		{
			FIR_A.filt(&GEN_Ia.m_buff[cnt1]);
			FIR_B.filt(&GEN_Ib.m_buff[cnt1]);
			FIR_C.filt(&GEN_Ic.m_buff[cnt1]);

			// Компенсация АЧХ КИХ фильтра на 50 Гц:
			FIR_A.m_out = FIR_A.m_out / FIR_A.m_Km;
			FIR_B.m_out = FIR_B.m_out / FIR_B.m_Km;
			FIR_C.m_out = FIR_C.m_out / FIR_C.m_Km;

			// запись в файл синусоид фаз А, В и С:
			file_out1 << FIR_A.m_out << "\n";
			file_out2 << FIR_B.m_out << "\n";
			file_out3 << FIR_C.m_out << "\n";

			// Выполняем преобразование Кларка:
			AB0_DQ0.Clark( &FIR_A.m_out , &FIR_B.m_out, &FIR_C.m_out);
			BUFF_ALPHA.fill_buff(&AB0_DQ0.m_A);
			BUFF_BETTA.fill_buff(&AB0_DQ0.m_B);

			// Запись результата в файл:
			file_out4 << AB0_DQ0.m_A << "\n";
			file_out5 << AB0_DQ0.m_B << "\n";

			// Расчет тока прямой последовательности:
			I1 = (BUFF_ALPHA.m_ptr_fill_down[0] + BUFF_ALPHA.m_ptr_fill_down[1])*0.25 + 3 / PI0 * (BUFF_BETTA.m_ptr_fill_down[0] - BUFF_BETTA.m_ptr_fill_down[1]);
			I2 = (BUFF_ALPHA.m_ptr_fill_down[0] + BUFF_ALPHA.m_ptr_fill_down[1])*0.25 - 3 / PI0 * (BUFF_BETTA.m_ptr_fill_down[0] - BUFF_BETTA.m_ptr_fill_down[1]);

			// Запись результата в файл:
			file_out6 << I1 << "\n";
			file_out7 << I2 << "\n";

			// расчет тока манипуляции:
			Iman = -I1*1.5 + I2;

			// запись результата в файл:
			file_out8 << Iman << "\n";
		}
	}

	return 0;
}

// Орган манипуляции ДФЗ, сделанный с использованием фазосдвигающего звена:

int test12() 
{
	int HBuffSize = 20;
	int NumOfSteps = 8;

	//-------------------------------------------------------
	// Буфферы:
	mirror_ring_buff BUFF_120_0;
	mirror_ring_buff BUFF_240_0;
	mirror_ring_buff BUFF_120_1;
	mirror_ring_buff BUFF_240_1;

	double Fn = 50;
	double Fs = 4000;
	double Buffsize = Fs / Fn * 120 / 360;

	BUFF_120_0.BuffInit(Buffsize);
	BUFF_240_0.BuffInit(2 * Buffsize);
	BUFF_120_1.BuffInit(Buffsize);
	BUFF_240_1.BuffInit(2 * Buffsize);
	BUFF_120_0.allocate(true);
	BUFF_240_0.allocate(true);
	BUFF_120_1.allocate(true);
	BUFF_240_1.allocate(true);

	//-------------------------------------------------------
	// Генераторы сигналов:
	signal_gen GEN_Ia;
	signal_gen GEN_Ib;
	signal_gen GEN_Ic;

	// инициализация:
	GEN_Ia.GenInit(4000, HBuffSize);
	GEN_Ib.GenInit(4000, HBuffSize);
	GEN_Ic.GenInit(4000, HBuffSize);

	// выделение памяти:
	GEN_Ia.allocate();
	GEN_Ib.allocate();
	GEN_Ic.allocate();

	//-------------------------------------------------------
	// Потоки для записи в файл
	std::ofstream file_out1;
	std::ofstream file_out2;
	std::ofstream file_out3;
	std::ofstream file_out4;
	std::ofstream file_out5;
	std::ofstream file_out6;
	std::ofstream file_out7;
	std::ofstream file_out8;

	// инициализация:
	file_out1.open("E:\\coding\\Ia.txt");
	file_out2.open("E:\\coding\\Ib.txt");
	file_out3.open("E:\\coding\\Ic.txt");
	file_out6.open("E:\\coding\\I1.txt");
	file_out7.open("E:\\coding\\I2.txt");
	file_out8.open("E:\\coding\\Iman.txt");

	// ПРомежуточне переменные:
	double I1;
	double I2;
	double Iman;

	//-------------------------------------------------------
	// Моделирование:
	for (int n = 0; n < NumOfSteps; n++)
	{
		// Генерирование основного сигнала:
		/*GEN_Ia.sin_gen(50 , 0.00, 1);
		GEN_Ib.sin_gen(50 , +120, 1);
		GEN_Ic.sin_gen(50 , -120, 1);*/

		// генерим синусы по методике ПАО ФСК ЕЭС:
		if (n < 2)
		{
			GEN_Ia.sin_gen(50, 0.00, 0.00);
			GEN_Ib.sin_gen(50, 0.00, 1.00);
			GEN_Ic.sin_gen(50, 180, 1.00);
		}
		else
		{
			GEN_Ia.sin_gen(50, 0.00, 0.00);
			GEN_Ib.sin_gen(50, 0.00, 0.00);
			GEN_Ic.sin_gen(50, 180, 0.00);
		}

		// Эмуляция прохода по сигналу фильтом:
		for (int cnt1 = 0; cnt1 < HBuffSize; cnt1++)
		{
			// запись в файл синусоид фаз А, В и С:
			file_out1 << GEN_Ia.m_buff[cnt1] << "\n";
			file_out2 << GEN_Ib.m_buff[cnt1] << "\n";
			file_out3 << GEN_Ic.m_buff[cnt1] << "\n";

			// Выполняем преобразование Кларка:
			BUFF_120_0.fill_buff(&GEN_Ib.m_buff[cnt1]);
			BUFF_240_0.fill_buff(&GEN_Ic.m_buff[cnt1]);
			BUFF_120_1.fill_buff(&GEN_Ic.m_buff[cnt1]);
			BUFF_240_1.fill_buff(&GEN_Ib.m_buff[cnt1]);

			// Расчет тока прямой последовательности:
			I1 = ( GEN_Ia.m_buff[cnt1] + BUFF_120_0.m_ptr_fill_down[int(Buffsize)] + BUFF_240_0.m_ptr_fill_down[int(2 * Buffsize)] ) / 3;
			I2 = ( GEN_Ia.m_buff[cnt1] + BUFF_120_1.m_ptr_fill_down[int(Buffsize)] + BUFF_240_1.m_ptr_fill_down[int(2 * Buffsize)] ) / 3;

			// Запись результата в файл:
			file_out6 << I1 << "\n";
			file_out7 << I2 << "\n";

			// расчет тока манипуляции:
			Iman = I1 / 5 + I2;

			// запись результата в файл:
			file_out8 << Iman << "\n";
		}
	}

	return 0;
}

// проверка работоспособности интегрирующих звеньев:
int test14()
{
    integrators int1;
    int1.int_Init(4000 , 50);

    //-------------------------------------------------------------------------------
    // ГЕНЕРАТОР СИГНАЛОВ:
    //-------------------------------------------------------------------------------
    // Генераторы сигналов:
    signal_gen GEN_MAIN;
    GEN_MAIN.GenInit(4000, 16);
    GEN_MAIN.allocate();

    //-------------------------------------------------------------------------------
    // ПОТОКИ ДЛЯ ЗАПИСИ В ФАЙЛ:
    //-------------------------------------------------------------------------------
    // Потоки для записи в файл
    std::ofstream file_out1;
    std::ofstream file_out2;
    std::ofstream file_out3;
    std::ofstream file_out4;
    std::ofstream file_out5;

    // потоки для записи в файл:
    file_out1.open("E:\\coding\\SIGNAL_NOT_FILT.txt");
    file_out2.open("E:\\coding\\SIGNAL_FILT.txt");
    file_out3.open("E:\\coding\\F.txt");
    file_out4.open("E:\\coding\\Km.txt");
    file_out5.open("E:\\coding\\pH.txt");

    for (int n = 0; n < 10; n++)
    {
        // Генерирование основного сигнала:
        GEN_MAIN.sin_gen(50, 0, 1);

        // Эмуляция прохода по сигналу фильтом:
        for (int cnt1 = 0; cnt1 < 16; cnt1++)
        {
            // запись в файл нефильтрованного сигнала:
            file_out1 << GEN_MAIN.m_buff[cnt1] << "\n";
            // фильтрация сигнала:
            int1.BackwardEuler(&GEN_MAIN.m_buff[cnt1]);
            // запись в файл отфильтрованного сигнала:
            file_out2 << int1.m_out << "\n";
        }
    }

    return 0;
}

// Запуск нужного примера:
int main()
{
  // test1();  // Пример работы с классом классических БИХ - фильтров
   test2();  // Пример работы с классом классических КИХ - фильтров
  // test3();  // Пример работы с классом простейших   БИХ - фильтров
  // test4();  // Пример работы с классом специальных математических функций
  // test5();  // Пример работы с классом трехфазных ортогональных преобразований
  // test6();  // Пример работы с классом однофазных ортогональных преобразований
  // test7();  // Пример работы с классом кольцевого буффера
  // test8();  // Пример работы с классом оконных функций
  // test9();  // Пример построения АЧХ и ФЧХ каскада цифровых фильтров
  // test10(); // орган манипуляции ДФЗ на ДПФ
  // test11(); // орган манипуляции ДФЗ по методике ABB
  // test12(); // орган манипуляции ДФЗ на фазосдвигающих звеньях

  return 0;
}
