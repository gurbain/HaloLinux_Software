#include "cmgCOM.h"
#include "typedef.h" 
#include "crc16.h"
#include <pthread.h>


using namespace std;

int serial_fd; 
int retVal = 0;
unsigned char msgTelem[256];
int counter= 0;
ofstream myfile;

// ------------------------ Read Response ------------------------

void cmgGSP::cmgRead(){ 
	msg_cmg_state statePoint;
	unsigned char messagelength;
	msg_header msgHeader; //msg_header is defined in cmgCOM.h
	msgHeader.startByte = 0;
	msgHeader.length = 0;
	msgHeader.ID = 0;
	int totalBytes = 0;
	int ReadCount = 0;
	string res;


	cout<<"in read function"<<endl;
	while(1) 
	{
		while (totalBytes == 0) {
			retVal = read(serial_fd, (unsigned char*)&msgHeader,1);

			if (retVal == -1)
				{printf("read() message failed: %s\n", strerror(errno));}

			if (msgHeader.startByte != FROM_CMG) {
				if(msgHeader.startByte == NEG_ACK)
				{
					cout<<"Negative Acknowledgement"<<endl;
				}
				else if(msgHeader.startByte == CRC_ERR){
					cout<<"CRC Error"<<endl;
				}
				else if(msgHeader.startByte == BAD_CMD_ID){
					cout<<"Bad Command ID"<<endl;
				}
				else if(msgHeader.startByte == INVLD_SETVAL){
					cout<<"Invalid Set Value"<<endl;
				}
				//cout<< "startByte: "<<(bitset<8>) msgHeader.startByte<<endl;
				else{
				printf("Bad start byte on read()\n");
				}
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

	cout << "totalBytes: " << totalBytes << endl;

		if (msgHeader.ID == CMD_SET_TORQUE && msgHeader.length == 17 ){

			msg_header *mHeadTorque;
			mHeadTorque = (msg_header *)&msgTelem;

			mHeadTorque->startByte = msgHeader.startByte;
			mHeadTorque->ID = msgHeader.ID;
			mHeadTorque->length = msgHeader.length;
			while (totalBytes < 20){
				retVal = read(serial_fd, (unsigned char*)&msgTelem[206+totalBytes], 1);

				if(retVal == -1)
					{printf("read() message failed: %s\n", strerror(errno));}

				totalBytes += retVal;	
			}
			printf("MSG2 RCV: StartByte=0x%X, ID=0x%X, Length=0x%X\n", msgHeader.startByte, msgHeader.ID, msgHeader.length);
			busy = false;
			break;
		}

		if (msgHeader.ID == GET_TELEM){

			while (totalBytes < 208) {
				retVal = read(serial_fd, (unsigned char*)&msgTelem[totalBytes-3], 1);
				
				if (retVal == -1)
				{
					printf("read() message failed: %s\n", strerror(errno));
				}
				totalBytes+=retVal;
			}
			printf("MSG2 RCV: StartByte=0x%X, ID=0x%X, Length=0x%X\n", msgHeader.startByte, msgHeader.ID, msgHeader.length);
			statePoint = parseTelem();
			break;
		}

		else
		{
			printf("MSG2 RCV: StartByte=0x%X, ID=0x%X, Length=0x%X\n", msgHeader.startByte, msgHeader.ID, msgHeader.length);
			busy = false;
			break;
		}
			
	}
}

// ------------------------ Close Communication with CMG ------------------------
void cmgGSP::closeCMG() //shuts down CMG
{

	//close serial port
	myfile.close();
	close(serial_fd);
	comm = false;

	printf("Closed cmg\n");

}


// ------------------------ Parsing Telemetry Response ------------------------
cmgGSP::msg_cmg_state cmgGSP::parseTelem(){
	msg_cmg_state cmgState;

	float telem_array[56];
	bool accept_bit;

	for(int i = 0; i<4;i++){
		f32 buffer;
		unsigned char b[] = {msgTelem[3+(4*i)], msgTelem[2+(4*i)], msgTelem[1+(4*i)], msgTelem[0+(4*i)]};
		memcpy(&buffer, &b, sizeof(buffer));
		telem_array [i] = buffer;

	}

	for(int i = 4; i<8;i++){
		unsigned int buffer;
		unsigned char b[] = {msgTelem[0+(4*i)], msgTelem[1+(4*i)], msgTelem[2+(4*i)], msgTelem[3+(4*i)]};
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

	memcpy(&accept_bit, &msgTelem[209], sizeof(accept_bit));


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


	write_data(telem_array, accept_bit);
	
	return cmgState;
}

// ------------------------ Saving Telemetry Response to TelemData.csv  ------------------------
void cmgGSP::write_data(float * telem_array, bool accept_bit){

	string columns[58] = {"Time", 
						  "Motor Bus Volts 0", "Motor Bus Volts 1", 
						  "TempLoc 0 ", "TempLoc 1 ",
						  "status 0", "status 1", "status 2", "status 3",
						  "Gmb Pos 0","Gmb Pos 1","Gmb Pos 2","Gmb Pos 3",
						  "Gmb Pos ERR 0","Gmb Pos ERR 1","Gmb Pos ERR 2","Gmb Pos ERR 3",
						  "Gmb Pos CMD 0","Gmb Pos CMD 1","Gmb Pos CMD 2","Gmb Pos CMD 3",
						  "Gmb Rate 0","Gmb Rate 1","Gmb Rate 2","Gmb Rate 3",
						  "Gmb Rate CMD 0","Gmb Rate CMD 1","Gmb Rate CMD 2","Gmb Rate CMD 3",
						  "Gmb Current 0","Gmb Current 1","Gmb Current 2","Gmb Current 3",
						  "Fly Rate 0","Fly Rate 1","Fly Rate 2","Fly Rate 3",
						  "Fly Rate CMD 0","Fly Rate CMD 1","Fly Rate CMD 2","Fly Rate CMD 3",
						  "Fly Current 0","Fly Current 1","Fly Current 2","Fly Current 3",
						  "TempPri 0","TempPri 1","TempPri 2","TempPri 3",
						  "TempAux 0","TempAux 1","TempAux 2","TempAux 3", 
						  "Rate 0", "Rate 1", "Rate 2", "Rate 3","Accept"};

	time_t now = time(0);
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
                        myfile << ","; }
            }
            myfile << endl;
	}
	
    for( int w = 0; w < 58; w++ )
    {
    	if (w==0){
    		myfile << dt;
    	}
	else if (w==57){
	myfile << accept_bit;	
	}	
    	else{
    	myfile << telem_array[w-1];
	}
    	if( w != 57 ){
    		myfile << ",";
        }
		
        
    }

    myfile << endl;
counter += 1;

}

