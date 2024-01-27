#ifndef COMTRADEDISCRETECHANNEL_H
#define COMTRADEDISCRETECHANNEL_H

#include "ComtradeChannelBase.h"

class ComtradeDiscreteChannel : public ComtradeChannelBase
{
protected:

    // info
    double m_NormalState = 0.0;

public:

    // constructors
    ComtradeDiscreteChannel(
            std::string _Name               = std::string(),
            std::string _ChannelPhaseID     = std::string(),
            std::string _ControlledObjectID = std::string(),
            double      _ChannelNormalState = 0.0);

    // virtual destructor
    virtual ~ComtradeDiscreteChannel();

    // getters
    double get_normal_state() const;

    // setters
    void set_normal_state(double _Value);

    // virtual methods override
    virtual void from_string( std::string _Input ) override;
    virtual std::string to_string() override;
};

#endif // COMTRADEDISCRETECHANNEL_H
