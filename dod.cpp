#include <iostream>
#include <tuple>
#include <chrono>
#include <string>
#include <vector>
#include <array>

// define USE_STATIC preprocessor macro to use a static array to contain a point's components (instead of a dynamic array)
// when USE_STATIC is defined, its value (which should be a number), is used to determine the number of components a point will have

// By the way, this is how you use msvc compiler to compile this file with USE_STATIC defined to 100:
//	cl /DUSE_STATIC=100 /EHsc /O2 dod.cpp
// /Ehsc enables exception handling, /O2 does max optimization

// And here is how you do it with g++
// g++ -DUSE_STATIC=100 -O2 dod.cpp -o dod.exe

#ifdef USE_STATIC
const int NUM_COMPONENTS_STATIC = USE_STATIC; // if USE_STATIC, this is the number of components a point will have
#endif

const int INNER_REPEAT = 1; // how many times to repeat the inner loop (to make benchmarking more compute bound)

struct Answer {
	std::vector<double> totals; // totals[0] = sum of all points' first component, totals[1] = sum of all points' second component, etc.
	long long durationUs; // how long it took to calculate the totals (in microseconds)
};

struct Point
{
#ifdef USE_STATIC
	std::array<double,NUM_COMPONENTS_STATIC> components;
#else
	std::vector<double> components;
#endif
};

// Calculate component wise sum, given a struct of arrays
Answer do_soa(const std::vector<std::vector<double>>& components)
{
	int numPoints = components[0].size();
	int numComponents = components.size();
	Answer answer;
	answer.totals.resize(numComponents, 0);

	auto start = std::chrono::high_resolution_clock::now();
	
	for (size_t c = 0; c < numComponents; c++)
	{
		for (size_t repeat = 0; repeat < INNER_REPEAT; repeat++)
		{
			for (size_t p = 0; p < numPoints; p++)
			{
				answer.totals[c] += components[c][p];
			}
		}
		
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

	answer.durationUs = duration;
	return answer;
}

// Calculate component wise sum, given an array of structs
Answer do_aos(const std::vector<Point>& points)
{
	int numComponents = points[0].components.size();
	int numPoints = points.size();
	Answer answer;
	answer.totals.resize(numComponents, 0);

	auto start = std::chrono::high_resolution_clock::now();

	for (size_t c = 0; c < numComponents; c++)
	{
		for (size_t repeat = 0; repeat < INNER_REPEAT; repeat++)
		{
			for (size_t p = 0; p < numPoints; p++)
			{
				answer.totals[c] += points[p].components[c];
			}
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

	answer.durationUs = duration;
	return answer;
}

int main(int argc, char* argv[])
{
	// if -h or --help, print usage
#ifdef USE_STATIC
	std::string usage = "Usage: " + std::string(argv[0]) + " <num points>" + " (num components is compiled to be " + std::to_string(NUM_COMPONENTS_STATIC) + ")";
#else
	std::string usage = "Usage: " + std::string(argv[0]) + " <num points> <num components>";
#endif
	if (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help"))
	{
		std::cout << usage << std::endl;
		return 0;
	}

#ifdef USE_STATIC
	int numArgsRequired = 2;
#else
	int numArgsRequired = 3;
#endif

	// improper usage
	if (argc != numArgsRequired)
	{
		std::cout << "incorrect usage" << std::endl;
		std::cout << usage << std::endl;
		return 1;
	}

	// get args
	int numPoints = std::stoi(std::string(argv[1]));
#ifdef USE_STATIC
	int numComponents = NUM_COMPONENTS_STATIC;
#else
	int numComponents = std::stoi(std::string(argv[2]));
#endif

	// struct of arrays
	std::vector<std::vector<double>> soa;
	for (size_t c = 0; c < numComponents; c++)
	{
		std::vector<double> values;
		values.resize(numPoints);
		for (size_t c = 0; c < numPoints; c++)
		{
			values[c] = 1;
		}
		soa.push_back(values);
	}

	// array of structs
	std::vector<Point> aos;
	for (size_t c = 0; c < numPoints; c++)
	{
		Point v;
#ifndef USE_STATIC
		v.components.resize(numComponents);
#endif
		for (size_t p = 0; p < numComponents; p++)
		{
			v.components[p] = 1;
		}
		aos.push_back(v);
	}

	// do soa
	Answer soaAnswer = do_soa(soa);
	Answer aosAnswer = do_aos(aos);

	// print results
	std::cout << "soa: " << soaAnswer.durationUs << "us" << std::endl;
	std::cout << "aos: " << aosAnswer.durationUs << "us" << std::endl;

	// print soa and aos results side by side (for debugging)
	//for (size_t c = 0; c < soaAnswer.totals.size(); c++)
	//{
	//	std::cout << soaAnswer.totals[c] << " " << aosAnswer.totals[c] << std::endl;
	//}

	return 0;
}
