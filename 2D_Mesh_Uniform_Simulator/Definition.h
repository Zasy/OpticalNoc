#ifndef electronic_torus_definition
#define electronic_torus_definition

#include "systemc.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include "time.h"
#include "stdlib.h"

int routing_algo = 0;
int total_pkt = 0;
bool DEBUG= false;
bool if_trace_data = false;

const bool MY_PRINT=false;
const bool MY_DEBUG=false;

int traffic_type;

double point_temp = 55;

using namespace std;
const int MAX_LOOP_NUMBER_ALLOWED=100;
const double PI=std::atan(1.0)*4;
#define PRINT 0
// mesh-based topology
int NOC_WIDTH = 8;
int NOC_HEIGHT = 8;
int NOC_DEPTH = 2;
int processor_no=NOC_WIDTH*NOC_HEIGHT*NOC_DEPTH;
int core_no=processor_no;
int noc_update_timer;



#define PROCESSOR_NUM       1
#define CLUSTER_PORTS       7
#define inputbuffersize     3
#define ROUTING_ALG			XYZ_ROUTING

#define unfolded_long_link_delay     1 //here we assume mesh
#define unfolded_normal_link_delay   1
#define folded_normal_link_delay     2
#define folded_short_link_delay      1


#define X_change_road_threshold NOC_WIDTH/2+1
#define Y_change_road_threshold NOC_HEIGHT/2+1
#define Z_change_road_threshold NOC_DEPTH/2+1
const int port_no=7;//torus or mesh
// definition of ports in a router
#define LEFT				1
#define RIGHT				2
#define UP					3
#define DOWN				4
#define TSV_UP				5
#define TSV_DOWN			6
//add for delay module
#define left_right			1
#define right_left			2
#define down_up				3
#define up_down				4
#define TSV_down_up			5
#define TSV_up_down			6
//if whether_unfolded == true or false
#define whether_unfolded	1

#define TO_PROCESSOR		0
#define CLOCK_CYCLE			1
const int flit_size=32;
const int max_link_delay=100;
const int infinite=3000;//should not be too large!!!!, else exception will be caught

const int vc_number=1;
const int vc_depth=16; //for 16x16 unfolded torus, the long link delay is 15 cycles, this need to send buffer full when left space <=29

const int routing_algorithm_delay=1;

int traffic_model=0;
int execution_iterations=10;
const double XbarPower=0.07;
const double BufferPower=0.0015;
const double Dynamic_power=0.0015;
const double RouterControlPower=1.8;
const double LocalConnectPower=0.04;
const double GlobalConnectPower=0.62;
const double BusPower_unit=3.6/2;//10
int		scaling_factor;		// scale task execution time by multiplying by the factor
int start_record_iteration;
bool START_RECORD=false;
bool END_RECORD=false;
int end_record_iteration;
char *model_file_name;

int const_pkt_length=128;
//enum processor_core_type{core_idle,core_waiting_ };


double lambda_MR_0_new = 1550;
double T_SRC = 25;
double T_0 = 25;	//room temperature
double lambda_VCSEL_0 = 1550;	//VCSEL wavelength at room temperature
double row_VCSEL = 0.09; //the temeprature-dependent wavelength shift of VCSEL, in nm per degree
double row_vcsel = 0.09;
double T_th = 40; // ????
double alpha = 2.4;
double belta = 0.00075;
double epsilon = 0.403;
double garma = 0.00217;
double L_MR_resonance_peak = 0.5;
double L_MR_passive = 0.005;
double row_MR = 0.06; //the temeprature-dependent wavelength shift of MR, in nm per degree
double fabrication_sigma = 0.4;
double P_MR_on = 0.02;
double Modulation_speed = 10;
double S_RX = -14.2;
double L_propagate = 0.17;
double L_crossing = 0.12;
double link_length;
//double crossing_number = 10;
double E_serializer = 0.16;
double E_driver = 0.1125;
double E_PD = 0.0003;
double E_deserializer = 0.128;
double E_TIA_LA = 0.3375;
double U_slope = 0.0729;
double U_th = 1.0135;
double P_thermaltuning = 3.5;
double lambda = 1550;
double elec_switch_off_on = 0.4;
double thermal_switch_off_on = -0.4;
double modulation_0_1 = 0.4;
double lambda_misplace_factor = -0.4;
double P_modulator_data_0 = 0.4;

