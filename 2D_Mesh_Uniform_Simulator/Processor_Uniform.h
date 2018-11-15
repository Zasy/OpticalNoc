#ifndef processor_with_uniform_traffic
#define processor_with_uniform_traffic

#include "Definition.h"

#define A 16807.0
#define M 2147483647.0
#define indifinite 4294967296
static double r_seed;

void generate_seed()
{
    srand((unsigned)time(NULL)); 
    r_seed=rand()%10; 
	if(r_seed==0)
		r_seed=1;
}

double rnd()//generate uniform distribution random in(0,1)
// random is stored in r_seed
{
	r_seed=fmod(A*r_seed,M);
//	printf("%lf\n",r_seed*4.656612875e-10);
	double k=r_seed*4.656612875e-10;
	return(k);
}

double gen_exp_dis_time(double a)//generate exponantial variable with parameter a
{
    return (-log(rnd())/a);
}


class Processor: public sc_module
{

public:
	sc_in<bool>   clk ;
	sc_out<router_forward_interface> data_out;
	sc_out<bool>		data_ready_out;	
	
	sc_out<router_forward_interface> tear_out;
	sc_out<bool>		tear_ready_out;
	
	sc_out<router_forward_interface> partial_out;
	sc_out<bool>		partial_ready_out;
				
	sc_out<router_forward_interface> ack_out;
	sc_out<bool>		ack_ready_out;	
	
	// input ports
	sc_in <router_forward_interface> data_in;
	sc_in <bool>		data_ready_in;
	
	sc_in <router_forward_interface> ack_in;
	sc_in <bool>		ack_ready_in;	
	sc_in <router_forward_interface> tear_in;
	sc_in <bool>		tear_ready_in;	
	sc_in <router_forward_interface> partial_in;
	sc_in <bool>		partial_ready_in;	
	int success;
	int tear_time_remain;
	bool buffer_req_out;
	bool buffer_tear_out;	
	flit_type temp_flit;
	flit_type partial_temp_flit; //store the temp flit_data to test the system 
    router_forward_interface	reg_data_in;			// temp storage of input data before deciding which vc to write in
	bool						reg_data_ready_in;	// temp storage of input data ready signal
	router_forward_interface	reg_tear_in;			// temp storage of input data before deciding which vc to write in
	bool						reg_tear_ready_in;	// temp storage of input data ready signal	
	router_forward_interface	reg_partial_in;			// temp storage of input data before deciding which vc to write in
	bool						reg_partial_ready_in;	// temp storage of input data ready signal	
	router_forward_interface	reg_ack_in;			// temp storage of input data before deciding which vc to write in
	bool						reg_ack_ready_in;	// temp storage of input data ready signal	
	
	router_forward_interface	reg_data_out;		// temp storage for output
	router_forward_interface    reg_tear_out;
	router_forward_interface    reg_partial_out;
    router_forward_interface	reg_ack_out;

	// basic parameters
	int						core_id;									// the id of the PU
	int next_time_remain;
	int	timer;
	

	// output buffer
	vector<flit_type>		output_buffer;
	

	int current_vc; //vc used now
	int packet_sequence;//current seq of the pkt of the core
		
	//recode all pkts information
	double total_pkt_delay;
	double total_pkt_number;
	double total_data_size;//in flit
	
	//Current sent pkt is composed of all the same flits defined below
	flit_type current_sent_packet;
	//sender state
	enum sending_state_type {free,core_sending_setup_pk,core_sending_data};
	sending_state_type send_pkt_state;

	//ofstream send_req_out_log;


	int						get_id();
	int						set_id(int pid);

	void receive_packet();
	void send_packet();
	void initial();
	
	void update_generator_buffer();
	void clear_request();
	void resend();
	

	SC_HAS_PROCESS(Processor);
	Processor(sc_module_name name)  //SC_CTOR(Processor)  //
	{
		SC_METHOD(receive_packet);
		sensitive<<clk.pos();

		SC_METHOD(send_packet);
		sensitive<<clk.pos();
		
		SC_METHOD(update_generator_buffer);
		sensitive<<clk.pos();

		SC_THREAD(clear_request);
		sensitive<<clk.pos();
		
		SC_THREAD(resend);
		sensitive<<clk.pos();
	}
};



