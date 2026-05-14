/*
 * HID Report Definitions for BLE HID Service
 *
 * Minimal standalone HID types and report descriptor macros extracted from
 * TinyUSB (MIT License, Copyright (c) 2019 Ha Thach, tinyusb.org).
 *
 * This file replaces the TinyUSB dependency for BLE HID on nRF54L (no USB).
 */

#ifndef HID_REPORT_DEFS_H_
#define HID_REPORT_DEFS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------+
// HID Report Types
//--------------------------------------------------------------------+

/// Standard HID keyboard report (8 bytes)
typedef struct __attribute__((packed)) {
  uint8_t modifier;
  uint8_t reserved;
  uint8_t keycode[6];
} hid_keyboard_report_t;

/// Standard HID mouse report
typedef struct __attribute__((packed)) {
  uint8_t buttons;
  int8_t  x;
  int8_t  y;
  int8_t  wheel;
  int8_t  pan;
} hid_mouse_report_t;

/// Standard HID gamepad report
typedef struct __attribute__((packed)) {
  int8_t  x;
  int8_t  y;
  int8_t  z;
  int8_t  rz;
  int8_t  rx;
  int8_t  ry;
  uint8_t hat;
  uint32_t buttons;
} hid_gamepad_report_t;

//--------------------------------------------------------------------+
// HID Keycode constants
//--------------------------------------------------------------------+
enum {
  HID_KEY_NONE               = 0x00,
  HID_KEY_A                  = 0x04,
  HID_KEY_B                  = 0x05,
  HID_KEY_C                  = 0x06,
  HID_KEY_D                  = 0x07,
  HID_KEY_E                  = 0x08,
  HID_KEY_F                  = 0x09,
  HID_KEY_G                  = 0x0A,
  HID_KEY_H                  = 0x0B,
  HID_KEY_I                  = 0x0C,
  HID_KEY_J                  = 0x0D,
  HID_KEY_K                  = 0x0E,
  HID_KEY_L                  = 0x0F,
  HID_KEY_M                  = 0x10,
  HID_KEY_N                  = 0x11,
  HID_KEY_O                  = 0x12,
  HID_KEY_P                  = 0x13,
  HID_KEY_Q                  = 0x14,
  HID_KEY_R                  = 0x15,
  HID_KEY_S                  = 0x16,
  HID_KEY_T                  = 0x17,
  HID_KEY_U                  = 0x18,
  HID_KEY_V                  = 0x19,
  HID_KEY_W                  = 0x1A,
  HID_KEY_X                  = 0x1B,
  HID_KEY_Y                  = 0x1C,
  HID_KEY_Z                  = 0x1D,
  HID_KEY_1                  = 0x1E,
  HID_KEY_2                  = 0x1F,
  HID_KEY_3                  = 0x20,
  HID_KEY_4                  = 0x21,
  HID_KEY_5                  = 0x22,
  HID_KEY_6                  = 0x23,
  HID_KEY_7                  = 0x24,
  HID_KEY_8                  = 0x25,
  HID_KEY_9                  = 0x26,
  HID_KEY_0                  = 0x27,
  HID_KEY_ENTER              = 0x28,
  HID_KEY_ESCAPE             = 0x29,
  HID_KEY_BACKSPACE          = 0x2A,
  HID_KEY_TAB                = 0x2B,
  HID_KEY_SPACE              = 0x2C,
  HID_KEY_MINUS              = 0x2D,
  HID_KEY_EQUAL              = 0x2E,
  HID_KEY_BRACKET_LEFT       = 0x2F,
  HID_KEY_BRACKET_RIGHT      = 0x30,
  HID_KEY_BACKSLASH          = 0x31,
  HID_KEY_EUROPE_1           = 0x32,
  HID_KEY_SEMICOLON          = 0x33,
  HID_KEY_APOSTROPHE         = 0x34,
  HID_KEY_GRAVE              = 0x35,
  HID_KEY_COMMA              = 0x36,
  HID_KEY_PERIOD             = 0x37,
  HID_KEY_SLASH              = 0x38,
  HID_KEY_CAPS_LOCK          = 0x39,
  HID_KEY_F1                 = 0x3A,
  HID_KEY_F2                 = 0x3B,
  HID_KEY_F3                 = 0x3C,
  HID_KEY_F4                 = 0x3D,
  HID_KEY_F5                 = 0x3E,
  HID_KEY_F6                 = 0x3F,
  HID_KEY_F7                 = 0x40,
  HID_KEY_F8                 = 0x41,
  HID_KEY_F9                 = 0x42,
  HID_KEY_F10                = 0x43,
  HID_KEY_F11                = 0x44,
  HID_KEY_F12                = 0x45,
  HID_KEY_DELETE             = 0x4C,
  HID_KEY_ARROW_RIGHT        = 0x4F,
  HID_KEY_ARROW_LEFT         = 0x50,
  HID_KEY_ARROW_DOWN         = 0x51,
  HID_KEY_ARROW_UP           = 0x52,
  HID_KEY_CONTROL_LEFT       = 0xE0,
  HID_KEY_SHIFT_LEFT         = 0xE1,
  HID_KEY_ALT_LEFT           = 0xE2,
  HID_KEY_GUI_LEFT           = 0xE3,
  HID_KEY_CONTROL_RIGHT      = 0xE4,
  HID_KEY_SHIFT_RIGHT        = 0xE5,
  HID_KEY_ALT_RIGHT          = 0xE6,
  HID_KEY_GUI_RIGHT          = 0xE7,
};

