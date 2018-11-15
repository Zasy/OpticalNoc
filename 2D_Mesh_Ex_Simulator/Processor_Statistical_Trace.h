#ifndef processor_with_traffic_mode_statistical_trace
#define processor_with_traffic_mode_statistical_trace

#include "Definition.h"
#include "GraphTask.h"
//sc_module(Processor)

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
	flit_type partial_temp_flit;
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

	// core state
	int						core_state;
	
	// basic parameters
	int						core_id;									// the id of the PU
	int	timer;
	// parameters related to the traffic
	vector<GraphTask>		task_list;							// the list of scheduled tasks, execute following the sequence defined by the vector

	int schedule_sequence;

	struct _compare_task {
		bool operator() (GraphTask t1, GraphTask t2) { return (t1.get_schedule() < t2.get_schedule());}
	} compare;

	// output buffer
	vector<flit_type>		output_buffer;
	
	//generator buffer: recording the pkts scheduled to be generated
	vector<generator_buffer_type>	generator_buffer;

	int current_vc; //vc used now
	int packet_sequence;//current seq of the pkt of the core
		
	//recode all pkts information
	double total_pkt_delay;
	double total_pkt_number;
	double total_data_size;//in flit
	double E_elec_total;// in pJ
	double E_opti_total;// in pJ
	//remained time of the being executed task
	int remained_execution_time;
	//remainded sending pkt time
	int remained_sending_time;
	//Current sent pkt is composed of all the same flits defined below
	flit_type current_sent_packet;
	//sender state
	enum sending_state_type {free,core_sending_setup_pk,core_sending_data};
	sending_state_type send_pkt_state;

	//ofstream send_req_out_log;

	int						get_state();
	int						get_id();
	int						set_id(int pid);

	int						get_task_offset_by_task_id(int tid);
	GraphTask&				get_task_by_task_id(int tid);					// return the task with the given id
	int						add_task(GraphTask &t);				// add a task to the task list 
	int						sort_task_list();					// sort the task list by schedule sequence
//	int						print_all_task();
	
	int waiting_list_offset;
	vector<int>	schedule_history;
	vector<int> task_in_waiting_list;//can be exeucted but maybe not ready

	void receive_packet();
	void send_packet();
	void initial();
	void check_pkt();
	void task_execution();
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
		SC_METHOD(task_execution);
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
	E_elec_total=0;
	E_opti_total=0;

	core_state = 0;
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

	//the offset of task to be checked in the waiting list
	waiting_list_offset=0;
	

	schedule_sequence=0;
}

