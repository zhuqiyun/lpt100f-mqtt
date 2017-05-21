/** 
 ******************************************************************************* 
 * @file    i2c.h  
 * @author  Yancy 
 * @version V1.0.0 
 * @date    29-May-2013 
 * @brief   I2C master driver header file 
 ******************************************************************************* 
 * @attention 
 * 
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS 
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE 
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,  
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING 
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS. 
 * 
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2> 
 */
#ifndef __I2C_H__
#define __I2C_H__

#include "common/type.h"

typedef enum
{
    PORT_A, /**< Use GPIOA for I2C */
	PORT_B,	/**< Use GPIOB for I2C */
	PORT_C	/**< Use GPIOC for I2C */
} PORT_SEL;

VOID* I2cMasterCreate(PORT_SEL SclPortSel, BYTE SclIndex, PORT_SEL SdaPortSel, BYTE SdaIndex);

/**
  * @brief  This function generates I2C start timing.
  * @param  None
  * @return None
  */
VOID I2cStart(VOID* I2cMasterHandle);

/**
  * @brief  This function generates I2C stop timing.
  * @param  None
  * @return None
  */
VOID I2cStop(VOID* I2cMasterHandle);

/**
  * @brief  This function send ACK to I2C slave.
  * @param  None
  * @return None
  */
VOID I2cSendAck(VOID* I2cMasterHandle);

/**
  * @brief  This function send NACK to I2C slave.
  * @param  None
  * @return None
  */
VOID I2cSendNoAck(VOID* I2cMasterHandle);


/**
  * @brief  This function checks ACK/NACK from I2C slave.
  * @param  None
  * @return None
  */
BOOL I2cChkAck(VOID* I2cMasterHandle);

/**
  * @brief  This function send one byte to I2C slave.
  * @param  Val: data to be sent
  * @return 
  *     @arg    True:  Receive ACK from I2C slave
  *     @arg    False: Not receive ACK from I2C slave
  */
BOOL I2cWriteByte(VOID* I2cMasterHandle, BYTE Val);

/**
  * @brief  This function recieve one byte from I2C slave.
  * @param  none
  * @return Data received from I2C slave
  */
BYTE I2cReadByte(VOID* I2cMasterHandle);


/**
  * @brief  This function send multiple bytes to I2C slave.
  * @param  Buf: Buffer pointer to the data to be send 
  * @param  Len: Data Length to be send in byte uint
  * @return 
  *     @arg TRUE:  success
  *     @arg FALSE: fail
  */
BOOL I2cWriteBytes(VOID* I2cMasterHandle, BYTE* Buf, BYTE Len);

/**
  * @brief  This function receive multiple bytes to I2C slave.
  * @param  Buf: Buffer pointer to hold the received data. 
  * @param  Len: Data Length to be received
  * @return None
  */
VOID I2cReadBytes(VOID* I2cMasterHandle, BYTE* Buf, BYTE Len);

/**
  * @brief  This function send device address and register address to I2C slave.
  * @param  DeviceAddr: I2C slave address 
  * @param  RegAddr: slave device's register address, usually before read/write slave device's register,
  *         i2c master should first send it.
  * @return 
  *     @arg TRUE: success
  *     @arg FALSE:fail
  */
BOOL I2cSendAddr(VOID* I2cMasterHandle, BYTE SlaveAddr, BYTE RegAddr, BYTE RwFlag);

/**
  * @brief  This function send multiple bytes to I2C slave with address
  * @param  Buf: Buffer pointer to the data to be send 
  * @param  Len: Data Length to be send in byte uint
  * @return 
  *     @arg TRUE: success
  *     @arg FALSE:fail
  */
BOOL I2cWriteNByte(VOID* I2cMasterHandle, BYTE SlaveAddr,BYTE RegAddr, BYTE* Buf, BYTE Len);


/**
  * @brief  This function read multiple bytes to I2C slave with address
  * @param  Buf: Buffer pointer to the data to be send 
  * @param  Len: Data Length to be send in byte uint
  * @return 
  *     @arg TRUE:  success
  *     @arg FALSE: fail
  */
BOOL I2cReadNByte(VOID* I2cMasterHandle, BYTE SlaveAddr,BYTE RegAddr, BYTE* Buf, BYTE Len);

#endif
