#ifndef E_GraphEdge
#define E_GraphEdge

#include "Definition.h"

class GraphEdge {

public:
	// critical functions for the statistical trace
	int						get_random_token_size();				// get a random token size using the parameters for Gaussian distribution
	int						get_random_pkt_interval();			// get a random packet generation interval using the parameter for Exponential distribution
	//notice: it is not related with pkt size. lambda=num_pkts(event)/task_execution_time

	// critical functions for the recorded trace
	int						get_recorded_token_size();			// get the recorded token size

	// basic functions to operate on the basic parameters
	int						get_id();
	int						set_id(int eid);

	int						get_src_task_id();
	int						set_src_task_id(int tid);
	int						get_dst_task_id();
	int						set_dst_task_id(int tid);

	int						get_src_proc_id();
	int						set_src_proc_id(int pid);
	int						get_dst_proc_id();
	int						set_dst_proc_id(int pid);
	
	int						get_max_token_size();
	int						set_max_token_size(int size);
	



	double get_mu(); 

	double get_sigma();

	double get_lambda();

	
	//set incoming token size
	void					set_token_size(int token_size);
	
	//void					record_incoming_pkt(int num);
	int						get_num_incoming_token(){return num_incoming_token;}
	void					consume_token(){num_incoming_token=num_incoming_token--; return;}
	
	int						set_statistical_token_size(double mu, double sigma);
	int						set_statistical_pkt_interval(double lambda);
	int						set_recorded_token_size(int s);
	
	
	
	void initial();
	
	struct token_info_type{
		int id;
		int length_in_pkt;
		int received_pkt;
	};
	//record the incoming token/pkt
	int record_pkt(flit_type pkt);

	//when the edge is outgoing edge, we keep the ID of current token
	int generate_new_token(){output_token_id++; return output_token_id;}
	int get_current_token_id(){return output_token_id;}
	
	//GraphEdge& operator=(const GraphEdge&);


	void record_token_size(int i){token_size_history.push_back(i);return;}
	vector<int>& get_token_size_history(){return token_size_history;}

	void recode_pkt_generate_time(int i){pkt_generate_time.push_back(i); return;}
	vector<int>& get_pkt_generate_time(){return pkt_generate_time;} 


	void set_out_memory(int a, int b){out_memory_start_address=a; out_memory_size=b;out_memory_remained=out_memory_size;out_memory_write_pointer=0;out_memory_read_pointer=0;return;}
	void record_out_address(int a){token_out_address_history.push_back(a); return;}
	vector<int> get_out_address(){return token_out_address_history;}
	bool update_out_memory_write_pointer(){if(out_memory_remained<=0) return false; record_out_address(out_memory_start_address+out_memory_write_pointer);out_memory_write_pointer=(out_memory_write_pointer+1)%out_memory_size;out_memory_remained--; return true;}
	bool update_out_memory_read_pointer(){if(out_memory_remained>=out_memory_size) return false; out_memory_read_pointer=(out_memory_read_pointer+1)%out_memory_size;out_memory_remained++; return true;}
	int get_out_memory_remained(){return out_memory_remained;}

	
	void set_in_memory(int a, int b){in_memory_start_address=a; in_memory_size=b;in_memory_remained=in_memory_size;in_memory_write_pointer=0;in_memory_read_pointer=0;return;}
	void record_in_address(int a){token_in_address_history.push_back(a); return;}
	vector<int> get_in_address(){return token_in_address_history;}
	bool update_in_memory_write_pointer(){if(in_memory_remained<=0) return false; record_in_address(in_memory_start_address+in_memory_write_pointer);in_memory_write_pointer=(in_memory_write_pointer+1)%in_memory_size;in_memory_remained--; return true;}
	bool update_in_memory_read_pointer(){if(in_memory_remained>=in_memory_size) return false; in_memory_read_pointer=(in_memory_read_pointer+1)%in_memory_size;in_memory_remained++; return true;}
	int get_in_memory_remained(){return in_memory_remained;}

	int get_in_memory_write_pointer(){return in_memory_write_pointer;}
	int get_in_memory_size(){return in_memory_size;}

	bool record_sent_pkt(flit_type pkt);
	/* Memory operation is as below:
	Execute_task: task's in_edge's in_memory_read, if the src_task is mapped to the same core as current task, the src_task's out_edge's out_memory_read is also updated
				  task's out_edge's out_memory_write, if the desk_task is mapped to the same core as current task, the dest_task's in_edge's in_memory_write is also updated
	Send_data_out: if both tasks of the out_edge are on the same core, do nothing; else, src_task's out_edge's out_memory_read
	receive_data: if both tasks of the out_edge are on the same core, do nothing; else, dest_task's in_edge's in_memory_write
	*/


	//GraphEdge() : {	}
	
//		#define A 16807.0
//	#define M 2147483647.0
//	
private:
	// the statistical token sizes follow Gaussian distribution
	double					mu_token_size;						// the mean of the token size
	double					sigma_token_size;					// the sd of the token size

