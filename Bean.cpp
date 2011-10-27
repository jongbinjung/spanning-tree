#include <iostream>
#include <stdio.h>
// #include <tchar.h> // Windows system specific header
#include <time.h>

using namespace std;

// set parameters
#define num_node 7          // # of actual nodes
#define num_prufer_code 5   // length of prufer number (num_node - 2)
#define NO_EDGE 999;        // large value set for non-connected edges

int pop_size=1000;          // population size (P)
int max_generation=100;     // maximum generations (N)
double cross_pro=0.5;       // Crossover probability (p_c)
double mutation_pro=0.005;  // mutation probability (p_m)

typedef char prufer_code[num_prufer_code];

struct My_gen
{
	prufer_code *p_code;
};
typedef struct My_gen gen;

double **rel_table;         // relation table containing edge weights

gen parents;
gen sel_children;
gen children;

gen Init();
int Selection(const double* fitness);
double* CalFitness();
void Crossover();
void Mutation();

int main(int argc, char *argv[])
{
	srand(time(NULL));

	int i, j, k;

    // create and initialize rel_table[][] to NO_EDGE
	rel_table=new double *[num_node];
	for(i=0;i<num_node;i++)
	{
		rel_table[i]=new double[num_node];
	}
    for(i=0;i<num_node;i++)
    {
        for(j=0;j<num_node;j++)
        {
            rel_table[i][j]=NO_EDGE;
        }
    }

    // assign significant edge values
    //  Edge 1
    rel_table[0][1] = 39;   rel_table[1][0] = 39;
    //  Edge 2
    rel_table[0][6] = 47;   rel_table[6][0] = 47;
    //  Edge 3
    rel_table[0][5] = 39;   rel_table[5][0] = 39;
    //  Edge 4
    rel_table[1][2] = 76;   rel_table[2][1] = 76;
    //  Edge 5
    rel_table[1][6] = 58;   rel_table[6][1] = 58;
    //  Edge 6
    rel_table[5][6] = 62;   rel_table[6][5] = 62;
    //  Edge 7
    rel_table[4][5] = 73;   rel_table[5][4] = 73;
    //  Edge 8
    rel_table[2][6] = 29;   rel_table[6][2] = 29;
    //  Edge 9
    rel_table[4][6] = 41;   rel_table[6][4] = 41;
    //  Edge 10
    rel_table[2][3] = 66;   rel_table[3][2] = 66;
    //  Edge 11
    rel_table[3][6] = 47;   rel_table[6][3] = 47;
    //  Edge 12
    rel_table[3][4] = 49;   rel_table[4][3] = 49;

	double p_cross, p_mut;
	int c_sel1, c_sel2;
	
	sel_children.p_code=new prufer_code[2];
	children.p_code=new prufer_code[pop_size];

	parents=Init();

	clock_t start, end;

	start=clock();

    // START : generation loop
	for(i=0;i<max_generation;i++)
	{
		double* fitness=CalFitness();

		double mean_fitness, min_fitness;
		
		mean_fitness=0;
		min_fitness=99999999999;

		int best;

		for(j=0;j<pop_size;j++)
		{
			mean_fitness=mean_fitness+fitness[j];
			if(min_fitness>fitness[j])
			{
				min_fitness=fitness[j];
				best=j;
			}
		}

		cout<<mean_fitness<<"\t"<<fitness[best]<<endl;

		for(j=0;j<pop_size/2;j++)
		{
			c_sel1=Selection(fitness);
			c_sel2=Selection(fitness);
			
			for(k=0;k<num_prufer_code;k++)
			{
				sel_children.p_code[0][k]=parents.p_code[c_sel1][k];
				sel_children.p_code[1][k]=parents.p_code[c_sel2][k];
			}

			Crossover();
			Mutation();

			for(k=0;k<num_prufer_code;k++)
			{
				children.p_code[2*j][k]=sel_children.p_code[0][k];
				children.p_code[2*j+1][k]=sel_children.p_code[1][k];
			}
		}

		for(k=0;k<num_prufer_code;k++)
		{
			children.p_code[pop_size-1][k]=parents.p_code[best][k];
		}		

		for(j=0;j<pop_size;j++)
		{
			for(k=0;k<num_prufer_code;k++)
			{
				parents.p_code[j][k]=children.p_code[j][k];
			}
		}
	} // END : generation loop
	
	double *fitness=CalFitness();

	double min_fitness;
	min_fitness=99999999999999999;

	int best;

	for(i=0;i<pop_size;i++)
	{
		if(min_fitness>fitness[i])
		{
			best=i;
		}
	}

	cout<<fitness[best]<<endl;
    for (i=0;i<num_prufer_code;i++)
    {
	    cout<<(int)parents.p_code[best][i]<<"\t";
    }
    cout << endl;
	end=clock();

	cout<<"Execution time : "<<(double)(end-start)/CLOCKS_PER_SEC<<"seconds"<<endl;

	delete sel_children.p_code;
	delete children.p_code;

	return 0;
}

