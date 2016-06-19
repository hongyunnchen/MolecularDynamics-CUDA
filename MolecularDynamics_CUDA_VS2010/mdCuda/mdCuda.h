#ifndef _MDCUDAH_
#define _MDCUDAH_

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include <math.h>
#include <Windows.h>

using namespace std;

const int MAX_ATOMS = 5000;

// Main functions
bool OpenFiles();
bool Input();
bool SolveCpu();
bool SolveCuda(int kernelStepCount);
void ForceCpu();
void LaunchCudaKernel(int numberOfSteps);
void Period(int i, int j, double &XIJ, double &YIJ, double &ZIJ, double &RIJ2, double &RIJ);
void FindBoundaries();
bool FileExists(const string& filename);
void WriteBsFile();
bool CloseFiles();
void ShowStatus();
void PrintInitial();
void PrintPeriodic();
void PrintFinal();
void PrintCoordinatesForcesEnergy();
void WritePicFile();
string GetTime();
void PrintElapsedTime();
double Randum(double U, double S);
void MaxWell();
void Reposition();
double Distance(int i, int j);
void SortAtoms(char sortAxis);
void GenerateLatis();

// Helper functions
bool CheckParameters(int argc, char* argv[]);
string GetValueStr();
int GetValueInt();
double GetValueDouble();
float GetValueFloat();
string SkipSpace();
string ReadLine();


// GLOBAL VARIABLES
string Title;	// First line of the input file
int MDSL;		// MD-STEPS
int IAVL;		// ROLLING AVERAGE STEPS
int IPPL;		// PERIODIC PRINTING STEPS
int ISCAL;		// SCALING STEP FOR TEMPERATURE
int IPD;		// PRINT CONTROL PARAMETER, IF 0 NO PRINT FOR DISTANCES
double TE;		// TEMPERATURE (IN KELVIN)
int NA;			// NUMBER OF MOVING PARTICLES
int NN;			// NUMBER OF TOTAL ATOMS
int IPBC;		// PBC PARAMETER, IF 0 NO PBC IS APPLIED
int LAYER;		// NUMBER OF LINES IN COORDINATE GENERATIONS
bool OnlyCpu;   // If true, the GPU is not used in calculations

// Periodic Boundary Properties
double PP[3];	// PERIODIC BOUNDARY LENGTHS
double PA[3];   // Lowest coordinates in all three axis
double PB[3];	// Highest coordinates in all three axis (PA[i]+PP[i])
double PL[3];	// Periodic boundary medium (PP / 2)

// Coordinates of atoms
double X[MAX_ATOMS];
double Y[MAX_ATOMS];
double Z[MAX_ATOMS];

// Force arrays
double FF[MAX_ATOMS][3];
double EE[MAX_ATOMS];

// Velocity arrays
double VV[MAX_ATOMS][3];
double VT[MAX_ATOMS];

// Energy variables
double EPOT,EKIN,ETOT,TEMP,SCFAC,TCALC,EKINA,TCALAV;
int MDS;				// Current md simulation step;

// Potential parameters
double RM;
double DT;
double A1;
double A2;
double RL1;
double RL2;
double AL1;
double AL2;
double D21;
double D22;
double BK;

// Files
ifstream fileInp;
ofstream fileOut, fileEne, filePic, fileBs;

// Timers
clock_t tStart, tStop;

// Program parameters
bool PSilent;

// Cuda Kernel
__global__ void mdKernel(int NA, float* FFX, float* FFY, float* FFZ, float* EE,	float* X, float* Y, float* Z, int IPBC, 
	float PP0, float PP1, float PP2, float AL1, float AL2, float A1, float A2, float RL1, float RL2, float D21, float D22, 
	float PA0, float PA1, float PA2, float PB0, float PB1, float PB2, float PL0, float PL1, float PL2);

// Cuda pointers
float *d_FFX;
float *d_FFY;
float *d_FFZ;
float *h_FFX,*h_FFY,*h_FFZ;
float *d_EE;
float *d_X, *d_Y, *d_Z;
float *h_X, *h_Y, *h_Z;

// Cuda textures
texture<int2> texX, texY, texZ;

// Cuda helper functions
#define CuErr(err)  CheckCudaErrors(err, __FILE__, __LINE__)
#define CuErr2(err)  CheckCudaErrors2(err, __FILE__, __LINE__)
void CheckCudaErrors(cudaError err, const char *file, const int line)
{
	if(err != cudaSuccess){
		cout << "Line " << line << " - cuda error: " << cudaGetErrorString(err) << endl << "Press enter to exit." << endl;
		string str;
		getline(cin, str);
		exit(-1);
	}
}

void CuErrC(string errorString)
{
	cudaError_t err = cudaGetLastError();
	if(err != cudaSuccess) {
		cout << "Cuda error: " << errorString << endl;
		string str;
		getline(cin, str);
		exit(-1);
	}
}

// Struct for coordinates
typedef struct
{
	double x, y, z;
} point;

#endif