// Keyboard modifier bits
enum {
  KEYBOARD_MODIFIER_LEFTCTRL   = (1 << 0),
  KEYBOARD_MODIFIER_LEFTSHIFT  = (1 << 1),
  KEYBOARD_MODIFIER_LEFTALT    = (1 << 2),
  KEYBOARD_MODIFIER_LEFTGUI    = (1 << 3),
  KEYBOARD_MODIFIER_RIGHTCTRL  = (1 << 4),
  KEYBOARD_MODIFIER_RIGHTSHIFT = (1 << 5),
  KEYBOARD_MODIFIER_RIGHTALT   = (1 << 6),
  KEYBOARD_MODIFIER_RIGHTGUI   = (1 << 7),
};

// Mouse button bits
enum {
  MOUSE_BUTTON_LEFT     = (1 << 0),
  MOUSE_BUTTON_RIGHT    = (1 << 1),
  MOUSE_BUTTON_MIDDLE   = (1 << 2),
  MOUSE_BUTTON_BACKWARD = (1 << 3),
  MOUSE_BUTTON_FORWARD  = (1 << 4),
};

// Gamepad hat directions
enum {
  GAMEPAD_HAT_CENTERED  = 0,
  GAMEPAD_HAT_UP        = 1,
  GAMEPAD_HAT_UP_RIGHT  = 2,
  GAMEPAD_HAT_RIGHT     = 3,
  GAMEPAD_HAT_DOWN_RIGHT = 4,
  GAMEPAD_HAT_DOWN      = 5,
  GAMEPAD_HAT_DOWN_LEFT = 6,
  GAMEPAD_HAT_LEFT      = 7,
  GAMEPAD_HAT_UP_LEFT   = 8,
};

//--------------------------------------------------------------------+
// HID Report Descriptor Macros
//--------------------------------------------------------------------+

// Basic HID items
// HID_REPORT_ID intentionally carries a trailing comma so it can be
// passed as the __VA_ARGS__ slot of TUD_HID_REPORT_DESC_*(...) which
// places it between two other items without an explicit separator.
// Mirrors the upstream TinyUSB convention.
#define HID_REPORT_ID(id)             0x85, id,
#define HID_USAGE_PAGE(x)             0x05, x
#define HID_USAGE_PAGE_N(x, n)        0x06, (x & 0xFF), ((x >> 8) & 0xFF)
#define HID_USAGE(x)                  0x09, x
#define HID_USAGE_N(x, n)             0x0A, (x & 0xFF), ((x >> 8) & 0xFF)
#define HID_USAGE_MIN(x)              0x19, x
#define HID_USAGE_MAX(x)              0x29, x
#define HID_USAGE_MIN_N(x, n)         0x1A, (x & 0xFF), ((x >> 8) & 0xFF)
#define HID_USAGE_MAX_N(x, n)         0x2A, (x & 0xFF), ((x >> 8) & 0xFF)
#define HID_LOGICAL_MIN(x)            0x15, x
#define HID_LOGICAL_MAX(x)            0x25, x
#define HID_LOGICAL_MIN_N(x, n)       0x16, (x & 0xFF), ((x >> 8) & 0xFF)
#define HID_LOGICAL_MAX_N(x, n)       0x26, (x & 0xFF), ((x >> 8) & 0xFF)
#define HID_REPORT_SIZE(x)            0x75, x
#define HID_REPORT_COUNT(x)           0x95, x
#define HID_COLLECTION(x)             0xA1, x
#define HID_END_COLLECTION            0xC0
#define HID_INPUT(x)                  0x81, x
#define HID_OUTPUT(x)                 0x91, x
#define HID_FEATURE(x)                0xB1, x
#define HID_UNIT(x)                   0x65, x
#define HID_UNIT_EXPONENT(x)          0x55, x