double CHIP_W = 10.0;
double TEMP_MAX = 85;
double TEMP_MIN = 55;
// vector<double> chip_temp;
// vector< vector<double> > chip_app_temp =

double lambda_bias = 0.4;
double L_VCSEL_0 = 1550;  // lambda_vcsel_0
double L_MR_0    = 1550;  // lambda_mr_0
double T_MR_0      = 55;
double T_VCSEL_0   = 25;

//new parameters
double k_e = 6.59;     // the fraction of optical power couple into
double k_d = 6.59;     // the fraction of optical power couple out
double k_p = 1;        // intinstic power loss per round-trip in the MR

// 温度分布文件 当尺寸大小发生变更时，需要随时的变化
double chip_temp[64];

double delta = 0.31;

int active_number[5][5] = {
		{ 0, 1, 1, 1, 1 },
		{ 1, 0, 0, 1, 1 },
		{ 1, 0, 0, 1, 1 },
		{ 1, 1, 1, 0, 0 },
		{ 1, 1, 1, 0, 0 } };

int passive_number[5][5] = {
		{ 0, 3, 0, 2, 2 },
		{ 2, 0, 6, 1, 4 },
		{ 3, 6, 0, 4, 0 },
		{ 0, 2, 4, 0, 6 },
		{ 3, 4, 2, 6, 0 } };

int crossing_number[5][5] = {
		{ 0, 4, 0, 2, 2 },
		{ 4, 0, 6, 0, 6 },
		{ 4, 4, 0, 4, 0 },
		{ 2, 3, 4, 0, 6 },
		{ 4, 2, 3, 4, 0 } };


double VCSEL_I_th (double T){
	return (alpha+belta*pow(T - T_th, 2));
}

double lambda_MR_new(double T){
	return (lambda_MR_0_new + row_MR*(T - T_0));
}

double lambda_VCSEL_new(double T) {
	return (lambda_VCSEL_0 + row_VCSEL*(T - T_0));//on-chip VCSEL, the wavelength at temperature T, increases linearly with temperature
}

double Delta_lambda_new(double T_src, double T_mid){
	return (lambda_VCSEL_new(T_src) - lambda_MR_new(T_mid));
}

double L_SW_active(double T_src, double T_mid){
	return (10 * log10(1 + pow(Delta_lambda_new(T_src, T_mid) / delta, 2)));
}

double L_SW_parking(){
	return 0.0007;
}

double VCSEL_slope (double T){
	return (epsilon - T*garma);
}

double get_E_MR_on(double N_active){
	return (N_active*P_MR_on/Modulation_speed);
}

double GetLambdaMr(double T){
	return (L_MR_0 + row_MR*(T - T_MR_0));
}

double GetLambdaVCSEL(double T){
	return (L_VCSEL_0 + row_vcsel*(T - T_VCSEL_0));
}

double GetDropPower(double L_VCSEL, double L_MR){
	double number = 2 * k_e * k_d;
	double divide = pow(k_e, 2) + pow(k_d, 2) + pow(k_p, 2);
	double k = pow(number/divide, 2);

	return k*pow(delta, 2) / (pow((L_VCSEL - L_MR), 2) + pow(delta, 2));
}

double GetThroughPower(double L_VCSEL, double L_MR){
	double number = 4*pow(k_e, 2)*(pow(k_d, 2) + pow(k_p, 2));
	double divide = pow(pow(k_e, 2) + pow(k_d, 2) + pow(k_p, 2), 2);
	double k = number/ divide  ;
	return 1 - k*(pow(delta, 2)) / (pow((L_VCSEL - L_MR - 1), 2) + pow(delta, 2));
}

double PowerToDb(double transfer){
	return -10 * log10(transfer);
}

double get_router_loss(int in, int out, int id){
	int N_active = active_number[in][out];
	int N_passive = passive_number[in][out];
	int N_crossing = crossing_number[in][out];

	double T_SRC = T_0;
	double L_VCSEL  = GetLambdaVCSEL(T_SRC);
	double L_MR     = GetLambdaMr(chip_temp[id]);

	double drop_loss = PowerToDb(GetDropPower(L_VCSEL, L_MR));
	double through_loss = PowerToDb(GetThroughPower(L_VCSEL, L_MR));

	return (N_active*drop_loss + N_passive*through_loss + N_crossing*L_crossing);
}

