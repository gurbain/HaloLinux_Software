



/*



 * cmgCOM.h



 *



 * This file contains contains the necessary communication protocol codes for interfacing



 * with the Honeybee CMG Controller.



 * Jose Gomez (version 1)



 *



 * Last modified 2014-07-21



 */







#ifndef __cmgCOM__



#define __cmgCOM__







//#include "spheres_types.h" // to be edited once communications established



//#include "exp_v2.h"  // To use expv2_uart_send







#include <stdio.h>



#include <stdlib.h>



#include <string.h>



#include <iostream>



#include <fstream>



#include <sys/time.h>



#include <sys/resource.h>



#include <pthread.h>



#include <unistd.h>



#include <signal.h>



#include <fcntl.h>



#include <termios.h>



#include <vector>



#include <deque>



#include <cstdio>



#include <errno.h>



#include "typedef.h"



#include "crc16.h"



#include <bitset>



#include <sstream>



#include <iomanip>



#include <ctime>



#include <string>















// To be edited once communications established



// UART COM Definitions



#define SERIAL_PORT	"/dev/ttyUSB0"



#define max_length			255  // maximum length (in BYTES) of UART Rx/Tx packets











// ------ Defined Command Codes for sending commands to the CMG Controller --------



#define START_BYTE		0x05 //START_BYTE for any message sent to CMG Controller



#define PING			0x10 // Ping, responds woth the smae body recieved



#define GET_TELEM		0x30 //Get most recent CMG telemetry



#define CMD_GIM_ENABLE		0x60 //Performs all steps necessary to hold gimbals in null momentum position



#define CMD_GIM_HOME		0x61 //slew the gimbal to find the index pulse of the encoder



#define CMD_GIM_MODE		0x62 //set the gimbal mode



#define CMD_GIM_RATE 		0x63 //set gimbal rate and optionally switch to rate mode



#define CMD_GIM_POSI		0x64 //set gimbal position and optionally switch to position mode



#define CMD_FLY_ENABLE		0x70 //enable or disable flywheel motor driver



#define CMD_FLY_RATE 		0x71 //accelerate/decelerate flywheels to the commanded rate



#define CMD_SET_TORQUE		0x20 //submit a new torque triplet to the steering law



#define CMD_SET_COS		0x21 //set the orientaiton of a CMG with a direction cosine matrix





// ------ Defined Command Codes for receiving data from the CMG Controller --------



#define FROM_CMG		0x06 // Regular Messages from the CMG controller start with this



#define NEG_ACK			0x15 // Error messages start with this sometimes?



//---ERROR CODES, part of msg starting with 0x15 -------



#define CRC_ERR			0x81 // CRC ERROR



#define BAD_CMD_ID		0x82 // Bad command ID



#define INVLD_SETVAL	0x83 // INvalid Set Value







#define cmgAll		0xFF







// ------------------------------------------------------------







#ifdef __cplusplus



#endif





// Structure to hold data to send to cmgCOM



typedef struct {



	float gimbal_rate[4];

	float gimbal_rateCmd[4];



	float gimbal_angle[4];



	float fly_rate[4];

	float gimbal_angleErr[4];



	float torque_gimRate[4];

	bool torque_accepted;



} msg_cmg_state;









// Structure to hold message header info (needs to be compatible with SPHERES header design)



typedef struct {



	unsigned char startByte;



	unsigned char ID;



	unsigned char length;



	//unsigned char crc1;



	//unsigned char crc2;



} msg_header;



















void queryCMGstate(); //Query the CMG to get telemetry data







void cmgPing(); //Ping the cmg to ensure a solid connection ****What we are working on right now*****







void cmgGimEnable(unsigned char actID, unsigned char cmd); //Enable the CMG Gimbal







void cmgGimHome(unsigned char actID); //Home the CMG Gimbal







void cmgGimMode( unsigned char actID, unsigned char mode); //Change the mode of the CMG Gimbal







void cmgGimRate(unsigned char actID, unsigned char rateMode, float rate); //Set the CMG Gimbal rate







void cmgGimPos(unsigned char actID, unsigned char posMode, float pos); //Set a specific CMG Gimbal position







void cmgFlyEnable(unsigned char actID, unsigned char enable); //Enable the CMG Flywheel







void cmgFlywheelRate(unsigned char actId, float w_flywheel); //Set a specific CMG Flywheel rate







void write_data(float *telem_array);







msg_cmg_state parseTelem();







void initAll();







void cmgOffAll();



void manualOps();







void positionSweep(unsigned char actID);







void gimbalSweep(unsigned char actID, f32 rate);





void cmgSetCosine(unsigned char actID, f32 ind_11, f32 ind_12, f32 ind_13,

									   f32 ind_21, f32 ind_22, f32 ind_23,

									   f32 ind_31, f32 ind_32, f32 ind_33);





void cmgSetTorques(unsigned char actID, f32 x_torque, f32 y_torque, f32 z_torque);







void cmgRead(); // reads cmg control board response







void cmgInit(unsigned char actID);







void cmgOpenPort(); // open serial port







void cmgOff(unsigned char actID);





void closeCMG(); // close port















#ifdef __cplusplus



#endif







#endif