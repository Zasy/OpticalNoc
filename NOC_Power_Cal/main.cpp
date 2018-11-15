//
// Created by zhanGGe on 2018/9/26.
//


#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <vector>
#include <queue>
#include <functional>

#include "graph/graph.h"
#include "mr/mr.h"
#include "routing_data.h"

using namespace std;


int getShortestNumberHop(int s, int d, int NOC_WIDTH, int NOC_HEIGHT){
    int s_x = s % NOC_WIDTH;
    int s_y = s / NOC_WIDTH;

    int d_x = d % NOC_WIDTH;
    int d_y = d / NOC_WIDTH;

    int delta_x = max(d_x - s_x, s_x - d_x);
    int delta_y = max(d_y - s_y, s_y - d_y);

    return delta_x + delta_y + 1;
}

int NOC_WIDTH   = 8;
int NOC_HEIGHT  = 8;

vector<int> getXYPath(int s, int d){
    vector<int> res;
    int s_x = s % NOC_WIDTH;
    int s_y = s / NOC_WIDTH;

    int d_x = d % NOC_WIDTH;
    int d_y = d / NOC_WIDTH;
    res.push_back(s);
    while(d_x != s_x){
        if(s_x < d_x){
            s_x++;
            s = s + 1;
            res.push_back(s);
        }else{
            s_x--;
            s = s - 1;
            res.push_back(s);
        }
    }


    while(d_y != s_y){
        if(s_y < d_y){
            s_y++;
            s = s + NOC_WIDTH;
            res.push_back(s);
        }else{
            s_y--;
            s = s - NOC_WIDTH;
            res.push_back(s);
        }
    }
    return res;
}

//vector<double> chip_temp = {
//        62.55,  66.05,  64.54,  64.52,  63.93,  63.88,  63.58,  63.03,
//        63.67,  65.41,  67.26,  67.3,   65.08,  65.01,  63.76,  61.9,
//        62.64,  65.57,  67.69,  67.89,  66.57,  64.23,  62.26,  61,
//        61.97,  64.34,  67.41,  67.4,   67.25,  64.02,  61.62,  60.41,
//        60.65,  63.41,  65.21,  66.96,  66.14,  63.3,   62.4,   60.49,
//        59.84,  61.8,   61.82,  63.59,  62.8,   60.95,  59.84,  59.42,
//        58.78,  59.72,  60.52,  61.57,  63.07,  59.63,  58.81,  58.25,
//        57.79,  58.12,  59.02,  64.63,  62.48,  59.21,  58.19,  57.46,
//};


void readTemperatureFile(string f, vector<double> & chip_temp){
    ifstream temperature_file(f, ios::binary);

    for(int i=0; i < NOC_HEIGHT*NOC_WIDTH; i++){

        string coreid;      // temp core id
        double temperature; // temp temperature of noc
        temperature_file >> coreid >> temperature ;
        chip_temp.push_back(temperature - 273.15);
    }

    for(int i=0; i < NOC_WIDTH; i++){
        for(int j=0; j < NOC_HEIGHT; j++){
            cout << chip_temp[i*NOC_HEIGHT + j] << ",\t";
        }
        cout << "\n";
    }
}

double GetPathLoss(vector<int> routinglist, vector<double> chip_temp){
    int in_d =0;
    int out_d = get_output_direaction(routinglist[0], routinglist[1], NOC_WIDTH, NOC_HEIGHT);
    double temp_loss = 0;

    temp_loss += GetRouterLoss(in_d, out_d, chip_temp[routinglist[0]]);
    for (int i = 1; i < (routinglist.size() - 1); i++)
    {
        in_d = get_input_direction(routinglist[i - 1], routinglist[i], NOC_WIDTH, NOC_HEIGHT);
        out_d = get_output_direaction(routinglist[i], routinglist[i + 1], NOC_WIDTH, NOC_HEIGHT);
        temp_loss += GetRouterLoss(in_d, out_d, chip_temp[routinglist[i]]);
        temp_loss += (link_length * L_propagate);
    }

    in_d = get_input_direction(routinglist[routinglist.size() - 2], routinglist[routinglist.size() - 1], NOC_WIDTH, NOC_HEIGHT);
    out_d = 0;

    temp_loss += GetRouterLoss(in_d, out_d, chip_temp[routinglist[routinglist.size() - 1]]);
    temp_loss += (link_length * L_propagate);
    return temp_loss;
}



