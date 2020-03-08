

#include "DP.h"

//#define MAX_N_RATIO 1001
//#define MAX_C_RATIO 1001
//#define MAX_I_RATIO 51

//bool UU[MAX_C_RATIO][MAX_N_RATIO][MAX_I_RATIO];
//int LOOK_UP[MAX_C_RATIO][MAX_N_RATIO][MAX_I_RATIO];

#define INF_DP 999999999999
#define TOLL_DP 0.000001


/*****************************************************************************/
void Load_SOLUTION(double *sol,int C, int n, int i_star,bool ***UU, vector < int > w)
/*****************************************************************************/
{

	int c=C;
	for (int jjj = n-1; jjj >= 0; jjj--)
	{

		if(i_star==0)
		{
			sol[jjj]=0;
		}
		else
		{
			if(UU[jjj][c][i_star]==1)
			{


				sol[jjj]=1;
				c-=w[jjj];

				if(c<0){cout << "c negative!!!!\n";exit(1);}

				i_star--;
			}
			else
			{
				sol[jjj]=0;
			}
		}
	}
}

/*****************************************************************************/
double DP_kp_RATIO_POWER(int n, int C, double * p, vector < int > w,double *sol,int max_i,double elle,double constant)
/*****************************************************************************/
{

	//cout << "OPERATIONS TO DO\t" << n*C+1*max_i << "\tn:\t"<< n << "\tC:\t"<< C << "\tMAX i:\t" << max_i<< endl << endl;

	// /////////////////////////////////////////////////////////////////////////////////////////
	bool ***UU =new bool**[n];
	for(int j = 0; j < n; j++)
	{
		UU[j]=new bool*[C+1];
		for(int s = 0; s <= C; s++)
		{
			UU[j][s]=new bool[max_i+1];
		}
	}
	double **LOOK_UP=new double*[C+1];
	for(int s = 0; s <= C; s++)
	{
		LOOK_UP[s]=new double[max_i+1];
	}
	///////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////////////////
	//INIT
	for(int i = 0; i <= max_i; i++)
	{
		for(int s = 0; s <= C; s++)
		{

			UU[0][s][i]=0;

			if(i==1)
			{
				if(w[0]<=s)
				{
					LOOK_UP[s][i]=p[0]+constant;

					UU[0][s][i]=1;
				}
				else
				{
					LOOK_UP[s][i]=-INF_DP;
				}
			}
			else
			{
				LOOK_UP[s][i]=-INF_DP;
			}
		}

		//for (int s = 0; s <= C; s++){cout << LOOK_UP[s][i] << "\t";}cout << endl;
	}
	/////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////////
	//for each item
	for (int j = 1; j < n; j++)
	{
		//cout << "ITEM\n" << j << endl;

		//for each value of cardinality
		for(int i = min(max_i,j+1); i >= 1; i--)
		{

			//CARDINALITY 1
			if(i==1)
			{
				//for each value of capacity
				for (int s = C; s >= 0; s--)
				{
					UU[j][s][i]=0;

					if(w[j]<=s)
					{

						/////////////////////////////////////////////////////////////////////////////////////////////////
						if( (p[j] + constant) >  LOOK_UP[s][i] + TOLL_DP)
						{
							LOOK_UP[s][i] = p[j] + constant;

							UU[j][s][i]=1;
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////
					}
				}
			}
			//OTHER CARDINALITIES
			else
			{

				//for each value of capacity
				for (int s = C; s >= 0; s--)
				{
					UU[j][s][i]=0;

					if(w[j]<=s)
					{

						/////////////////////////////////////////////////////////////////////////////////////////////////
						if( (  ( p[j] + LOOK_UP[s-w[j]][i-1])  )  >  ( LOOK_UP[s][i]  ) + TOLL_DP )
						{
							LOOK_UP[s][i]= p[j] + LOOK_UP[s-w[j]][i-1];

							UU[j][s][i]=1;
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////
					}
				}
			}

			//for (int s = 0; s <= C; s++){cout << LOOK_UP[s][i] << "\t";}cout << endl;
		}
	}



	//FIND THE OPTIMAL SOLUTION VALUE
	double  opt_val=-INF_DP;
	int i_star = -1;
	for(int i = 1; i <= max_i; i++)
	{

		//cout  << "i\t" << i << "\t" << LOOK_UP[C][i] << "\t" <<   pow(LOOK_UP[C][i],elle)/(double)(n-i) << "\t"<< pow(pow(LOOK_UP[C][i],elle)/(double)(n-i),1.0/elle) << endl;

		if(LOOK_UP[C][i]<=TOLL_DP){continue;}

		double val=pow(pow(LOOK_UP[C][i],elle)/(double)(n-i),1.0/elle);

		//cout << "val\t" << val << endl;

		if ( val  > opt_val + TOLL_DP)
		{
			opt_val = val;
			i_star = i;
		}
	}

	//cout << "i_star\t" << i_star << endl;

	if(i_star==-1)
	{
		cout << "ALL NEGATIVE....";

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		for(int j = 0; j < n; j++){for(int s = 0; s <= C; s++){delete[]UU[j][s];}delete[]UU[j];}delete[]UU;
		for(int s = 0; s <= C; s++){delete[]LOOK_UP[s];}delete[]LOOK_UP;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		return -1;
	}


	//////////////////////////////////////
	Load_SOLUTION(sol,C,n,i_star,UU, w);
	//////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(int j = 0; j < n; j++){for(int s = 0; s <= C; s++){delete[]UU[j][s];}delete[]UU[j];}delete[]UU;
	for(int s = 0; s <= C; s++){delete[]LOOK_UP[s];}delete[]LOOK_UP;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return opt_val;

}





/*****************************************************************************/
double DP_kp_RATIO_POWER_LIGHT(int n, int C, double * p, vector < int > w,double *sol,int max_i,double elle,double constant)
/*****************************************************************************/
{



	//cout << "OPERATIONS TO DO\t" << n*C+1*max_i << "\tn:\t"<< n << "\tC:\t"<< C << "\tMAX i:\t" << max_i<< endl << endl;

	// /////////////////////////////////////////////////////////////////////////////////////////

	double **LOOK_UP=new double*[C+1];
	for(int s = 0; s <= C; s++)
	{
		LOOK_UP[s]=new double[max_i+1];
	}
	///////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////////////////
	//INIT
	for(int i = 0; i <= max_i; i++)
	{
		for(int s = 0; s <= C; s++)
		{

			if(i==1)
			{
				if(w[0]<=s)
				{
					LOOK_UP[s][i]=p[0]+constant;


				}
				else
				{
					LOOK_UP[s][i]=-INF_DP;
				}
			}
			else
			{
				LOOK_UP[s][i]=-INF_DP;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////////
	//for each item
	for (int j = 1; j < n; j++)
	{
		//cout << "ITEM\n" << j << endl;

		//for each value of cardinality
		for(int i = min(max_i,j+1); i >= 1; i--)
		{

			//CARDINALITY 1
			if(i==1)
			{
				//for each value of capacity
				for (int s = C; s >= 0; s--)
				{


					if(w[j]<=s)
					{

						/////////////////////////////////////////////////////////////////////////////////////////////////
						if( (p[j] + constant) >  LOOK_UP[s][i] + TOLL_DP)
						{
							LOOK_UP[s][i] = p[j] + constant;


						}
						/////////////////////////////////////////////////////////////////////////////////////////////////
					}
				}
			}
			//OTHER CARDINALITIES
			else
			{

				//for each value of capacity
				for (int s = C; s >= 0; s--)
				{

					if(w[j]<=s)
					{

						/////////////////////////////////////////////////////////////////////////////////////////////////
						if( (  ( p[j] + LOOK_UP[s-w[j]][i-1])  )  >  ( LOOK_UP[s][i]  ) + TOLL_DP )
						{
							LOOK_UP[s][i]= p[j] + LOOK_UP[s-w[j]][i-1];


						}
						/////////////////////////////////////////////////////////////////////////////////////////////////
					}
				}
			}
		}

	}



	//FIND THE OPTIMAL SOLUTION VALUE
	double  opt_val=-INF_DP;
	int i_star = -1;
	for(int i = 1; i <= max_i; i++)
	{

		//cout  << "i\t" << i << "\t" << LOOK_UP[C][i] << "\t" <<   pow(LOOK_UP[C][i],elle)/(double)(n-i) << "\t"<< pow(pow(LOOK_UP[C][i],elle)/(double)(n-i),1.0/elle) << endl;

		if(LOOK_UP[C][i]<=TOLL_DP){continue;}

		double val=pow(pow(LOOK_UP[C][i],elle)/(double)(n-i),1.0/elle);

		//cout << "val\t" << val << endl;

		if ( val  > opt_val + TOLL_DP)
		{
			opt_val = val;
			i_star = i;
		}
	}
	if(i_star==-1)
	{
		cout << "ALL NEGATIVE....";

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		for(int s = 0; s <= C; s++){delete[]LOOK_UP[s];}delete[]LOOK_UP;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		return -1;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(int s = 0; s <= C; s++){delete[]LOOK_UP[s];}delete[]LOOK_UP;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return opt_val;

}


