// Author: Markus Schordan, 2011.

#include <CL/cl.h>
#include <malloc.h>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>


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
		exit(EXIT_FAILURE);
	}
}

void print_separation_line(string s,int empty=0) {
	for(int i=empty;i<0;i++) {
		cout << endl;
	}
	int length=79;
	int n=length/s.size();
	for(int i=0; i<n; i++) {
		cout << s;
	}
	int rest=length%s.size();
	for(int i=0; i<rest; i++) {
		cout << s[i];
	}
	cout << endl;
	for(int i=0;i<empty;i++) {
		cout << endl;
	}
}

void print_name(string s, int fieldwidth) {
	cout.setf(ios::left); 
	cout.width(fieldwidth);
	cout<<s<<": ";
}
void print_pname(string s) {
	print_name(s,20);
}
void print_dname(string s) {
	print_name(s,31);
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
    struct stat statbuf;
    FILE        *fh;
    char        *source;

    fh = fopen(filename, "r");
    if (fh == 0)
        return 0;

    stat(filename, &statbuf);
    source = (char *) malloc(statbuf.st_size + 1);
    fread(source, statbuf.st_size, 1, fh);
    source[statbuf.st_size] = '\0';

    return source;
}
int main(int argc, char **argv)
{
	cl_context myctx;
	cl_device_id deviceIds;
	createContext(myctx,deviceIds);
	// create the program
	char * filename = "rotation.cl";

	char *source = LoadProgramSourceFromFile(filename);
	cl_int err;
	cl_program myprog = clCreateProgramWithSource ( myctx,1, (const char **) & source, NULL, &err);
	// build the program
	cl_int ciErrNum = clBuildProgram( myprog, 0,NULL, NULL, NULL, NULL);
	//Use the “image_rotate” function as the kernel
	cl_kernel mykernel = clCreateKernel (myprog , "image_rotate", &err);
}

void shrCheckError(cl_uint ErrCode)
{
	string error = cl_errorstring(ErrCode);
	cout << error;
}
