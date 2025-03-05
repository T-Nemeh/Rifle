#ifndef MAGASSY_HPP
#define MAGASSY_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "Magazine.hpp"
#include "Bullet.hpp"

using namespace cadmium;


class MagAssy : public cadmium::Coupled {
public:
  
    Port<int> in_initBullets;
    Port<int> in_initMagSeating;
    Port<int> in_bulletLoaded;
    Port<int> out_bulletReady;
    Port<int> out_isDud;

    // Constructor
    MagAssy(const std::string& id) : Coupled(id) {
        // Initialize ports
        in_initBullets = addInPort<int>("in_initBullets");
        in_initMagSeating = addInPort<int>("in_initMagSeating"); 
        in_bulletLoaded = addInPort<int>("in_bulletLoaded");
        out_bulletReady = addOutPort<int>("out_bulletReady");
        out_isDud = addOutPort<int>("out_isDud");


        auto bullet = addComponent<Bullet>("Bullet");
        auto magazine = addComponent<Magazine>("Magazine");

        addCoupling(this->in_initBullets, magazine->in_initBullets);
        addCoupling(this->in_initMagSeating, magazine->in_initMagSeating);
        addCoupling(this->in_bulletLoaded, magazine->in_bulletLoaded);

        addCoupling(magazine->out_bulletReady, bullet->in_bulletReady);
        addCoupling(bullet->out_isDud, this->out_isDud);
        addCoupling(bullet->out_bulletReady, this->out_bulletReady);
    }

    
};

#endif // MAGASSY_HPP