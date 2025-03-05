
#ifndef MAGAZINE_HPP
#define MAGAZINE_HPP

#include <random>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct MagazineState {
    enum class States {PASSIVE, ACTIVE};
    States currentState;
    double sigma;
    int tempMsgVal, bulletsLeft, magSeating, bulletReady;

    explicit MagazineState(): sigma(1), currentState(States::PASSIVE), tempMsgVal(0), bulletsLeft(0), magSeating(0), bulletReady(0)  {
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const MagazineState& state) {
    out  << "{" << state.sigma << "}";
    return out;
}
#endif

class Magazine : public Atomic<MagazineState> {
    public:

    Port<int> in_initBullets;
    Port<int> in_bulletLoaded; //bullet loaded from magazine to chamber by BoltAssy
    Port<int> in_initMagSeating;
    Port<int> out_bulletReady;


    Magazine(const std::string id) : Atomic<MagazineState>(id, MagazineState()) {
        in_initBullets = addInPort<int>("in_initBullets");
        in_bulletLoaded = addInPort<int>("in_bulletLoaded");
        out_bulletReady = addOutPort<int>("out_bulletReady");
        in_initMagSeating = addInPort<int>("in_initMagSeating");
    }


    void internalTransition(MagazineState& state) const override {

        state.currentState = MagazineState::States::PASSIVE;
        state.sigma = std::numeric_limits<double>::infinity();
    }

 
    void externalTransition(MagazineState& state, double e) const override {

        state.sigma -= e;
        if(!in_initBullets->empty()){
            state.tempMsgVal = in_initBullets->getBag().back();
            if ((state.tempMsgVal >= 0) && (state.tempMsgVal<30)){
                state.bulletsLeft = state.tempMsgVal;
            }
        }
        else if(!in_initMagSeating->empty()){
            state.tempMsgVal = in_initMagSeating->getBag().back();
            state.magSeating =  state.tempMsgVal;
        }
        else if(!in_bulletLoaded->empty()){
            state.tempMsgVal = in_bulletLoaded->getBag().back(); 
            if(state.tempMsgVal == 1){
                state.bulletsLeft--;
            }
        }
        if(state.bulletsLeft >= 0){
            if(state.magSeating == 1){
                state.bulletReady = 1;
            }
        }
        else state.bulletReady = 0;

        state.currentState = MagazineState::States::ACTIVE;
        state.sigma = 0.0;
    }
    
    
    // output function
    void output(const MagazineState& state) const override {
       
        
        out_bulletReady->addMessage(state.bulletReady);
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const MagazineState& state) const override {     
            return state.sigma;
    }
};

#endif