void Processor::check_pkt()
{
	return;
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
				int eid=temp_flit.edge_id;
				int tid=temp_flit.src_task;
				//cout<<"error in 8"<<endl;
				GraphTask& src_task=get_task_by_task_id(tid);
				
				GraphEdge& src_edge=src_task.get_outgoing_edge_by_eid(eid);
				assert(src_edge.get_src_task_id()==tid);
				
				for(int i=0;i<temp_flit.consumed_token;i++)
				{
					assert(src_edge.update_in_memory_read_pointer());
				}

				return;
			}

			timer=temp_flit.packet_length-1;

		}
		
		if(timer==0){
			////send_req_out_log<<sc_time_stamp()<<" 2 receive packet "<<temp_flit.src<<"->"<<temp_flit.dest<<" sequence " <<temp_flit.packet_sequence<<endl;
			check_pkt();
			if(temp_flit.control_type==false)
			{

				int temp_src=temp_flit.src_task;
				int temp_edge_id=temp_flit.edge_id;
				int temp_task=temp_flit.dest_task;
				int j=0;
				int k=0;
				//cout<<"error in 9"<<endl;
				GraphTask& dest_task=get_task_by_task_id(temp_task);
				
				GraphEdge& dest_edge=dest_task.get_incoming_edge_by_eid(temp_edge_id);

				//record the pkt
				dest_edge.record_pkt(temp_flit);
				//send_req_out_log<<sc_time_stamp()<<"Core "<<core_id<<" Task "<< temp_task <<" receive packet "<<temp_flit.src<<"->"<<temp_flit.dest<<" sequence " <<temp_flit.packet_sequence<<endl;

		
				if(START_RECORD)
				{
					int temp=sc_time_stamp().to_double()/1000;//to ns
					temp=temp-temp_flit.generate_time;
					total_pkt_delay=total_pkt_delay+temp;

					//recode sent pkt and data size
					total_pkt_number++;
					total_data_size=total_data_size+temp_flit.packet_length;

					int src_z = temp_flit.src / (NOC_WIDTH * NOC_HEIGHT);
					int src_y = (temp_flit.src - src_z*(NOC_WIDTH * NOC_HEIGHT)) / NOC_WIDTH;
					int src_x = (temp_flit.src - src_z*(NOC_WIDTH * NOC_HEIGHT)) % NOC_WIDTH;
				
					int dst_z = temp_flit.dest / (NOC_WIDTH * NOC_HEIGHT);
					int dst_y = (temp_flit.dest - dst_z*(NOC_WIDTH * NOC_HEIGHT)) / NOC_WIDTH;
					int dst_x = (temp_flit.dest - dst_z*(NOC_WIDTH * NOC_HEIGHT)) % NOC_WIDTH;
				
					int dis_x=dst_x-src_x;
					int dis_y=dst_y-src_y;
					int dis_z=dst_z-src_z;

					//double power_consumption=0;

					int N_MR_on=2;//the number of on_state MRs
					if(dis_y!=0){
						N_MR_on=N_MR_on+1;
						if(dis_z!=0){
							N_MR_on=N_MR_on+1;
						}
					}
			
					if (dis_x==0 && dis_y==0 && dis_z==0){
						
					}
			
					else {
						
						int hop_num=0;
						if(dis_x>0){
							hop_num=dis_x;
						}
						else{
							hop_num=-dis_x;
						}
						if(dis_y>0){
							hop_num=hop_num+dis_y;
						}
						else{
							hop_num=hop_num-dis_y;
						}
						if(dis_z>0){
							hop_num=hop_num+dis_z;
						}
						else{
							hop_num=hop_num-dis_z;
						}


						double E_elec=N_MR_on*MR_power*(temp_flit.packet_length*flit_size/optical_bandwidth + 3.48/3*0.001*10*hop_num) +2*E_local_link*(temp_flit.packet_length*flit_size+9)+E_global_link*9*hop_num+9*(buf_depth*E_buffer*(hop_num+1) + E_crossbar*(hop_num+1))+(E_control*(hop_num+1));//in 16*16 unfolded, hop length is 10/16mm
						double I=2.4+0.00075*(max_temperature-40)*(max_temperature-40)+pow(10,(-14.2-10*log10(0.403-0.13*max_temperature/60)+temp_flit.loss)/10);
						double E_opti=temp_flit.packet_length*flit_size*(0.1125+0.1*(0.0729*I+1.0135)*I+0.0003+0.3375+0.288+0.1*N_MR_on*thermal_tuning_per_nm*fabs(VCSEL_0+VCSEL_shift*(max_temperature-room_temperature)-(MR_0_optimal+MR_shift*(min_temperature-room_temperature)))); 
						E_opti_total=E_opti_total+E_opti;
						E_elec_total=E_elec_total+E_elec;

					}		
				}
				addData(temp_flit);
				total_pkt += 1;
				loss_list_log << temp_flit.src << "\t" << temp_flit.dest << "\t" << temp_flit.optical_loss << "\t"<< temp_flit.generate_time <<"\t"<< sc_time_stamp().to_double() / 1000 <<endl;
				double temp_power = get_Power_EO( temp_flit.optical_loss, T_0, temp_flit.N_active);
				power_list_log << temp_flit.src << "\t" << temp_flit.dest << "\t" << temp_power << "\t"<< temp_flit.generate_time <<"\t"<< sc_time_stamp().to_double() / 1000 << endl;

				if (temp_flit.dest == 17 && temp_flit.src == 0){
					trace_list_log << temp_flit.generate_time << "\t" << temp_flit.src << "\t" << temp_flit.dest << "\t" << sc_time_stamp().to_double() / 1000 << endl;
					for (int i = 0; i < temp_flit.routing_list.size(); i++)
					{
						trace_list_log << temp_flit.routing_list[i].id << "\t";
					}
					trace_list_log << "\n";
				}

						
			}//end if tail

		}	
		timer=timer-1;
		if(timer < 0){
			timer = -1;
		}
}


