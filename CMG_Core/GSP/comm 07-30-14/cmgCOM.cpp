	
/*
 * com_cmg.c
 *
 * This file contains a library for communication with the Honeybee CMG Controller.
 *	The associated header file (com_cmg.h) contains the necessary communication protocol codes
 *
 * Jose Gomez 
 * Isaac Garza 
 * Last modified 2014-07-21
 */


#include "cmgCOM.h"
#include "typedef.h"
#include "crc16.h"

using namespace std;
int serial_fd;
istringstream serialport;
unsigned char msgTelem[256];
int retVal= 0;
int counter= 0;
ofstream myfile;

// ------------------------ Ping ------------------------
void cmgPing()
{
	int msg_length;
	unsigned char data[5];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 3;


	msg_header *mHead;
	mHead = (msg_header *)&data;

	mHead->startByte = START_BYTE; //0x05
	mHead->ID = PING; //0x10
	mHead->length = 0;

	crcBYTES= crcGenerate(data,crclen); //generates a 16-bit CRC
	leftCRC = crcBYTES >> 8; //splits the 16-bit CRC into left byte
	rightCRC = crcBYTES & 0xFF; //splits the 16-bit CRC into right byte

	memcpy(&data[sizeof(msg_header)], &leftCRC, sizeof(unsigned char)); //how we are copying bytes into our data array
	memcpy(&data[sizeof(msg_header)+1], &rightCRC, sizeof(unsigned char));

	msg_length = sizeof(data);

	//CMGwrite(msg_length, (unsigned char*)&data); // new function needs updating
	int retVal2;
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	cmgRead();
	
	}
}


// ------------------------ Sending a Message to Query the CMG State ------------------------
void queryCMGstate()
{
	int msg_length;
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

	msg_length = sizeof(data);

	//CMGwrite(msg_length, (unsigned char*)&data); // new function needs updating
	int retVal2;
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	cmgRead();
	}
	
}

// ------------------------ Gimbal Enable/Disable ------------------------
void cmgGimEnable(unsigned char actID, unsigned char cmd) // actID (0,1,2, or 3) cmd (0 or 1)
{
	int msg_length;
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

	msg_length = sizeof(data);


	//CMGwrite(msg_length, (unsigned char*)&data); // new function needs updating
	int retVal2;
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	printf("Gim Enabled-----------------------------------------\n");
	cmgRead();
	}
}


// ------------------------ Gimbal Home ------------------------
void cmgGimHome(unsigned char actID)
{
	int msg_length;
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

	msg_length = sizeof(data);

	//CMGwrite(msg_length, (unsigned char*)&data); // new function needs updating
	int retVal2;
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	printf("Gim Home-----------------------------------------\n");
	cmgRead();
	}

}

// ------------------------ Gimbal Mode ------------------------
void cmgGimMode( unsigned char actID, unsigned char mode)
{
	int msg_length;
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
	msg_length = sizeof(data);

	//CMGwrite(msg_length, (unsigned char*)&data); // new function needs updating
	int retVal2;
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	printf("Gim Mode-----------------------------------------\n");
	cmgRead();
	}

}

// ------------------------ Gimbal Rate ------------------------
void cmgGimRate(unsigned char actID, unsigned char rateMode, f32 rate)
{
	int msg_length;
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

	msg_length = sizeof(data);

	int retVal2;
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	printf("Gim RATE-----------------------------------------\n");
	cmgRead();
	}

}

// ------------------------ Gimbal Position ------------------------
void cmgGimPos(unsigned char actID, unsigned char posMode, f32 pos)
{
	int msg_length;
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
	mHead->ID = CMD_GIM_POSI; //0x63
	mHead->length = 6;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+1], &posMode, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+2], &pos_array[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+3], &pos_array[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+4], &pos_array[2], sizeof(unsigned char)); 
	memcpy(&data[sizeof(msg_header)+5], &pos_array[3], sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+6], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+7], &rightCRC, sizeof(unsigned char));

	msg_length = sizeof(data);

	//CMGwrite(msg_length, (unsigned char*)&data); // new function needs updating
	int retVal2;
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));
	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	printf("GIM POS-----------------------------------------\n");
	cmgRead();
	}
	
}

