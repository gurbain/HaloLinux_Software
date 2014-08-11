	
/*
 * com_cmg.c
 *
 * This file contains a library for communication with the Honeybee CMG Controller.
 * The associated header file (com_cmg.h) contains the necessary communication protocol codes
 *
 * See "356.ICD.001 rev IR (Draper CMG Array C&D Specification).xlsx" for specific command and read/write information
 *
 * Jose Gomez 
 * Isaac Garza 
 * Last modified 2014-07-30
 */


#include "cmgCOM.h"
#include "typedef.h" 
#include "crc16.h"

using namespace std;

istringstream serialport;
bool busy = false;
bool comm = false;
int retVal2 = 0;


// ------------------------ Ping ------------------------
void cmgGSP::cmgPing()
{
	busy = true;
	unsigned char data[5]; //length of data array is dictated by number of bytes in ICD Command, including two CRC bytes (in this particular case, there is no body; otherwise it would be 7)
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 3; //crclen is the data length not including the two CRC bytes


	msg_header *mHead; //from cmgCOM.h
	mHead = (msg_header *)&data; //typecasting &data as a msg_header*

	mHead->startByte = START_BYTE; 
	mHead->ID = PING; //0x10
	mHead->length = 0;

	crcBYTES= crcGenerate(data,crclen); //generates a 16-bit CRC
	leftCRC = crcBYTES >> 8; //splits the 16-bit CRC into left byte
	rightCRC = crcBYTES & 0xFF; //splits the 16-bit CRC into right byte

	memcpy(&data[sizeof(msg_header)], &leftCRC, sizeof(unsigned char)); //memcpy copies data from 2nd argument into 1st argument - length of data is given by 3rd argument
	memcpy(&data[sizeof(msg_header)+1], &rightCRC, sizeof(unsigned char));

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}


// ------------------------ Sending a Message to Query the CMG State ------------------------
void cmgGSP::queryCMGstate()
{
	while(comm){
	

	unsigned char data[5];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 3;

	msg_header *mHead;
	mHead = (msg_header *) &data;

	mHead->startByte = START_BYTE;
	mHead->ID = GET_TELEM; //0x30
	mHead->length = 0;

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+1], &rightCRC, sizeof(unsigned char));
	cout<<"busy: "<<busy<<endl;
	if(busy == false){
		pthread_mutex_lock(&read_write);
		retVal2 = write(serial_fd, (void *)&data, sizeof(data));
		pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
	{
		printf("Write failed\n");
	}
	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

	}
	else
	{ 
		usleep(1000*1000);
	}
}
}

// ------------------------ Gimbal Enable/Disable ------------------------
void cmgGSP::cmgGimEnable(unsigned char actID, unsigned char cmd)
{
	busy = true;
	unsigned char data[7];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 5;


	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE;
	mHead->ID = CMD_GIM_ENABLE; // 0x60
	mHead->length = 2;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+1], &cmd, sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+2], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+3], &rightCRC, sizeof(unsigned char));

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}


// ------------------------ Gimbal Home ------------------------
void cmgGSP::cmgGimHome(unsigned char actID)
{
	busy = true;	
	unsigned char data[6];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 4;

	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE;
	mHead->ID = CMD_GIM_HOME; //0x61
	mHead->length = 1;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+1], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+2], &rightCRC, sizeof(unsigned char));

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}

// ------------------------ Gimbal Mode ------------------------
void cmgGSP::cmgGimMode( unsigned char actID, unsigned char mode)
{
	busy = true;	
	unsigned char data[7];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 5;

	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE;
	mHead->ID = CMD_GIM_MODE;  //0x62
	mHead->length = 2;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+1], &mode, sizeof(unsigned char));
	
	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+2], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+3], &rightCRC, sizeof(unsigned char));

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}

// ------------------------ Gimbal Rate ------------------------
void cmgGSP::cmgGimRate(unsigned char actID, unsigned char rateMode, f32 rate)
{
	busy = true;	
	unsigned char data[11];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 9;

	unsigned char rate_array [sizeof(f32)]; // spliting float into 4 individual bytes
	memcpy(&rate_array, &rate, sizeof(f32));

	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE;
	mHead->ID = CMD_GIM_RATE; //0x63
	mHead->length = 6;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+1], &rateMode, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+2], &rate_array[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+3], &rate_array[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+4], &rate_array[2], sizeof(unsigned char)); 
	memcpy(&data[sizeof(msg_header)+5], &rate_array[3], sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+6], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+7], &rightCRC, sizeof(unsigned char));

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}

