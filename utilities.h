/**
 * @file utilities.htmlinclude [block] file-name
 * @author Felix P. Basiya Jr (you@domain.com)
 * @brief some utilities ex. for GPIB control, file handling, etc.
 * @version 0.1
 * @date 2024-12-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef UTILITIES_H
#define UTILITIES_H

#include "2EDL501xxxU2x_A0_4_R25_2018A.h" ///< Main project .h
#include "MST_Config.h" ///< \todo  this should not be needed, but there is some include order issue
#include <chrono>

using namespace std::chrono;


class gpibCon
{
    private:
        int address;
        int timeout;
        double range;
        double resolution;
        double voltage;
        double current;

    public:

		gpibCon(int addr) : address(addr){};

        //void gpibsend(int addr, char* msg);
        //void gpibreceive(int addr, char* msg, int size);
        void set_address(int addr);
        int get_address();
		double getMeas(char* unit);
		double getMeas2();


		
        bool gpib_connect();
		bool gpib_reset();
        bool gpib_init();
		bool gpib_acal(char* command);
        bool gpib_measMode(char* cmd1, char* cmd2);

        //template <typename T>
        bool gpib_setRange(char* cm1,double range);
        //set autoszero prior dc measurement: AZERO ON
        //set offset compensation : OCOMP ON
    
};

#endif // UTILITIES_H