// ------------------------ FlyWheel Enable ------------------------
void cmgFlyEnable(unsigned char actID, unsigned char enable)
{
	int msg_length;
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

	msg_length = sizeof(data);

	//CMGwrite(msg_length, (unsigned char*)&data); // new function needs updating
	int retVal2;
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	printf("Fly Enabled-----------------------------------------\n");
	cmgRead();
	}
}

// ------------------------ FlyWheel Rate ------------------------
void cmgFlywheelRate(unsigned char actID, f32 w_flywheel)
{
	int msg_length;
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
	mHead->ID = CMD_FLY_RATE;
	mHead->length = 5;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+1], &rate_array[0], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+2], &rate_array[1], sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+3], &rate_array[2], sizeof(unsigned char)); 
	memcpy(&data[sizeof(msg_header)+4], &rate_array[3], sizeof(unsigned char));

	crcBYTES = crcGenerate(data,crclen);
	leftCRC = crcBYTES >> 8;
	rightCRC = crcBYTES & 0xFF;

	memcpy(&data[sizeof(msg_header)+5], &leftCRC, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+6], &rightCRC, sizeof(unsigned char));

	msg_length = sizeof(data);

	//CMGwrite(msg_length, (unsigned char*)&data); // new function needs updating
	int retVal2;
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	printf("Fly Rate-----------------------------------------\n");
	cmgRead();
	}
	

}

// ------------------------ Torques ------------------------
void cmgSetTorques( f32 x_torque, f32 y_torque, f32 z_torque)
{
	/*********** The torques are f32 format, but have LSB/MSB on the ICD -- 
		Will assume that simply splitting into four bytes will work*/

	int msg_length;
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

	//memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)], &torque_x_array[0], sizeof(unsigned char));
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

	msg_length = sizeof(data);
		for(int i=0; i<17; i++){
	cout<<"data["<<i<<"]:"<<(bitset<8>) data[i]<<endl; //debugging
}

	int retVal2; 
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))

	{

		printf("Write failed\n");

		//exit(1);

	}

	else

	{

	printf("Write succesful\n");
	printf("Torque Commands-----------------------------------------\n");
	cmgRead();
	}


}

// ------------------------ Set Cosine Matrix ------------------------

void cmgSetCosine(unsigned char actID, f32 ind_11, f32 ind_12, f32 ind_13,f32 ind_21, f32 ind_22, f32 ind_23,f32 ind_31, f32 ind_32, f32 ind_33)
{
	int msg_length;
	unsigned char data[42];
	u16 crcBYTES;
	unsigned char leftCRC;
	unsigned char rightCRC;
	int crclen = 40;

	unsigned char DCM_11 [sizeof(f32)]; // spliting float into 4 individual bytes
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
	mHead->ID = CMD_SET_COS;
	mHead->length = 37;

	memcpy(&data[sizeof(msg_header)], &actID, sizeof(unsigned char));
	memcpy(&data[sizeof(msg_header)+1], &DCM_11[0], sizeof(unsigned char));
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

	msg_length = sizeof(data);
	
		for(int i=0; i<42; i++){
	cout<<"data["<<i<<"]:"<<(bitset<8>) data[i]<<endl; //debugging
}

	int retVal2; 
	retVal2 = write(serial_fd, (void *)&data, sizeof(data));

	if (retVal2 < sizeof(data))
		printf("Write failed\n");

	else{
	printf("Write succesful\n");
	printf("Set Direction Cosine Matrices-----------------------------------------\n");
	cmgRead();
	}


}


