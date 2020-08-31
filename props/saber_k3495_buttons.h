// k3495 props file, includes a 2 button mode.  Incorporates
// on-the-fly volume controls and color wheel
//
// Button config:
//
// 2 Button:
// POWER
// Turn On - short click while OFF
// Turn Off - hold and wait till blade is off while ON
// Blast Effect - short click while ON
// Color Change Mode - Triple click power button while ON to enter or exit mode, twist hilt to change color
// single click power button to exit color change mode
// Can also turn off to exit color change mode
// Volume Down - hold POWER + short click AUX while OFF (Wraps)
// AUX
// Next Preset - short click while OFF
// Lockup - hold while ON
// Battery level - hold while off
// Button Lock - triple click while ON


#ifndef PROPS_SABER_K3495_BUTTONS_H
#define PROPS_SABER_K3495_BUTTONS_H

#include "prop_base.h"

#undef PROP_TYPE
#define PROP_TYPE SaberK3495Buttons

#ifndef BUTTON_DOUBLE_CLICK_TIMEOUT
#define BUTTON_DOUBLE_CLICK_TIMEOUT 300
#endif

#ifndef BUTTON_SHORT_CLICK_TIMEOUT
#define BUTTON_SHORT_CLICK_TIMEOUT 300
#endif

#ifndef BUTTON_HELD_TIMEOUT
#define BUTTON_HELD_TIMEOUT 300
#endif

#ifndef BUTTON_HELD_MEDIUM_TIMEOUT
#define BUTTON_HELD_MEDIUM_TIMEOUT 1000
#endif

#ifndef BUTTON_HELD_LONG_TIMEOUT
#define BUTTON_HELD_LONG_TIMEOUT 2000
#endif

// The Saber class implements the basic states and actions
// for the saber.
class SaberK3495Buttons : public PropBase {
public:
  SaberK3495Buttons() : PropBase() {}
  const char* name() override { return "SaberK3495Buttons"; }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (EVENTID(button, event, modifiers)) {
      case EVENTID(BUTTON_POWER, EVENT_PRESSED, MODE_ON):
      case EVENTID(BUTTON_AUX, EVENT_PRESSED, MODE_ON):
        if (accel_.x < -0.15) {
          pointing_down_ = true;
        } else {
          pointing_down_ = false;
        }
      return true;

      // Turn ON
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_OFF):
        On();
        return true;
      
      // Turn OFF
      case EVENTID(BUTTON_POWER, EVENT_FIRST_HELD_LONG, MODE_ON):
        if (button_disable_) {
          return true;
        }
        color_wheel_active_ = false;
        Off();
        return true;

      // Blaster Deflection
      case EVENTID(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT, MODE_ON):
        if (button_disable_) {
          return true;
        }
        
        if (color_wheel_active_)
        {
          color_wheel_active_ = false;
          ToggleColorChangeMode();
          return true;
        }
        
        STDOUT.println("DoBlast");
        SaberBase::DoBlast();
        return true;

      // Color Change Mode
      case EVENTID(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
        if (button_disable_) {
          return true;
        }
        
        color_wheel_active_ = !color_wheel_active_;
        ToggleColorChangeMode();
        return true;

      // Volume Down
      case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_SHORT, MODE_OFF | BUTTON_POWER):
        STDOUT.println("Volume Down");
        if (dynamic_mixer.get_volume() > 0) {
          dynamic_mixer.set_volume(std::max<int>(0, dynamic_mixer.get_volume() - VOLUME * 0.25));
        } else {
          dynamic_mixer.set_volume(VOLUME);
        }
        talkie.SayNumber(dynamic_mixer.get_volume()*100/VOLUME);
        talkie.Say(spPERCENT);
        STDOUT.print("Current Volume: ");
        STDOUT.println(dynamic_mixer.get_volume());
        return true;

      // Next Preset
      case EVENTID(BUTTON_AUX, EVENT_FIRST_CLICK_SHORT, MODE_OFF):
        next_preset();
        return true;

      // Lockup Start
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD, MODE_ON):
        if (button_disable_) {
          return true;
        }

        if (!SaberBase::Lockup()) {
          if (pointing_down_) {
            SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
          } else {
            SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
          }
          SaberBase::DoBeginLockup();
          return true;
        }
        break;

      // Battery Level
      case EVENTID(BUTTON_AUX, EVENT_FIRST_HELD_LONG, MODE_OFF):
        talkie.SayNumber((int)floorf(battery_monitor.battery_percent()));
        talkie.Say(spPERCENT);
        return true;
      
      // Button Lock
      case EVENTID(BUTTON_AUX, EVENT_THIRD_SAVED_CLICK_SHORT, MODE_ON):
        if (button_disable_) {
          button_disable_ = false;
        } else {
          button_disable_ = true;
        }
        beeper.Beep(0.5, 2000.0);
        return true;

      // Lockup End
      case EVENTID(BUTTON_AUX, EVENT_RELEASED, MODE_ANY_BUTTON | MODE_ON):
        if (SaberBase::Lockup()) {
          SaberBase::DoEndLockup();
          SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
          return true;
        }
    }
    return false;
  }
private:
  bool pointing_down_ = false;
  bool button_disable_ = false;
  bool color_wheel_active_ = false;
};

#endif
