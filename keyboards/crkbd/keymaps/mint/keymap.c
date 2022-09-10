/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>

#ifdef OLED_ENABLE
char wpm_str[10];
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3(KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC, LCTL_T(KC_ESC), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_SFTENT, KC_LGUI, MO(2), KC_SPC, KC_ENT, MO(1), KC_RALT),
    [1] = LAYOUT_split_3x6_3(KC_TILD, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC, KC_ESC, KC_NO, KC_NO, KC_DEL, KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_PSCR, KC_RCTL, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_HOME, KC_END, KC_PGUP, KC_PGDN, KC_DEL, KC_NO, KC_LALT, MO(3), KC_SPC, KC_ENT, KC_TRNS, KC_RSFT),
    [2] = LAYOUT_split_3x6_3(KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, KC_LCTL, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_MINS, KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS, KC_LSFT, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_LGUI, KC_TRNS, KC_SPC, KC_ENT, MO(3), KC_RALT),
    [3] = LAYOUT_split_3x6_3(QK_BOOT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LGUI, KC_TRNS, KC_SPC, KC_ENT, KC_TRNS, KC_RALT)
};


#ifdef OLED_ENABLE
// WPM-responsive animation stuff here
#define IDLE_FRAMES 5
#define IDLE_SPEED 10  // below this wpm value your animation will idle
// #define PREP_FRAMES 1 // uncomment if >1
#define TAP_FRAMES 2
#define ANIM_WPM_LOWER 20  // above this wpm value typing animation to trigger
#define ANIM_FRAME_DURATION_MAX 450 // longest animation duration in ms
#define ANIM_FRAME_DURATION_MIN 100 // shortest animation duration in ms
#define IDLE_FRAME_DURATION 300  // how long each frame lasts in ms
#define ANIM_FRAME_RATIO 2.5 // how aggressively animation speeds up with wpm
// #define SLEEP_TIMER 60000 // should sleep after this period of 0 wpm, needs fixing
#define ANIM_SIZE 525  // number of bytes in array, minimize for adequate firmware size, max is 1024
#define MAX(x, y) (((x) > (y)) ? (x) : (y)) // Math.max macro

uint32_t curr_anim_duration = 0; // variable animation duration
uint32_t bongo_timer = 0;
uint8_t  current_idle_frame = 0;
// uint8_t current_prep_frame = 0; // uncomment if PREP_FRAMES >1
uint8_t current_tap_frame = 0;

// Code containing pixel art, contains:
// 5 idle frames, 1 prep frame, and 2 tap frames

