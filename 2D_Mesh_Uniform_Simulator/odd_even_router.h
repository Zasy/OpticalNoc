#include <systemc.h>
#include "Definition.h"
#include "global_data.h"
#include "defination.h"

class Router : public sc_module {
public:
	SC_HAS_PROCESS(Router);

	Router(sc_module_name name=sc_gen_unique_name("Router")) 
		//: sc_module(name) 
	{
		
		SC_THREAD(receiver);
		dont_initialize();
		sensitive << clk.pos();

		SC_THREAD(sender);
		dont_initialize();
		sensitive << clk.pos();

		SC_THREAD(arbiter);
		dont_initialize();
		sensitive << clk.pos();

		// the temp storage is initialized to zero
		for (int i=0; i<port_no; i++) {
			//reg_data_in[i] = 0;
			reg_data_ready_in[i] = 0;
			//reg_data_out[i] = 0;

			//reg_tear_in[i] = 0;
			reg_tear_ready_in[i] = 0;
			//reg_tear_out[i] = 0;			
			
			//reg_ack_in[i] = 0;
			reg_ack_ready_in[i] = 0;
			//reg_ack_out[i] = 0;		
				
			//reg_partial_in[i] = 0;
			reg_partial_ready_in[i] = 0;
			//reg_partial_out[i] = 0;			
			buffer_req_out[i]=0;
			buffer_tear_out[i]=0;
			buffer_partial_out[i]=0;
			buffer_ack_out[i]=0;
			busy[i] = 0;
			timer[i]=-1;

			input_link_output[i] = -1;									
			rr_pos[i]=0;
			read[i]=0;
			write[i]=0;							
		}

		ack=-1;
		des=-1;
		id = -1;
		pos_x = -1;
		pos_y = -1;
		rr = 0;
		//rt = 4;
		busy_in=0;
		busy_out=0;

		for(int i=0;i<PROCESSOR_NUM;i++){
			ack_flag[i]=0;
		}

		for (int i = 0; i < NOC_WIDTH*NOC_HEIGHT*NOC_HEIGHT*NOC_WIDTH; i++){
			for (int j = 0; j < 4; j++)
			{
				qtable[i][j].loss = 0.0;
				qtable[i][j].tuning_power = 0.0;
				qtable[i][j].N_active = 0;
			}
		}
	}

	
	void receiver() {
     while(true){
		for (int i=0; i<port_no; i++){

			if(timer[i]==0){
				if(i == TO_PROCESSOR){
						if(input_link_output[i]==-1){
							cout<<"error in cluster receiver"<<endl;
							cout<<"port "<<i<<endl;
						}
						busy[input_link_output[i]]=0;
						//cout<<"at time "<<sc_time_stamp()<<" tear down "<<endl;
						if(input_link_output[i] >= PROCESSOR_NUM){
							busy_out=0;
						}
												
						input_link_output[i]=-1;
				}	 
                
				else {
						if(input_link_output[i]==-1){
							cout<<"error in cluster receiver 2"<<endl;
						}					
					
						busy[input_link_output[i]]=0;
						if(input_link_output[i] == TO_PROCESSOR){
							busy_in=0;
						}						
						input_link_output[i]=-1;
				}			
			}
		
			timer[i]=timer[i]-1;		
		
			reg_data_ready_in[i] = data_ready_in[i];
			reg_data_in[i] = data_in[i].read();
			
			reg_ack_ready_in[i] = ack_ready_in[i];
			reg_ack_in[i] = ack_in[i].read();	
					
			reg_tear_ready_in[i] = tear_ready_in[i];
			reg_tear_in[i] = tear_in[i].read();			
			
			reg_partial_ready_in[i] = partial_ready_in[i];
			reg_partial_in[i] = partial_in[i].read();
			
			// store data if ready 
			if (reg_data_ready_in[i] == 1) {

				reg_data_in[i].data_flit.receive_time = sc_time_stamp().to_double() / 1000;
  				request[i][write[i]]=reg_data_in[i];
				write[i]=(write[i]+1)%inputbuffersize;
						
			}
				
			if (reg_tear_ready_in[i] == 1) {
				flit_type temp_flit;
				temp_flit = reg_tear_in[i].data_flit;
				int source_id = temp_flit.src;
				
				int src_y = source_id / NOC_WIDTH;
				int src_x = source_id % NOC_WIDTH;
				int a=0;
				if(src_x>pos_x){
					a=src_x-pos_x;
				}
				else{
					a=pos_x-src_x;
				}
				int b=0;
				if(src_y>pos_y){
					b=src_y-pos_y;
				}
				else{
					b=pos_y-src_y;
				}
				int c = a+b+1;
				timer[i]=temp_flit.packet_length - c;	

				if (timer[i]<1){
					timer[i]=0;
				}
				
				tear_out[input_link_output[i]].write(reg_tear_in[i]);
				buffer_tear_out[input_link_output[i]]=1;
				//cout<<sc_time_stamp()<<" cluster "<<id<<" receive tear down pk"<<endl;
				
			 }			
			
			
			
			if (reg_partial_ready_in[i] == 1) {
                if(i== TO_PROCESSOR){
					cout<<"error in cluster partial tear down"<<endl;
				}
				int k;
							   
	            busy[i]=0;
					
				for(k=0;k<port_no;k++){
					  if (input_link_output[k]==i) {
					      break;
					  }
				}
				if(k == TO_PROCESSOR){
					  busy_out=0;
				}
					
				
					
				//cout<<sc_time_stamp()<<" router "<<id<<" receive partial to processor 0 "<<i<<endl;													
				partial_out[k].write(reg_partial_in[i]);
				buffer_partial_out[k]=1;
				input_link_output[k]=-1;
									
			}	
			
			if (reg_ack_ready_in[i] == 1) {
				
				int k;
				
				for(k=0;k<port_no;k++){
					  if (input_link_output[k]==i) {
					      break;
					  }
				}
					
								
						
				//cout<<sc_time_stamp()<<" router "<<id<<" receive ack to processor 0 "<<i<<endl;													
				ack_out[k].write(reg_ack_in[i]);
				buffer_ack_out[k]=1;
			}		
													
		}	
		wait();
		//rr_pos=(rr_pos+1)%4;
		}
		
	}

	

