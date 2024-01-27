#ifndef COMTRADECHANNELBASE_H
#define COMTRADECHANNELBASE_H

// STL includes
#include <string>
#include <vector>
using namespace std;

// custom external includes
#include "Utils.h"
using namespace STRING_EXTENSION;

// Abstrct channel channel
class ComtradeChannelBase
{
protected:

    // info
    std::string         m_Name               = std::string();
    size_t              m_Number             = 0;
    std::string         m_PhaseID            = std::string();
    std::string         m_ControlledObjectID = std::string();
    std::vector<double> m_Data;

public:

    // constructors
    ComtradeChannelBase(
            std::string _Name               = std::string(),
            std::string _ChannelPhaseID     = std::string(),
            std::string _ControlledObjectID = std::string());

    // virtual destructor
    virtual ~ComtradeChannelBase();

    // getters
    std::string get_name() const;
    size_t      get_number() const;
    string      get_phase_id() const;
    string      get_controlled_object_id() const;
    double      get_sample( size_t _N ) const;

    // setters
    void set_name( std::string _Name );
    void set_number(size_t newChannelNumber);
    void set_phase_id(const string &newChannelPhaseID);
    void set_controlled_object_id(const string &newControlledObjectID);
    void set_sample( size_t _N, double _Value );

    // public methods
    void resize( size_t _Size );

    // pure virtual methods
    virtual void from_string( std::string _Input ) = 0;
    virtual std::string to_string() = 0;
};

#endif // COMTRADECHANNELBASE_H
