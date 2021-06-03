// A.M. Tykvinsky, 29.09.2020
//-----------------------------------------------------------------------------------------------------------------
// КОЛЬЦЕВОЙ БУФФЕР
//-----------------------------------------------------------------------------------------------------------------

#ifndef _MIRROR_RING_BUF_X32_HPP
#define _MIRROR_RING_BUF_X32_HPP

#include <math.h>
#include <cfloat>
#include <iostream>

//КЛАСС "КОЛЬЦЕВОЙ БУФЕР"

class mirror_ring_buff
{

private:

	// Переменные - члены класса mirror_ring_buff:
	int    m_ringBuffSize;        // размер буффера
	int    m_ringBuffPos;        // положение ползунка в массиве буффера
	bool   m_ringBuffFilled;    // состояние буффера (заполнен или нет)
	bool   m_BuffType;         // тип буффера

	// Значения, которые может принимать переменная m_BuffType:
	// true  - зеркальный буффер ( двойной буффер для имитации стекового заполнения   )
	// false - обычный буффер    ( одинарный буффер без имитации стекового заполнения )

public:

	float *m_ptr_start_up;        // указатель на стартовый элемент ( с которого начинается заполнение ) верхней половины зеркального буффера 
	float *m_ptr_start_down;     // указатель на стартовый элемент ( с которого начинается заполнение ) нижней половины зеркального буффера 
	float *m_ptr_fill_up;       // указатель для записи данных в верхнюю половину зеркального буффера
	float *m_ptr_fill_down;    // указатель для записи данных в нижнюю половину зеркального буффера
	float *m_data;            // указатель на массив с данными
	float *m_buff;     	  // указатель на данные из буффера

	//Конструктор объекта класса mirror_ring_buff:
	mirror_ring_buff();

	// Метод выделения памяти под буффер заданного размера:
	void allocate( bool mode );

	// Метод освобождения памяти:
	void deallocate();

	//Деструктор объекта класса mirror_ring_buff:
	~mirror_ring_buff();

	//Инициализация буффера:
	int BuffInit(int BuffSize);	

	// Вывести значения массива буффера на экран:
	int ShowBuff();
	
	// Встроенные функции: 	
	//-----------------------------------------------------------------------------------------------------------------
	// Узнать размер буффера:
	inline int getBuffSize()
	{
		return m_ringBuffSize;
	}
	//-----------------------------------------------------------------------------------------------------------------
	//Узнать позицию буффера:
	inline int getBuffPos()
	{
		return m_ringBuffPos;
	}
	//-----------------------------------------------------------------------------------------------------------------
	//Узнать состояние буффера (заполнен или нет):
	inline bool getBuffState()
	{
		return m_ringBuffFilled;
	}
	//-----------------------------------------------------------------------------------------------------------------
	// Функция заполнения кольцевого буффера:
	int inline fill_buff(double *in, int framesPerCycle)
	{

		if (m_BuffType == true)
		{
			for (int n = 0; n < framesPerCycle; n++)
			{
				//пишем точку в буффер:
				*m_ptr_fill_up = *in++;
				*m_ptr_fill_down-- = *m_ptr_fill_up--;

				// Если вышли за пределы буффера, сбрасываем индекс
				if (--m_ringBuffPos <= 0)
				{
					m_ptr_fill_up   = m_ptr_start_up;
					m_ptr_fill_down = m_ptr_start_down;
					m_ringBuffPos   = m_ringBuffSize;
				}
			}
		}
		else 
		{
			for (int n = 0; n < framesPerCycle; n++)
			{
				m_buff[m_ringBuffPos++] = *in++;

				if (m_ringBuffPos >= m_ringBuffSize) 
				{
					m_ringBuffPos    = 0;
					m_ringBuffFilled = true;
				}
			}
		}

		return 0;
	}
	//-----------------------------------------------------------------------------------------------------------------
	// Функция заполнения кольцевого буффера:
	int inline fill_buff(double *in)
	{
		if (m_BuffType == true)
		{
			//пишем точку в буффер:
			*m_ptr_fill_up   = *in;
			*m_ptr_fill_down-- = *m_ptr_fill_up--;

			// Если вышли за пределы буффера, сбрасываем индекс
			if (--m_ringBuffPos <= 0)
			{
				m_ptr_fill_up   = m_ptr_start_up;
				m_ptr_fill_down = m_ptr_start_down;
				m_ringBuffPos   = m_ringBuffSize;
			}
		}
		else 
		{
			m_buff[m_ringBuffPos++] = *in;

			if (m_ringBuffPos >= m_ringBuffSize)
			{
				m_ringBuffPos    = 0;
				m_ringBuffFilled = true;
			}
		}

		return 0;
	}
	//-----------------------------------------------------------------------------------------------------------------
	// Функция заполнения кольцевого буффера:
	int inline fill_buff(float *in, int framesPerCycle)
	{

		if (m_BuffType == true)
		{
			for (int n = 0; n < framesPerCycle; n++)
			{
				//пишем точку в буффер:
				*m_ptr_fill_up = *in++;
				*m_ptr_fill_down-- = *m_ptr_fill_up--;

				// Если вышли за пределы буффера, сбрасываем индекс
				if (--m_ringBuffPos <= 0)
				{
					m_ptr_fill_up   = m_ptr_start_up;
					m_ptr_fill_down = m_ptr_start_down;
					m_ringBuffPos   = m_ringBuffSize;
				}
			}
		}
		else 
		{
			for (int n = 0; n < framesPerCycle; n++)
			{
				m_buff[m_ringBuffPos++] = *in++;

				if (m_ringBuffPos >= m_ringBuffSize) 
				{
					m_ringBuffPos    = 0;
					m_ringBuffFilled = true;
				}
			}
		}

		return 0;
	}
	//-----------------------------------------------------------------------------------------------------------------
	// Функция заполнения кольцевого буффера:
	int inline fill_buff(float *in)
	{
		if (m_BuffType == true)
		{
			//пишем точку в буффер:
			*m_ptr_fill_up   = *in;
			*m_ptr_fill_down-- = *m_ptr_fill_up--;

			// Если вышли за пределы буффера, сбрасываем индекс
			if (--m_ringBuffPos <= 0)
			{
				m_ptr_fill_up   = m_ptr_start_up;
				m_ptr_fill_down = m_ptr_start_down;
				m_ringBuffPos   = m_ringBuffSize;
			}
		}
		else 
		{
			m_buff[m_ringBuffPos++] = *in;

			if (m_ringBuffPos >= m_ringBuffSize)
			{
				m_ringBuffPos    = 0;
				m_ringBuffFilled = true;
			}
		}

		return 0;
	}
};

#endif


