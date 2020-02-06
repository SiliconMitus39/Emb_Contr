/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Silicon Mitus sm5803 battery charger driver.
 */


#ifndef DRIVER_CHARGER_SM5803_H_
#define DRIVER_CHARGER_SM5803_H_
#include "../board/nucleo-f072rb/board.h"


/* Generic */
#define SM5803_OFFSET_VOLTAGE_MIN		    ((uint32_t)(0xAA0))/* 2720 = 0xAA0 */

/* 	Masks to set at zero the bits of the byte  */

#define SM5803_ZERO_MASK_ALL_BITs  			((uint32_t)(0x00))

#define SM5803_ZERO_MASK_BIT0  				((uint32_t)(0xFE))
#define SM5803_ZERO_MASK_BIT1   			((uint32_t)(0xFD))
#define SM5803_ZERO_MASK_BIT2  				((uint32_t)(0xFB))
#define SM5803_ZERO_MASK_BIT3  				((uint32_t)(0xF7))
#define SM5803_ZERO_MASK_BIT4  				((uint32_t)(0xEF))
#define SM5803_ZERO_MASK_BIT5  				((uint32_t)(0xDF))
#define SM5803_ZERO_MASK_BIT6  				((uint32_t)(0xBF))
#define SM5803_ZERO_MASK_BIT7  				((uint32_t)(0x7F))

#define SM5803_LOW_WORD_32				   	((uint32_t)(0x0000FFFF))
#define SM5803_HIGH_WORD_32				   	((uint32_t)(0x0000FFFF))

/* Functions & Constants for maintenance */
#define SM5803_ERR_VAL                      ((int8_t)(-1))
#define SM5803_MAX_3BIT						((uint32_t)(0x07))
#define SM5803_I2C_OFFSET                   (0x08U)
#define USB_C_PORT_0 						((uint32_t)(0x00U))
#define USB_C_PORT_1 						((uint32_t)(0x01U))
#define SM5803_I2C_NULL						((uint32_t)(0x00U))

#define SM5803_FUNC_AND(a,b)				(a & b)

#define SM5803_VAL_CHRGNG_CURRENT_mA(REG) (REG*100)


/*---------------------------------*/
/* Set System Address page for i2c */
/* I2C interface                   */
/*---------------------------------*/
/* Generic port of the charger */
#define I2C_PORT_CHARGER 0

#define SM5803_I2C_SAD_PAGE_30				((int)0x30)
#define SM5803_I2C_SAD_PAGE_31				((int)0x31)
#define SM5803_I2C_SAD_PAGE_32				((int)0x32)

/*---------------------------------*/
/* Set Data  System Address 0x30   */
/*---------------------------------*/

/*** Set Address Register of BP_PLATFORM	 ***/
#define SM5803_BP_PLATFORM					((uint32_t)(0x18))

#define SM5803_CP_ISO_CL_R1_L_MASK				((uint32_t)0x1F)


/*Descriptions*****************************************************************
#define SM5803_BP_PLATFORM_MASK
 * Default: 00000000b
 * Represents the number (0 to 31) of the platform as selected during
 * power-up by measuring the value of the external resistor connected
 * to the PMODE pin.
 * ***************************************************************************/
/* Function: OTG CURRENT */
#define SM5803_BP_PLATFORM_MASK				((uint32_t)0x1F)

/* Default */
#define SM5803_BP_PLATFORM_DEFAULT	        ((uint32_t)0x00)
/*** THE END BP_PLATFORM	 ***/

/*** Set Address Register of STATUS_REG_1 ***/
#define SM5803_BP_STATUS_REG_1				((uint32_t)(0x01))

/*Descriptions*****************************************************************
 * Default: 00000000b
 * bat_det
 * Status of the BAT_DET signal.
 * 0 = Battery is not present
 * 1 = Battery is present
 * chg_det
 * Indication of charger insertion.
 * 0 = No charger connected
 * 1 = VBUS connected
 * vbus_ovh
 * Over-voltage flag of vbus.
 * 0 = Over-voltage not detected
 * 1 = Over-voltage detected
 * vbus_short
 * Short flag of vbus.
 * 0 = Short not detected
 * 1 = Short detected
 * vbus_uvl
 * Under-voltage flag of vbus.
 * 0 = Under-voltage not detected
 * 1 = Under-voltage detected
 * vbus_short_b
 * Indication of vbus short during discharge.
 * 0 = Short detected
 * 1 = Short not detected
 * vpwr_ok
 * Indication that vpwr is in the proper range.
 * 0 = Vpwr out of range
 * 1 = Vpwr inside the range
 * vsys_ok
 * Indication that vsys is in the proper range.
 * 0 = Vsys out of range
 * 1 = Vsys inside the range
 *
 * *************************************************************************/
