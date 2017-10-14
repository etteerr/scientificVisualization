#include "stdinc.h"
#include <map>
#include "vtk_app.h"

unsigned int numData = 0;
vtkSmartPointer<vtkStructuredPointsReader> dataset[512];

int entry(std::map<std::string, std::string>& arguments) {

	/* Load data */
	unsigned int start, step, end;
	start = atoi(arguments["-s"].c_str());
	step = atoi(arguments["-j"].c_str());
	end = atoi(arguments["-e"].c_str());
	std::string filename = arguments["-f"];

	//vtksys::SystemTools::ChangeDirectory(arguments["-d"]);

	

	for (unsigned int i = start; i < end; i += step) {
		vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
		char endfilename[4096];
		sprintf_s(endfilename, 4096, filename.c_str(), i);
		
		/* Create full path*/
		std::string fn;
		fn.append(arguments["-d"]);
		fn.append(endfilename);
		//std::string fn =  vtksys::SystemTools::CollapseFullPath(endfilename);


		/* Verbosity */
		dprintf(dbglvl_NOTIFICATION, "loading %s...\n", fn.c_str());

		/* vtk load dataset*/
		reader->SetFileName(fn.c_str());
		reader->Update();

		/* Save to dataset */
		dataset[numData++] = reader;
	}

	/* Verbosity */
	dprintf(dbglvl_NOTIFICATION, "Done!\n");

	return 1;
}