void Processor::initial()
{
//initial: Can NOLY BE called by outside because the output port is not bound yet!

	current_vc=0;
	packet_sequence=0;

	//recode all pkts information
	total_pkt_delay=0;
	total_pkt_number=0;
	total_data_size=0;

	next_time_remain=0;	
	generate_seed();
	gen_exp_dis_time(injection_rate);

	timer = -1;
	success = 0;
	tear_time_remain=-1;
	buffer_req_out=0;	
	buffer_tear_out=0;
	reg_data_ready_in = 0;
	reg_tear_ready_in = 0;
	reg_ack_ready_in= 0;
	reg_partial_ready_in = 0;
	//send_req_out_log.open("processor_log.txt",ios::out);
	
	//intial the output port
	router_forward_interface temp_data;
	temp_data.ready=false;
	data_out.write(temp_data);
	tear_out.write(temp_data);
	partial_out.write(temp_data);
	ack_out.write(temp_data);

	
	send_pkt_state=free;
}

void Processor::receive_packet()
{
		
		reg_data_ready_in = data_ready_in;
		reg_data_in = data_in.read();
			
		if(reg_data_ready_in == 1)
		{
			temp_flit=reg_data_in.data_flit;

			//if it is a control type, just update the out_memory size
			if(temp_flit.control_type==true)
			{
				cout<<"error in receiving control packet"<<endl;
			}

			timer=temp_flit.packet_length-1;

		}
		
		if(timer==0){
			////send_req_out_log<<sc_time_stamp()<<" 2 receive packet "<<temp_flit.src<<"->"<<temp_flit.dest<<" sequence " <<temp_flit.packet_sequence<<endl;
			
			if(temp_flit.control_type==false)
			{

				int temp_src=temp_flit.src_task;
				int temp_task=temp_flit.dest_task;
				int j=0;
				int k=0;
				
				int temp=sc_time_stamp().to_double()/1000;//to ns
				temp=temp-temp_flit.generate_time;
				total_pkt_delay=total_pkt_delay+temp;

				//recode sent pkt and data size
				total_pkt_number++;
				total_data_size=total_data_size+temp_flit.packet_length;
				
				if (DEBUG){
					//add the code for routing data
					routing_list_log << "src " << temp_flit.src << " " << "dest " << temp_flit.dest << endl;
					routing_list_log << "generate_time " << temp_flit.generate_time << endl;
					for (int i = 0; i < temp_flit.routing_list.size(); i++)
					{
						routing_list_log << "the id is " << temp_flit.routing_list[i].id << "," << "the receive_time is " << temp_flit.routing_list[i].receive_time << ',' << "the leave_time is " << temp_flit.routing_list[i].leave_time << endl;
					}
					routing_list_log << "finish_time " << sc_time_stamp().to_double() / 1000 << endl;
					routing_list_log << "\n";
				}
				if (if_trace_data){
					//add the code for trace data
					trace_list_log << temp_flit.generate_time << "\t" << temp_flit.src << "\t" << temp_flit.dest << "\t" << sc_time_stamp().to_double() / 1000 << endl;
					for (int i = 0; i < temp_flit.routing_list.size(); i++)
					{
						trace_list_log << temp_flit.routing_list[i].id << "\t";
					}
					trace_list_log << "send_times="<<temp_flit.send_times<< "\t";
					trace_list_log << "\n";
				}

				loss_list_log << temp_flit.src << "\t" << temp_flit.dest << "\t" << temp_flit.optical_loss << "\t" << temp_flit.generate_time << "\t" << sc_time_stamp().to_double() / 1000 << endl;
				double temp_power = get_Power_EO( temp_flit.optical_loss, T_0, temp_flit.N_active);
				power_list_log << temp_flit.src << "\t" << temp_flit.dest << "\t" << temp_power << endl;
				temp_flit.power_all = temp_power;
				addData(temp_flit);
				total_pkt += 1;				
			}

		}//end if tail

		timer=timer-1;
		if(timer < 0){
			timer = -1;
		}
}

