/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Test USB PE module.
 */
#ifndef __CROS_TEST_USB_PE_H
#define __CROS_TEST_USB_PE_H

#include "common.h"

/*
 * Test references to port
 */
#define PORT0 0

/*
 * Parameters for pe_run
 *
 * pe_run(port, evt, enable)
 *    evt - currently ignored in the implementation
 *    enable - 0 Disable/1 Enable
 */
#define EVT_IGNORED 0

#define ENABLED 1
#define DISABLED 0

/**
 * usb_pe_drp_sm.c locally defined.  If it changes there, it must
 * be changed here as well.
 */

/*
 * Policy Engine Layer Flags
 */

/* At least one successful PD communication packet received from port partner */
#define PE_FLAGS_PD_CONNECTION               BIT(0)
/* Accept message received from port partner */
#define PE_FLAGS_ACCEPT                      BIT(1)
/* Power Supply Ready message received from port partner */
#define PE_FLAGS_PS_READY                    BIT(2)
/* Protocol Error was determined based on error recovery current state */
#define PE_FLAGS_PROTOCOL_ERROR              BIT(3)
/* Set if we are in Modal Operation */
#define PE_FLAGS_MODAL_OPERATION             BIT(4)
/* A message we requested to be sent has been transmitted */
#define PE_FLAGS_TX_COMPLETE                 BIT(5)
/* A message sent by a port partner has been received */
#define PE_FLAGS_MSG_RECEIVED                BIT(6)
/* A hard reset has been requested but has not been sent, not currently used */
#define PE_FLAGS_HARD_RESET_PENDING          BIT(7)
/* Port partner sent a Wait message. Wait before we resend our message */
#define PE_FLAGS_WAIT                        BIT(8)
/* An explicit contract is in place with our port partner */
#define PE_FLAGS_EXPLICIT_CONTRACT           BIT(9)
/* Waiting for Sink Capabailities timed out.  Used for retry error handling */
#define PE_FLAGS_SNK_WAIT_CAP_TIMEOUT        BIT(10)
/* Power Supply voltage/current transition timed out */
#define PE_FLAGS_PS_TRANSITION_TIMEOUT       BIT(11)
/* Flag to note current Atomic Message Sequence is interruptible */
#define PE_FLAGS_INTERRUPTIBLE_AMS           BIT(12)
/* Flag to note Power Supply reset has completed */
#define PE_FLAGS_PS_RESET_COMPLETE           BIT(13)
/* Flag to note a Structured Vendor Defined Message should be sent */
#define PE_FLAGS_SEND_SVDM                   BIT(14)
/* VCONN swap operation has completed */
#define PE_FLAGS_VCONN_SWAP_COMPLETE         BIT(15)
/* Flag to note no more discover identity messages are sent to port partner */
#define PE_FLAGS_DISCOVER_PORT_IDENTITY_DONE BIT(16)
/* Flag to note Swap Source Start timer should be set at PE_SRC_Startup entry */
#define PE_FLAGS_RUN_SOURCE_START_TIMER      BIT(17)
/* Flag to note Port Discovery port partner replied with BUSY */
#define PE_FLAGS_VDM_REQUEST_BUSY            BIT(18)
/* Flag to note Port Discovery port partner replied with NAK */
#define PE_FLAGS_VDM_REQUEST_NAKED           BIT(19)
/* Flag to note FRS/PRS context in shared state machine path */
#define PE_FLAGS_FAST_ROLE_SWAP_PATH         BIT(20)
/* Flag to note if FRS listening is enabled */
#define PE_FLAGS_FAST_ROLE_SWAP_ENABLED      BIT(21)
/* Flag to note TCPC passed on FRS signal from port partner */
#define PE_FLAGS_FAST_ROLE_SWAP_SIGNALED     BIT(22)

/* List of all Policy Engine level states */
enum usb_pe_state {
	/* Normal States */
	PE_SRC_STARTUP,
	PE_SRC_DISCOVERY,
	PE_SRC_SEND_CAPABILITIES,
	PE_SRC_NEGOTIATE_CAPABILITY,
	PE_SRC_TRANSITION_SUPPLY,
	PE_SRC_READY,
	PE_SRC_DISABLED,
	PE_SRC_CAPABILITY_RESPONSE,
	PE_SRC_HARD_RESET,
	PE_SRC_HARD_RESET_RECEIVED,
	PE_SRC_TRANSITION_TO_DEFAULT,
	PE_SNK_STARTUP,
	PE_SNK_DISCOVERY,
	PE_SNK_WAIT_FOR_CAPABILITIES,
	PE_SNK_EVALUATE_CAPABILITY,
	PE_SNK_SELECT_CAPABILITY,
	PE_SNK_READY,
	PE_SNK_HARD_RESET,
	PE_SNK_TRANSITION_TO_DEFAULT,
	PE_SNK_GIVE_SINK_CAP,
	PE_SNK_GET_SOURCE_CAP,
	PE_SNK_TRANSITION_SINK,
	PE_SEND_SOFT_RESET,
	PE_SOFT_RESET,
	PE_SEND_NOT_SUPPORTED,
	PE_SRC_PING,
	PE_GIVE_BATTERY_CAP,
	PE_GIVE_BATTERY_STATUS,
	PE_DRS_EVALUATE_SWAP,
	PE_DRS_CHANGE,
	PE_DRS_SEND_SWAP,
	PE_PRS_SRC_SNK_EVALUATE_SWAP,
	PE_PRS_SRC_SNK_TRANSITION_TO_OFF,
	PE_PRS_SRC_SNK_WAIT_SOURCE_ON,
	PE_PRS_SRC_SNK_SEND_SWAP,
	PE_PRS_SNK_SRC_EVALUATE_SWAP,
	PE_PRS_SNK_SRC_TRANSITION_TO_OFF,
	PE_PRS_SNK_SRC_ASSERT_RP,
	PE_PRS_SNK_SRC_SOURCE_ON,
	PE_PRS_SNK_SRC_SEND_SWAP,
	PE_FRS_SNK_SRC_START_AMS,
	PE_VCS_EVALUATE_SWAP,
	PE_VCS_SEND_SWAP,
	PE_VCS_WAIT_FOR_VCONN_SWAP,
	PE_VCS_TURN_ON_VCONN_SWAP,
	PE_VCS_TURN_OFF_VCONN_SWAP,
	PE_VCS_SEND_PS_RDY_SWAP,
	PE_DO_PORT_DISCOVERY,
	PE_VDM_REQUEST,
	PE_VDM_ACKED,
	PE_VDM_RESPONSE,
	PE_HANDLE_CUSTOM_VDM_REQUEST,
	PE_WAIT_FOR_ERROR_RECOVERY,
	PE_BIST,
	PE_DR_SNK_GET_SINK_CAP,

	/* Super States */
	PE_PRS_FRS_SHARED,
};

void set_state_pe(const int port, const enum usb_pe_state new_state);
enum usb_pe_state get_state_pe(const int port);

void pe_set_flag(int port, int flag);
void pe_clr_flag(int port, int flag);
int pe_chk_flag(int port, int flag);
int pe_get_all_flags(int port);
void pe_set_all_flags(int port, int flags);

#endif /* __CROS_TEST_USB_PE_H */
