// A.M. Tykvinsky, 29.09.2020
//-----------------------------------------------------------------------------------------------------------------
// КОЛЬЦЕВОЙ БУФФЕР
//-----------------------------------------------------------------------------------------------------------------

#include "mirror_ring_buff_x32.hpp"

//-----------------------------------------------------------------------------------------------------------------
// инициализация буффера: 
int mirror_ring_buff::BuffInit(int BuffSize)
{
	m_ringBuffSize = BuffSize;
	m_ringBuffPos  = 0;
	m_BuffType     = true;
	
	// Все указатели должны быть проинициализированы значением NULL
	// в противном случае алгоритм падает с ошибкой обращения в память...
	m_data           = NULL;
	m_buff           = NULL;
	m_ptr_fill_up    = NULL;
	m_ptr_fill_down  = NULL;
	m_ptr_start_down = NULL;
	m_ptr_start_up   = NULL;
	
	return 0;

}
//-----------------------------------------------------------------------------------------------------------------
// конструктор по умолчанию:
mirror_ring_buff::mirror_ring_buff()
{
	m_ringBuffSize = 16;
	m_ringBuffPos  = 0;
	m_BuffType	   = true;
	
	// Все указатели должны быть проинициализированы значением NULL
	// в противном случае алгоритм падает с ошибкой обращения в память...
	m_data           = NULL;
	m_buff           = NULL;
	m_ptr_fill_up    = NULL;
	m_ptr_fill_down  = NULL;
	m_ptr_start_down = NULL;
	m_ptr_start_up   = NULL;

}
//-----------------------------------------------------------------------------------------------------------------
// Выделение памяти:
void mirror_ring_buff::allocate(bool mode)
{
	m_BuffType = mode;

	if (mode == true) // создание зеркального( двойного ) кольцевого буффера с имитацией стекового заполнения
	{
		// выделение памяти с инициализацией начального положения ползунка кольцевого буффера:
		//m_data            = new double[2 * m_ringBuffSize];
		m_data            = new float[2 * m_ringBuffSize];
		m_ringBuffPos     = m_ringBuffSize;

		// инициализация указателей:
		m_buff            = &m_data[0];
		m_ptr_fill_up     = &m_data[2 * m_ringBuffSize - 1];
		m_ptr_fill_down   = &m_data[m_ringBuffSize - 1];
		m_ptr_start_down  = m_ptr_fill_down;
		m_ptr_start_up    = m_ptr_fill_up;

		//заполнение массива нулями:
		for (int n = 0; n < 2 * m_ringBuffSize; n++) m_data[n] = 0;
	}
	else // создание одинарного кольцевого буффера с имитацией стекового заполнения
	{
		// выделение памяти с инициализацией начального положения ползунка кольцевого буффера:
		//m_data			 = new double[m_ringBuffSize];
		m_data			 = new float[m_ringBuffSize];
		m_ringBuffPos    = 0;

		// инициализация указателей:
		m_buff           = &m_data[0];
		m_ptr_fill_up    = NULL;
		m_ptr_fill_down  = NULL;
		m_ptr_start_down = NULL;
		m_ptr_start_up   = NULL;

		//заполнение массива нулями:
		for (int n = 0; n < m_ringBuffSize; n++) m_data[n] = 0;
	}

}
//-----------------------------------------------------------------------------------------------------------------
// Очистка памяти:
void mirror_ring_buff::deallocate()
{
	// освобождение памяти:
	if( m_data != NULL )
	{
		delete[]    m_data;
		 
		// обнуление указателей:
		m_data           = NULL;
		m_buff           = NULL;
		m_ptr_fill_up    = NULL;
		m_ptr_fill_down  = NULL;
		m_ptr_start_down = NULL;
		m_ptr_start_up   = NULL; 
	} 
}
//-----------------------------------------------------------------------------------------------------------------
// Деструктор:
mirror_ring_buff::~mirror_ring_buff() {}
//-----------------------------------------------------------------------------------------------------------------
// Показать массив зеркального кольцевого буффера:
int mirror_ring_buff::ShowBuff() 
{
	if (m_buff != NULL && m_data != NULL)
	{
		if (m_BuffType == true)
		{
			for (int n = 0; n < 2 * m_ringBuffSize; n++)
			{
				std::cout << "buff[" << n << "]" << " = " << m_buff[n] << "\n";
			}
		}
		else
		{
			for (int n = 0; n < m_ringBuffSize; n++)
			{
				std::cout << "buff[" << n << "]" << " = " << m_buff[n] << "\n";
			}
		}

		std::cout << "\n" << "\n";
	}
	else 
	{
		std::cout << "buffer cannot be shown as it does not excist or it has been deleten..." << "\n";
	}

	return 0;
}
