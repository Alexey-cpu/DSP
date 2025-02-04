#include "ComtradeChannelBase.h"

// constructors
ComtradeChannelBase::ComtradeChannelBase(
        std::string _Name,
        std::string _ChannelPhaseID,
        std::string _ControlledObjectID) :
    m_Name(_Name),
    m_PhaseID(_ChannelPhaseID),
    m_ControlledObjectID(_ControlledObjectID){}

// virtual destructor
ComtradeChannelBase::~ComtradeChannelBase(){}

// getters
std::string ComtradeChannelBase::get_name() const
{
    return m_Name;
}

size_t ComtradeChannelBase::get_number() const
{
    return m_Number;
}

string ComtradeChannelBase::get_phase_id() const
{
    return m_PhaseID;
}

string ComtradeChannelBase::get_controlled_object_id() const
{
    return m_ControlledObjectID;
}

double ComtradeChannelBase::get_sample( size_t _N ) const
{
    return _N < m_Data.size() ? m_Data[_N] : 0.0;
}

// setters
void ComtradeChannelBase::set_name( std::string _Name )
{
    m_Name = _Name;
}

void ComtradeChannelBase::set_number(size_t newChannelNumber)
{
    m_Number = newChannelNumber;
}

void ComtradeChannelBase::set_phase_id(const string &newChannelPhaseID)
{
    m_PhaseID = newChannelPhaseID;
}

void ComtradeChannelBase::set_controlled_object_id(const string &newControlledObjectID)
{
    m_ControlledObjectID = newControlledObjectID;
}

void ComtradeChannelBase::set_sample( size_t _N, double _Value )
{
    if( _N < m_Data.size() )
        m_Data[_N] = _Value;
}

void ComtradeChannelBase::resize( size_t _Size )
{
    m_Data.resize( _Size );
}
