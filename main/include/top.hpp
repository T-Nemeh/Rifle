#ifndef SAMPLE_TOP_HPP
#define SAMPLE_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "RifleQueueGenerator.hpp"
#include "Rifle.hpp"


using namespace cadmium;

struct top_coupled : public Coupled {

    /**
     * Constructor function for the blinkySystem model.
     * @param id ID of the blinkySystem model.
     */
    top_coupled(const std::string& id) : Coupled(id) {
        auto rifleGen = addComponent<RifleQueueGenerator>("rifleGen");
        auto rifle = addComponent<Rifle>("rifle");
      
        addCoupling(rifleGen->out_triggerPressed, rifle->in_triggerPressed);
        addCoupling(rifleGen->out_firingSelector, rifle->in_firingSelector);
        addCoupling(rifleGen->out_boltBack, rifle->in_boltBack);
        addCoupling(rifleGen->out_magSeating, rifle->in_magSeating);
        addCoupling(rifleGen->out_bulletLoaded, rifle->in_bulletLoaded);


        
    }

};

#endif