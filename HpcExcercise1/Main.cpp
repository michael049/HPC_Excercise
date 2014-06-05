// Author: Markus Schordan, 2011.

#include <CL/cl.h>
#include <malloc.h>
#include "tga.h"
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <math.h>

using namespace std;
using namespace tga;
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
		exit(EXIT_FAILURE);
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

static char * LoadProgramSourceFromFile(const char *filename)
{
    ifstream is (filename, ifstream::binary);
	if (!is){
		cout << "Could not open file" << filename << endl;
		exit(1);
	}
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);
        
	//allocate memory + 1 for '0' terminator
    char* buffer = (char*)malloc(sizeof(char)*(length+1));
    is.read (buffer,length);
    is.close();

	//terminate string with '0'
	buffer[length] = 0;
	return buffer;
}


void printErrorAndExit(cl_int errorId, const char* errorMessage){
		cout<<errorMessage<<endl;
		cout<<cl_errorstring(errorId)<<endl;
		exit(EXIT_FAILURE);
}

void exitOnError(cl_int errorId, const char* errorMessage){
	if (errorId != CL_SUCCESS){
		printErrorAndExit(errorId, errorMessage);
	}
}



cl_device_id selectDevice(cl_platform_id platformId, cl_uint deviceIndex){
	cl_device_id device_id;
	int err = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, deviceIndex, &device_id, NULL);
	exitOnError(err, "Error: Failed to create a device group!\n");
	return device_id;
}

cl_platform_id* GetPlatforms(){
	cl_int error;
	cl_uint numPlatforms;
	cl_platform_id* platformIds;
	error = clGetPlatformIDs(0, NULL, &numPlatforms);
	exitOnError(error, "Error Getting Number of Platforms");

	//allocate Platform Size
	platformIds=(cl_platform_id*)malloc(sizeof(cl_platform_id)*numPlatforms);

	//Get Platforms
	error=clGetPlatformIDs(numPlatforms, platformIds, NULL);
	exitOnError(error, "Error Getting Platforms");
	return platformIds;
}

cl_context createContext(cl_device_id deviceId){
	int err;
	cl_context context = clCreateContext(0, 1, &deviceId, NULL, NULL, &err);
	if (!context){
		printErrorAndExit(err, "Error: Failed to create a compute context!\n");
    }
	return context;
}

cl_command_queue createCommandQueue(cl_device_id deviceId, cl_context context){
	int err;
	cl_command_queue commandQueue = clCreateCommandQueue(context, deviceId, CL_QUEUE_PROFILING_ENABLE, &err);
    if (!commandQueue){
        printErrorAndExit(err, "Error: Failed to create a command commands!\n");
    }
	return commandQueue;
}

cl_mem createInputBufferAndCopyDataToIt(cl_context context, cl_command_queue commandQueue, const void* data, size_t memorySize){
	int err;

	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, memorySize, NULL, &err);    
    if (!inputBuffer){
        printErrorAndExit(err, "Error: Failed to allocate device memory!\n");
    }

	err = clEnqueueWriteBuffer(commandQueue, inputBuffer, CL_TRUE, 0, memorySize, data, 0, NULL, NULL);
	exitOnError(err, "Error: Failed to write to source array!\n");

	return inputBuffer;
}

cl_mem createInputOutputBufferAndCopyDataToIt(cl_context context, cl_command_queue commandQueue, const void* data, size_t memorySize){
	int err;

	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, memorySize, NULL, &err);    
    if (!inputBuffer){
        printErrorAndExit(err, "Error: Failed to allocate device memory!\n");
    }

	err = clEnqueueWriteBuffer(commandQueue, inputBuffer, CL_TRUE, 0, memorySize, data, 0, NULL, NULL);
	exitOnError(err, "Error: Failed to write to source array!\n");

	return inputBuffer;
}


cl_mem createOutputBuffer(cl_context context, size_t memorySize){
	int err;
    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, memorySize, NULL, &err);    
	if (!outputBuffer){
		printErrorAndExit(err, "Error: Failed to allocate device memory!\n");
    }
	return outputBuffer;
}

cl_program createAndBuildProgram(cl_context context, cl_device_id deviceId, const char* kernelSource){

	int err;
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &err);
    if (!program){
		printErrorAndExit(err, "Error: Failed to create compute program!\n");
    }
    
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
		printf(kernelSource);
        size_t len;
        char buffer[2048];        
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }
	return program;
}

cl_kernel createKernel(cl_program program, const char* kernelMethodName){
	int err;
    cl_kernel kernel = clCreateKernel(program, kernelMethodName, &err);
    if (!kernel || err != CL_SUCCESS){
		printErrorAndExit(err, "Error: Failed to create compute kernel!\n");
    }
	return kernel;
}

void printTotalTime(cl_event timer){
	cl_ulong startTime, endTime;
	clWaitForEvents(1, &timer);    
	clGetEventProfilingInfo(timer, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &startTime, NULL);
	clGetEventProfilingInfo(timer, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, NULL);
	double totalTime = (endTime - startTime)/1000000.0;
	cout << "Total Kernel Time: "<<totalTime<<"ms"<<endl;
}

int main(int argc, char **argv)
{
  int err;
	char* kernelSource = LoadProgramSourceFromFile("rotate_image.cl");
          
	cl_platform_id* platformIds = GetPlatforms();
    cl_device_id device_id = selectDevice(platformIds[1], 1);
	cl_context context = createContext(device_id);
	cl_command_queue commands = createCommandQueue(device_id, context);
	cl_program program = createAndBuildProgram(context, device_id, kernelSource);
	cl_kernel kernel = createKernel(program, "image_rotate");
        
    TGAImage image;
    LoadTGA(&image, "lenna.tga");
    unsigned long imageSize = image.imageData.size();
    vector<unsigned char> outputImageData(image.width * image.height * image.bpp);    
    
	cl_mem inputBuffer = createInputBufferAndCopyDataToIt(context, commands, &image.imageData[0], sizeof(unsigned char) * imageSize);
	cl_mem outputBuffer = createOutputBuffer(context, sizeof(unsigned char) * imageSize);

        
	float cost = cos((90) * (CL_M_PI/180));
	float sint = sin((90) * (CL_M_PI/180));
    
    err =  0;
    err =  clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&inputBuffer);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&outputBuffer);
    err = clSetKernelArg(kernel, 2, sizeof(cl_int), (void*)&image.width);
    err = clSetKernelArg(kernel, 3, sizeof(cl_int), (void*)&image.height);
    err = clSetKernelArg(kernel, 4, sizeof(float), (void*)&sint);
    err = clSetKernelArg(kernel, 5, sizeof(float), (void*)&cost);
    
	clFinish(commands); //ensure all previous commands have been executed
    size_t global[2] = {image.width, image.height};		
	cl_event event_timer;
	err = clEnqueueNDRangeKernel(commands, kernel, 2, 0, global, 0, 0, NULL, &event_timer);
	exitOnError(err, "Error: Failed to execute kernel!");
        
    // Read back the results from the device to verify the output
    err = clEnqueueReadBuffer( commands, outputBuffer, CL_TRUE, 0, sizeof(unsigned char) * imageSize, &outputImageData[0], 0, NULL, NULL );
    exitOnError(err, "Error: Failed to read output array!");
    

	image.imageData = outputImageData;    
    tga::saveTGA(image, "output.tga");
    
    cout<<"Size: "<<imageSize<<endl;
	printTotalTime(event_timer);
    return 0;
}

void shrCheckError(cl_uint ErrCode)
{
	string error = cl_errorstring(ErrCode);
	cout << error;
}
