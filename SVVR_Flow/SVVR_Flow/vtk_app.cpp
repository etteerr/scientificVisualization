#include "stdinc.h"
#include <map>
#include "vtk_app.h"

unsigned int numData = 0;
vtkSmartPointer<vtkStructuredPointsReader> dataset[512];

int entry(std::map<std::string, std::string>& arguments) {

	if (loadDatasets(arguments))
		return 1;

	return 0;
}

int loadDatasets(std::map<std::string, std::string> & arguments)
{
	/* Load data */
	unsigned int start, step, end;
	start = atoi(arguments["-s"].c_str());
	step = atoi(arguments["-j"].c_str());
	end = atoi(arguments["-e"].c_str());
	std::string filename = arguments["-f"];

	/* Load files specified by arguments */
	for (unsigned int i = start; i < end; i += step) {
		/* Create filename */
		char endfilename[4096];
		sprintf_s(endfilename, 4096, filename.c_str(), i);

		/* Create full path*/
		std::string fn; //filename (full path)
		fn.append(arguments["-d"]);
		fn.append(endfilename);

		/* Load pointset data */
		int retval = LoadPointsSet(fn);
		if (retval < 0) return 1;
	}

	return 0;
}


int LoadPointsSet(std::string &fn)
{
	/* Verbosity */
	dprintf(dbglvl_NOTIFICATION, "loading %s...\n", fn.c_str());

	/* Create reader*/
	vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();

	/* vtk load dataset*/
	try {
		reader->SetFileName(fn.c_str());
		reader->Update();
	}
	catch (std::exception e) {
		dprintf(dbglvl_ERROR, "Exception while loading %s!\n", fn.c_str());
		dprintf(dbglvl_ERROR, "Reason: %s\n", e.what());
		return -1;
	}

	/* Check if loading was successfull */
	if (reader->GetErrorCode() != NOERROR) {
		dprintf(dbglvl_ERROR, "Error while loading %s! (code: %i)\n", fn.c_str(), reader->GetErrorCode());
		dprintf(dbglvl_ERROR, "\tError: %s\n", vtkErrorCode::GetStringFromErrorCode(reader->GetErrorCode()));
		return -1;
	}

	/* Save to dataset */
	dataset[numData++] = reader;

	return numData-1;
}