// ------------------------ open Serial Communications ------------------------
void cmgOpenPort()
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

	if(cfsetospeed(&options, B115200)<0){ 		// sets output baud rate
		printf("Error setting input Baud rate");
		exit(1);}								//cfseti/ospeeds need to be directly above tcsetattr()
	tcsetattr(serial_fd, TCSANOW, &options); 	// sets baud rates specified in cfsetisspeed and cfsetospeed now

	usleep(3000*1000);// to make sure port is opened before continuing

	cout << "serial port open" << endl;
}

// ------------------------ Read Response ------------------------

void cmgRead(){ 
	msg_cmg_state statePoint1;
	unsigned char messagelength;
	msg_header msgHeader; //msg_header is defined in cmgCOM.h
	msgHeader.startByte = 0;
	msgHeader.length = 0;
	msgHeader.ID = 0;
	int totalBytes = 0;
	int ReadCount = 0;
	//int retVal= 0;


	cout<<"in read function"<<endl;
	while(1) //infinte loop, since we always want to be reading data
	{
		while (totalBytes == 0) {
			retVal = read(serial_fd, (unsigned char*)&msgHeader,1);

			if (retVal == -1)
			{
				printf("read() message failed: %s\n", strerror(errno));
			}
			if (msgHeader.startByte != FROM_CMG) {
				cout<<"startByte: "<<(bitset<8>) msgHeader.startByte<<endl;
				printf("Bad start byte on read()\n");
				totalBytes = 0;
			} 
			else {
				totalBytes+=retVal;
			}
		}

		while (totalBytes < sizeof(msgHeader)) {
			retVal = read(serial_fd, (unsigned char*)&msgHeader+totalBytes, sizeof(msgHeader)-totalBytes);		
			if (retVal == -1)
			{
				printf("read() message failed: %s\n", strerror(errno));
			}
			totalBytes+=retVal;
		}
	printf("MSG2 RCV: StartByte=0x%X, ID=0x%X, Length=0x%X\n", msgHeader.startByte, msgHeader.ID, msgHeader.length);
	cout<<"totalBytes: "<<totalBytes<<endl;

		if (msgHeader.ID == CMD_SET_TORQUE && msgHeader.length ==17 ){

			msg_header *mHeadTorque;
			mHeadTorque = (msg_header *)&msgTelem;

			mHeadTorque->startByte = msgHeader.startByte;
			mHeadTorque->ID = msgHeader.ID;
			mHeadTorque->length = msgHeader.length;
			while (totalBytes < 20){
				retVal = read(serial_fd, (unsigned char*)&msgTelem[totalBytes + 208], 1);

				if(retVal == -1)
					printf("read() message failed: %s\n", strerror(errno));

				totalBytes += retVal;	
			}

			break;
		}

		if (msgHeader.ID == GET_TELEM){

			/*msg_header *mHeadTelem;
			mHeadTelem = (msg_header *)&msgTelem;

			mHeadTelem->startByte = msgHeader.startByte;
			mHeadTelem->ID = msgHeader.ID; 
			mHeadTelem->length =  msgHeader.length;*/

			while (totalBytes < 208) {
				retVal = read(serial_fd, (unsigned char*)&msgTelem[totalBytes-3], 1);
				//cout<< "in telem while"<<endl;			
				if (retVal == -1)
				{
					printf("read() message failed: %s\n", strerror(errno));
				}
				totalBytes+=retVal;
			}
			//statePoint1 = parseTelem();
			//usleep(1000*1000);
			break;
		} 

		else
		{
			printf("MSG2 RCV: StartByte=0x%X, ID=0x%X, Length=0x%X\n", msgHeader.startByte, msgHeader.ID, msgHeader.length);
			break;
		}
			
	}
}

// ------------------------ close communication with cmg ------------------------
void closeCMG() //shuts down CMG
{

	//close serial port
	myfile.close();
	close(serial_fd);

	printf("Closed cmg\n");

}