	// the statistical packet generation intervals follow Exponential distribution
	double					lambda_pkt_interval;				// the rate parameter, the inverse of the average packet generation interval

	// the recorded token sizes
	int						rec_token_size;						// the recorded token size on the edge
	
	// basic parameters
	int						id;									// the id of the edge
	int						src_task_id;						// the id of the source task
	int						dst_task_id;						// the id of the destination task
	int						src_proc_id;						// the id of the PU the source task is assigned
	int						dst_proc_id;						// the id of the PU the destination task is assigned
	int						max_token_size;						// the maximum possible token size generated by the execution of the source task

	// the number of token which has come as a whole token
	int						num_incoming_token;					
	
	//the number of partially received tokens
	vector<token_info_type>  received_token_list; 
	//the number of partially sent tokens
	vector<token_info_type> sent_token_list;

	//the output token ID
	int  output_token_id;

	vector<int> token_size_history;
	
	vector<int> pkt_generate_time;

	int						out_memory_start_address; // the start address to write data for outgoing edge
	int						out_memory_size;
	vector<int>				token_out_address_history;
	int						out_memory_write_pointer;
	int						out_memory_read_pointer;
	int						out_memory_remained;

	int						in_memory_start_address; // the start address to write data for outgoing edge
	int						in_memory_size;
	vector<int>				token_in_address_history;
	int						in_memory_write_pointer;
	int						in_memory_read_pointer;
	int						in_memory_remained;

};

/*inline  GraphEdge& GraphEdge :: operator=(const GraphEdge& arg)
{
	mu_token_size=arg.mu_token_size;						// the mean of the token size
	sigma_token_size=arg.sigma_token_size;					// the sd of the token size

	lambda_pkt_interval=arg.lambda_pkt_interval;				// the rate parameter, the inverse of the average packet generation interval

	rec_token_size=arg.rec_token_size;						// the recorded token size on the edge

	id=arg.id;									// the id of the edge
	src_task_id=arg.src_task_id;						// the id of the source task
	dst_task_id=arg.dst_task_id;						// the id of the destination task
	src_proc_id=arg.src_proc_id;						// the id of the PU the source task is assigned
	dst_proc_id=arg.dst_proc_id;						// the id of the PU the destination task is assigned
	max_token_size=arg.max_token_size;						// the maximum possible token size generated by the execution of the source task

	// runtime parameters
	num_incoming_token=arg.num_incoming_token;					// the number of token which has come as a whole token
	received_token_list=arg.received_token_list;			
	

	output_token_id=arg.output_token_id;

	token_size_history=arg.token_size_history;
	
	pkt_generate_time=arg.pkt_generate_time;

	out_memory_start_address=arg.out_memory_start_address; // the start address to write data for outgoing edge
	out_memory_size=arg.out_memory_size;
	token_out_address_history=arg.token_out_address_history;
	out_memory_write_pointer=arg.out_memory_write_pointer;
	out_memory_read_pointer=arg.out_memory_read_pointer;
	out_memory_remained=arg.out_memory_remained;

	in_memory_start_address=arg.in_memory_start_address; // the start address to write data for ingoing edge
	in_memory_size=arg.in_memory_size;
	token_in_address_history=arg.token_in_address_history;
	in_memory_write_pointer=arg.in_memory_write_pointer;
	in_memory_read_pointer=arg.in_memory_read_pointer;
	in_memory_remained=arg.in_memory_remained;
	//these numbers are used for rnd
	return(*this);
}
*/
void GraphEdge ::initial()
{
	output_token_id=0;	
	num_incoming_token=0;
	//generate_seed();
	gen_exp_dis_time(0.1);
	return;
}

int GraphEdge::record_pkt(flit_type pkt)
{
	int i;
	// find token
	for(i=0;i<received_token_list.size();i++)
	{
		if(received_token_list.at(i).id==pkt.token_id)
		{
			break;
		}
	}
	//new token
	if(i>=received_token_list.size())
	{
		//it consumes/resevers memory immediately, 
		//However, it only applys when this is an inter-core pkt. Otherwise, the memory is reserved already --in fact, data is located at the same place
		if(pkt.src!=pkt.dest)
		{
			assert(update_in_memory_write_pointer());
		}
		token_info_type temp;
		temp.id=pkt.token_id;
		temp.length_in_pkt=pkt.token_length_in_pkt;
		temp.received_pkt=1;
		received_token_list.push_back(temp);
	}
	else
	{
		if(received_token_list.at(i).length_in_pkt!=pkt.token_length_in_pkt)
		{
			cout<<"error receiving token"<<endl;
			return 0;
		}
		received_token_list.at(i).received_pkt++;
	}
	//check token state
	if(received_token_list.at(i).received_pkt==received_token_list.at(i).length_in_pkt)
	{
		num_incoming_token++;
		received_token_list.erase(received_token_list.begin()+i);
	}
	else if(received_token_list.at(i).received_pkt>received_token_list.at(i).length_in_pkt)
	{
		cout<<"error receiving token"<<endl;
		return 0;
	}

	return 1;
}
/*void GraphEdge::record_incoming_pkt(int num)
{
		if (num!=1){ cout<< "recode_pkt_error";} 
		if(num_expected_incoming_pkt<1)
		cout<<"error! You should set the num_expected_incoming_pkt first!";

		num_incoming_pkt=num_incoming_pkt+num; 
		if(num_incoming_pkt>=num_expected_incoming_pkt) 
		{
			num_incoming_pkt=num_incoming_pkt-num_expected_incoming_pkt;
			num_incoming_token++;
		}
		return;
}
*/
/*//this function should never return true as long as we set the num_expected_incoming_pkt first before receiving the pkt 
bool GraphEdge::check_token()
{
		if(num_expected_incoming_pkt<1)
		cout<<"error checking token";
		if(num_incoming_pkt>=num_expected_incoming_pkt) 
		{
			num_incoming_pkt=num_incoming_pkt-num_expected_incoming_pkt;
			num_incoming_token++;
			return true;
		}
		return false;
}
*/


