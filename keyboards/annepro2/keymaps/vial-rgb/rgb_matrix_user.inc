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

/* A warm-to-cold ramp with red and blue at the two ends.
 *
 * No magenta or violet: those only appeared in an earlier version because
 * walking down the hue wheel from red to blue has to pass through them. This
 * palette does not walk the hue wheel at all, it crosses straight from the
 * warm half to the cold half.
 *
 * Yellow and light blue sit next to each other, so interpolating between them
 * produces a pale green. That is deliberate: it is the crossover the Obins
 * firmware had. To drop the green and use a neutral midpoint instead, put
 * these three between yellow and light blue, mirrored below as well:
 *     {.r = 0xFF, .g = 0xFF, .b = 0x99},
 *     {.r = 0xFF, .g = 0xFF, .b = 0xFF},
 *     {.r = 0xB0, .g = 0xE8, .b = 0xFF},
 *
 * The palette wraps, so the second half retraces the first in reverse. Going
 * straight from blue back to red would blend through purple, which is what we
 * are avoiding. For a one-way sweep, delete everything after the BLUE line. */
static const rgb_t PALETTE[] = {
    {.r = 0xFF, .g = 0x00, .b = 0x00}, /* red        -- warm end */
    {.r = 0xFF, .g = 0x40, .b = 0x00},
    {.r = 0xFF, .g = 0x80, .b = 0x00}, /* orange */
    {.r = 0xFF, .g = 0xBF, .b = 0x00},
    {.r = 0xFF, .g = 0xFF, .b = 0x00}, /* yellow     -- crossover, via pale green */
    {.r = 0x00, .g = 0xAA, .b = 0xFF}, /* light blue */
    {.r = 0x00, .g = 0x55, .b = 0xFF},
    {.r = 0x00, .g = 0x00, .b = 0xFF}, /* blue       -- cold end */
    /* ---- mirror, so the wrap does not blend through purple ---- */
    {.r = 0x00, .g = 0x55, .b = 0xFF},
    {.r = 0x00, .g = 0xAA, .b = 0xFF},
    {.r = 0xFF, .g = 0xFF, .b = 0x00},
    {.r = 0xFF, .g = 0xBF, .b = 0x00},
    {.r = 0xFF, .g = 0x80, .b = 0x00},
    {.r = 0xFF, .g = 0x40, .b = 0x00},
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