// ------------------------ Parsing needed telem response ------------------------
msg_cmg_state parseTelem(){
	msg_cmg_state cmgState;

	float telem_array[56];

	for(int i = 0; i<4;i++){
		f32 buffer;
		unsigned char b[] = {msgTelem[3+(4*i)], msgTelem[2+(4*i)], msgTelem[1+(4*i)], msgTelem[0+(4*i)]};
		memcpy(&buffer, &b, sizeof(buffer));
		telem_array [i] = buffer;

	}

	for(int i = 4; i<8;i++){
		unsigned int buffer;
		unsigned char b[] = {msgTelem[3+(4*i)], msgTelem[2+(4*i)], msgTelem[1+(4*i)], msgTelem[0+(4*i)]};
		memcpy(&buffer, &b, sizeof(buffer));
		telem_array [i] = buffer;

	}

	for(int i = 8; i<52;i++){
		f32 buffer;
		unsigned char b[] = {msgTelem[3+(4*i)], msgTelem[2+(4*i)], msgTelem[1+(4*i)], msgTelem[0+(4*i)]};
		memcpy(&buffer, &b, sizeof(buffer));
		telem_array [i] = buffer;

	}

	cmgState.torque_accepted = msgTelem[209];

	for(int i = 0; i<4;i++){
		f32 buffer;
		unsigned char b[] = {msgTelem[213+(4*i)], msgTelem[212+(4*i)], msgTelem[211+(4*i)], msgTelem[210+(4*i)]};
		memcpy(&buffer, &b, sizeof(buffer));
		telem_array [52+i] = buffer;

	}
	
	for (int j = 0; j<4; j++){
		cmgState.gimbal_angle[j] = telem_array[8+j];
		cmgState.gimbal_angleErr[j] = telem_array[12+j];
		cmgState.gimbal_rate[j] = telem_array[20+j];
		cmgState.gimbal_rateCmd[j] = telem_array[24+j];
		cmgState.fly_rate[j] = telem_array[32+j];
		}

	for (int i = 0; i<4; i++){
			cmgState.torque_gimRate[i] = telem_array[52+i];
	}


	write_data(telem_array);
	
	return cmgState;
}
// ------------------------ Saving telem response ------------------------
void write_data(float * telem_array){
	string columns[57] = {"Time", "Motor Bus Volts 0", "Motor Bus Volts 1", "TempLoc 0 ", "TempLoc 1 ", "status 0", "status 1", "status 2", "status 3","Gmb Pos 0","Gmb Pos 1","Gmb Pos 2","Gmb Pos 3","Gmb Pos ERR 0","Gmb Pos ERR 1","Gmb Pos ERR 2","Gmb Pos ERR 3","Gmb Pos CMD 0","Gmb Pos CMD 1","Gmb Pos CMD 2","Gmb Pos CMD 3","Gmb Rate 0","Gmb Rate 1","Gmb Rate 2","Gmb Rate 3","Gmb Rate CMD 0","Gmb Rate CMD 1","Gmb Rate CMD 2","Gmb Rate CMD 3","Gmb Current 0","Gmb Current 1","Gmb Current 2","Gmb Current 3","Fly Rate 0","Fly Rate 1","Fly Rate 2","Fly Rate 3","Fly Rate CMD 0","Fly Rate CMD 1","Fly Rate CMD 2","Fly Rate CMD 3","Fly Current 0","Fly Current 1","Fly Current 2","Fly Current 3","TempPri 0","TempPri 1","TempPri 2","TempPri 3","TempAux 0","TempAux 1","TempAux 2","TempAux 3", "Rate 0", "Rate 1", "Rate 2", "Rate 3"};

	time_t now = time(0);
	//char* dt = ctime(&now); local time
	tm *gmtm = gmtime(&now); 
	char* dt = asctime(gmtm);// UTC time
	size_t length = strlen(dt);
	dt[length-1]=NULL;

	if ( counter == 0 ){
		myfile.open ("TelemData.csv");
            for( int y = 0; y < 57; y++ )

            {

                 myfile << columns[y];

                  if( y != 56 ){

                        myfile << ",";}
            }
            myfile << endl;

	}
	
	
    for( int w = 0; w < 57; w++ )
    {
    	if (w==0){
    		myfile << dt;
    	}
    	else{
    	myfile << telem_array[w-1];
	}

    		if( w != 56 ){

    			myfile << ",";
            }
       
    }

    myfile << endl;
counter += 1;

}

