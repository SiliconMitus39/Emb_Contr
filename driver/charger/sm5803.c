/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Silicon Mitus sm5803 battery charger driver.
 */

#include "battery_smart.h"
#include "sm5803.h"
#include "charge_ramp.h"
#include "charger.h"
#include "common.h"
#include "console.h"
#include "hooks.h"
#include "i2c.h"
#include "timer.h"


/* Charger parameters */

static const struct charger_info sm5803_charger_info = {
		.name = "sm5803",
		.voltage_max = 18000,
		.voltage_min = 2720,
		.voltage_step = 10,
		.current_max = 6000,
		.current_min = 100,
		.current_step = 100,
		.input_current_max = 3200,
		.input_current_min = 100,
		.input_current_step = 50
};
/* End charger parameters */

/* Console output macros */
#define CPRINTF(format, args...) cprintf(CC_CHARGER, format, ## args)
#define CPRINTS(format, args...) cprints(CC_CHARGER, format, ## args)
/* Structures and Constants */
const float sm5803_full_scale_const = 1.4035;

/* Local Functions-----------------------------------------------------------*/

/**
 * @brief  : The function evaluates the full
 * 		     scale vales of the voltage in mV
 *
 * @param N: value in three bits of the full scale
 *                              round the value up.
 * @return : full scale value
 */
static inline float full_scale_val(uint8_t N) {
	if ((N > 7))
		return SM5803_ERR_VAL;

	return (sm5803_full_scale_const * (1 << (SM5803_MAX_3BIT - N)));
}

/**
 * @brief  : The function evaluates the content
 * 		     of the register at 8 bits
 *
 * @param  : page_addr the I2C address
 *
 * @param  : offset the address register
 *
 * @return : the value of the read register
 */
static inline int raw_read8(int page_addr, int offset, int *value) {
	return i2c_read8(I2C_PORT_CHARGER, page_addr, offset, value);
}

/**
 * @brief  : The function write the content
 * 		     of the register at 8 bits
 *
 * @param  : page_addr the I2C address
 *
 * @param  : offset the address register
 *
 * @return : the value of the write register
 */
static inline int raw_write8(int page_addr, int offset, int value) {
	return i2c_write8(I2C_PORT_CHARGER, page_addr, offset, value);
}

/**
 * @brief  : The function evaluates the content
 * 		     of the register at 16 bits
 *
 * @param  : page_addr the I2C address
 *
 * @param  : offset the address register
 *
 * @return : the value of the read register
 */
static inline int raw_read16(int page_addr, int offset, int *value) {
	return i2c_read16(I2C_PORT_CHARGER, page_addr, offset, value);
}

/**
 * @brief  : The function write the content
 * 		     of the register at 16 bits
 *
 * @param  : page_addr the I2C address
 *
 * @param  : offset the address register
 *
 * @return : the value of the write register
 */
static inline int raw_write16(int page_addr, int offset, int value) {
	return i2c_write16(I2C_PORT_CHARGER, page_addr, offset, value);
}


/**
 *@brief              :     evaluate the main voltage
 *
 *
 * @param vpwr_reg_set:		the voltage value calculated
 *
 * @param vchg_pwr	  :		voltage in mV of the charger
 *
 * @param *rv0		  :		pointer to content of the result of the read and
 * 							write of the register retrieved by i2c
 *
 * @param offset	  :		the offset to address in i2c of the charger
 *
 * @return 			  : 	EC_SUCCESS on success, an error otherwise.
 */
