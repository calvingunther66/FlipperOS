#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUSB_RHPORT0_MODE OPT_MODE_HOST

// CFG_TUSB_OS is defined by the board/compiler flags usually,
// using PICO_RP2040 defs by default in SDK for None OS

#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN __attribute__((aligned(4)))

//--------------------------------------------------------------------
// HOST CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUH_ENUMERATION_BUFSIZE 256

#define CFG_TUH_HUB 1
#define CFG_TUH_HID 4 // Max 4 HID devices (Keyboard + Mouse)
#define CFG_TUH_MSC 0
#define CFG_TUH_CDC 0
#define CFG_TUH_BTH 0 // Bluetooth not needed

// HID buffer size
#define CFG_TUH_HID_EP_BUFSIZE 64

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