// Usage page values
#define HID_USAGE_PAGE_DESKTOP        0x01
#define HID_USAGE_PAGE_CONSUMER       0x0C
#define HID_USAGE_PAGE_BUTTON         0x09
#define HID_USAGE_PAGE_KEYBOARD       0x07
#define HID_USAGE_PAGE_LED            0x08

// Desktop usage values
#define HID_USAGE_DESKTOP_POINTER     0x01
#define HID_USAGE_DESKTOP_MOUSE       0x02
#define HID_USAGE_DESKTOP_JOYSTICK    0x04
#define HID_USAGE_DESKTOP_GAMEPAD     0x05
#define HID_USAGE_DESKTOP_KEYBOARD    0x06
#define HID_USAGE_DESKTOP_X           0x30
#define HID_USAGE_DESKTOP_Y           0x31
#define HID_USAGE_DESKTOP_Z           0x32
#define HID_USAGE_DESKTOP_RX          0x33
#define HID_USAGE_DESKTOP_RY          0x34
#define HID_USAGE_DESKTOP_RZ          0x35
#define HID_USAGE_DESKTOP_WHEEL       0x38
#define HID_USAGE_DESKTOP_HAT_SWITCH  0x39

// Consumer usage
#define HID_USAGE_CONSUMER_CONTROL    0x01

// Collection types
#define HID_COLLECTION_APPLICATION    0x01
#define HID_COLLECTION_PHYSICAL       0x00
#define HID_COLLECTION_LOGICAL        0x02

// Input/Output/Feature flags
#define HID_DATA                      (0 << 0)
#define HID_CONSTANT                  (1 << 0)
#define HID_ARRAY                     (0 << 1)
#define HID_VARIABLE                  (1 << 1)
#define HID_ABSOLUTE                  (0 << 2)
#define HID_RELATIVE                  (1 << 2)

//--------------------------------------------------------------------+
// TUD HID Report Descriptor Templates
//--------------------------------------------------------------------+

// Standard Keyboard Report Descriptor
#define TUD_HID_REPORT_DESC_KEYBOARD(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     ), \
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD ), \
  HID_COLLECTION ( HID_COLLECTION_APPLICATION  ), \
    __VA_ARGS__ \
    /* Modifier keys */ \
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_KEYBOARD ), \
    HID_USAGE_MIN   ( 0xE0                    ), \
    HID_USAGE_MAX   ( 0xE7                    ), \
    HID_LOGICAL_MIN ( 0                       ), \
    HID_LOGICAL_MAX ( 1                       ), \
    HID_REPORT_SIZE ( 1                       ), \
    HID_REPORT_COUNT( 8                       ), \
    HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
    /* Reserved byte */ \
    HID_REPORT_COUNT( 1                       ), \
    HID_REPORT_SIZE ( 8                       ), \
    HID_INPUT       ( HID_CONSTANT            ), \
    /* LED output (5 bits + 3 padding) */ \
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED      ), \
    HID_USAGE_MIN   ( 1                       ), \
    HID_USAGE_MAX   ( 5                       ), \
    HID_REPORT_COUNT( 5                       ), \
    HID_REPORT_SIZE ( 1                       ), \
    HID_OUTPUT      ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
    HID_REPORT_COUNT( 1                       ), \
    HID_REPORT_SIZE ( 3                       ), \
    HID_OUTPUT      ( HID_CONSTANT            ), \
    /* Keycodes (6 bytes) */ \
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_KEYBOARD ), \
    HID_USAGE_MIN   ( 0                       ), \
    HID_USAGE_MAX   ( 0x65                    ), \
    HID_LOGICAL_MIN ( 0                       ), \
    HID_LOGICAL_MAX ( 0x65                    ), \
    HID_REPORT_COUNT( 6                       ), \
    HID_REPORT_SIZE ( 8                       ), \
    HID_INPUT       ( HID_DATA | HID_ARRAY    ), \
  HID_END_COLLECTION

