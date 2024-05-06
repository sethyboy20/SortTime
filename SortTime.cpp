#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <Windows.h>
#include <stdio.h>
#include <intrin.h>
#include <time.h>
#pragma comment(lib, "user32.lib")
using namespace std;

double BubbleSort(vector<int>& set, int n) {
	time_t start, end;
	double timeComp;
	start = clock();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (set[j] > set[j + 1]) {
				int temp = set[j];
				set[j] = set[j + 1];
				set[j + 1] = temp;
			}
		}
	}
	end = clock();
	timeComp = (difftime(end, start) / CLOCKS_PER_SEC);
	return timeComp;
}

double InsertionSort(vector<int>& set, int n) {
	time_t start, end;
	double timeComp;
	start = clock();
	for (int i = 1; i < n; i++) {
		int j = i;
		while (j > 0 && set[j - 1] > set[j]) {
			int temp = set[j];
			set[j] = set[j - 1];
			set[j - 1] = temp;
			j--;
		}
	}
	end = clock();
	timeComp = (difftime(end, start) / CLOCKS_PER_SEC);
	return timeComp;
}


double GetCPUSpeed()
{
	// From https://stackoverflow.com/questions/850774/how-to-determine-the-hardware-cpu-and-ram-on-a-machine
	wchar_t Buffer[_MAX_PATH];
	DWORD BufSize = _MAX_PATH;
	DWORD dwMHz = _MAX_PATH;
	HKEY hKey;

	// open the key where the proc speed is hidden:
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0,
		KEY_READ,
		&hKey);
	if (lError != ERROR_SUCCESS)
	{// if the key is not found, tell the user why:
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			lError,
			0,
			Buffer,
			_MAX_PATH,
			0);
		wprintf(Buffer);
		return 0;
	}

	// query the key:
	RegQueryValueEx(hKey, L"~MHz", NULL, NULL, (LPBYTE)&dwMHz, &BufSize);
	return (double)dwMHz;
}

int main(int argc, const char** argv) {
	try {
		// Check if there is 1 string entered
		if (argc != 2) {
			throw runtime_error("Wrong number of arguments");
		}

		// Read the filename from argument
		string filename = argv[1];

		if (filename.size() > 4 &&
			filename.substr(filename.size() - 4, filename.size()) != ".txt") {
			throw runtime_error("Invalid extension");
		}

		ifstream inFile;
		inFile.open(filename);

		vector<int> elements;

		// Read data file
		if (inFile.is_open()) {
			string currLine;
			while (getline(inFile, currLine)) {
				elements.push_back(stoi(currLine));
			}
			cout << filename << " read" << endl;
		}
		else {
			cout << "Could not open " << filename << endl;
		}
		inFile.close();

		ofstream outFile("output.txt");

		// Output results to file
		if (outFile.is_open()) {
			SYSTEM_INFO siSysInfo;
			GetNativeSystemInfo(&siSysInfo);

			outFile << "CPU Stats:" << endl;
			outFile << "\tCPU Architecture: ";
			switch (siSysInfo.wProcessorArchitecture) {
				case 0xffff:
					outFile << setw(11) << "Unknown";
					break;
				case 0:
				case 5:
					outFile << setw(10) << "32-bit";
					break;
				case 6:
				case 9:
				case 12:
					outFile << setw(10) << "64-bit";
					break;
			}
			outFile << endl;

			int width = (int)to_string(siSysInfo.dwNumberOfProcessors).size();
			outFile << "\tCPU Logic Cores: ";
			outFile << setw(5 + width) << siSysInfo.dwNumberOfProcessors << " Cores";
			outFile << endl;

			outFile << "\tCPU Frequency: ";
			outFile << setw(10) << (floor((GetCPUSpeed() / 1000) * 10 + 0.5) / 10) << " GHz";
			outFile << endl << endl;
			outFile << "Time Complexities:" << endl;
			outFile << "\tBubble Sort Time Complexity: " << setw(10) << "O(n^2)" << endl;
			outFile << "\tInsertion Sort Time Complexity: " << setw(10) << "O(n^2)" << endl;
			outFile << endl << endl;

			for (int power = 1; power <= 5; power++) {
				cout << "Running for n = 10^" << power << "..." << endl;
				outFile << "========== 10e" << power << " ==========" << endl;
				int n = pow(10, power);
				outFile << "\tn: " << n << endl;
				outFile << endl;

				vector<int> setSort;

				for (int i = 0; i < n; i++) {
					setSort.push_back(elements[i]);
				}

				double bubbleTC = BubbleSort(setSort, n);

				outFile << "\tBubble Sort:" << endl;
				outFile << "\t\tAlgorithm Time: ";
				outFile << setprecision(2) << scientific << bubbleTC << " seconds" << endl;
				if (power == 1) {
					outFile << "\t\tFirst Ten: " << endl;
					for (int i = 0; i < n; i++) {
						outFile << "\t\t\t(" << (i + 1) << ")";
						outFile << setw(10) << setSort[i] << endl;
					}
				}
				outFile << endl;

				setSort.clear();

				for (int i = 0; i < n; i++) {
					setSort.push_back(elements[i]);
				}

				double insertTC = InsertionSort(setSort, n);

				outFile << "\tInsertion Sort:" << endl;
				outFile << "\t\tAlgorithm Time: ";
				outFile << setprecision(2) << scientific << insertTC << " seconds" << endl;
				if (power == 1) {
					outFile << "\t\tFirst Ten: " << endl;
					for (int i = 0; i < n; i++) {
						outFile << "\t\t\t(" << (i + 1) << ")";
						outFile << setw(10) << setSort[i] << endl;
					}
				}
				outFile << endl;

				setSort.clear();
			}
			outFile << "==========================" << endl;

			cout << "Results written to output.txt" << endl;
		}
		else {
			cout << "Could not write to file" << endl;
		}
	}
	catch (runtime_error& excpt) {
		cout << "ERROR: " << excpt.what() << endl;
		cout << "Please enter a filename to read with extension .txt (ex: sample.txt)" << endl;
	}
}