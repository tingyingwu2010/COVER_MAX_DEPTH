#ifndef VARIABLE_local_HEADER
#define VARIABLE_local_HEADER


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <errno.h>
#include <cstring>
#include <cstdlib>

using namespace std;

#include </home/fabio/ILOG/CPLEX_Studio_AcademicResearch129/cplex/include/ilcplex/cplex.h>
//#include </Users/fabiofurini/Applications/IBM/ILOG/CPLEX_Studio127/cplex/include/ilcplex/cplex.h>



///////////////////////////////
/*     INPUT DATA            */
extern int *profits;
extern vector < int > weights;
extern int capacity;
extern int item_number;
//extern int item_input;
//extern int cap_input;
///////////////////////////////

extern int cat;
extern int R;

///////////////////////////////
/*      INPUT PARAMETERS     */
extern double timeLimit;
extern char *istname;
extern int algo;
extern int option;
///////////////////////////////


typedef struct data
{
	char *istname;
	int algorithm;
	int number_of_CPU;
	int timeLimit;

	int item_number;
	double capacity;
	double *weights;
	double *profits;

	int status,ccnt,rcnt,nzcnt,lpstat,nodecount,cur_numrows,cur_numcols;
	int* rmatbeg,*rmatind,*cmatbeg, *cmatind;
	double *rmatval,*cmatval,*rngval,*x,*pi,*obj, *lb, *ub,*rhs,coef_p,objval,bestobjval;
	char *xctype,*sense;
	char **rowname,**colname;

	CPXENVptr env;
	CPXLPptr  lp;

	CPXENVptr env_sep,env_sep_bis;
	CPXLPptr  lp_sep,lp_sep_bis;
	double *profit_sep;
	double *cover;
	double objval_cover;
	int cap_sep;
	int *x_int;


	vector < int > w_DP;
	double *sol_DP;
	vector < int > order_DP;


} data;

#endif
