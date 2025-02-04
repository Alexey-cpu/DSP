#include "Comtrade.h"

// constructors
Comtrade::Comtrade(
        std::string _StationName,
        double      _NominalFrequency,
        double      _RateFrequency,
        double      _OscillogramDuration,
        std::vector<ComtradeChannelBase*> _Channels ) :
    m_StationName(_StationName),
    m_NominalFrequency(_NominalFrequency),
    m_RateFrequency(_RateFrequency)
{
    this->set_samples_number( _OscillogramDuration * _RateFrequency );

    for( auto channel : _Channels )
    {
        if( channel == nullptr )
            continue;

        channel->resize( this->get_samples_number() );
        m_Channels.push_back( channel );
    }
}

// virtual destructor
Comtrade::~Comtrade()
{
    clear();
}

void Comtrade::clear()
{
    // remove channels
    for( auto channel : m_Channels )
    {
        if( channel != nullptr )
            delete channel;
    }

    this->set_samples_number(0);
}

// service methods
void Comtrade::write_cfg( std::string _Path )
{
    if( _Path.empty() )
        return;

    // get current time
    double oscDuration                      = this->get_oscillogram_duration();
    double oscDurationFloatingPointPart     = oscDuration - (time_t)oscDuration;
    time_t registratorTriggerTime           = chrono::system_clock::to_time_t( chrono::system_clock::now() );
    time_t registratorRecordFinishTime      = chrono::system_clock::to_time_t( chrono::system_clock::now() + chrono::microseconds( (size_t)(oscDuration*1e6) ) );
    tm*    registratorTriggerTimeStamp      = std::localtime(&registratorTriggerTime);
    tm*    registratorRecordFinishTimeStamp = std::localtime(&registratorRecordFinishTime);

    // open files
    std::ofstream cfg;
    cfg.open( _Path );

    // write station name, identification and revision year
    cfg << this->get_station_name() << ",";
    cfg << this->get_station_id() << ",";
    cfg << "1999\n";

    // write number and type of channels
    cfg << std::to_string( this->get_number_of_channels() ) << ",";
    cfg << std::to_string( this->get_number_of_analog_channels() ) << "A,";
    cfg << std::to_string( this->get_number_of_discrete_channels() ) << "D\n";

    // write analog channels information
    for( auto channel : this->get_analog_channels() )
    {
        if( channel != nullptr )
            cfg << channel->to_string();
    }

    // write discrete channels information
    for( auto channel : this->get_discrete_channels() )
    {
        if( channel != nullptr )
            cfg << channel->to_string();
    }

    // write line frequency
    cfg << std::to_string( this->get_nominal_frequency() ) << "\n";

    // write sampling rate information
    cfg << std::to_string( this->get_number_of_rate_frequencies() ) << "\n";
    cfg << std::to_string( (size_t)this->get_rate_frequency() ) << "," << std::to_string( ( this->get_samples_number() - 1 ) ) << "\n";

    double t1 =(double)registratorTriggerTimeStamp->tm_sec;

    cfg << registratorTriggerTimeStamp->tm_mday << "/"
        << ( registratorTriggerTimeStamp->tm_mon < 10 ? "0" + std::to_string( registratorTriggerTimeStamp->tm_mon ) : std::to_string( registratorTriggerTimeStamp->tm_mon ) ) << "/"
        << ( registratorTriggerTimeStamp->tm_year + 1900 ) << ","
        << ( registratorTriggerTimeStamp->tm_hour < 10 ? "0" + std::to_string( registratorTriggerTimeStamp->tm_hour ) : std::to_string( registratorTriggerTimeStamp->tm_hour ) ) << ":"
        << ( registratorTriggerTimeStamp->tm_min < 10 ? "0" + std::to_string( registratorTriggerTimeStamp->tm_min ) : std::to_string( registratorTriggerTimeStamp->tm_min ) ) << ":"
        << ( registratorTriggerTimeStamp->tm_sec < 10 ? "0" + std::to_string( t1 ) : std::to_string( t1 ) ) << "\n";

    double t2 =(double)registratorRecordFinishTimeStamp->tm_sec + oscDurationFloatingPointPart;

    cfg << registratorRecordFinishTimeStamp->tm_mday << "/"
        << ( registratorRecordFinishTimeStamp->tm_mon < 10 ? "0" + std::to_string( registratorRecordFinishTimeStamp->tm_mon ) : std::to_string( registratorRecordFinishTimeStamp->tm_mon ) ) << "/"
        << ( registratorRecordFinishTimeStamp->tm_year + 1900 ) << ","
        << ( registratorRecordFinishTimeStamp->tm_hour < 10 ? "0" + std::to_string( registratorRecordFinishTimeStamp->tm_hour ) : std::to_string( registratorRecordFinishTimeStamp->tm_hour ) ) << ":"
        << ( registratorRecordFinishTimeStamp->tm_min < 10 ? "0" + std::to_string( registratorRecordFinishTimeStamp->tm_min ) : std::to_string( registratorRecordFinishTimeStamp->tm_min ) ) << ":"
        << ( registratorTriggerTimeStamp->tm_sec < 10 ? "0" + std::to_string( t2 ) : std::to_string( t2 ) ) << "\n";

    // wrte the file type
    cfg << "ASCII" << "\n";

    // time stamp multiplication factor
    cfg << "1";

    // write dat
    cfg.close();
}