	void arbiter() {
		int j,k;
		
		while(true){
			wait(0.8, SC_NS);
			//cout << "arbitor time is " << sc_time_stamp().to_double() / 1000 << endl;
			
			k = TO_PROCESSOR;	
			for (int a=0; a<port_no; a++) {
			    j=(a+rr_pos[k])%port_no;	
				int direction = routing(request[j][read[j]].data_flit, j);				
				if (read[j] != write[j] && direction == k) {
					if(j== TO_PROCESSOR) {  
						cout<<"error!! the destination is the source processor"<<endl;
					}
					
					else {
						//int direction = routing(request[j][read[j]].data_flit, j);	
																																																		
						if(direction != TO_PROCESSOR){
							cout<<"error in routing!! the direction is not the processor"<<endl;					
						}	
						
						else{
							if(busy_in==0 && busy[direction]==0){
								busy_in=1;
								busy[direction]=1;
								input_link_output[j]=direction;
								flit_type temp_flit;
								temp_flit = request[j][read[j]].data_flit;

								routing_data temp_routing;
								temp_routing.id = id;
								temp_routing.receive_time = request[j][read[j]].data_flit.receive_time;
								temp_routing.leave_time = sc_time_stamp().to_double() / 1000;

								request[j][read[j]].data_flit.routing_list.push_back(temp_routing);


								TracePocket temp_pocket, temp_pocket1;
								temp_pocket = request[j][read[j]].data_flit.tracedata;
								temp_pocket1 = Get_router_data(j, direction, request[j][read[j]].data_flit, id);
								temp_pocket.loss = temp_pocket.loss + temp_pocket1.loss;
								temp_pocket.tuning_power = temp_pocket.tuning_power + temp_pocket1.tuning_power;
								temp_pocket.N_active = temp_pocket.N_active + temp_pocket1.N_active;
								request[j][read[j]].data_flit.tracedata = temp_pocket;

								ack=temp_flit.src;
								des=temp_flit.dest;

								updated_flag[j - 1] = true;
								router_qpocket[j - 1].dst_id = request[j][read[j]].data_flit.dest;
								router_qpocket[j - 1].src_id = temp_flit.src;

								router_qpocket[j - 1].qvalue.loss = Get_Loss_router(active_number[j][direction], passive_number[j][direction], \
									crossing_number[j][direction]);
								router_qpocket[j - 1].qvalue.tuning_power = Get_tuning_power_router(active_number[j][direction], passive_number[j][direction], \
									chip_temp[temp_flit.src], chip_temp[id]);
								router_qpocket[j - 1].qvalue.N_active = active_number[j][direction];

								if (j == 1){

									router_qpocket[j - 1].trans_id = pos_x - 1 + pos_y*NOC_WIDTH;
									router_qpocket[j - 1].trans_port = RIGHT;
								}
								else if (j == 2){
									router_qpocket[j - 1].trans_id = pos_x + 1 + pos_y*NOC_WIDTH;
									router_qpocket[j - 1].trans_port = LEFT;
								}
								else if (j == 3){
									router_qpocket[j - 1].trans_id = pos_x + (pos_y + 1)*NOC_WIDTH;
									router_qpocket[j - 1].trans_port = DOWN;
								}
								else if (j == 4){
									router_qpocket[j - 1].trans_id = pos_x + (pos_y - 1)*NOC_WIDTH;
									router_qpocket[j - 1].trans_port = UP;
								}

								data_out[direction].write(request[j][read[j]]);
								buffer_req_out[direction]=1;
								rr_pos[k] = (j+1)%port_no;	
								//rr = (k+1)%PROCESSOR_NUM;
								//reg_ack_out[j]=packet.get_code();
								read[j]=(read[j]+1)%inputbuffersize;							
							}
						}																
					
					}
				}  //end if
				
			}
		

	for (int b=PROCESSOR_NUM; b<port_no; b++) {	
				k=b;	
				for (int a=0; a<port_no; a++) {
					j=(a+rr_pos[k])%port_no;	
					int direction = routing(request[j][read[j]].data_flit, j);				
					if (read[j] != write[j] && direction == k) {
						if(j==TO_PROCESSOR) {  
																										
							if (busy_out==0 && busy[direction]==0){
								busy_out=1;	
								busy[direction]=1;
									
								routing_data temp_routing;
								temp_routing.id = id;
								temp_routing.receive_time = request[j][read[j]].data_flit.receive_time;
								temp_routing.leave_time = sc_time_stamp().to_double() / 1000;

								request[j][read[j]].data_flit.routing_list.push_back(temp_routing);


								TracePocket temp_pocket, temp_pocket1;
								temp_pocket = request[j][read[j]].data_flit.tracedata;
								temp_pocket1 = Get_router_data(j, direction, request[j][read[j]].data_flit, id);
								temp_pocket.loss = temp_pocket.loss + temp_pocket1.loss;
								temp_pocket.tuning_power = temp_pocket.tuning_power + temp_pocket1.tuning_power;
								temp_pocket.N_active = temp_pocket.N_active + temp_pocket1.N_active;
								request[j][read[j]].data_flit.tracedata = temp_pocket;


								data_out[direction].write(request[j][read[j]]);
								buffer_req_out[direction]=1;
								rr_pos[k] = (j+1)%port_no;
								input_link_output[j]=direction;	
								read[j]=(read[j]+1)%inputbuffersize;
							}						
							
						}
					
						else {
							
							if(busy[direction]==0){
								busy[direction]=1;
								
								//updated
								routing_data temp_routing;
								temp_routing.id = id;
								temp_routing.receive_time = request[j][read[j]].data_flit.receive_time;
								temp_routing.leave_time = sc_time_stamp().to_double() / 1000;

								request[j][read[j]].data_flit.routing_list.push_back(temp_routing);

								flit_type temp_flit;
								temp_flit = request[j][read[j]].data_flit;

								TracePocket temp_pocket, temp_pocket1;
								temp_pocket = request[j][read[j]].data_flit.tracedata;
								temp_pocket1 = Get_router_data(j, direction, request[j][read[j]].data_flit, id);
								temp_pocket.loss = temp_pocket.loss + temp_pocket1.loss;
								temp_pocket.tuning_power = temp_pocket.tuning_power + temp_pocket1.tuning_power;
								temp_pocket.N_active = temp_pocket.N_active + temp_pocket1.N_active;
								request[j][read[j]].data_flit.tracedata = temp_pocket;


								//add to q-table
								updated_flag[j - 1] = true;
								router_qpocket[j - 1].dst_id = request[j][read[j]].data_flit.dest;
								router_qpocket[j - 1].src_id = temp_flit.src;
								router_qpocket[j - 1].qvalue.loss = Get_Loss_router(active_number[j][direction], passive_number[j][direction], \
									crossing_number[j][direction]) + qtable[NOC_WIDTH*NOC_HEIGHT*temp_flit.src + temp_flit.dest][direction - 1].loss;
								router_qpocket[j - 1].qvalue.tuning_power = Get_tuning_power_router(active_number[j][direction], passive_number[j][direction], \
									chip_temp[temp_flit.src], chip_temp[id]) + qtable[NOC_WIDTH*NOC_HEIGHT*temp_flit.src + temp_flit.dest][direction - 1].tuning_power;
								router_qpocket[j - 1].qvalue.N_active = active_number[j][direction] + qtable[NOC_WIDTH*NOC_HEIGHT*temp_flit.src + temp_flit.dest][direction - 1].N_active;
								if (j == 1){
									router_qpocket[j - 1].trans_id = pos_x - 1 + pos_y*NOC_WIDTH;
									router_qpocket[j - 1].trans_port = RIGHT;
								}
								else if(j == 2){
									router_qpocket[j - 1].trans_id = pos_x + 1 + pos_y*NOC_WIDTH;
									router_qpocket[j - 1].trans_port = LEFT;
								}
								else if(j == 3){
									router_qpocket[j - 1].trans_id = pos_x + (pos_y + 1)*NOC_WIDTH;
									router_qpocket[j - 1].trans_port = DOWN;
								}
								else if (j == 4){
									router_qpocket[j - 1].trans_id = pos_x + (pos_y - 1)*NOC_WIDTH;
									router_qpocket[j - 1].trans_port = UP;
								}

								data_out[direction].write(request[j][read[j]]);								
								buffer_req_out[direction]=1;
								rr_pos[k] = (j+1)%port_no;
								//reg_data_out[direction] = packet.get_code();
								input_link_output[j]=direction;	
								read[j]=(read[j]+1)%inputbuffersize;							
							}
						}
				}  //end if
				
			}	
	}
	wait();
	}
}