/* Function: Status */
#define SM5803_VSYS_OK_BIT0_EN				((uint32_t)BIT(0))
#define SM5803_VPWR_OK_BIT1_EN				((uint32_t)BIT(1))
#define SM5803_ANA_VBUS_MONITOR_BIT2_EN		((uint32_t)BIT(2))
#define SM5803_VBUS_UVL_BIT3_EN				((uint32_t)BIT(3))
#define SM5803_VBUS_SHORT_BIT4_EN			((uint32_t)BIT(4))
#define SM5803_VBUS_OVH_BIT5_EN				((uint32_t)BIT(5))
#define SM5803_CHG_DET_BIT6_EN				((uint32_t)BIT(6))
#define SM5803_BAT_DET_BIT7_EN				((uint32_t)BIT(7))

/* Default */
#define SM5803_BP_STATUS_REG_1_DEFAULT	    ((uint32_t)0x00)
/*** THE END STATUS_REG_1 ***/

/*** Set Address Register of STATUS_REG_1 ***/
#define SM5803_BP_STATUS_REG_2				((uint32_t)(0x02))

/*Descriptions*****************************************************************
 * Default: 00000000b
 * sleep_state
 * Sleep status according to reg16[6:0] on charger page.
 * 0 = System not in sleep
 * 1 = System in sleep
 * bat_wu
 * 0 = None
 * 1 = A previous powerup already happened (when digital is in powerdown
 * some information are stored in the always-on domain). In order to read
 * this bit , regC4[2] must be previously written
 * chg_done_to_fg
 * 0 = None
 * 1 = An End Of Charge event has been triggered.  This bit is cleared
 * by writing a 1. Writing a 0 has no effect
 * bat_det_fg
 * Status of the BAT_DET signal before vbat measurements during boot.
 * 0 = Battery not present
 * 1 = Battery present
 * vbat_short
 * Short flag of vbat.
 * 0 = Short not detected
 * 1 = Short detected
 *
 * ***************************************************************************/
/* Function: Status */
#define SM5803_VBAT_SHORT_BIT0_EN			((uint32_t)BIT(0))
#define SM5803_BAT_DET_FG_BIT1_EN			((uint32_t)BIT(1))
#define SM5803_CHG_DONE_TO_FG_BIT2_EN		((uint32_t)BIT(2))
#define SM5803_BAT_WU_BIT3_EN				((uint32_t)BIT(3))

#define SM5803_CHRG_INHIBIT					((uint32_t)0xF8)
#define SM5803_CHRG_ENABLE			    	((uint32_t)0x01)


/* Default */
#define SM5803_BP_STATUS_REG_2_DEFAULT	    ((uint32_t)0x00)
/*** THE END STATUS_REG_1 ***/

/*---------------------------------*/
/* Set Data  System Address 0x31   */
/*---------------------------------*/

/*** Set Address Register of GP_PSYSs***/
#define SM5803_GP_PSYS_REG1		  			((uint32_t)(0x04))
#define SM5803_GP_PSYS_PHOT_R8			((uint32_t)(0x0E))
#define SM5803_GP_PSYS_MASK_R1				((uint32_t)(0x0E))
/*Descriptions*****************************************************************
 * Default: 00000101b
 * Set the psys gain factor.
 * 000 =  full scale = 179.648 W
 * 001 = full scale = 89.824 W
 * 010 =full scale = 44.912  W
 * 011 = full scale = 22.456 W
 * 100 = full scale = 11.228  W
 * 101 = full scale = 5.614 W
 * 110 = full scale = 2.807 W
 * 111 = full scale = 1.4035 W
 *
 *****************************************************************************/
/* Full scale psys*/
/*#define SM5803_PSYS_GAIN_BIT1_EN        	((uint32_t)BIT(1))
#define SM5803_PSYS_GAIN_BIT2_EN       		((uint32_t)BIT(2))
#define SM5803_PSYS_GAIN_BIT3_EN       	  	((uint32_t)BIT(3))*/

#define SM5803_GP_PSYS_REG1_MASK			((uint32_t)0x0E)

/* Default */
#define SM5803_GP_PSYS_REG1_DEFAULT			((uint32_t)0x05)
/*** THE END GP_PSYS_REG1 ***/

/*** Set Address Register of PSYS_PHOT_REG8 ***/
#define SM5803_GP_PSYS_PHOT_REG8  			(0x14U)

