#ifndef NOC_TORUS
#define NOC_TORUS

#include "Definition.h"
#include "router.h"
//#include "electronic_processor.h"
#include "Processor_Uniform.h"
#include "global_data.h"
#include "time.h"
#include "stdio.h"
#include "math.h"
#include <iostream>
//////////////////////////////////////////////////////////
//
//	Definition of signals
//
//    				o
//    				|^
//    				||
//    				||
//   connectToLower || connectToUpper
//    				||
//    				||
//    				v|    connectToRight
//    				o ----------------------> o
//     				 <----------------------
//     				      connectToLeft
//
//////////////////////////////////////////////////////////


	class electronic_chip : public sc_module
{
public:

	sc_in<bool>				clk;

	Router***				router;//;//[NOC_WIDTH][NOC_HEIGHT][NOC_DEPTH];
	
	// signals for data
	sc_signal<router_forward_interface>***	connectToRight;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	connectToLeft;// ;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	connectToUpper;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	connectToLower;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	connectToProc ;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	connectFromProc;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	connectToTSVUpper;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	connectToTSVLower;//[NOC_WIDTH][NOC_HEIGHT];

	sc_signal<router_forward_interface>***	tearToRight;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	tearToLeft;// ;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	tearToUpper;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	tearToLower;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	tearToProc ;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	tearFromProc;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	tearToTSVUpper;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	tearToTSVLower;//[NOC_WIDTH][NOC_HEIGHT];



	sc_signal<router_forward_interface>***	partialToRight;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	partialToLeft;// ;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	partialToUpper;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	partialToLower;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	partialToProc ;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	partialFromProc;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	partialToTSVUpper;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	partialToTSVLower;//[NOC_WIDTH][NOC_HEIGHT];


	sc_signal<router_forward_interface>***	ackToRight;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	ackToLeft;// ;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	ackToUpper;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	ackToLower;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	ackToProc ;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	ackFromProc;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	ackToTSVUpper;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	ackToTSVLower;//[NOC_WIDTH][NOC_HEIGHT];


	sc_signal<router_forward_interface>***	dataout;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	datain;// ;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	tearout;//;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	tearin;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	partialout;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	partialin;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	ackout;//[NOC_WIDTH][NOC_HEIGHT];
	sc_signal<router_forward_interface>***	ackin;//[NOC_WIDTH][NOC_HEIGHT];

	sc_signal<bool>***			readyout;
	sc_signal<bool>***			readyin;
	sc_signal<bool>***			readytearout;
	sc_signal<bool>***			readytearin;
	sc_signal<bool>***			readypartialout;
	sc_signal<bool>***			readypartialin;
	sc_signal<bool>***			readyackout;
	sc_signal<bool>***			readyackin;
	// useful signals for data ready
	sc_signal<bool>***			readyConnectToRight;
	sc_signal<bool>***			readyConnectToLeft;
	sc_signal<bool>***			readyConnectToUpper;
	sc_signal<bool>***			readyConnectToLower;
	sc_signal<bool>***			readyConnectToProc;
	sc_signal<bool>***			readyConnectFromProc;
	sc_signal<bool>***			readyConnectToTSVUpper;
	sc_signal<bool>***			readyConnectToTSVLower;

	sc_signal<bool>***			readytearToRight;
	sc_signal<bool>***			readytearToLeft;
	sc_signal<bool>***			readytearToUpper;
	sc_signal<bool>***			readytearToLower;
	sc_signal<bool>***			readytearToProc;
	sc_signal<bool>***			readytearFromProc;
	sc_signal<bool>***			readytearToTSVUpper;
	sc_signal<bool>***			readytearToTSVLower;

	sc_signal<bool>***			readypartialToRight;
	sc_signal<bool>***			readypartialToLeft;
	sc_signal<bool>***			readypartialToUpper;
	sc_signal<bool>***			readypartialToLower;
	sc_signal<bool>***			readypartialToProc;
	sc_signal<bool>***			readypartialFromProc;
	sc_signal<bool>***			readypartialToTSVUpper;
	sc_signal<bool>***			readypartialToTSVLower;

	sc_signal<bool>***			readyackToRight;
	sc_signal<bool>***			readyackToLeft;
	sc_signal<bool>***			readyackToUpper;
	sc_signal<bool>***			readyackToLower;
	sc_signal<bool>***			readyackToProc;
	sc_signal<bool>***			readyackFromProc;
	sc_signal<bool>***			readyackToTSVUpper;
	sc_signal<bool>***			readyackToTSVLower;

