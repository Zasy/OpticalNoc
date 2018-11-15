#include "Definition.h"
#include "electronic_torus.h"
#include "global_data.h"
#include "time.h"
#include "stdio.h"
#include "math.h"

#include <iostream>
#include <string>
using namespace std;
int sc_main(int argc, char* argv[]) {

	double start_time, end_time;
	start_time = clock();


	int program_id=0;

	//input foldername/filename like: traffic/fattree_16/fpppp_fattree_16.stp

	string file_name(argv[1]);
	if(argc>=11)
	{
		NOC_WIDTH=atoi(argv[2]);
		NOC_HEIGHT=atoi(argv[3]);
		NOC_DEPTH=atoi(argv[4]);
		execution_iterations=atoi(argv[5]); //in RT, this number would be overloaded by input file
		start_record_iteration=atoi(argv[6]);
		end_record_iteration=atoi(argv[7]);
		program_id=atoi(argv[8]);
		token_size_scale =atoi(argv[9]);

		application_num = atoi(argv[10]);
		routing_algo = atoi(argv[11]);

		START_RECORD=false;
		END_RECORD=false;
		app_str = app_str_set[application_num];
		processor_no=NOC_WIDTH*NOC_HEIGHT*NOC_DEPTH;
		core_no=processor_no;
	}
	else
	{
		NOC_WIDTH=4;//8;//4;//32;/256;
		NOC_HEIGHT=4;//5;//4;//7;/10;
		NOC_DEPTH=2;
		processor_no=NOC_WIDTH*NOC_HEIGHT*NOC_DEPTH;
		core_no=processor_no;
		execution_iterations=10;
		start_record_iteration=1;
		end_record_iteration=9;
		token_size_scale = 1;
		application_num = 6;
		START_RECORD=false;
		END_RECORD=false;
	}

	//parameters
	link_length = CHIP_W / NOC_WIDTH;
	lambda_MR_0_new = lambda_VCSEL_0 - (row_MR * (55 - T_0));

	string routing_name;
	string trace_name;
	string proc_state_name;
	string loss_name;
	string power_name;
	string performance_name;
	string temp_trace_name;
	string fold_name = "./data/";


	size_t found_new = file_name.rfind("/");
	size_t found_stp = file_name.rfind("p");
	string type_name = file_name.substr(found_new + 1, found_new + 7);

	routing_name = fold_name + "route.txt";
	const char* c_routing_name = routing_name.c_str();
	routing_list_log.open(c_routing_name, ios::out | ios::app);
	
	trace_name = fold_name + "trace.txt";
	const char* c_trace_name = trace_name.c_str();
	trace_list_log.open(c_trace_name, ios::out| ios::app);

	loss_name = fold_name + app_str + "_loss.txt";
	const char* c_loss_name = loss_name.c_str();
	loss_list_log.open(c_loss_name, ios::out| ios::app);

	power_name = fold_name + app_str + "_power.txt";
	const char* c_power_name = power_name.c_str();
	power_list_log.open(c_power_name, ios::out| ios::app);

	temp_trace_name = fold_name + app_str + "_"+ to_string(routing_algo) +"_temp_trace.txt";
	const char* c_temp_trace_name = temp_trace_name.c_str();
	temp_trace_log.open(c_temp_trace_name, ios::out | ios::app);


	ofstream formatted_throughput_delay_log;
	ofstream throughput_delay_log;


	sc_clock clk("clk", 1, SC_NS, 0.5);
	electronic_chip *noc;
	noc = new electronic_chip("noc");
	noc->clk(clk);
	noc->init(clk);
	//cout<<"\nStarting Simulation\n\n";

	noc->load_traffic(file_name);		
	sc_start();
	cout <<sc_time_stamp() << "  succeeded"<<endl;

	
	size_t found_prim=file_name.rfind("/");
	string input_folder_name=file_name.substr(0, found_prim+1);
	stringstream temp_out_name;
	temp_out_name<<"mesh"<<NOC_WIDTH<<"x"<<NOC_HEIGHT<<"x"<<NOC_DEPTH<<"_"<<program_id<<"_throughput_delay.txt";

	string output_file_name=input_folder_name+temp_out_name.str();
	
	formatted_throughput_delay_log.open(output_file_name.c_str(),ios::out|ios::app);

	performance_name = fold_name + "performance.txt";
	const char* c_performance_name = performance_name.c_str();
	performance_log.open(c_performance_name, ios::out| ios::app);
	

	end_time = (clock()-start_time)/(double)(CLOCKS_PER_SEC);



	double result_loss, result_power;

	result_loss = getAverageLoss();
	result_power = getAveragePower();


	performance_log << file_name << "\t" << app_str << "\t"\
		<<delta << "\t" << point_temp << "\t" \
		<<L_MR_passive << "\t" << routing_algo << "\t" \
		<<result_loss << "\t" << result_power << "\t" \
		<<noc->Average_ETE_delay<<"\t"<<noc->appliction_performance<<"\t" << endl;

	// for(int i=0;i<execution_iterations;i++)
	// {
	// 	formatted_throughput_delay_log << noc->task_start_time.at(i)<<"	"<<noc->task_end_time.at(i)<<"	";
	// }

	// for(int i=0; i< noc->injection_rate_list.size();i++)
	// {
	// 	formatted_throughput_delay_log << noc->injection_rate_list.at(i)<<"	";
	// }
	formatted_throughput_delay_log<<endl;
	delete noc;							

	formatted_throughput_delay_log.close();

	return 0;

}