// ------------------------ Gimbal Position ------------------------
void cmgGSP::cmgGimPos(unsigned char actID, unsigned char posMode, f32 pos)
{
	busy = true;	
	unsigned char data[11];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 9;

	unsigned char pos_array [sizeof(f32)]; // spliting float into 4 individual bytes
	memcpy(&pos_array, &pos, sizeof(f32));

	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE;
	mHead->ID = CMD_GIM_POSI; //0x64
	mHead->length = 6;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+1], &posMode, sizeof(unsigned char)); //copying the position bytes into the msg_header
	memcpy(&data[sizeof(msg_header)+2], &pos_array[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+3], &pos_array[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+4], &pos_array[2], sizeof(unsigned char)); 
	memcpy(&data[sizeof(msg_header)+5], &pos_array[3], sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+6], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+7], &rightCRC, sizeof(unsigned char));

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}

// ------------------------ FlyWheel Enable ------------------------
void cmgGSP::cmgFlyEnable(unsigned char actID, unsigned char enable)
{
	busy = true;	
	unsigned char data[7];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 5;

	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE;
	mHead->ID = CMD_FLY_ENABLE; //0x70
	mHead->length = 2;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+1], &enable, sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+2], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+3], &rightCRC, sizeof(unsigned char));

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}

// ------------------------ FlyWheel Rate ------------------------
void cmgGSP::cmgFlywheelRate(unsigned char actID, f32 w_flywheel)
{
	busy = true;	
	unsigned char data[10];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 8;

	unsigned char rate_array [sizeof(f32)]; // spliting float into 4 individual bytes
	memcpy(&rate_array, &w_flywheel, sizeof(f32));

	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE;
	mHead->ID = CMD_FLY_RATE; //0x71
	mHead->length = 5;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+1], &rate_array[0], sizeof(unsigned char)); //copying the rate bytes into the msg_header
	memcpy(&data[sizeof(msg_header)+2], &rate_array[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+3], &rate_array[2], sizeof(unsigned char)); 
	memcpy(&data[sizeof(msg_header)+4], &rate_array[3], sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+5], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+6], &rightCRC, sizeof(unsigned char));

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}

// ------------------------ Torques ------------------------
void cmgGSP::cmgSetTorques( f32 x_torque, f32 y_torque, f32 z_torque)
{
	busy = true;	
	unsigned char data[17];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 15;

	unsigned char torque_x_array [sizeof(f32)]; // spliting x-axis torque into 4 individual bytes
	memcpy(&torque_x_array, &x_torque, sizeof(f32));

	unsigned char torque_y_array [sizeof(f32)]; // spliting y-axis torque into 4 individual bytes
	memcpy(&torque_y_array, &y_torque, sizeof(f32));

	unsigned char torque_z_array [sizeof(f32)]; // spliting z-axis torque into 4 individual bytes
	memcpy(&torque_z_array, &z_torque, sizeof(f32));

	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE;
	mHead->ID = CMD_SET_TORQUE; //0x20
	mHead->length = 12;

	memcpy(&data[sizeof(msg_header)], &torque_x_array[0], sizeof(unsigned char)); //copying the torque bytes into the msg_header
	memcpy(&data[sizeof(msg_header)+1], &torque_x_array[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+2], &torque_x_array[2], sizeof(unsigned char)); 
	memcpy(&data[sizeof(msg_header)+3], &torque_x_array[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+4], &torque_y_array[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+5], &torque_y_array[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+6], &torque_y_array[2], sizeof(unsigned char)); 
	memcpy(&data[sizeof(msg_header)+7], &torque_y_array[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+8], &torque_z_array[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+9], &torque_z_array[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+10], &torque_z_array[2], sizeof(unsigned char)); 
	memcpy(&data[sizeof(msg_header)+11], &torque_z_array[3], sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+12], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+13], &rightCRC, sizeof(unsigned char));

	for(int i=0; i<17; i++){
		cout<<"data["<<i<<"]:"<<(bitset<8>) data[i]<<endl; //debugging
	}

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}

// ------------------------ Set Cosine Matrix ------------------------

