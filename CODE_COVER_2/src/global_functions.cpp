

#include "global_functions.h"

//#define print_ist_features

/*********************************/
int identify_instance_category(char * istname)
/*********************************/
{

	for(int i=0; i<10000; i++)
	{
		if(istname[i]=='.'){
			return atoi(&istname[i-1]);
		}
	}

	cout << "ERROR IST NAME\n\n";
	exit(2);
}






/*********************************/
void read_instance_file(char *probname)
/*********************************/
{


	int j;

	char *istname=new char[100];
	strcpy(istname,probname);
	strcat(istname,"\0");

	ifstream in(istname);
	if(!in)
	{
		ofstream err("Error.log",ios::app);
		cout << "File could not be opened. " << endl;
		err << "File could not be opened. " << endl;
		err.close();
		exit(1);
	}

	//cout << "INSTANCE->\t" << istname << endl;

	in >> item_number;
	in >> capacity;

	cout << "\n-->item_number\t"<<item_number<<endl;
	cout << "\n-->capacity\t"<<capacity<<endl;


	//weights=new int[item_number];
	profits=new int[item_number];


	int dummy=0;
	long total_weight=0;
	for(j=0; j<item_number; j++)
	{

		in >> profits[j];

		int dummy;
		in >> dummy;

		weights.push_back(dummy);

		total_weight+=weights[j];
		//cout<< profits[j]<<" "<<weights[j]<<" "<<weights[j]<<" "<<total_weight<<endl;

	}

	//cerr<<capacity<<endl;



	for(j=0; j<item_number; j++)
	{
		if(weights[j]>=capacity){weights[j]=capacity;}
	}


	cout << "\n\nFeatures:\n";
	cout << "Items\t\t" << item_number << "\n";
	cout << "Capacity\t" << capacity << "\n";



#ifdef print_ist_features

	cout << "Weights\t\t";
	for(j=0; j<item_number; j++)
	{
		cout << weights[j] << "\t";
	}
	cout << "\n";
	cout << "Profits\t\t";
	for(j=0; j<item_number; j++)
	{
		cout << profits[j]<< "\t";
	}
	cout << "\n";


#endif

	in.close();


	delete [] istname;

	cout << "Instance read!\n";
	///////////////////////////////////////////////////////////

}


/*********************************/
void free_data_prob()
/*********************************/
{

	//delete[]weights;
	delete[]profits;
}


