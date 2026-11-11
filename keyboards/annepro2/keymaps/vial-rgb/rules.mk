VIA_ENABLE = yes
VIAL_ENABLE = yes

LTO_ENABLE = yes

QMK_SETTINGS = yes
TAP_DANCE_ENABLE = yes
MOUSEKEY_ENABLE = yes
NKRO_ENABLE = yes
COMBO_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes
COMMAND_ENABLE = no
CONSOLE_ENABLE = no

# Unlike the plain `vial` keymap, RGB matrix is enabled here. It only fits
# because most rgb_matrix effects are disabled in animations.h -- see the
# comment there for the size budget.
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_CUSTOM_USER = yes
RGBLIGHT_ENABLE = no

# A note on which lighting protocol vial.json declares.
#
# Vial speaks two of them. "vialrgb" (VIALRGB_ENABLE = yes) is the native
# one: the firmware reports which effects it has using the stable IDs in
# quantum/vialrgb_effects.inc, so the GUI always labels them correctly no
# matter how many effects are compiled out. "qmk_rgb_matrix" is the
# VIA-compatible one, where the GUI assumes QMK's canonical effect order
# and addresses effects by raw index.
#
# This keymap declares qmk_rgb_matrix, and that has a visible cost: with 33
# effects compiled out the enum is compacted, so the indices no longer line
# up with what the GUI expects and the names in the dropdown are wrong. You
# pick effects by trying them.
#
# The reason is CUSTOM_RAINBOW. vialrgb_effects.inc is a closed table of
# known QMK effects, and a custom one has no ID in it -- qmk_id_to_vialrgb_id
# returns 0, which the GUI reads as "off". Under vialrgb the custom effect
# would be invisible, misreported, and replaced by the first thing you touch
# in the RGB tab.
#
# So: correct names without the custom effect, or the custom effect with
# scrambled names. Drop rgb_matrix_user.inc and switch to vialrgb if you
# ever decide the palette is not worth it.
