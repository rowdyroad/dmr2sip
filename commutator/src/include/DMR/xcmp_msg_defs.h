//The MOTOTRBO ADP XCMP Console Demo Application software is considered Motorola Confidential Restricted information and
//contains intellectual property owned by Motorola, Inc., which is protected by U.S. copyright laws and international
//copyright treaties.  Unauthorized reproduction or distribution of this software is strictly prohibited.  2009 Motorola,
//Inc. All Rights Reserved.

#ifndef _XCMP_MSG_DEFS_H
#define _XCMP_MSG_DEFS_H

#include "xnl_messages_def.h"
#include <stdint.h>

/* XCMP Opcode definition */
#define XCMP_REQUEST_OPCODE_MASK       0x0000
#define XCMP_REPLY_OPCODE_MASK         0x8000
#define XCMP_BRDCST_OPCODE_MASK        0xB000

#define XCMP_RADIO_STATUS              0x000E
#define XCMP_RADIO_STATUS_REQ          (XCMP_REQUEST_OPCODE_MASK | XCMP_RADIO_STATUS)
#define XCMP_RADIO_STATUS_REPLY	   (XCMP_REPLY_OPCODE_MASK | XCMP_RADIO_STATUS)

#define XCMP_DEVICE_INIT_STATUS        0x0400
#define XCMP_DEVICE_INIT_STATUS_BRDCST (XCMP_BRDCST_OPCODE_MASK | XCMP_DEVICE_INIT_STATUS)

#define XCMP_DISPLAY_TEXT           0x0401
#define XCMP_DISPLAY_TEXT_REQ       (XCMP_REQUEST_OPCODE_MASK | XCMP_DISPLAY_TEXT)
#define XCMP_DISPLAY_TEXT_REPLY	(XCMP_REPLY_OPCODE_MASK | XCMP_DISPLAY_TEXT)
#define XCMP_DISPLAY_TEXT_BRDCST       (XCMP_BRDCST_OPCODE_MASK | XCMP_DISPLAY_TEXT)

#define XCMP_INDICATOR_UPDATE          0x0402
#define XCMP_INDICATOR_UPDATE_BRDCST   (XCMP_BRDCST_OPCODE_MASK | XCMP_INDICATOR_UPDATE)

#define XCMP_PHYSICAL_USER_INPUT        0x0405
#define XCMP_PHYSICAL_USER_INPUT_BRDCST (XCMP_BRDCST_OPCODE_MASK | XCMP_PHYSICAL_USER_INPUT)

#define XCMP_VOLUME_CTRL               0x0406
#define XCMP_VOLUME_CTRL_BRDCST        (XCMP_BRDCST_OPCODE_MASK | XCMP_VOLUME_CTRL)

#define XCMP_SPKR_CTRL                 0x0407
#define XCMP_SPKR_CTRL_BRDCST          (XCMP_BRDCST_OPCODE_MASK  | XCMP_SPKR_CTRL)

#define XCMP_MIC_CTRL                0x040E
#define XCMP_MIC_CTRL_REQ            (XCMP_REQUEST_OPCODE_MASK | XCMP_MIC_CTRL)
#define XCMP_MIC_CTRL_REPLY          (XCMP_REPLY_OPCODE_MASK   | XCMP_MIC_CTRL)
#define XCMP_MIC_CTRL_BRDCST         (XCMP_BRDCST_OPCODE_MASK  | XCMP_MIC_CTRL)

#define XCMP_BRIGHTNESS                0x0411
#define XCMP_BRIGHTNESS_REQ            (XCMP_REQUEST_OPCODE_MASK | XCMP_BRIGHTNESS)
#define XCMP_BRIGHTNESS_REPLY          (XCMP_REPLY_OPCODE_MASK   | XCMP_BRIGHTNESS)
#define XCMP_BRIGHTNESS_BRDCST         (XCMP_BRDCST_OPCODE_MASK  | XCMP_BRIGHTNESS)

#define XCMP_EMER_CTRL                  0x0413
#define XCMP_EMER_CTRL_BRDCST           (XCMP_BRDCST_OPCODE_MASK  | XCMP_EMER_CTRL)

#define XCMP_TX_CTRL                    0x0415
#define XCMP_TX_CTRL_REQ                (XCMP_REQUEST_OPCODE_MASK | XCMP_TX_CTRL)
#define XCMP_TX_CTRL_REPLY              (XCMP_REPLY_OPCODE_MASK   | XCMP_TX_CTRL)
#define XCMP_TX_CTRL_BRDCST             (XCMP_BRDCST_OPCODE_MASK  | XCMP_TX_CTRL)

#define XCMP_CALL_CTRL                  0x041E
#define XCMP_CALL_CTRL_REQ              (XCMP_REQUEST_OPCODE_MASK | XCMP_CALL_CTRL)
#define XCMP_CALL_CTRL_REPLY            (XCMP_REPLY_OPCODE_MASK   | XCMP_CALL_CTRL)
#define XCMP_CALL_CTRL_BRDCST           (XCMP_BRDCST_OPCODE_MASK  | XCMP_CALL_CTRL)

#define XCMP_RMT_RADIO_CTRL             0x041C
#define XCMP_RMT_RADIO_CTRL_REQ         (XCMP_REQUEST_OPCODE_MASK | XCMP_RMT_RADIO_CTRL)
#define XCMP_RMT_RADIO_CTRL_REPLY       (XCMP_REPLY_OPCODE_MASK   | XCMP_RMT_RADIO_CTRL)
#define XCMP_RMT_RADIO_CTRL_BRDCST      (XCMP_BRDCST_OPCODE_MASK  | XCMP_RMT_RADIO_CTRL)

