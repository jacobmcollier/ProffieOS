#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 2
#define VOLUME 1000
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 1.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_ADAFRUIT_SPIFLASH
#define ENABLE_AL8807
#define SAVE_STATE
#endif

#ifdef CONFIG_PRESETS
Preset presets[] = {
    { "Lightside", "tracks/venus.wav", StyleNormalPtr<BLUE, WHITE,   300, 800>(), "Lightside_Solid_Blue"},
    { "Darkside",  "tracks/venus.wav", StyleNormalPtr<RED, WHITE, 300, 800>(), "Darkside_Solid_Red"},
    { "Darkside",  "tracks/venus.wav", StyleNormalPtr<AudioFlicker<RED, WHITE>, WHITE, 300, 800>(), "Darkside_Flicker_Red"},
    { "DarkSaber", "tracks/venus.wav", StyleNormalPtr<WHITE, WHITE, 300, 800>(), "DarkSaber_Solid_White"},
    { "DarkSaber", "tracks/venus.wav", StyleNormalPtr<AudioFlicker<WHITE, Black>, WHITE, 300, 800>(), "DarkSaber_Flicker_White"},
};

struct RedLED {
  static constexpr float MaxAmps = 0.7;
  static constexpr float MaxVolts = 2.5;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 1.5;
  static constexpr float R = 0;
  static const int Red = 255;
  static const int Green = 0;
  static const int Blue = 0;
};

struct GreenLED {
  static constexpr float MaxAmps = 0.7;
  static constexpr float MaxVolts = 2.5;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 1.5;
  static constexpr float R = 0;
  static const int Red = 0;
  static const int Green = 255;
  static const int Blue = 0;
};

struct BlueLED {
  static constexpr float MaxAmps = 0.7;
  static constexpr float MaxVolts = 2.5;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 1.5;
  static constexpr float R = 0;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
};

struct WhiteLED {
  static constexpr float MaxAmps = 0.7;
  static constexpr float MaxVolts = 2.5;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 1.5;
  static constexpr float R = 0;
  static const int Red = 255;
  static const int Green = 255;
  static const int Blue = 255;
};

BladeConfig blades[] = {
    { 0, // Blade ID resistance
      SimpleBladePtr<RedLED, GreenLED, BlueLED, WhiteLED,
      //bladePowerPin1, bladePowerPin2, bladePowerPin3, bladePowerPin4>(),
      bladePowerPin2, bladePowerPin1, bladePowerPin4, bladePowerPin3>(), // To correct pinout issue in first demo board
      CONFIGARRAY(presets),
    },
};
#endif

#ifdef CONFIG_BUTTONS
Button PowerButton(BUTTON_POWER, powerButtonPin, "pow");
Button AuxButton(BUTTON_AUX, auxPin, "aux");
#endif