// ------------------------ Initializing CMGs ------------------------
void cmgGSP::cmgInit(unsigned char actID){
	unsigned char mode = 1;
	f32 rate = 6000.0;
	msg_cmg_state statevar;
	int count2 = 0;

	cmgGimHome(actID);
	usleep(10000*1000);
	cmgFlyEnable(actID, 1);
	usleep(3000*1000);
	cmgFlywheelRate(actID, rate);
	if(actID==3){
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
	} while ((statevar.fly_rate[actID]<5995 && count2 < 210)); 
	}
	else{
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
	} while ((statevar.fly_rate[actID]<2050 && count2 < 210));
	}

}

// ------------------------ Initializing CMGs ------------------------

void cmgGSP::initAll(){
	unsigned char mode = 1;
	unsigned char actID;
	f32 pos;
	msg_cmg_state statePoint;
	f32 mat11 = 0;
	f32 mat12 = 0;
	f32 mat13 = 1;
	f32 mat21 = -1;
	f32 mat22 = 0;
	f32 mat23 = 0;
	f32 mat31 = 0;
	f32 mat32 = -1;
	f32 mat33 = 0;

	f32 mat211 = 0;
	f32 mat212 = -1;
	f32 mat213 = 0;
	f32 mat221 = 1;
	f32 mat222 = 0;
	f32 mat223 = 0;
	f32 mat231 = 0;
	f32 mat232 = 0;
	f32 mat233 = 1;

	cmgInit(0);
	cmgInit(2);
	cmgInit(1);
	cmgInit(3);

	for(int i=0; i<4; i++)
	{
		actID = (unsigned char) i;
		if(i==0 || i==3) {pos = -135;}
		else{ pos = -45;}


		cmgGimEnable(actID, mode);
	// usleep(1000*1000);
		cmgGimMode(actID, 2);
	// usleep(1000*1000);
		cmgGimPos(actID,mode,pos);
		usleep(5000*1000);
		queryCMGstate();
	// usleep(1000*1000);
		statePoint = parseTelem();
	// usleep(1000*1000);
		cout<< "Gim pos : "<<statePoint.gimbal_angle[actID]<<endl;
		if(statePoint.gimbal_angle[actID] > pos+1 || statePoint.gimbal_angle[actID] < pos-1){
			printf("Gimbal position incorrect.\n");
		}
	}

	cmgSetCosine(0, mat11, mat12, mat13, mat21, mat22, mat23, mat31, mat32, mat33);
	cmgSetCosine(1, mat11, mat12, mat13, mat21, mat22, mat23, mat31, mat32, mat33);
	cmgSetCosine(2, mat211, mat212, mat213, mat221, mat222, mat223, mat231, mat232, mat233);
	cmgSetCosine(3, mat211, mat212, mat213, mat221, mat222, mat223, mat231, mat232, mat233);


}

