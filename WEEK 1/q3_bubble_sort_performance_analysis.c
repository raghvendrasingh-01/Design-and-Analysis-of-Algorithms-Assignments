
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 25

int comparisons1 = 0;
int comparisons2 = 0;

void generateRandom(int arr[], int n)
{
    for(int i=0;i<n;i++)
        arr[i]=rand()%100;
}

void copyArray(int src[], int dest[], int n)
{
    for(int i=0;i<n;i++)
        dest[i]=src[i];
}

void bubbleSortEarlyStop(int arr[], int n)
{
    comparisons1=0;

    for(int i=0;i<n-1;i++)
    {
        int swapped=0;

        for(int j=0;j<n-i-1;j++)
        {
            comparisons1++;
            if(arr[j]>arr[j+1])
            {
                int t=arr[j];
                arr[j]=arr[j+1];
                arr[j+1]=t;
                swapped=1;
            }
        }

        if(!swapped)
            break;
    }
}

void bubbleSortFullPass(int arr[], int n)
{
    comparisons2=0;

    for(int i=0;i<n-1;i++)
    {
        for(int j=0;j<n-i-1;j++)
        {
            comparisons2++;
            if(arr[j]>arr[j+1])
            {
                int t=arr[j];
                arr[j]=arr[j+1];
                arr[j+1]=t;
            }
        }
    }
}

void printArray(int arr[],int n)
{
    for(int i=0;i<n;i++)
        printf("%d ",arr[i]);
    printf("\n");
}

void createCSV(int n)
{
    FILE *fp=fopen("bubble_sort_analysis.csv","w");
    if(fp==NULL)
    {
        printf("Unable to create CSV file.\n");
        return;
    }

    fprintf(fp,"Algorithm,Comparisons\n");
    fprintf(fp,"Early Termination,%d\n",comparisons1);
    fprintf(fp,"Full Pass,%d\n",comparisons2);

    fclose(fp);
    printf("\nCSV file 'toh.csv' created successfully.\nOpen it in Excel, LibreOffice or Google Sheets and insert a Line Chart.\n");
}

void printGraph()
{
    printf("\nComparison Graph\n");
    printf("------------------------------\n");

    printf("Early Termination : ");
    for(int i=0;i<comparisons1;i+=5)
        printf("#");
    printf(" (%d)\n",comparisons1);

    printf("Full Pass         : ");
    for(int i=0;i<comparisons2;i+=5)
        printf("\u2588");
    printf(" (%d)\n",comparisons2);
}

int main()
{
    int n;
    int arr[MAX],a1[MAX],a2[MAX];

    srand(time(NULL));

    printf("===== Bubble Sort Performance Analysis =====\n");
    printf("Enter number of elements (max %d): ",MAX);
    scanf("%d",&n);

    generateRandom(arr,n);

    copyArray(arr,a1,n);
    copyArray(arr,a2,n);

    printf("\nRandom Array:\n");
    printArray(arr,n);

    bubbleSortEarlyStop(a1,n);
    bubbleSortFullPass(a2,n);

    printf("\nComparisons (Early Termination) : %d\n",comparisons1);
    printf("Comparisons (Full Pass)         : %d\n",comparisons2);

    createCSV(n);
    printGraph();

    printf("\nTime Complexity:\n");
    printf("Early Termination : Best O(n), Worst O(n^2)\n");
    printf("Full Pass         : O(n^2)\n");
    printf("Space Complexity  : O(1)\n");

    printf("\nConclusion:\n");
    printf("The early termination version performs fewer comparisons\n");
    printf("when the array becomes sorted before all passes.\n");
    printf("The full-pass version always performs all passes,\n");
    printf("making it less efficient.\n");

    return 0;
}
