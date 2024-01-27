#ifndef COMTRADE_H
#define COMTRADE_H

#include "ComtradeAnalogChannel.h"
#include "ComtradeDiscreteChannel.h"

class Comtrade
{
    // channels and their creators
    std::vector< ComtradeChannelBase* > m_Channels;

    // info
    string         m_StationName             = std::string();
    string         m_StationID               = std::string();
    double         m_NominalFrequency        = 50.0;
    size_t         m_NumberOfRateFrequencies = 1;
    double         m_RateFrequency           = 4000.0;
    size_t         m_SamplesNumber           = 0;

    // service methods
    void   write_cfg( std::string _Path );
    void   write_dat( std::string _Path );
    bool   parse_cfg( std::string _Data );
    bool   parse_dat( std::string _Data );
    string read_file( std::string _Path );
    void   clear();

public:

    // constructors
    Comtrade(
            std::string _StationName         = std::string(),
            double      _NominalFrequency    = 50.0,
            double      _RateFrequency       = 4000.0,
            double      _OscillogramDuration = 1.0,
            std::vector<ComtradeChannelBase*> _Channels = std::vector<ComtradeChannelBase*>() );

    // virtual destructor
    virtual ~Comtrade();

    // getters
    std::string get_station_name() const;
    std::string get_station_id() const;
    size_t      get_number_of_channels() const;
    size_t      get_number_of_analog_channels() const;
    size_t      get_number_of_discrete_channels() const;
    double      get_nominal_frequency() const;
    size_t      get_number_of_rate_frequencies() const;
    double      get_rate_frequency() const;
    size_t      get_samples_number() const;
    double      get_oscillogram_duration() const;

    std::vector< ComtradeAnalogChannel* > get_analog_channels() const;
    std::vector< ComtradeDiscreteChannel* > get_discrete_channels() const;
    std::vector< ComtradeChannelBase* > get_channels() const;

    // setters
    void set_station_name( std::string _Value );
    void set_station_id( std::string _Value );
    void set_nominal_frequency( double _Value );
    void set_number_of_rate_frequencies( size_t _Value );
    void set_rate_frequency( double _Value );
    void set_samples_number( size_t _Value );

    // public methods
    ComtradeAnalogChannel* add_analog_channel(
            std::string _Name                                 = std::string(),
            std::string _ChannelPhaseID                       = std::string(),
            std::string _ControlledObjectID                   = std::string(),
            std::string _ChannelMeasurementUnits              = std::string(),
            double      _CalibrationAdditionCoefficient       = 0.0,
            double      _CalibrationMultiplierCoefficient     = 1.0,
            double      _SignalTimeDelay                      = 0.0,
            double      _SignalPrimaryMultiplierCoefficient   = 1.0,
            double      _SignalSecondaryMultiplierCoefficient = 1.0,
            char        _SignalPrimarySecondaryID             = 'P' );

    ComtradeDiscreteChannel* add_discrete_channel(
            std::string _Name               = std::string(),
            std::string _ChannelPhaseID     = std::string(),
            std::string _ControlledObjectID = std::string(),
            double      _ChannelNormalState = 0.0 );

    void remove_channel( std::string _Name );
    void remove_channel( std::function<bool(ComtradeChannelBase*_Channel)> _Predicate );

    ComtradeChannelBase* find_channel( std::function<bool(ComtradeChannelBase*_Channel)> _Predicate ) const;
    ComtradeAnalogChannel* find_analog_channel( std::string _Name ) const;
    ComtradeDiscreteChannel* find_discrete_channel( std::string _Name ) const;
    void from_file( std::string _Directory, std::string _FileName );
    void to_file( std::string _Directory, std::string _FileName );

#ifdef DEBUGGER
    void print_station_info();
#endif

    // static API
    static double compute_oscillogram_duration( int _NumberOfSamples, double _RateFrequency );
};

#endif // COMTRADE_H
