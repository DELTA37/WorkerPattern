#include "SLAU/SLAU.hpp"

int main(int argc, char** argv) {
  double* A;
  double* b;
  double* res;
  unsigned int n;
  if (argc > 1) {
    readFromFile(argv[1], A, b, n);
    print(A, b, n);
    solve(A, b, n, res);
    printf("Solution: \n");
    print(res, n);
    printf("Error: \n");
    printf("%lf", errorMax(A, b, n, res));
    printf("\n");
  }
  else {
    readFromConsole(A, b, n);
    solve(A, b, n, res);
    printf("Solution: \n");
    print(res, n);
    printf("Error: \n");
    printf("%lf", errorMax(A, b, n, res));
    printf("\n");
  }
  free(A);
  free(b);
  free(res);
  return 0;
}
