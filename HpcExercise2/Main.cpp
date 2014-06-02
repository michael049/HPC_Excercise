#include "CL/cl.h"
#include <malloc.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#define MEM_SIZE (128)

using namespace std;

// list of error codes from "CL/cl.h"
string cl_errorstring(cl_int err) {
	switch (err) {
	case CL_SUCCESS:                          return string("Success");
	case CL_DEVICE_NOT_FOUND:                 return string("Device not found");
	case CL_DEVICE_NOT_AVAILABLE:             return string("Device not available");
	case CL_COMPILER_NOT_AVAILABLE:           return string("Compiler not available");
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:    return string("Memory object allocation failure");
	case CL_OUT_OF_RESOURCES:                 return string("Out of resources");
	case CL_OUT_OF_HOST_MEMORY:               return string("Out of host memory");
	case CL_PROFILING_INFO_NOT_AVAILABLE:     return string("Profiling information not available");
	case CL_MEM_COPY_OVERLAP:                 return string("Memory copy overlap");
	case CL_IMAGE_FORMAT_MISMATCH:            return string("Image format mismatch");
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:       return string("Image format not supported");
	case CL_BUILD_PROGRAM_FAILURE:            return string("Program build failure");
	case CL_MAP_FAILURE:                      return string("Map failure");
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:     return string("Misaligned sub buffer offset");
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return string("Exec status error for events in wait list");
	case CL_INVALID_VALUE:                    return string("Invalid value");
	case CL_INVALID_DEVICE_TYPE:              return string("Invalid device type");
	case CL_INVALID_PLATFORM:                 return string("Invalid platform");
	case CL_INVALID_DEVICE:                   return string("Invalid device");
	case CL_INVALID_CONTEXT:                  return string("Invalid context");
	case CL_INVALID_QUEUE_PROPERTIES:         return string("Invalid queue properties");
	case CL_INVALID_COMMAND_QUEUE:            return string("Invalid command queue");
	case CL_INVALID_HOST_PTR:                 return string("Invalid host pointer");
	case CL_INVALID_MEM_OBJECT:               return string("Invalid memory object");
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:  return string("Invalid image format descriptor");
	case CL_INVALID_IMAGE_SIZE:               return string("Invalid image size");
	case CL_INVALID_SAMPLER:                  return string("Invalid sampler");
	case CL_INVALID_BINARY:                   return string("Invalid binary");
	case CL_INVALID_BUILD_OPTIONS:            return string("Invalid build options");
	case CL_INVALID_PROGRAM:                  return string("Invalid program");
	case CL_INVALID_PROGRAM_EXECUTABLE:       return string("Invalid program executable");
	case CL_INVALID_KERNEL_NAME:              return string("Invalid kernel name");
	case CL_INVALID_KERNEL_DEFINITION:        return string("Invalid kernel definition");
	case CL_INVALID_KERNEL:                   return string("Invalid kernel");
	case CL_INVALID_ARG_INDEX:                return string("Invalid argument index");
	case CL_INVALID_ARG_VALUE:                return string("Invalid argument value");
	case CL_INVALID_ARG_SIZE:                 return string("Invalid argument size");
	case CL_INVALID_KERNEL_ARGS:              return string("Invalid kernel arguments");
	case CL_INVALID_WORK_DIMENSION:           return string("Invalid work dimension");
	case CL_INVALID_WORK_GROUP_SIZE:          return string("Invalid work group size");
	case CL_INVALID_WORK_ITEM_SIZE:           return string("Invalid work item size");
	case CL_INVALID_GLOBAL_OFFSET:            return string("Invalid global offset");
	case CL_INVALID_EVENT_WAIT_LIST:          return string("Invalid event wait list");
	case CL_INVALID_EVENT:                    return string("Invalid event");
	case CL_INVALID_OPERATION:                return string("Invalid operation");
	case CL_INVALID_GL_OBJECT:                return string("Invalid OpenGL object");
	case CL_INVALID_BUFFER_SIZE:              return string("Invalid buffer size");
	case CL_INVALID_MIP_LEVEL:                return string("Invalid mip-map level");
	case CL_INVALID_GLOBAL_WORK_SIZE:         return string("Invalid gloal work size");
	case CL_INVALID_PROPERTY:                 return string("Invalid property");
	default:                                  return string("Unknown error code");
	}
}

void handle_clerror(cl_int err) {
	if (err != CL_SUCCESS){
		cerr << "OpenCL Error: " << cl_errorstring(err) << string(".") << endl;
		//exit(EXIT_FAILURE);
	}
}

cl_int createDevice(cl_platform_id * platforms, cl_uint & numPlatforms, cl_context& myctx,  cl_device_id & deviceIds)
{
	cl_int error=0;
	clGetDeviceIDs (platforms[0],CL_DEVICE_TYPE_GPU,1, &deviceIds, &numPlatforms);
	cl_command_queue myqueue;
	myqueue = clCreateCommandQueue(myctx, deviceIds, 0, &error);
	return error;
}

