#include <vector>
#include <math.h> 
using namespace std;


int get_input_direction(int src, int dst, int NOC_WIDTH, int NOC_HEIGHT){
	int NON_X = -1;
	int NON_Y =  (0 - NOC_WIDTH);
	int NON_Z = (0 - NOC_WIDTH*NOC_HEIGHT);

	if ((dst - src) == 1){
		return 1;
	}
	else if((dst -src) == -1){
		return 2;
	}
	else if((dst - src) == NOC_WIDTH){
		return 4;
	}
	else if((dst - src) == NON_Y){
		return 3;
	}
	else if((dst - src) == NOC_WIDTH*NOC_HEIGHT){
		return 6;
	}
	else if((dst - src) == NON_Z){
		return 5;
	}

	return 0;
}

int get_output_direaction(int src, int dst, int NOC_WIDTH, int NOC_HEIGHT){

	int NON_X = -1;
	int NON_Y =  (0 - NOC_WIDTH);
	int NON_Z = (0 - NOC_WIDTH*NOC_HEIGHT);

	if ((dst - src) == 1){
		return 2;
	}
	else if ((dst - src) == -1){
		return 1;
	}
	else if ((dst - src) == NOC_WIDTH){
		return 3;
	}
	else if ((dst - src) == NON_Y){
		return 4;
	}
	else if ((dst - src) == NOC_WIDTH*NOC_HEIGHT){
		return 5;
	}
	else if((dst - src) == NON_Z){
		return 6;
	}
	return 0;
}