#define XCMP_MENU_NAVIGATION            0x041F
#define XCMP_MENU_NAVIGATION_REQ        (XCMP_REQUEST_OPCODE_MASK | XCMP_MENU_NAVIGATION)
#define XCMP_MENU_NAVIGATION_REPLY      (XCMP_REPLY_OPCODE_MASK   | XCMP_MENU_NAVIGATION)
#define XCMP_MENU_NAVIGATION_BRDCST     (XCMP_BRDCST_OPCODE_MASK  | XCMP_MENU_NAVIGATION)

#define XCMP_CHAN_SELECTION            0x040D
#define XCMP_CHAN_SELECTION_REQ        (XCMP_REQUEST_OPCODE_MASK | XCMP_CHAN_SELECTION)
#define XCMP_CHAN_SELECTION_REPLY      (XCMP_REPLY_OPCODE_MASK   | XCMP_CHAN_SELECTION)
#define XCMP_CHAN_SELECTION_BRDCST     (XCMP_BRDCST_OPCODE_MASK  | XCMP_CHAN_SELECTION)


typedef enum
{
    XCMP_DEVICE_FAMILY = 0x00,
    XCMP_DEVICE_DISPLAY = 0x02,
    XCMP_DEVICE_RF_BAND = 0x04,
    XCMP_DEVICE_GPIO_CTRL = 0x05,
    XCMP_DEVICE_RADIO_TYPE = 0x07,
    XCMP_DEVICE_KEYPAD = 0x09,
    XCMP_DEVICE_CHANNEL_KNOB = 0x0D,
    XCMP_DEVICE_VIRTUAL_PERSONALITY_SUPPORT = 0x0E,
} xcmpDeviceType_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t xcmp_ver[4];
    uint8_t dev_init_type;
    uint8_t dev_type;
    uint16_t dev_status;
    uint8_t array_size;
    uint8_t dev_descriptor_array[1]; /* Place holder for the descriptor array */
} xcmp_dev_init_status_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  function;
    uint8_t  intensity;
} xcmp_brightness_request_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  pui_source;
    uint8_t  pui_type;
    uint16_t pui_id;
    uint8_t           pui_state;
    uint8_t           pui_state_min;
    uint8_t           pui_state_max;
} xcmp_pui_broadcast_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  display_region;
    uint8_t  encoding_type;
    uint16_t num_text;
    uint8_t           string[1];
} xcmp_display_text_broadcast_t;

typedef struct{
    uint8_t addr_type;
    uint8_t addr_size;
    uint8_t rmt_addr[1]; /* The length of address is variable, so this is just a place holder */
} xcmp_remote_addr_t;

typedef struct {
    uint8_t id_size;
    uint8_t group_id[3]; /* For MOTOTRBO, the id_size must be set to 3 */
} xcmp_group_id_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  function;
    uint8_t  call_type;
    xcmp_remote_addr_t rmt_addr;
} xcmp_call_ctrl_request_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t result;
    uint8_t function;
    uint8_t call_state;
} xcmp_call_ctrl_reply_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  call_type;
    uint8_t  call_state;
    xcmp_remote_addr_t rmt_addr;
} xcmp_call_ctrl_broadcast_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  feature;
    uint8_t  operation;
    xcmp_remote_addr_t rmt_addr;
} xcmp_rmt_radio_ctrl_request_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcde;
    uint8_t function;
    uint8_t display_mode;
    uint16_t menu_id;
    uint8_t data[2];        /* The size of "Count" field is depended on XCMP version */
} xcmp_menu_navigation_request_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcde;
    uint8_t result;
    uint8_t function;
    uint8_t display_mode;
    uint8_t data[1];       /* place holder for rest message data */
} xcmp_menu_navigation_reply_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  function;
    uint16_t zone_num;
    uint16_t channel_num;
} xcmp_chan_zone_selection_request_t;

typedef struct {
    xnl_msg_hdr_t msg_hdr;
    uint16_t xcmp_opcde;
    uint8_t result;
    uint8_t function;
    uint16_t zone_num;
    uint16_t channel_num;
    uint8_t status[1];
} xcmp_chan_zone_selection_reply_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint16_t zone_num;
    uint16_t channel_num;
} xcmp_chan_zone_selection_broadcast_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  function;
    uint8_t  mode;
    uint8_t  source;
} xcmp_tx_ctrl_request_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  result;
    uint8_t  function;
    uint8_t  mode;
    uint8_t  state;
} xcmp_tx_ctrl_reply_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  mode;
    uint8_t  state;
    uint8_t  state_change_reason;
} xcmp_tx_ctrl_broadcast_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  condition;
} xcmp_radio_status_request_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  function;
    uint8_t  mic;
    uint8_t  signaling;
    uint8_t  gain;
} xcmp_mic_ctrl_request_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  result;
    uint8_t  function;
    uint8_t  mic;
    uint8_t  signaling;
    uint8_t  gain;
} xcmp_mic_ctrl_reply_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  result;
    uint8_t  condition;
    uint8_t           status[1];
} xcmp_radio_status_reply_t;

typedef struct {
    xnl_msg_hdr_t  msg_hdr;
    uint16_t xcmp_opcode;
    uint8_t  status;
    xcmp_remote_addr_t rmt_addr;
} xcmp_emergency_ctrl_broadcast_t;

#endif /* _XCMP_MSG_DEFS_H */