/***********************************************************************************/
void kp_load_cplex(data *KP_instance)
/***********************************************************************************/
{

	int i,j;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * setting the CPLEX environment

	//opening the environment
	KP_instance->env=CPXopenCPLEX(&(KP_instance->status));
	if(KP_instance->status!=0)
	{
		printf("cannot open CPLEX environment\n");
		exit(-1);
	}

	//opening the pointer to the problem
	KP_instance->lp=CPXcreateprob(KP_instance->env,&(KP_instance->status),"KP");
	if(KP_instance->status!=0)
	{
		printf("cannot create problem\n");
		exit(-1);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * creating the variables *
	KP_instance->ccnt=KP_instance->item_number;
	KP_instance->obj=(double*) calloc(KP_instance->ccnt,sizeof(double));
	KP_instance->lb=(double*) calloc(KP_instance->ccnt,sizeof(double));
	KP_instance->ub=(double*) calloc(KP_instance->ccnt,sizeof(double));
	KP_instance->xctype=(char*) calloc(KP_instance->ccnt,sizeof(char));


	KP_instance->colname=(char**) calloc(KP_instance->ccnt,sizeof(char*));
	for(i=0;i<KP_instance->ccnt;i++){KP_instance->colname[i]=(char*) calloc(2000,sizeof(char));}

	for(i=0; i<KP_instance->ccnt; i++)
	{
		KP_instance->obj[i]=KP_instance->profits[i];
		KP_instance->lb[i]=0.0;
		//		KP_instance->ub[i]=CPX_INFBOUND;
		KP_instance->ub[i]=1.0;
		KP_instance->xctype[i]='B';
		sprintf(KP_instance->colname[i], "x%d",i+1);
	}

	KP_instance->status=CPXnewcols(KP_instance->env,KP_instance->lp,KP_instance->ccnt,KP_instance->obj,KP_instance->lb,KP_instance->ub,KP_instance->xctype,KP_instance->colname);
	if(KP_instance->status!=0)
	{
		printf("error in CPXnewcols\n");
		exit(-1);
	}

	free(KP_instance->obj);
	free(KP_instance->lb);
	free(KP_instance->ub);
	free(KP_instance->xctype);

	for(i=0;i<KP_instance->ccnt;i++){free(KP_instance->colname[i]);}
	free(KP_instance->colname);


	// * setting the objective function in the maximization form
	CPXchgobjsen(KP_instance->env,KP_instance->lp,CPX_MAX);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * creating the knapsack constraint *
	KP_instance->rcnt=1;
	KP_instance->nzcnt=KP_instance->item_number;
	KP_instance->rhs=(double*) calloc(KP_instance->rcnt,sizeof(double));
	KP_instance->sense=(char*) calloc(KP_instance->rcnt,sizeof(double));

	KP_instance->rhs[0]=KP_instance->capacity;
	KP_instance->sense[0]='L';


	KP_instance->rmatbeg=(int*) calloc(KP_instance->rcnt,sizeof(int));
	KP_instance->rmatind=(int*) calloc(KP_instance->nzcnt,sizeof(int));
	KP_instance->rmatval=(double*) calloc(KP_instance->nzcnt,sizeof(double));

	for(i=0; i<KP_instance->item_number; i++)
	{
		KP_instance->rmatval[i]=KP_instance->weights[i];
		KP_instance->rmatind[i]=i;
	}

	KP_instance->rmatbeg[0]=0;

	KP_instance->status=CPXaddrows(KP_instance->env,KP_instance->lp,0,KP_instance->rcnt,KP_instance->nzcnt,KP_instance->rhs,KP_instance->sense,KP_instance->rmatbeg,KP_instance->rmatind,KP_instance->rmatval,NULL,NULL);
	if(KP_instance->status!=0)
	{
		printf("error in CPXaddrows\n");
		exit(-1);
	}

	free(KP_instance->rmatbeg);
	free(KP_instance->rmatval);
	free(KP_instance->rmatind);
	free(KP_instance->rhs);
	free(KP_instance->sense);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////


	//	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	// * writing the created ILP model on a file *
	//	KP_instance->status=CPXwriteprob(KP_instance->env,KP_instance->lp,"kp.lp",NULL);
	//	if(KP_instance->status!=0) {
	//		printf("error in CPXwriteprob\n");
	//		exit(-1);
	//	}
	//	/////////////////////////////////////////////////////////////////////////////////////////////////////////




}

/***********************************************************************************/
void kp_free_cplex(data *KP_instance)
/***********************************************************************************/
{


	KP_instance->status=CPXfreeprob(KP_instance->env,&(KP_instance->lp));
	if(KP_instance->status!=0) {printf("error in CPXfreeprob\n");exit(-1);}

	KP_instance->status=CPXcloseCPLEX(&(KP_instance->env));
	if(KP_instance->status!=0) {printf("error in CPXcloseCPLEX\n");exit(-1);}

}

/***********************************************************************************/
double kp_solve_cplex(data *KP_instance)
/***********************************************************************************/
{

	int i,j;

	// * Set video output *
	KP_instance->status = CPXsetintparam (KP_instance->env, CPX_PARAM_SCRIND, CPX_OFF);
	if (KP_instance->status)
	{
		printf ("error for CPX_PARAM_SCRIND\n");
	}


	//	// * Set  tolerance *
	//	KP_instance->status = CPXsetdblparam (KP_instance->env, CPX_PARAM_EPAGAP, 0.0);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_EPAGAP\n");
	//	}
	//
	//	// * Set  tolerance *
	//	KP_instance->status = CPXsetdblparam (KP_instance->env, CPX_PARAM_EPGAP, 0.0);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_EPGAP\n");
	//	}
	//
	//
	//	// * Set mip tolerances integrality *
	//	KP_instance->status = CPXsetdblparam (KP_instance->env, CPX_PARAM_EPINT, 0.0);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_EPINTP\n");
	//	}
	//
	//	// * Set Feasibility tolerance *
	//	KP_instance->status = CPXsetdblparam (KP_instance->env, CPX_PARAM_EPRHS, 1e-9);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_EPRHS\n");
	//	}
	//
	//	// * Set number of CPU*
	//
	//	KP_instance->status = CPXsetintparam (KP_instance->env, CPX_PARAM_THREADS, KP_instance->number_of_CPU);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_THREADS\n");
	//	}
	//
	//	// * Set time limit *
	//
	//	KP_instance->status = CPXsetdblparam (KP_instance->env, CPX_PARAM_TILIM,KP_instance->timeLimit);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_TILIM\n");
	//	}



	///////////////////////////////////////////////////////////////////////////////////
	// * solving the MIP model
	clock_t time_start=clock();


	KP_instance->status=CPXmipopt(KP_instance->env,KP_instance->lp);
	if(KP_instance->status!=0)
	{
		printf("error in CPXmipopt\n");
		exit(-1);
	}

	clock_t time_end=clock();
	double solution_time=(double)(time_end-time_start)/(double)CLOCKS_PER_SEC;

	printf("\n\nsolution_time ->\t\%f",solution_time);


	///////////////////////////////////////////////////////////////////////////////////


	// * getting the solution

	//KP_instance->x=(double*) calloc(KP_instance->item_number,sizeof(double));


	KP_instance->status=CPXgetmipx(KP_instance->env,KP_instance->lp,KP_instance->x,0,KP_instance->item_number-1);
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipx\n");
		exit(-1);
	}

	KP_instance->status=CPXgetmipobjval(KP_instance->env,KP_instance->lp,&(KP_instance->objval));
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipobjval\n");
		exit(-1);
	}

	printf("\n\nMIP solution value ->\t\%f",KP_instance->objval);


	//	printf("\n\nSolution\n");
	//	for (i = 0; i < KP_instance->item_number; i++)
	//	{
	//		printf("item %d -> %d\n",i+1 ,(int)(KP_instance->x[i]+0.5));
	//	}
	//	printf("\n\n");

	KP_instance->status=CPXgetbestobjval(KP_instance->env,KP_instance->lp,&(KP_instance->bestobjval));
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetbestobjval\n");
		exit(-1);
	}

	KP_instance->lpstat=CPXgetstat(KP_instance->env,KP_instance->lp);
	KP_instance->nodecount = CPXgetnodecnt(KP_instance->env, KP_instance->lp);


	///////////////////////////////////////////////////////////////////////////////////
	/* linear programming relaxation*/

	double solution_time_lp=0;

	KP_instance->status = CPXchgprobtype (KP_instance->env, KP_instance->lp, CPXPROB_LP);

	clock_t time_start_lp=clock();
	KP_instance->status=CPXlpopt(KP_instance->env,KP_instance->lp);
	if(KP_instance->status!=0)
	{
		printf("err_FILEor in CPXlpopt slave solve\n");
		exit(-1);
	}

	clock_t time_end_lp=clock();

	solution_time_lp=(double)(time_end_lp-time_start_lp)/(double)CLOCKS_PER_SEC;

	double cplex_lp;

	KP_instance->status=CPXgetobjval(KP_instance->env,KP_instance->lp,&cplex_lp);
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipobjval\n");
		exit(-1);
	}

	printf("\n\nLP solution value ->\t\%f",cplex_lp);

	KP_instance->status=CPXgetx(KP_instance->env,KP_instance->lp,KP_instance->x,0,KP_instance->item_number-1);
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipx\n");
		exit(-1);
	}

	//	printf("\n\nSolution\n");
	//	for (i = 0; i < KP_instance->item_number; i++)
	//	{
	//		printf("item %d -> %.3f (p/w %.3f)\n",i+1,KP_instance->x[i],KP_instance->profits[i]/KP_instance->weights[i]);
	//	}
	//	printf("\n");

	///////////////////////////////////////////////////////////////////////////////////

	KP_instance->cur_numcols=CPXgetnumcols(KP_instance->env,KP_instance->lp);
	KP_instance->cur_numrows=CPXgetnumrows(KP_instance->env,KP_instance->lp);

	//printf("\nnumcols\t%d\n",KP_instance->cur_numcols);
	//printf("\nnumrows\t%d\n",KP_instance->cur_numrows);

	FILE *out;
	out=fopen("info_kp.txt","a+");
	fprintf(out,"%s\t%f\t%f\t%f\t%f\t%d\t%d\t%f\t%f\n",KP_instance->istname,KP_instance->capacity,KP_instance->objval,KP_instance->bestobjval,solution_time,KP_instance->lpstat,KP_instance->nodecount,cplex_lp,solution_time_lp);
	fclose(out);

	//free(KP_instance->x);

	return KP_instance->objval;

}