/*Descriptions*****************************************************************
 * Default : 00000000b
 * Psys_val value, this is the bus for the DAC.
 * The value of the lsb depends on psys_gain.
 *****************************************************************************/
/* Psys value */
#define SM5803_PSYS_VAL_MASK				(0xFFU)

/* Default*/
#define SM5803_GP_PSYS_PHOT_REG8_DEFAULT	(0x00U)
/*** THE END PSYS_PHOT_REG8 ***/

/*** Set Address Register of VBUS MEAS LSB ***/
#define SM5803_GP_VBUS_MEAS_LSB  			((uint32_t)(0x49))

/*Descriptions*****************************************************************
 * Default : 00000000b
 *Current measure of VBUS. This field contains the
 *Current 2 LSB bits [1:0] of VBUS[9:0].
 *
 *****************************************************************************/
#define SM5803_GP_VBUS_MEAS_LSB_MASK        ((uint32_t)0x03)

/* Default
#define SM5803_GP_VBUS_MEAS_LSB _DEFAULT	((uint32_t)0x00) */
/*** THE END VBUS MEAS LSB ***/

/*** Set Address Register of VBUS MEAS LSB ***/
#define SM5803_GP_VBUS_MEAS_MSB  			((uint32_t)(0x48))

/*Descriptions*****************************************************************
 * Default : 00000000b
 *Current measure of VBUS. This field contains the 8 MSB bits [9:2]
 *of VBUS[9:0]
 *
 *****************************************************************************/
#define SM5803_CP_VBUS_MSB_MASK      		((uint32_t)0xFF)

/* Default*/
#define SM5803_GP_VBUS_Meas_MSB _DEFAULT	((uint32_t)0x00)
/*** THE END VBUS MEAS LSB ***/


/*---------------------------------*/
/* Set Data  System Address 0x32   */
/*---------------------------------*/

/*** Set Address Register of CP_DISCH_CONF_REG1 ***/
#define SM5803_CP_DISCH_CONF_REG1  			((uint32_t)(0x30))

/*Descriptions*****************************************************************
 * Default: 00011100b
 * Voltage loop regulation value on VCHG_PWR. This field contains
 * the 8 MSB  bits [10:3] of VPWR_REG_SET[10:0].
 * The regulation value is 2.72V + VPWR_REG_SET*10mV (the LSB is 10mV).
 *
 * The lowest value is internally saturated to 0x01C (3 V) if reg31[5]=0.
 * According to the number of cells, the highest value is internally
 * saturated to the following if reg31[5]=0:
 *
 * 1 cell  : 0x3A0 (12 V);
 * 2/3/4 cells: 0x6F2 (20.5 V);
 *
 *****************************************************************************/
#define SM5803_CP_VPWR_REG_FILTER_MSB_MASK	((uint32_t)0xF8)
#define SM5803_CP_VPWR_REG_SET_MSB_MASK    	((uint32_t)0x07)
#define SM5803_SHIFT_R_CONST				((uint32_t)0x05)
/* Default */
#define SM5803_CP_DISCH_CONF_REG1_DEFAULT	((uint32_t)0x1C)
/*** THE END CP_DISCH_CONF_REG1 ***/

/*** Set Address Register of CP_DISCH_CONF_REG2 ***/
#define SM5803_CP_DISCH_CONF_REG2  			((uint32_t)(0x31))

/*Descriptions*****************************************************************
 * Default: 00000100b
 * Voltage loop regulation value on VCHG_PWR. This field contains
 * the 3 LSB  bits [2:0] of VPWR_REG_SET[10:0].
 * The regulation value is 2.72V + VPWR_REG_SET*10mV (the lsb is 10mV).
 *
 * The lowest value is internally saturated to 0x01C (3 V) if reg31[5]=0.
 * According to the number of cells, the highest value is internally
 * saturated to the following if reg31[5]=0:
 *
 * 1 cell  : 0x3A0 (12 V);
 * 2/3/4 cells: 0x6F2 (20.5 V);
 *
 *****************************************************************************/
/* Set discharge voltage LSB  bits */
/*#define SM5803_CP_VPWR_REG_SET_LSB_BIT0_EN	((uint32_t)BIT(0))
#define SM5803_CP_VPWR_REG_SET_LSB_BIT1_EN	((uint32_t)BIT(1))
#define SM5803_CP_VPWR_REG_SET_LSB_BIT2_EN	((uint32_t)BIT(2))*/