	double total_delay;
	double total_data_in_flit;
	double total_num_of_pkt;
	double total_throughput;
	double average_pkt_delay;

	Processor** proc_list;
	
	//functions
	Processor& get_proc(int pid);
	
	void  init(sc_clock &global_clk);
	void construct();
	void check_finish();
	
	void declare_ports_signals();
	void test();
	void updating_q_table();


	SC_HAS_PROCESS(electronic_chip);
	electronic_chip(sc_module_name name=sc_gen_unique_name("electronic_chip")) : sc_module(name) {
		
		declare_ports_signals();
		construct();

		SC_METHOD(check_finish);
		sensitive<<clk.pos();

		SC_THREAD(test);
		sensitive << clk.pos();

		SC_THREAD(updating_q_table);
		sensitive << clk.pos();

	}

};

void electronic_chip :: declare_ports_signals()
{
		// signals for data
	connectToRight= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		connectToRight[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			connectToRight[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	connectToLeft= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		connectToLeft[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			connectToLeft[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	connectToUpper= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		connectToUpper[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			connectToUpper[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	connectToLower= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		connectToLower[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			connectToLower[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	connectToProc= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		connectToProc[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			connectToProc[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	connectFromProc= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		connectFromProc[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			connectFromProc[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	connectToTSVUpper= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		connectToTSVUpper[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			connectToTSVUpper[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	connectToTSVLower= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		connectToTSVLower[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			connectToTSVLower[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	
	tearToRight= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearToRight[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearToRight[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	tearToLeft= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearToLeft[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearToLeft[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	tearToUpper= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearToUpper[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearToUpper[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	tearToLower= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearToLower[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearToLower[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	tearToProc= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearToProc[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearToProc[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	tearFromProc= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearFromProc[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearFromProc[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	tearToTSVUpper= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearToTSVUpper[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearToTSVUpper[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	tearToTSVLower= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearToTSVLower[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearToTSVLower[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	
	partialToRight= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialToRight[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialToRight[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	partialToLeft= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialToLeft[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialToLeft[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	partialToUpper= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialToUpper[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialToUpper[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	partialToLower= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialToLower[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialToLower[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	partialToProc= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialToProc[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialToProc[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	partialFromProc= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialFromProc[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialFromProc[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	partialToTSVUpper= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialToTSVUpper[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialToTSVUpper[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	partialToTSVLower= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialToTSVLower[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialToTSVLower[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	


	

	ackToRight= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackToRight[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackToRight[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	ackToLeft= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackToLeft[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackToLeft[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	ackToUpper= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackToUpper[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackToUpper[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	ackToLower= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackToLower[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackToLower[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	ackToProc= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackToProc[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackToProc[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	ackFromProc= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackFromProc[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackFromProc[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	ackToTSVUpper= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackToTSVUpper[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackToTSVUpper[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	ackToTSVLower= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackToTSVLower[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackToTSVLower[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	

	dataout= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		dataout[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			dataout[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	datain= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		datain[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			datain[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	tearout= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearout[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearout[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	tearin= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		tearin[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			tearin[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	partialout= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialout[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialout[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	partialin= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		partialin[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			partialin[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	ackout= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackout[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackout[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}
	
	ackin= new sc_core::sc_signal<router_forward_interface>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		ackin[i]=new sc_core::sc_signal<router_forward_interface>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			ackin[i][j]=new sc_core::sc_signal<router_forward_interface> [NOC_DEPTH];
		}
	}

	///bool signals
	readyout= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyout[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyout[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyin= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyin[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyin[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readytearout= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearout[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearout[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readytearin= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearin[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearin[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readypartialout= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialout[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialout[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readypartialin= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialin[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialin[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	readyackout= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackout[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackout[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyackin= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackin[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackin[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}

	readyConnectToRight= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyConnectToRight[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyConnectToRight[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyConnectToLeft= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyConnectToLeft[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyConnectToLeft[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyConnectToUpper= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyConnectToUpper[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyConnectToUpper[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyConnectToLower= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyConnectToLower[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyConnectToLower[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyConnectToProc= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyConnectToProc[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyConnectToProc[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyConnectFromProc= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyConnectFromProc[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyConnectFromProc[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyConnectToTSVUpper= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyConnectToTSVUpper[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyConnectToTSVUpper[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyConnectToTSVLower= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyConnectToTSVLower[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyConnectToTSVLower[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}


	readytearToRight= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearToRight[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearToRight[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readytearToLeft= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearToLeft[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearToLeft[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readytearToUpper= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearToUpper[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearToUpper[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readytearToLower= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearToLower[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearToLower[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readytearToProc= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearToProc[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearToProc[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readytearFromProc= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearFromProc[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearFromProc[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readytearToTSVUpper= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearToTSVUpper[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearToTSVUpper[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readytearToTSVLower= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readytearToTSVLower[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readytearToTSVLower[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}

	readypartialToRight= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialToRight[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialToRight[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readypartialToLeft= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialToLeft[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialToLeft[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readypartialToUpper= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialToUpper[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialToUpper[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readypartialToLower= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialToLower[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialToLower[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readypartialToProc= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialToProc[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialToProc[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readypartialFromProc= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialFromProc[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialFromProc[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readypartialToTSVUpper= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialToTSVUpper[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialToTSVUpper[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readypartialToTSVLower= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readypartialToTSVLower[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readypartialToTSVLower[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyackToRight= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackToRight[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackToRight[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyackToLeft= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackToLeft[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackToLeft[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyackToUpper= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackToUpper[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackToUpper[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyackToLower= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackToLower[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackToLower[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyackToProc= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackToProc[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackToProc[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyackFromProc= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackFromProc[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackFromProc[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyackToTSVUpper= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackToTSVUpper[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackToTSVUpper[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}
	
	readyackToTSVLower= new sc_core::sc_signal<bool>** [NOC_WIDTH];//;//
	for(int i=0;i<NOC_WIDTH;i++)
	{
		readyackToTSVLower[i]=new sc_core::sc_signal<bool>* [NOC_HEIGHT];
		for(int j=0;j<NOC_HEIGHT;j++){
			readyackToTSVLower[i][j]=new sc_core::sc_signal<bool> [NOC_DEPTH];
		}
	}

	

	

}


void electronic_chip :: updating_q_table(){
	while (true){
		wait(0.2, SC_NS);
		//cout << "updating time is " << sc_time_stamp().to_double() / 1000 << endl;

		for( int i = 0; i < NOC_WIDTH ; i++){
			for( int j = 0; j < NOC_HEIGHT; j++){
				for( int k = 0; k < NOC_DEPTH; k++){
					for( int n = 0 ; n < 6; n++){
						int new_x = 0;
						int new_y = 0;
						int new_z = 0;

						qpocket temp_qpocket;
						flit_type temp_flit;

						if (router[i][j][k].updated_flag[n]){
							temp_qpocket = router[i][j][k].router_qpocket[n];
							temp_flit = temp_qpocket.trans_flit;
							new_z = temp_qpocket.trans_id /(NOC_WIDTH*NOC_HEIGHT);
							new_y = temp_qpocket.trans_id % (NOC_WIDTH*NOC_HEIGHT) / NOC_WIDTH;
							new_x = temp_qpocket.trans_id % (NOC_WIDTH*NOC_HEIGHT) % NOC_WIDTH;
							router[i][j][k].updated_flag[n] = false; 
							// cout << "trans_id: " << temp_qpocket.trans_id << endl;
							// cout << "(" << new_x << "," << new_y << "," << new_z << ")" <<endl;
							// cout << "trans_loss: " << temp_qpocket.loss <<endl;
							router[new_x][new_y][new_z].qtable[temp_qpocket.dst_id][temp_qpocket.trans_port - 1] = temp_qpocket.loss;


							if (temp_qpocket.dst_id == 10  && temp_qpocket.trans_id == 0 && temp_flit.src == 0) {
																
							}

						}
					}
				}
			}
		}
		wait();
	}
}




void electronic_chip :: init(sc_clock &global_clk) {
		for (int i=0; i<NOC_WIDTH; i++) {
			for (int j=0; j<NOC_HEIGHT; j++){
				for (int k=0; k<NOC_DEPTH; k++){
					router[i][j][k].clk(global_clk);
				}
			}
		}
		
	}

void electronic_chip ::test() {
	 while(true){
		wait(0.9, SC_NS);	 
		for (int i=0; i<NOC_WIDTH; i++) {
			for (int j=0; j<NOC_HEIGHT; j++){
				for (int k=0; k<NOC_DEPTH; k++){
					if(router[i][j][k].ack!=-1){
				        
						int p = router[i][j][k].ack;				
					
						(*proc_list[p]).success=1;												
					
						router[i][j][k].ack=-1;
											
					 
					}								
				}
			}
	  }
	  wait();
	}
}

void electronic_chip ::construct() {
		int i,j,k;
	//	cout << "@ " << sc_time_stamp() << " In construct() of NetworkOnChip" <<endl;

		// set the id and pos
		// initialize the routers
		
		
		router=new Router** [NOC_WIDTH];
		for(int i=0;i<NOC_WIDTH;i++)
		{
			router[i]=new Router* [NOC_HEIGHT];
			for (int j=0; j<NOC_HEIGHT; j++){
				router[i][j]=new Router[NOC_DEPTH];
			}
		}
		
		
		proc_list= new Processor *[processor_no];
		for(int i=0;i<processor_no;i++)
		{
			string name0="core_";
			std::stringstream out;
			out<<i;
			string n1;
			out>>n1;
			name0=name0+n1;
			char * name_temp;
			name_temp= new char [name0.size()+1];
			strcpy(name_temp, name0.c_str());
			proc_list[i]= new Processor(name_temp);
			(*proc_list[i]).set_id(i);
			(*proc_list[i]).clk(clk);
		}
		

		
		for (i=0; i<NOC_WIDTH; i++) {
			for (j=0; j<NOC_HEIGHT; j++){
				for (k=0; k<NOC_DEPTH; k++){
					router[i][j][k].setid(k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i);
					router[i][j][k].setpos(i,j,k);
				}
			}
		}

		// connect the routers
		for (i=0; i<NOC_WIDTH; i++) {
			for (j=0; j<NOC_HEIGHT; j++){
				for (k=0; k<NOC_DEPTH; k++){
					router[i][j][k].data_out[RIGHT](connectToRight[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].data_in[LEFT](connectToRight[i][j][k]);

					router[i][j][k].data_ready_out[RIGHT](readyConnectToRight[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].data_ready_in[LEFT](readyConnectToRight[i][j][k]);

					router[i][j][k].data_in[RIGHT](connectToLeft[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].data_out[LEFT](connectToLeft[i][j][k]);

					router[i][j][k].data_ready_in[RIGHT](readyConnectToLeft[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].data_ready_out[LEFT](readyConnectToLeft[i][j][k]);

					router[i][j][k].tear_out[RIGHT](tearToRight[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].tear_in[LEFT](tearToRight[i][j][k]);

					router[i][j][k].tear_ready_out[RIGHT](readytearToRight[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].tear_ready_in[LEFT](readytearToRight[i][j][k]);

					router[i][j][k].tear_in[RIGHT](tearToLeft[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].tear_out[LEFT](tearToLeft[i][j][k]);

					router[i][j][k].tear_ready_in[RIGHT](readytearToLeft[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].tear_ready_out[LEFT](readytearToLeft[i][j][k]); 

					router[i][j][k].partial_out[RIGHT](partialToRight[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].partial_in[LEFT](partialToRight[i][j][k]);

					router[i][j][k].partial_ready_out[RIGHT](readypartialToRight[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].partial_ready_in[LEFT](readypartialToRight[i][j][k]);

					router[i][j][k].partial_in[RIGHT](partialToLeft[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].partial_out[LEFT](partialToLeft[i][j][k]);

					router[i][j][k].partial_ready_in[RIGHT](readypartialToLeft[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].partial_ready_out[LEFT](readypartialToLeft[i][j][k]); 

					router[i][j][k].ack_out[RIGHT](ackToRight[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].ack_in[LEFT](ackToRight[i][j][k]);

					router[i][j][k].ack_ready_out[RIGHT](readyackToRight[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].ack_ready_in[LEFT](readyackToRight[i][j][k]);


					router[i][j][k].ack_in[RIGHT](ackToLeft[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].ack_out[LEFT](ackToLeft[i][j][k]);

					router[i][j][k].ack_ready_in[RIGHT](readyackToLeft[i][j][k]);
					router[(i+1)%NOC_WIDTH][j][k].ack_ready_out[LEFT](readyackToLeft[i][j][k]); 
				}
			}
		}
		
		
		
		for (i=0; i<NOC_WIDTH; i++) {
			for (j=0; j<NOC_HEIGHT; j++){
				for (k=0; k<NOC_DEPTH; k++){
					router[i][j][k].data_out[UP](connectToUpper[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].data_in[DOWN](connectToUpper[i][j][k]);

					router[i][j][k].data_ready_out[UP](readyConnectToUpper[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].data_ready_in[DOWN](readyConnectToUpper[i][j][k]);

					router[i][j][k].data_in[UP](connectToLower[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].data_out[DOWN](connectToLower[i][j][k]);

					router[i][j][k].data_ready_in[UP](readyConnectToLower[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].data_ready_out[DOWN](readyConnectToLower[i][j][k]);

				
					router[i][j][k].tear_out[UP](tearToUpper[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].tear_in[DOWN](tearToUpper[i][j][k]);

					router[i][j][k].tear_ready_out[UP](readytearToUpper[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].tear_ready_in[DOWN](readytearToUpper[i][j][k]);

					router[i][j][k].tear_in[UP](tearToLower[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].tear_out[DOWN](tearToLower[i][j][k]);

					router[i][j][k].tear_ready_in[UP](readytearToLower[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].tear_ready_out[DOWN](readytearToLower[i][j][k]);	
				
							
					router[i][j][k].partial_out[UP](partialToUpper[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].partial_in[DOWN](partialToUpper[i][j][k]);

					router[i][j][k].partial_ready_out[UP](readypartialToUpper[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].partial_ready_in[DOWN](readypartialToUpper[i][j][k]);


					router[i][j][k].partial_in[UP](partialToLower[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].partial_out[DOWN](partialToLower[i][j][k]);

					router[i][j][k].partial_ready_in[UP](readypartialToLower[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].partial_ready_out[DOWN](readypartialToLower[i][j][k]);
													
					router[i][j][k].ack_out[UP](ackToUpper[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].ack_in[DOWN](ackToUpper[i][j][k]);

					router[i][j][k].ack_ready_out[UP](readyackToUpper[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].ack_ready_in[DOWN](readyackToUpper[i][j][k]);

					router[i][j][k].ack_in[UP](ackToLower[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].ack_out[DOWN](ackToLower[i][j][k]);

					router[i][j][k].ack_ready_in[UP](readyackToLower[i][j][k]);
					router[i][(j+1)%NOC_HEIGHT][k].ack_ready_out[DOWN](readyackToLower[i][j][k]);
				}
			
			}
		}
		
		for (i=0; i<NOC_WIDTH; i++) {
			for (j=0; j<NOC_HEIGHT; j++){
				for (k=0; k<NOC_DEPTH; k++){
					router[i][j][k].data_out[TSV_UP](connectToTSVUpper[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].data_in[TSV_DOWN](connectToTSVUpper[i][j][k]);

					router[i][j][k].data_ready_out[TSV_UP](readyConnectToTSVUpper[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].data_ready_in[TSV_DOWN](readyConnectToTSVUpper[i][j][k]);

					router[i][j][k].data_in[TSV_UP](connectToTSVLower[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].data_out[TSV_DOWN](connectToTSVLower[i][j][k]);

					router[i][j][k].data_ready_in[TSV_UP](readyConnectToTSVLower[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].data_ready_out[TSV_DOWN](readyConnectToTSVLower[i][j][k]);

				
					router[i][j][k].tear_out[TSV_UP](tearToTSVUpper[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].tear_in[TSV_DOWN](tearToTSVUpper[i][j][k]);

					router[i][j][k].tear_ready_out[TSV_UP](readytearToTSVUpper[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].tear_ready_in[TSV_DOWN](readytearToTSVUpper[i][j][k]);

					router[i][j][k].tear_in[TSV_UP](tearToTSVLower[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].tear_out[TSV_DOWN](tearToTSVLower[i][j][k]);

					router[i][j][k].tear_ready_in[TSV_UP](readytearToTSVLower[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].tear_ready_out[TSV_DOWN](readytearToTSVLower[i][j][k]);	
				
							
					router[i][j][k].partial_out[TSV_UP](partialToTSVUpper[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].partial_in[TSV_DOWN](partialToTSVUpper[i][j][k]);

					router[i][j][k].partial_ready_out[TSV_UP](readypartialToTSVUpper[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].partial_ready_in[TSV_DOWN](readypartialToTSVUpper[i][j][k]);


					router[i][j][k].partial_in[TSV_UP](partialToTSVLower[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].partial_out[TSV_DOWN](partialToTSVLower[i][j][k]);

					router[i][j][k].partial_ready_in[TSV_UP](readypartialToTSVLower[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].partial_ready_out[TSV_DOWN](readypartialToTSVLower[i][j][k]);
													
					router[i][j][k].ack_out[TSV_UP](ackToTSVUpper[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].ack_in[TSV_DOWN](ackToTSVUpper[i][j][k]);

					router[i][j][k].ack_ready_out[TSV_UP](readyackToTSVUpper[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].ack_ready_in[TSV_DOWN](readyackToTSVUpper[i][j][k]);

					router[i][j][k].ack_in[TSV_UP](ackToTSVLower[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].ack_out[TSV_DOWN](ackToTSVLower[i][j][k]);

					router[i][j][k].ack_ready_in[TSV_UP](readyackToTSVLower[i][j][k]);
					router[i][j][(k+1)%NOC_DEPTH].ack_ready_out[TSV_DOWN](readyackToTSVLower[i][j][k]);
				}
			
			}
		}



		
		for (i=0; i<NOC_WIDTH; i++) {
			for (j=0; j<NOC_HEIGHT; j++){		
				for (k=0; k<NOC_DEPTH; k++){
					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).data_in(datain[i][j][k]);
					router[i][j][k].data_out[TO_PROCESSOR](datain[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).data_ready_in(readyin[i][j][k]);
					router[i][j][k].data_ready_out[TO_PROCESSOR](readyin[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).data_ready_out(readyout[i][j][k]);
					router[i][j][k].data_ready_in[TO_PROCESSOR](readyout[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).data_out(dataout[i][j][k]);
					router[i][j][k].data_in[TO_PROCESSOR](dataout[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).tear_out(tearout[i][j][k]);
					router[i][j][k].tear_in[TO_PROCESSOR](tearout[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).tear_ready_out(readytearout[i][j][k]);
					router[i][j][k].tear_ready_in[TO_PROCESSOR](readytearout[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).tear_in(tearin[i][j][k]);
					router[i][j][k].tear_out[TO_PROCESSOR](tearin[i][j][k]);
				
					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).tear_ready_in(readytearin[i][j][k]);
					router[i][j][k].tear_ready_out[TO_PROCESSOR](readytearin[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).partial_out(partialout[i][j][k]);
					router[i][j][k].partial_in[TO_PROCESSOR](partialout[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).partial_ready_out(readypartialout[i][j][k]);
					router[i][j][k].partial_ready_in[TO_PROCESSOR](readypartialout[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).partial_in(partialin[i][j][k]);
					router[i][j][k].partial_out[TO_PROCESSOR](partialin[i][j][k]);
				
					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).partial_ready_in(readypartialin[i][j][k]);
					router[i][j][k].partial_ready_out[TO_PROCESSOR](readypartialin[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).ack_out(ackout[i][j][k]);
					router[i][j][k].ack_in[TO_PROCESSOR](ackout[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).ack_ready_out(readyackout[i][j][k]);
					router[i][j][k].ack_ready_in[TO_PROCESSOR](readyackout[i][j][k]);

					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).ack_in(ackin[i][j][k]);
					router[i][j][k].ack_out[TO_PROCESSOR](ackin[i][j][k]);
				
					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).ack_ready_in(readyackin[i][j][k]);
					router[i][j][k].ack_ready_out[TO_PROCESSOR](readyackin[i][j][k]);				
				
				}
			}
		}
		for (i=0; i<NOC_WIDTH; i++) {
			for (j=0; j<NOC_HEIGHT; j++){
				for (k=0; k<NOC_DEPTH; k++){
					(*proc_list[k*(NOC_WIDTH*NOC_HEIGHT)+j*NOC_WIDTH+i]).initial();
				}
			}
		}
		
	}
	Processor& electronic_chip :: get_proc(int pid)
{

	for (unsigned int i=0; i<processor_no; i++) {
		Processor &p = (*proc_list[i]);
		if (pid == p.get_id()) {
			return p;
		}
	}
	cout <<pid<<" No processor is found with the given id." << endl;
	assert (0);
	
}


void electronic_chip ::check_finish() {
	total_delay=0;
	total_data_in_flit=0;
	total_num_of_pkt=0;
	total_throughput=0;
	average_pkt_delay=0;

	for(int i=0;i<processor_no;i++)
	{
		total_num_of_pkt=total_num_of_pkt+(*proc_list[i]).total_pkt_number;
		total_delay=total_delay+(*proc_list[i]).total_pkt_delay;
		total_data_in_flit=total_data_in_flit+(*proc_list[i]).total_data_size;
	}

	average_pkt_delay=total_delay/total_num_of_pkt; //in cycle (ns)
		
	total_throughput=(double)total_data_in_flit*flit_size/simulation_time; //In bits per cycle (bits per ns)

	
}




#endif


