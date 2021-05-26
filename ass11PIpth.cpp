/*
 * compute pi by approximating the area under the curve
 * f(x) = 4 / (1 + x*x) between 0 and 1.
 *
 * command-line argument gives number of threads.
 *
 * compile:  g++ -Wall -pedantic -g -pthread -std=c++11 PIpth.cpp -o PIpth
 */

// --------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

void performPIsum(unsigned int, unsigned int);
bool getArgs(int, char *[], unsigned int &);


// --------------------------------------------------------
//  global variables
//  trained professional, do not attempt try at home

#define NUM_STEPS 100000000
#define MIN_THREAD_COUNT 1
#define MAX_THREAD_COUNT 8192

int		numThreads;
double		sum, step;

mutex		sumMutex;


// --------------------------------------------------------
//  main program

int main(int argc, char *argv[])
{
	thread		*thdList;
	double		pi;
	const double	PIREF = 3.141592653589793238462643;
	double		err;
	unsigned int	threadCount=0;

	numThreads = 0;
	sum = 0.0;
	step = 1.0 / static_cast<double>(NUM_STEPS);


	if (!getArgs(argc, argv, threadCount))
		return 0;

	unsigned long hwthd = thread::hardware_concurrency();
	cout << "Hardware Cores: " << hwthd << endl;
	cout << "Thread Count: " << threadCount << endl;


	// create thread array
	thdList = new thread[threadCount];

	// start threads
	for(unsigned int i=0; i<threadCount; i++)
		thdList[i] = thread(performPIsum, i, threadCount);

	// wait for threads to finish
	for(unsigned int i=0; i<threadCount; i++)
		thdList[i].join();

	/* finish computation */
	pi = step * sum;
	err = fabs(pi - PIREF);

	/* print results */
	cout << "P-threads program results with " << threadCount
		 << " threads:" << endl;
	cout << "  pi is approximately " << fixed << showpoint <<
		setprecision(16) << pi << endl;
	cout << "  error is " << fixed << showpoint <<
		setprecision(16) << err << endl;

	return 0;
}

// --------------------------------------------------------
//  thread function
//	code to be executed by each thread

void performPIsum(unsigned int strt, unsigned int threadCount)
{
	double	x;
	double	part_sum = 0.0;

	// do this thread's part of computation
	for (unsigned int i=strt; i < NUM_STEPS; i+=threadCount) {
		x = (static_cast<double>(i + 0.5)) * step;
		part_sum += 4.0 / (1.0 + x*x);
	}

	lock_guard<std::mutex> lock2(sumMutex);
	sum += part_sum;

}

// ----------------------------------------------------------------------
//  Deal with command line arguments.
//	get/check thread count
//	get/check prime limit

//  Required format:
//	./PIthd -t <thereadCount>

bool getArgs(int argc, char *argv[], unsigned int &threadCnt)
{
	stringstream ss;

	if (argc == 1) {
		cout << "Usage: ./PIpth -t <thereadCount> " << endl;
		return	false;
	}

	if (argc != 3 || string(argv[1]) != "-t") {
		cout << "Error, command line arguments invalid." << endl;
		return	false;
	}

	if (string(argv[2]) != "") {
		ss << argv[2];
		ss >> threadCnt;
	}

	if (threadCnt < MIN_THREAD_COUNT || threadCnt > MAX_THREAD_COUNT) {
		cout << "Error, invalid thread count." << endl;
		return	false;
	}

	return	true;
}