double GetNocLoss(vector<double> chip_temp){

    double all_loss = 0;
    int process_no = NOC_HEIGHT * NOC_WIDTH;
    Graph g(process_no);

    for(int i = 0 ; i < NOC_WIDTH; i++){
        for(int j = 0; j < NOC_HEIGHT; j++){
            int src = j*NOC_HEIGHT + i;
            if(i - 1 >=0){
                g.addEdge(src, src - 1);
            }
            if(i + 1 < NOC_WIDTH){
                g.addEdge(src, src + 1);
            }

            if(j - 1 >= 0){
                g.addEdge(src, src - NOC_WIDTH);
            }

            if(j + 1 < NOC_WIDTH){
                g.addEdge(src, src + NOC_WIDTH);
            }
        }
    }
    int count = 0;
    for(int s = 0; s < NOC_HEIGHT*NOC_WIDTH; s++){
        for(int d = 0; d < NOC_WIDTH*NOC_HEIGHT; d++){
            if(s == d) continue;
            count++;
            int hops = getShortestNumberHop(s, d, NOC_WIDTH, NOC_HEIGHT);
            int k = 1;
            vector<vector<int>> paths = g.printAllPaths(s, d, hops);

            auto cmp = [&chip_temp](vector<int> left, vector<int> right){ return GetPathLoss(left, chip_temp) < GetPathLoss(right, chip_temp);};
            priority_queue<vector<int>, vector<vector<int>>, decltype(cmp)> q(cmp);

            priority_queue<double> q_loss;
            for(vector<int> path: paths){
                if(q.size() < k){
                    q.push(path);
                }else{
                    vector<int> temp_path = q.top();
                    if(GetPathLoss(temp_path, chip_temp) > GetPathLoss(path, chip_temp)){
                        q.pop();
                        q.push(path);
                    }
                }
            }
            all_loss += GetPathLoss(q.top(), chip_temp);
        }
    }

    return all_loss / count;
}


double GetXYNocLoss(vector<double> chip_temp){
    double loss = 0;
    int count = 0;

    for(int s = 0; s < NOC_WIDTH*NOC_HEIGHT; s++){
        for(int d = 0; d < NOC_WIDTH*NOC_HEIGHT; d++){
            count++;
            vector<int> p = getXYPath(s, d);

            loss += GetPathLoss(p, chip_temp);
        }
    }

    return loss / count;
}


double GetNocWorstLoss(vector<double> chip_temp){
    double loss = 0;
    int process_no = NOC_HEIGHT * NOC_WIDTH;
    Graph g(process_no);

    for(int i = 0 ; i < NOC_WIDTH; i++){
        for(int j = 0; j < NOC_HEIGHT; j++){
            int src = j*NOC_HEIGHT + i;
            if(i - 1 >=0){
                g.addEdge(src, src - 1);
            }
            if(i + 1 < NOC_WIDTH){
                g.addEdge(src, src + 1);
            }

            if(j - 1 >= 0){
                g.addEdge(src, src - NOC_WIDTH);
            }

            if(j + 1 < NOC_WIDTH){
                g.addEdge(src, src + NOC_WIDTH);
            }
        }
    }
    int count = 0;
    for(int s = 0; s < NOC_HEIGHT*NOC_WIDTH; s++){
        for(int d = 0; d < NOC_WIDTH*NOC_HEIGHT; d++){
            if(s == d) continue;
            count++;
            int hops = getShortestNumberHop(s, d, NOC_WIDTH, NOC_HEIGHT);
            int k = 1;
            vector<vector<int>> paths = g.printAllPaths(s, d, hops);

            auto cmp = [&chip_temp](vector<int> left, vector<int> right){ return GetPathLoss(left, chip_temp) < GetPathLoss(right, chip_temp);};
            priority_queue<vector<int>, vector<vector<int>>, decltype(cmp)> q(cmp);

            priority_queue<double> q_loss;
            for(vector<int> path: paths){
                if(q.size() < k){
                    q.push(path);
                }else{
                    vector<int> temp_path = q.top();
                    if(GetPathLoss(temp_path, chip_temp) > GetPathLoss(path, chip_temp)){
                        q.pop();
                        q.push(path);
                    }
                }
            }
            int temp = GetPathLoss(q.top(), chip_temp);

            loss = (loss > temp) ? loss : temp;
        }
    }

    return loss;
}
double getNocXYWorstLoss(vector<double> chip_temp){
    double loss = 0;

    for(int s = 0; s < NOC_WIDTH*NOC_HEIGHT; s++){
        for(int d = 0; d < NOC_WIDTH*NOC_HEIGHT; d++){
            vector<int> p = getXYPath(s, d);
            double temp = GetPathLoss(p, chip_temp);
            loss = (loss > temp) ? loss : temp;
        }
    }

    return loss;
}
// Driver program
int main()
{
    link_length = CHIP_W / NOC_WIDTH ;
    // Create a graph given in the above diagram
    int process_no = NOC_HEIGHT * NOC_WIDTH;
    vector<double> chip_temp;
    readTemperatureFile("8x8steady.txt", chip_temp);


    cout << "Proposed\t" << "XY" << endl;
    cout << GetNocLoss(chip_temp) << "\t" << GetXYNocLoss(chip_temp);

    //getNocXYWorstLoss(chip_temp);

}