#ifndef E_GraphTask
#define E_GraphTask
#include "GraphEdge.h"

class GraphTask {

public:
	// critical functions for the statistical trace
	int						get_random_execution_time();								// get a random execution time using the parameters for Gaussian distribution
	int						set_statistical_execution_time(double mu, double sigma);	//

	// critical functions for the recorded trace
	int						get_recorded_execution_time();								// get the recorded execution time
	int						set_recorded_execution_time(int t);							// set the recorded execution time
	
	// assistant functions
	int						add_incoming_edge(GraphEdge &e);
	int						add_outgoing_edge(GraphEdge &e);
//	int						print_all_edge();
//	GraphEdge&				get_edge(int eid);											// get edge by id

	// basic functions
	int						get_id();
	int						set_id(int tid);

	int						get_proc_id();
	int						set_proc_id(int pid);

	int						get_schedule();
	int						set_schedule(int s);

	int						get_max_time();
	int						set_max_time(int t);

	double					get_mu();
	double					get_sigma();



	GraphEdge& get_incoming_edge_by_eid(int eid);
	GraphEdge& get_incoming_edge_by_offset(int eid);
	GraphEdge& get_outgoing_edge_by_eid(int eid);
	GraphEdge& get_outgoing_edge_by_offset(int eid);
	int get_size_of_incoming_edge_list();
	int get_size_of_outgoing_edge_list();

	int set_required_times(int k){required_times=k; return 0;}
	int get_required_times(){return required_times;}
	void initial(){ completed_times =0;task_state=0;return;}
	void add_completed_times(){completed_times++;}
	int get_completed_times(){return completed_times;}
	void set_task_state(int i){task_state=i;return; }
	int get_task_state(){return task_state;}
	//double drand() ;
	//double gen_normal_dist(double m, double s) ;

	//GraphTask& operator=(const GraphTask&);

	void record_execution_time(int start, int end){start_time.push_back(start);end_time.push_back(end);}
	int get_start_time(int i){return start_time.at(i);}
	int get_end_time(int i){return end_time.at(i);}

	void record_duration(int i){duration_time_history.push_back(i); return;}
	vector<int>& get_duration_history(){return duration_time_history;}

	void record_schedule_history(int a){schedule_history.push_back(a); return;}
	vector<int>& get_schedule_history(){return schedule_history;}
private:
	// the statistical task executions follow Gaussian distribution
	double					mu_time;							// the mean of the task execution time
	double					sigma_time;							// the sd of the task execution time

	// the recorded task executions
	int						rec_time;							// the recorded execution time of the task

	// basic parameters
	int						id;									// the id of the task
	int						proc_id;							// the id of the PU the task is assigned
	int						schedule;							// the sequence number the task is assigned to execute on the PU
	int						max_time;							// the worst-case execution time of the task

	vector<GraphEdge>		incoming_edge_list;					// each entry is an incoming edge
	vector<GraphEdge>		outgoing_edge_list;					// each entry is an outgoing edge
	int	completed_times;//the times that this task has been finished
	int required_times;//the times required for this this task. In this application, it is always 1.

	int task_state;
	

	vector<int> start_time;
	vector<int> end_time;


	vector<int> duration_time_history;

	vector<int>	schedule_history;
};

/*GraphTask& GraphTask ::  operator=(const GraphTask& arg)
{
	mu_time=arg.mu_time;							// the mean of the task execution time
	sigma_time=arg.sigma_time;							// the sd of the task execution time

	// the recorded task executions
	rec_time=arg.rec_time;							// the recorded execution time of the task

	// basic parameters
	id=arg.id;									// the id of the task
	proc_id=arg.proc_id;							// the id of the PU the task is assigned
	schedule=arg.schedule;							// the sequence number the task is assigned to execute on the PU
	max_time=arg.max_time;							// the worst-case execution time of the task


	incoming_edge_list=arg.incoming_edge_list;					// each entry is an incoming edge
	outgoing_edge_list=arg.outgoing_edge_list;					// each entry is an outgoing edge
	completed_times=arg.completed_times;//the times that this task has been finished
	 required_times=arg.required_times;//the times required for this this task. In this application, it is always 1.

	 start_time=arg.start_time;
	 end_time=arg.end_time;
	task_state=arg.task_state;
	

	return (*this);
}
*/
GraphEdge& GraphTask ::  get_incoming_edge_by_eid(int eid) 
{
	for (unsigned int i=0; i<incoming_edge_list.size(); i++) 
	{
		GraphEdge &e = incoming_edge_list.at(i) ;
		if (e.get_id() == eid)
			return e;
	}
	//return NULL;
}
GraphEdge& GraphTask ::  get_incoming_edge_by_offset(int i) 
{
	GraphEdge &e = incoming_edge_list.at(i) ;
	return e;
}