// Standard Mouse Report Descriptor
#define TUD_HID_REPORT_DESC_MOUSE(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     ), \
  HID_USAGE      ( HID_USAGE_DESKTOP_MOUSE    ), \
  HID_COLLECTION ( HID_COLLECTION_APPLICATION  ), \
    __VA_ARGS__ \
    HID_USAGE      ( HID_USAGE_DESKTOP_POINTER), \
    HID_COLLECTION ( HID_COLLECTION_PHYSICAL   ), \
      /* Buttons (5 bits + 3 padding) */ \
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_BUTTON  ), \
      HID_USAGE_MIN   ( 1                      ), \
      HID_USAGE_MAX   ( 5                      ), \
      HID_LOGICAL_MIN ( 0                      ), \
      HID_LOGICAL_MAX ( 1                      ), \
      HID_REPORT_COUNT( 5                      ), \
      HID_REPORT_SIZE ( 1                      ), \
      HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
      HID_REPORT_COUNT( 1                      ), \
      HID_REPORT_SIZE ( 3                      ), \
      HID_INPUT       ( HID_CONSTANT           ), \
      /* X, Y (relative) */ \
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_DESKTOP ), \
      HID_USAGE       ( HID_USAGE_DESKTOP_X    ), \
      HID_USAGE       ( HID_USAGE_DESKTOP_Y    ), \
      HID_LOGICAL_MIN ( 0x81                   ), \
      HID_LOGICAL_MAX ( 0x7F                   ), \
      HID_REPORT_SIZE ( 8                      ), \
      HID_REPORT_COUNT( 2                      ), \
      HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ), \
      /* Wheel + Pan (relative) */ \
      HID_USAGE       ( HID_USAGE_DESKTOP_WHEEL), \
      HID_LOGICAL_MIN ( 0x81                   ), \
      HID_LOGICAL_MAX ( 0x7F                   ), \
      HID_REPORT_SIZE ( 8                      ), \
      HID_REPORT_COUNT( 1                      ), \
      HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ), \
      HID_USAGE_PAGE_N( 0x0C, 2               ), \
      HID_USAGE_N     ( 0x0238, 2             ), \
      HID_LOGICAL_MIN ( 0x81                   ), \
      HID_LOGICAL_MAX ( 0x7F                   ), \
      HID_REPORT_SIZE ( 8                      ), \
      HID_REPORT_COUNT( 1                      ), \
      HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ), \
    HID_END_COLLECTION, \
  HID_END_COLLECTION

// Consumer Control Report Descriptor
#define TUD_HID_REPORT_DESC_CONSUMER(...) \
  HID_USAGE_PAGE_N ( 0x0C, 2                  ), \
  HID_USAGE        ( HID_USAGE_CONSUMER_CONTROL), \
  HID_COLLECTION   ( HID_COLLECTION_APPLICATION), \
    __VA_ARGS__ \
    HID_LOGICAL_MIN  ( 0x00                    ), \
    HID_LOGICAL_MAX_N( 0x03FF, 2               ), \
    HID_USAGE_MIN    ( 0x00                    ), \
    HID_USAGE_MAX_N  ( 0x03FF, 2               ), \
    HID_REPORT_COUNT ( 1                       ), \
    HID_REPORT_SIZE  ( 16                      ), \
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE ), \
  HID_END_COLLECTION

// Gamepad Report Descriptor
#define TUD_HID_REPORT_DESC_GAMEPAD(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     ), \
  HID_USAGE      ( HID_USAGE_DESKTOP_GAMEPAD  ), \
  HID_COLLECTION ( HID_COLLECTION_APPLICATION  ), \
    __VA_ARGS__ \
    /* X, Y, Z, Rz (8 bit) */ \
    HID_USAGE_PAGE   ( HID_USAGE_PAGE_DESKTOP ), \
    HID_USAGE        ( HID_USAGE_DESKTOP_X    ), \
    HID_USAGE        ( HID_USAGE_DESKTOP_Y    ), \
    HID_USAGE        ( HID_USAGE_DESKTOP_Z    ), \
    HID_USAGE        ( HID_USAGE_DESKTOP_RZ   ), \
    HID_LOGICAL_MIN  ( 0x81                   ), \
    HID_LOGICAL_MAX  ( 0x7F                   ), \
    HID_REPORT_COUNT ( 4                      ), \
    HID_REPORT_SIZE  ( 8                      ), \
    HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
    /* Rx, Ry (8 bit) */ \
    HID_USAGE        ( HID_USAGE_DESKTOP_RX   ), \
    HID_USAGE        ( HID_USAGE_DESKTOP_RY   ), \
    HID_LOGICAL_MIN  ( 0x81                   ), \
    HID_LOGICAL_MAX  ( 0x7F                   ), \
    HID_REPORT_COUNT ( 2                      ), \
    HID_REPORT_SIZE  ( 8                      ), \
    HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
    /* Hat switch (4 bits + 4 padding) */ \
    HID_USAGE_PAGE   ( HID_USAGE_PAGE_DESKTOP ), \
    HID_USAGE        ( HID_USAGE_DESKTOP_HAT_SWITCH ), \
    HID_LOGICAL_MIN  ( 1                      ), \
    HID_LOGICAL_MAX  ( 8                      ), \
    HID_REPORT_COUNT ( 1                      ), \
    HID_REPORT_SIZE  ( 8                      ), \
    HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
    /* Buttons (32 bits) */ \
    HID_USAGE_PAGE   ( HID_USAGE_PAGE_BUTTON  ), \
    HID_USAGE_MIN    ( 1                      ), \
    HID_USAGE_MAX    ( 32                     ), \
    HID_LOGICAL_MIN  ( 0                      ), \
    HID_LOGICAL_MAX  ( 1                      ), \
    HID_REPORT_COUNT ( 32                     ), \
    HID_REPORT_SIZE  ( 1                      ), \
    HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
  HID_END_COLLECTION