double get_Power_EO(double loss, double T_v, int N_active){
	double I = (VCSEL_I_th(T_v)+pow(10,((S_RX+loss-10*log10(VCSEL_slope(T_v)))/10)));

	return (get_E_MR_on(N_active)+E_serializer+E_driver+pow(Modulation_speed,-1)*(U_slope*I+U_th)*I+E_PD+E_deserializer+E_TIA_LA);
	
}

struct routing_data {

	int id;

	int receive_time;
	int leave_time;

	routing_data& operator=(const routing_data&);
	bool operator==(const routing_data&) const;
};

inline routing_data& routing_data::operator=(const routing_data &arg)
{
	id = arg.id;
	receive_time = arg.receive_time;
	leave_time = arg.leave_time;

	return (*this);
}

inline bool routing_data::operator ==(const routing_data &arg) const{
	return (id == arg.id) && (receive_time == arg.receive_time) && (leave_time == arg.leave_time);
} 

struct loss_pkg {
	int src;
	int dst;
	int count;
	int iter;
	double loss_val;
	double power_val;
};

vector<loss_pkg> pkt_data;

struct flit_type
{
       int src; //the src and dest of cores id of each packet: information for routing
       int dest;

	   int src_task; //the src and dest task id of each packet : information for application flow
	   int dest_task;

	   bool control_type;// control pkt type notice
	   int consumed_token;//feedback information, telling the sender that one token is consumed 
	   int available_buffer_size;//feedback information, telling the sender how many buffers are aviable now

	   int edge_id;//the edge in the appilcation 

	   int src_chip;
	   int dest_chip;
	   int token_id; // this id is for token on a specific edge
	   int packet_length; //in flits
	   int token_length_in_pkt;// one token may be packeted in many packets
	   int token_length_in_flit;
	   int rank_in_token;//the offset of packets in the same token
	   int packet_sequence;
	   int generate_time;
	   int direction;
	   double power;

	   double switching_capacity_consumed;
	   int encountered_router_number;

	   int vc_selection;//add for torus deadlock break

	   int waiting_time;
	   int from_node_level;
	   int from_node_rank;

	   bool head;
	   bool tail;
	   int routing_delay;

       flit_type& operator=(const flit_type&);
	   bool operator==(const flit_type&) const;
	   
	   vector<routing_data> routing_list;
	   int receive_time;
	   int send_times;

	   double optical_loss;
	   double power_all;
	
	   int N_active;
};




inline flit_type& flit_type::operator =(const flit_type &arg)
{
       src=arg.src;
       dest=arg.dest;

	   src_task=arg.src_task;
	   dest_task=arg.dest_task;

	   control_type=arg.control_type;
	   consumed_token=arg.consumed_token;
	   available_buffer_size=arg.available_buffer_size;

	   edge_id=arg.edge_id;
	   src_chip=arg.src_chip;
	   dest_chip=arg.dest_chip;
	   token_id=arg.token_id;
	   token_length_in_pkt=arg.token_length_in_pkt;
	   token_length_in_flit=arg.token_length_in_flit; 
	   rank_in_token=arg.rank_in_token;  
	 
	   packet_length=arg.packet_length;
	   packet_sequence=arg.packet_sequence;
	   generate_time=arg.generate_time;
	   head=arg.head;
	   tail=arg.tail;
	   direction=arg.direction;
	   power=arg.power;
	   routing_delay=arg.routing_delay;
	   switching_capacity_consumed=arg.switching_capacity_consumed;
	   encountered_router_number=arg.encountered_router_number;

	   vc_selection=arg.vc_selection;//add for torus vc selection

	   waiting_time=arg.waiting_time;
	   from_node_level=arg.from_node_level;
	   from_node_rank=arg.from_node_rank;

	   routing_list = arg.routing_list;
	   receive_time = arg.receive_time;
	   send_times = arg.send_times;	

	   optical_loss = arg.optical_loss;
	   power_all = arg.power_all;
	   N_active = arg.N_active;
	   	  
       return (*this);
}



