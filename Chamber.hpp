#ifndef CHAMBER_HPP
#define CHAMBER_HPP

#include <random>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct ChamberState {
    enum class States {PASSIVE, ACTIVE};
    States currentState;
    double sigma;
    int dudBullet; 
    int bulletIn;
    
    explicit ChamberState(): sigma(1), currentState(States::PASSIVE), dudBullet(2), bulletIn(0){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const ChamberState& state) {
    out  << "{" << state.sigma << "} {dudBullet: " << state.dudBullet << ", bulletIn: " << state.bulletIn << "}";
    return out;
}
#endif

class Chamber : public Atomic<ChamberState> {
public:

    Port<int> in_isDud;
    Port<int> in_bulletLoaded;
    Port<int> out_boltBack;
    Port<int> out_bulletFired;
    Port<int> out_casing;

    Chamber(const std::string id) : Atomic<ChamberState>(id, ChamberState()) {
        in_isDud = addInPort<int>("in_isDud");
        in_bulletLoaded = addInPort<int>("in_bulletLoaded");
        out_boltBack = addOutPort<int>("out_boltBack");
        out_bulletFired = addOutPort<int>("out_bulletFired");
        out_casing = addOutPort<int>("out_casing");
    }

    // internal transition
    void internalTransition(ChamberState& state) const override {
        // Reset the variables after firing
        state.currentState = ChamberState::States::PASSIVE;
        state.dudBullet = 2;        // Clear the dudBullet variable
        state.bulletIn = 0;         // Clear the bulletIn variable
        state.sigma = std::numeric_limits<double>::infinity();  // No further events
    }

    // external transition
    void externalTransition(ChamberState& state, double e) const override {
        if (!in_isDud->empty()) {
            state.dudBullet = in_isDud->getBag().back();
        }

        if (!in_bulletLoaded->empty()) {
            state.bulletIn = in_bulletLoaded->getBag().back();
            state.sigma = 5.0; 
        }
    }
    
    // output function
    void output(const ChamberState& state) const override {
        if ((state.dudBullet == 0) && (state.bulletIn == 1)) {
            out_boltBack->addMessage(1);    // bolt is back
            out_bulletFired->addMessage(1); // Bullet fired
            out_casing->addMessage(1);     // Casing ejected
        }
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const ChamberState& state) const override {     
        return state.sigma; 
    }
};

#endif
