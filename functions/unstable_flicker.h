#ifndef FUNCTIONS_UNSTABLE_FLICKER_H
#define FUNCTIONS_UNSTABLE_FLICKER_H

// Usage: UnstableFlickerF
// Returns either 0 or 32768. The returned value
// is held for a random duration.

class UnstableFlickerF {
 public:
  void run(BladeBase* blade) {
    uint32_t now = micros();
    if (now > next_update_) {
      if (value_)
      {
        
        value_ = 0;
        next_update_ = now + random(100000, 500000);
      }
      else
      {
        value_ = 32768;
        next_update_ = now + random(10000, 100000);
        //next_update_ = now + clampi32(dynamic_mixer.last_sum() << 3, 1000, 10000);
      }
    }
  }
  int getInteger(int led) { return value_; }
  
 private:
  int value_;
  uint32_t next_update_;
};

#endif
