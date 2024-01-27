#include "ComtradeAnalogChannel.h"

// constructors
ComtradeAnalogChannel::ComtradeAnalogChannel(
        std::string _Name,
        std::string _PhaseID,
        std::string _ControlledObjectID,
        std::string _MeasurementUnits,
        double      _CalibrationAdditionCoefficient,
        double      _CalibrationMultiplierCoefficient,
        double      _Skew,
        double      _PrimaryMultiplierCoefficient,
        double      _SecondaryMultiplierCoefficient,
        char        _PrimarySecondaryID) :
    ComtradeChannelBase(_Name, _PhaseID, _ControlledObjectID),
    m_MeasurementUnits(_MeasurementUnits),
    m_CalibrationAdditionCoefficient(_CalibrationAdditionCoefficient),
    m_CalibrationMultiplierCoefficient(_CalibrationMultiplierCoefficient),
    m_Skew(_Skew),
    m_PrimaryMultiplierCoefficient(_PrimaryMultiplierCoefficient),
    m_SecondaryMultiplierCoefficient(_SecondaryMultiplierCoefficient),
    m_PrimarySecondaryID(_PrimarySecondaryID){}

// virtual destructor
ComtradeAnalogChannel::~ComtradeAnalogChannel(){}

// getters
string ComtradeAnalogChannel::get_measurement_units() const
{
    return m_MeasurementUnits;
}

double ComtradeAnalogChannel::get_calibration_addition_coefficient() const
{
    return m_CalibrationAdditionCoefficient;
}

double ComtradeAnalogChannel::get_calibration_multiplier_coefficient() const
{
    return m_CalibrationMultiplierCoefficient;
}

double ComtradeAnalogChannel::get_channel_skew() const
{
    return m_Skew;
}

double ComtradeAnalogChannel::get_minimum_level() const
{
    return m_Data.empty() ? 0.0 : *std::min_element( m_Data.begin(), m_Data.end() );
}

double ComtradeAnalogChannel::get_maximum_level() const
{
    return m_Data.empty() ? 0.0 : *std::max_element( m_Data.begin(), m_Data.end() );
}

double ComtradeAnalogChannel::get_primary_multiplier_coefficient() const
{
    return m_PrimaryMultiplierCoefficient > 0 ? m_PrimaryMultiplierCoefficient : 1.0;
}

double ComtradeAnalogChannel::get_secondary_multiplier_coefficient() const
{
    return m_SecondaryMultiplierCoefficient > 0 ? m_SecondaryMultiplierCoefficient : 1.0;
}

char ComtradeAnalogChannel::get_primary_secondary_id() const
{
    return m_PrimarySecondaryID;
}

// setters
void ComtradeAnalogChannel::set_measurement_units(const string& _Value)
{
    m_MeasurementUnits = _Value;
}

void ComtradeAnalogChannel::set_calibration_addition_coefficient(double _Value)
{
    m_CalibrationAdditionCoefficient = _Value;
}

void ComtradeAnalogChannel::set_calibration_multiplier_coefficient(double _Value)
{
    m_CalibrationMultiplierCoefficient = _Value;
}

void ComtradeAnalogChannel::ComtradeAnalogChannel::set_time_delay(double _Value)
{
    m_Skew = _Value;
}

void ComtradeAnalogChannel::set_primary_multiplier_coefficient(double _Value)
{
    m_PrimaryMultiplierCoefficient = _Value;
}

void ComtradeAnalogChannel::set_secondary_multiplier_coefficient(double _Value)
{
    m_SecondaryMultiplierCoefficient = _Value;
}

void ComtradeAnalogChannel::set_primary_secondary_id(char _Value)
{
    m_PrimarySecondaryID = _Value;
}

// virtual methods override
void ComtradeAnalogChannel::from_string( std::string _Input )
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
        m_MeasurementUnits = __from_string__<std::string>( vector[number] );
    number++;

    if( vector.size() > number )
        m_CalibrationMultiplierCoefficient = __from_string__<double>( vector[number] );
    number++;

    if( vector.size() > number )
        m_CalibrationAdditionCoefficient = __from_string__<double>( vector[number] );
    number++;

    if( vector.size() > number )
        m_Skew = __from_string__<double>( vector[number] );
    number++;

    if( vector.size() > number )
        m_MinimumLevel = __from_string__<double>( vector[number] );
    number++;

    if( vector.size() > number )
        m_MaximumLevel = __from_string__<double>( vector[number] );
    number++;

    if( vector.size() > number )
        m_PrimaryMultiplierCoefficient = __from_string__<double>( vector[number] );
    number++;

    if( vector.size() > number )
        m_SecondaryMultiplierCoefficient = __from_string__<double>( vector[number] );
    number++;

    if( vector.size() > number )
        m_PrimarySecondaryID = __from_string__<char>( vector[number] );
    number++;
}

bool ComtradeAnalogChannel::is_precise( double _Value )
{
    return std::abs( std::abs(_Value) - (size_t)std::abs(_Value) ) == 0;
}

std::string ComtradeAnalogChannel::to_string()
{
    std::string output;

    output += std::to_string( this->get_number() ) + ",";
    output += this->get_name() + ",";
    output += this->get_phase_id() + ",";
    output += this->get_controlled_object_id() + ",";
    output += this->get_measurement_units() + ",";

    double value = this->get_calibration_multiplier_coefficient();
    if( is_precise( value ) )
        output += std::to_string( (int)value ) + ",";
    else
        output += std::to_string( value ) + ",";

    value = this->get_calibration_addition_coefficient();
    if( is_precise( value ) )
        output += std::to_string( (int)value ) + ",";
    else
        output += std::to_string( value ) + ",";

    value = this->get_channel_skew();
    if( is_precise( value ) )
        output += std::to_string( (int)value ) + ",";
    else
        output += std::to_string( value ) + ",";

    value = this->get_minimum_level();
    if( is_precise( value ) )
        output += std::to_string( (int)value ) + ",";
    else
        output += std::to_string( value ) + ",";

    value = this->get_maximum_level();
    if( is_precise( value ) )
        output += std::to_string( (int)value ) + ",";
    else
        output += std::to_string( value ) + ",";

    value = this->get_primary_multiplier_coefficient();
    if( is_precise( value ) )
        output += std::to_string( (int)value ) + ",";
    else
        output += std::to_string( value ) + ",";

    value = this->get_secondary_multiplier_coefficient();
    if( is_precise( value ) )
        output += std::to_string( (int)value ) + ",";
    else
        output += std::to_string( value ) + ",";

    output += this->get_primary_secondary_id();
    output += "\n";

    return output;
}