#define SM5803_CP_DISCH_CONF_REG2_MASK		((uint32_t)(0x07))

/* Default */
#define SM5803_CP_DISCH_CONF_REG2_DEFAULT	((uint32_t)(0x04))
/*** THE END CP_DISCH_CONF_REG2 ***/

/*** Set Address Register of CP_FLOW_REG1 ***/
#define SM5803_CP_FLOW_REG1					((uint32_t)(0x1C))

/*Descriptions*****************************************************************
 * Default: 00000001b
 * Enable the current loop regulation on VBUS OUT.
 * The current loop value is expressed in reg34[6:0].
 * 0 = Current loop disabled
 * 1 = Current loop enabled
 *
 *****************************************************************************/
#define SM5803_CP_FLOW_REG1_MASK				((uint32_t)(0x02))

/* Set Function mode */
#define SM5803_CHG_EN_BIT0_EN       		((uint32_t)BIT(0))
#define SM5803_VBUSIN_DISCH_EN_BIT1_EN 		((uint32_t)BIT(1))
/*  Set discharge option  bits */
#define SM5803_DIRECT_SRC_BIT2_EN         	((uint32_t)BIT(2))

/* Default */
#define SM5803_CP_FLOW_REG1_DEFAULT			((uint32_t)(0x01))
/*** THE END CP_FLOW_REG1 ***/

/*** Set Address Register of CP_FLOW_REG2 ***/
#define SM5803_CP_FLOW_REG2					((uint32_t)(0x1D))
#define SM5803_CP_FLOW_REG2_MASK			((uint32_t)(0x07))

/*Descriptions*****************************************************************
 * Default: 00000000b
 * fw_fastchg_cmd
 * In platform with batfet this command let start a fast-charge
 * In independently from internal measurements.
 *	0 = Fast-charge not forced
 *	1 = Fast-charge forced
 *
 * fw_prechg_cmd
 * In platform with batfet this command let start a pre-charge
 * independently from internal from internal measurements.
 *  0 = Pre-charge not forced
 *  1 = Pre-charge forced
 *
 *  fw_trkl_cmd
 *  In platform with batfet this command let start a trickle charge
 *  independently from internal measurements.
 *   0 = Trickle charge not forced
 *   1 = Trickle charge forced
 *
 *****************************************************************************/
/* Set pre_charge  bits */
#define SM5803_BIT0_AUTO_TRKL_EN_BF			((uint32_t)BIT(0))
#define SM5803_BIT1_AUTO_PRECHRG_EN_BF		((uint32_t)BIT(1))
#define SM5803_BIT2_AUTO_FASTCHRG_EN		((uint32_t)BIT(2))

/*Default*/
#define SM5803_CP_FLOW_REG2_DEFAULT 		((uint32_t)(0x00))
/*** THE END CP_FLOW_REG2 ***/

/*** Set Address Register of CP_MPPT_REG3 ***/
#define SM5803_CP_MPPT_REG3					((uint32_t)(0x27))

/*Descriptions*****************************************************************
 * Default: 11000100b
 * frs_en
 * Enable of fast role swap feature.
 * 0 = Feature disabled
 * 1 = Feature enabled
 *
 *frs_mode
 * Setting of the ppm_chg_go behavior during frs.
 * 0 = pm_chg_go toggles
 * 1 = pm_chg_go doesn't toggle
 *
 *****************************************************************************/
/* Set disable otg  bits */
#define SM5803_BIT6_FRS_MODE_EN				BIT(6)
#define SM5803_BIT7_FRS_EN_EN				BIT(7)

/*Default*/
#define SM5803_CP_MPPT_REG3_DEFAULT			((uint32_t)(0xC4))
/*** THE END CP_MPPT_REG3 ***/

/*** Set Address Register of CP_PRE_FAST_CONF_REG4 ***/
#define SM5803_CP_PRE_FAST_CONF_REG4		((uint32_t)(0x3C))
#define SM5803_CP_PRE_FAST_CONF_REG4_MASK		((uint32_t)(0x3F))

/*Descriptions*****************************************************************
 * Default: 11000000b
 * Fast charge current limit. In case a battery charge is ongoing and the
 * battery temperature monitoring has been enabled (BATTEMP_MON_EN equals to 1)
 * reading back this register will return the current resulting from the
 * programmed value and the (possible) reduction due to exceeded
 * temperature limits.
 * Value is ICHG_FAST_SET * 100mA
 *
 * ***************************************************************************/
/* Fast charge current */
#define SM5803_ICHG_FAST_SET				((uint32_t)(0x1F))

