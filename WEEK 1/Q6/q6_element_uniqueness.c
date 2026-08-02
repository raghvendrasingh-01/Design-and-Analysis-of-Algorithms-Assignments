
#include <stdio.h>

#define MAX 100

/* Returns 1 if duplicates are found, otherwise 0 */
int hasDuplicates(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (arr[i] == arr[j])
                return 1;
        }
    }
    return 0;
}

void printArray(int arr[], int n)
{
    printf("\nArray: ");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main()
{
    int arr[MAX];
    int n;

    printf("===== Element Uniqueness Check =====\n");

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter %d numbers:\n", n);

    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printArray(arr, n);

    if (hasDuplicates(arr, n))
        printf("\nDuplicate elements are present.\n");
    else
        printf("\nAll elements are unique.\n");

    printf("\nTime Complexity  : O(n^2)\n");
    printf("Space Complexity : O(1)\n");

    printf("\nConclusion:\n");
    printf("This method compares every pair of elements.\n");
    printf("For sufficiently large values of n, it becomes slow because\n");
    printf("the number of comparisons grows quadratically.\n");
    printf("More efficient methods such as sorting or hashing can be used\n");
    printf("for large datasets.\n");

    return 0;
}
