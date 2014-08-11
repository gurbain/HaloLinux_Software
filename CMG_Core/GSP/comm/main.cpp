
#include "cmgCOM.h" 

using namespace std;

int main(){
//msg_cmg_state statePoint;
	cmgGSP cmgGSP_object;

	cmgGSP_object.cmgOpenPort();
	int msgCount=0;
	cmgGSP_object.init_pthread();

	while(msgCount<40){
	usleep(2000*1000);
	msgCount+=1;
	cout<<"msgCount: "<<msgCount<<endl;
	cmgGSP_object.cmgPing();

}
	// usleep(1000*1000);
	//initAll();
	// usleep(1000*1000);

	//manualOps();

	//cout<<"CMG Shut Off procedures Commencing----------------------------------"<<endl;

	//cmgOffAll();
	return 0;

}