#ifndef STYLES_UNSTABLE_FLICKER_H
#define STYLES_UNSTABLE_FLICKER_H

// Usage: UnstableFlicker<A, B>
// Or: UnstableFlickerL<B>
// A, B: COLOR
// return value: COLOR
// LED is randomly chosen as COLOR1 or COLOR2, then stays
// that color for a duration based on audio. Quiet audio
// means shorter duration, loud audio means longer duration.

class BladeBase;

#include "alpha.h"
#include "../functions/unstable_flicker.h"
#include "layers.h"

template<class B>
using UnstableFlickerL = AlphaL<B, UnstableFlickerF>;

template<class A, class B>
using UnstableFlicker = Layers<A, UnstableFlickerL<B>>;

#endif