//--------------------------------------------------------------------+
// ASCII to Keycode mapping (US keyboard layout)
//--------------------------------------------------------------------+

#define HID_ASCII_TO_KEYCODE \
    {0, 0                     }, /* 0x00 Null      */ \
    {0, 0                     }, /* 0x01           */ \
    {0, 0                     }, /* 0x02           */ \
    {0, 0                     }, /* 0x03           */ \
    {0, 0                     }, /* 0x04           */ \
    {0, 0                     }, /* 0x05           */ \
    {0, 0                     }, /* 0x06           */ \
    {0, 0                     }, /* 0x07           */ \
    {0, HID_KEY_BACKSPACE     }, /* 0x08 Backspace */ \
    {0, HID_KEY_TAB           }, /* 0x09 Tab       */ \
    {0, HID_KEY_ENTER         }, /* 0x0A Line Feed */ \
    {0, 0                     }, /* 0x0B           */ \
    {0, 0                     }, /* 0x0C           */ \
    {0, HID_KEY_ENTER         }, /* 0x0D CR        */ \
    {0, 0                     }, /* 0x0E           */ \
    {0, 0                     }, /* 0x0F           */ \
    {0, 0                     }, /* 0x10           */ \
    {0, 0                     }, /* 0x11           */ \
    {0, 0                     }, /* 0x12           */ \
    {0, 0                     }, /* 0x13           */ \
    {0, 0                     }, /* 0x14           */ \
    {0, 0                     }, /* 0x15           */ \
    {0, 0                     }, /* 0x16           */ \
    {0, 0                     }, /* 0x17           */ \
    {0, 0                     }, /* 0x18           */ \
    {0, 0                     }, /* 0x19           */ \
    {0, 0                     }, /* 0x1A           */ \
    {0, HID_KEY_ESCAPE        }, /* 0x1B Escape    */ \
    {0, 0                     }, /* 0x1C           */ \
    {0, 0                     }, /* 0x1D           */ \
    {0, 0                     }, /* 0x1E           */ \
    {0, 0                     }, /* 0x1F           */ \
    {0, HID_KEY_SPACE         }, /* 0x20 Space     */ \
    {1, HID_KEY_1             }, /* 0x21 !         */ \
    {1, HID_KEY_APOSTROPHE    }, /* 0x22 "         */ \
    {1, HID_KEY_3             }, /* 0x23 #         */ \
    {1, HID_KEY_4             }, /* 0x24 $         */ \
    {1, HID_KEY_5             }, /* 0x25 %         */ \
    {1, HID_KEY_7             }, /* 0x26 &         */ \
    {0, HID_KEY_APOSTROPHE    }, /* 0x27 '         */ \
    {1, HID_KEY_9             }, /* 0x28 (         */ \
    {1, HID_KEY_0             }, /* 0x29 )         */ \
    {1, HID_KEY_8             }, /* 0x2A *         */ \
    {1, HID_KEY_EQUAL         }, /* 0x2B +         */ \
    {0, HID_KEY_COMMA         }, /* 0x2C ,         */ \
    {0, HID_KEY_MINUS         }, /* 0x2D -         */ \
    {0, HID_KEY_PERIOD        }, /* 0x2E .         */ \
    {0, HID_KEY_SLASH         }, /* 0x2F /         */ \
    {0, HID_KEY_0             }, /* 0x30 0         */ \
    {0, HID_KEY_1             }, /* 0x31 1         */ \
    {0, HID_KEY_2             }, /* 0x32 2         */ \
    {0, HID_KEY_3             }, /* 0x33 3         */ \
    {0, HID_KEY_4             }, /* 0x34 4         */ \
    {0, HID_KEY_5             }, /* 0x35 5         */ \
    {0, HID_KEY_6             }, /* 0x36 6         */ \
    {0, HID_KEY_7             }, /* 0x37 7         */ \
    {0, HID_KEY_8             }, /* 0x38 8         */ \
    {0, HID_KEY_9             }, /* 0x39 9         */ \
    {1, HID_KEY_SEMICOLON     }, /* 0x3A :         */ \
    {0, HID_KEY_SEMICOLON     }, /* 0x3B ;         */ \
    {1, HID_KEY_COMMA         }, /* 0x3C <         */ \
    {0, HID_KEY_EQUAL         }, /* 0x3D =         */ \
    {1, HID_KEY_PERIOD        }, /* 0x3E >         */ \
    {1, HID_KEY_SLASH         }, /* 0x3F ?         */ \
    {1, HID_KEY_2             }, /* 0x40 @         */ \
    {1, HID_KEY_A             }, /* 0x41 A         */ \
    {1, HID_KEY_B             }, /* 0x42 B         */ \
    {1, HID_KEY_C             }, /* 0x43 C         */ \
    {1, HID_KEY_D             }, /* 0x44 D         */ \
    {1, HID_KEY_E             }, /* 0x45 E         */ \
    {1, HID_KEY_F             }, /* 0x46 F         */ \
    {1, HID_KEY_G             }, /* 0x47 G         */ \
    {1, HID_KEY_H             }, /* 0x48 H         */ \
    {1, HID_KEY_I             }, /* 0x49 I         */ \
    {1, HID_KEY_J             }, /* 0x4A J         */ \
    {1, HID_KEY_K             }, /* 0x4B K         */ \
    {1, HID_KEY_L             }, /* 0x4C L         */ \
    {1, HID_KEY_M             }, /* 0x4D M         */ \
    {1, HID_KEY_N             }, /* 0x4E N         */ \
    {1, HID_KEY_O             }, /* 0x4F O         */ \
    {1, HID_KEY_P             }, /* 0x50 P         */ \
    {1, HID_KEY_Q             }, /* 0x51 Q         */ \
    {1, HID_KEY_R             }, /* 0x52 R         */ \
    {1, HID_KEY_S             }, /* 0x53 S         */ \
    {1, HID_KEY_T             }, /* 0x54 T         */ \
    {1, HID_KEY_U             }, /* 0x55 U         */ \
    {1, HID_KEY_V             }, /* 0x56 V         */ \
    {1, HID_KEY_W             }, /* 0x57 W         */ \
    {1, HID_KEY_X             }, /* 0x58 X         */ \
    {1, HID_KEY_Y             }, /* 0x59 Y         */ \
    {1, HID_KEY_Z             }, /* 0x5A Z         */ \
    {0, HID_KEY_BRACKET_LEFT  }, /* 0x5B [         */ \
    {0, HID_KEY_BACKSLASH     }, /* 0x5C \         */ \
    {0, HID_KEY_BRACKET_RIGHT }, /* 0x5D ]         */ \
    {1, HID_KEY_6             }, /* 0x5E ^         */ \
    {1, HID_KEY_MINUS         }, /* 0x5F _         */ \
    {0, HID_KEY_GRAVE         }, /* 0x60 `         */ \
    {0, HID_KEY_A             }, /* 0x61 a         */ \
    {0, HID_KEY_B             }, /* 0x62 b         */ \
    {0, HID_KEY_C             }, /* 0x63 c         */ \
    {0, HID_KEY_D             }, /* 0x64 d         */ \
    {0, HID_KEY_E             }, /* 0x65 e         */ \
    {0, HID_KEY_F             }, /* 0x66 f         */ \
    {0, HID_KEY_G             }, /* 0x67 g         */ \
    {0, HID_KEY_H             }, /* 0x68 h         */ \
    {0, HID_KEY_I             }, /* 0x69 i         */ \
    {0, HID_KEY_J             }, /* 0x6A j         */ \
    {0, HID_KEY_K             }, /* 0x6B k         */ \
    {0, HID_KEY_L             }, /* 0x6C l         */ \
    {0, HID_KEY_M             }, /* 0x6D m         */ \
    {0, HID_KEY_N             }, /* 0x6E n         */ \
    {0, HID_KEY_O             }, /* 0x6F o         */ \
    {0, HID_KEY_P             }, /* 0x70 p         */ \
    {0, HID_KEY_Q             }, /* 0x71 q         */ \
    {0, HID_KEY_R             }, /* 0x72 r         */ \
    {0, HID_KEY_S             }, /* 0x73 s         */ \
    {0, HID_KEY_T             }, /* 0x74 t         */ \
    {0, HID_KEY_U             }, /* 0x75 u         */ \
    {0, HID_KEY_V             }, /* 0x76 v         */ \
    {0, HID_KEY_W             }, /* 0x77 w         */ \
    {0, HID_KEY_X             }, /* 0x78 x         */ \
    {0, HID_KEY_Y             }, /* 0x79 y         */ \
    {0, HID_KEY_Z             }, /* 0x7A z         */ \
    {1, HID_KEY_BRACKET_LEFT  }, /* 0x7B {         */ \
    {1, HID_KEY_BACKSLASH     }, /* 0x7C |         */ \
    {1, HID_KEY_BRACKET_RIGHT }, /* 0x7D }         */ \
    {1, HID_KEY_GRAVE         }, /* 0x7E ~         */ \
    {0, HID_KEY_DELETE        }  /* 0x7F Delete    */

