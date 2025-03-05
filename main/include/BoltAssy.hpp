#ifndef BOLTASSY_HPP
#define BOLTASSY_HPP

#include <random>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct BoltAssyState {
    double sigma;
    enum class States {PASSIVE, ACTIVE};
    States currentState;
    int tempMsgVal, boltFree, readyBullet, boltState;
    
    
    explicit BoltAssyState() : sigma(1), tempMsgVal(0), boltFree(0), readyBullet(0), boltState(0) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const BoltAssyState& state) {
    out  << "{" << state.sigma << ", " 
         << "State: " << (state.currentState == BoltAssyState::States::PASSIVE ? "PASSIVE" : "ACTIVE") 
         << ", boltFree: " << state.boltFree 
         << ", readyBullet: " << state.readyBullet 
         << ", boltState: " << state.boltState << "}";
    return out;
}
#endif

class BoltAssy : public Atomic<BoltAssyState> {
public:

    Port<int> in_bulletReady;
    Port<int> in_releaseBolt;
    Port<int> in_boltBack;
    Port<int> out_bulletLoaded;
    Port<int> out_boltPosn;


    BoltAssy(const std::string& id) : Atomic<BoltAssyState>(id, BoltAssyState()) {
       
        in_bulletReady = addInPort<int>("in_bulletReady");
        in_releaseBolt = addInPort<int>("in_releaseBolt");
        in_boltBack = addInPort<int>("in_boltBack");
        out_bulletLoaded = addOutPort<int>("out_bulletLoaded");
        out_boltPosn = addOutPort<int>("out_boltPosn");
    }

    
    void internalTransition(BoltAssyState& state) const override {
       
        state.sigma = std::numeric_limits<double>::infinity();
        state.currentState = BoltAssyState::States::PASSIVE;
    }


    void externalTransition(BoltAssyState& state, double e) const override {
     
        if (!in_bulletReady->empty()) {
            state.readyBullet = in_bulletReady->getBag().back();  
        }

        if (!in_releaseBolt->empty()) {
            if (in_releaseBolt->getBag().back() == 1 && state.boltState == 1) {
                state.boltFree = 1;  // Release the bolt if it is pulled back
            }
        } else if (!in_boltBack->empty()) {
            if (in_boltBack->getBag().back() == 1 && state.boltState == 0) {
                state.boltState = 1;  // Move bolt back if it is forward
            } else if (in_boltBack->getBag().back() == 1 && state.boltState == 1) {
                state.boltState = 0;  // Move bolt forward if it is already back
            }
        }

        // Generate a random value for bullet loading with a 90% success rate
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        double randVal = dis(gen); 

        if (state.boltFree == 1 && state.boltState == 1) {
            if (state.readyBullet == 1) {
                // If a bullet is ready to load, 90% chance to load successfully
                if (randVal < 0.90) {
                    state.boltState = 0;  // Move the bolt forward to load the bullet
                } else {
                    state.boltState = 2;  
                }
            } else {
                state.boltState = 0;  // Move bolt forward if no bullet is ready
            }

        
            state.readyBullet = 0;
            state.boltFree = 0;
            state.currentState = BoltAssyState::States::ACTIVE;
            state.sigma = 0.0;  
        }
    }

    // Output function
    void output(const BoltAssyState& state) const override {
       
        if (state.boltState == 0) {
            out_bulletLoaded->addMessage(1);  // Bullet loaded successfully
        } else if (state.boltState == 2) {
            out_bulletLoaded->addMessage(0);  // No bullet loaded (issue)
        }

        // Output the current state of the bolt (0 = forward, 1 = back, 2 = issue)
        out_boltPosn->addMessage(state.boltState);
    }

    // Time advance function
    [[nodiscard]] double timeAdvance(const BoltAssyState& state) const override {
        return state.sigma;  
    }
};

#endif // BOLTASSY_HPP