static inline void max_voltage(int vpwr_reg_set, int vchg_pwr, int *rv0,
							   uint32_t offset) {

	int reg_val, temp;

	/* Check for the maximum voltage */
	if (vchg_pwr < 18000) {

		/* Retrieving actual value of the SM5803_CP_DISCH_CONF_REG1 */
		raw_read16(SM5803_I2C_SAD_PAGE_32 + offset, SM5803_CP_DISCH_CONF_REG1,
				&reg_val);
		reg_val &= SM5803_CP_VPWR_REG_FILTER_MSB_MASK;

		/* Send vchg_pwr */
		temp = vpwr_reg_set & SM5803_CP_VPWR_REG_SET_MSB_MASK;
		vpwr_reg_set = (((vpwr_reg_set >> (SM5803_SHIFT_R_CONST + 3))
				<< (SM5803_SHIFT_R_CONST + 3)) | reg_val) | temp;
		*rv0 = raw_write16(SM5803_I2C_SAD_PAGE_32+ offset,
				(int) SM5803_CP_DISCH_CONF_REG1, vpwr_reg_set);

	} else {

		*rv0 = EC_ERROR_INVAL;
	}
}

/**
 *@brief              :     evaluate the main current
 *
 * @param cls_limit:		the current limit calculated
 *
 *
 * @param *rv1	   :		pointer to content of the result of the read and
 * 							write of the register retrieved by i2c
 *
 * @param offset   :		the offset to address in i2c of the charger
 *
 * @return         : 		EC_SUCCESS on success, an error otherwise.
 */
static inline void max_current(int cls_limit, int *rv1,uint32_t offset) {
	int old_reg5;

	/* Retrieving actual value of the SM5803_CP_DISCH_CONF_REG5 */
	raw_read8(SM5803_I2C_SAD_PAGE_32 + offset ,
			  SM5803_CP_DISCH_CONF_REG5, &old_reg5);
	old_reg5 &= SM5803_CP_DISCH_CONF_REG5_MASK;

	/* Check for current saturated */
	if (cls_limit < 3200) {
		cls_limit /= 50;
		cls_limit &= SM5803_CLS_LIMIT;
		cls_limit |= old_reg5;

		/* Send vchg_pwr */
		*rv1 = raw_write8(SM5803_I2C_SAD_PAGE_32 + offset,
		SM5803_CP_DISCH_CONF_REG5, (cls_limit | old_reg5));

	} else {

		*rv1 = EC_ERROR_INVAL;
	}
}

/**
 *@brief              :     get the system power
 *
 * @param retWordPtr:		pointer to register content
 *
 *
 * @param *psys_valPtr :	pointer to register content
 *
 *
 * @param offset   :		the offset to address in i2c of the charger
 *
 * @return         : 		success, an error otherwise.
 */
static inline int getsysPwr(int *retWordPtr, int *psys_valPtr, int offset){

	int  ret, retWord = 0;

		ret = raw_read8(SM5803_I2C_SAD_PAGE_31 + offset,
						SM5803_GP_PSYS_REG1, retWordPtr);
		retWord = *retWordPtr;
		retWord &= SM5803_GP_PSYS_MASK_R1;
		*retWordPtr = retWord;
		ret = raw_read8(SM5803_I2C_SAD_PAGE_31 + offset,
						SM5803_GP_PSYS_PHOT_REG8, psys_valPtr);

			return ret;

}

/**
 *@brief            :     get the status of the system
 *
 * @param *status	:	  pointer to register status
 *
 *
 * @param *pv 		:	  pointer to correctness of value read
 *
 *
 * @param offset    :	  the offset to address in i2c of the charger
 *
 * @return          : 	  success, an error otherwise.
 */
static int chGetSt(int *status, uint32_t offset){

	int rv= EC_SUCCESS;
	/* Default status */
		*status = SM5803_BP_STATUS_REG_1_DEFAULT;

		/* Read Value */
		rv = raw_read16(SM5803_I2C_SAD_PAGE_30 + offset, (int) SM5803_BP_STATUS_REG_1,
				status);
		return rv;
}

/**
 *@brief            :     get the status of the system
 *
 * @param *status	:	  pointer to register status
 *
 *
 * @param *pv 		:	  pointer to correctness of value read
 *
 *
 * @param offset    :	  the offset to address in i2c of the charger
 *
 * @return          : 	  success, an error otherwise.
 */