inline bool flit_type::operator ==(const flit_type &arg) const
{
	return (src==arg.src)&&(dest==arg.dest)&&(src_task==arg.src_task) && (dest_task==arg.dest_task) 
		&&(src_chip==arg.src_chip)&&(dest_chip==arg.dest_chip)&&
		(packet_length==arg.packet_length)&&(edge_id == arg.edge_id) &&
		(packet_sequence==arg.packet_sequence)&&(generate_time==arg.generate_time)&&
		(head==arg.head)&&(tail==arg.tail)&&(direction==arg.direction)&&(power==arg.power)&&
		(switching_capacity_consumed==arg.switching_capacity_consumed)&&
		(encountered_router_number==arg.encountered_router_number)&&(vc_selection==arg.vc_selection)&& (token_length_in_pkt==arg.token_length_in_pkt)&&
		(token_length_in_flit==arg.token_length_in_flit)&& 
	   (rank_in_token==arg.rank_in_token)&&(token_id==arg.token_id)&&(control_type==arg.control_type) &&
	   (consumed_token==arg.consumed_token)&&
	   (available_buffer_size==arg.available_buffer_size)&&
		(routing_list == arg.routing_list)&&
		(receive_time == arg.receive_time)&&
		(send_times == arg.send_times)&&
		(optical_loss == arg.optical_loss)&&
		(power_all == arg.power_all)&&
		(N_active == arg.N_active);
}




inline ostream&
operator<<(ostream& os, const flit_type& arg)
{
	os<<"src="<<arg.src<<" dest="<<arg.dest<<endl;
       return os;
}

extern  void sc_trace(sc_trace_file *tf, const flit_type& arg, const  std::string& name) 
{
       sc_trace(tf,arg.src,"src");//,name ".src");
       sc_trace(tf,arg.dest,"dest");//,name + ".dest");
	   sc_trace(tf,arg.packet_sequence,"seq");
}
struct generator_buffer_type{
	int						time_to_generate_pkt;
	flit_type				pkt_to_generate;						
};

struct output_buffer_type{
	flit_type				pkt_to_send;
};


struct qpocket{
	double loss;
	int trans_id;
	int trans_port;
	int dst_id;

	flit_type trans_flit;
};


struct Request_buffer
	{
		int src;
		int dest;
		bool wait_req_flag;
		bool wait_tear_flag;
	};

struct router_forward_interface
{
	bool ready;
	int vc_id;
	flit_type data_flit;
	router_forward_interface& operator=(const router_forward_interface&);
	bool operator==(const router_forward_interface&) const;
};

inline router_forward_interface& router_forward_interface::operator =(const router_forward_interface &arg)
{
       ready=arg.ready;
	   vc_id=arg.vc_id;
	   data_flit=arg.data_flit;

       return (*this);
}

inline bool router_forward_interface::operator ==(const router_forward_interface &arg) const
{
	return (ready==arg.ready) && (vc_id==arg.vc_id) && (data_flit==arg.data_flit);
}


inline ostream&
operator<<(ostream& os, const router_forward_interface& arg)
{
	os<<"ready= "<<arg.ready<<", Vc_id="<<arg.vc_id<<",data:"<<arg.data_flit<<endl;
       return os;
}
extern  void sc_trace(sc_trace_file *tf, const router_forward_interface& arg, const  std::string& name) 
{
	sc_trace(tf,arg.vc_id,"src");

}

struct router_backward_interface
{
	bool buffer_full[vc_number];
	int available_vc;
	int vc_available[vc_number];//add for torus deadlock break
	
	router_backward_interface& operator=(const router_backward_interface&);
	bool operator==(const router_backward_interface&) const;
};

inline router_backward_interface& router_backward_interface::operator =(const router_backward_interface &arg)
{
	for(int i=0;i<vc_number;i++){
		   buffer_full[i]=arg.buffer_full[i];
		   vc_available[i]=arg.vc_available[i];
	}
	   available_vc=arg.available_vc;
       return (*this);
}

inline bool router_backward_interface::operator ==(const router_backward_interface &arg) const
{

	for(int i=0;i<vc_number;i++)
	{
		if(buffer_full[i]!=arg.buffer_full[i] || vc_available[i]!=arg.vc_available[i] )
			return false;
	}
	return (available_vc==arg.available_vc);
}



inline ostream&
operator<<(ostream& os, const router_backward_interface& arg)
{
	os<<"available_vc="<<arg.available_vc<<endl;
       return os;
}
extern  void sc_trace(sc_trace_file *tf, const router_backward_interface& arg, const  std::string& name) 
{
	sc_trace(tf,arg.available_vc,"src");

}

class interchip_control_packet_type
{
public:

