#include "Definition.h"
#include "electronic_torus.h"
#include "global_data.h"
#include "time.h"
#include "stdio.h"
#include "math.h"

#include <iostream>
#include <fstream>

int sc_main(int argc, char* argv[]) {
	// 导入温度分布文件
	string file_name(argv[8]);
	if (argc == 9){
		injection_rate = atof(argv[1]);
		NOC_WIDTH=atoi(argv[2]);
		NOC_HEIGHT=atoi(argv[3]);
		NOC_DEPTH=atoi(argv[4]);
		simulation_time=atoi(argv[5]);
		routing_algo = atoi(argv[6]);
		traffic_type = atoi(argv[7]);
		processor_no=NOC_WIDTH*NOC_HEIGHT*NOC_DEPTH;
		core_no=processor_no;
	}
	else {
		injection_rate = 0.1;
		NOC_WIDTH=8;
		NOC_HEIGHT=8;
		NOC_DEPTH=2;
		simulation_time=100000;
		routing_algo = 0;
		traffic_type = 0;
		processor_no=NOC_WIDTH*NOC_HEIGHT*NOC_DEPTH;
		core_no=processor_no;
	}
	// 读取温度分布文件
	//readTemperatureFile(file_name, chip_temp);

	std::ifstream temperature_file(file_name, ios::binary);

	for(int i=0; i < NOC_HEIGHT*NOC_WIDTH; i++){

		string coreid;      // temp core id
		double temperature; // temp temperature of noc
		temperature_file >> coreid >> temperature ;
		chip_temp[i] = temperature - 273.15;
	}

	for(int i= 0; i < NOC_HEIGHT; i++){
		for(int j =0; j < NOC_WIDTH; j++){
			cout << chip_temp[i*NOC_WIDTH + j] << "\t";
		}
		cout << endl;
	}
	// parameters
	link_length = CHIP_W / NOC_WIDTH ;
	lambda_MR_0_new= lambda_VCSEL_0 - (row_MR * (point_temp - T_0));
	
	string routing_name ;
	string trace_name ;
	string loss_name;
	string power_name;
	string fold_name = "./data/";

	routing_name = fold_name + "route.txt";
	const char* c_routing_name = routing_name.c_str();
	routing_list_log.open(c_routing_name, ios::out | ios::app);
	cout << routing_name << endl;

	trace_name = fold_name + "trace.txt";
	const char* c_trace_name = trace_name.c_str();
	trace_list_log.open(c_trace_name, ios::out| ios::app);

	loss_name = fold_name + "loss.txt";
	const char* c_loss_name = loss_name.c_str();
	loss_list_log.open(c_loss_name, ios::out| ios::app);

	power_name = fold_name + "power.txt";
	const char* c_power_name = power_name.c_str();
	power_list_log.open(c_power_name, ios::out| ios::app);

	sc_clock clk("clk", 1, SC_NS, 0.5);
	electronic_chip *noc;
	noc = new electronic_chip("noc");
	noc->clk(clk);
	noc->init(clk);
	//cout<<"\nStarting Simulation\n\n";
	cout<<"\nStarting Simulation\n\n";
	sc_start(simulation_time, SC_NS);

//	sc_close_vcd_trace_file(tf);
	cout<<"\nTerminating Simulation\n\n";

	noc->check_finish();

	double result_loss, result_power, worst_loss, worst_power;

	result_loss  = getAverageLoss();
	result_power = getAveragePower();
	worst_loss 	 = getWorstLoss();
	worst_power	 = getWorstPower(); 


	cout<< "#output#" <<traffic_type << "\t"\
		<< routing_algo << "\t" << traffic_type << "\t" << file_name<<"\t" \
		<< routing_algo << "\t" \
		<< result_loss  << "\t" << result_power << "\t" \
		<< worst_loss 	<< "\t" << worst_power << "\t" \
		<< noc->total_throughput<<"\t"<<noc->average_pkt_delay<<"\t" << endl;


	ofstream formatted_throughput_delay_log;
	ofstream throughput_delay_log;

	formatted_throughput_delay_log.open("uniform_formatted_3D_OMesh_throughput_delay.txt",ios::out|ios::app);

	formatted_throughput_delay_log \
		<< NOC_WIDTH << "\t"<<NOC_HEIGHT << "\t" \
		<<injection_rate<< "\t" << routing_algo << "\t" << traffic_type << "\t" << file_name<<"\t"\
		<<noc->total_throughput<<"\t"<<noc->average_pkt_delay<<"\t" \
		<< result_loss  << "\t" << result_power << "\t" \
		<< worst_loss 	<< "\t" << worst_power << endl;

	formatted_throughput_delay_log<<endl;


	formatted_throughput_delay_log.close();


	return 0;

}
