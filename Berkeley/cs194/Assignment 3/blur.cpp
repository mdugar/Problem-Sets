#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include <emmintrin.h>
#include "pmmintrin.h"

using namespace std;

double timestamp()
{
  struct timeval tv;
  gettimeofday (&tv, 0);
  return tv.tv_sec + 1e-6*tv.tv_usec;
}

// Simple Blur
void simple_blur(float* out, int n, float* frame, int* radii){
  for(int r=0; r<n; r++)
    for(int c=0; c<n; c++){
      int rd = radii[r*n+c];
      int num = 0;
      float avg = 0;
      for(int r2=max(0,r-rd); r2<=min(n-1, r+rd); r2++)
        for(int c2=max(0, c-rd); c2<=min(n-1, c+rd); c2++){
          avg += frame[r2*n+c2];
          num++;
        }
      out[r*n+c] = avg/num;
    }
}

// My Blur
void my_blur(float* out, int n, float* frame, int* radii){
  int r2, c2;
  
  for(int r=0; r<n; r++)
    for(int c=0; c<n; c++){
      int rd = radii[r*n+c];

      __m128 sum0 = _mm_setzero_ps();

      int iMin = max(0,r-rd);
      int jMin = max(0, c-rd);
      int iMax = min(n, r+rd+1);
      int jMax = min(n, c+rd+1);

      float avg = 0;
      
      for(r2=iMin; r2<iMax; r2++) {
        for(c2=jMin; c2<jMax; c2+=4) {
	  sum0 = _mm_add_ps(sum0, _mm_load_ps(frame + r2*n + c2 + 0));
	  _mm_prefetch((__m128i*)(frame+ r2*n + c2 + 4), _MM_HINT_NTA);
        }
        for(; c2<=min(n-1, c+rd); c2++) {
	  sum0 = _mm_add_ss(sum0, _mm_load_ss(frame + r2*n + c2));
	  _mm_prefetch((__m128i*)(frame+ r2*n + c2 + 1), _MM_HINT_NTA);	  
        }		
      }

      sum0 = _mm_hadd_ps(sum0, sum0);
      sum0 = _mm_hadd_ps(sum0, sum0);

      out[r*n+c] = (_mm_cvtss_f32(sum0))/(jMax-jMin);
    }
}

int main(int argc, char *argv[])
{
  //Generate random radii
  srand(0);
  int n = 3000;
  int* radii = new int[n*n];
  for(int i=0; i<n*n; i++)
    radii[i] = 6*i/(n*n) + rand()%6;

  //Generate random frame
  float* frame = new float[n*n];
  for(int i=0; i<n*n; i++)
    frame[i] = rand()%256;

  //Blur using simple blur
  float* out = new float[n*n];
  double time = timestamp();
  simple_blur(out, n, frame, radii);
  time = timestamp() - time;

  //Blur using your blur
  float* out2 = new float[n*n];
  double time2 = timestamp();
  my_blur(out2, n, frame, radii);
  time2 = timestamp() - time2;

  //Check result
  for(int i=0; i<n; i++)
    for(int j=0; j<n; j++){
      float dif = out[i*n+j] - out2[i*n+j];
      if(dif*dif>1.0f){
        printf("Your blur does not give the right result!\n");
        printf("For element (row, column) = (%d, %d):\n", i, j);
        printf("  Simple blur gives %.2f\n", out[i*n+j]);
        printf("  Your blur gives %.2f\n", out2[i*n+j]);
        exit(-1);
      }
  }

  //Delete
  delete[] radii;
  delete[] frame;
  delete[] out;
  delete[] out2;

  //Print out Time
  printf("Time needed for naive blur = %.3f seconds.\n", time);
  printf("Time needed for your blur = %.3f seconds.\n", time2);
}