bool GraphEdge::record_sent_pkt(flit_type pkt)
{//only for inter-core pkts
	int i;
	// find token
	for(i=0;i<sent_token_list.size();i++)
	{
		if(sent_token_list.at(i).id==pkt.token_id)
		{
			break;
		}
	}
	//new token
	if(i>=sent_token_list.size())
	{
		//new token would reserve new memory, therefore, we should check the buffer state of dest
		if(in_memory_remained<=0)
		{	
			return false;
		}
		else
		{//it consumes/resevers memory immediately, 
			assert(update_in_memory_write_pointer());
			token_info_type temp;
			temp.id=pkt.token_id;
			temp.length_in_pkt=pkt.token_length_in_pkt;
			temp.received_pkt=1;
			sent_token_list.push_back(temp);
		}
		
	}
	else
	{
		if(sent_token_list.at(i).length_in_pkt!=pkt.token_length_in_pkt)
		{
			cout<<"error receiving token"<<endl;
			return false;
		}
		sent_token_list.at(i).received_pkt++;
	}
	//check token state
	if(sent_token_list.at(i).received_pkt==sent_token_list.at(i).length_in_pkt)
	{
		sent_token_list.erase(sent_token_list.begin()+i);
		//at this moment, a buffer size is emptied
		assert(update_out_memory_read_pointer());
	}
	else if(sent_token_list.at(i).received_pkt>sent_token_list.at(i).length_in_pkt)
	{
		cout<<"error receiving token"<<endl;
		return false;
	}

	return true;
}

int	GraphEdge::get_random_token_size() {
	double a=gen_normal_dist(mu_token_size,sigma_token_size);
	if (a>max_token_size)
		a=max_token_size;
	else if(a<=0)
		a=1;
	return a;
}



int	GraphEdge::set_statistical_token_size(double mu, double sigma) {

	assert (mu>0 && sigma>0);
	mu_token_size = mu;
	sigma_token_size = sigma;

	return 0;
}






int	GraphEdge::get_random_pkt_interval() {
		
	double a=gen_exp_dis_time(lambda_pkt_interval);
	return a;
}

int	GraphEdge::set_statistical_pkt_interval(double lambda) {

	assert (lambda > 0);
	lambda_pkt_interval = lambda;

	return 0;
}

int	GraphEdge::get_recorded_token_size() {
	
	return rec_token_size;
}

int	GraphEdge::set_recorded_token_size(int s) {
	
	assert (s > 0);
	rec_token_size = s;

	return 0;
}


int	GraphEdge::get_id() {

	return id;
}

int	GraphEdge::set_id(int eid) {

	assert (eid >= 0);
	id = eid;

	return 0;
}

int	GraphEdge::get_src_task_id() {
	
	return src_task_id;
}

int	GraphEdge::set_src_task_id(int tid) {
	
	assert (tid >= 0);
	src_task_id = tid;

	return 0;
}

int	GraphEdge::get_dst_task_id() {
	
	return dst_task_id;
}

int	GraphEdge::set_dst_task_id(int tid) {
	
	assert (tid >= 0);
	dst_task_id = tid;

	return 0;
}

int	GraphEdge::get_src_proc_id() {
	
	return src_proc_id;
}

int	GraphEdge::set_src_proc_id(int pid) {
	
	assert (pid >= 0);
	src_proc_id = pid;

	return 0;
}

int	GraphEdge::get_dst_proc_id() {
	
	return dst_proc_id;
}

int	GraphEdge::set_dst_proc_id(int pid) {
	
	assert (pid >= 0);
	dst_proc_id = pid;

	return 0;
}
	
int	GraphEdge::get_max_token_size() {
	
	return max_token_size;
}

int	GraphEdge::set_max_token_size(int size) {
	
	assert (size > 0);
	max_token_size = size;

	return 0;
}

double GraphEdge::get_mu() {

	return mu_token_size;
}

double GraphEdge::get_sigma() {
	
	return sigma_token_size;
}

double GraphEdge::get_lambda() {

	return lambda_pkt_interval;
}




#endif