GraphTask& Processor::get_task_by_task_id(int tid)
{
	for (unsigned int i=0; i<task_list.size(); i++) 
	{
		GraphTask &e = task_list.at(i) ;
		if (e.get_id() == tid)
			return e;
	}
	cout<<"error in finding task 1"<<endl;
	exit(0);
}
int Processor::get_task_offset_by_task_id(int tid)
{
	for (unsigned int i=0; i<task_list.size(); i++) 
	{
		GraphTask &e = task_list.at(i) ;
		if (e.get_id() == tid)
			return i;
	}
	cout<<"error in finding task 2"<<endl;
	exit(0);
}

void Processor::task_execution()
{
		if(task_list.size()<=0)
			return;
		
		int current_task_id;
		//if this is the first time, we have nothing in the waiting list
		//we put the first task in the list
		//Or, all tasks are finished, we can still load the first one. It is PK
		if(task_in_waiting_list.size()==0)
		{
			task_in_waiting_list.push_back(task_list.at(0).get_id());
			waiting_list_offset=0;
		}

		current_task_id=task_in_waiting_list.at(waiting_list_offset);
		//cout<<"error in 1"<<endl;
		GraphTask &c_task= get_task_by_task_id(current_task_id);
		
		switch(c_task.get_task_state())
		{
		case 0: //idle
		{	
			core_state = 1;
			//proc_state[get_id()].push_back(c_task.get_task_state());
			//if it is finished, we just jump to next task
			if(c_task.get_completed_times()>=c_task.get_required_times())
			{
				
				task_in_waiting_list.erase(task_in_waiting_list.begin()+waiting_list_offset);
				//when we erease current one, offset would point to next one automatically
				//except that it is the end
				if(waiting_list_offset>=task_in_waiting_list.size())
				{
					waiting_list_offset=0;
				}

				break;
			}
			//first, we check whether the output buffer is full
			int j;
			for(j=0;j< c_task.get_size_of_outgoing_edge_list();j++)
			{
				GraphEdge &edge_temp=c_task.get_outgoing_edge_by_offset(j);
				if(edge_temp.get_out_memory_remained()<=0)
				{
					break;
				}
			}
			if(j < c_task.get_size_of_outgoing_edge_list())
			{
				waiting_list_offset=(waiting_list_offset+1)%task_in_waiting_list.size();	
				break; //buffer is not ready, jump to next candidate, wait next cycle
			}
			//end of check output buffer


			if(c_task.get_size_of_incoming_edge_list()== 0)//the starting task without dependency on other tasks
			{
					c_task.set_task_state(1);
					remained_execution_time=c_task.get_random_execution_time();
					c_task.record_duration(remained_execution_time);
					schedule_history.push_back(c_task.get_id());
					int temp_start=sc_time_stamp().to_double()/1000;
					c_task.record_execution_time(temp_start, temp_start+remained_execution_time);

					//we need inform the src_task of the edge connecting c_task that one token is consumed
					//However, since this task is independent of other tasks, there are no such src_task


					//generate the token to the outgoing edge list, the token is decompsed into multiple pkts,
					//put them in the generator buffer
					for(int j=0;j< c_task.get_size_of_outgoing_edge_list();j++)
					{
						GraphEdge &edge_temp=c_task.get_outgoing_edge_by_offset(j);

						//update memory 
						assert(edge_temp.update_out_memory_write_pointer());
						//if the dest_task is on the same proc, we need also update the dest_task's incoming-edge's in_memory_write
						int dest_proc_id=c_task.get_outgoing_edge_by_offset(j).get_dst_proc_id();
						if(dest_proc_id==c_task.get_proc_id())
						{
							assert(dest_proc_id==core_id);
							int dest_task_id=c_task.get_outgoing_edge_by_offset(j).get_dst_task_id();
							int edge_id=c_task.get_outgoing_edge_by_offset(j).get_id();
							//cout<<"error in 2"<<endl;
							GraphTask &dst_task= get_task_by_task_id(dest_task_id);
							
							GraphEdge &in_edge=dst_task.get_incoming_edge_by_eid(edge_id);
							//assert(in_edge.update_in_memory_write_pointer());
						}


						flit_type temp_pkt;
						double generated_token_size=edge_temp.get_random_token_size();
						temp_pkt.token_length_in_flit=ceil (generated_token_size/flit_size);
						edge_temp.record_token_size((int)generated_token_size);
						temp_pkt.token_length_in_pkt=ceil( (double)temp_pkt.token_length_in_flit/const_pkt_length);// the number of pkts required for the token
						temp_pkt.token_id=edge_temp.get_current_token_id();
					
						//set the pkt interval rate, it is set by input already
						//edge_temp.set_statistical_pkt_interval((double)temp_pkt.token_length_in_pkt/remained_execution_time);

						temp_pkt.src=edge_temp.get_src_proc_id();
						temp_pkt.dest=edge_temp.get_dst_proc_id();
						temp_pkt.src_task=edge_temp.get_src_task_id();
						temp_pkt.dest_task=edge_temp.get_dst_task_id();
						temp_pkt.edge_id=edge_temp.get_id();
						//temp_pkt.loss=0;
						
						

						generator_buffer_type buffer_temp;
						int temp_offset=0;
						for(int k=0;k<temp_pkt.token_length_in_pkt;k++)
						{	
							if(k<temp_pkt.token_length_in_pkt-1)
								temp_pkt.packet_length=const_pkt_length;
							else
								temp_pkt.packet_length=temp_pkt.token_length_in_flit-(temp_pkt.token_length_in_pkt-1)*const_pkt_length;
							temp_pkt.rank_in_token=k;	
							temp_offset+=edge_temp.get_random_pkt_interval();	
							temp_offset=min(temp_offset,remained_execution_time);
							buffer_temp.time_to_generate_pkt=temp_offset;
							buffer_temp.pkt_to_generate=temp_pkt;
							generator_buffer.push_back(buffer_temp);
						}
						//generate new token ID for next token
						edge_temp.generate_new_token();

					}//end for	
					
				break;	
			}
			else// the task that depends on other tasks
			{
				int j;
				//check the task requirement first
				for( j=0;j < c_task.get_size_of_incoming_edge_list();j++)
				{
					GraphEdge &edge_temp=c_task.get_incoming_edge_by_offset(j);
					if(edge_temp.get_num_incoming_token()>0)//there is token
							continue;
					else
						break; //the requirement is not satisfied; we wait
				}
				if(j < c_task.get_size_of_incoming_edge_list())
				{
					waiting_list_offset=(waiting_list_offset+1)%task_in_waiting_list.size();	
					break; //not satisied, jump to next candidate, wait next cycle
				}

				//requirement is satisfied; run the task
				
				for(j=0;j< c_task.get_size_of_incoming_edge_list();j++)
				{
					GraphEdge &tempInEdge=c_task.get_incoming_edge_by_offset(j);
						
						
						tempInEdge.consume_token();//consume the token
						
						//update the buffer space of this incoming edge
						
						//assert(tempInEdge.update_in_memory_read_pointer());
						//if the src_task is at the same core, also update the out_memory_read
						int src_proc_id=tempInEdge.get_src_proc_id();
						if(src_proc_id==c_task.get_proc_id())
						{
							assert(src_proc_id==core_id);
							int src_task_id=tempInEdge.get_src_task_id();
							int edge_id=tempInEdge.get_id();
							//cout<<"error in 3"<<endl;
							GraphTask &src_task= get_task_by_task_id(src_task_id);
							
							GraphEdge &out_edge=src_task.get_outgoing_edge_by_eid(edge_id);
							assert(out_edge.update_out_memory_read_pointer());
						}
						//on the other hand, if src_task is not on the same core,
						////we need also inform the src_task of the edge connecting c_task that one token is consumed

						else
						{
							flit_type temp_pkt;
							GraphEdge &edge_temp=c_task.get_incoming_edge_by_offset(j);

							temp_pkt.control_type=true;
							temp_pkt.token_length_in_flit=1;
							temp_pkt.packet_length=1;
							temp_pkt.token_length_in_pkt=1;// the number of pkts required for the token

							//important control information
							temp_pkt.consumed_token=1;
							temp_pkt.available_buffer_size=edge_temp.get_in_memory_remained();
							//temp_pkt.loss=0;
							temp_pkt.src=edge_temp.get_dst_proc_id();
							temp_pkt.dest=edge_temp.get_src_proc_id();
							temp_pkt.src_task=edge_temp.get_src_task_id();
							temp_pkt.dest_task=edge_temp.get_dst_task_id();
							temp_pkt.edge_id=edge_temp.get_id();
							
							temp_pkt.token_id=0;
							temp_pkt.rank_in_token=0;
							temp_pkt.generate_time=sc_time_stamp().to_double()/1000;

							//output_buffer.push_back(temp_pkt);

						}


				}

				c_task.set_task_state(1); //run task
				remained_execution_time=c_task.get_random_execution_time();			//set run time
				c_task.record_duration(remained_execution_time);
				schedule_history.push_back(c_task.get_id());
				int temp_start=sc_time_stamp().to_double()/1000;
				c_task.record_execution_time(temp_start, temp_start+remained_execution_time);	
				//generate the token to the outgoing edge list, the token is decompsed into multiple pkts,
				//put them in the generator buffer
					for(j=0;j< c_task.get_size_of_outgoing_edge_list();j++)
					{
						GraphEdge &edge_temp=c_task.get_outgoing_edge_by_offset(j);

						//update memory 
						assert(edge_temp.update_out_memory_write_pointer());
						//if the dest_task is on the same proc, we need also update the dest_task's incoming-edge's in_memory_write
						int dest_proc_id=c_task.get_outgoing_edge_by_offset(j).get_dst_proc_id();
						if(dest_proc_id==c_task.get_proc_id())
						{
							assert(dest_proc_id==core_id);
							int dest_task_id=c_task.get_outgoing_edge_by_offset(j).get_dst_task_id();
							int edge_id=c_task.get_outgoing_edge_by_offset(j).get_id();
							//cout<<"error in 4"<<endl;
							GraphTask &dst_task= get_task_by_task_id(dest_task_id);
							
							GraphEdge &in_edge=dst_task.get_incoming_edge_by_eid(edge_id);
							//assert(in_edge.update_in_memory_write_pointer());
						}


						flit_type temp_pkt;
						double generated_token_size=edge_temp.get_random_token_size();
						temp_pkt.token_length_in_flit=ceil (generated_token_size/flit_size);
						edge_temp.record_token_size((int)generated_token_size);
						temp_pkt.token_length_in_pkt=ceil( (double)temp_pkt.token_length_in_flit/const_pkt_length);// the number of pkts required for the token
						temp_pkt.token_id=edge_temp.get_current_token_id();
						
						
						//set the pkt interval rate, here, it is set in input
						//edge_temp.set_statistical_pkt_interval((double)temp_pkt.token_length_in_pkt/remained_execution_time);

						temp_pkt.src=edge_temp.get_src_proc_id();
						temp_pkt.dest=edge_temp.get_dst_proc_id();
						temp_pkt.src_task=edge_temp.get_src_task_id();
						temp_pkt.dest_task=edge_temp.get_dst_task_id();
						temp_pkt.edge_id=edge_temp.get_id();
						//temp_pkt.loss=0;

						generator_buffer_type buffer_temp;
						int temp_offset=0;
						for(int k=0;k<temp_pkt.token_length_in_pkt;k++)
						{	
							if(k<temp_pkt.token_length_in_pkt-1)
								temp_pkt.packet_length=const_pkt_length;
							else
								temp_pkt.packet_length=temp_pkt.token_length_in_flit-(temp_pkt.token_length_in_pkt-1)*const_pkt_length;
							temp_pkt.rank_in_token=k;	
							temp_offset+=edge_temp.get_random_pkt_interval();
							temp_offset=min(temp_offset,remained_execution_time);
							buffer_temp.time_to_generate_pkt=temp_offset;
							buffer_temp.pkt_to_generate=temp_pkt;
							generator_buffer.push_back(buffer_temp);
						}
						//generate new token ID for next token
						edge_temp.generate_new_token();
					}//end for		
					break;
				}//end else

				break; //break the switch
		}
		case 1:
		{
				core_state = 2;
				//proc_state[get_id()].push_back(c_task.get_task_state());
				remained_execution_time--;
				if(remained_execution_time<=0)
				{
					//each task would recode the history
					c_task.record_schedule_history(schedule_sequence);
					schedule_sequence++;

					c_task.set_task_state(0);
					remained_execution_time=0;
					c_task.add_completed_times();	
					
					//if it is finished, we first erase it in the waiting list 
					task_in_waiting_list.erase(task_in_waiting_list.begin()+waiting_list_offset);
					//when we erease current one, offset would point to next one automatically
					//except that it is the end
					if(waiting_list_offset>=task_in_waiting_list.size())
					{
						waiting_list_offset=0;
					}

					//if this is a highest priority task in task list, we can schedule this task of next loop now
					int task_offset=get_task_offset_by_task_id(current_task_id);
					if(task_offset==0)
					{
						task_in_waiting_list.push_back(current_task_id);
					}

					//we can also schedule the next task(lower priority then current task) in the task list in current loop
					task_offset++;
					if(task_offset>=task_list.size())
					{
						//this is the lowest priotiry task, we have finished the loop , do nothting
					}
					else
					{
						task_in_waiting_list.push_back(task_list.at(task_offset).get_id());
					}	
				}
				break;
		}
	}//end switch
}

