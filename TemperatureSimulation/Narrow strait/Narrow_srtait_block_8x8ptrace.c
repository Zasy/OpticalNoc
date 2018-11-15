#include "stdio.h"
#include "string.h" 

float sp(int t)
{
	float power;
	if(t<=1000/4)
		power=0.192;
	else if(t<=9*1000/10)
		power=3.9;
	else
		power=0;
	return power;
}

float p(int t)
{
	float power;
	power=3.9;
	return power;
}


int main()
{
	int i,j;
	int n=1000;
	float k=0;
	FILE *fp;
	fp=fopen("8x8.ptrace","w+");
	
	for(i=1;i<=8;i++)
	{
		for(j=1;j<=8;j++)
		{
			if(i==8&&j==8)
				fprintf(fp,"P_%d_%d",i,j);
			else
				fprintf(fp,"P_%d_%d\t\t",i,j);
		}
	}
	fprintf(fp,"\n"); 			//»»ÐÐ 

	for(i=1;i<=n;i++)
	{
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,k,k,k,k,k,k);  				//P_1_1 to P_1_8 
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,k,k,k,k,k,k);
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,k,k,k,sp(i),sp(i),sp(i));
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",sp(i),p(i),sp(i),k,k,p(i),sp(i),p(i));
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",p(i),p(i),sp(i),k,k,p(i),p(i),p(i));
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",p(i),p(i),sp(i),k,k,p(i),p(i),sp(i));
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,k,k,k,k,k,k);
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f",k,k,k,k,k,k,k,k);  //P_8_1 to P_8_8 
		fprintf(fp,"\n");
	}
	
	fclose(fp);
 } 
