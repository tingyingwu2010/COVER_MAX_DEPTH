

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
	profits=new double[item_number];


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