GraphEdge& GraphTask ::  get_outgoing_edge_by_eid(int eid) 
{
	for (unsigned int i=0; i<outgoing_edge_list.size(); i++) 
	{
		GraphEdge &e = outgoing_edge_list.at(i) ;
		if (e.get_id() == eid)
			return e;
	}
}
GraphEdge& GraphTask ::  get_outgoing_edge_by_offset(int i) 
{
	GraphEdge &e = outgoing_edge_list.at(i) ;
	return e;
}

int  GraphTask ::get_size_of_incoming_edge_list(){ return incoming_edge_list.size();}
int	 GraphTask ::get_size_of_outgoing_edge_list(){ return outgoing_edge_list.size();}


int GraphTask::add_incoming_edge(GraphEdge &e) {

	incoming_edge_list.push_back(e);
	return 0;
}

int GraphTask::add_outgoing_edge(GraphEdge &e) {

	outgoing_edge_list.push_back(e);
	return 0;
}

/*
int GraphTask::print_all_edge() {

	cout << "    In edges: " << endl;
	if (0 == incoming_edge_list.size()) {
		cout << "      No edge" << endl;
	}
	else {
		for (unsigned int i=0; i<incoming_edge_list.size(); i++) {
			GraphEdge &e = incoming_edge_list.at(i);
			cout << "      Edge " << e.get_id() << ": " << e.get_src_task_id() << " " << e.get_dst_task_id() << " " << e.get_src_proc_id() << " " << e.get_dst_proc_id() << " "
				<< e.get_max_token_size() << " " << e.get_recorded_token_size() << " " << e.get_mu() << " " << e.get_sigma() << " " << e.get_lambda() << endl;
		}
	}
	
	cout << "    Out edges: " << endl;
	if (0 == outgoing_edge_list.size()) {
		cout << "      No edge" << endl;
	}
	else {
		for (unsigned int i=0; i<outgoing_edge_list.size(); i++) {
			GraphEdge &e = outgoing_edge_list.at(i);
			cout << "      Edge " << e.get_id() << ": " << e.get_src_task_id() << " " << e.get_dst_task_id() << " " << e.get_src_proc_id() << " " << e.get_dst_proc_id() << " "
				<< e.get_max_token_size() << " " << e.get_recorded_token_size() << " " << e.get_mu() << " " << e.get_sigma() << " " << e.get_lambda() << endl;
		}
	}

	return 0;
}
*/

int	GraphTask::get_random_execution_time() {

	int a=gen_normal_dist(mu_time,sigma_time);
	if(a>max_time)
		a=max_time;
	else if(a<=0)
		a=1;
	return a;
	
}

int	GraphTask::set_statistical_execution_time(double mu, double sigma) {

	assert (mu>0 && sigma>0);
	mu_time = mu;
	sigma_time = sigma;

	return 0;
}

int	GraphTask::get_recorded_execution_time() {
	
	return rec_time;
}

int	GraphTask::set_recorded_execution_time(int t) {

	assert (t > 0);
	rec_time = t;

	return 0;
}

int	GraphTask::get_id() {

	return id;
}

int	GraphTask::set_id(int tid) {

	assert (tid >= 0);
	id = tid;

	return 0;
}

int	GraphTask::get_proc_id() {
	
	return proc_id;
}

int	GraphTask::set_proc_id(int pid) {
	
	assert (pid >= 0);
	proc_id = pid;

	return 0;
}

int	GraphTask::get_schedule() {
	
	return schedule;
}

int	GraphTask::set_schedule(int s) {
	
	assert (s >= 0);
	schedule = s;

	return 0;
}
	
int	GraphTask::get_max_time() {
	
	return max_time;
}

int	GraphTask::set_max_time(int t) {
	
	assert (t > 0);
	max_time = t;

	return 0;
}

double GraphTask::get_mu() {

	return mu_time;
}

double GraphTask::get_sigma() {
	
	return sigma_time;
}

#endif


