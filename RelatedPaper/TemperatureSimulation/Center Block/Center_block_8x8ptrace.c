#include "stdio.h"
#include "string.h" 


float p(int t)
{
	float power;
	power=3.9;
	return power;
}


int main()
{
	int i,j;
	float k=0;
	FILE *fp;          			//�½��ļ�ָ�����fp 
	fp=fopen("8x8.ptrace","w+");	//���ļ� 8x8.txt
	
	for(i=1;i<=8;i++)  			//����ڵ����ƣ��� P_1_1 
	{
		for(j=1;j<=8;j++)
		{
			if(i==8&&j==8)
				fprintf(fp,"P_%d_%d",i,j);
			else
				fprintf(fp,"P_%d_%d\t",i,j);
		}
	}
	fprintf(fp,"\n"); 			//���� 

	
	for(i=1;i<=1000;i++)  			//��������ڵ�Ĺ��� 
	{
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,k,k,k,k,k,k);  				//P_1_1 to P_1_8 
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,k,k,k,k,k,k);
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,p(i),p(i),p(i),p(i),k,k);
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,p(i),p(i),p(i),p(i),k,k);
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,p(i),p(i),p(i),p(i),k,k);
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,p(i),p(i),p(i),p(i),k,k);
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",k,k,k,k,k,k,k,k);
		fprintf(fp,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f",k,k,k,k,k,k,k,k);  //P_8_1 to P_8_8 
		fprintf(fp,"\n");
	}
	
	fclose(fp);      //�ر��ļ�
	return 1;
}