static int chSetOpt(int option, uint32_t offset) {
	int optOld;
	option  &= 0xFF;
	raw_read8(SM5803_I2C_SAD_PAGE_32 + offset, SM5803_CP_FLOW_REG2, &optOld);

	option |= ((SM5803_CP_FLOW_REG2_MASK) | optOld);

	return raw_write8(SM5803_I2C_SAD_PAGE_32 + offset, SM5803_CP_FLOW_REG2,
			option);
}

/**
 * @brief                :     Set the input current in the system.
 *
 * @param input_current	:	  input current to be set.
 *
 * @param offset        :	  the offset to address in i2c of the charger.
 *
 * @return              : 	  success, an error otherwise.
 */
static int ch_in_curr( int input_current, uint32_t offset){

	int  temp = 0, raw;
			raw_read8(SM5803_I2C_SAD_PAGE_32 + offset, SM5803_CP_ISO_CL_REG1, &temp);
			temp &= 0xE0;
			if (input_current > 0) {
				input_current = (input_current- 100) / 100;
				input_current |= temp;
			} else {
				input_current = 0;
			}

			raw = raw_write8(SM5803_I2C_SAD_PAGE_32 + offset, SM5803_CP_ISO_CL_REG1,
					input_current);
			raw_read8(SM5803_I2C_SAD_PAGE_32 + offset, SM5803_CP_ISO_CL_REG1, &temp);
           CPRINTS("reg WRITE : %d",temp &= 0xE0 );
			return raw;
}

/**
 * @brief            :     get the the current
 *
 * @param offset    :	  the offset to address in i2c of the charger
 *
 * @return          : 	  success, an error otherwise.
 */
static int ch_get_curr( int *current, uint32_t offset ){
	int reg1 = 0, rv;
	rv = raw_read8(SM5803_I2C_SAD_PAGE_32,
				    SM5803_CP_PRE_FAST_CONF_REG4 + offset, &reg1);

	reg1 &= 0x3F;
	/*	CPRINTS(" rv_inner = %d",*rv);*/
		if (!(rv))
			*current = SM5803_VAL_CHRGNG_CURRENT_mA(reg1);
		return rv;
}

/**
 *@brief            :     set the the current
 *
 * @param *current	:	  pointer to current value
 *
 *
 * @param offset    :	  the offset to address in i2c of the charger
 *
 * @return          : 	  write the register ad hoc
 */
static int ch_set_curr( int current, uint32_t offset) {
	int reg_inner;
	raw_read8(SM5803_I2C_SAD_PAGE_32 + offset,
			  SM5803_CP_PRE_FAST_CONF_REG4, &reg_inner);

	reg_inner &= (~SM5803_CP_PRE_FAST_CONF_REG4_MASK);//correzione da 0x3F a 0xC0
	current /= 100;
	current |= reg_inner;

	return raw_write8(SM5803_I2C_SAD_PAGE_32 + offset,
	                  SM5803_CP_PRE_FAST_CONF_REG4, current);
}


/**
 *@brief            :     set the the current
 *
 *
 * @param enabled	:	  pointer to register
 *
 *
 * @param offset    :	  the offset to address in i2c of the charger
 *
 * @return          : 	  write the register ad hoc
 */
static int ch_en_otg(int enabled, uint32_t offset) {
	int data, temp;

	raw_read8(SM5803_I2C_SAD_PAGE_32 + offset, SM5803_CP_MPPT_REG3, &data);

	if (enabled >= 0 && enabled <= 3) {
		temp = SM5803_FUNC_AND((uint8_t)enabled, SM5803_BIT6_FRS_MODE_EN) |
		       SM5803_FUNC_AND((uint8_t)enabled, SM5803_BIT7_FRS_EN_EN);
	} else {
		return EC_ERROR_NOT_CALIBRATED;
	}
	data &= 0x3F;
	data = temp << 6;
	/* Writing the register */
	return raw_write8(SM5803_I2C_SAD_PAGE_32 + offset,
					  SM5803_CP_MPPT_REG3, data);
}


