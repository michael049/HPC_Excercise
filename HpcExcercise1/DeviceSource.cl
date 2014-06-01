__kernel void scan(__global float *g_odata, __global float *g_idata, __global float *temp, __global int * num) 
{  
	//float temp[]; // allocated on invocation  
	int thid = get_global_id(0);  
	int pout = 0, pin = 1;  
	int n = *num;
	// Load input into shared memory.  
	 // This is exclusive scan, so shift right by one  
	 // and set first element to 0  
	temp[pout*n+thid] = (thid > 0) ? g_idata[thid-1] : 0;  
	barrier(CLK_LOCAL_MEM_FENCE);  
	for (int offset = 1; offset < n; offset *= 2)  
	{  
	  pout = 1 - pout; // swap double buffer indices  
	  pin = 1 - pout;  
	  if (thid >= offset)  
		temp[pout*n+thid] = temp[pin*n+thid - offset] + temp[pin*n+thid];
	  else  
		temp[pout*n+thid] = temp[pin*n+thid];  
	  barrier(CLK_LOCAL_MEM_FENCE); 
	}  
	g_odata[thid] = temp[pout*n+thid]; // write output 
} 