void Processor::update_generator_buffer()
{
	double temp;
	int dest_temp;
	int core_x, core_y, core_z;
	int dest_x, dest_y, dest_z;
	if(next_time_remain>0) next_time_remain--;
	if(next_time_remain==0)
	{	
		temp=gen_exp_dis_time(injection_rate);
		next_time_remain=int (temp*const_pkt_length);

		if(traffic_type == 0){
			dest_temp=int (rnd()*processor_no);
			while(dest_temp==core_id)
			{
				dest_temp=int( rnd()*processor_no);
			}
		}else if(traffic_type == 1){
			core_z = core_id /(NOC_WIDTH*NOC_HEIGHT);
			core_y = core_id % (NOC_WIDTH*NOC_HEIGHT) / NOC_WIDTH;
			core_x = core_id % (NOC_WIDTH*NOC_HEIGHT) % NOC_WIDTH;
			
			dest_z = NOC_DEPTH - 1 - core_z;
			dest_y = NOC_HEIGHT - 1 -core_x;
			dest_x = NOC_WIDTH - 1 - core_y;

			dest_temp = NOC_WIDTH*NOC_HEIGHT*dest_z + NOC_WIDTH*dest_y + dest_z;
		}else if(traffic_type == 2){
			core_z = core_id /(NOC_WIDTH*NOC_HEIGHT);
			core_y = core_id % (NOC_WIDTH*NOC_HEIGHT) / NOC_WIDTH;
			core_x = core_id % (NOC_WIDTH*NOC_HEIGHT) % NOC_WIDTH;
			
			dest_z = NOC_DEPTH - 1 - core_z;
			dest_y = NOC_HEIGHT - 1 -core_y;
			dest_x = NOC_WIDTH - 1 - core_x;

			dest_temp = NOC_WIDTH*NOC_HEIGHT*dest_z + NOC_WIDTH*dest_y + dest_z;
		}else if(traffic_type == 3){
			core_z = core_id /(NOC_WIDTH*NOC_HEIGHT);
			core_y = core_id % (NOC_WIDTH*NOC_HEIGHT) / NOC_WIDTH;
			core_x = core_id % (NOC_WIDTH*NOC_HEIGHT) % NOC_WIDTH;
			
			dest_z = NOC_DEPTH - 1 - core_z;
			dest_y = core_x;
			dest_x = core_y;

			dest_temp = NOC_WIDTH*NOC_HEIGHT*dest_z + NOC_WIDTH*dest_y + dest_z;

		}

		if (dest_temp != core_id){
			flit_type temp_pkt;
			temp_pkt.src=core_id;
			temp_pkt.dest=dest_temp;
			temp_pkt.generate_time=sc_time_stamp().to_double()/1000; //ns, in fact cycle
			temp_pkt.control_type=false;
			temp_pkt.packet_length=const_pkt_length;
			temp_pkt.send_times=0;
			temp_pkt.optical_loss = 0;
			temp_pkt.power_all = 0;
			temp_pkt.N_active = 0;
			output_buffer.push_back(temp_pkt);
		}
	}
}
	

void Processor::resend(){

	int k;
	double temp=0;

	while (true) {
		reg_partial_ready_in = partial_ready_in;
		reg_partial_in = partial_in.read();

		wait(0.2, SC_NS);

		//if receive the partial packet
		if (reg_partial_ready_in == 1){
			reg_partial_ready_in == 0;
			temp = rnd();
			
			reg_partial_in.data_flit.optical_loss = 0;
			reg_partial_in.data_flit.N_active = 0;
			partial_temp_flit = reg_partial_in.data_flit;

			k=temp*6;
			wait(k, SC_NS);
			wait(0.5, SC_NS);
			success = 2;			
		}

		wait();
	}
}