void Processor::resend(){
	int k;
	double temp=0;

	while(true){
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



void Processor::update_generator_buffer()
{
	for(int i=0;i<generator_buffer.size();i++)
	{
		generator_buffer.at(i).time_to_generate_pkt--;
		if(generator_buffer.at(i).time_to_generate_pkt<=0)
		{
			flit_type temp_pkt=generator_buffer.at(i).pkt_to_generate;
			generator_buffer.erase(generator_buffer.begin()+i);
			i--; //notice: when we erase the i-th item, the i+1 th item becomes the i-th item. Thus,we still chceck i-th iterm in next loop
			temp_pkt.generate_time=sc_time_stamp().to_double()/1000; //ns, in fact cycle
			temp_pkt.control_type=false;
			output_buffer.push_back(temp_pkt);

			//recode the pkt generation time of the edge
			int src_task=temp_pkt.src_task;
			int edge_id=temp_pkt.edge_id;
			//cout<<"error in 5"<<endl;
			GraphTask &t=get_task_by_task_id(src_task);
			
			GraphEdge &ed=t.get_outgoing_edge_by_eid(edge_id);
			ed.recode_pkt_generate_time(temp_pkt.generate_time);
		}
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

		//notice, the dest core may be the same core-- two tasks are mapped on the same core
		//also, we do not calculate this pkt delay
		//In this case, the out_memory of current_task is also the in_memory of dest_task, therefore, the buffer size is NOT updated

		if(current_sent_packet.src==current_sent_packet.dest)
		{
			if(current_sent_packet.dest != core_id)
			{
				cout<<"error!"<<endl;
				exit(0);
			}
			//get dest edge
			//cout<<"error in 6"<<endl;
			GraphTask& dest_task=get_task_by_task_id(current_sent_packet.dest_task);
			
			GraphEdge& dest_edge=dest_task.get_incoming_edge_by_eid(current_sent_packet.edge_id);
			//recode the pkt
			dest_edge.record_pkt(current_sent_packet);

			//delete the pkt at the sender
			output_buffer.erase(output_buffer.begin());
			break;
		}

		//dest is on other cores,
		
		//else we send data out, first we delete the pkt
		output_buffer.erase(output_buffer.begin());
		
		if(current_sent_packet.control_type==false)
		{
			//update the buffer, producing a space
			//cout<<"error in 7"<<endl;
			GraphTask& src_task=get_task_by_task_id(current_sent_packet.src_task);
			
			GraphEdge& src_edge=src_task.get_outgoing_edge_by_eid(current_sent_packet.edge_id);
			//however, before sending the data, we need check the buffer state in the dest core
			//if buffer is not enough, we put this packet at the end of output_buffer, check next packet
			
			//if recoed_sent_pkt fails, it means buffer at dest is not free, we have to wait
			//otherwise, it is sent and recorded 
			if(src_edge.record_sent_pkt(current_sent_packet)==false)
			{
				output_buffer.push_back(current_sent_packet);
				break;
			}
		}
		//configure the pkt
		current_sent_packet.src_chip=0;
		current_sent_packet.dest_chip=0;
		current_sent_packet.from_node_level=-1;
		current_sent_packet.from_node_rank=-1;
		current_sent_packet.power=0;
		current_sent_packet.loss=0;
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

		current_sent_packet.optical_loss = 0;
		current_sent_packet.power_all = 0;
		current_sent_packet.N_active = 0;
		
					
		router_forward_interface temp_data;
		temp_data.data_flit=current_sent_packet;
		//temp_data.vc_id=current_vc;
		temp_data.ready=true;
		data_out.write(temp_data);
		buffer_req_out = 1;
		//send_req_out_log<<sc_time_stamp()<<" 1 send packet "<<current_sent_packet.src<<"->"<<current_sent_packet.dest<<" sequence " <<current_sent_packet.packet_sequence<<endl;

		remained_sending_time=current_sent_packet.packet_length-1;

		send_pkt_state=core_sending_setup_pk;

						
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
		{//a path has been successful established
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
		else if(success == 2){
			router_forward_interface temp_data;
			temp_data.data_flit=partial_temp_flit;
			//temp_data.vc_id=current_vc;
			temp_data.ready=true;
			temp_data.data_flit.send_times += 1;
			data_out.write(temp_data);
			buffer_req_out = 1;
			success=0;
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



int Processor::add_task(GraphTask &t) {

	task_list.push_back(t);
	return 0;
}

int Processor::sort_task_list() {

	sort( task_list.begin(), task_list.end(), compare);
	//this is important, since we want to put the first task in the waiting list
	//otherwise, the task would not be started
	//if(task_list.size()>0)
	//{
		//task_in_waiting_list.push_back(task_list.at(0).get_id());
	//}
	/*for(int i=0;i<task_list.size()-1;i++)
		for(int j=i+1;j<task_list.size();j++)
		{
			if(task_list.at(i).get_schedule()>=task_list.at(j).get_schedule())
			{
				GraphTask temp=task_list.at(i);
				task_list.at(i)=task_list.at(j);
				task_list.at(j)=temp;

			}
		}
	*/
	return 0;
}


int	Processor::get_id() {

	return core_id;
}

int Processor::get_state(){
	return core_state;
}

int	Processor::set_id(int pid) {

	assert (pid >= 0);
	core_id = pid;

	return 0;
}


#endif

