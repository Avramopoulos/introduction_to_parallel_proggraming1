/*****************************************************************************************
  DESCRIPTION:   PTP programming using MPI (First asignement in introduction to parallel programming)
  AUTHOR:        George panagioths Avramopoulos
  CLASS:         ICE1-5006, ice.uniwa.gr 
  DATE:          1/12/2021
  AM:            19390001
*****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <limits.h>

void main(int argc, char** argv)  
{

 int response;
 int my_rank,p;
 int ctr=0;
 int i,k,res,finres,num;
 int source,target;
 int max_value=INT_MIN,min_value=INT_MAX,finmax_value,finmin_value;
 int tag1=50, tag2=60, tag3=70,tag4=80,tag5=90,tag6=100,tag7=110,tag8=120,tag9=130,tag10=140;
 int plithos;
 int data[100];
 int data_loc[100];
 float delta_array[100];
 float finres2,res2,res3,median;
 float *local_array;
   
   MPI_Status status;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &p);
   
response=1;
do{
   max_value=INT_MIN,min_value=INT_MAX;  
   
 if(ctr>0 && my_rank != 0  ){ //only enters if the programme has been executed atleast once
       MPI_Recv(&response, 1, MPI_INT, 0, tag5, MPI_COMM_WORLD, &status); //receives the answer from the user if it wants to terminate the programme or not ('1'== continues , '2'== terminates the programme) 
       if(response==2)
       	break; //breaks the do while and terminates the remaining processes
       }
       	
   if (my_rank == 0)  {
   
       printf("Write the number of elements:\n");
       scanf("%d", &plithos); //gets the number of elements
       
       printf("Write %d elements (one by one):\n", plithos); 
       for (k=0; k<plithos; k++)
            scanf("%d", &data[k]); //gets the elements and puts them in an array
            
       for (target = 1; target < p; target++) 
           MPI_Send(&plithos, 1, MPI_INT, target, tag1, MPI_COMM_WORLD); //sends the number of elements to the rest of the processes
      num = plithos/p;  //the number of elements that corespond to each process
      k=num;
      for (target = 1; target < p; target++)  {//changes the process to be sent
         MPI_Send(&data[k], num, MPI_INT, target, tag2, MPI_COMM_WORLD); //sends to all other processes the elements that corespond to each process
         k+=num;  //changes the elements to be send from the array
         }
      for (k=0; k<num; k++) 
         data_loc[k]=data[k];
         
     }
   else  { 	//all the other processes do this except from the root process
       MPI_Recv(&plithos, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);
       num = plithos/p;
       MPI_Recv(&data_loc[0], num, MPI_INT, 0, tag2, MPI_COMM_WORLD, &status);
     }

   res = 0;
   for (k=0; k<num; k++){
        res += data_loc[k]; //res is the local sum of each process
        if(max_value<data_loc[k])
        	max_value=data_loc[k]; //finds the maximum value in each process (will be used in the vector ? later)
        if(min_value>data_loc[k])
        	min_value=data_loc[k];	//finds the minimum value in each process (will be used in the vector ? later)
	}	
	
   if (my_rank != 0)  { //all the other processes do this except from the root process
       MPI_Send(&res, 1, MPI_INT, 0, tag3, MPI_COMM_WORLD);
       MPI_Send(&max_value, 1, MPI_INT, 0, tag6, MPI_COMM_WORLD);
       MPI_Send(&min_value, 1, MPI_INT, 0, tag7, MPI_COMM_WORLD);
    }
   else  {//root process does this
       finmax_value=max_value; 
       finmin_value=min_value;
       finres = res;
       printf("\n Result of process %d: %d\n", my_rank, res);
       printf("\nprocess %d has the following local max and min values\n local max: %d local min: %d",my_rank,max_value,min_value);
       
       for (source = 1; source < p; source++)  { //receives the local sum,local min and local max values from the rest of the processes
           MPI_Recv(&res, 1, MPI_INT, source, tag3, MPI_COMM_WORLD, &status);
           MPI_Recv(&max_value, 1, MPI_INT, source, tag6, MPI_COMM_WORLD, &status);
           MPI_Recv(&min_value, 1, MPI_INT, source, tag7, MPI_COMM_WORLD, &status);
           finres += res; //finds the total sum of all the processes
           printf("\nprocess %d has the following local max and min values\n local max: %d local min: %d",source,max_value,min_value);
           if(finmax_value<max_value)
        	finmax_value=max_value;	//finds the maximum value in the array (will be used in the vector ? later)
           if(finmin_value>min_value)
        	finmin_value=min_value;	//finds the minimum value in the array (will be used in the vector ? later)
           printf("\n Result of process %d: %d\n", source, res);
        }
        
    median=(float)finres/plithos; //finds the median value 
    printf("\n Max: %d Min: %d",finmax_value,finmin_value);
    printf("\n\n\n Median value: %.2f\n", median);
    
    for (target = 1; target < p; target++) { //sends the median,min and max values in the rest of the processes
           MPI_Send(&median, 1, MPI_FLOAT, target, tag4, MPI_COMM_WORLD);
           MPI_Send(&finmax_value, 1, MPI_INT, target, tag9, MPI_COMM_WORLD);
           MPI_Send(&finmin_value, 1, MPI_INT, target, tag10, MPI_COMM_WORLD);
           }
   }

if(my_rank!=0){ //receives the median,min and max values in the rest of the processes
	MPI_Recv(&median, 1, MPI_FLOAT, 0, tag4, MPI_COMM_WORLD, &status);
	MPI_Recv(&finmax_value, 1, MPI_INT, 0, tag9, MPI_COMM_WORLD, &status);
	MPI_Recv(&finmin_value, 1, MPI_INT, 0, tag10, MPI_COMM_WORLD, &status);
	}
local_array=(float*)calloc(num,sizeof(float));	//makes an array to be used to store the results of the vector ?

res2=0; //local result of the variance
for (k=0; k<num; k++){
        res2 += (data_loc[k]-median)*(data_loc[k]-median);
        if(finmax_value-finmin_value!=0) //checks that the denominator is not 0
     		local_array[k]= (((float)data_loc[k]-finmin_value)/(int)(finmax_value-finmin_value))*100; //calculates the vector ?
     	else{
     		printf("\nCan't divide with 0 thus the new array cant be completed\n");
     		printf("\nExiting the program...\n");
     		exit(2);
     		}
    }
    

if (my_rank != 0)  {//the rest of the processes send local result and the local vector ?
       MPI_Send(&res2, 1, MPI_FLOAT, 0, tag5, MPI_COMM_WORLD);
       MPI_Send(&local_array[0], num, MPI_FLOAT, 0, tag8, MPI_COMM_WORLD);
}
else{//root process enters this block
       finres2 = res2;
       for (k=0; k<num; k++)
      	   delta_array[k]=local_array[k];

       for (source = 1; source < p; source++)  {
           //receives the local result and the local vector ?
           MPI_Recv(&res2, 1, MPI_FLOAT, source, tag5, MPI_COMM_WORLD, &status);
           MPI_Recv(&local_array[0], num, MPI_FLOAT, source, tag8, MPI_COMM_WORLD, &status);
           
           finres2 += res2; //sums the variance 
           
           for(i=0;i<num;i++){
           	delta_array[k]=local_array[i]; //all the local arrays are made into one array
           	k++;
           }
            k+=num;
            
        }
    float variance=(float)finres2/plithos;
    printf("\n\n\n Variance value: %.2f\n", variance);
    
    for(i=0;i<plithos;i++)
    	printf("\n\n\n Delta array value of position %d : is : %.2f\n", i,delta_array[i]);
    	
    printf("\n\n\nWrite '1' to continue or write '2' to terminate the program\n");
    scanf("%d",&response);
    for (target = 1; target < p; target++)
    	MPI_Send(&response, 1, MPI_INT, target, tag5, MPI_COMM_WORLD);
   }
   
  ctr++;
 }while(response != 2);
 //uncomment the following statement to see that all the processes have been terminated
 //printf("\nbye from the the process: %d \n",my_rank);
   MPI_Finalize();
}