// Keycode to ASCII mapping (reverse of above, for convenience)
#define HID_KEYCODE_TO_ASCII \
    {0     , 0      }, /* 0x00 */ \
    {0     , 0      }, /* 0x01 */ \
    {0     , 0      }, /* 0x02 */ \
    {0     , 0      }, /* 0x03 */ \
    {'a'   , 'A'    }, /* 0x04 */ \
    {'b'   , 'B'    }, /* 0x05 */ \
    {'c'   , 'C'    }, /* 0x06 */ \
    {'d'   , 'D'    }, /* 0x07 */ \
    {'e'   , 'E'    }, /* 0x08 */ \
    {'f'   , 'F'    }, /* 0x09 */ \
    {'g'   , 'G'    }, /* 0x0A */ \
    {'h'   , 'H'    }, /* 0x0B */ \
    {'i'   , 'I'    }, /* 0x0C */ \
    {'j'   , 'J'    }, /* 0x0D */ \
    {'k'   , 'K'    }, /* 0x0E */ \
    {'l'   , 'L'    }, /* 0x0F */ \
    {'m'   , 'M'    }, /* 0x10 */ \
    {'n'   , 'N'    }, /* 0x11 */ \
    {'o'   , 'O'    }, /* 0x12 */ \
    {'p'   , 'P'    }, /* 0x13 */ \
    {'q'   , 'Q'    }, /* 0x14 */ \
    {'r'   , 'R'    }, /* 0x15 */ \
    {'s'   , 'S'    }, /* 0x16 */ \
    {'t'   , 'T'    }, /* 0x17 */ \
    {'u'   , 'U'    }, /* 0x18 */ \
    {'v'   , 'V'    }, /* 0x19 */ \
    {'w'   , 'W'    }, /* 0x1A */ \
    {'x'   , 'X'    }, /* 0x1B */ \
    {'y'   , 'Y'    }, /* 0x1C */ \
    {'z'   , 'Z'    }, /* 0x1D */ \
    {'1'   , '!'    }, /* 0x1E */ \
    {'2'   , '@'    }, /* 0x1F */ \
    {'3'   , '#'    }, /* 0x20 */ \
    {'4'   , '$'    }, /* 0x21 */ \
    {'5'   , '%'    }, /* 0x22 */ \
    {'6'   , '^'    }, /* 0x23 */ \
    {'7'   , '&'    }, /* 0x24 */ \
    {'8'   , '*'    }, /* 0x25 */ \
    {'9'   , '('    }, /* 0x26 */ \
    {'0'   , ')'    }, /* 0x27 */ \
    {'\r'  , '\r'   }, /* 0x28 Enter   */ \
    {'\x1b', '\x1b' }, /* 0x29 Escape  */ \
    {'\b'  , '\b'   }, /* 0x2A Bkspace */ \
    {'\t'  , '\t'   }, /* 0x2B Tab     */ \
    {' '   , ' '    }, /* 0x2C Space   */ \
    {'-'   , '_'    }, /* 0x2D */ \
    {'='   , '+'    }, /* 0x2E */ \
    {'['   , '{'    }, /* 0x2F */ \
    {']'   , '}'    }, /* 0x30 */ \
    {'\\'  , '|'    }, /* 0x31 */ \
    {'#'   , '~'    }, /* 0x32 */ \
    {';'   , ':'    }, /* 0x33 */ \
    {'\''  , '"'    }, /* 0x34 */ \
    {'`'   , '~'    }, /* 0x35 */ \
    {','   , '<'    }, /* 0x36 */ \
    {'.'   , '>'    }, /* 0x37 */ \
    {'/'   , '?'    }, /* 0x38 */ \
    {0     , 0      }, /* 0x39 Caps */ \
    {0     , 0      }, /* 0x3A F1   */ \
    {0     , 0      }, /* 0x3B F2   */ \
    {0     , 0      }, /* 0x3C F3   */ \
    {0     , 0      }, /* 0x3D F4   */ \
    {0     , 0      }, /* 0x3E F5   */ \
    {0     , 0      }, /* 0x3F F6   */ \
    {0     , 0      }, /* 0x40 F7   */ \
    {0     , 0      }, /* 0x41 F8   */ \
    {0     , 0      }, /* 0x42 F9   */ \
    {0     , 0      }, /* 0x43 F10  */ \
    {0     , 0      }, /* 0x44 F11  */ \
    {0     , 0      }, /* 0x45 F12  */ \
    {0     , 0      }, /* 0x46 PrtSc  */ \
    {0     , 0      }, /* 0x47 ScrLk  */ \
    {0     , 0      }, /* 0x48 Pause  */ \
    {0     , 0      }, /* 0x49 Insert */ \
    {0     , 0      }, /* 0x4A Home   */ \
    {0     , 0      }, /* 0x4B PgUp   */ \
    {0     , 0      }, /* 0x4C Del    */ \
    {0     , 0      }, /* 0x4D End    */ \
    {0     , 0      }, /* 0x4E PgDn   */ \
    {0     , 0      }, /* 0x4F Right  */ \
    {0     , 0      }, /* 0x50 Left   */ \
    {0     , 0      }, /* 0x51 Down   */ \
    {0     , 0      }, /* 0x52 Up     */ \
    {0     , 0      }, /* 0x53 NumLk  */ \
    {'/'   , '/'    }, /* 0x54 KP /   */ \
    {'*'   , '*'    }, /* 0x55 KP *   */ \
    {'-'   , '-'    }, /* 0x56 KP -   */ \
    {'+'   , '+'    }, /* 0x57 KP +   */ \
    {'\r'  , '\r'   }, /* 0x58 KP Ent */ \
    {'1'   , 0      }, /* 0x59 KP 1   */ \
    {'2'   , 0      }, /* 0x5A KP 2   */ \
    {'3'   , 0      }, /* 0x5B KP 3   */ \
    {'4'   , 0      }, /* 0x5C KP 4   */ \
    {'5'   , 0      }, /* 0x5D KP 5   */ \
    {'6'   , 0      }, /* 0x5E KP 6   */ \
    {'7'   , 0      }, /* 0x5F KP 7   */ \
    {'8'   , 0      }, /* 0x60 KP 8   */ \
    {'9'   , 0      }, /* 0x61 KP 9   */ \
    {'0'   , 0      }, /* 0x62 KP 0   */ \
    {'.'   , 0      }, /* 0x63 KP .   */ \
    {0     , 0      }, /* 0x64        */ \
    {0     , 0      }, /* 0x65 App    */ \
    {0     , 0      }, /* 0x66 Power  */ \
    {'='   , 0      }, /* 0x67 KP =   */ \
    {0     , 0      }, /* 0x68 F13    */ \
    {0     , 0      }, /* 0x69 F14    */ \
    {0     , 0      }, /* 0x6A F15    */ \
    {0     , 0      }, /* 0x6B F16    */ \
    {0     , 0      }, /* 0x6C F17    */ \
    {0     , 0      }, /* 0x6D F18    */ \
    {0     , 0      }, /* 0x6E F19    */ \
    {0     , 0      }, /* 0x6F F20    */ \
    {0     , 0      }, /* 0x70 F21    */ \
    {0     , 0      }, /* 0x71 F22    */ \
    {0     , 0      }, /* 0x72 F23    */ \
    {0     , 0      }, /* 0x73 F24    */ \
    {0     , 0      }, /* 0x74        */ \
    {0     , 0      }, /* 0x75        */ \
    {0     , 0      }, /* 0x76        */ \
    {0     , 0      }, /* 0x77        */ \
    {0     , 0      }, /* 0x78        */ \
    {0     , 0      }, /* 0x79        */ \
    {0     , 0      }, /* 0x7A        */ \
    {0     , 0      }, /* 0x7B        */ \
    {0     , 0      }, /* 0x7C        */ \
    {0     , 0      }, /* 0x7D        */ \
    {0     , 0      }, /* 0x7E        */ \
    {0     , 0      }  /* 0x7F        */

#ifdef __cplusplus
}
#endif

#endif /* HID_REPORT_DEFS_H_ */