// ------------------------ Initializing CMGs ------------------------
void cmgInit(unsigned char actID){
	unsigned char mode = 1;
	f32 rate = 6000.0;
	msg_cmg_state statevar;
	int count2 = 0;


	cmgGimHome(actID);
	usleep(10000*1000);
	cmgFlyEnable(actID, 1);
	usleep(3000*1000);
	cmgFlywheelRate(actID, rate);
	do
	{
	queryCMGstate();
	if(count2 == 0){
	usleep(30000*1000);
	count2 += 30;}
	else{
	usleep(1000*1000);}
	count2 += 1;
	cout<<"spinup rate count2: "<<count2<<endl;
	statevar = parseTelem();
	cout<< "Fly rate: "<<statevar.fly_rate[actID]<<endl;
	} while ((statevar.fly_rate[actID]<2050 && count2 < 210)); // not the best check

}

// ------------------------ Initializing CMGs ------------------------

void initAll(){
	cmgInit(0);
	usleep(1000*1000);
	cmgInit(1);
	usleep(1000*1000);
	cmgInit(2);
	usleep(1000*1000);
	cmgInit(3);
	usleep(1000*1000);
}

// ------------------------ Initializing CMGs ------------------------

void cmgOff(unsigned char actID){
	unsigned char mode = 1;
	f32 Grate = 0.0;
	f32 Frate = 2000;
	f32 pos = 0;
	msg_cmg_state statevar;
	unsigned char cmd = 0;
	int count4=0;
	int count5=0;
	
	cmgGimMode(actID, 1);
	usleep(1000*1000);

	cmgGimRate(actID, mode, Grate);
	do
	{
	queryCMGstate();
	usleep(1000*1000);
	statevar = parseTelem();
	count5+=1;
	cout<<"Setting GIM rate to 0 ---------------- "<<endl;
	} while (((statevar.gimbal_rate[actID]>0.1) || (statevar.gimbal_rate[actID]< -0.1))&& count5<30 );
	count5 =0;
	cmgGimMode(actID, 2);
	usleep(1000*1000);

	cmgGimPos(actID, mode, pos);
	do
	{
	queryCMGstate();
	usleep(1000*1000);
	statevar = parseTelem();
	count5+=1;
	cout<<"Setting GIM POS to 0 ---------------- "<<endl;
	} while (((statevar.gimbal_angle[actID]>1) || (statevar.gimbal_angle[actID]< -1))&& count5<20 );

	cmgGimEnable(actID, cmd);

	cmgFlywheelRate(actID, Frate);

	do
	{
	queryCMGstate();
	usleep(1000*1000);
	count4+=1;
	statevar = parseTelem();
	cout<< "Fly rate to zero current FLY Rate: "<<statevar.fly_rate[actID]<<endl;
	} while ((statevar.fly_rate[actID]>2020) && (count4 < 210));

	cmgFlyEnable(actID, 0);


}

// ------------------------ Initializing CMGs ------------------------
void cmgOffAll(){
	cmgOff(0);
	usleep(1000*1000);
	cmgOff(1);
	usleep(1000*1000);
	cmgOff(2);
	usleep(1000*1000);
	cmgOff(3);
	usleep(1000*1000);

	closeCMG();

}