void Processor::send_packet()
{
	//clear the previous data: since the output port is signal, there is delta delay for updating
	router_forward_interface temp_data;
	temp_data.ready=false;
	data_out.write(temp_data);

	switch(send_pkt_state)
	{
	case free:
	{	
		if(output_buffer.empty())
			break;

		current_sent_packet=output_buffer.front();

		//dest is on other cores,
		//else we send data out, first we delete the pkt
		output_buffer.erase(output_buffer.begin());
		//configure the pkt
		current_sent_packet.src_chip=0;
		current_sent_packet.dest_chip=0;
		current_sent_packet.from_node_level=-1;
		current_sent_packet.from_node_rank=-1;
		current_sent_packet.power=0;
		current_sent_packet.encountered_router_number=0;
		current_sent_packet.switching_capacity_consumed=0;
		current_sent_packet.packet_sequence=packet_sequence;
		packet_sequence++;
	
		current_sent_packet.waiting_time=0;

		if(current_sent_packet.packet_length>1)
		{
			current_sent_packet.head=true;
			current_sent_packet.tail=false;
		}
		else if(current_sent_packet.packet_length==1)
		{
			current_sent_packet.head=true;
			current_sent_packet.tail=true;
		}
		else
		{
			cout<<"packet_length_error"<<endl;
			exit(0);
		}
		
					
		router_forward_interface temp_data;
		temp_data.data_flit=current_sent_packet;
		//temp_data.vc_id=current_vc;
		temp_data.ready=true;
		data_out.write(temp_data);
		buffer_req_out = 1;
		//send_req_out_log<<sc_time_stamp()<<" 1 send packet "<<current_sent_packet.src<<"->"<<current_sent_packet.dest<<" sequence " <<current_sent_packet.packet_sequence<<endl;

		send_pkt_state=core_sending_setup_pk;
		success = 0;

						
		break;
	}

	case core_sending_data:
	{
		if(tear_time_remain==0){
			
			send_pkt_state=free;
		
		}
		break;	
	}


	case core_sending_setup_pk:
	{
		//if the length of packet is 1, we know it has been sent out already,
		//however, we can not reset the state in case 0--we need
		//leave some time for router to response the previous packet
		//,especially, update the states of vc/buffer
		

		if(success==1)
		{	//a path has been successful established
			//start sending data
			//send_req_out_log<<sc_time_stamp()<<" 2 begin sendind data "<<id<<"->"<<req_dest[req_read]<<endl;
			
			if(current_sent_packet.packet_length == 1){
				send_pkt_state=free;
			}
			else {
				send_pkt_state=core_sending_data;
				tear_time_remain=current_sent_packet.packet_length;
			}

			success=0;
			
			router_forward_interface temp_data;
			temp_data.data_flit=current_sent_packet;
			
			temp_data.ready=true;
			tear_out.write(temp_data);
			buffer_tear_out = 1;
		}
		else if(success==2){
			router_forward_interface temp_data;
			temp_data.data_flit=partial_temp_flit;
			//temp_data.vc_id=current_vc;
			temp_data.ready=true;
			temp_data.data_flit.send_times += 1;
			data_out.write(temp_data);
			buffer_req_out = 1;
			success=0;
			//send_req_out_log<<sc_time_stamp()<<" 1 send packet "<<current_sent_packet.src<<"->"<<current_sent_packet.dest<<" sequence " <<current_sent_packet.packet_sequence<<endl;

		}

		break;
	}
	default:
	{
		cout<<"error"<<endl;
	}

	}//end switch
	
	if(tear_time_remain>0){
		tear_time_remain --;
	}		
}

void Processor::clear_request()
	{
		while(true)
		{
			wait(0.01, SC_NS);
		
			data_ready_out.write(0);
			tear_ready_out.write(0);
			partial_ready_out.write(0);
			ack_ready_out.write(0);
			
			if(buffer_req_out==1 || buffer_tear_out==1){
			    wait(0.5, SC_NS);
				if (buffer_req_out==1) {				
					data_ready_out.write(1);	
					buffer_req_out=0;		
				}		
			    if (buffer_tear_out==1) {
					tear_ready_out.write(1);	
					buffer_tear_out=0;	
				}			
			}	
			
			wait();

		}
	}





int	Processor::get_id() {

	return core_id;
}

int	Processor::set_id(int pid) {

	assert (pid >= 0);
	core_id = pid;

	return 0;
}


#endif

