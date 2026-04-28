#include <stdio.h>
#include <omp.h>

double f(double x) {
    return x * x;
}

int main() {
    int n;
    double a, b;

    printf("Enter lower limit (a): ");
    scanf("%lf", &a);
    printf("Enter upper limit (b): ");
    scanf("%lf", &b);
    printf("Enter number of intervals (n): ");
    scanf("%d", &n);

    double h = (b - a) / n;
    double sum_red = 0.0, result_red, start, end;

    // --- Reduction Method ---
    start = omp_get_wtime();
    #pragma omp parallel for reduction(+:sum_red)
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum_red += f(x);
    }
    result_red = h * ((f(a) + f(b)) / 2.0 + sum_red);
    end = omp_get_wtime();

    printf("\n--- Using Reduction ---\n");
    printf("Result: %f\n", result_red);
    printf("Time: %f seconds\n", end - start);

    // --- Critical Section Method ---
    double sum_crit = 0.0;
    double result_crit;
    start = omp_get_wtime();

    #pragma omp parallel
    {
        double local_sum = 0.0;
        #pragma omp for
        for (int i = 1; i < n; i++) {
            double x = a + i * h;
            local_sum += f(x);
        }
        #pragma omp critical
        {
            sum_crit += local_sum;
        }
    }
    result_crit = h * ((f(a) + f(b)) / 2.0 + sum_crit);
    end = omp_get_wtime();

    printf("\n--- Using Critical ---\n");
    printf("Result: %f\n", result_crit);
    printf("Time: %f seconds\n", end - start);

    return 0;
}