	void sender() {
		while(true){
				wait(0.1, SC_NS);
				for(int i=0; i<port_no; i++)	{		
			          if(data_ready_out[i]==1)  {
					       data_ready_out[i]=0;
						   //data_out[i]=0;
						   //if(id==0)
								////cout<<sc_time_stamp()<<" data ready out "<<i<<" is clear"<<endl;						   
					  }
					  if(tear_ready_out[i]==1)  {
					       tear_ready_out[i]=0;
						   //tear_out[i]=0;
						   //if(id==0)
								////cout<<sc_time_stamp()<<" data ready out "<<i<<" is clear"<<endl;						   
					  }
					  
					  if(partial_ready_out[i]==1)  {
					       partial_ready_out[i]=0;
						   //partial_out[i]=0;
						   //if(id==0)
								////cout<<sc_time_stamp()<<" data ready out "<<i<<" is clear"<<endl;						   
					  }		
					  
					if(ack_ready_out[i]==1)  {
					       ack_ready_out[i]=0;
						   //ack_out[i]=0;
						   //if(id==0)
								////cout<<sc_time_stamp()<<" data ready out "<<i<<" is clear"<<endl;						   
					  }		
											  			  
					  
			    }		
		
		
		
		
				wait(0.1, SC_NS);				
				for (int i=0; i<port_no; i++) {
					// send packet if having data, and the receiver has empty buffer space
					if(buffer_req_out[i]==1 || buffer_tear_out[i]==1 || buffer_partial_out[i]==1 || buffer_ack_out[i]==1){
						if (buffer_req_out[i]==1) {				
							data_ready_out[i].write(1);	
							buffer_req_out[i]=0;		
						}		
						if (buffer_tear_out[i]==1) {
							tear_ready_out[i].write(1);	
							buffer_tear_out[i]=0;	
						}	
						if (buffer_partial_out[i]==1) {				
							partial_ready_out[i].write(1);	
							buffer_partial_out[i]=0;		
						}		
						if (buffer_ack_out[i]==1) {
							ack_ready_out[i].write(1);	
							buffer_ack_out[i]=0;	
						}	
					
					}																						
					
				}
				wait();
		
		}
	}
	

	
	void setid(int num) {
		id = num;
	}
	int  getid() {
		return id;
	}

