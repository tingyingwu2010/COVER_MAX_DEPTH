#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <float.h>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

#include  "global_variables.h"

#include "global_functions.h"
#include "global_variables.h"
#include "DP.h"


/******************************************/
int main(int argc, char** argv)
/******************************************/
{


	istname=new char[2000];

	if (argc == 5) {

		/*param 1 -- INSTANCE*/
		strcpy(istname, argv[1]);

		/*param 2 -- TIME LIMIT*/
		timeLimit=atof(argv[2]);

		/*param 3 -- ALGO*/
		algo=atoi(argv[3]);

		/*param 4 -- OPTIONS*/
		option=atoi(argv[4]);
	}
	else {

		cout << "param 1 -- INSTANCE\n";
		cout << "param 2 -- TIME LIMIT\n";
		cout << "param 3 -- ALGO\n";
		cout << "param 4 -- OPTIONS\n";

		cout << "ERROR NUMBER STANDARD PARAMETERS" << endl;
		exit(2);
	}

	cout << "\n\nINSTANCE\t\t" <<  istname <<"\n";
	cout << "-> TIME LIMIT\t\t"<< timeLimit <<"\n";
	cout << "-> ALGO\t\t\t"<< algo <<"\n";
	cout << "-> OPTIONS\t\t"<< option <<"\n";


	read_instance_file(istname);
	cout << "-> item_input\t\t"<< item_number <<"\n";
	cout << "-> capacity\t\t"<< capacity <<"\n";


	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	int max_i=0;
	vector < int > weights_sorted;
	for(int j=0; j<item_number; j++)
	{
		weights_sorted.push_back(weights[j]);
	}

	sort(weights_sorted.begin(), weights_sorted.end());

	int dummy_counter=0;
	int dummy_weight=0;
	for(int j=0; j<item_number; j++)
	{

		dummy_weight+=weights_sorted[j];

		if(dummy_weight>capacity)
		{
			break;
		}

		max_i=j+1;
	}


	cout << "max_i\t" << max_i<< endl;
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////


	if(algo==2)
	{

		cout << "\n\nDP NEW ----->>> WITH SOLUTION\n\n";

		double *sol=new double[item_number];
		double val_KP;
		int cap_sol=0;
		double profit_sol=0;
		int item_sol=0;

		//		///////////////////////////////////////////////////////////////////////////////
		//		///////////////////////////////////////////////////////////////////////////////
		double time_start=clock();


//		cout << "VERSIONE TOY...\n\n";
//		//ATTENZIONE A CALCOLARE BENE max_i
//		max_i=item_number;
//		double cap_sep;
//		double constant;
//		for(int j=0; j<item_number; j++)
//		{
//			cap_sep+=weights[j];
//			constant-=profits[j];
//		}
//
//		constant+=1;
//		cap_sep-=capacity;
//		cap_sep-=1;
//		cout << "costant\t" << constant << endl;
//		cout << "cap_sep\t" << cap_sep << endl;
//		val_KP=DP_kp_RATIO_POWER(item_number,cap_sep,profits,weights,sol,max_i,(double)option,constant);

		val_KP=DP_kp_RATIO_POWER(item_number,capacity,profits,weights,sol,max_i,(double)option,1);

		double time_finish=clock();
		double time_DP=(double)(time_finish-time_start)/(double)CLOCKS_PER_SEC;
		///////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////

		cout << "SOL VALUE\t" << val_KP << endl;
		cout << "time_DP\t" << time_DP << endl;

		for(int j=0; j<item_number; j++)
		{
			if(sol[j]>0)
			{
				cout << "item\t" << j << "\t" <<profits[j] << "\t" << weights[j]<< endl;

				item_sol++;
				cap_sol+=weights[j];
				profit_sol+=profits[j];

			}
			//cout << sol[j];
		}
		cout << endl;

		cout << "SOL VALUE\t" << val_KP << endl;
		cout << "time_DP\t" << time_DP << endl;
		cout << "item_sol\t" << item_sol << endl;
		cout << "profit_sol\t" << profit_sol << endl;
		cout << "profit_sol ratio\t" << (profit_sol+1)/pow(item_number-item_sol,1.0/option) << endl;
		cout << "cap_sol\t" << cap_sol << endl;

		if(cap_sol>capacity)
		{
			cout << "HORROR VAQUI -->> SOLLA INFEASIBLE\n";
			exit(-1);
		}

		delete []sol;

		//		/////////////////////////////////////////////////////////////////////////////////////////

		ofstream compact_file;
		compact_file.open("info_DP_SOL.txt", ios::app);
		compact_file << fixed
				<<  istname <<"\t"
				<< timeLimit <<"\t"
				<< algo <<"\t"
				<< option <<"\t"

				<< max_i <<"\t"

				<< item_number <<"\t"
				<< capacity <<"\t"

				<< val_KP <<"\t"
				<< time_DP <<"\t"
				<< item_sol <<"\t"

				<< profit_sol <<"\t"
				<< cap_sol <<"\t"
				<< (profit_sol+1)/pow(item_number-item_sol,1.0/option)<<"\t"

				<< endl;
		compact_file.close();
		//		/////////////////////////////////////////////////////////////////////////////////////////


	}

	if(algo==3)
	{
		cout << "\n\nDP NEW ----->>> LIGHT NEW!\n\n";

		double *sol=new double[item_number];
		double val_KP;
		int cap_sol=-1;
		double profit_sol=-1;
		int item_sol=-1;

		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////
		double time_start=clock();

		//ATTENZIONE A CALCOLARE BENE max_i
//		double cap_sep;
//		double constant;
//		for(int j=0; j<item_number; j++)
//		{
//			cap_sep+=weights[j];
//			constant-=profits[j];
//		}
//		constant+=1;
//		cap_sep-=capacity;
//		cap_sep-=1;
//		cout << "costant\t" << constant << endl;
//		cout << "cap_sep\t" << cap_sep << endl;
//		val_KP=DP_kp_RATIO_POWER_LIGHT(item_number,cap_sep,profits,weights,sol,max_i,(double)option,constant);

		val_KP=DP_kp_RATIO_POWER_LIGHT(item_number,capacity,profits,weights,sol,max_i,(double)option,1);


		double time_finish=clock();
		double time_DP=(double)(time_finish-time_start)/(double)CLOCKS_PER_SEC;
		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////

		cout << fixed << "SOL VALUE\t" << val_KP << endl;
		cout << "time_DP\t" << time_DP << endl;

		delete []sol;

		/////////////////////////////////////////////////////////////////////////////////////////
		ofstream compact_file;
		compact_file.open("info_DP_light_new.txt", ios::app);
		compact_file << fixed
				<<  istname <<"\t"
				<< timeLimit <<"\t"
				<< algo <<"\t"
				<< option <<"\t"

				<< max_i <<"\t"

				<< item_number <<"\t"
				<< capacity <<"\t"

				<< val_KP <<"\t"
				<< time_DP <<"\t"
				<< item_sol <<"\t"

				<< profit_sol <<"\t"
				<< cap_sol <<"\t"
				//				<< (profit_sol-1)/pow(item_sol,1.0/option)<<"\t"
				<< 0<<"\t"

				<< endl;
		compact_file.close();
		/////////////////////////////////////////////////////////////////////////////////////////


	}


	//////////////////
	free_data_prob();
	//////////////////

	delete[] istname;	

	printf("\nDONE!");

	return 1;
}




