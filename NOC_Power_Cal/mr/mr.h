//
// Created by zhanGGe on 2018/9/26.
//

#ifndef NOC_POWER_CAL_MR_H
#define NOC_POWER_CAL_MR_H

#endif //NOC_POWER_CAL_MR_H
#include <math.h>
double k_e = 6.59;     // the fraction of optical power couple into
double k_d = 6.59;     // the fraction of optical power couple out
double k_p = 1;        // intinstic power loss per round-trip in the MR

double L_VCSEL_0 = 1550;  // lambda_vcsel_0
double L_MR_0    = 1550;  // lambda_mr_0

double T_0       = 25;    // normal temperture
double row_vcsel   = 0.09;
double alpha       = 2.4;
double belta       = 0.00075;
double T_th        = 40;
double epsilon     = 0.403;
double garma       = 0.00217;
double L_MR_resonance_peak     = 0.5;
double row_MR                  = 0.06;
double fabrication_sigma       = 0.4;
double P_MR_ON                 = 0.02;
double Modulation_speed        = 10;
double S_RX                    = -14.2;
double L_propagate             = 0.17;
double L_crossing              = 0.12;
double link_length;
//double crossing_number         = 10;
double E_serializer            = 0.16;
double E_driver                = 0.1125;
double E_PD                    = 0.0003;
double E_deserializer          = 0.128;
double E_TIA_LA                = 0.3375;
double U_slope                 = 0.0729;
double U_th                    = 1.0135;
double P_thermaltuning         = 3.5;
double Lambda                  = 1550;
double elec_switch_off_on      = 0.4;
double thermal_switch_off_on   = -0.4;
double modulation_0_1          = 0.4;
double lambda_misplace_factor  = 3;
double P_modulator_data_0      = 0;

double CHIP_W = 10.0;
double lambda_bias = 1;
double delta       = 0.31;// delta in 3-db bandwidth
double T_MR_0      = 55;
double T_VCSEL_0   = T_0;



double VCSEL_I_th(double T){
    return (alpha + belta*pow(T - T_th, 2));
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
    double k = number/ divide;
    return 1 - k*(pow(delta, 2)) / (pow((L_VCSEL - L_MR - lambda_bias), 2) + pow(delta, 2));
}

double PowerToDb(double transfer){
    return -10 * log10(transfer);
}

double VCSEL_slope(double T){
    return (epsilon - T*garma);
}


double get_E_MR_on(double N_active){
    return (N_active*P_MR_ON/Modulation_speed);
}

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


double GetRouterLoss(int in, int out, double T_mid){
    int N_active = active_number[in][out];
    int N_passive = passive_number[in][out];
    int N_crossing = crossing_number[in][out];

    double T_SRC = T_0;
    double L_VCSEL  = GetLambdaVCSEL(T_SRC);
    double L_MR     = GetLambdaMr(T_mid);

    double drop_loss = PowerToDb(GetDropPower(L_VCSEL, L_MR));
    double through_loss = PowerToDb(GetThroughPower(L_VCSEL, L_MR));

    return (N_active*drop_loss + N_passive*through_loss + N_crossing*L_crossing);
}

double GetPowerEO(double loss, double T_v, double N_active){
    double I = (VCSEL_I_th(T_v)+pow(10,((S_RX+loss-10*log10(VCSEL_slope(T_v)))/10)));
    return (get_E_MR_on(N_active)+E_serializer+E_driver+pow(Modulation_speed,-1)*(U_slope*I+U_th)*I+E_PD+E_deserializer+E_TIA_LA);
}
