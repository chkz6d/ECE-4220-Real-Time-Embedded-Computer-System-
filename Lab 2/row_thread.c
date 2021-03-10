#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


struct mystruct
{
    int **matrix_2d;
    int *matrix_1d;
    int **result;
    int i;
    int y;
    int counter; //how many times the covolution will run
};
void *convolution(void *ptr)
{
    struct mystruct *item = (struct mystruct*)ptr;
    int  i = (*item).i;
        for(int j = 0; j < (*item).y; j++)
        {
            if(j == 0)
            {
                item->result[i][j] = item->matrix_1d[1] * item->matrix_2d[i][j] + item->matrix_1d[2] * item->matrix_2d[i][j + 1];
            }
            else if(j == (*item).y - 1)
            {
                item->result[i][j] = item->matrix_1d[0] * item->matrix_2d[i][j - 1] + item->matrix_1d[1] * item->matrix_2d[i][j];
            }
            else
            {
                item->result[i][j] = item->matrix_1d[0] * item->matrix_2d[i][j - 1] + item->matrix_1d[1] * item->matrix_2d[i][j] + item->matrix_1d[2] * item->matrix_2d[i][j + 1];
            }
            item->counter++;
        }
    return NULL;
}
int main()
{
    FILE *fileopen; //declare file pointer
    fileopen = fopen("2x10.txt", "r"); //open text file 2x10.txt and "r" - read
    int x, y;
    int **array;
    int **result;

    int masking_x;
    int masking_y;
    int *array_2;

    if(fileopen == NULL)
    {
        printf("File does not exist. \n");
        return 0;
    }
    else
    {
        fscanf(fileopen,"%d", &x); //get the number of rows from the text file
        fscanf(fileopen,"%d", &y); //get the number of columns from the text file

        //2D pointer array
        array = malloc(sizeof(int*) * x);
        result = malloc(sizeof(int*) * x);
        for(int i = 0; i < x; i++)
        {
            array[i] = malloc(sizeof(int) * y);
            result[i] = malloc(sizeof(int) *y);
        }
        
        for (int i = 0; i < x;i++)
        {
            for (int j = 0; j < y;j++)
            {
                fscanf(fileopen, "%d", &array[i][j]);
            }
        } 

        //1D pointer array
        fscanf(fileopen, "%d", &masking_x);
        fscanf(fileopen, "%d", &masking_y);
        array_2 = malloc(sizeof(int) * masking_y);
        for(int i = 0; i < masking_y; i++)
        {
            fscanf(fileopen, "%d", &array_2[i]);
        }

        fclose(fileopen);

    }


    struct mystruct *convo;
    convo = malloc(sizeof(struct mystruct) * x);

    for (int i = 0; i < x;i++)
    {
        convo[i].matrix_2d = array;
        convo[i].matrix_1d = array_2;
        convo[i].result = result;
        convo[i].i = i;
        convo[i].y = y;
    }
    
    pthread_t thread[x];

    clock_t beginning = clock();
    for(int i=0; i < x ;i++)
    {    
        pthread_create(&thread[i], NULL, convolution, &convo[i]);
    }
    for(int i=0; i < x;i++)
    {
        pthread_join(thread[i],NULL);
    }
    
    clock_t end = clock();
    
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            printf(" %d  ", result[i][j]);

        }
        printf(" \n");
    }
    
    int counter;
    for (int i=0; i < x;i++)
    {
        counter = counter + convo[i].counter;
    }
    double total_time = (double)(end-beginning)/ CLOCKS_PER_SEC;
    printf("Total time: %lf seconds\n", total_time);
    printf("Total count of convolution: %d", counter);
    return 0;
}
