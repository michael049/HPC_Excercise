
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

__kernel void
post_process( 	__read_only image2d_t inputImage, 
			__write_only image2d_t transformedImage,
			__global float3* transformationMatrix )
{
	size_t x =  get_global_id(0);
	size_t y =  get_global_id(1);
 
	int2 coords = {x,y};
	uint4 inputPixel = read_imageui( inputImage, sampler, coords );
 
	int2 transCoords = { coords.x*transformationMatrix[0].x + coords.y*transformationMatrix[0].y + transformationMatrix[0].z, coords.x*transformationMatrix[1].x + coords.y*transformationMatrix[1].y + transformationMatrix[1].z };
 
	write_imageui( transformedImage, transCoords, inputPixel );
}