void positionSweep(unsigned char actID){
	unsigned char mode = 1;
	f32 pos = 45.0;
	msg_cmg_state statePoint;

	cmgGimEnable(actID, mode);
	usleep(1000*1000);


	cmgGimMode(actID, 2);
	usleep(1000*1000);

	do
	{
	cmgGimPos(actID,mode,pos);
	usleep(5000*1000);
	queryCMGstate();
	usleep(1000*1000);
	statePoint = parseTelem();
	cout<< "Gim pos to 45 current Position: "<<statePoint.gimbal_angle[actID]<<endl;
	if(statePoint.gimbal_angle[actID] > 46 || statePoint.gimbal_angle[actID] < 44){
		printf("Gimbal position incorrect breaking out.\n");
		break;
	}

	usleep(10000*1000);
	pos = -45.0;

	cmgGimPos(actID,mode,pos);
	usleep(5000*1000);
	queryCMGstate();
	usleep(1000*1000);
	statePoint = parseTelem();
	cout<< "Gim pos to -45 current Position: "<<statePoint.gimbal_angle[actID]<<endl;
	if(statePoint.gimbal_angle[actID] > -44 || statePoint.gimbal_angle[actID] < -46){
		printf("Gimbal position incorrect breaking out.\n");
		break;
	}

	
	usleep(10000*1000);
	pos = 135.0;

	cmgGimPos(actID,mode,pos);
	usleep(5000*1000);
	queryCMGstate();
	usleep(1000*1000);
	statePoint = parseTelem();
	cout<< "Gim pos to 135 current Position: "<<statePoint.gimbal_angle[actID]<<endl;
	if(statePoint.gimbal_angle[actID] > 136 || statePoint.gimbal_angle[actID] < 134){
		printf("Gimbal position incorrect breaking out.\n");
		break;
	}

	usleep(10000*1000);
	pos = -135.0;

	cmgGimPos(actID,mode,pos);
	usleep(5000*1000);
	queryCMGstate();
	usleep(1000*1000);
	statePoint = parseTelem();
	cout<< "Gim pos to -135 current Position: "<<statePoint.gimbal_angle[actID]<<endl;
	if(statePoint.gimbal_angle[actID] > -134 || statePoint.gimbal_angle[actID] < -136){
		printf("Gimbal position incorrect breaking out.\n");
		break;
	}

	} while(false);

}


void gimbalSweep(unsigned char actID, f32 rate){
	unsigned char mode = 1;
	msg_cmg_state statePoint;
        int count3 =0;

	cmgGimMode(actID, 1);
	usleep(1000*1000);

	cmgGimRate(actID, mode, rate);
	usleep(1000*1000);

	do
	{
	queryCMGstate();
	usleep(1000*1000);
	count3+=1;
	statePoint = parseTelem();
	cout<< "Gim rate: "<<statePoint.gimbal_rate[actID]<<" For CMG: "<<actID<<endl;
	} while (statePoint.gimbal_rate[actID]<(15/4)&& count3<60);

	usleep(20000*1000);


}

