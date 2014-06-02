__kernel void scan(__global float *g_odata, __global float *g_idata, __global float *temp, __global int * num) 
{  
	//float temp[]; // allocated on invocation  
	int thid = get_global_id(0);  
	int offset = 1; 
	int n = *num;
	
	temp[2*thid] = g_idata[2*thid]; // load input into shared memory  
    temp[2*thid+1] = g_idata[2*thid+1];  

	for (int d = n>>1; d > 0; d >>= 1)                    // build sum in place up the tree  
    {   
		barrier(CLK_LOCAL_MEM_FENCE);  
		
		if (thid < d)  
		{  
			int ai = offset*(2*thid+1)-1;  
			int bi = offset*(2*thid+2)-1; 
			temp[bi] += temp[ai];  
		}  

		offset *= 2;  
		
		if (thid == 0) { temp[n - 1] = 0; } // clear the last element 
		
		for (int d = 1; d < n; d *= 2) // traverse down tree & build scan  
		{  
			offset >>= 1;  
			barrier(CLK_LOCAL_MEM_FENCE);  
			if (thid < d)                       
			{  
				int ai = offset*(2*thid+1)-1;  
				int bi = offset*(2*thid+2)-1; 
				float t = temp[ai];  
				temp[ai] = temp[bi];  
				temp[bi] += t;   
			}  
		}  
		
		barrier(CLK_LOCAL_MEM_FENCE);  
		g_odata[2*thid] = temp[2*thid]; // write results to device memory  
		g_odata[2*thid+1] = temp[2*thid+1];  
	}
} 

__kernel void NavierScan(__global float *g_odata, __global float *g_idata, __global float *temp, __global int * num) 
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