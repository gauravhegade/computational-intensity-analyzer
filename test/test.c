// even more highly complex function
#include <complex.h>
#include <math.h>

int complexFunction1(double complex c, int max_iter) {
  double complex z = 0 + 0 * I;
  int n = 0;
  while (cabs(z) <= 100 && n < max_iter) {
    z = z * z + c;
    n++;
  }
  return n;
}

void complexFunction2(int width, int height, int max_iter) {
  double complex min = -2.0 - 1.5 * I;
  double complex max = 1.0 + 1.5 * I;
  double dx = creal(max - min) / (width - 1);
  double dy = cimag(max - min) / (height - 1);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double complex c = min + x * dx + y * dy * I;
      int n = complexFunction1(c, max_iter);
      // Normally, the result 'n' would be used to color a pixel
      // For simplicity, we're not storing the result in this example
    }
  }
}

// complex_arithmetic operations example
void complexArithmetic(int n, float *results) {
  for (int i = 0; i < n; i++) {
    float x = (float)i / n;
    float y = x * x;
    y = y * (3.0f - y); // Polynomial transformation
    y = (y - 0.5f) * 8.0f;
    y = y * y * y;
    results[i] = y; // One memory operation per iteration
  }
}

// example from problem statement
void compute(float *data, int N) {
  for (int i = 0; i < N; ++i) {
    data[i] = sin(data[i]) + cos(data[i]);
  }
}

void initialize(float *data, int N) {
  for (int i = 0; i < N; ++i) {
    data[i] = 0.0f;
  }
}
