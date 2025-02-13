/**
 * @file utilities.cpp
 * 
 * @ Felix P. Basiya Jr (felixcute@domain.com)
 * @brief: GPIB codes for measurement of DUT.. 
 * @version 0.1
 * @date 2024-12-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "utilities.h"

//void gpibCon::gpibsend(int addr, char* msg)
//{
//    // send message to gpib
//}

void gpibCon::set_address(int addr)
{
    address = addr;
}

int gpibCon::get_address()
{
    return address;
}

//void gpibCon::gpibreceive(int addr, char* msg, int size){
//	//do something..
//
//}




double gpibCon::getMeas2() {

	/********
	First, several variables are declared, including value and nsamp for storing measurement results, gstat and srq_stat for storing status codes, 
	and several character arrays for storing command and data strings. The DMM_ADDRS and MEAS constants are set to specific integer values representing 
	the device address and a measurement command, respectively.

	The code then defines several arrays and structures used for GPIB communication. The ladd and tadd arrays store the listen and talk addresses for the GPIB device, 
	respectively. The gp_talk structure is initialized with the ladd array and other parameters to configure the GPIB send operation. Similarly, the gp_listn structure 
	is initialized with the tadd array and other parameters to configure the GPIB receive operation. The spl_add array and spl_str structure are used for a special polling operation.

	The code proceeds to send a command to the GPIB device to take a measurement by formatting the command string into the tdat array and calling the gpib function with the HP_SEND
	operation and the gp_talk structure. It then performs a special polling operation using the gpib function with the HP_CSPOL operation and the spl_str structure.

	Next, the code retrieves the mean of the measurements by sending the "RMATH MEAN" command to the GPIB device and receiving the response into the ldat array. 
	The response is parsed using sscanf to extract the mean value into the value variable.

	Finally, the code retrieves the number of measurements by sending the "RMATH NSAMP" command to the GPIB device and receiving the response into the ldat array. 
	The response is parsed using sscanf to extract the number of samples into the nsamp variable. The function then returns the value variable, which contains the mean of the measurements.
	************/

	double  value, nsamp;
	int  gstat, srq_stat;
	char  buf[30];
	char near tdat[80];
	int DMM_ADDRS=22;
	int MEAS=18;

	char near ladd[] = { DMM_ADDRS, -1, -1, -1 }; //{HP_LAD, -1, -1, -1 };   // listen  strunct 
	char near tadd[] = { DMM_ADDRS, -1, -1, -1 }; // { HP_TAD, -1, -1, -1 }; // talk struct

	/* GPIB listen address */
	HP_GPIBSEND_STR gp_talk = {ladd,1,'\r',80,tdat};
	
	/* GPIB listen address */
	char near ldat[130]; /* GPIB listen data buffer */
	HP_GPIBRECV_STR gp_listn = { tadd, 1, '\r', 128, ldat };
	char near spl_add[] = { DMM_ADDRS, -1 }; //{ DMM_ADDRS + HP_TAD, -1 };
	char near spl_dat[5];
	HP_GPIBSPOL_STR spl_str = { spl_add, spl_dat };

	/* take measurement*/
	sprintf(tdat, "MEM FIFO; CALL MEAS\r");
	gstat = gpib(HP_SEND, &gp_talk);
	srq_stat = gpib(HP_CSPOL, &spl_str);

	/* Retrieve mean of NSAMP measurements, put into value */
	sprintf(tdat, "RMATH MEAN\r");
	gstat = gpib(HP_SEND, &gp_talk);
	if (gpib(HP_RECV, &gp_listn))
		sscanf(ldat, "%lf", &value);

	/* Retrieve number of measurements*/
	sprintf(tdat, "RMATH NSAMP\r");
	gstat = gpib(HP_SEND, &gp_talk );
	if(gpib(HP_RECV, &gp_listn))
	sscanf(ldat, "%lf", &nsamp);

	return value;
	
}


