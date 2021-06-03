// #pragma once

#ifndef _MIRROR_RING_BUF_ARRAY_HPP
#define _MIRROR_RING_BUF_ARRAY_HPP

#include "mirror_ring_buff_x32.hpp"

//КЛАСС "МАССИВ ИЗ КОЛЬЦЕВЫХ БУФЕРОВ"

class mirror_ring_buff_array
{

private:

	int  m_ringBuffArraySize;  // к - во кольцевых буфферов
	int  m_ringBuffSize;      // размер одного кольцевого буффера

	//Надо задать модификатор доступа public, т.к. по умолчанию он private:
public:

	mirror_ring_buff *m_MBUFF;        // буффер

	//Конструктор объекта класса ring_buff:
	mirror_ring_buff_array();

	//Деструктор объекта класса ring_buff:
	~mirror_ring_buff_array();

	// функция инициализации динамического объекта:
	int RingBuffArrayInit(int ArraySize , int ringBuffSize);

	// Метод выделения памяти под буффер заданного размера:
	void allocate(bool mode);

	// Метод освобождения памяти:
	void deallocate();

	// функция получения размерности массива из кольцевых буфферов:
	int getRingBuffArraySize();

	// Функция отображения элементов массива m_BUFF:
	void ShowBuffArray();

};

#endif 


