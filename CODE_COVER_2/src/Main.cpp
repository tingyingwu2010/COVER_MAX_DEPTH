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

#define TOLL_SEP 0.000001

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

	///////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	data KP_instance;

	KP_instance.istname=(char*) calloc(2000,sizeof(char));
	strcpy(KP_instance.istname, argv[1]);


	KP_instance.item_number=item_number;
	KP_instance.capacity=capacity;


	KP_instance.weights=(double*) calloc(KP_instance.item_number,sizeof(double));
	KP_instance.profits=(double*) calloc(KP_instance.item_number,sizeof(double));
	KP_instance.profit_sep=(double*) calloc(KP_instance.item_number,sizeof(double));
	KP_instance.cover=(double*) calloc(KP_instance.item_number,sizeof(double));

	KP_instance.x=(double*) calloc(KP_instance.item_number,sizeof(double));
	KP_instance.x_int=(int*) calloc(KP_instance.item_number,sizeof(int));

	KP_instance.cap_sep=0;

	for(int i=0; i<KP_instance.item_number; i++)
	{
		KP_instance.profits[i]=profits[i];
		KP_instance.weights[i]=weights[i];
		KP_instance.cap_sep+=KP_instance.weights[i];
	}
	KP_instance.cap_sep-=(capacity+1);

	KP_instance.timeLimit=timeLimit;
	KP_instance.number_of_CPU=1;
	///////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	for(int i=0; i<KP_instance.item_number; i++)
	{
		KP_instance.w_DP.push_back(KP_instance.weights[i]);
		KP_instance.order_DP.push_back(i);
	}
	///////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cout << "\n\n********************SOLVE THE KP PROBLEM\n";

	kp_load_cplex(&KP_instance);

	clock_t time_start_KP=clock();
	double val_KP_MIP=kp_solve_cplex(&KP_instance);
	clock_t time_end_KP=clock();
	double solution_time_KP=(double)(time_end_KP-time_start_KP)/(double)CLOCKS_PER_SEC;

	kp_free_cplex(&KP_instance);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cout << "\n\n********************OPTIMIZE OVER THE COVER CLOSURE\n";

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

		if(dummy_weight>KP_instance.cap_sep)
		{
			break;
		}

		max_i=j+1;
	}


	cout << "max_i\t" << max_i<< endl;
	cout << "cap_sep\t" << KP_instance.cap_sep<< endl;
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////




	kp_load_cplex(&KP_instance);

	kp_load_cplex_cover(&KP_instance);

	kp_load_cplex_cover_bis(&KP_instance);

	int number_of_covers=0;

	double LP_CURRENT=-1;
	double LP_INIT=-1;

	clock_t time_start=clock();

	while(1)
	{

		LP_CURRENT=kp_solve_cplex_LP(&KP_instance);

		if(number_of_covers==0)
		{
			LP_INIT=LP_CURRENT;
		}

		double magic_constant=0;
		for(int i=0; i<KP_instance.item_number; i++)
		{
			KP_instance.profit_sep[i]=1-KP_instance.x[i];
			magic_constant+=KP_instance.x[i];
			//cout << KP_instance.profit_sep[i] << endl;
		}

		double cover_val=-1;


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(algo==1)
		{
			cover_val=kp_solve_cplex_cover(&KP_instance);

			if(cover_val<1-TOLL_SEP)
			{

				add_cover(&KP_instance);

				number_of_covers++;
			}
			else
			{
				break;
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(algo==2)
		{
			cover_val=kp_solve_cplex_cover_bis(&KP_instance);


			if(cover_val<1-TOLL_SEP)
			{

				add_cover(&KP_instance);

				number_of_covers++;
			}
			else
			{
				break;
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(algo==3)
		{

			//			double val_DP=DP_kp_RATIO(KP_instance.item_number,KP_instance.cap_sep,KP_instance.profit_sep,weights,KP_instance.cover,max_i,(double)option,1-KP_instance.item_number+magic_constant);
			//			cout << "\nval_DP\t" << val_DP << endl;

			cout << "\n\nCONSTANT\t" <<(1-KP_instance.item_number+magic_constant) << endl <<endl;

			//ATTENTION THAT IT LOAD THE COMPLEMENT OF THE COVER!
			double val_DP=DP_kp_RATIO_POWER_FOR_COVER(KP_instance.item_number,KP_instance.cap_sep,KP_instance.profit_sep,weights,KP_instance.cover,max_i,(double)option,(1-KP_instance.item_number+magic_constant));

			cout << "\nval_DP\t" << val_DP << endl;

			double dummy=0;
			int card=0;
			for (int i = 0; i < KP_instance.item_number; i++)
			{
				dummy=dummy + KP_instance.cover[i] * (1- KP_instance.x[i]);
				card+=KP_instance.cover[i];
				KP_instance.cover[i]=1-KP_instance.cover[i];
				//cout << KP_instance.cover[i];
			}
			dummy+=(1-KP_instance.item_number+magic_constant);
			//cout << "RICALCOLATA\n" << dummy / pow(KP_instance.item_number-card,1.0/option);


			cover_val=val_DP;
			//cout << "\ncover_val\t" << cover_val << endl;

			if(cover_val>TOLL_SEP)
			{

				add_cover(&KP_instance);

				number_of_covers++;
			}
			else
			{
				break;
			}


		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(algo==4)
		{


			cout << "\n\nCONSTANT\t" <<(1-KP_instance.item_number+magic_constant) << endl <<endl;

			//it adds all violated covers (for the different values of cardinality)
			int ADDED=DP_kp_RATIO_POWER_FOR_COVER_ALL(&KP_instance,KP_instance.item_number,KP_instance.cap_sep,KP_instance.profit_sep,weights,KP_instance.cover,max_i,(double)option,(1-KP_instance.item_number+magic_constant));

			cout << "COVER ADDED\t" << ADDED << endl;


			if(ADDED>0)
			{

				cover_val=1;
				number_of_covers+=ADDED;
			}
			else
			{
				break;
			}


		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(algo==5)
		{


			cover_val=kp_solve_DP_cover_bis(&KP_instance);


			if(cover_val<1-TOLL_SEP)
			{

				add_cover(&KP_instance);

				number_of_covers++;
			}
			else
			{
				break;
			}


		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		if(cover_val==-1){break;}

	}

	clock_t time_end=clock();
	double solution_time=(double)(time_end-time_start)/(double)CLOCKS_PER_SEC;


	kp_free_cplex(&KP_instance);

	kp_free_cplex_cover(&KP_instance);
	kp_free_cplex_cover_bis(&KP_instance);

	cout << "\n\nGENERATED\t" << number_of_covers << "\tCOVERS" <<endl;
	cout << "TIME\t" << solution_time <<endl;
	cout << fixed << "LP_CURRENT\t" << LP_CURRENT << endl;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//	/////////////////////////////////////////////////////////////////////////////
	ofstream compact_file;
	compact_file.open("info_Exensive.txt", ios::app);
	compact_file << fixed
			<< istname << "\t"

			<< algo << "\t"
			<< option << "\t"

			<< KP_instance.item_number << "\t"
			<< KP_instance.capacity << "\t"

			<< LP_CURRENT << "\t"
			<< number_of_covers << "\t"
			<< solution_time << "\t"

			<<  val_KP_MIP << "\t"
			<<  solution_time_KP << "\t"

			<<   LP_INIT << "\t"

			<<    max_i << "\t"
			<< KP_instance.cap_sep << "\t"


			<< endl;
	compact_file.close();
	//	/////////////////////////////////////////////////////////////////////////////


	free(KP_instance.weights);
	free(KP_instance.profits);
	free(KP_instance.profit_sep);
	free(KP_instance.x);
	free(KP_instance.x_int);
	free(KP_instance.cover);
	free(KP_instance.istname);

	//////////////////
	free_data_prob();
	//////////////////

	delete[] istname;	

	printf("\nDONE!");

	return 1;
}


