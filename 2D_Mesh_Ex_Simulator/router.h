#include <systemc.h>
#include "Definition.h"
#include "global_data.h"
// #include "shortest11.h"
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
		pos_z = -1;
		rr = 0;
		//rt = 4;
		busy_in=0;
		busy_out=0;
		//cout<<"KOKOKOKOKOKO"<<endl;
        //cout<<table_direction[0][0]<<" "<<table_direction[1][0]<<" "<<table_direction[2][0]<<" "<<table_direction[3][0]<<endl;
		for(int i=0;i<PROCESSOR_NUM;i++){
			ack_flag[i]=0;
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
				int src_z = source_id / (NOC_WIDTH * NOC_HEIGHT);
				int src_y = (source_id - src_z*(NOC_WIDTH * NOC_HEIGHT)) / NOC_WIDTH;
				int src_x = (source_id - src_z*(NOC_WIDTH * NOC_HEIGHT)) % NOC_WIDTH;
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
				int c=0;
				if(src_z>pos_z){
					c=src_z-pos_z;
				}
				else{
					c=pos_z-src_z;
				}
				int d = a+b+c+1;
				timer[i]=temp_flit.packet_length - d;
				if (timer[i]<1){
					timer[i]=0;
				}
				//cout << "link: " << input_link_output[i] <<endl;
				tear_out[input_link_output[i]].write(reg_tear_in[i]);
				buffer_tear_out[input_link_output[i]]=1;
				//cout<<sc_time_stamp()<<" cluster "<<id<<" receive tear down pk"<<endl;
				//cout << "end." << endl;
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

			k = TO_PROCESSOR;
			for (int a=0; a<port_no; a++) {
			    j=(a+rr_pos[k])%port_no;
				int direction = routing(request[j][read[j]].data_flit, j);
				if (read[j] != write[j] && direction == k) {
					if(j== TO_PROCESSOR) {
						cout<<"error!! the destination is the source processor"<<endl;
					}

					else {
						int direction = routing(request[j][read[j]].data_flit, j);

						if(direction != TO_PROCESSOR){
							cout<<"error in routing!! the direction is not the processor"<<endl;
						}

						else{
							if(busy_in==0 && busy[direction]==0){
								routing_data temp_routing;
								temp_routing.id = id;
								temp_routing.receive_time = request[j][read[j]].data_flit.receive_time;
							    temp_routing.leave_time = sc_time_stamp().to_double() / 1000;
							    request[j][read[j]].data_flit.routing_list.push_back(temp_routing);	

								double temp_loss;
								temp_loss = get_router_loss(j, direction, id);
								temp_loss += link_length*L_propagate;
								int temp_active = active_number[j][direction];

								request[j][read[j]].data_flit.N_active += temp_active;
								request[j][read[j]].data_flit.optical_loss += temp_loss;

								busy_in=1;
								busy[direction]=1;
								input_link_output[j]=direction;


								double optical_loss=0;
								if(j==TO_PROCESSOR){
									if(direction==NORTH){
										optical_loss=inject_north;
									}
									else if(direction==SOUTH){
										optical_loss=inject_south;
									}
									else if(direction==WEST){
										optical_loss=inject_west;
									}
									else if(direction==EAST){
										optical_loss=inject_east;
									}
									else if(direction==TSV_UP){
										optical_loss=inject_up;
									}
									else if(direction==TSV_DOWN){
										optical_loss=inject_down;
									}
									else {
										cout<<sc_time_stamp()<<" error in routing 1"<<endl;
									}
								}
								else if(j==NORTH){

									if(direction==SOUTH){
										optical_loss=north_south+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=north_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=north_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=north_eject+waveguide+crossing;
									}
									//else {

										//cout<<sc_time_stamp()<<" error in routing 2"<<endl;
									//}
									if((pos_z==0 && pos_x==0) || (pos_z==1 && pos_x==NOC_WIDTH-1)){
										optical_loss=optical_loss-crossing;
									}
								}
								else if(j==SOUTH){

									if(direction==NORTH){
										optical_loss=south_north+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=south_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=south_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=south_eject+waveguide+crossing;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 3"<<endl;
									//}
									if((pos_z==0 && pos_x==0) || (pos_z==1 && pos_x==NOC_WIDTH-1)){
										optical_loss=optical_loss-crossing;
									}
								}
								else if(j==WEST){

									if(direction==NORTH){
										optical_loss=west_north+waveguide+crossing;
									}
									else if(direction==SOUTH){
										optical_loss=west_south+waveguide+crossing;
									}

									else if(direction==EAST){
										optical_loss=west_east+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=west_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=west_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=west_eject+waveguide+crossing;
									}
									else {
										cout<<sc_time_stamp()<<" error in routing 4"<<endl;
									}
									if((pos_z==0 && pos_y==NOC_HEIGHT-1) || (pos_z==1 && pos_y==0)){
										optical_loss=optical_loss-crossing;
									}
								}
								else if(j==EAST){

									if(direction==NORTH){
										optical_loss=east_north+waveguide+crossing;
									}
									else if(direction==SOUTH){
										optical_loss=east_south+waveguide+crossing;
									}

									else if(direction==WEST){
										optical_loss=east_west+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=east_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=east_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=east_eject+waveguide+crossing;
									}
									else {
										cout<<sc_time_stamp()<<" error in routing 5"<<endl;
									}
									if((pos_z==0 && pos_y==NOC_HEIGHT-1) || (pos_z==1 && pos_y==0)){
										optical_loss=optical_loss-crossing;
									}
								}

								else if(j==TSV_UP){


									if(direction==TO_PROCESSOR){
										optical_loss=up_eject+waveguide;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 61"<<endl;
									//}

								}
								else if(j==TSV_DOWN){


									if(direction==TO_PROCESSOR){
										optical_loss=down_eject+waveguide;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 7"<<endl;
									//}
								}

								flit_type temp_flit;
								temp_flit = request[j][read[j]].data_flit;
								ack=temp_flit.src;
								des=temp_flit.dest;

								temp_flit.loss=temp_flit.loss+optical_loss;


								// if(temp_flit.dest == 17 && temp_flit.src == 0){
								// 	cout << "id: " << id <<" input_port: " << j << " output: " << direction << " loss:" << temp_loss << endl;
								// }

								router_forward_interface temp_data;
								temp_data.data_flit=temp_flit;

								//data_out[direction].write(temp_data);
								data_out[direction].write(request[j][read[j]]);
								buffer_req_out[direction]=1;

								
								updated_flag[j - 1] = true;
								router_qpocket[j - 1].dst_id = temp_flit.dest;
								router_qpocket[j - 1].src_id = temp_flit.src;
								router_qpocket[j - 1].loss = temp_loss;
								// router_qpocket[j - 1].trans_flit = temp_flit;
								
								if (j == 1) {
									router_qpocket[j - 1].trans_id = (pos_x + NOC_WIDTH - 1) % NOC_WIDTH + pos_y*NOC_WIDTH + pos_z*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j - 1].trans_port = RIGHT;
								}
								else if (j == 2) {
									router_qpocket[j -1].trans_id = (pos_x + NOC_WIDTH + 1) % NOC_WIDTH + pos_y*NOC_WIDTH + pos_z*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = LEFT;
								}
								else if (j == 3) {
									router_qpocket[j -1].trans_id = pos_x + ((pos_y + 1 + NOC_HEIGHT) % NOC_HEIGHT) * NOC_WIDTH + pos_z*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = DOWN;
								}
								else if (j == 4){
									router_qpocket[j -1].trans_id = pos_x + ((pos_y - 1 + NOC_HEIGHT) % NOC_HEIGHT) * NOC_WIDTH + pos_z*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = UP;
								}
								else if (j == 5){
									router_qpocket[j -1].trans_id = pos_x + pos_y*NOC_WIDTH + (pos_z + 1)*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = TSV_DOWN;
								}
								else if(j == 6){
									router_qpocket[j -1].trans_id = pos_x + pos_y*NOC_WIDTH + (pos_z - 1)*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = TSV_UP;
								}	

								rr_pos[k] = (j+1)%port_no;
								//rr = (k+1)%PROCESSOR_NUM;
								//reg_ack_out[j]=packet.get_code();
								read[j]=(read[j]+1)%inputbuffersize;
							}
							else{
								// partial_out[j].write(request[j][read[j]]);
								// buffer_partial_out[j]=1;

								// read[j]=(read[j]+1)%inputbuffersize;
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
								routing_data temp_routing;
								temp_routing.id = id;
								temp_routing.receive_time = request[j][read[j]].data_flit.receive_time;
								temp_routing.leave_time = sc_time_stamp().to_double() / 1000;
								request[j][read[j]].data_flit.routing_list.push_back(temp_routing);									

								double temp_loss;
								temp_loss = get_router_loss(j, direction, id);
								int temp_active = active_number[j][direction];

								request[j][read[j]].data_flit.optical_loss += temp_loss;
								request[j][read[j]].data_flit.N_active += temp_active;
								busy_out=1;
								busy[direction]=1;

								double optical_loss=0;
								if(j==TO_PROCESSOR){
									if(direction==NORTH){
										optical_loss=inject_north;
									}
									else if(direction==SOUTH){
										optical_loss=inject_south;
									}
									else if(direction==WEST){
										optical_loss=inject_west;
									}
									else if(direction==EAST){
										optical_loss=inject_east;
									}
									else if(direction==TSV_UP){
										optical_loss=inject_up;
									}
									else if(direction==TSV_DOWN){
										optical_loss=inject_down;
									}
									else {
										cout<<sc_time_stamp()<<" error in routing 1"<<endl;
									}
								}
								else if(j==NORTH){

									if(direction==SOUTH){
										optical_loss=north_south+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=north_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=north_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=north_eject+waveguide+crossing;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 2"<<endl;
									//}
									if((pos_z==0 && pos_x==0) || (pos_z==1 && pos_x==NOC_WIDTH-1)){
										optical_loss=optical_loss-crossing;
									}
								}
								else if(j==SOUTH){

									if(direction==NORTH){
										optical_loss=south_north+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=south_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=south_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=south_eject+waveguide+crossing;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 3"<<endl;
									//}
									if((pos_z==0 && pos_x==0) || (pos_z==1 && pos_x==NOC_WIDTH-1)){
										optical_loss=optical_loss-crossing;
									}
								}
								else if(j==WEST){

									if(direction==NORTH){
										optical_loss=west_north+waveguide+crossing;
									}
									else if(direction==SOUTH){
										optical_loss=west_south+waveguide+crossing;
									}

									else if(direction==EAST){
										optical_loss=west_east+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=west_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=west_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=west_eject+waveguide+crossing;
									}
									else {
										cout<<sc_time_stamp()<<" error in routing 4"<<endl;
									}
									if((pos_z==0 && pos_y==NOC_HEIGHT-1) || (pos_z==1 && pos_y==0)){
										optical_loss=optical_loss-crossing;
									}
								}
								else if(j==EAST){

									if(direction==NORTH){
										optical_loss=east_north+waveguide+crossing;
									}
									else if(direction==SOUTH){
										optical_loss=east_south+waveguide+crossing;
									}

									else if(direction==WEST){
										optical_loss=east_west+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=east_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=east_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=east_eject+waveguide+crossing;
									}
									else {
										cout<<sc_time_stamp()<<" error in routing 5"<<endl;
									}
									if((pos_z==0 && pos_y==NOC_HEIGHT-1) || (pos_z==1 && pos_y==0)){
										optical_loss=optical_loss-crossing;
									}
								}

								else if(j==TSV_UP){


									if(direction==TO_PROCESSOR){
										optical_loss=up_eject+waveguide;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 62"<<endl;
									//}

								}
								else if(j==TSV_DOWN){


									if(direction==TO_PROCESSOR){
										optical_loss=down_eject+waveguide;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 7"<<endl;
									//}
								}



								flit_type temp_flit;
								temp_flit = request[j][read[j]].data_flit;
								temp_flit.loss=temp_flit.loss+optical_loss;

								router_forward_interface temp_data;
								temp_data.data_flit=temp_flit;
								
								// if(temp_flit.dest == 17 && temp_flit.src == 0){
								// 	cout << "id: " << id <<" input_port: " << j << " output: " << direction << " loss:" << temp_loss << endl;
								// }

								//data_out[direction].write(temp_data);
								data_out[direction].write(request[j][read[j]]);
								buffer_req_out[direction]=1;
								rr_pos[k] = (j+1)%port_no;
								input_link_output[j]=direction;
								read[j]=(read[j]+1)%inputbuffersize;
							}

						}

						else {

							if(busy[direction]==0){
								routing_data temp_routing;
								temp_routing.id = id;
								temp_routing.receive_time = request[j][read[j]].data_flit.receive_time;
								temp_routing.leave_time = sc_time_stamp().to_double() / 1000;
								request[j][read[j]].data_flit.routing_list.push_back(temp_routing);

								double temp_loss;
								temp_loss = get_router_loss(j, direction, id);

								temp_loss += (link_length * L_propagate);
								request[j][read[j]].data_flit.optical_loss  = request[j][read[j]].data_flit.optical_loss + temp_loss;
								
								int temp_active = active_number[j][direction];
								request[j][read[j]].data_flit.N_active += temp_active;
								
								busy[direction]=1;

								double optical_loss=0;
								if(j==TO_PROCESSOR){
									if(direction==NORTH){
										optical_loss=inject_north;
									}
									else if(direction==SOUTH){
										optical_loss=inject_south;
									}
									else if(direction==WEST){
										optical_loss=inject_west;
									}
									else if(direction==EAST){
										optical_loss=inject_east;
									}
									else if(direction==TSV_UP){
										optical_loss=inject_up;
									}
									else if(direction==TSV_DOWN){
										optical_loss=inject_down;
									}
									else {
										cout<<sc_time_stamp()<<" error in routing 1"<<endl;
									}
								}
								else if(j==NORTH){

									if(direction==SOUTH){
										optical_loss=north_south+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=north_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=north_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=north_eject+waveguide+crossing;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 2"<<endl;
									//}
									if((pos_z==0 && pos_x==0) || (pos_z==1 && pos_x==NOC_WIDTH-1)){
										optical_loss=optical_loss-crossing;
									}
								}
								else if(j==SOUTH){

									if(direction==NORTH){
										optical_loss=south_north+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=south_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=south_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=south_eject+waveguide+crossing;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 3"<<endl;
									//}
									if((pos_z==0 && pos_x==0) || (pos_z==1 && pos_x==NOC_WIDTH-1)){
										optical_loss=optical_loss-crossing;
									}
								}
								else if(j==WEST){

									if(direction==NORTH){
										optical_loss=west_north+waveguide+crossing;
									}
									else if(direction==SOUTH){
										optical_loss=west_south+waveguide+crossing;
									}

									else if(direction==EAST){
										optical_loss=west_east+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=west_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=west_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=west_eject+waveguide+crossing;
									}
									else {
										cout<<sc_time_stamp()<<" error in routing 4"<<endl;
									}
									if((pos_z==0 && pos_y==NOC_HEIGHT-1) || (pos_z==1 && pos_y==0)){
										optical_loss=optical_loss-crossing;
									}
								}
								else if(j==EAST){

									if(direction==NORTH){
										optical_loss=east_north+waveguide+crossing;
									}
									else if(direction==SOUTH){
										optical_loss=east_south+waveguide+crossing;
									}

									else if(direction==WEST){
										optical_loss=east_west+waveguide+crossing;
									}
									else if(direction==TSV_UP){
										optical_loss=east_up+waveguide+crossing;
									}
									else if(direction==TSV_DOWN){
										optical_loss=east_down+waveguide+crossing;
									}
									else if(direction==TO_PROCESSOR){
										optical_loss=east_eject+waveguide+crossing;
									}
									else {
										cout<<sc_time_stamp()<<" error in routing 5"<<endl;
									}
									if((pos_z==0 && pos_y==NOC_HEIGHT-1) || (pos_z==1 && pos_y==0)){
										optical_loss=optical_loss-crossing;
									}
								}

								else if(j==TSV_UP){


									if(direction==TO_PROCESSOR){
										optical_loss=up_eject+waveguide;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 63"<<endl;
									//}

								}
								else if(j==TSV_DOWN){


									if(direction==TO_PROCESSOR){
										optical_loss=down_eject+waveguide;
									}
									//else {
										//cout<<sc_time_stamp()<<" error in routing 7"<<endl;
									//}
								}

								flit_type temp_flit;
								temp_flit = request[j][read[j]].data_flit;
								temp_flit.loss=temp_flit.loss+optical_loss;


								// if(temp_flit.dest == 17 && temp_flit.src == 0){
								// 	cout << "id: " << id <<" input_port: " << j << " output: " << direction << " loss:" << temp_loss << endl;
								// }

								router_forward_interface temp_data;
								temp_data.data_flit=temp_flit;




								//data_out[direction].write(temp_data);
								data_out[direction].write(request[j][read[j]]);
								buffer_req_out[direction]=1;

								updated_flag[j - 1] = true;
								router_qpocket[j - 1].dst_id = temp_flit.dest;
								router_qpocket[j - 1].src_id = temp_flit.src;
								router_qpocket[j - 1].loss = temp_loss + qtable[temp_flit.dest][direction - 1];

								//router_qpocket[j - 1].trans_flit = temp_flit;
								if (j == 1) {							
									router_qpocket[j - 1].trans_id = (pos_x + NOC_WIDTH - 1) % NOC_WIDTH + pos_y*NOC_WIDTH + pos_z*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j - 1].trans_port = RIGHT;
								}
								else if (j == 2) {
									router_qpocket[j -1].trans_id = (pos_x + NOC_WIDTH + 1) % NOC_WIDTH + pos_y*NOC_WIDTH + pos_z*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = LEFT;
								}
								else if (j == 3) {
									router_qpocket[j -1].trans_id = pos_x + ((pos_y + 1 + NOC_HEIGHT) % NOC_HEIGHT) * NOC_WIDTH + pos_z*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = DOWN;
								}
								else if (j == 4){
									router_qpocket[j -1].trans_id = pos_x + ((pos_y - 1 + NOC_HEIGHT) % NOC_HEIGHT) * NOC_WIDTH + pos_z*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = UP;
								}
								else if (j == 5){
									router_qpocket[j -1].trans_id = pos_x + pos_y*NOC_WIDTH + (pos_z + 1)*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = TSV_DOWN;
								}
								else if(j == 6){
									router_qpocket[j -1].trans_id = pos_x + pos_y*NOC_WIDTH + (pos_z - 1)*(NOC_WIDTH*NOC_HEIGHT);
									router_qpocket[j -1].trans_port = TSV_UP;
								}


								rr_pos[k] = (j+1)%port_no;
								//reg_data_out[direction] = packet.get_code();
								input_link_output[j]=direction;
								read[j]=(read[j]+1)%inputbuffersize;
							}
							else{

								if (routing_algo == 0 || routing_algo == 1 || routing_algo == 3){
									partial_out[j].write(request[j][read[j]]);
									buffer_partial_out[j]=1;
									read[j]=(read[j]+1)%inputbuffersize;
								}

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

	void setpos(int x, int y, int z) {
		pos_x = x;
		pos_y = y;
		pos_z = z;
	}
	void getpos(int &x, int &y, int &z) {
		x = pos_x;
		y = pos_y;
		z = pos_z;
	}

	void get_xyz(int address, int &x, int &y, int &z)
{
	z = address / (NOC_WIDTH * NOC_HEIGHT);
	y = (address - z*(NOC_WIDTH * NOC_HEIGHT)) / NOC_WIDTH;
	x = (address - z*(NOC_WIDTH * NOC_HEIGHT)) % NOC_WIDTH;
	assert (address == z*(NOC_WIDTH*NOC_HEIGHT)+y*NOC_WIDTH + x);
}
/*
	int  routing(flit_type head_flit, int j) {
		int dst=head_flit.dest;
		int dst_x=-1;
		int dst_y=-1;
		int dst_z=-1;
		get_xyz(dst, dst_x, dst_y, dst_z);

		int d = -1;

		if (pos_x != dst_x) {
			if (pos_x < dst_x) {
				if((dst_x - pos_x) <= (pos_x + NOC_WIDTH - dst_x)){
					d = RIGHT;
				}else{
					d = LEFT;
				}
			}
			else if (pos_x > dst_x) {
				if ((pos_x - dst_x) <= (dst_x + NOC_WIDTH - pos_x)){
					d = LEFT;
				}else{
					d = RIGHT;
				}
			}

		}
		else if(pos_y != dst_y){
			if (pos_y < dst_y){
				if((dst_y - pos_y) <= (pos_y + NOC_WIDTH - dst_y)){
					d = UP;
				}else {
					d = DOWN;
				}
			}
			else if (pos_y > dst_y){
				if((pos_y - dst_y) <= (dst_y + NOC_WIDTH - pos_y)){
					d = DOWN;
				}else {
					d = UP;
				}
			}
		}
		else {
			if (pos_z < dst_z) {
				d = TSV_UP;
			}
			else if (pos_z > dst_z) {
				d = TSV_DOWN;
			}
			else {
				d = TO_PROCESSOR;
			}

		}

		if (d<0) {
				assert(0);
		}

        //cout<<d<<"   ";
		return d;

	}
*/
/*
	int  routing(flit_type head_flit, int j) {
		int dst=head_flit.dest;
		int dst_x=-1;
		int dst_y=-1;
		int dst_z=-1;
		get_xyz(dst, dst_x, dst_y, dst_z);

		int d = -1;

		if (pos_x != dst_x) {
			if (pos_x < dst_x) {
				d = RIGHT;
			}
			else if (pos_x > dst_x) {
				d = LEFT;
			}

		}
		else if (pos_y != dst_y){
			if (pos_y < dst_y) {
				d = UP;
			}
			else if (pos_y > dst_y) {
				d = DOWN;
			}

		}
		else {
			if (pos_z < dst_z) {
				d = TSV_UP;
			}
			else if (pos_z > dst_z) {
				d = TSV_DOWN;
			}
			else {
				d = TO_PROCESSOR;
			}

		}

		if (d<0) {
				assert(0);
		}

        //cout<<d<<"   ";
		return d;
	}
*/
/*
	int  routing(flit_type head_flit, int j) {
		int dst=head_flit.dest;
		int dst_x=-1;
		int dst_y=-1;
		int dst_z=-1;
		get_xyz(dst, dst_x, dst_y, dst_z);

		vector<int> avil_d;

		int d, direction;
		double temp_value;

		if(dst == head_flit.src){
			return 1;
		}
		
		if((dst_x == pos_x) && (dst_y == pos_y) && (dst_z == pos_z)){
			return TO_PROCESSOR;
		}

		if (pos_x != dst_x) {
			if (pos_x < dst_x) {
				avil_d.push_back(RIGHT);
			}
			else if (pos_x > dst_x) {
				avil_d.push_back(LEFT);
			}

		}
		if (pos_y != dst_y){
			if (pos_y < dst_y) {
				avil_d.push_back(UP);
			}
			else if (pos_y > dst_y) {
				avil_d.push_back(UP);
			}

		}
		
		if (pos_z < dst_z) {
			avil_d.push_back(TSV_UP);
		}
		else if (pos_z > dst_z) {
			avil_d.push_back(TSV_DOWN);
		}



		if (d<0) {
				assert(0);
		}

        d = avil_d[0];
		temp_value = qtable[dst][d - 1];
		for (int i = 1; i < avil_d.size(); i++){
			direction = avil_d[i];
			if (qtable[dst][direction - 1] < temp_value){

				// cout << "size: " << avil_d.size() << endl;
				// cout << "temp_value: " << temp_value << endl;
				// cout << "compare_value: " << qtable[dst][direction - 1] << endl;
				// cout << endl; 
				d = direction;
				temp_value = qtable[dst][d - 1];
			}
		}

		return d;
	}



*/
/*
int routing(flit_type head_flit, int j){
 	int dst=head_flit.dest;
	int src = head_flit.src;
	
	int dst_x=-1;
	int dst_y=-1;
	int dst_z=-1;

	int src_x = -1;
	int src_y = -1;
	int src_z = -1;

	get_xyz(dst, dst_x, dst_y, dst_z);
	get_xyz(src, src_x, src_y, src_z);
	vector<int> avil_d;

	int d, direction;
	double temp_value;

	int e_x = dst_x - pos_x;
	int e_y = dst_y - pos_y;

	if(dst == head_flit.src){
		return 1;
	}
	
	if((dst_x == pos_x) && (dst_y == pos_y) && (dst_z == pos_z)){
		return TO_PROCESSOR;
	}
	

	if((dst_x == pos_x) && (dst_y == pos_y)){
		if (dst_z > pos_z){
			avil_d.push_back(TSV_UP);
		}else if(dst_z < pos_z){
			avil_d.push_back(TSV_DOWN);
		}else{
			avil_d.push_back(TO_PROCESSOR);
		}
	}else{
		
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
					else if (e_y < 0)
					{
						avil_d.push_back(4);
					}
				}
			}
		}
	}
	
	d = avil_d[0];
	temp_value = qtable[dst][d - 1];
	for (int i = 1; i < avil_d.size(); i++){
		direction = avil_d[i];

		if (qtable[dst][direction] < temp_value){

			// cout << "size: " << avil_d.size() << endl;
			// cout << "temp_value: " << temp_value << endl;
			// cout << "compare_value: " << qtable[dst][direction - 1] << endl;
			// cout << endl; 
			d = direction;
			temp_value = qtable[dst][d - 1];
		}
	}


	return d; 

}
*/

/*
 * router type:
 * 0 - torus q-learning
 * 1 - torus XYZ
 * 2 - mesh XYZ
 * 3 - mesh q-learning
 * */


int routing(flit_type head_flit, int j){
	int dst=head_flit.dest;
	int src = head_flit.src;
	
	int dst_x=-1;
	int dst_y=-1;
	int dst_z=-1;

	int src_x = -1;
	int src_y = -1;
	int src_z = -1;

	get_xyz(dst, dst_x, dst_y, dst_z);
	get_xyz(src, src_x, src_y, src_z);
	vector<int> avil_d;

	int d, direction;
	double temp_value;

	if(dst == head_flit.src){
		return 1;
	}
	
	if((dst_x == pos_x) && (dst_y == pos_y) && (dst_z == pos_z)){
		return TO_PROCESSOR;
	}



	switch(routing_algo){
		case 0:
			if (pos_x < dst_x){
				if ((dst_x - pos_x) <= (pos_x + NOC_WIDTH - dst_x)){
					avil_d.push_back(RIGHT);
				}else {
					avil_d.push_back(LEFT);
				}
			}else if (pos_x > dst_x){
				if ((pos_x - dst_x) <= (dst_x + NOC_WIDTH - pos_x)){
					avil_d.push_back(LEFT);
				}else {
					avil_d.push_back(RIGHT);
				}
			}

			if (pos_y < dst_y){
				if ((dst_y - pos_y) <= (pos_y + NOC_HEIGHT - dst_y)){
					avil_d.push_back(UP);
				}else {
					avil_d.push_back(DOWN);
				}
			}else if (pos_y > dst_y){
				if ((pos_y - dst_y) <= (dst_y + NOC_HEIGHT - pos_y)){
					avil_d.push_back(DOWN);
				}else {
					avil_d.push_back(UP);
				}
			}

			if (pos_z < dst_z){
				avil_d.push_back(TSV_UP);
			}else if(pos_z > dst_z){
				avil_d.push_back(TSV_DOWN);
			}

			d = avil_d[0];
			temp_value = qtable[dst][d - 1];
			for (int i = 1; i < avil_d.size(); i++){
				direction = avil_d[i];
				if (qtable[dst][direction - 1] < temp_value){

					// cout << "size: " << avil_d.size() << endl;
					// cout << "temp_value: " << temp_value << endl;
					// cout << "compare_value: " << qtable[dst][direction - 1] << endl;
					// cout << endl; 
					d = direction;
					temp_value = qtable[dst][d - 1];
				}
			}

			break;
		
		case 1:

			if (pos_x != dst_x) {
				if (pos_x < dst_x) {
					if((dst_x - pos_x) <= (pos_x + NOC_WIDTH - dst_x)){
						d = RIGHT;
					}else{
						d = LEFT;
					}
				}
				else if (pos_x > dst_x) {
					if ((pos_x - dst_x) <= (dst_x + NOC_WIDTH - pos_x)){
						d = LEFT;
					}else{
						d = RIGHT;
					}
				}

			}
			else if(pos_y != dst_y){
				if (pos_y < dst_y){
					if((dst_y - pos_y) <= (pos_y + NOC_WIDTH - dst_y)){
						d = UP;
					}else {
						d = DOWN;
					}
				}
				else if (pos_y > dst_y){
					if((pos_y - dst_y) <= (dst_y + NOC_WIDTH - pos_y)){
						d = DOWN;
					}else {
						d = UP;
					}
				}
			}
			else {
				if (pos_z < dst_z) {
					d = TSV_UP;
				}
				else if (pos_z > dst_z) {
					d = TSV_DOWN;
				}
				else {
					d = TO_PROCESSOR;
				}

			}

			break;
			
		case 2:
			if (pos_x != dst_x) {
				if (pos_x < dst_x) {
					d = RIGHT;
				}
				else if (pos_x > dst_x) {
					d = LEFT;
				}

			}
			else if (pos_y != dst_y){
				if (pos_y < dst_y) {
					d = UP;
				}
				else if (pos_y > dst_y) {
					d = DOWN;
				}

			}
			else {
				if (pos_z < dst_z) {
					d = TSV_UP;
				}
				else if (pos_z > dst_z) {
					d = TSV_DOWN;
				}
				else {
					d = TO_PROCESSOR;
				}

			}

			break;

		case 3:

			if (pos_x != dst_x) {
				if (pos_x < dst_x) {
					avil_d.push_back(RIGHT);
				}
				else if (pos_x > dst_x) {
					avil_d.push_back(LEFT);
				}

			}
			if (pos_y != dst_y){
				if (pos_y < dst_y) {
					avil_d.push_back(UP);
				}
				else if (pos_y > dst_y) {
					avil_d.push_back(DOWN);
				}

			}
			
			if (pos_z < dst_z) {
				avil_d.push_back(TSV_UP);
			}
			else if (pos_z > dst_z) {
				avil_d.push_back(TSV_DOWN);
			}


			d = avil_d[0];
			temp_value = qtable[dst][d - 1] + get_router_loss(j,d,id);
			for (int i = 1; i < avil_d.size(); i++){
				direction = avil_d[i];
				
				if ((qtable[dst][direction - 1] + get_router_loss(j, direction, id)) < temp_value){
					d = direction;
					temp_value = qtable[dst][d - 1] + get_router_loss(j ,d, id);
				}
			}

			break;
		default:
			d = 0;
	}


	if (d<0) {
			assert(0);
	}
	return d;
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
	int					pos_z;
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

	bool				updated_flag[6];
	qpocket				router_qpocket[6];
	double				(*qtable)[4] = new double[NOC_WIDTH*NOC_HEIGHT*NOC_DEPTH][4];

};

//#endif