	void setpos(int x, int y) {
		pos_x = x;
		pos_y = y;
	}

	void setqtable(){
		int i;
		for (i = 0; i < 4; i++){
			updated_flag[i] = false;
		}

	}
	void getpos(int &x, int &y) {
		x = pos_x; 
		y = pos_y;
	}
	
	void get_xy(int address, int &x, int &y) 
{
	y = address / NOC_WIDTH;
	x = address % NOC_WIDTH;
	assert (address == y*NOC_WIDTH + x);
}

	int  routing(flit_type head_flit, int j) {
		int dst=head_flit.dest;
		int src = head_flit.src;

		int dst_x=-1;
		int dst_y=-1;
		int src_x = -1;
		int src_y = -1;

		int d, direction;

		get_xy(src, src_x, src_y);
		get_xy(dst, dst_x, dst_y);

		int e_x = dst_x - pos_x;
		int e_y = dst_y - pos_y;

		vector<int> avil_d;
		if (e_x == 0 && e_y == 0){
			return 0;
		}

		if (e_x == 0)
		{
			if (e_y > 0)
			{
				//avil_d[UP] = 1;
				avil_d.push_back(3);
			}
			else{
				avil_d.push_back(4);
			}
		}
		else
		{
			if (e_x > 0){
				if (e_y == 0){
					avil_d.push_back(2);
				}
				else
				{
					if (dst_x % 2 == 1 || e_x != 1){
						avil_d.push_back(2);
					}
					if ((pos_x % 2 == 1) || pos_x == src_x){
						if (e_y > 0){
							avil_d.push_back(3);
						}
						else{
							avil_d.push_back(4);
						}
					}
				}
			}
			else{
				avil_d.push_back(1);
				if (pos_x % 2 == 0){
					if (e_y > 0){
						avil_d.push_back(3);
					}
					else if(e_y < 0)
					{
						avil_d.push_back(4);
					}
				}
			}
		}
		//return avil_d[0];
		//return avil_d[rand()%avil_d.size()];
		double temp_value;
		direction = avil_d[0];
		d = avil_d[0];
		TracePocket temp_pocket = qtable[NOC_HEIGHT*NOC_WIDTH*src + dst][d - 1];
		TracePocket temp_router = Get_router_data(j, d, head_flit, id);

		temp_value = power_total(chip_temp[head_flit.src], temp_router, head_flit.tracedata, \
			temp_pocket, Get_hopnum(head_flit.src, head_flit.dest));
		/*
		if (num_of_1 != 0)/n
		{
		cout << "the router no is " << id << " " << "the num is " << num_of_1 << endl;
		}
		*/

		for (int i = 1; i < avil_d.size(); i++)
		{
			//cout << temp_value << endl;
			//cout << qtable[NOC_HEIGHT*NOC_WIDTH*src + dst][avil_d[i] - 1] << endl;
			d = avil_d[i];
			temp_pocket = qtable[NOC_HEIGHT*NOC_WIDTH*src + dst][d - 1];
			temp_router = Get_router_data(j, d, head_flit, id);

			if (power_total(chip_temp[head_flit.src], temp_router, head_flit.tracedata, temp_pocket, Get_hopnum(head_flit.src, head_flit.dest)) < temp_value){
				//cout << "NOC_WIDTH + NOC_HEIGHT \t" << NOC_WIDTH << "\t" << NOC_HEIGHT << endl;
				//cout << "Data========================================" << endl;
				direction = avil_d[i];
				temp_value = power_total(chip_temp[head_flit.src], temp_router, head_flit.tracedata, temp_pocket, Get_hopnum(head_flit.src, head_flit.dest));
				//cout << "change the initial router " << src << "\t" << dst << endl;
			}
		}

		return direction;
	}	
	
	
	

public:
	sc_in <bool>		clk;
	// output port, including data out, data ready and feedback signal of full buffer from receiver
	sc_out<router_forward_interface> data_out[port_no];
	sc_out<bool>					 data_ready_out[port_no];
	sc_out<router_forward_interface> ack_out[port_no];
	sc_out<bool>					 ack_ready_out[port_no];	
	sc_out<router_forward_interface> tear_out[port_no];
	sc_out<bool>					 tear_ready_out[port_no];
	sc_out<router_forward_interface> partial_out[port_no];
	sc_out<bool>					 partial_ready_out[port_no];	
		