/**
 *@brief            :     get the the voltage
 *
 * @param *voltage :	  pointer define the result of the reading
 *
 * @param offset    :	  the offset to address in i2c of the charger
 *
 * @return          : 	  success, an error otherwise.
 */
static int ch_get_vol(int *voltage, uint32_t offset) {
	int ret= 0;
	int temp1 = 0, temp2 = 0;

		 ret = raw_read16(SM5803_I2C_SAD_PAGE_32 + offset , SM5803_CP_PRE_FAST_CONF_REG2, &temp1);
         temp2 = temp1;
         temp1 &= SM5803_VBAT_FAST_SET_LSB_MASK;
         temp2 = (temp2 >> SM5803_SHIFT_R_CONST)|temp1;
         *voltage = (SM5803_OFFSET_VOLTAGE_MIN + temp2*10);

	return ret;
}

/**
 *@brief            :     set the the voltage
 *
 * @param *voltage :	  pointer define the result of the reading
 *
 * @param offset    :	  the offset to address in i2c of the charger
 *
 * @return          : 	  success, an error otherwise.
 */
static int ch_set_vol(int voltage, uint32_t offset) {

	int temp1 = 0;
	voltage = (voltage-2720)/10;
	raw_read16(SM5803_I2C_SAD_PAGE_32 + offset,
					 SM5803_CP_PRE_FAST_CONF_REG2, &temp1);
	voltage = ((voltage >> 0x3) << 8) | (voltage & SM5803_VBAT_FAST_SET_LSB_MASK);
	temp1 &= 0xF8;
	voltage = (voltage | temp1);
	return raw_write16(SM5803_I2C_SAD_PAGE_32 + offset,
					   SM5803_CP_PRE_FAST_CONF_REG2, voltage);

}


/**
 *@brief            :     enable the charging
 *
 * @param *voltage :	  pointer define the result of the reading
 *
 * @param offset    :	  the offset to address in i2c of the charger
 *
 * @return          : 	  success, an error otherwise.
 */
static int charge_enable(void){
	int reg, rv;

	rv = raw_read8(SM5803_I2C_SAD_PAGE_32, SM5803_CP_FLOW_REG1, &reg );
	if (rv)
	return EC_ERROR_PARAM1;

	reg |=  SM5803_CP_FLOW_REG1_DEFAULT;

    rv = raw_write8(SM5803_I2C_SAD_PAGE_32,SM5803_CP_FLOW_REG1,reg);
	return rv;
}

/* End Local Functions-------------------------------------------------------*/



#ifdef CONFIG_CHARGE_RAMP_HW
#endif

/* Charger interfaces */

int charger_get_vbus_voltage(int port) {

	int reg_val, temp;
	if (++port) {
		temp = raw_read16(SM5803_I2C_SAD_PAGE_32,
				(int) SM5803_CP_DISCH_CONF_REG1, &reg_val);
		if (temp == 0) {
			temp = reg_val & SM5803_CP_VPWR_REG_SET_MSB_MASK;
			reg_val = (reg_val >> SM5803_SHIFT_R_CONST) | temp;
			reg_val = SM5803_OFFSET_VOLTAGE_MIN + reg_val * 10;
		}
	}
	return reg_val;

}


int charger_set_otg_current_voltage(int cls_limit, int vchg_pwr) {

	int vpwr_reg_set = ((vchg_pwr / 10) - SM5803_OFFSET_VOLTAGE_MIN),rv0, rv1;


	/* The correct port detection */

			/* Check for the maximum voltage */
			max_voltage(vpwr_reg_set, vchg_pwr, &rv0, SM5803_I2C_NULL);

			/* Check for current saturated */
			max_current(cls_limit, &rv1, SM5803_I2C_NULL);



	rv0 |= rv1;

	/* Add when needed. */
	return rv0;
}

