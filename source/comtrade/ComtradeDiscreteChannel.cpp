#include "ComtradeDiscreteChannel.h"

// constructors
ComtradeDiscreteChannel::ComtradeDiscreteChannel(
        std::string _Name,
        std::string _PhaseID,
        std::string _ControlledObjectID,
        double      _NormalState) :
    ComtradeChannelBase(_Name, _PhaseID, _ControlledObjectID),
    m_NormalState(_NormalState){}

// virtual destructor
ComtradeDiscreteChannel::~ComtradeDiscreteChannel(){}

// getters
double ComtradeDiscreteChannel::get_normal_state() const
{
    return m_NormalState;
}

// setters
void ComtradeDiscreteChannel::set_normal_state(double _Value)
{
    m_NormalState = _Value;
}

// virtual methods override
void ComtradeDiscreteChannel::from_string( std::string _Input )
{
    if( _Input.empty() )
        return;

    // retrieve data separated by comma
    std::vector< string > vector = __split__( _Input, "," );

    // retrive parameters
    size_t number = 0;
    if( vector.size() > number )
        m_Number = __from_string__<size_t>( vector[number] );
    number++;

    if( vector.size() > number )
        m_Name = __from_string__<std::string>( vector[number] );
    number++;

    if( vector.size() > number )
        m_PhaseID = __from_string__<std::string>( vector[number] );
    number++;

    if( vector.size() > number )
        m_ControlledObjectID = __from_string__<std::string>( vector[number] );
    number++;

    if( vector.size() > number )
        m_NormalState = __from_string__<double>( vector[number] );
    number++;
}

std::string ComtradeDiscreteChannel::to_string()
{
    std::string output;

    output += std::to_string( this->get_number() ) + ",";
    output += this->get_name() + ",";
    output += this->get_phase_id() + ",";
    output += this->get_controlled_object_id() + ",";
    output += std::to_string( this->get_normal_state() );
    output += "\n";

    return output;
}