void Comtrade::write_dat( std::string _Path )
{
    if( _Path.empty() )
        return;

    // open files
    std::ofstream dat;
    dat.open( _Path );

    double dt    = 1.0 / this->get_rate_frequency();
    double value = 0.0;

    for( size_t i = 0 ; i < this->get_samples_number() ; i++ )
    {
        dat << i << "," << (long long)( i * dt * 1e6 ) << ",";

        for( size_t j = 0 ; j < m_Channels.size() - 1 ; j++ )
        {
            value = m_Channels[j]->get_sample(i);
            if( ComtradeAnalogChannel::is_precise( value ) )
                dat << std::fixed << std::to_string( (long long)value ) << ",";
            else
                dat << std::fixed << std::to_string( value ) << ",";
        }

        value = m_Channels[ m_Channels.size() - 1 ]->get_sample(i);

        if( ComtradeAnalogChannel::is_precise( value ) )
            dat << std::fixed << std::to_string( (long long)value ) << "\n";
        else
            dat << std::fixed << std::to_string( value ) << "\n";
    }

    // write dat
    dat.close();
}

bool Comtrade::parse_cfg( std::string _Data )
{
    if( _Data.empty() )
    {
        #ifdef DEBUGGER
        Debugger::LogError(
                    STRINGIFY(Comtrade),
                    STRINGIFY(parse_cfg),
                    "Input .cfg file format error: file is empty" );
        #endif

        return false;
    }

    // split file string
    std::vector<string> vector = __split__( _Data, "\n" );

    if( vector.size() <= 0 )
    {
        #ifdef DEBUGGER
        Debugger::Log(
                    STRINGIFY(Comtrade),
                    STRINGIFY(parse_cfg),
                    "Input .cfg file format error: file is empty" );
        #endif

        return false;
    }

    // parse string
    size_t number = 0;
    size_t m_NumberOfChannels       = 0;
    size_t m_AnalogChannelsNumber   = 0;
    size_t m_DiscreteChannelsNumber = 0;

    // parse station ID and station unique number
    if( vector.size() > number )
    {
        std::vector<string> data = __split__( vector[number], "," );

        size_t localNumber = 0;
        if( data.size() > localNumber )
            m_StationName = __from_string__< std::string >( data[localNumber] );
        localNumber++;

        if( data.size() > localNumber )
            m_StationID = __from_string__< std::string >( data[localNumber] );
        localNumber++;
    }
    number++;

    // parse the number of channels, number of analogue and discrete channels
    if( vector.size() > number )
    {
        std::vector<string> data = __split__( vector[number], "," );

        // skip parsing total channels number
        size_t localNumber = 0;
        if( data.size() > localNumber )
            m_NumberOfChannels = __from_string__< size_t >( data[localNumber] );
        localNumber++;

        // parses analogue channels number
        if( data.size() > localNumber )
            m_AnalogChannelsNumber = __from_string__<size_t>( __remove_symbol__( data[localNumber], 'A' ) );
        localNumber++;

        // parse logical channels number
        if( data.size() > localNumber )
            m_DiscreteChannelsNumber = __from_string__<size_t>( __remove_symbol__( data[localNumber], 'D' ) );
        localNumber++;

        // check retrieved data
        if( m_AnalogChannelsNumber + m_DiscreteChannelsNumber < m_NumberOfChannels )
        {
            #ifdef DEBUGGER
            string message = "Input .cfg file format error: number of analog/discrete signals mismatch: \n";
            message += std::to_string(m_NumberOfChannels) + "\n";
            message += std::to_string(m_AnalogChannelsNumber) + "\n";
            message += std::to_string(m_DiscreteChannelsNumber) + "\n";

            Debugger::LogError(
                        STRINGIFY(Comtrade),
                        STRINGIFY(parse_cfg),
                        message );
            #endif

            return false;
        }
    }
    else
    {
        #ifdef DEBUGGER
        Debugger::LogError(
                    STRINGIFY(Comtrade),
                    STRINGIFY(parse_cfg),
                    "Input .cfg file format error: file does not contain channel info \n" );
        #endif

        return false;
    }
    number++;

    // parse analog signals
    for( size_t i = 0 ; i < m_AnalogChannelsNumber ; i++ )
    {
        this->add_analog_channel()->from_string( vector[number] );
        number++;
    }

    // parse discrete signals
    for( size_t i = 0 ; i < m_DiscreteChannelsNumber ; i++ )
    {
        this->add_discrete_channel()->from_string( vector[number] );
        number++;
    }

    // parse reference frame frequency
    if( vector.size() > number )
    {
        m_NominalFrequency = __from_string__< double >( vector[number] );
    }
    else
    {
        #ifdef DEBUGGER
        Debugger::LogError(
                    STRINGIFY(Comtrade),
                    STRINGIFY(parse_cfg),
                    "Input .cfg file format error: file does not contain nominal frequency info \n" );
        #endif

        return false;
    }
    number++;

    // read number of rate frequencies in file
    if( vector.size() > number )
    {
        m_NumberOfRateFrequencies = __from_string__< size_t >( vector[number] );
    }
    else
    {
        #ifdef DEBUGGER
        Debugger::LogError(
                    STRINGIFY(Comtrade),
                    STRINGIFY(parse_cfg),
                    "Input .cfg file format error: file does not contain number of rate frequencies info \n" );
        #endif

        return false;
    }
    number++;

    // read
    if( vector.size() > number )
    {
        std::vector<string> data = __split__( vector[number], "," );

        size_t localNumber = 0;
        if( data.size() > localNumber )
            m_RateFrequency = __from_string__< double >( data[localNumber] );
        localNumber++;

        if( data.size() > localNumber )
            this->set_samples_number( __from_string__< size_t >( data[localNumber] ) + 1 );
        localNumber++;
    }
    else
    {
        #ifdef DEBUGGER
        Debugger::LogError(
                    STRINGIFY(Comtrade),
                    STRINGIFY(parse_cfg),
                    "Input .cfg file format error: file does not contain rate frequency info \n" );
        #endif

        return false;
    }
    number++;

    return true;
}