int charger_is_sourcing_otg_power(int port) {
	int ret = 0, cp_flow_reg1;

	if (++port){
	ret = raw_read8(SM5803_I2C_SAD_PAGE_32,
	SM5803_CP_FLOW_REG1, &cp_flow_reg1);
	if (ret == 0)
		ret = (SM5803_CP_FLOW_REG1_MASK & cp_flow_reg1) >> 1;
	}
	return ret;
}

const struct charger_info* charger_get_info(void) {

	return &sm5803_charger_info;
}

int charger_post_init(void) {

	/* Set charger input current limit */
	return charger_set_input_current(CONFIG_CHARGER_INPUT_CURRENT);

}

int charger_get_system_power(void) {
	int ret = 0;
	int retWord;
	int psys_val;
	ret = getsysPwr(&retWord, &psys_val, SM5803_I2C_NULL);

	if (!(ret == EC_ERROR_INVAL))
		ret = (((full_scale_val(retWord >> 1) * psys_val) / 256) * (1e-6));

	return ret;
}

int charger_get_status(int *status) {
	int rv = EC_SUCCESS;
	rv = chGetSt(status,SM5803_I2C_NULL);
	return rv;
}

int charger_set_mode(int mode) {

	int rv;
	int option;

	rv = charger_get_option(&option);
	if (rv)
		return rv;


	if (mode & CHARGER_CHARGE_INHIBITED){
		option |= SM5803_CHRG_INHIBIT;

	}
	else
		{
		option |= ~SM5803_CHRG_INHIBIT;
		charge_enable();

		}
	return charger_set_option(option);
}

int charger_enable_otg_power(int enabled) {
	int ret = 0;

	/* enabled :
	 * 0x00
	 * 0x01
	 * 0x02
	 * 0x03
	 */
	ret = ch_en_otg(enabled, SM5803_I2C_NULL);

	return ret;
}

int charger_get_current(int *current) {
	int rv = 0;
	rv = ch_get_curr(current, SM5803_I2C_NULL);
	return rv;
}

int charger_set_current(int current) {
	int  ret = 0;
	ret = ch_set_curr(current, SM5803_I2C_NULL);
	return ret;
}

int charger_get_voltage(int *voltage) {
	int ret = 0;
	ret =  ch_get_vol(voltage, SM5803_I2C_NULL);
	return ret;
}

int charger_set_voltage(int voltage) {
	int ret = 0;

	ret = ch_set_vol(voltage, SM5803_I2C_NULL);

	return ret;
}


int charger_discharge_on_ac(int enable) {
	int rv, option;

	rv =charger_get_option(&option);
	if (rv)
		return rv;
	if (enable) {
		option |= SM5803_DIRECT_SRC_BIT2_EN;
	} else
		option &= ~SM5803_DIRECT_SRC_BIT2_EN;
	return charger_set_option(option);
}


int charger_get_input_current(int *input_current) {
	int rv = 0, reg;
	rv = raw_read8(SM5803_I2C_SAD_PAGE_32 + SM5803_I2C_NULL,
	SM5803_CP_ISO_CL_REG1, &reg);

	if (!rv) {
		reg &= SM5803_CP_ISO_CL_R1_L_MASK;
		*input_current = reg * (100);
	}
	return rv;
}

int charger_set_input_current(int input_current) {

    int ret = 0;
	ret = ch_in_curr(input_current, SM5803_I2C_NULL);
	return ret;
}

int charger_manufacturer_id(int *id) {
	*id =58;
	return 0;
}

int charger_device_id(int *id) {
	*id =03;
	return 0;
}

int charger_get_option(int *option) {

	int ret = 0;

	ret = raw_read8(SM5803_I2C_SAD_PAGE_32 + SM5803_I2C_NULL,
	SM5803_CP_FLOW_REG2, option);
	return ret;

}

int charger_set_option(int option) {

	int ret = EC_SUCCESS , temp;

	ret = charger_get_option(&temp);
	temp &= (~SM5803_CP_FLOW_REG2_MASK);
	option |= temp;

	if(ret == EC_SUCCESS)
	ret = chSetOpt(option, SM5803_I2C_NULL);

	return ret;
}

