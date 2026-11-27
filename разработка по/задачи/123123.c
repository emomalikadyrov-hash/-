#include <stdio.h>

int main()
{
    int n;
    int a;
    int b;
    int i;
    int j;

    int count;


    scanf("%d",&n);


    for (i = 1; i <= n; i++)
    {

        if (n % i == 0)
        {

            printf("%d ", i);
        }
    }


    printf("\n");

    // 4.2

    scanf("%d %d", &a, &b);


    for (i = 1; i <=a; i++)
    {

        if (a % i == 0)
        {

            if (b % i == 0)
            {

                printf("%d", i);
            }
        }
    }


    // 4.3
    scanf("%d",&n);

    for (i = 2; i <= n; i++)
    {

        count = 0;


        for (j = 1; j <=i; j++)
        {

            if (i % j == 0)
            {

                count++;
            }
        }


        if (count == 2)
        {

             printf("%d ", i);
        }

    }
    printf("\n");

    // 4.4


    scanf("%d %d", &a, &b);


    while (a != b)
    {
        if (a>b)
        {
            a = a - b;
        }
        else if (b>a)
        {
            b = b - a;
        }
    }


    printf("%d", a);

    return 0;
}















