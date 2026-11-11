/* Custom rainbow: a user-defined palette scrolled across the keyboard.
 *
 * The stock CYCLE_* effects walk the full HSV hue wheel, so you get the
 * whole spectrum and cannot choose the colors. This one interpolates
 * between the entries of PALETTE below, in RGB, so any set of colors
 * works -- including ones with no hue at all (white, grey, black).
 *
 * The Vial brightness and speed sliders both still apply. */

RGB_MATRIX_EFFECT(CUSTOM_RAINBOW)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

/* Yellow -> orange -> red -> blue -> light blue, in descending hue order,
 * with intermediate steps so the gradient reads smooth rather than as five
 * hard stops.
 *
 * Two things worth knowing about the shape of this list:
 *
 * Going down in hue from red (0 deg) to blue (240 deg) is 120 degrees the
 * short way, and that way passes through magenta and violet. Those are not
 * in the requested set but something has to bridge the gap; the alternative
 * direction is 240 degrees through green and cyan, which is worse.
 *
 * The palette wraps, so the last entry blends back into the first. Running
 * straight from light blue back up to yellow would drag the gradient through
 * green, so instead the second half retraces the first in reverse. The loop
 * closes seamlessly and contains only the colors above. For a one-way sweep
 * that does pass through green, delete everything after the AZZURRO line. */
static const rgb_t PALETTE[] = {
    {.r = 0xFF, .g = 0xFF, .b = 0x00}, /* yellow          h=60  */
    {.r = 0xFF, .g = 0xBF, .b = 0x00}, /*                 h=45  */
    {.r = 0xFF, .g = 0x80, .b = 0x00}, /* orange          h=30  */
    {.r = 0xFF, .g = 0x40, .b = 0x00}, /*                 h=15  */
    {.r = 0xFF, .g = 0x00, .b = 0x00}, /* red             h=0   */
    {.r = 0xFF, .g = 0x00, .b = 0x80}, /*                 h=330 */
    {.r = 0xFF, .g = 0x00, .b = 0xFF}, /* magenta         h=300 */
    {.r = 0x80, .g = 0x00, .b = 0xFF}, /*                 h=270 */
    {.r = 0x00, .g = 0x00, .b = 0xFF}, /* blue            h=240 */
    {.r = 0x00, .g = 0x55, .b = 0xFF}, /*                 h=220 */
    {.r = 0x00, .g = 0xAA, .b = 0xFF}, /* light blue      h=200 */
    /* ---- mirror, so the wrap does not go through green ---- */
    {.r = 0x00, .g = 0x55, .b = 0xFF},
    {.r = 0x00, .g = 0x00, .b = 0xFF},
    {.r = 0x80, .g = 0x00, .b = 0xFF},
    {.r = 0xFF, .g = 0x00, .b = 0xFF},
    {.r = 0xFF, .g = 0x00, .b = 0x80},
    {.r = 0xFF, .g = 0x00, .b = 0x00},
    {.r = 0xFF, .g = 0x40, .b = 0x00},
    {.r = 0xFF, .g = 0x80, .b = 0x00},
    {.r = 0xFF, .g = 0xBF, .b = 0x00},
};

#define PALETTE_SIZE (sizeof(PALETTE) / sizeof(PALETTE[0]))

/* pos walks 0..255 across the palette; blend the two neighbouring entries. */
static rgb_t palette_at(uint8_t pos) {
    uint16_t scaled = (uint16_t)pos * PALETTE_SIZE;
    uint8_t  idx    = scaled >> 8;
    uint8_t  frac   = scaled & 0xFF;
    rgb_t    a      = PALETTE[idx];
    rgb_t    b      = PALETTE[(idx + 1) % PALETTE_SIZE];

    rgb_t out = {
        .r = a.r + scale8(b.r - a.r, frac),
        .g = a.g + scale8(b.g - a.g, frac),
        .b = a.b + scale8(b.b - a.b, frac),
    };
    return out;
}

static bool CUSTOM_RAINBOW(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint8_t time = scale16by8(g_rgb_timer, qadd8(rgb_matrix_config.speed, 1) >> 2);
    uint8_t val  = rgb_matrix_config.hsv.v;

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        /* Scroll left-to-right by LED x coordinate. Use point[i].y for a
         * vertical sweep, or (x + y) / 2 for a diagonal one. */
        rgb_t c = palette_at(g_led_config.point[i].x - time);
        rgb_matrix_set_color(i, scale8(c.r, val), scale8(c.g, val), scale8(c.b, val));
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