	// input port, including data in, data ready and feedback signal of full buffer to sender
	sc_in <router_forward_interface> data_in[port_no];
	sc_in <bool>					 data_ready_in[port_no];
	sc_in <router_forward_interface> tear_in[port_no];
	sc_in <bool>					 tear_ready_in[port_no];
	sc_in <router_forward_interface> partial_in[port_no];
	sc_in <bool>					 partial_ready_in[port_no];			
	sc_in <router_forward_interface> ack_in[port_no];
	sc_in <bool>					 ack_ready_in[port_no];	
	
	
	bool							 busy[port_no];	// used by input port
	int								 timer[port_no];
	
	// used by input port
	router_forward_interface		reg_data_in[port_no];			// temp storage of input data before deciding which vc to write in
	bool							reg_data_ready_in[port_no];	// temp storage of input data ready signal
	router_forward_interface		reg_tear_in[port_no];			// temp storage of input data before deciding which vc to write in
	bool							reg_tear_ready_in[port_no];	// temp storage of input data ready signal	
	router_forward_interface		reg_partial_in[port_no];			// temp storage of input data before deciding which vc to write in
	bool							reg_partial_ready_in[port_no];	// temp storage of input data ready signal	
	router_forward_interface		reg_ack_in[port_no];			// temp storage of input data before deciding which vc to write in
	bool							reg_ack_ready_in[port_no];	// temp storage of input data ready signal	
	
