#ifndef RIFLEQUEUEGENERATOR_HPP
#define RIFLEQUEUEGENERATOR_HPP

#include <iostream>
#include <limits>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

// State structure for the generator
struct RifleQueueGeneratorState {
    int messages_sent;      // Number of messages sent so far
    double sigma;           // Time until next internal transition
    int test_phase;         // Scenario selector (1, 2, or 3)
    int firing_mode;        // 0 = safe, 1 = single, 2 = auto
    bool trigger_pressed;   // True if trigger is pressed
    bool bolt_back;         // True if bolt is in the "back" position
    bool mag_seated;        // True if magazine is seated
    int bullets_remaining;  // Number of bullets remaining

    // Constructor initializes the state
    RifleQueueGeneratorState()
        : messages_sent(0),
          sigma(1.0),
          test_phase(1),       
          firing_mode(0),
          trigger_pressed(false),
          bolt_back(false),
          mag_seated(true),
          bullets_remaining(10) {}
};

#ifndef NO_LOGGING
#include <ostream>
std::ostream& operator<<(std::ostream &out, const RifleQueueGeneratorState& state) {
    out << "{Messages Sent: " << state.messages_sent 
        << ", Sigma: " << state.sigma 
        << ", Test Phase: " << state.test_phase
        << ", Firing Mode: " << state.firing_mode
        << ", Trigger Pressed: " << (state.trigger_pressed ? "Yes" : "No")
        << ", Bolt Back: " << (state.bolt_back ? "Yes" : "No")
        << ", Mag Seated: " << (state.mag_seated ? "Yes" : "No")
        << ", Bullets Remaining: " << state.bullets_remaining << "}";
    return out;
}
#endif

// RifleQueueGenerator atomic model: Generates test events for 3 scenarios.
class RifleQueueGenerator : public Atomic<RifleQueueGeneratorState> {
public:
    Port<int> out_triggerPressed;
    Port<int> out_firingSelector;
    Port<int> out_boltBack;
    Port<int> out_magSeating;
    Port<int> out_bulletLoaded;

    
    RifleQueueGenerator(const std::string& id, int maxMessages = 30, double interval = 1.0)
        : Atomic<RifleQueueGeneratorState>(id, RifleQueueGeneratorState()),
          MAX_MESSAGES(maxMessages), INTERVAL(interval)
    {
        out_triggerPressed = addOutPort<int>("out_triggerPressed");
        out_firingSelector = addOutPort<int>("out_firingSelector");
        out_boltBack = addOutPort<int>("out_boltBack");
        out_magSeating = addOutPort<int>("out_magSeating");
        out_bulletLoaded = addOutPort<int>("out_bulletLoaded");
    }


    void internalTransition(RifleQueueGeneratorState& state) const override {
        state.messages_sent++;

        if(state.test_phase == 1) {
            // Scenario 1: Alternate trigger press; update firing mode every 5 messages.
            if(state.messages_sent % 5 == 0) {
                state.firing_mode = (state.firing_mode + 1) % 3;
            }
            state.trigger_pressed = !state.trigger_pressed;
        }
        else if(state.test_phase == 2) {
            // Scenario 2: Toggle magazine seating every 6 messages and fire if seated.
            if(state.messages_sent % 6 == 0) {
                state.mag_seated = !state.mag_seated;
            }
            if(state.mag_seated && state.bullets_remaining > 0) {
                state.trigger_pressed = true;
                state.bullets_remaining--;
            }
            else {
                state.trigger_pressed = false;
            }
        }
        else if(state.test_phase == 3) {
            // Scenario 3: Toggle bolt position; fire if bolt is back and bullets are available.
            state.bolt_back = !state.bolt_back;
            if(state.bolt_back && state.bullets_remaining > 0) {
                state.trigger_pressed = true;
                state.bullets_remaining--;
            }
            else {
                state.trigger_pressed = false;
            }
        }

        // If maximum messages sent or bullets run out, set sigma to infinity.
        if(state.messages_sent >= MAX_MESSAGES || state.bullets_remaining <= 0) {
            state.sigma = std::numeric_limits<double>::infinity();
        }
        else {
            state.sigma = INTERVAL;
        }
    }

  
    void externalTransition(RifleQueueGeneratorState& state, double e) const override {}


    void output(const RifleQueueGeneratorState& state) const override {
        out_triggerPressed->addMessage(state.trigger_pressed ? 1 : 0);
        out_firingSelector->addMessage(state.firing_mode);
        out_boltBack->addMessage(state.bolt_back ? 1 : 0);
        out_magSeating->addMessage(state.mag_seated ? 1 : 0);
        // Output a bullet-loaded message if trigger is pressed and bullets are available.
        if(state.trigger_pressed && state.bullets_remaining > 0) {
            out_bulletLoaded->addMessage(1);
        }
    }

    [[nodiscard]] double timeAdvance(const RifleQueueGeneratorState& state) const override {
        return state.sigma;
    }

private:
    const int MAX_MESSAGES;
    const double INTERVAL;
};

#endif // RIFLEQUEUEGENERATOR_HPP