	bool request;
	bool tear_down;
	bool grant;
	bool fail;
	int src_chip;
	int dest_chip;
	int bus_id;
	

	interchip_control_packet_type& operator = (const interchip_control_packet_type& arg)
	{
		request=arg.request;
		tear_down=arg.tear_down;
		grant=arg.grant;
		fail=arg.fail;
		src_chip=arg.src_chip;
		dest_chip=arg.dest_chip;
		bus_id=arg.bus_id;
		return (*this);
	}
	 bool operator==(const interchip_control_packet_type& arg) const
	 {
		 return ((request==arg.request)&&(tear_down==arg.tear_down)&&(grant==arg.grant)&&(fail==arg.fail)&&(src_chip==arg.src_chip)&&(dest_chip==arg.dest_chip)&&(bus_id==arg.bus_id));
	 }
};
inline ostream& 
operator<<(ostream& os, const interchip_control_packet_type& arg)
{
	os<<"req="<<arg.request<<endl;
       return os;
}

extern  void sc_trace(sc_trace_file *tf, const interchip_control_packet_type& arg, const  std::string& name) 
{
       sc_trace(tf,arg.request,"req");//,name ".src");
	   sc_trace(tf,arg.grant,"grant");//,name + ".dest");
	   sc_trace(tf,arg.fail,"fail");
}

//Generate_real_random_number_with U(0,1)
static double gen_uniform(){
	static bool gen_uniform_initial=false;
	if(!gen_uniform_initial)
	{
		srand((unsigned)time(NULL));
		gen_uniform_initial=true;
	}
	double temp=0;
	while(temp==0)
	{
		double tp= rand();
		if(tp<0 || tp>RAND_MAX)
		{
			cout<<"error generating rand()"<<endl;
		}
		temp= tp / ((double)RAND_MAX+1) ; //Notice you should not use RAND_MAX+1: overflow may happen
	}
	return temp;
}


// normal random variate generator
// mean m, standard deviation s
//Box-Muller Method to generate Gaussian
//http://en.wikipedia.org/wiki/Normal_distribution#Generating_values_from_normal_distribution
static double gen_normal_dist(double m, double s)
{
	double U=gen_uniform();
	double V=gen_uniform();
	double X=sqrt(-2*log(U))*cos(2*PI*V);
	return m+s*X;
}


bool addData(flit_type temp_flit){
	int length = pkt_data.size();

	bool result = false;
	for(int i=0; i < length; i++){
		if(temp_flit.src == pkt_data[i].src && temp_flit.dest == pkt_data[i].dst){
			if (temp_flit.optical_loss < pkt_data[i].loss_val){
				pkt_data[i].loss_val = temp_flit.optical_loss;
				pkt_data[i].power_val = temp_flit.power_all;
			}
			pkt_data[i].count += 1;
			return true;			
		}
	}
	loss_pkg temp_pkt;
	temp_pkt.src = temp_flit.src;
	temp_pkt.dst = temp_flit.dest;
	temp_pkt.count = 1;
	temp_pkt.loss_val = temp_flit.optical_loss;
	temp_pkt.power_val = temp_flit.power_all;

	pkt_data.push_back(temp_pkt);
	return false;
}

double getAverageLoss(){
	double total_loss;
	for(int i=0; i < pkt_data.size(); i++){
		total_loss += (pkt_data[i].count * pkt_data[i].loss_val);
		//total_power += (pkt_data[i].count * pkt_data[i].power_val)
	}

	return total_loss / total_pkt;
}

double getAveragePower(){
	double total_power;
	for(int i=0; i < pkt_data.size(); i++){
		total_power += (pkt_data[i].count * pkt_data[i].power_val);
	}

	return total_power / total_pkt;
}


double getWorstLoss(){
	double worst_loss = 0;
	for(int i=0; i < pkt_data.size(); i++){
		if(pkt_data[i].loss_val > worst_loss){
			worst_loss = pkt_data[i].loss_val;
		}
	}
	return worst_loss;
}

double getWorstPower(){
	double worst_power = 0;
	for(int i=0; i < pkt_data.size(); i++){
		if(pkt_data[i].power_val > worst_power){
			worst_power = pkt_data[i].power_val;
		}
	}

	return worst_power;
}

// 读取温度的分布文件
void readTemperatureFile(string f, vector<double> & chip_temp){

}

#endif