// ------------------------ Shutting Down the CMGs ------------------------

void cmgGSP::cmgOff(unsigned char actID){
	unsigned char mode = 1;
	f32 Grate = 0.0;
	f32 Frate = 2000;
	f32 pos = 0;
	msg_cmg_state statevar;
	unsigned char cmd = 0;
	int count4=0;
	int count5=0;
	
	cmgGimMode(actID, 1);
	// usleep(1000*1000);

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
	// usleep(1000*1000);

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

void cmgGSP::cmgOffAll(){
	cmgOff(0);
	cmgOff(1);
	cmgOff(2);
	cmgOff(3);

	closeCMG();

}

//--------------- Manually Operating the CMGs --------------

void cmgGSP::manualOps()
{
	msg_cmg_state statePoint;
	int actID1;
	unsigned char actID;
	f32 pos;
	f32 rate;
	unsigned char mode = 1;
	int count3 =0;
	f32 x;
	f32 y;
	f32 z;
	int w;
	float torqueTime;
	string response;

	do
	{


	cout << " What Function ? (spell correctly Gimpos,GimRate,CosMat,Torque, exit): " << endl;
	cin >> response;

	if(response == "Gimpos")
	{
	w = 1;
	}
	else if(response == "GimRate")
	{
	w = 2;
	}
	else if(response == "Torque"){
	w = 3;
	}
	else if(response == "exit"){
	w = 4;
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


	cmgGimMode(actID, 2);

	cmgGimPos(actID,mode,pos);
	usleep(5000*1000);
	queryCMGstate();
	// usleep(1000*1000);
	statePoint = parseTelem();
	// usleep(1000*1000);
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
	// usleep(1000*1000);

	cmgGimRate(actID, mode, rate);
	// usleep(1000*1000);

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
	cout << "What x torque? (in N-m)"<<endl;
	cin>>x;
	cout << "What y torque? (in N-m)"<<endl;
	cin>>y;
 	cout << "What z torque? (in N-m)"<<endl;
	cin>>z;
	cout << "Time duration of torque? (in seconds)"<<endl;
	cin>>torqueTime;


	queryCMGstate();
	// usleep(1000*1000);
	statePoint = parseTelem();
	// set to torque mode
	cmgGimMode(All_CMG, 4);
	// set torque
	cmgSetTorques(x, y, z);
	usleep(1000*1000);
	queryCMGstate();
	// usleep(1000*1000);
	statePoint = parseTelem();
	usleep(torqueTime*1000*1000);

	cout << " Torques set to ZERO ----------------------------------"<<endl;

	queryCMGstate();
	// usleep(1000*1000);
	statePoint = parseTelem();
	//set to torque mode
	cmgGimMode(All_CMG, 4);
	cmgSetTorques(0, 0, 0);
	usleep(1000*1000);
	queryCMGstate();
	// usleep(1000*1000);
	statePoint = parseTelem();


	break;

	case 4:
	cout << "Exiting"<<endl;
	break;




  default:
    cout << "Incorrect function";
  }

} while (response != "exit");


}


void cmgGSP::init_pthread(){ //creates thread

int retVal1 = pthread_create(&cmgtelem, NULL, this->telem_thread_helper, this);

		if (retVal1 != 0)

		{

			printf("pthread_create CMGThread failed\n");

			return exit(1);

		}
}
