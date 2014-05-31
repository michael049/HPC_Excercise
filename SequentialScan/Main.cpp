#include <malloc.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char **argv){

	std::vector<int> input;
	std::vector<int> output;
	
	for (int i = 0; i < 20; i++)
	{
		input.push_back(i+1);
		printf("%i,",input[i]); 
	}
	printf("\n"); 

	output.push_back(0);
	for (int i = 1; i < input.size(); i++)
	{
		output.push_back(output[i-1]+input[i-1]);
	}

	for (int i = 0; i < output.size(); i++)
	{
		printf("%i,",output[i]); 
	}

	int x;
	cin >> x;
}

