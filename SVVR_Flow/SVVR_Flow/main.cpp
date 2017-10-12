#include "stdinc.h"
#include "vtk_app.h"
#include <map>

std::map<std::string, std::string> margs;
std::map<std::string, std::string> args_info;

void parseArgs(int, char**, std::map<std::string, std::string>&, std::map<std::string, std::string> &);
void print_arg_info(std::map<std::string, std::string> &argi);

int main(int nargs, char ** args) {
	/* Set argument info  (Note: Not limited to these!) */
	args_info["-d"] = "Data directory";
	args_info["-f"] = "Filename format (file%u.vtk for range from -s to -e with -j steps)";
	args_info["-s"] = "The fileformat start number (%u)";
	args_info["-e"] = "The fileformat end   number (%u)";
	args_info["-j"] = "The fileformat step size (-s 1 -e 10 -j 3 => 1,4,7,10)";
	/* Set default arguments  (Note: Not limited to these!) */
	margs["-d"] = "D:\\VTK_FLOW_DATA\\";
	margs["-f"] = "carotid-bifurcation.%u.vtk";
	margs["-s"] = "7800";
	margs["-e"] = "8000";
	margs["-j"] = "20";

	/* Parse arguments */
	print_arg_info(args_info);
	parseArgs(nargs, args, margs, args_info);

	/* Call main application */
	int res = entry(margs);

	/* Halt execution till keypress */
	if (res) {
		printf("vtk_app exited with code %i\n", res);
		getchar();
	}
	return res;
}



void print_arg_info(std::map<std::string, std::string> &argi) {
	using namespace std;

	printf("Usage:\n");
	for (map<string, string>::iterator it = argi.begin(); it != argi.end(); it++) {
		printf("\t%s: %s\n", it->first.data(), it->second.data());
	}
}

void parseArgs(int nargs, char** args, std::map<std::string, std::string> &amap, std::map<std::string, std::string> &arg_info) {
	using namespace std;

	int i = 1;

	for (; i < nargs; i++) {
		char fchar = args[i][0];

		switch (fchar) {
		case '-':
		case '/':
			amap[string(args[i])] = string(args[++i]);
			break;
		default:
			amap["base"] = string(args[i]);
		}
	}

	/* be verbose */
	printf("Arguments:\n");
	for (map<string, string>::iterator it = amap.begin(); it != amap.end(); it++)
		printf("\t%s: %s\n", it->first.data(), it->second.data());
}