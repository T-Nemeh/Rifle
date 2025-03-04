#ifndef RIFLE_HPP
#define RIFLE_HPP

#include <string>
#include "cadmium/modeling/devs/coupled.hpp"
#include "MagAssy.hpp"
#include "TrigAssy.hpp"
#include "BoltAssy.hpp"
#include "Chamber.hpp"

using namespace cadmium;



class Rifle : public Coupled {
public:
    Port<int> in_triggerPressed; 
    Port<int> in_firingSelector;
    Port<int> in_boltBack; 
    Port<int> in_magSeating;
    Port<int> in_bulletLoaded;
    Port<int> out_releaseBolt;
    Rifle(const std::string& id) 
        : Coupled(id)
    {
        in_triggerPressed = addInPort<int>("in_triggerPressed");
        in_firingSelector = addInPort<int>("in_firingSelector");
        in_boltBack = addInPort<int>("in_boltBack");
        in_magSeating = addInPort<int>("in_magSeating");
        in_bulletLoaded = addInPort<int>("in_bulletLoaded");
        out_releaseBolt = addOutPort<int>("out_releaseBolt");

        auto magAssy = addComponent<MagAssy>("MagAssy");
        auto trig    = addComponent<TrigAssy>("TA");
        auto bolt    = addComponent<BoltAssy>("BA");
        auto chamber = addComponent<Chamber>("Chbr");

        // Internal Couplings
        addCoupling(magAssy->out_bulletReady, bolt->in_bulletReady);
        addCoupling(magAssy->out_isDud, chamber->in_isDud);
        addCoupling(trig->out_releaseBolt, bolt->in_releaseBolt);
        addCoupling(bolt->out_bulletLoaded, chamber->in_bulletLoaded);
        addCoupling(bolt->out_bulletLoaded, magAssy->in_bulletLoaded);
        addCoupling(chamber->out_boltBack, bolt->in_boltBack);
        addCoupling(chamber->out_boltBack, trig->in_boltBack);

        // External Input Couplings 
        addCoupling(this->in_triggerPressed,trig->in_triggerPressed);
        addCoupling(this->in_firingSelector, trig->in_firingSelector);
        addCoupling(this->in_boltBack, bolt->in_boltBack);
        addCoupling(this->in_magSeating, magAssy->in_initMagSeating);
        addCoupling(this->in_bulletLoaded, magAssy->in_bulletLoaded);
    }
};

#endif // RIFLE_HPP