bool Comtrade::parse_dat( std::string _Data )
{
    if( _Data.empty() )
    {
        Debugger::Log( STRINGIFY(Comtrade), STRINGIFY(parse_dat), "_Input .dat file error: file is empty" );
        return false;
    }

    // allocate COMTRADE file row string buffer
    string row;
    row.reserve(256);

    // allocate COMTRADE file column string buffer
    string column;
    column.reserve(256);

    int rowNumber = 0;

    for( size_t i = 0 ; i < _Data.size() ; i++ )
    {
        if( _Data[i] != '\n' )
        {
            row.push_back( _Data[i] );
        }
        else
        {
            row.push_back( _Data[i] );

            int columnNumber = -2; // skip sample number and time colum

            for( size_t j = 0 ; j < row.size() ; j++ )
            {
                if( row[j] != ',' && row[j] != '\n'  )
                {
                    column.push_back( row[j] );
                }
                else
                {
                    if( columnNumber >= (int)m_Channels.size() )
                    {
                        Debugger::Log( STRINGIFY(Comtrade), STRINGIFY(parse_dat), "_Input .dat file error: line " + std::to_string(rowNumber) + " contains more columns than channels" );
                        return false;
                    }

                    if( columnNumber >= 0 )
                    {
                        m_Channels[columnNumber]->set_sample( rowNumber, __from_string__<double>( column ) );
                    }

                    columnNumber++;
                    column.clear();
                }
            }

            if( columnNumber > 0 && columnNumber < (int)m_Channels.size()-1 )
            {
                Debugger::Log( STRINGIFY(Comtrade), STRINGIFY(parse_dat), "_Input .dat file error: line " + std::to_string(rowNumber) + " contains less columns than channels" );
                return false;
            }

            rowNumber++;
            row.clear();
        }
    }

    return true;
}

string Comtrade::read_file( std::string _Path )
{
    if( _Path.empty() )
    {
        return string();
    }

    string output;

    ifstream file;
    file.open( _Path );

    if( !file.is_open() )
    {
        return string();
    }

    while( true )
    {
        char s;
        file.get(s);

        if( file.eof() )
            break;

        output.push_back( s );
    }

    file.close();

    return output;
}

