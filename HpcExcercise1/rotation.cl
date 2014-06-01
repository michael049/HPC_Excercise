
__kernel void image_rotate(
	__global float * src_data, __global float * dest_data, //Data in global memory
	int W, int H, //Image Dimensions
	float sinTheta, float cosTheta ) //Rotation Parameters
{
	//Thread gets its index within index space
	const int ix = get_global_id(0);
	const int iy = get_global_id(1);
	//Calculate location of data to move into ix and iy– Output decomposition as mentioned
	float xpos = ( ((float) ix)*cosTheta + ((float)iy )*sinTheta);
	float ypos = ( ((float) iy)*cosTheta - ((float)ix)*sinTheta);
	if (( ((int)xpos>=0) && ((int)xpos< W))) //Bound Checking
		&& (((int)ypos>=0) && ((int)ypos< H)))
	{
		//Read (xpos,ypos) src_data and store at (ix,iy) in dest_data
		dest_data[iy*W+ix]=
			src_data[(int)(floor(ypos*W+xpos))];
	}
}