// To make your own pixel art:
// save a png/jpeg of an 128x32 image (resource: https://www.pixilart.com/draw )
// follow this guide up to and including "CONVERT YOUR IMAGE" https://docs.splitkb.com/hc/en-us/articles/360013811280-How-do-I-convert-an-image-for-use-on-an-OLED-display-
// replace numbers in brackets with your own
// if you start getting errors when compiling make sure you didn't accedentally delete a bracket
void render_bongocat(void) {
    static const char PROGMEM idle[IDLE_FRAMES][ANIM_SIZE] = {
        {
        //Idle 1 - 128x32
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0,
        0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xc0, 0xc0, 0xc0, 0x80,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xcf, 0xff,
        0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
        0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30,
        0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d,
        0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26,
        0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f,
        0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
        //Idle 2 - 128x32
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xe0, 0xf0,
        0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0x9f, 0x9f, 0xff,
        0xff, 0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
        0x9f, 0x9f, 0x1e, 0x1e, 0x3f, 0x3d, 0x3d, 0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0xf9, 0xf9, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0, 0xc0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30,
        0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d,
        0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26,
        0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f,
        0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
        //Idle 3 - 128x32
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xe0,
        0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0x9f, 0x9f, 0xff,
        0xff, 0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xfe, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0xfe, 0x7e, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
        0x9f, 0x9f, 0x1e, 0x1e, 0x3f, 0x3d, 0x3d, 0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0xf9, 0xf9, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0, 0xc0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30,
        0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d,
        0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26,
        0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f,
        0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
        //Idle 4 - 128x32
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xe0,
        0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xcf, 0xcf, 0xff,
        0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xfe, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfc, 0x3c, 0x1c, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
        0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30,
        0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d,
        0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26,
        0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f,
        0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
        //Idle 5 - 128x32
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0,
        0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xc0, 0xc0, 0xc0, 0x80,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xcf, 0xff,
        0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x0f, 0x06, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
        0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30,
        0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d,
        0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26,
        0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f,
        0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        }
    };

    static const char PROGMEM prep[][ANIM_SIZE] = {
        {
        //Prepare - 128x32
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0,
        0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0xf0, 0xf8,
        0xdc, 0x36, 0x3e, 0xee, 0xfc, 0xe0, 0x9c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xcf, 0xff,
        0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0x7e, 0xbe, 0xbc, 0xbc, 0x7c, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x44, 0xc4, 0xc4, 0x68, 0x78,
        0x59, 0x89, 0x91, 0x91, 0xd3, 0xf3, 0xa3, 0x23, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
        0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0x81,
        0xfa, 0xff, 0xe6, 0xe7, 0xfd, 0xff, 0xfc, 0xf3, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30,
        0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x13, 0x12, 0x3c,
        0x2c, 0x26, 0x27, 0x45, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26,
        0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x34, 0x3c, 0x2c, 0x44, 0xc8, 0xc8,
        0xe9, 0xb9, 0x11, 0x11, 0x13, 0x93, 0xe3, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        }
    };

    static const char PROGMEM tap[TAP_FRAMES][ANIM_SIZE] = {
        {
        //Tap left - 128x32
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0,
        0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0xf0, 0xf8,
        0xdc, 0x36, 0x3e, 0xee, 0xfc, 0xe0, 0x9c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xcf, 0xff,
        0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
        0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x44, 0xc4, 0xc4, 0x68, 0x78,
        0x59, 0x89, 0x91, 0x91, 0xd3, 0xf3, 0xa3, 0x23, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
        0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00,
        0x3f, 0x1f, 0x07, 0x83, 0xc0, 0xe0, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30,
        0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x13, 0x12, 0x3c,
        0x2c, 0x26, 0x27, 0x45, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26,
        0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f,
        0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08,
        0x10, 0x10, 0x11, 0x11, 0x21, 0x23, 0x23, 0x21, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
        //Tap right - 128x32
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0,
        0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08,
        0x10, 0x10, 0x10, 0x10, 0xa0, 0xa0, 0x20, 0x20, 0x40, 0x46, 0x4f, 0x5f, 0x9f, 0x9c, 0x90, 0x80,
        0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xcf, 0xff,
        0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0x7e, 0xbe, 0xbc, 0xbc, 0x7c, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x07, 0x0f, 0x8f, 0xc7, 0xe3, 0x31, 0x38, 0x2c, 0x04, 0x64, 0xf8, 0xfe, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
        0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0x81,
        0xfa, 0xff, 0xe6, 0xe7, 0xfd, 0xff, 0xfc, 0xf3, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10, 0x30,
        0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d,
        0x2d, 0x25, 0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26,
        0xc7, 0xc5, 0x69, 0x39, 0x19, 0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x34, 0x3c, 0x2c, 0x44, 0xc8, 0xc8,
        0xe9, 0xb9, 0x11, 0x11, 0x13, 0x93, 0xe3, 0x63, 0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08,
        0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
    };

    // assumes 1 frame prep stage
    void animation_phase(void) {
        if (get_current_wpm() <= IDLE_SPEED) {
            current_idle_frame = (current_idle_frame + 1) % IDLE_FRAMES;
            oled_write_raw_P(idle[abs((IDLE_FRAMES - 1) - current_idle_frame)], ANIM_SIZE);
        }

        if (get_current_wpm() > IDLE_SPEED && get_current_wpm() < ANIM_WPM_LOWER) {
            // oled_write_raw_P(prep[abs((PREP_FRAMES-1)-current_prep_frame)], ANIM_SIZE); // uncomment if IDLE_FRAMES >1
            oled_write_raw_P(prep[0], ANIM_SIZE);  // remove if IDLE_FRAMES >1
        }

        if (get_current_wpm() >= ANIM_WPM_LOWER) {
            current_tap_frame = (current_tap_frame + 1) % TAP_FRAMES;
            oled_write_raw_P(tap[abs((TAP_FRAMES - 1) - current_tap_frame)], ANIM_SIZE);
        }
    }

    // variable animation duration. Don't want this value to get near zero as it'll bug out.
    curr_anim_duration = MAX(ANIM_FRAME_DURATION_MIN, ANIM_FRAME_DURATION_MAX - ANIM_FRAME_RATIO * get_current_wpm());

    if (get_current_wpm() > ANIM_WPM_LOWER) {
        oled_on();  // not essential but turns on animation OLED with any alpha keypress

        if (timer_elapsed32(bongo_timer) > curr_anim_duration) {
            bongo_timer = timer_read32();
            animation_phase();
        }
    } else {
        if (timer_elapsed32(bongo_timer) > IDLE_FRAME_DURATION) {
            bongo_timer = timer_read32();
            animation_phase();
        }
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  }
  return rotation;
}

#define L_BASE 0
#define L_LOWER 2
#define L_RAISE 4
#define L_ADJUST 8

void oled_render_layer_state(void) {
    // oled_write_P(PSTR("Layer:"), false);
    switch (layer_state) {
        case L_BASE:
            oled_write_ln_P(PSTR("Default"), false);
            break;
        case L_LOWER:
            oled_write_ln_P(PSTR("Lower"), false);
            break;
        case L_RAISE:
            oled_write_ln_P(PSTR("Raise"), false);
            break;
        case L_ADJUST:
        case L_ADJUST|L_LOWER:
        case L_ADJUST|L_RAISE:
        case L_ADJUST|L_LOWER|L_RAISE:
            oled_write_ln_P(PSTR("Adjust"), false);
            break;
    }
}


char keylog_str[24] = {};

const char code_to_name[60] = {
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

void set_keylog(uint16_t keycode, keyrecord_t *record) {
  char name = ' ';
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) ||
        (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) { keycode = keycode & 0xFF; }
  if (keycode < 60) {
    name = code_to_name[keycode];
  }

  // update keylog
  snprintf(keylog_str, sizeof(keylog_str), "%dx%d, k%2d : %c",
           record->event.key.row, record->event.key.col,
           keycode, name);
}

void oled_render_keylog(void) {
    oled_write(keylog_str, false);
}

void render_bootmagic_status(bool status) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
    if (status) {
        oled_write_ln_P(logo[0][0], false);
        oled_write_ln_P(logo[0][1], false);
    } else {
        oled_write_ln_P(logo[1][0], false);
        oled_write_ln_P(logo[1][1], false);
    }
}

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    oled_write_P(crkbd_logo, false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        render_bongocat();

        oled_set_cursor(12, 0);
        oled_render_layer_state();

        oled_set_cursor(1, 10);
        sprintf(wpm_str, "WPM:%03d", get_current_wpm());  // edit the string to change wwhat shows up, edit %03d to change how many digits show up
        oled_write(wpm_str, false);                       // writes wpm on top left corner of string
    } else {
        oled_render_logo();
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    set_keylog(keycode, record);
  }
  return true;
}

#endif // OLED_ENABLE
