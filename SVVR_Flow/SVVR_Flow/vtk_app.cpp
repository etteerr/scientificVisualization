#include "stdinc.h"
#include <map>
#include "vtk_app.h"

int entry(std::map<std::string, std::string>& arguments) {

	/* Load data */
	unsigned int start, step, end;
	start = atoi(arguments["-s"].c_str());
	step = atoi(arguments["-j"].c_str());
	end = atoi(arguments["-e"].c_str());
	std::string filename = arguments["-f"];

	vtksys::SystemTools::ChangeDirectory(arguments["-d"]);

	for (unsigned int i = start; i < end; i += step) {
		char endfilename[4096];
		sprintf_s(endfilename, 4096, filename.c_str(), i);
		printf("loading %s...\n", endfilename);

		/* vtk load dataset*/
	}

	return 1;
}