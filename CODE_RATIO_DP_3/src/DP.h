#ifndef DP_HEADER
#define DP_HEADER


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


#include  "global_variables.h"
#include  "global_functions.h"

using namespace std;

/*****************************************************************************/
double DP_kp_RATIO_POWER_LIGHT(int n, int C, double * p, vector < int > w,double *sol,int max_i,double elle,double constant);
/*****************************************************************************/

/*****************************************************************************/
double DP_kp_RATIO_POWER(int n, int C, double * p, vector < int > w,double *sol,int max_i,double elle,double constant);
/*****************************************************************************/

#endif