// ------------------------ Initializing CMGs ------------------------
void manualOps()
{
	msg_cmg_state statePoint;
	int actID1;
	unsigned char actID;
	f32 pos;
	f32 rate;
	unsigned char mode = 1;
	int count3 =0;
	f32 mat11;
	f32 mat12;
	f32 mat13;
	f32 mat21;
	f32 mat22;
	f32 mat23;
	f32 mat31;
	f32 mat32;
	f32 mat33;
	f32 x;
	f32 y;
	f32 z;
	int w;
	float torqueTime;
	string response;

	do
	{


	cout << " What Function ? (spell correctly Gimpos,GimRate,CosMat,Torque): " << endl;
	cin >> response;

	if(response == "Gimpos")
	{
	w = 1;
	}
	else if(response == "GimRate")
	{
	w = 2;
	}
	else if(response == "CosMat")
	{
	w = 3;
	}
	else if(response == "Torque"){
	w = 4;
	}
	else if(response == "exit"){
	w = 5;
	}
	else
	{
		w = 0;
	}


	switch (w) {


  	case 1:

    cout << "What actID ?" << endl;
    cin >> actID1;
    actID = (unsigned char) actID1;
	cout << "What pos?" << endl;
	cin>> pos;

	cmgGimEnable(actID, mode);
	usleep(1000*1000);


	cmgGimMode(actID, 2);
	usleep(1000*1000);

	cmgGimPos(actID,mode,pos);
	usleep(5000*1000);
	queryCMGstate();
	usleep(1000*1000);
	statePoint = parseTelem();
	usleep(1000*1000);
	cout<< "Gim pos : "<<statePoint.gimbal_angle[actID]<<endl;
	if(statePoint.gimbal_angle[actID] > pos+1 || statePoint.gimbal_angle[actID] < pos-1){
		printf("Gimbal position incorrect breaking out.\n");
		break;
	}

    break;

  	case 2:

    cout << "What actID?"<<endl;
	cin >> actID1;
	actID = (unsigned char) actID1;
	cout << "What  Rate?"<<endl;
	cin >> rate;

    cmgGimMode(actID, 1);
	usleep(1000*1000);

	cmgGimRate(actID, mode, rate);
	usleep(1000*1000);

	do
	{
	queryCMGstate();
	usleep(1000*1000);
	count3+=1;
	statePoint = parseTelem();
	cout<< "Gim rate: "<<statePoint.gimbal_rate[actID]<<" For CMG: "<<actID<<endl;
	} while (statePoint.gimbal_rate[actID]<(15/4) && count3<60);

    break;

    case 3:

    cout << "What actID?"<<endl;
    cin >> actID1;
    actID = (unsigned char) actID1; 
	cout << "What 11 matrix value?"<<endl;
	cin>> mat11;
	cout << "What 12 matrix value?"<<endl;
	cin >> mat12 ;
	cout << "What 13 matrix value?"<<endl;
	cin>> mat13;
	cout << "What 21 matrix value?"<<endl;
	cin >> mat21 ;
	cout << "What 22 matrix value?"<<endl;
	cin >> mat22 ;
	cout << "What 23 matrix value?"<<endl;
	cin >> mat23 ;
	cout << "What 31 matrix value?"<<endl;
	cin >> mat31 ;
	cout << "What 32 matrix value?"<<endl;
	cin >> mat32 ;
	cout << "What 33 matrix value?"<<endl;
	cin>> mat33;

    cmgSetCosine(actID, mat11, mat12, mat13, mat21, mat22, mat23, mat31, mat32, mat33);
	usleep(2000*1000);

	break;

	case 4:
	cout << "What x torque? (in N-m)"<<endl;
	cin>>x;
	cout << "What y torque? (in N-m)"<<endl;
	cin>>y;
 	cout << "What z torque? (in N-m)"<<endl;
	cin>>z;
	cout << "Time duration of torque? (in seconds)"<<endl;
	cin>>torqueTime;


	queryCMGstate();
	usleep(1000*1000);
	statePoint = parseTelem();
	cmgSetTorques(x, y, z);
	usleep(1000*1000);
	queryCMGstate();
	usleep(1000*1000);
	statePoint = parseTelem();
	usleep(torqueTime*1000*1000);

	cout << " Torques set to ZERO ----------------------------------"<<endl;

	queryCMGstate();
	usleep(1000*1000);
	statePoint = parseTelem();
	cmgSetTorques(0, 0, 0);
	usleep(1000*1000);
	queryCMGstate();
	usleep(1000*1000);
	statePoint = parseTelem();


	break;

	case 5:
	cout << "Exiting"<<endl;
	break;




  default:
    cout << "incorrect function";
  }

} while (response != "exit");


}


int main(){
msg_cmg_state statePoint;

	cmgOpenPort();
	usleep(1000*1000);
	initAll();
	usleep(1000*1000);

	manualOps();

	cout<<"CMG Shut Off procedures Commencing----------------------------------"<<endl;

	cmgOffAll();
	return 0;

}	