void cmgGSP::cmgSetCosine(unsigned char actID, f32 ind_11, f32 ind_12, f32 ind_13,
									   f32 ind_21, f32 ind_22, f32 ind_23,
									   f32 ind_31, f32 ind_32, f32 ind_33) 
									   // ind_21 represents the index at row 2, column 1 in the matrix
{
	busy = true;	
	unsigned char data[42];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 40;

	unsigned char DCM_11 [sizeof(f32)]; //splitting float into 4 individual bytes
	memcpy(&DCM_11, &ind_11, sizeof(f32)); 

	unsigned char DCM_12 [sizeof(f32)];
	memcpy(&DCM_12, &ind_12, sizeof(f32));

	unsigned char DCM_13 [sizeof(f32)];
	memcpy(&DCM_13, &ind_13, sizeof(f32));

	unsigned char DCM_21 [sizeof(f32)];
	memcpy(&DCM_21, &ind_21, sizeof(f32));

	unsigned char DCM_22 [sizeof(f32)];
	memcpy(&DCM_22, &ind_22, sizeof(f32));

	unsigned char DCM_23 [sizeof(f32)];
	memcpy(&DCM_23, &ind_23, sizeof(f32));

	unsigned char DCM_31 [sizeof(f32)];
	memcpy(&DCM_31, &ind_31, sizeof(f32));

	unsigned char DCM_32 [sizeof(f32)];
	memcpy(&DCM_32, &ind_32, sizeof(f32));

	unsigned char DCM_33 [sizeof(f32)];
	memcpy(&DCM_33, &ind_33, sizeof(f32));


	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE;
	mHead->ID = CMD_SET_COS; //0x21
	mHead->length = 37;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+1], &DCM_11[0], sizeof(unsigned char)); //copying the Directional Cosine Matrix bytes into the msg_header
	memcpy(&data[sizeof(msg_header)+2], &DCM_11[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+3], &DCM_11[2], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+4], &DCM_11[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+5], &DCM_12[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+6], &DCM_12[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+7], &DCM_12[2], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+8], &DCM_12[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+9], &DCM_13[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+10], &DCM_13[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+11], &DCM_13[2], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+12], &DCM_13[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+13], &DCM_21[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+14], &DCM_21[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+15], &DCM_21[2], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+16], &DCM_21[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+17], &DCM_22[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+18], &DCM_22[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+19], &DCM_22[2], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+20], &DCM_22[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+21], &DCM_23[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+22], &DCM_23[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+23], &DCM_23[2], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+24], &DCM_23[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+25], &DCM_31[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+26], &DCM_31[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+27], &DCM_31[2], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+28], &DCM_31[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+29], &DCM_32[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+30], &DCM_32[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+31], &DCM_32[2], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+32], &DCM_32[3], sizeof(unsigned char));

	memcpy(&data[sizeof(msg_header)+33], &DCM_33[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+34], &DCM_33[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+35], &DCM_33[2], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+36], &DCM_33[3], sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+37], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+38], &rightCRC, sizeof(unsigned char));

		for(int i=0; i<42; i++){
	cout<<"data["<<i<<"]:"<<(bitset<8>) data[i]<<endl; //debugging
	}

	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	pthread_mutex_lock(&read_write);
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	pthread_mutex_unlock(&read_write);
	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	pthread_mutex_lock(&read_write);
	cmgRead();
	pthread_mutex_unlock(&read_write);
	}

}

void cmgGSP::cmgWrite(unsigned char *msg_write)
{
	/*cout<<"sizeof(msg_write: "<<sizeof(data)<<endl;

	int retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))
		{printf("Write failed\n");
	}

	else{
	printf("Write succesful\n");

	cmgRead();
	}*/

}
// ------------------------ open Serial Communications ------------------------
void cmgGSP::cmgOpenPort()
{
	
	struct termios options;
	
	serialport.str(SERIAL_PORT);
	cout << "Using Serial Port: " << serialport.str() <<endl;

	//open the serial port
	serial_fd = open(serialport.str().c_str(), O_RDWR | O_NOCTTY | O_NDELAY); // Opening port
	cout<<serial_fd<<endl;

	if (serial_fd == -1)
	{
		printf("Open Serial Port Failed: %s\n", SERIAL_PORT);
		exit(1);
	}
	else
	{
		fcntl(serial_fd, F_SETFL, 0);
	}

	/****** These are options that were included in main.cpp of
	SerialTest2 in spheres\Missions\Halo\FlightSoftware\HaloLinux_Software\Utilities\SerialTest2
	*******/

	//get termios options
	tcgetattr(serial_fd, &options);

	
	//set up number of bits
	options.c_cflag &= CSIZE;
	options.c_cflag |= (CS8|CLOCAL|CREAD);
		

	//set up no parity checking
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag |= CLOCAL | CREAD;

	//disable hardware/software flow control
	options.c_cflag &= ~CRTSCTS;
	options.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

	//set canonical
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG); //new
	options.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OLCUC | OPOST);

	//set termios options
	tcflush(serial_fd, TCIFLUSH);
	if(cfsetispeed(&options, B115200)<0){
		printf("Error setting input Baud rate"); //sets input baud rate
		exit(1);
	}
		//set up baud rates
	if(cfsetospeed(&options, B115200)<0){ // sets output baud rate
		printf("Error setting input Baud rate");
		exit(1);}	//cfseti/ospeeds need to be directly above tcsetattr()
	tcsetattr(serial_fd, TCSANOW, &options); // sets baud rates specified in cfsetisspeed and cfsetospeed now

	usleep(3000*1000);// to make sure port is opened before continuing

	cout << "serial port open" << endl;
	comm = true;
	pthread_mutex_init(&read_write, NULL);
}

