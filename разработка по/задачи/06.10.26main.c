#include <stdio.h>

void swap(int *a, int *b)
{
    int c = *a;
    *a = *b;
    *b = c;
}
float circle_length(float radius)
{
    return 2 * 3.14 * radius;
}
int factorial(int n)
{
    int result = 1;
    for (int i = 1; i <= n; i++)
{
        result = result * i;
}

return result;
}

int is_prime(int n)
{
    int count = 0;
    for (int i = 1; i <= n; i++)
{
        if (n % i == 0)
{
            count++;
}
}
    if (count == 2)
        return 1;
    else
        return 0;
}
int main()
{
    int a, b, n;

// 5.1
scanf("%d %d", &a, &b);
swap(&a, &b);
printf("%d %d\n", a, b);

// 5.2
float radius;
scanf("%f", &radius);
printf("%f\n", circle_length(radius));

// 5.3
for (n = 1; n <= 10; n++)
{
    printf("%d! = %d\n", n, factorial(n));
}

// 5.4
for (n = 1; n <= 100; n++)
{
    if (is_prime(n))
    {
        printf("%d ", n);
    }
}

return 0;
}