gen Init()
{
	gen parents;
	parents.p_code= new prufer_code[pop_size];

	for(int i=0;i<pop_size;i++)
	{
		for(int j=0;j<num_prufer_code;j++)
		{
			parents.p_code[i][j]=rand()%num_node;
		}
	}

	return parents;
}


int Selection(const double* fitness)
{
	int i;
	double *cum_fitness;

	cum_fitness=new double[pop_size];

	double mean_fitness=0;

	for(i=0;i<pop_size;i++)
	{
		if(i==0)
		{
			cum_fitness[i]=1.0/fitness[i];
		}
		else
		{
			cum_fitness[i]=cum_fitness[i-1]+1.0/fitness[i];
		}
	}

	for(i=0;i<pop_size;i++)
	{
		cum_fitness[i]=cum_fitness[i]/cum_fitness[pop_size-1];
	}

	double p_sel;

	p_sel=(double)(rand()-1)/RAND_MAX;

	for(i=0;i<pop_size;i++)
	{
		if(cum_fitness[i]>p_sel)
		{
			return i;
		}
	}
}

double* CalFitness()
{
	int i,j,k;
	int least_num;
	int *count;

	double* fitness;

	fitness=new double[pop_size];
	count=new int[num_node];

	for(i=0;i<pop_size;i++)
	{
		for(j=0;j<num_node;j++)
		{
			count[j]=1;
		}
		for(j=0;j<num_prufer_code;j++)
		{
			count[parents.p_code[i][j]]++;
		}

		fitness[i]=0;

		for(j=0;j<num_prufer_code;j++)
		{
			int min_count;

			min_count=num_prufer_code;

			for(k=0;k<num_node;k++)
			{
				if(min_count>count[k] && count[k]!=0)
				{
					least_num=k;
					min_count=count[k];
				}
			}

			double tem=rel_table[least_num][parents.p_code[i][j]];
			fitness[i]=fitness[i]+tem;

			count[least_num]--;
			count[parents.p_code[i][j]]--;
		}

		int temp[2];

		for(j=0;j<2;j++)
		{
			for(k=0;k<num_node;k++)
			{
				if(count[k]!=0)
				{
					temp[j]=k;
					count[k]--;
					break;
				}
			}
		}

		double tem=rel_table[temp[0]][temp[1]];
		fitness[i]=fitness[i]+tem;
	}

	return fitness;
}

void Crossover()
{
	double p_cross=(double)rand()/RAND_MAX;

	if(p_cross<cross_pro)
	{
		int loc_cross=rand()%(num_prufer_code-2);
		for(int i=loc_cross;i<num_prufer_code;i++)
		{
			char temp, temp1;
			temp=sel_children.p_code[0][i];
			temp1=sel_children.p_code[1][i];
			sel_children.p_code[0][i]=sel_children.p_code[1][i];
			sel_children.p_code[1][i]=temp;
		}
	}
}

void Mutation()
{
	int i, j;
	double p_mut;

	for(i=0;i<2;i++)
	{
		for(j=0;j<num_prufer_code;j++)
		{
			p_mut=(double)rand()/RAND_MAX;
			if(p_mut<mutation_pro)
			{
				sel_children.p_code[i][j]=rand()%num_prufer_code;
			}
		}
	}
}