/* Default */
#define SM5803_CP_PRE_FAST_CONF_REG4_DEFAULT \
												((uint32_t)(0xC0))
/*** THE END CP_PRE_FAST_CONF_REG4 ***/

/*** Set Address Register of CP_ISO_CL_REG1 ***/
#define SM5803_CP_ISO_CL_REG1		((0x24U))

/*Descriptions*****************************************************************
 * Default: 00000100b
 *
 * Raw input current limit is equal to CHG_ILIM_RAW*100mA.
 * The default value is set by the platform. It can be 100mA, 500mA or 1.5A.
 * The register is loaded with the default value any time the charger
 * is removed.
 * ***************************************************************************/
/**/
#define SM5803_CHG_ILIM_RAW_MASK            ((uint32_t)(0x1F))

/* Default */
#define SM5803_CP_ISO_CL_REG1_DEFAULT		((uint32_t)(0x04))
/*** THE END CP_ISO_CL_REG1 ***/

/*** Set Address Register of PRE_FAST_CONF_REG2 ***/
#define SM5803_CP_PRE_FAST_CONF_REG2		((uint32_t)(0x3A))

/*Descriptions*****************************************************************
 * Default: 10111000b
 * Battery voltage setting during Fast Charge in Standard Temperature Region.
 * This field contains the 8 MSB  bits [10:3] of VBAT_FAST_SET[10:0].
 * The default value is overridden by the platform detection.
 *
 *  VBAT: 2720mV + (VBAT_FAST_SET *10mV)
 *
 * The lowest value is internally saturated to 0x01C (3 V) if reg32[2]=0.
 *  According to the number of cells, the highest value is internally
 *  saturated to the following (even if reg32[2]=1):
 *
 *  1 cell  : 0x0B2 (4.5 V);
 *  2 cells: 0x274 (9.0 V);
 *  3 cells: 0x436 (13.5 V);
 *  4 cells: 0x5F8 (18 V);
 * ***************************************************************************/
/* Funcion: MAX_CHARGE_VOLTAGE */
#define SM5803_VBAT_FAST_SET_MSB_MASK		((uint32_t)(0x000000FF))

/* Default */
#define SM5803_CP_PRE_FAST_CONF_REG2_DEFAULT\
											((uint32_t)(0x3A))
/*** THE END PRE_FAST_CONF_REG2 ***/

/*** Set Address Register of PRE_FAST_CONF_REG3 ***/
#define SM5803_CP_PRE_FAST_CONF_REG3		((uint32_t)(0x3B))

/*Descriptions*****************************************************************
 * Default: 00000000b
 * This field is the minimum battery level to keep the system alive.
 * 0000 = 2.3 V
 * 0001 = 2.4 V
 * 0010 = 2.5 V
 * 0011 = 2.6 V
 * 0100 = 2.7 V
 * 0101 = 2.8 V
 * 0110 = 2.9 V
 * 0111 = 3.0 V
 * 1000 = 3.1 V
 * 1001 = 3.2 V
 * 1010 = 3.3 V
 * 1011 = 3.4 V
 * 1100 = 3.5 V
 * 1101 = 3.6 V
 * 1110 = 3.7 V
 * 1111 = 3.8 V
 * ***************************************************************************/
/* Function: MAX_CHARGE_VOLTAGE */
#define SM5803_SW_WAKE_UP_TH_MASK           ((uint32_t)(0xF0))


#define SM5803_VBAT_FAST_SET_LSB_MASK       ((uint32_t)(0x00000007))

/* Default */
#define SM5803_CP_PRE_FAST_CONF_REG3_DEFAULT \
											((uint32_t)(0x00))
/*** THE END PRE_FAST_CONF_REG3 ***/

/*** Set Address Register of DISCH_CONF_REG5 ***/
#define SM5803_CP_DISCH_CONF_REG5			((uint32_t)(0x34))
#define SM5803_CP_DISCH_CONF_REG5_MASK		((uint32_t)(0x80))
/*Descriptions*****************************************************************
 * Default: 00000000b
 * Current limit on VBUS during discharge.
 * The limited value is CLS_LIMIT*50mA (saturated to 3.2A).
 * ***************************************************************************/
/* Function: OTG CURRENT */
#define SM5803_CLS_LIMIT					((uint32_t)(0x7F))

/* Default */
#define SM5803_CP_DISCH_CONF_REG5_DEFAULT	((uint32_t)(0x00))
/*** THE END DISCH_CONF_REG5 ***/




#endif /* DRIVER_CHARGER_SM5803_H_ */