cl_int createContext(cl_context& myctx, cl_device_id & deviceIds)
{
	cl_uint numPlatforms;
	cl_platform_id* platformIds;
	cl_uint numDevices;
	cl_int err;

	// query the number of platforms
	err = clGetPlatformIDs(0, NULL, &numPlatforms);
	/*shrCheckError(errcode, CL_SUCCESS); */

	// now get all the platform IDs
	cl_platform_id * platforms;
	platforms=(cl_platform_id*)alloca(sizeof(cl_platform_id)*numPlatforms);
	err = clGetPlatformIDs(numPlatforms,  platforms, NULL);
	/*shrCheckError(errcode, CL_SUCCESS);*/

	// set platform property - we j&ust pick the first one
	cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (int) platforms[0], 0};
	 myctx = clCreateContextFromType ( properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &err);
	 return createDevice(platforms,numPlatforms, myctx,   deviceIds) ;
}

string GetClSource(string filename)
{
	std::ifstream ifs(filename);
	std::string source( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
	size_t test = source.length();
	return source;
}

int main(int argc, char **argv){
	
	cl_context myctx;
	cl_device_id deviceIds;
	cl_int err;
	cl_command_queue command_queue = NULL;
	cl_mem memobjA = NULL;
	cl_mem memobjB = NULL;
	cl_mem memobjC = NULL;
	cl_mem memobjT = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	createContext(myctx,deviceIds);

	string sourceString = GetClSource("DeviceSource.cl");
	const char * src_c = sourceString.c_str();
	const size_t sourceSize = sourceString.length();

	/* Create Command Queue */
	command_queue = clCreateCommandQueue(myctx, deviceIds, 0, &err); 
		
	/* Create Memory Buffer */
	float A [] = { 0,0,0,0,0,0 };
	float B [] = { 1,2,3,4,5,6 };
	float * T = NULL;
	T = (float *) malloc(sizeof(float)*6*2);	
	int num = 6;

	memobjA = clCreateBuffer(myctx, CL_MEM_READ_WRITE, 6*sizeof(float), NULL, &err);
	memobjB = clCreateBuffer(myctx, CL_MEM_READ_WRITE, 6*sizeof(float), NULL, &err);
	memobjC = clCreateBuffer(myctx, CL_MEM_READ_WRITE, sizeof(int), NULL, &err);
	memobjT = clCreateBuffer(myctx, CL_MEM_READ_WRITE, 6*2*sizeof(float), NULL, &err);
 
	/* Copy input data to the memory buffer */
	err = clEnqueueWriteBuffer(command_queue, memobjA, CL_TRUE, 0, 6*sizeof(float), A, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(command_queue, memobjB, CL_TRUE, 0, 6*sizeof(float), B, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(command_queue, memobjT, CL_TRUE, 0, 6*2*sizeof(float), T, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(command_queue, memobjC, CL_TRUE, 0, sizeof(int), &num, 0, NULL, NULL);

	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(myctx, 1, &src_c,
		&sourceSize, &err);
 
	/* Build Kernel Program */
	err = clBuildProgram(program, 1, &deviceIds, NULL, NULL, NULL);

	if(err != CL_SUCCESS)
	{
		size_t len;
		clGetProgramBuildInfo(program, deviceIds, CL_PROGRAM_BUILD_LOG, NULL, NULL, &len);
		//allocate memory:
		char *log = new char[len]; 
		//then recall clGetProgramBuildInfo():
		clGetProgramBuildInfo(program, deviceIds, CL_PROGRAM_BUILD_LOG, len, log, NULL);
		printf(log);
		ofstream myfile;
		myfile.open ("log.txt");
		myfile << log;
	}

	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "scan", &err);

	/* Set OpenCL kernel arguments */
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobjA);
	err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobjB);
	err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&memobjC);
	err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memobjT);
 
	/* Set OpenCL Kernel Parameters */
	//err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
 
	/* Execute OpenCL Kernel */
	size_t global_item_size = 6;
    size_t local_item_size = 1;
    /* Execute OpenCL kernel as data parallel */
    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
    &global_item_size, &local_item_size, 0, NULL, NULL);
	//err = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
 
	/* Copy results from the memory buffer */
	err = clEnqueueReadBuffer(command_queue, memobjA, CL_TRUE, 0,
		6 * sizeof(float),A, 0, NULL, NULL);
 
	/* Display Result */
	for (int i = 0; i < 6; i++)
	{
		printf("%f ",A[i]);
		//printf("%f ",B[i]);
	}

	getchar();
}

void shrCheckError(cl_uint ErrCode)
{
	string error = cl_errorstring(ErrCode);
	cout << error;
}
