//------------------------------------------------------------------------
// A.M. Tykvinsky, 19.03.2021
//------------------------------------------------------------------------
// Интеграторы
//------------------------------------------------------------------------

class integrators
{

private:

	double m_Fs; // частота дискретизации АЦП
	double m_Fn; // номинальная частота сети
	double m_Ts; // период дискретизации АЦП
		
	double m_prev_input;
	double m_prev_output;
	
public:
	
	// выход фильтра:
	double m_out;
	
	// функции интегрирования:
	//----------------------------------------------------------------
	// Прямой метод Эйлера
	inline int ForwardEuler( double *input )
	{
		m_out  = m_out + m_prev_input * m_Ts;
		m_prev_input = *input;
		return 0;
	}
	//----------------------------------------------------------------
	// Обратный метод Эйлера
	inline int BackwardEuler( double *input )
	{
		m_prev_output = m_prev_output + m_prev_input * m_Ts;
		m_prev_input  = *input;
		m_out  = m_prev_output + (*input) * m_Ts;
		return 0;
	}
	//----------------------------------------------------------------
	// Метод трапеций
	inline int Trapezoidal( double *input )
	{
		m_out  = ( m_prev_input + *input ) * 0.5 * m_Ts + m_out;
		m_prev_input = *input;
		return 0;
	}
	//----------------------------------------------------------------
	
	// констурктор по умолчанию:
	integrators();
	
	// деструктор:
	~integrators();
	
	// функция инициализации:
	int int_Init( double m_Fs , double m_Fn );
};
