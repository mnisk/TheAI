/*
 * Copyright 2004-2006, Jérôme Duval. All rights reserved.
 * Copyright 2005-2010, Axel Dörfler, axeld@pinc-software.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef AT_KEYMAP_H
#define AT_KEYMAP_H


#include <SupportDefs.h>
#include <usb/USB_hid.h>
#include <usb/USB_hid_page_consumer.h>
#include <usb/USB_hid_page_generic_desktop.h>


#define HID_CONSUMER(code) ((B_HID_USAGE_PAGE_CONSUMER << 16) | (B_HID_UID_CON_##code))
#define HID_GD(code) ((B_HID_USAGE_PAGE_GENERIC_DESKTOP << 16) | (B_HID_UID_GD_##code))


const static uint32 kATKeycodeMap[] = {
	0x1,	// Esc			1
	0x12, 	// 1
	0x13,	// 2
	0x14,	// 3
	0x15,	// 4
	0x16,	// 5
	0x17,	// 6
	0x18,	// 7
	0x19,	// 8
	0x1a,	// 9			10
	0x1b,	// 0
	0x1c,	// -
	0x1d,	// =
	0x1e,	// BACKSPACE
	0x26,	// TAB
	0x27,	// Q
	0x28,	// W
	0x29,	// E
	0x2a,	// R
	0x2b,	// T			20
	0x2c,	// Y
	0x2d,	// U
	0x2e,	// I
	0x2f,	// O
	0x30,	// P
	0x31,   // [
	0x32,	// ]
	0x47,	// ENTER
	0x5c,	// Left Control
	0x3c,	// A			30
	0x3d,	// S
	0x3e,	// D
	0x3f,	// F
	0x40,	// G
	0x41,	// H
	0x42,	// J
	0x43,	// K
	0x44,	// L
	0x45,	// ;
	0x46,	// '			40
	0x11,	// `
	0x4b,	// Left Shift
	0x33, 	// \ (backslash -- note: don't remove non-white-space after BS char)
	0x4c,	// Z
	0x4d,	// X
	0x4e,	// C
	0x4f,	// V
	0x50,	// B
	0x51,	// N
	0x52,	// M			50
	0x53,	// ,
	0x54,	// .
	0x55,	// /
	0x56,	// Right Shift
	0x24,	// *
	0x5d,	// Left Alt
	0x5e,	// Space
	0x3b,	// Caps
	0x02,	// F1
	0x03,	// F2			60
	0x04,	// F3
	0x05,	// F4
	0x06,	// F5
	0x07,	// F6
	0x08,	// F7
	0x09,	// F8
	0x0a,	// F9
	0x0b,	// F10
	0x22,	// Num
	0x0f,	// Scroll		70
	0x37,	// KP 7
	0x38,	// KP 8
	0x39,	// KP 9
	0x25,	// KP -
	0x48,	// KP 4
	0x49,	// KP 5
	0x4a,	// KP 6
	0x3a,	// KP +
	0x58,	// KP 1
	0x59,	// KP 2			80
	0x5a,	// KP 3
	0x64,	// KP 0
	0x65,	// KP .
	0x7e,	// SystReq
	0x00,	// UNMAPPED
	0x69,	// <
	0x0c,	// F11
	0x0d,	// F12
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		90
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		100
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		110
	0x00,   // UNMAPPED
	0x6e,   // Katakana/Hiragana, second key right to spacebar, japanese
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x6b,   // Ro (\\ key, japanese)
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		120
	0x6d,   // Henkan, first key right to spacebar, japanese
	0x00,   // UNMAPPED
	0x6c,   // Muhenkan, key left to spacebar, japanese
	0x00,   // UNMAPPED
	0x6a,   // Yen (macron key, japanese)
	0x70,   // Keypad . on Brazilian ABNT2
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		130
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		140
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	HID_CONSUMER(SCAN_PREVIOUS_TRACK), // media: Previous track
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		150
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	HID_CONSUMER(SCAN_NEXT_TRACK), // media: Next track
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x5b,   // KP Enter
	0x60,   // Right Control
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	HID_CONSUMER(MUTE), // media: Volume mute	160
	HID_CONSUMER(AL_CALCULATOR),   // media: Calculator
	HID_CONSUMER(PLAY), // media: Play
	0x00,   // UNMAPPED
	HID_CONSUMER(STOP), // media: Stop
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		170
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	HID_CONSUMER(VOLUME_DECREMENT), // media: Volume down
	0x00,   // UNMAPPED
	HID_CONSUMER(VOLUME_INCREMENT), // media: Volume up
	0x00,   // UNMAPPED
	HID_CONSUMER(AC_HOME), // media: www home
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		180
	0x23,   // KP /
	0x00,   // UNMAPPED
	0x0e,   // Print Screen
	0x5f,   // Right Alt
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED		190
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x10,   // Pause
	0x20,   // Home
	0x57,	// Up Arrow		200
	0x21,   // Page Up
	0x00,   // UNMAPPED
	0x61,   // Left Arrow
	0x00,   // UNMAPPED
	0x63,   // Right Arrow
	0x00,   // UNMAPPED
	0x35,   // End
	0x62,   // Down Arrow
	0x36,   // Page Down
	0x1f,   // Insert		210
	0x34,   // Delete
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x66,   // Left Gui
	0x67,   // Right Gui	220
	0x68,   // Menu
	HID_GD(SYSTEM_POWER_DOWN), // power: Shutdown
	HID_GD(SYSTEM_SLEEP), // power: Sleep
	0x00,   // UNMAPPED 0x60
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	HID_GD(SYSTEM_WAKE_UP), // power: Wake
	0x00,   // UNMAPPED
	HID_CONSUMER(AC_SEARCH),   // media: www Search
	HID_CONSUMER(AC_BOOKMARKS), // medie: www Favorites 		230
	HID_CONSUMER(AC_REFRESH),   // media: www Refresh
	HID_CONSUMER(AC_STOP),   // media: www Stop
	HID_CONSUMER(AC_FORWARD),   // media: www Forward
	HID_CONSUMER(AC_BACK),   // media: www back
	HID_CONSUMER(AL_LOCAL_MACHINE_BROWSER),   // media: My computer
	HID_CONSUMER(AL_EMAIL_READER),   // media: email
	HID_CONSUMER(AL_CONSUMER_CONTROL_CONFIGURATION),   // media: select
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
	0x00,   // UNMAPPED
};


#endif	// AT_KEYMAP_H