/***********************************************************************************/
double kp_solve_cplex_LP(data *KP_instance)
/***********************************************************************************/
{

	int i,j;

	// * Set video output *
	KP_instance->status = CPXsetintparam (KP_instance->env, CPX_PARAM_SCRIND, CPX_OFF);
	if (KP_instance->status)
	{
		printf ("error for CPX_PARAM_SCRIND\n");
	}

	// * Set number of CPU*

	KP_instance->status = CPXsetintparam (KP_instance->env, CPX_PARAM_THREADS, KP_instance->number_of_CPU);
	if (KP_instance->status)
	{
		printf ("error for CPX_PARAM_THREADS\n");
	}

	//	// * Set time limit *
	//
	//	KP_instance->status = CPXsetdblparam (KP_instance->env, CPX_PARAM_TILIM,KP_instance->timeLimit);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_TILIM\n");
	//	}


	///////////////////////////////////////////////////////////////////////////////////
	/* linear programming relaxation*/

	double solution_time_lp=0;

	KP_instance->status = CPXchgprobtype (KP_instance->env, KP_instance->lp, CPXPROB_LP);

	clock_t time_start_lp=clock();
	KP_instance->status=CPXlpopt(KP_instance->env,KP_instance->lp);
	if(KP_instance->status!=0)
	{
		printf("err_FILEor in CPXlpopt slave solve\n");
		exit(-1);
	}

	clock_t time_end_lp=clock();

	solution_time_lp=(double)(time_end_lp-time_start_lp)/(double)CLOCKS_PER_SEC;

	double cplex_lp;

	KP_instance->status=CPXgetobjval(KP_instance->env,KP_instance->lp,&cplex_lp);
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipobjval\n");
		exit(-1);
	}

	printf("\n\nLP solution value ->\t\%f",cplex_lp);

	KP_instance->status=CPXgetx(KP_instance->env,KP_instance->lp,KP_instance->x,0,KP_instance->item_number-1);
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipx\n");
		exit(-1);
	}

	//	printf("\n\nSolution\n");
	//	for (i = 0; i < KP_instance->item_number; i++)
	//	{
	//		printf("item %d -> %.3f (p/w %.3f)\n",i+1,KP_instance->x[i],KP_instance->profits[i]/KP_instance->weights[i]);
	//	}
	//	printf("\n");

	///////////////////////////////////////////////////////////////////////////////////

	//	KP_instance->cur_numcols=CPXgetnumcols(KP_instance->env,KP_instance->lp);
	//	KP_instance->cur_numrows=CPXgetnumrows(KP_instance->env,KP_instance->lp);
	//
	//	printf("\nnumcols\t%d\n",KP_instance->cur_numcols);
	//	printf("\nnumrows\t%d\n",KP_instance->cur_numrows);



	return cplex_lp;

}