// getters
string Comtrade::get_station_name() const
{
    return m_StationName;
}

std::string Comtrade::get_station_id() const
{
    return m_StationID;
}

size_t Comtrade::get_number_of_channels() const
{
    return get_number_of_analog_channels() + get_number_of_discrete_channels();
}

size_t Comtrade::get_number_of_analog_channels() const
{
    return this->get_analog_channels().size();
}

size_t Comtrade::get_number_of_discrete_channels() const
{
    return this->get_discrete_channels().size();
}

double Comtrade::get_nominal_frequency() const
{
    return m_NominalFrequency;
}

size_t Comtrade::get_number_of_rate_frequencies() const
{
    return m_NumberOfRateFrequencies;
}

double Comtrade::get_rate_frequency() const
{
    return m_RateFrequency;
}

size_t Comtrade::get_samples_number() const
{
    return m_SamplesNumber;
}

double Comtrade::get_oscillogram_duration() const
{
    return (double)this->get_samples_number() / ( this->get_rate_frequency() > 0 ? this->get_rate_frequency() : 1.0 );
}

// setters
void Comtrade::set_station_name( std::string _Value )
{
    m_StationName = _Value;
}

void Comtrade::set_station_id( std::string _Value )
{
    m_StationID = _Value;
}

void Comtrade::set_nominal_frequency( double _Value )
{
    m_NominalFrequency = _Value;
}

void Comtrade::set_number_of_rate_frequencies( size_t _Value )
{
    m_NumberOfRateFrequencies = ( _Value > 0 ? _Value : 1 );
}

void Comtrade::set_rate_frequency( double _Value )
{
    m_RateFrequency = _Value;
}

void Comtrade::set_samples_number( size_t _Value )
{
    m_SamplesNumber = _Value;

    if( m_SamplesNumber <= 0 )
        return;

    for( auto channel : m_Channels )
    {
        if( channel != nullptr )
        {
            channel->resize( m_SamplesNumber );
        }
    }
}

// public methods
ComtradeAnalogChannel* Comtrade::add_analog_channel(
        std::string   _Name,
        std::string   _ChannelPhaseID,
        std::string   _ControlledObjectID,
        std::string   _ChannelMeasurementUnits,
        double        _CalibrationAdditionCoefficient,
        double        _CalibrationMultiplierCoefficient,
        double        _SignalTimeDelay,
        double        _SignalPrimaryMultiplierCoefficient,
        double        _SignalSecondaryMultiplierCoefficient,
        char          _SignalPrimarySecondaryID )
{
    // create channel
    ComtradeAnalogChannel* channel = new ComtradeAnalogChannel();

    // setup channel attributes
    channel->set_name( _Name );
    channel->set_phase_id( _ChannelPhaseID );
    channel->set_controlled_object_id( _ControlledObjectID );
    channel->set_measurement_units( _ChannelMeasurementUnits );
    channel->set_calibration_addition_coefficient( _CalibrationAdditionCoefficient );
    channel->set_calibration_multiplier_coefficient( _CalibrationMultiplierCoefficient );
    channel->set_time_delay( _SignalTimeDelay );
    channel->set_primary_multiplier_coefficient( _SignalPrimaryMultiplierCoefficient );
    channel->set_secondary_multiplier_coefficient( _SignalSecondaryMultiplierCoefficient );
    channel->set_primary_secondary_id( _SignalPrimarySecondaryID );

    // resize channel
    channel->resize( this->get_samples_number() );

    // save channel in the object
    this->m_Channels.push_back(channel);

    return channel;
}

ComtradeDiscreteChannel* Comtrade::add_discrete_channel(
        std::string   _Name,
        std::string   _ChannelPhaseID,
        std::string   _ControlledObjectID,
        double        _ChannelNormalState )
{
    // create channel
    ComtradeDiscreteChannel* channel = new ComtradeDiscreteChannel();

    // setup channel attributes
    channel->set_name( _Name );
    channel->set_phase_id( _ChannelPhaseID );
    channel->set_controlled_object_id( _ControlledObjectID );
    channel->set_normal_state(_ChannelNormalState);

    // resize channel
    channel->resize( this->get_samples_number() );

    // save channel in the object
    this->m_Channels.push_back(channel);

    return channel;
}

void Comtrade::remove_channel( std::string _Name )
{
    for( auto i = m_Channels.begin() ; i != m_Channels.end() ; i++ )
    {
        ComtradeChannelBase* channel =
                dynamic_cast<ComtradeChannelBase*>(*i);

        if( channel->get_name() == _Name )
        {
            this->m_Channels.erase(i);
            return;
        }
    }
}

