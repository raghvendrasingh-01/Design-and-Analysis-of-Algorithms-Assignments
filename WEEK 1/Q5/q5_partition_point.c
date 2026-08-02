
#include <stdio.h>

#define MAX 100

/* Binary Search to find first occurrence of 1 */
int findPartitionPoint(int arr[], int n)
{
    int low = 0, high = n - 1;
    int ans = -1;

    while (low <= high)
    {
        int mid = (low + high) / 2;

        if (arr[mid] == 1)
        {
            ans = mid;
            high = mid - 1;   // Search left for first 1
        }
        else
        {
            low = mid + 1;
        }
    }

    return ans;
}

void printArray(int arr[], int n)
{
    printf("Array: ");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main()
{
    int n;
    int arr[MAX];

    printf("===== Partition Point Finder =====\n");

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter %d elements (0s followed by 1s):\n", n);

    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printArray(arr, n);

    int partition = findPartitionPoint(arr, n);

    if (partition == -1)
    {
        printf("\nNo transition found.\n");
        printf("The array contains only 0s.\n");
    }
    else if (partition == 0)
    {
        printf("\nTransition at index 0.\n");
        printf("The array contains only 1s.\n");
    }
    else
    {
        printf("\nPartition Point = %d\n", partition);
        printf("Transition occurs between index %d and %d.\n",
               partition - 1, partition);
        printf("0 -> 1 transition found successfully.\n");
    }
    
    printf("\nTime Complexity  : O(log n)\n");
    printf("Space Complexity : O(1)\n");

    printf("\nConclusion:\n");
    printf("Binary Search efficiently finds the first occurrence of 1,\n");
    printf("which is the partition point between 0s and 1s.\n");

    return 0;
}
