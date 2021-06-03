#include "mirror_ring_buff_array.hpp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// КОЛЬЦЕВОЙ БУФЕР:
//-----------------------------------------------------------------------------
// инициализация буффера: 
int mirror_ring_buff_array::RingBuffArrayInit(int ArraySize, int ringBuffSize)
{

	m_ringBuffArraySize = ArraySize;
	m_ringBuffSize      = ringBuffSize;
	m_MBUFF = NULL;

	return 0;
}
//-----------------------------------------------------------------------------
// Деструктор:
mirror_ring_buff_array::~mirror_ring_buff_array() {}
//-----------------------------------------------------------------------------
// конструктор по умолчанию:
mirror_ring_buff_array::mirror_ring_buff_array() 
{
	m_ringBuffArraySize = 3;
	m_ringBuffSize      = 3;
	m_MBUFF             = NULL;
}
//-----------------------------------------------------------------------------
// Выделение памяти:
void mirror_ring_buff_array::allocate(bool mode)
{
	// mode         - тип одного кольцевого буффера:
	// mode = true  - зеркальный (двойной) кольцевой буффер с имитацией стекового заполнения 
	// mode = false - одинарный кольцевой  буффер без имитации стекового заполнения

	// выделенме памяти под массив из объектов типа ring_buff:
	m_MBUFF = new mirror_ring_buff[m_ringBuffArraySize];
	// Инициализация элементов массива объектов типа ring_buff с выделением памяти:
	for (int cnt = 0; cnt < m_ringBuffArraySize; cnt++)
	{
		m_MBUFF[cnt].BuffInit(m_ringBuffSize);
		m_MBUFF[cnt].allocate(mode);
	}
}
//-----------------------------------------------------------------------------
// Очистка памяти:
void mirror_ring_buff_array::deallocate()
{

	if (m_MBUFF != NULL) // исправление ошибки обращения в память на устройстве МКПА-РЗ
	{
		// Освобождение памяти из под элементов массива объектов типа ring_buff:
		for (int cnt = 0; cnt < m_ringBuffArraySize; cnt++) m_MBUFF[cnt].deallocate();
		// Удаление указателя на массив объектов типа ring_buff:
		delete[] m_MBUFF;
	}
}
//-----------------------------------------------------------------------------
// Функция получения размерности массива из кольцевых буфферов:
int mirror_ring_buff_array::getRingBuffArraySize()
{
	return m_ringBuffArraySize;
}
//-----------------------------------------------------------------------------
//Показать буффер:
void mirror_ring_buff_array::ShowBuffArray()
{

	if (m_MBUFF != NULL) // сначала убеждаемся в том, что массив существует в памяти!
	{
		for (int n = 0; n < m_ringBuffArraySize; n++)
		{
			for (int m = 0; m < m_ringBuffSize; m++)
			{
				std::cout << n + 1 << " :" << m_MBUFF[n].m_buff[m] << "\n";
			}
			std::cout << "\n";
		}
	}

}