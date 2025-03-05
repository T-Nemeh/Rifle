#ifndef BULLET_HPP
#define BULLET_HPP

#include <random>
#include <iostream>
#include <limits>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct BulletState {
    double sigma;
    enum class States {PASSIVE, ACTIVE};
    States currentState;
    int bulletRdy = 0;
    int isDud = 0;

    explicit BulletState() 
        : sigma(1), 
          currentState(States::PASSIVE), 
          bulletRdy(0), 
          isDud(0) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const BulletState& state) {
    out  << "{" << state.sigma << ", " 
         << "State: " << (state.currentState == BulletState::States::PASSIVE ? "PASSIVE" : "ACTIVE") 
         << ", bulletRdy: " << state.bulletRdy 
         << ", isDud: " << state.isDud << "}";
    return out;
}
#endif

class Bullet : public Atomic<BulletState> {
public:
    Port<int> in_bulletReady;
    Port<int> out_isDud;
    Port<int> out_bulletReady;

    Bullet(const std::string& id) 
        : Atomic<BulletState>(id, BulletState()) 
    {
        
        in_bulletReady = addInPort<int>("in_bulletReady");
        out_isDud = addOutPort<int>("out_isDud");
        out_bulletReady = addOutPort<int>("out_bulletReady");
    }

    // Internal transition
    void internalTransition(BulletState& state) const override {
        state.currentState = BulletState::States::PASSIVE;
        state.sigma = std::numeric_limits<double>::infinity();
    }

    // External transition
    void externalTransition(BulletState& state, double e) const override {
        if (!in_bulletReady->empty()) {
            state.bulletRdy = in_bulletReady->getBag().back(); 
        }
        // Random number generation to determine if the bullet is a dud (95% chance it is not a dud)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        double randVal = dis(gen);

        if (randVal < 0.95) {
            state.isDud = 0;  // Not a dud
        } else {
            state.isDud = 1;  // Dud
        }

        state.currentState = BulletState::States::ACTIVE;
        state.sigma = 0;  // No delay
    }

    // Output function
    void output(const BulletState& state) const override {
        out_isDud->addMessage(state.isDud);      
        out_bulletReady->addMessage(state.bulletRdy); 
    }

    // Time advance function
    [[nodiscard]] double timeAdvance(const BulletState& state) const override {
        return state.sigma;
    }
};

#endif // BULLET_HPP