	bool buffer_req_out[port_no];
	bool buffer_tear_out[port_no];
	bool buffer_partial_out[port_no];
	bool buffer_ack_out[port_no];

	// used by output port
	router_forward_interface		reg_data_out[port_no];		// temp storage for output
	router_forward_interface        reg_tear_out[port_no];
	router_forward_interface        reg_partial_out[port_no];
	router_forward_interface        reg_ack_out[port_no];	
	
	router_forward_interface		reg_head_data[port_no];
	int								buffer_counter_in[port_no];	// 0 for no space, 1 for one space
	int								buffer_counter_out[port_no];	// 0 for no space, 1 for 1 space, 2 for more than 1 space
	router_forward_interface        flag_set[port_no];
	sc_signal<int>					flag_success[port_no];

	int								input_link_output[port_no];
	
    router_forward_interface        request[port_no][inputbuffersize];
	int								delay[port_no];
//new
	int                 data_delay[port_no];
	int                 tear_delay[port_no];
	int                 partial_delay[port_no];
	int                 ack_delay[port_no];



	int                 read[port_no];
	int					write[port_no];
	
	int					packet_num;
	// internal variables
	int					id;
	int					pos_x;
	int					pos_y;
	int					ack;
	int                 des;
	int					tear_num;
	int					partial_num;
	
	int					send;
	int					rr_pos[port_no];
	int					rr;
	int					rt;
	bool				ack_flag[PROCESSOR_NUM];
	bool				busy_out;
	bool				busy_in;
	
	TracePocket         (*qtable)[4] = new TracePocket[NOC_WIDTH*NOC_HEIGHT*NOC_WIDTH*NOC_HEIGHT][4];

	bool				updated_flag[4];
	qpocket				router_qpocket[4];

};

//#endif


