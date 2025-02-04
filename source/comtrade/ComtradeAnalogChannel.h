#ifndef COMTRADEANALOGCHANNEL_H
#define COMTRADEANALOGCHANNEL_H

#include "ComtradeChannelBase.h"

class ComtradeAnalogChannel : public ComtradeChannelBase
{
protected:

    // info
    std::string    m_MeasurementUnits                 = std::string();
    double         m_CalibrationAdditionCoefficient   = 0.0;
    double         m_CalibrationMultiplierCoefficient = 1.0;
    double         m_Skew                             = 0.0;
    mutable double m_MinimumLevel                     = 0.0;
    mutable double m_MaximumLevel                     = 0.0;
    double         m_PrimaryMultiplierCoefficient     = 1.0;
    double         m_SecondaryMultiplierCoefficient   = 1.0;
    char           m_PrimarySecondaryID               = 'P';

public:

    // constructors
    ComtradeAnalogChannel(
            std::string _Name                             = std::string(),
            std::string _PhaseID                          = std::string(),
            std::string _ControlledObjectID               = std::string(),
            std::string _MeasurementUnits                 = std::string(),
            double      _CalibrationAdditionCoefficient   = 0.0,
            double      _CalibrationMultiplierCoefficient = 1.0,
            double      _Skew                             = 0.0,
            double      _PrimaryMultiplierCoefficient     = 1.0,
            double      _SecondaryMultiplierCoefficient   = 1.0,
            char        _PrimarySecondaryID               = 'P');

    // virtual destructor
    virtual ~ComtradeAnalogChannel();

    // getters
    string get_measurement_units() const;
    double get_calibration_addition_coefficient() const;
    double get_calibration_multiplier_coefficient() const;
    double get_channel_skew() const;
    double get_minimum_level() const;
    double get_maximum_level() const;
    double get_primary_multiplier_coefficient() const;
    double get_secondary_multiplier_coefficient() const;
    char   get_primary_secondary_id() const;

    // setters
    void set_measurement_units(const string& _Value);
    void set_calibration_addition_coefficient(double _Value);
    void set_calibration_multiplier_coefficient(double _Value);
    void set_time_delay(double _Value);
    void set_primary_multiplier_coefficient(double _Value);
    void set_secondary_multiplier_coefficient(double _Value);
    void set_primary_secondary_id(char _Value);

    // virtual methods override
    virtual void from_string( std::string _Input ) override;
    virtual std::string to_string() override;

    // static API
    static bool is_precise( double _Value );
};

#endif // COMTRADEANALOGCHANNEL_H
