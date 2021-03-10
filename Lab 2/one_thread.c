#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


struct mystruct
{
    int **matrix_2d;
    int *matrix_1d; 
    int **result; //result after the convolution
    int x; //rows
    int y; //columns
    int counter; //how many times the covolution will run
};
//convolution function takes the pointer of the array
void *convolution(void *ptr)
{
    struct mystruct *item = (struct mystruct*)ptr;

    //convolution
    for(int i = 0; i < (*item).x; i++)
    {
        for(int j = 0; j < (*item).y; j++)
        {
            //the left side of the first value is 0, so ignore the first product of convolution
            if(j == 0)
            {
                item->result[i][j] = item->matrix_1d[1] * item->matrix_2d[i][j] + item->matrix_1d[2] * item->matrix_2d[i][j + 1];
            }
            //the right side of the last value is 0 as well, so ignore the last product of convolution
            else if(j == (*item).y - 1)
            {
                item->result[i][j] = item->matrix_1d[0] * item->matrix_2d[i][j - 1] + item->matrix_1d[1] * item->matrix_2d[i][j];
            }
            else
            {
                item->result[i][j] = item->matrix_1d[0] * item->matrix_2d[i][j - 1] + item->matrix_1d[1] * item->matrix_2d[i][j] + item->matrix_1d[2] * item->matrix_2d[i][j + 1];
            }
            //count how many times convolution ran
            item->counter++;
        }
    }
    return NULL;
}
int main()
{
    FILE *fileopen; //declare file pointer
    fileopen = fopen("2x10.txt", "r"); //open text file 2x10.txt and "r" - read
    int x, y; //size of rows and columns
    int **array; // double pointer array because it's 2d matrix (has rows and columns)
    int **result; // get the result of convolution from 2d matrix (has rows and columns)

    int masking_x; 
    int masking_y;
    int *array_2; // 1d matrix meaning it only has one row and multiple columns
     
     //to check if file opened
    if(fileopen == NULL)
    {
        printf("File does not exist. \n");
        return 0;
    }
    //when file open
    else
    {
        fscanf(fileopen,"%d", &x); //get the number of rows from the text files, use fscanf if you get only one value
        fscanf(fileopen,"%d", &y); //get the number of columns from the text file

        //2D pointer array
        array = malloc(sizeof(int*) * x); // *int because it's double array, memory for rows
        result = malloc(sizeof(int*) * x);
        //memory for columns
        for(int i = 0; i < x; i++)
        {
            array[i] = malloc(sizeof(int) * y);
            result[i] = malloc(sizeof(int) * y);
        }
        //after gain memory for array, assign values in array
        for (int i = 0; i < x;i++)
        {
            for (int j = 0; j < y;j++)
            {
                fscanf(fileopen, "%d", &array[i][j]); 
            }
        } 

        //1D pointer array, tips:when we fscanf text file the program will read in order just like we reading book
        fscanf(fileopen, "%d", &masking_x); // row for 1D matrix
        fscanf(fileopen, "%d", &masking_y); // column for 1D matrix
        array_2 = malloc(sizeof(int) * masking_y); //since the row is one, only need memory for columns
        //assign values into 1D matrix (columns)
        for(int i = 0; i < masking_y; i++)
        {
            fscanf(fileopen, "%d", &array_2[i]);
        }

        fclose(fileopen);

    }


    struct mystruct *convo = malloc(sizeof(struct mystruct)*1);
    
    convo->matrix_2d = array;
    convo->matrix_1d = array_2;
    convo->result = result;
    convo->x = x;
    convo->y = y;
    
    pthread_t thread;

    //time the pthread
    clock_t beginning = clock();
    
    pthread_create(&thread, NULL, convolution, &convo);
    pthread_join(thread,NULL);

    //stops the time
    clock_t end = clock();
    
    //print out the result of convolution
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            printf(" %d  ", result[i][j]);

        }
        printf(" \n");
    }
    //to calculate the total time
    double total_time = (double)(end-beginning)/ CLOCKS_PER_SEC;
    
    
    printf("Total time: %lf seconds\n", total_time);
    printf("Total count of convolution: %d", convo->counter);
    return 0;
}

