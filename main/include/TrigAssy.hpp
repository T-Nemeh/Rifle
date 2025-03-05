#ifndef TRIGASSY_HPP
#define TRIGASSY_HPP

#include <limits>   // for std::numeric_limits
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct TrigAssyState {
    enum class States { PASSIVE, ACTIVE };
    
    
    States currentState;
    
    double sigma;
    
    int triggerPull;     // 0 or 1
    int firingSelector;  // 0 = safe, 1 = single, 2 = auto

    
    TrigAssyState()
        : currentState(States::PASSIVE),
          sigma(std::numeric_limits<double>::infinity()), 
          triggerPull(0),
          firingSelector(1) {}  
};

#ifndef NO_LOGGING

std::ostream& operator<<(std::ostream &out, const TrigAssyState &state) {
    // Prints: { sigma, currentStateAsInt }
    out << "{" << state.sigma << ", " << static_cast<int>(state.currentState) << "}";
    return out;
}
#endif

class TrigAssy : public Atomic<TrigAssyState> {
public:
    
    Port<int> in_triggerPressed;
    Port<int> in_firingSelector;
    Port<int> in_boltBack;
    Port<int> out_releaseBolt;

    
    explicit TrigAssy(const std::string &id, double preparationTime = 0.0)
        : Atomic<TrigAssyState>(id, TrigAssyState()) {

        
        in_triggerPressed = addInPort<int>("in_triggerPressed");
        in_firingSelector = addInPort<int>("in_firingSelector");
        in_boltBack       = addInPort<int>("in_boltBack");

        
        out_releaseBolt   = addOutPort<int>("out_releaseBolt");
    }


    void internalTransition(TrigAssyState &s) const override {
        // If in single-shot mode and the trigger was pressed → reset the trigger
        if (s.firingSelector == 1 && s.triggerPull == 1) {
            s.triggerPull = 0; 
        }
        
        s.currentState = TrigAssyState::States::PASSIVE;
        s.sigma = std::numeric_limits<double>::infinity();
    }


    void externalTransition(TrigAssyState &s, double e) const override {
        
        if (!in_triggerPressed->empty()) {
            s.triggerPull = in_triggerPressed->getBag().back();  
        }
        
        if (!in_firingSelector->empty()) {
            s.firingSelector = in_firingSelector->getBag().back();
        }

        if (!in_boltBack->empty()) {
            
        }
        
        s.currentState = TrigAssyState::States::ACTIVE;
        s.sigma = 0.0;
    }

    void output(const TrigAssyState &s) const override {
      
        if (s.triggerPull != 1) {
            return; 
        }

    
        switch (s.firingSelector) {
            case 0: // SAFE
                // Trigger pressed while on safe => output 0 (bolt stays locked)
                out_releaseBolt->addMessage(0);
                break;

            case 1: // SINGLE
                // If trigger pulled => release bolt
                out_releaseBolt->addMessage(1);
                // The actual reset of the triggerPull is in internalTransition().
                break;

            case 2: // AUTO
                // If trigger pulled => release bolt
                out_releaseBolt->addMessage(1);
                // Keep triggerPull = 1 so that if the bolt cycles (boltBack),
                // we can fire again on next external transition.
                break;

            default:
                // Do nothing
                break;
        }
    }


    [[nodiscard]] double timeAdvance(const TrigAssyState &s) const override {
        return s.sigma;
    }


};

#endif // TRIGASSY_HPP
