#ifndef glo_function_HEADER
#define glo_function_HEADER


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


#include "global_variables.h"

/*********************************/
void read_instance_file(char *probname);
/*********************************/

/*********************************/
void free_data_prob();
/*********************************/

/*********************************/
void read_instance(char *probname);
/*********************************/

/*********************************/
int identify_instance_category(char * istname);
/*********************************/


#endif