/***********************************************************************************/
void kp_load_cplex_cover(data *KP_instance)
/***********************************************************************************/
{

	int i,j;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * setting the CPLEX environment

	//opening the environment
	KP_instance->env_sep=CPXopenCPLEX(&(KP_instance->status));
	if(KP_instance->status!=0)
	{
		printf("cannot open CPLEX environment\n");
		exit(-1);
	}

	//opening the pointer to the problem
	KP_instance->lp_sep=CPXcreateprob(KP_instance->env_sep,&(KP_instance->status),"SEP");
	if(KP_instance->status!=0)
	{
		printf("cannot create problem\n");
		exit(-1);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * creating the variables *
	KP_instance->ccnt=KP_instance->item_number;
	KP_instance->obj=(double*) calloc(KP_instance->ccnt,sizeof(double));
	KP_instance->lb=(double*) calloc(KP_instance->ccnt,sizeof(double));
	KP_instance->ub=(double*) calloc(KP_instance->ccnt,sizeof(double));
	KP_instance->xctype=(char*) calloc(KP_instance->ccnt,sizeof(char));


	KP_instance->colname=(char**) calloc(KP_instance->ccnt,sizeof(char*));
	for(i=0;i<KP_instance->ccnt;i++){KP_instance->colname[i]=(char*) calloc(2000,sizeof(char));}

	for(i=0; i<KP_instance->ccnt; i++)
	{
		KP_instance->obj[i]=0.0;
		KP_instance->lb[i]=0.0;
		//		KP_instance->ub[i]=CPX_INFBOUND;
		KP_instance->ub[i]=1.0;
		KP_instance->xctype[i]='B';
		sprintf(KP_instance->colname[i], "x%d",i+1);
	}

	KP_instance->status=CPXnewcols(KP_instance->env_sep,KP_instance->lp_sep,KP_instance->ccnt,KP_instance->obj,KP_instance->lb,KP_instance->ub,KP_instance->xctype,KP_instance->colname);
	if(KP_instance->status!=0)
	{
		printf("error in CPXnewcols\n");
		exit(-1);
	}

	free(KP_instance->obj);
	free(KP_instance->lb);
	free(KP_instance->ub);
	free(KP_instance->xctype);

	for(i=0;i<KP_instance->ccnt;i++){free(KP_instance->colname[i]);}
	free(KP_instance->colname);


	// * setting the objective function in the maximization form
	CPXchgobjsen(KP_instance->env_sep,KP_instance->lp_sep,CPX_MIN);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * creating the knapsack constraint *
	KP_instance->rcnt=1;
	KP_instance->nzcnt=KP_instance->item_number;
	KP_instance->rhs=(double*) calloc(KP_instance->rcnt,sizeof(double));
	KP_instance->sense=(char*) calloc(KP_instance->rcnt,sizeof(double));

	KP_instance->rhs[0]=KP_instance->capacity+1;
	KP_instance->sense[0]='G';


	KP_instance->rmatbeg=(int*) calloc(KP_instance->rcnt,sizeof(int));
	KP_instance->rmatind=(int*) calloc(KP_instance->nzcnt,sizeof(int));
	KP_instance->rmatval=(double*) calloc(KP_instance->nzcnt,sizeof(double));

	for(i=0; i<KP_instance->item_number; i++)
	{
		KP_instance->rmatval[i]=KP_instance->weights[i];
		KP_instance->rmatind[i]=i;
	}

	KP_instance->rmatbeg[0]=0;

	KP_instance->status=CPXaddrows(KP_instance->env_sep,KP_instance->lp_sep,0,KP_instance->rcnt,KP_instance->nzcnt,KP_instance->rhs,KP_instance->sense,KP_instance->rmatbeg,KP_instance->rmatind,KP_instance->rmatval,NULL,NULL);
	if(KP_instance->status!=0)
	{
		printf("error in CPXaddrows\n");
		exit(-1);
	}

	free(KP_instance->rmatbeg);
	free(KP_instance->rmatval);
	free(KP_instance->rmatind);
	free(KP_instance->rhs);
	free(KP_instance->sense);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	// * writing the created ILP model on a file *
	//	KP_instance->status=CPXwriteprob(KP_instance->env_sep,KP_instance->lp_sep,"sep.lp",NULL);
	//	if(KP_instance->status!=0) {
	//		printf("error in CPXwriteprob\n");
	//		exit(-1);
	//	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/***********************************************************************************/
void kp_load_cplex_cover_bis(data *KP_instance)
/***********************************************************************************/
{

	int i,j;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * setting the CPLEX environment

	//opening the environment
	KP_instance->env_sep_bis=CPXopenCPLEX(&(KP_instance->status));
	if(KP_instance->status!=0)
	{
		printf("cannot open CPLEX environment\n");
		exit(-1);
	}

	//opening the pointer to the problem
	KP_instance->lp_sep_bis=CPXcreateprob(KP_instance->env_sep_bis,&(KP_instance->status),"SEP_bis");
	if(KP_instance->status!=0)
	{
		printf("cannot create problem\n");
		exit(-1);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * creating the variables *
	KP_instance->ccnt=KP_instance->item_number;
	KP_instance->obj=(double*) calloc(KP_instance->ccnt,sizeof(double));
	KP_instance->lb=(double*) calloc(KP_instance->ccnt,sizeof(double));
	KP_instance->ub=(double*) calloc(KP_instance->ccnt,sizeof(double));
	KP_instance->xctype=(char*) calloc(KP_instance->ccnt,sizeof(char));


	KP_instance->colname=(char**) calloc(KP_instance->ccnt,sizeof(char*));
	for(i=0;i<KP_instance->ccnt;i++){KP_instance->colname[i]=(char*) calloc(2000,sizeof(char));}

	for(i=0; i<KP_instance->ccnt; i++)
	{
		KP_instance->obj[i]=0.0;
		KP_instance->lb[i]=0.0;
		//		KP_instance->ub[i]=CPX_INFBOUND;
		KP_instance->ub[i]=1.0;
		KP_instance->xctype[i]='B';
		sprintf(KP_instance->colname[i], "x%d",i+1);
	}

	KP_instance->status=CPXnewcols(KP_instance->env_sep_bis,KP_instance->lp_sep_bis,KP_instance->ccnt,KP_instance->obj,KP_instance->lb,KP_instance->ub,KP_instance->xctype,KP_instance->colname);
	if(KP_instance->status!=0)
	{
		printf("error in CPXnewcols\n");
		exit(-1);
	}

	free(KP_instance->obj);
	free(KP_instance->lb);
	free(KP_instance->ub);
	free(KP_instance->xctype);

	for(i=0;i<KP_instance->ccnt;i++){free(KP_instance->colname[i]);}
	free(KP_instance->colname);


	// * setting the objective function in the maximization form
	CPXchgobjsen(KP_instance->env_sep_bis,KP_instance->lp_sep_bis,CPX_MAX);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * creating the knapsack constraint *
	KP_instance->rcnt=1;
	KP_instance->nzcnt=KP_instance->item_number;
	KP_instance->rhs=(double*) calloc(KP_instance->rcnt,sizeof(double));
	KP_instance->sense=(char*) calloc(KP_instance->rcnt,sizeof(double));

	KP_instance->rhs[0]=KP_instance->cap_sep;
	KP_instance->sense[0]='L';


	KP_instance->rmatbeg=(int*) calloc(KP_instance->rcnt,sizeof(int));
	KP_instance->rmatind=(int*) calloc(KP_instance->nzcnt,sizeof(int));
	KP_instance->rmatval=(double*) calloc(KP_instance->nzcnt,sizeof(double));

	for(i=0; i<KP_instance->item_number; i++)
	{
		KP_instance->rmatval[i]=KP_instance->weights[i];
		KP_instance->rmatind[i]=i;
	}

	KP_instance->rmatbeg[0]=0;

	KP_instance->status=CPXaddrows(KP_instance->env_sep_bis,KP_instance->lp_sep_bis,0,KP_instance->rcnt,KP_instance->nzcnt,KP_instance->rhs,KP_instance->sense,KP_instance->rmatbeg,KP_instance->rmatind,KP_instance->rmatval,NULL,NULL);
	if(KP_instance->status!=0)
	{
		printf("error in CPXaddrows\n");
		exit(-1);
	}

	free(KP_instance->rmatbeg);
	free(KP_instance->rmatval);
	free(KP_instance->rmatind);
	free(KP_instance->rhs);
	free(KP_instance->sense);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	// * writing the created ILP model on a file *
	//	KP_instance->status=CPXwriteprob(KP_instance->env_sep_bis,KP_instance->lp_sep_bis,"sep_bis.lp",NULL);
	//	if(KP_instance->status!=0) {
	//		printf("error in CPXwriteprob\n");
	//		exit(-1);
	//	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/***********************************************************************************/
double kp_solve_cplex_cover(data *KP_instance)
/***********************************************************************************/
{

	int i,j;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	KP_instance->rmatind=(int*) calloc(KP_instance->nzcnt,sizeof(int));
	for(i=0; i<KP_instance->item_number; i++)
	{
		KP_instance->rmatind[i]=i;
	}
	KP_instance->status = CPXchgobj (KP_instance->env_sep,KP_instance->lp_sep, KP_instance->item_number, KP_instance->rmatind, KP_instance->profit_sep);
	free(KP_instance->rmatind);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	// * writing the created ILP model on a file *
	//	KP_instance->status=CPXwriteprob(KP_instance->env_sep,KP_instance->lp_sep,"sep_obj.lp",NULL);
	//	if(KP_instance->status!=0) {
	//		printf("error in CPXwriteprob\n");
	//		exit(-1);
	//	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////


	// * Set video output *
	KP_instance->status = CPXsetintparam (KP_instance->env_sep, CPX_PARAM_SCRIND, CPX_OFF);
	if (KP_instance->status)
	{
		printf ("error for CPX_PARAM_SCRIND\n");
	}


	//	// * Set number of CPU*
	//
	//	KP_instance->status = CPXsetintparam (KP_instance->env_sep, CPX_PARAM_THREADS, KP_instance->number_of_CPU);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_THREADS\n");
	//	}
	//
	//	// * Set time limit *
	//
	//	KP_instance->status = CPXsetdblparam (KP_instance->env_sep, CPX_PARAM_TILIM,KP_instance->timeLimit);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_TILIM\n");
	//	}



	///////////////////////////////////////////////////////////////////////////////////
	// * solving the MIP model
	clock_t time_start=clock();


	KP_instance->status=CPXmipopt(KP_instance->env_sep,KP_instance->lp_sep);
	if(KP_instance->status!=0)
	{
		printf("error in CPXmipopt\n");
		exit(-1);
	}

	clock_t time_end=clock();
	double solution_time=(double)(time_end-time_start)/(double)CLOCKS_PER_SEC;

	//printf("\n\nsolution_time ->\t\%f",solution_time);

	///////////////////////////////////////////////////////////////////////////////////


	// * getting the solution

	KP_instance->status=CPXgetmipx(KP_instance->env_sep,KP_instance->lp_sep,KP_instance->cover,0,KP_instance->item_number-1);
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipx\n");
		exit(-1);
	}

	KP_instance->status=CPXgetmipobjval(KP_instance->env_sep,KP_instance->lp_sep,&(KP_instance->objval_cover));
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipobjval\n");
		exit(-1);
	}

	//	printf("\n\n************COVER solution value ->\t\%f",KP_instance->objval_cover);
	//
	//
	//	printf("\n\n******************COVER\n");
	//	for (i = 0; i < KP_instance->item_number; i++)
	//	{
	//		printf("item %d -> %d\n",i+1 ,(int)(KP_instance->cover[i]+0.5));
	//	}
	//	printf("\n\n");

	return KP_instance->objval_cover;

}


/***********************************************************************************/
double kp_solve_cplex_cover_bis(data *KP_instance)
/***********************************************************************************/
{

	int i,j;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	KP_instance->rmatind=(int*) calloc(KP_instance->nzcnt,sizeof(int));
	for(i=0; i<KP_instance->item_number; i++)
	{
		KP_instance->rmatind[i]=i;
	}
	KP_instance->status = CPXchgobj (KP_instance->env_sep_bis,KP_instance->lp_sep_bis, KP_instance->item_number, KP_instance->rmatind, KP_instance->profit_sep);
	free(KP_instance->rmatind);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	// * writing the created ILP model on a file *
	//	KP_instance->status=CPXwriteprob(KP_instance->env_sep,KP_instance->lp_sep,"sep_obj.lp",NULL);
	//	if(KP_instance->status!=0) {
	//		printf("error in CPXwriteprob\n");
	//		exit(-1);
	//	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////


	// * Set video output *
	KP_instance->status = CPXsetintparam (KP_instance->env_sep_bis, CPX_PARAM_SCRIND, CPX_OFF);
	if (KP_instance->status)
	{
		printf ("error for CPX_PARAM_SCRIND\n");
	}


	//	// * Set number of CPU*
	//
	//	KP_instance->status = CPXsetintparam (KP_instance->env_sep, CPX_PARAM_THREADS, KP_instance->number_of_CPU);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_THREADS\n");
	//	}
	//
	//	// * Set time limit *
	//
	//	KP_instance->status = CPXsetdblparam (KP_instance->env_sep, CPX_PARAM_TILIM,KP_instance->timeLimit);
	//	if (KP_instance->status)
	//	{
	//		printf ("error for CPX_PARAM_TILIM\n");
	//	}



	///////////////////////////////////////////////////////////////////////////////////
	// * solving the MIP model
	clock_t time_start=clock();


	KP_instance->status=CPXmipopt(KP_instance->env_sep_bis,KP_instance->lp_sep_bis);
	if(KP_instance->status!=0)
	{
		printf("error in CPXmipopt\n");
		exit(-1);
	}

	clock_t time_end=clock();
	double solution_time=(double)(time_end-time_start)/(double)CLOCKS_PER_SEC;

	//printf("\n\nsolution_time ->\t\%f",solution_time);


	///////////////////////////////////////////////////////////////////////////////////


	// * getting the solution

	KP_instance->status=CPXgetmipx(KP_instance->env_sep_bis,KP_instance->lp_sep_bis,KP_instance->cover,0,KP_instance->item_number-1);
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipx\n");
		exit(-1);
	}

	KP_instance->status=CPXgetmipobjval(KP_instance->env_sep_bis,KP_instance->lp_sep_bis,&(KP_instance->objval_cover));
	if(KP_instance->status!=0)
	{
		printf("error in CPXgetmipobjval\n");
		exit(-1);
	}


	/*****************************************************************************/

	//cout<< "\nVAL CPLEX\t" << KP_instance->objval_cover << endl;
	//cin.get();


	double opt_val=-KP_instance->objval_cover+KP_instance->item_number;

	for (i = 0; i < KP_instance->item_number; i++)
	{
		opt_val-=KP_instance->x[i];
		KP_instance->cover[i]=1-KP_instance->cover[i];
	}


	//	printf("\n\n************COVER solution value ->\t\%f",KP_instance->objval_cover);
	//
	//

	//printf("\n\n******************COVER\n");
	//for (i = 0; i < KP_instance->item_number; i++)
	//{
	//	printf("item %d -> %d\n",i+1 ,(int)(KP_instance->cover[i]+0.5));
	//}
	//printf("\n\n");

	return opt_val;

}


/***********************************************************************************/
double kp_solve_DP_cover_bis(data *KP_instance)
/***********************************************************************************/
{

	int i,j;


	///////////////////////////////////////////////////////////////////////////////////
	// * solving the DP
	clock_t time_start=clock();


	KP_instance->objval_cover=DP_kp01_advanced(KP_instance->item_number,KP_instance->cap_sep, KP_instance->profit_sep,KP_instance->w_DP,KP_instance->cover,true,KP_instance->order_DP);

	clock_t time_end=clock();
	double solution_time=(double)(time_end-time_start)/(double)CLOCKS_PER_SEC;

	//printf("\n\nsolution_time ->\t\%f",solution_time);


	///////////////////////////////////////////////////////////////////////////////////


	//cout<< "\nVAL DP\t" << KP_instance->objval_cover << endl;
	//cin.get();


	double opt_val=-KP_instance->objval_cover+KP_instance->item_number;

	for (i = 0; i < KP_instance->item_number; i++)
	{
		opt_val-=KP_instance->x[i];
		KP_instance->cover[i]=1-KP_instance->cover[i];
	}


	//	printf("\n\n************COVER solution value ->\t\%f",KP_instance->objval_cover);
	//
	//

	//printf("\n\n******************COVER\n");
	//for (i = 0; i < KP_instance->item_number; i++)
	//{
	//	printf("item %d -> %d\n",i+1 ,(int)(KP_instance->cover[i]+0.5));
	//}
	//printf("\n\n");

	return opt_val;

}

/***********************************************************************************/
void kp_free_cplex_cover(data *KP_instance)
/***********************************************************************************/
{


	KP_instance->status=CPXfreeprob(KP_instance->env_sep,&(KP_instance->lp_sep));
	if(KP_instance->status!=0) {printf("error in CPXfreeprob\n");exit(-1);}

	KP_instance->status=CPXcloseCPLEX(&(KP_instance->env_sep));
	if(KP_instance->status!=0) {printf("error in CPXcloseCPLEX\n");exit(-1);}

}


/***********************************************************************************/
void kp_free_cplex_cover_bis(data *KP_instance)
/***********************************************************************************/
{


	KP_instance->status=CPXfreeprob(KP_instance->env_sep_bis,&(KP_instance->lp_sep_bis));
	if(KP_instance->status!=0) {printf("error in CPXfreeprob\n");exit(-1);}

	KP_instance->status=CPXcloseCPLEX(&(KP_instance->env_sep_bis));
	if(KP_instance->status!=0) {printf("error in CPXcloseCPLEX\n");exit(-1);}

}

/***********************************************************************************/
void add_cover(data *KP_instance)
/***********************************************************************************/
{

	int i,j;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * creating the knapsack constraint *
	KP_instance->rcnt=1;
	KP_instance->nzcnt=KP_instance->item_number;
	KP_instance->rhs=(double*) calloc(KP_instance->rcnt,sizeof(double));
	KP_instance->sense=(char*) calloc(KP_instance->rcnt,sizeof(double));

	KP_instance->sense[0]='L';


	KP_instance->rmatbeg=(int*) calloc(KP_instance->rcnt,sizeof(int));
	KP_instance->rmatind=(int*) calloc(KP_instance->nzcnt,sizeof(int));
	KP_instance->rmatval=(double*) calloc(KP_instance->nzcnt,sizeof(double));

	int counter=0;
	for(i=0; i<KP_instance->item_number; i++)
	{
		if(KP_instance->cover[i]>0.5)
		{
			KP_instance->rmatval[i]=1.0;
			counter++;
		}
		else{
			KP_instance->rmatval[i]=0.0;
		}
		KP_instance->rmatind[i]=i;
	}

	KP_instance->rhs[0]=counter-1;


	KP_instance->rmatbeg[0]=0;

	KP_instance->status=CPXaddrows(KP_instance->env,KP_instance->lp,0,KP_instance->rcnt,KP_instance->nzcnt,KP_instance->rhs,KP_instance->sense,KP_instance->rmatbeg,KP_instance->rmatind,KP_instance->rmatval,NULL,NULL);
	if(KP_instance->status!=0)
	{
		printf("error in CPXaddrows\n");
		exit(-1);
	}

	free(KP_instance->rmatbeg);
	free(KP_instance->rmatval);
	free(KP_instance->rmatind);
	free(KP_instance->rhs);
	free(KP_instance->sense);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////


	//	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	// * writing the created ILP model on a file *
	//	KP_instance->status=CPXwriteprob(KP_instance->env,KP_instance->lp,"sep_cover.lp",NULL);
	//	if(KP_instance->status!=0) {
	//		printf("error in CPXwriteprob\n");
	//		exit(-1);
	//	}
	//	/////////////////////////////////////////////////////////////////////////////////////////////////////////

}




