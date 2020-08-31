#ifndef FUNCTIONS_UNSTABLE_FLICKER_H
#define FUNCTIONS_UNSTABLE_FLICKER_H

// Usage: UnstableFlickerF
// Returns either 0, 15000, 25000, or 32768 based on audio volume

class UnstableFlickerF {
 public:
  void run(BladeBase* blade) {
    value_ = clampi32(dynamic_mixer.last_sum() << 3, 0, 32768);
    if (value_ > 20000)
    {
      value_ = 32768;
    }
    else if (value_ > 15000)
    {
      value_ = 25000;
    }
    else if (value_ > 5000)
    {
      value_ = 15000;
    }
    else
    {
      value_ = 0;
    }
  }
  int getInteger(int led) { return value_; }
  
 private:
  int value_;
  uint32_t next_update_;
};

#endif