double gpibCon::getMeas(char* unit) {
	
	auto start = high_resolution_clock::now();							//start of time stamp
	try {
		// connect to gpib
		//char msg[200] = "\0";
		//double* rec;
		char msg[200];
		char* ptr;
			

		//gpibsend(address, "*IDN?");
		gpibsend(address, "TARM", "HOLD");
		gpibsend(address, "TRIG", "AUTO");
		gpibsend(address, "NDIG", "8");
		gpibsend(address, "RQS", "33");
		gpibreceive(address, msg, 100);
		
		double reading = strtod(msg, &ptr);

		if (msg != NULL)
		{
// 
			etsprintf("Connected to address: %i\n", address);
			etsprintf("Measurement Value in %s : is %0.8f\n ",unit,reading);
			//etsprintf("%s\n",ptr);
			auto stop = high_resolution_clock::now();					//end of time stamp
			auto duration = duration_cast<microseconds>(stop - start);
			etsprintf("time duration measurement in uS: %i\n", duration.count());

			return reading;
		}
		else
		{
			etsprintf("Failed to connect to GPIB device");
			return 0;
		}
	}
	catch (const std::exception& e) {
		etsprintf("Error!");
		return 0;
	}
}

bool gpibCon::gpib_connect()
{

	auto start= high_resolution_clock::now();

	try{
		// connect to gpib

		gpibsend(address, "*IDN?");

		etsprintf("Connected to address: %d\n", address);

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		etsprintf("time duration in uS: %i\n", duration.count());
		return true;

	}
	catch (const std::exception& e) {
		etsprintf("Error!");
		return false;
	}

}

bool gpibCon::gpib_reset() {
	try {
		gpibsend(address, "RESET");
		return true;
	}
	catch(const std::exception& e)
		{return false; }

}

bool gpibCon::gpib_init()
{
    //set_address(addr);
    //bool connected = gpib_connect();

    try
    {
        // do some initialization
		//gpibsend(address,"RESET");

		// initial settings
		//bool conn;
		gpibmode(TRUE);
		//conn = remoteDMM.gpib_connect();
		//gpibsend(address, "RESET");
		int ret = gpib(HP_STAT, 1);
		gpib(HP_SDR, 0);
		gpib(HP_SDC, 0);
		gpibsend(address, "BEEP", "OFF");
		gpibsend(address, "AZERO", "OFF");
		gpibsend(address, "DISP", "ON");
		//gpibsend(22, "MFORMAT", "DINT");
		//gpibsend(22, "OFORMAT", "DINT");

		//gpibsend(22, "TARM", "SYN");
		//gpibsend(22, "TARM", "SGL");

		gpibsend(address, "NRDGS", "10", "AUTO");
		gpibsend(address, "NPLC", "1");
		gpibsend(address, "INBUF", "ON");
		gpibsend(address, "END", "ON");
		gpibsend(address, "TARM", "HOLD");
		gpibsend(address, "TRIG", "AUTO");
		gpibsend(address, "NDIG", "8");
		gpibsend(address, "RQS", "33");
		//gpibsend(22, "ID?\r");
		//gpibsend(22, "TARM", "SGL", "1");

		//gpibsend(22, "MEM","FIFO");
		//gpibsend(22, "CALL", "18");


		
        return true;
    }
    catch (const std::exception& e)
    {
		etsprintf("Error!");
        return false;
    }
}


    //ACAL DCV/OHMS/ACV/ALL
bool gpibCon::gpib_acal(char* command)
{
    // do auto calibration

    gpibsend(address, "ACAL", command);
    return true;
}


bool gpibCon::gpib_measMode(char* cmd1, char* cmd2)
{
	// send command: FUNC DCV/ACV/DCI/ACI/RES/FREQ
	try{
		gpibsend(address, cmd1, cmd2);
		return true;
	}
	catch (const std::exception& e) {
		etsprintf("Error!");
		return false;
	}
}

//template <typename T>
bool gpibCon::gpib_setRange(char* cm1, double range)
{
	// range: "ONCE/ON"
	// cmd: ARANGE/DCV: 0.1, 1, 10, 100, 1000
	try {
		gpibsend(address, cm1, range);
		return true;
	}
	catch (const std::exception& e) {
		etsprintf("Error!");
		return false;
	}
}
