#ifdef CONFIG_TOP
#include "proffieboard_v2_config.h"
#define NUM_BLADES 1
#define NUM_BUTTONS 2
#define VOLUME 1500
const unsigned int maxLedsPerStrip = 144;
#define CLASH_THRESHOLD_G 3.0
#define ENABLE_AUDIO
#define ENABLE_MOTION
#define ENABLE_ADAFRUIT_SPIFLASH
#define ENABLE_AL8807
#define SAVE_STATE
#endif


#ifdef CONFIG_PRESETS
Preset presets[] = {
    { "Lightside", "tracks/none.wav", StyleNormalPtr<ColorChange<TrFade<100>, BLUE, IceBlue, Orange, Yellow, SilverWhite, Purple, RED, GREEN>, WHITE,   300, 800>(), "Lightside_Solid_Blue"},
    { "Darkside",  "tracks/none.wav", StyleNormalPtr<ColorChange<TrFade<100>, RED, GREEN, BLUE, IceBlue, Orange, Yellow, SilverWhite, Purple>, WHITE, 300, 800>(), "Darkside_Solid_Red"},
    { "Darkside",  "tracks/none.wav", StyleNormalPtr<AudioFlicker<ColorChange<TrFade<100>, RED, GREEN, BLUE, IceBlue, Orange, Yellow, SilverWhite, Purple>, Black>, WHITE, 300, 800>(), "Darkside_Flicker_Red"},
    { "DarkSaber", "tracks/none.wav", StyleNormalPtr<ColorChange<TrFade<100>, SilverWhite, Purple, RED, GREEN, BLUE, IceBlue, Orange, Yellow>, WHITE, 300, 800>(), "DarkSaber_Solid_White"},
    { "DarkSaber", "tracks/none.wav", StyleNormalPtr<AudioFlicker<ColorChange<TrFade<100>, SilverWhite, Purple, RED, GREEN, BLUE, IceBlue, Orange, Yellow>, Black>, WHITE, 300, 800>(), "DarkSaber_Flicker_White"},
};

// Takes battery voltage into account which is high right now...
// Max current is 2.5V average
// R = 5+ gives 99.9% duty cycle, 3.3V average
// R = 2 gives 77% duty cycle, 2.5V average
// R = 1 gives 66% duty cycle, 2V average
// R = 0 gives 44% duty cycle, 1.47V average
// Need 2.5V average to be max
// duty cycle also dependent on battery level...
struct RedLED {
  static constexpr float MaxAmps = 0.7;
  static constexpr float MaxVolts = 2.5;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 1.5;
  static constexpr float R = 5;
  static const int Red = 255;
  static const int Green = 0;
  static const int Blue = 0;
};

struct GreenLED {
  static constexpr float MaxAmps = 0.7;
  static constexpr float MaxVolts = 2.5;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 1.5;
  static constexpr float R = 5;
  static const int Red = 0;
  static const int Green = 255;
  static const int Blue = 0;
};

struct BlueLED {
  static constexpr float MaxAmps = 0.7;
  static constexpr float MaxVolts = 2.5;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 1.5;
  static constexpr float R = 5;
  static const int Red = 0;
  static const int Green = 0;
  static const int Blue = 255;
};

struct WhiteLED {
  static constexpr float MaxAmps = 0.7;
  static constexpr float MaxVolts = 2.5;
  static constexpr float P2Amps = 0.35;
  static constexpr float P2Volts = 1.5;
  static constexpr float R = 5;
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
