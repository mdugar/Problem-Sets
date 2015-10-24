#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <unistd.h>

#include "clhelp.h"

int main(int argc, char *argv[])
{
  std::string reduce_kernel_str;
  
  std::string reduce_name_str = 
    std::string("reduce");
  std::string reduce_kernel_file = 
    std::string("reduce.cl");

  cl_vars_t cv; 
  cl_kernel reduce;
  
  readFile(reduce_kernel_file,
	   reduce_kernel_str);
  
  initialize_ocl(cv);
  
  compile_ocl_program(reduce, cv, reduce_kernel_str.c_str(),
		      reduce_name_str.c_str());

  int *h_A, *h_Y;
  cl_mem g_Out, g_In;
  int n = (1<<24);

  int c;
  /* how long do you want your arrays? */
  while((c = getopt(argc, argv, "n:"))!=-1)
    {
      switch(c)
	{
	case 'n':
	  n = atoi(optarg);
	  break;
	}
    }
  if(n==0)
    return 0;

  int original_N = n;
  h_A = new int[n];
  h_Y = new int[n];

  for(int i = 0; i < n; i++)
    {
      h_A[i] = 1;
      h_Y[i] = 0;
    }

  cl_int err = CL_SUCCESS;
  g_Out = clCreateBuffer(cv.context,CL_MEM_READ_WRITE,
			 sizeof(int)*n,NULL,&err);
  CHK_ERR(err);  
  g_In = clCreateBuffer(cv.context,CL_MEM_READ_WRITE,
			sizeof(int)*n,NULL,&err);
  CHK_ERR(err);

  //copy data from host CPU to GPU
  err = clEnqueueWriteBuffer(cv.commands, g_Out, true, 0, sizeof(int)*n,
			     h_Y, 0, NULL, NULL);
  CHK_ERR(err);


  err = clEnqueueWriteBuffer(cv.commands, g_In, true, 0, sizeof(int)*n,
			     h_A, 0, NULL, NULL);
  CHK_ERR(err);


  double t0 = timestamp();

  /*
   * First iteration!
   */
  size_t local_work_size[1] = {512};
  size_t global_work_size[1] = {n};

  err = clSetKernelArg(reduce, 0, sizeof(cl_mem), &g_In);
  CHK_ERR(err);
  err = clSetKernelArg(reduce, 1, sizeof(cl_mem), &g_Out);
  CHK_ERR(err);
  err = clSetKernelArg(reduce, 2, 512 * sizeof(cl_int), NULL);
  CHK_ERR(err);
  err = clSetKernelArg(reduce, 3, sizeof(int), &n);
  CHK_ERR(err);  

  err = clEnqueueNDRangeKernel(cv.commands,
			       reduce,
			       1,
			       NULL,
			       global_work_size,
			       local_work_size,
			       0,
			       NULL,
			       NULL
			       );
  CHK_ERR(err);
  
 
  //These are the only arguments we need to change... no need to change every iteration
  int m;
  err = clSetKernelArg(reduce, 0, sizeof(cl_mem), &g_Out);
  CHK_ERR(err);  
  err = clSetKernelArg(reduce, 3, sizeof(int), &n);
  CHK_ERR(err);

  //Keep reducing by 512 until we're done
  for(m = n/512; m > 1; m /= 512) {
    global_work_size[0] = m;

    //Local size cannot be larger than global size
    if(m < 512)
      local_work_size[0] = m;
    err = clEnqueueNDRangeKernel(cv.commands,
				 reduce,
				 1,
				 NULL,
				 global_work_size,
				 local_work_size,
				 0,
				 NULL,
				 NULL
				 );
    CHK_ERR(err);
  }
  
  t0 = timestamp()-t0;
  
  
  //read result of GPU on host CPU
  err = clEnqueueReadBuffer(cv.commands, g_Out, true, 0, sizeof(int)*n,
			    h_Y, 0, NULL, NULL);
  CHK_ERR(err);

  int sum=0.0f;
  for(int i = 0; i < n; i++)
    {
      sum += h_A[i];
    }
  
  if(sum!=h_Y[0])
    {
      printf("WRONG: CPU sum = %d, GPU sum = %d\n", sum, h_Y[0]);
      printf("WRONG: difference = %d\n", sum-h_Y[0]);
    }
  else
    {
      printf("CORRECT: %d,%g\n",n,t0);
    }
 
  uninitialize_ocl(cv);
  
  delete [] h_A; 
  delete [] h_Y;
  
  clReleaseMemObject(g_Out); 
  clReleaseMemObject(g_In);
  
  return 0;
}
