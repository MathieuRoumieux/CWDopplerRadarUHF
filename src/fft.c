/* Fast Fourier Transform
 * Cooley-Tukey algorithm with 2-radix DFT
 */

/*MIT License

Copyright (c) 2019 Brendan Ashworth

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <complex.h>

#define PI 3.14159265358979323846

void fft_slow(int* x, double complex* X, unsigned int N) 
{
    unsigned int n, k;

    // Iterate through, allowing X_K = sum_N of the complex frequencies.
    for (k = 0; k < N; k++) {
        for (n = 0; n < N; n++) {
            X[k] += x[n] * cexp(-2 * PI * I * n * k / N);
        }
    }
}

void fft_radix2(int* x, double complex* X, unsigned int N, unsigned int s) 
{
    unsigned int k;
    double complex t;

    // At the lowest level pass through (delta T=0 means no phase).
    if (N == 1) 
    {
        X[0] = x[0];
        return;
    }

    // Cooley-Tukey: recursively split in two, then combine beneath.
    fft_radix2(x, X, N/2, 2*s);
    fft_radix2(x+s, X + N/2, N/2, 2*s);

    for (k = 0; k < N/2; k++) {
        t = X[k];
        X[k] = t + cexp(-2 * PI * I * k / N) * X[k + N/2];
        X[k + N/2] = t - cexp(-2 * PI * I * k / N) * X[k + N/2];
    }
}

void fft(int* x, double complex* X, unsigned int N) 
{
    fft_radix2(x, X, N, 1);
}
