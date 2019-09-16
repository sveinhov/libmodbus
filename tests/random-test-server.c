/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <errno.h>

#include <modbus.h>

// basic file operations
//#include <iostream>
//#include <fstream>
//using namespace std;

int main(void)
{
	modbus_t *ctx;
	//allocate enough memory to store the results in dest (at least nb * sizeof(uint16_t)), = nb * 2.
	uint16_t tab_reg[80];
	uint16_t tab_regb[80];
	uint16_t sample[2];
	uint16_t samplewrite[2];
	int rc;
	int i;
	int rc2;

	ctx = modbus_new_tcp("192.168.51.31", 502);
	if (modbus_connect(ctx) == -1) {
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}
	//READ DATA REGISTERS:
	rc = modbus_read_registers(ctx, 12388, 40, tab_reg);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return -1;
	}
	
	FILE *f = fopen("raw.txt", "w");
	FILE *fo = fopen("out.txt", "w");

	for (i = 0; i < rc; i+=2) {
				
		printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
		fprintf(f, "%u \n%u \n", tab_reg[i], tab_reg[i+1]);
		
		float test = 0;

		
		if (i == 8){
			test = tab_reg[i];//Read 16bit register for SBP Setpoint
		}
		else{
			sample[0] = tab_reg[i];
			sample[1] = tab_reg[i + 1];
			test = modbus_get_float_badc(sample);
		}		
		
		fprintf(fo, "%f \n", test);
	}
	
	fclose(f);
	fclose(fo);

	//READ BOOL REGISTERS:


	rc = modbus_read_registers(ctx, 12289, 33, tab_regb);
	

	FILE *fb = fopen("rawb.txt", "w");
	FILE *fob = fopen("outb.txt", "w");

	for (i = 0; i < rc; i++) {

		fprintf(fb, "%i\n", tab_regb[i]);
		
		//Using masking
		//uint16_t  k = 2048;
		//uint16_t mask = 1 << k;
		//uint16_t masked_n = tab_regb[i] & mask;
		uint16_t mask = 2048;
		uint16_t testb = -1;
		testb = tab_regb[i] & mask;
		//uint16_t thebit = masked_n >> k;
		
		//fprintf(fob, "%i\n", thebit);
	}

	fclose(fb);
	fclose(fob);

	//samplewrite
	float anumber;
	anumber	= 45.00;
	modbus_set_float_badc(anumber, samplewrite);


	rc2 = modbus_write_registers(ctx, 12400, 2, samplewrite);


	//uint16_t one = 1;
	//rc2 = modbus_write_register(ctx, 412398, one);


	modbus_close(ctx);
	modbus_free(ctx);

    return 0;
}