void Comtrade::remove_channel( std::function<bool(ComtradeChannelBase*_Channel)> _Predicate )
{
    for( auto i = m_Channels.begin() ; i != m_Channels.end() ; i++ )
    {
        ComtradeChannelBase* channel =
                dynamic_cast<ComtradeChannelBase*>(*i);

        if( _Predicate( channel ) )
        {
            this->m_Channels.erase(i);
            return;
        }
    }
}

ComtradeChannelBase* Comtrade::find_channel( std::function<bool(ComtradeChannelBase*_Channel)> _Predicate ) const
{
    if ( _Predicate == nullptr )
        return nullptr;

    // look among analog channels
    for( auto channel : this->get_analog_channels() )
    {
        if( _Predicate( channel ) )
            return channel;
    }

    // look among discrete channels
    for( auto channel : this->get_discrete_channels() )
    {
        if( _Predicate( channel ) )
            return channel;
    }

    return nullptr;
}

ComtradeAnalogChannel* Comtrade::find_analog_channel( std::string _Name ) const
{
    ComtradeChannelBase* channel =
    this->find_channel
            (
                [_Name](ComtradeChannelBase* _Channel)->bool
                {
                    ComtradeAnalogChannel* channel = dynamic_cast<ComtradeAnalogChannel*>(_Channel);
                    return channel != nullptr && channel->get_name() == _Name;
                }
            );

    return dynamic_cast<ComtradeAnalogChannel*>(channel);
}

ComtradeDiscreteChannel* Comtrade::find_discrete_channel( std::string _Name ) const
{
    ComtradeChannelBase* channel =
    this->find_channel
            (
                [_Name](ComtradeChannelBase* _Channel)->bool
                {
                    ComtradeDiscreteChannel* channel = dynamic_cast<ComtradeDiscreteChannel*>(_Channel);
                    return channel != nullptr && channel->get_name() == _Name;
                }
            );

    return dynamic_cast<ComtradeDiscreteChannel*>(channel);
}

void Comtrade::from_file( std::string _Directory, std::string _FileName )
{
    if( _Directory.empty() || _FileName.empty() )
        return;

    if( !this->parse_cfg( read_file( _Directory + "/" + _FileName + ".cfg" ) ) )
    {
        this->clear();
        return;
    }

    if( !this->parse_dat( read_file( _Directory + "/" + _FileName + ".dat" ) ) )
    {
        this->clear();
        return;
    }
}

void Comtrade::to_file( std::string _Directory, std::string _FileName )
{
    if( _Directory.empty() || _FileName.empty() )
        return;

    this->write_cfg( _Directory + "/" + _FileName + ".cfg" );
    this->write_dat( _Directory + "/" + _FileName + ".dat" );
}

void Comtrade::print_station_info()
{
    printf("%s %s \n", this->get_station_name().c_str(), this->get_station_id().c_str() );
    printf("ADC rate frequency       : %f Hz\n", this->get_rate_frequency() );
    printf("Network frequency        : %f Hz\n", this->get_nominal_frequency() );
    printf("analog channels number   : %i \n", (int)this->get_number_of_analog_channels() );
    printf("discrete channels number : %i \n\n", (int)this->get_number_of_discrete_channels() );
}

std::vector< ComtradeAnalogChannel* > Comtrade::get_analog_channels() const
{
    // retrieve analog channels
    std::vector< ComtradeAnalogChannel* > output;

    for( auto channel : this->m_Channels )
    {
        ComtradeAnalogChannel* instance = dynamic_cast<ComtradeAnalogChannel*>(channel);

        if( instance == nullptr )
            continue;

        instance->set_number( output.size() );
        output.push_back(instance);
    }

    return output;
}

std::vector< ComtradeDiscreteChannel* > Comtrade::get_discrete_channels() const
{
    // retrieve analog channels
    std::vector< ComtradeDiscreteChannel* > output;

    for( auto channel : this->m_Channels )
    {
        ComtradeDiscreteChannel* instance = dynamic_cast<ComtradeDiscreteChannel*>(channel);

        if( instance == nullptr )
            continue;

        instance->set_number( output.size() );
        output.push_back(instance);
    }

    return output;
}

double Comtrade::compute_oscillogram_duration( int _NumberOfSamples, double _RateFrequency )
{
    return __abs__( _RateFrequency ) > 0 ?
                (double)_NumberOfSamples / __abs__( _RateFrequency ) :
                0.0;
}
