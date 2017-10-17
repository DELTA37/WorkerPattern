#include "SLAU/SLAU.hpp"
#include "TaskManager/Task.hpp"
#include "TaskManager/Worker.hpp"

static TaskManager tm(THREAD_NUM);

struct SumTask : public Task {
  double a11;
  double a12;
  double a21;
  double a22;
  double* arr1;
  double* arr2;
  int n;
  SumTask(double a11, double a12, double a21, double a22, double* arr1, double* arr2, int n) {
    this->a11 = a11;
    this->a12 = a12;
    this->a21 = a21;
    this->a22 = a22;
    this->arr1 = arr1;
    this->arr2 = arr2;
    this->n = n;
  }
  void Execute(void) {
    for (int i = 0; i < n; i++) {
      double x = a11 * arr1[i] + a12 * arr2[i];
      double y = a21 * arr1[i] + a22 * arr2[i];
      arr1[i] = x;
      arr2[i] = y;
    }
  }
};

struct ChangeTask : public SumTask {
  ChangeTask(double* arr1, double* arr2, int n) : SumTask(0, 1, 1, 0, arr1, arr2, n) {}
};

int readFromFile(const char* filename, double* (&A), double* (&b), unsigned int &n) {
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    A = NULL;
    b = NULL;
    return -1;
  }
  else {
    fscanf(fp, "%u", &n);
    A = (double*)malloc(n * n * sizeof(double));
    b = (double*)malloc(n * sizeof(double));
    for (size_t i = 0; i < n; i++)
    for (size_t j = 0; j < n; j++) {
      if (fscanf(fp, "%lf", &(A[at(i, j, n)])) <= 0) {
        return -1;
      }
    }
    for (size_t i = 0; i < n; i++) {
      if (fscanf(fp, "%lf", &(b[i])) <= 0) {
        return -1;
      }
    }
    fclose(fp);
  }
  return 0;
}

double errorMax(double* A, double* b, unsigned int n, double* res) {
  double err = 0;
  for (size_t i = 0; i < n; i++) {
    double part_err = b[i];
    for (size_t j = 0; j < n; j++) {
      part_err -= A[at(i, j, n)] * res[j];
    }
    err += part_err * part_err;
  }
  return err;
}

int readFromConsole(double* (&A), double* (&b), unsigned int &n) {
  scanf("%u", &n);
  A = (double*)malloc(n * n * sizeof(double));
  b = (double*)malloc(n * sizeof(double));
  for (size_t i = 0; i < n; i++)
  for (size_t j = 0; j < n; j++) {
    if (scanf("%lf", &(A[at(i, j, n)])) <= 0) {
      return -1;
    }
  }
  for (size_t i = 0; i < n; i++) {
    if (scanf("%lf", &(b[i])) <= 0) {
      return -1;
    }
  }
  return 0;
}

void print(double* A, double* b, unsigned int n) {
  for (size_t i = 0; i < n; i++) {
  for (size_t j = 0; j < n; j++) {
    printf("%lf", A[at(i, j, n)]);
  }
    printf("\n");
  }
  for (size_t i = 0; i < n; i++) {
    printf("%lf", b[i]);
    printf("\n");
  }
}

void print(double* res, unsigned int n) {
  for (size_t i = 0; i < n; i++) {
    printf("%lf", res[i]);
    printf("\n");
  }
}



static inline void rotate(double* a, double* b, unsigned int n, size_t i, size_t j, size_t k) {
  double r = a[at(i, k, n)] * a[at(i, k, n)] + a[at(j, k, n)] * a[at(j, k, n)];
  double c = a[at(i, k, n)] / r;
  double s = a[at(j, k, n)] / r;

  size_t s1 = n / 4;
  size_t s2 = n / 4;
  size_t s3 = n / 4;
  size_t s4 = n - s1 - s2 - s3;
  
  tm.addTask(std::unique_ptr<Task>(new SumTask(c, s, -s, c, a + i * n, a + j * n, s1)));
  tm.addTask(std::unique_ptr<Task>(new SumTask(c, s, -s, c, a + i * n + s1, a + j * n + s1, s2)));
  tm.addTask(std::unique_ptr<Task>(new SumTask(c, s, -s, c, a + i * n + s1 + s2, a + j * n + s1 + s2, s3)));
  tm.addTask(std::unique_ptr<Task>(new SumTask(c, s, -s, c, a + i * n + s1 + s2 + s3, a + j * n + s1 + s2 + s3, s4)));

  a[at(j, k, n)] = 0;

  double mb = b[i];
  b[i] = c * mb + s * b[j];
  b[j] = - s * mb + c * b[j];
}

static inline void changeRaw(double* A, double* b, unsigned int n, size_t i, size_t j) {
  size_t s1 = n / 4;
  size_t s2 = n / 4;
  size_t s3 = n / 4;
  size_t s4 = n - s1 - s2 - s3;

  tm.addTask(std::unique_ptr<Task>(new ChangeTask(A + i * n,                A + j * n,                s1)));
  tm.addTask(std::unique_ptr<Task>(new ChangeTask(A + i * n + s1,           A + j * n + s1,           s2)));
  tm.addTask(std::unique_ptr<Task>(new ChangeTask(A + i * n + s1 + s2,      A + j * n + s1 + s2,      s3)));
  tm.addTask(std::unique_ptr<Task>(new ChangeTask(A + i * n + s1 + s2 + s3, A + j * n + s1 + s2 + s3, s4)));

  double x = b[i];
  double y = b[j];
  b[i] = y;
  b[j] = x;
}

static inline void findMax(double* A, double* b, unsigned int n, size_t col, size_t &first, size_t &second) {
  // n >= 2
  first = col;
  second = n + 1;
  for (size_t l = col; l < n; l++) {
    if (abs(A[at(first, col, n)]) < abs(A[at(l, col, n)])) {
      second = first;
      first = l;
      continue;
    }
    if (first != l) {
      if (second == n + 1) {
        second = l;
      }
      else if (abs(A[at(second, col, n)]) <= abs(A[at(l, col, n)])) {
        second = l;
      }
    }
  }
}

static inline void rotation(double* A, double* b, unsigned int n) {
  size_t i, j;
  double c = 0;
  for (size_t col = 0; col < n; col++) {
    do {
      findMax(A, b, n, col, i, j);
      if (j == n + 1) {
        break;
      }
      c = A[at(j, col, n)]; 
      tm.join();
      rotate(A, b, n, i, j, col);
      tm.join();
      changeRaw(A, b, n, i, col);
      tm.join();
    }
    while (c > 0);
  }
}

int solve(double* A, double* b, unsigned int n, double* (&res)) {
  res = (double*)malloc(n * sizeof(double));

  tm.start();
  rotation(A, b, n);
  tm.stop();

  print(A, b, n);
  for (int i = n - 1; i >= 0; i--) {
    res[i] = b[i];
    for (int j = n - 1; j > i; j--) {
      res[i] -= A[at(i, j, n)] * res[j];
    }
    if (fabs(A[at(i, i, n)]) <= 0) {
      return 1;
    }
    res[i] = res[i] / A[at(i, i, n)];
  }
  return 0;
}


