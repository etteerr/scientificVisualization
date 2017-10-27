#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingOpenVR);


#include "stdinc.h"
#include <map>
#include "vtk_app.h"


unsigned int numData = 0;
unsigned int curData = 0;
unsigned int numPointes = 600;
unsigned int renderTech = 0;
double scale = 1.0;
double tresh = 0.0000001;
vtkSmartPointer<vtkStructuredPointsReader> dataset[512];
vtkSmartPointer<vtkMaskPoints> datasetRed[512];

vtkSmartPointer<vtkVectorNorm> datasetnorm[512];
vtkSmartPointer<vtkGlyph3D> svm[512];
vtkSmartPointer<vtkPolyDataMapper> vol[512];
vtkSmartPointer<vtkActor> volAct[512];

vtkSmartPointer<vtkActor> hhacts[512];
vtkSmartPointer<vtkHedgeHog> hh[512];
vtkSmartPointer<vtkPolyDataMapper> pdm[512];

vtkSmartPointer<vtkMarchingCubes> mc[512];

/* Renderer */
vtkSmartPointer<vtkOpenVRRenderer> ovr;

class ejdInteract : public vtkOpenVRRenderWindowInteractor {
public:
	vtkTypeMacro(ejdInteract, vtkOpenVRRenderWindowInteractor);
	static ejdInteract *New()
	{
		return new ejdInteract;
	}
	void DoOneEvent(vtkOpenVRRenderWindow *renWin, vtkRenderer *ren);
};




void initActorsHH() {
	//glCopyTexImage2D invalid operation
#pragma omp parallel for
	for (int i = 0; i < numData; i++) {
		
		datasetRed[i] = vtkSmartPointer<vtkMaskPoints>::New();
		datasetRed[i]->SetInputConnection(dataset[i]->GetOutputPort());
		datasetRed[i]->SetRandomMode(2);
		datasetRed[i]->SetMaximumNumberOfPoints(numPointes);

		/* glyph */
		datasetnorm[i] = vtkSmartPointer<vtkVectorNorm>::New();
		datasetnorm[i]->SetInputConnection(dataset[i]->GetOutputPort());
		datasetnorm[i]->SetNormalize(1);
		datasetnorm[i]->Update();
		if (i == 0) {
			scale = 10.0 / datasetnorm[i]->GetImageDataOutput()->GetScalarRange()[1];
			tresh = datasetnorm[i]->GetImageDataOutput()->GetScalarRange()[0] * 1.01; // Add 1% to min as threshold 
			if (tresh < 0.0000001)
				tresh = 0.0000001;
			dprintf(dbglvl_VERBOSE, "Range found with max %f, setting scale to %e\n", datasetnorm[i]->GetImageDataOutput()->GetScalarRange()[1], scale);
		}


		vtkSmartPointer<vtkArrowSource> arrow = vtkSmartPointer<vtkArrowSource>::New();

		svm[i] =  vtkSmartPointer<vtkGlyph3D>::New();
		svm[i]->SetSourceConnection(arrow->GetOutputPort());
		svm[i]->SetInputConnection(datasetRed[i]->GetOutputPort());
		svm[i]->SetScaleFactor(scale);
		svm[i]->SetScaleMode(VTK_SCALE_BY_VECTOR);
		svm[i]->SetColorModeToColorByScalar();
		svm[i]->Update();

		vol[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
		vol[i]->SetInputConnection(svm[i]->GetOutputPort());
		vol[i]->Update();

		volAct[i] = vtkSmartPointer<vtkActor>::New();
		volAct[i]->SetMapper(vol[i]);

		/* Reduced hh */
		//hh[i] = vtkSmartPointer<vtkHedgeHog>::New();
		//hh[i]->SetInputConnection(datasetRed[i]->GetOutputPort());
		//hh[i]->SetScaleFactor(scale);
		//hh[i]->Update();

		//pdm[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
		//pdm[i]->SetInputConnection(hh[i]->GetOutputPort());
		//pdm[i]->SetScalarVisibility(1);

		hhacts[i] = vtkSmartPointer<vtkActor>::New();
		//hhacts[i]->SetMapper(pdm[i]);

		/* Test */
		mc[i] =
			vtkSmartPointer<vtkMarchingCubes>::New();
		mc[i]->SetInputConnection(datasetnorm[i]->GetOutputPort());
		mc[i]->ComputeNormalsOn();
		mc[i]->ComputeGradientsOn();
		mc[i]->SetValue(0, tresh);  // second value acts as threshold


		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		vtkSmartPointer<vtkLookupTable> lut =
			vtkSmartPointer<vtkLookupTable>::New();

		lut->SetHueRange(60.0, 180.0); //yellow to blue through green
		lut->Build();

		mapper->SetInputConnection(mc[i]->GetOutputPort());
		mapper->SetLookupTable(lut);
		mapper->ScalarVisibilityOff();
		mapper->Update();
		hhacts[i]->SetMapper(mapper);
		//hhacts[i]->ForceTranslucentOn();
		//hhacts[i]->ForceOpaqueOn();
		hhacts[i]->PickableOff();
		hhacts[i]->GetProperty()->SetColor(1.0, 0.5, 0.2);
	}

	for (int i = 0; i < numData; i++) {
		mc[i]->SetValue(0, tresh);  // second value acts as threshold
		svm[i]->SetScaleFactor(scale);
	}
}

int entry(std::map<std::string, std::string>& arguments) {

	if (loadDatasets(arguments))
		return 1;

	dprintf(dbglvl_VERBOSE, "Loaded %u datasets.\n", numData);

	initActorsHH();

	ovr = vtkSmartPointer<vtkOpenVRRenderer>::New();
	vtkSmartPointer<vtkOpenVRRenderWindow> ovrw = vtkSmartPointer<vtkOpenVRRenderWindow>::New();
	vtkSmartPointer<ejdInteract> ovrwi = vtkSmartPointer<ejdInteract>::New();

	ovr->SetBackground(0.0, 0.0, 0.1);
	ovr->SetBackground2(0.0, 0.0, 0.4);
	ovr->SetGradientBackground(1);
	ovr->AddActor(hhacts[0]);
	ovr->AddActor(volAct[0]);
	ovrw->AddRenderer(ovr);
	ovrw->SetMultiSamples(0);
	ovrw->SetInteractor(ovrwi);

	ovrwi->Start();


	return 0;
}

int loadDatasets(std::map<std::string, std::string> & arguments)
{
	/* Load data */
	int start, step, end, error = 0;
	start = atoi(arguments["-s"].c_str());
	step = atoi(arguments["-j"].c_str());
	end = atoi(arguments["-e"].c_str());
	std::string filename = arguments["-f"];
	if (!step)
		step = INT_MAX-start-1;

	/* Load files specified by arguments */
	unsigned int ourNumbers = numData;
#pragma omp parallel for
	for (int i = start; i < end; i += step) {
		/* Create filename */
		char endfilename[4096];
		sprintf_s(endfilename, 4096, filename.c_str(), i);

		/* Create full path*/
		std::string fn; //filename (full path)
		fn.append(arguments["-d"]);
		fn.append(endfilename);

		/* Load pointset data */
		int retval = LoadPointsSet(fn, ourNumbers + (i - start) / step);
		/* Inc data */
#pragma omp critical
		numData++;
		/* Check errors */
		if (retval < 0) {
#pragma omp critical
			error = 1;
		}
	}

	/* Return -1 on error */
	if (error)
		return -1;

	return 0;
}


int LoadPointsSet(std::string &fn, unsigned int mynumber)
{
	/* Verbosity */
	dprintf(dbglvl_NOTIFICATION, "loading %s to dataset[%u]...\n", fn.c_str(), mynumber);

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
	if (reader->GetErrorCode() != 0) {
		dprintf(dbglvl_ERROR, "Error while loading %s! (code: %i)\n", fn.c_str(), reader->GetErrorCode());
		dprintf(dbglvl_ERROR, "\tError: %s\n", vtkErrorCode::GetStringFromErrorCode(reader->GetErrorCode()));
		return -1;
	}

	/* Save to dataset */
	dataset[mynumber] = reader;

	return 0;
}

void ejdInteract::DoOneEvent(vtkOpenVRRenderWindow * renWin, vtkRenderer * ren)
{
	if (!renWin || !ren)
	{
		return;
	}
	vr::IVRSystem *pHMD = renWin->GetHMD();

	if (!pHMD)
	{
		// try rendering to create the HMD connection
		renWin->Render();
		return;
	}

	vr::VREvent_t event;
	vtkOpenVROverlay *ovl = renWin->GetDashboardOverlay();
	bool result = false;


	/* Set ori*/
	hhacts[curData]->SetPosition(volAct[curData]->GetPosition());
	hhacts[curData]->SetOrientation(volAct[curData]->GetOrientation());



	if (vr::VROverlay() &&
		vr::VROverlay()->IsOverlayVisible(ovl->GetOverlayHandle()))
	{
		result = vr::VROverlay()->PollNextOverlayEvent(
			ovl->GetOverlayHandle(),
			&event, sizeof(vr::VREvent_t));

		if (result)
		{
			int height = ovl->GetOverlayTexture()->GetHeight();
			switch (event.eventType)
			{
			case vr::VREvent_MouseButtonDown:
			{
				if (event.data.mouse.button == vr::VRMouseButton_Left)
				{
					ovl->MouseButtonPress(event.data.mouse.x, height - event.data.mouse.y - 1);
				}
			}
			break;

			case vr::VREvent_MouseButtonUp:
			{
				if (event.data.mouse.button == vr::VRMouseButton_Left)
				{
					ovl->MouseButtonRelease(event.data.mouse.x, height - event.data.mouse.y - 1);
				}
			}
			break;

			case vr::VREvent_MouseMove:
			{
				ovl->MouseMoved(event.data.mouse.x, height - event.data.mouse.y - 1);
			}
			break;

			case vr::VREvent_OverlayShown:
			{
				renWin->RenderOverlay();
			}
			break;

			case vr::VREvent_Quit:
				this->Done = true;
				break;
			}
		}
	}
	else
	{
		result = pHMD->PollNextEvent(&event, sizeof(vr::VREvent_t));

		if (result)
		{
			vr::TrackedDeviceIndex_t tdi = event.trackedDeviceIndex;

			// is it a controller button action?
			if (pHMD->GetTrackedDeviceClass(tdi) ==
				vr::ETrackedDeviceClass::TrackedDeviceClass_Controller &&
				(event.eventType == vr::VREvent_ButtonPress ||
					event.eventType == vr::VREvent_ButtonUnpress))
			{
				vr::ETrackedControllerRole role = pHMD->GetControllerRoleForTrackedDeviceIndex(tdi);

				this->UpdateTouchPadPosition(pHMD, tdi);

				// 0 = right hand 1 = left
				int pointerIndex =
					(role == vr::ETrackedControllerRole::TrackedControllerRole_RightHand ? 0 : 1);
				this->PointerIndexLookup[pointerIndex] = tdi;

				vr::TrackedDevicePose_t &tdPose = renWin->GetTrackedDevicePose(tdi);
				double pos[3];
				double ppos[3];
				double wxyz[4];
				this->ConvertPoseToWorldCoordinates(ren, tdPose, pos, wxyz, ppos);

				// so even though we have world coordinates we have to convert them to
				// screen coordinates because all of VTKs picking code is currently
				// based on screen coordinates
				ren->SetWorldPoint(pos[0], pos[1], pos[2], 1.0);
				ren->WorldToDisplay();
				double *displayCoords = ren->GetDisplayPoint();

				this->SetEventPosition(displayCoords[0], displayCoords[1], pointerIndex);
				this->SetWorldEventPosition(pos[0], pos[1], pos[2], pointerIndex);
				this->SetPhysicalEventPosition(ppos[0], ppos[1], ppos[2], pointerIndex);
				this->SetWorldEventOrientation(wxyz[0], wxyz[1], wxyz[2], wxyz[3], pointerIndex);
				this->SetPointerIndex(pointerIndex);

				if (event.eventType == vr::VREvent_ButtonPress)
				{
					if (pointerIndex) {
						dprintf(dbglvl_VERBOSE, "Left Button VR %u\n", event.data.controller.button);
					}
					else {
						dprintf(dbglvl_VERBOSE, "Right Button VR %u\n", event.data.controller.button);
					}
					if (event.data.controller.button == vr::EVRButtonId::k_EButton_Axis1)
					{
						/* Triggered :O */
						this->LeftButtonPressEvent();
						if (pointerIndex) {
							/* Left */
						}
						else {
							/* Right */
						}
					}
					if (event.data.controller.button == vr::EVRButtonId::k_EButton_Axis0)
					{
						/* The joy of thign */
						//this->RightButtonPressEvent();
						if (pointerIndex) {
							/* Left */
							(numPointes -= 500);
							
						}
						else {
							/* Right */
							(numPointes += 500);
						}
						for (unsigned int i = 0; i<numData; i++)
							datasetRed[i]->SetMaximumNumberOfPoints(numPointes);

					}
					static int pingpong = 1, pingdong = 0;
					if (event.data.controller.button == vr::EVRButtonId::k_EButton_Grip)
					{
						/* Squeeze */
						this->MiddleButtonPressEvent();
						if (pointerIndex) {
							/* Left */
							if (pingdong) {
								tresh *= 0.9;
							}
							else
								scale *= 0.9;
						}
						else {
							/* Right */
							if (pingdong) {
								//tresh++;
								tresh *= 1.1;
							}
							else
								scale *=1.1;
						}
						for (unsigned int i = 0; i < numData; i++) {
							//hh[i]->SetScaleFactor(scale);
							svm[i]->SetScaleFactor(scale);
							mc[i]->SetValue(0, tresh);  // second value acts as threshold
						}
					}
					if (event.data.controller.button == vr::EVRButtonId::k_EButton_ApplicationMenu)
					{
						/* B and Y*/
						//this->FourthButtonPressEvent();
						
						if (pointerIndex) {
							/* Left */
							//hhacts[curData]->SetPosition(volAct[curData]->GetPosition());
							//hhacts[curData]->SetOrientation(volAct[curData]->GetOrientation());
							if (!pingdong) ovr->RemoveActor(hhacts[curData]);
							//ovr->AddActor(hhacts[curData]);
							if (pingdong == 0) renderTech = 0;
							if (pingdong == 0) pingpong = 0;
							pingdong = 0;
						}
						else {
							/* Right */

							//volAct[curData]->SetPosition(hhacts[curData]->GetPosition());
							//volAct[curData]->SetOrientation(hhacts[curData]->GetOrientation());
							//ovr->RemoveActor(hhacts[curData]);
							if (pingpong)
								pingdong = 1;
							ovr->AddActor(hhacts[curData]);
							renderTech = 1;
							pingpong = 1;
						}
					}
					if (event.data.controller.button == 7)
					{
						/* A and X*/
						if (pointerIndex) {
							/* Left */
							if (curData) {
								ovr->RemoveActor(hhacts[curData]);
								ovr->RemoveActor(volAct[curData]);
								--curData;

								volAct[curData]->SetPosition(volAct[curData + 1]->GetPosition());
								volAct[curData]->SetOrientation(volAct[curData + 1]->GetOrientation());
								hhacts[curData]->SetPosition(volAct[curData]->GetPosition());
								hhacts[curData]->SetOrientation(volAct[curData]->GetOrientation());

								ovr->AddActor(volAct[curData]);
								if (renderTech) ovr->AddActor(hhacts[curData]);
								/*
								if (renderTech) {
									volAct[curData]->SetPosition(volAct[curData + 1]->GetPosition());
									volAct[curData]->SetOrientation(volAct[curData + 1]->GetOrientation());
									ovr->AddActor(volAct[curData]);
								}
								else {
									hhacts[curData]->SetPosition(hhacts[curData + 1]->GetPosition());
									hhacts[curData]->SetOrientation(hhacts[curData + 1]->GetOrientation());
									ovr->AddActor(hhacts[curData]);
								}
								*/
							}
						}
						else {
							/* Right */
							if ((curData + 1) < numData) {
								ovr->RemoveActor(hhacts[curData]);
								ovr->RemoveActor(volAct[curData]);
								++curData;

								volAct[curData]->SetPosition(volAct[curData - 1]->GetPosition());
								volAct[curData]->SetOrientation(volAct[curData - 1]->GetOrientation());
								hhacts[curData]->SetPosition(volAct[curData]->GetPosition());
								hhacts[curData]->SetOrientation(volAct[curData]->GetOrientation());

								ovr->AddActor(volAct[curData]);
								if (renderTech) ovr->AddActor(hhacts[curData]);
								/*
								if (renderTech) {
									volAct[curData]->SetPosition(volAct[curData - 1]->GetPosition());
									volAct[curData]->SetOrientation(volAct[curData - 1]->GetOrientation());
									ovr->AddActor(volAct[curData]);
								}
								else {
									hhacts[curData]->SetPosition(hhacts[curData - 1]->GetPosition());
									hhacts[curData]->SetOrientation(hhacts[curData - 1]->GetOrientation());
									ovr->AddActor(hhacts[curData]);
								}
								*/
							}
						}
					}
				}
				if (event.eventType == vr::VREvent_ButtonUnpress)
				{
					if (event.data.controller.button == vr::EVRButtonId::k_EButton_Axis1)
					{
						this->LeftButtonReleaseEvent();
					}
					if (event.data.controller.button == vr::EVRButtonId::k_EButton_Axis0)
					{
						//this->RightButtonReleaseEvent();
					}
					if (event.data.controller.button == vr::EVRButtonId::k_EButton_Grip)
					{
						this->MiddleButtonReleaseEvent();
					}
					if (event.data.controller.button == vr::EVRButtonId::k_EButton_ApplicationMenu)
					{
						//this->FourthButtonReleaseEvent();
					}
				}
			}
		}
		else
		{
			// if pointers are down track the movement
			if (this->PointersDownCount)
			{
				for (int i = 0; i < VTKI_MAX_POINTERS; i++)
				{
					if (this->PointersDown[i])
					{
						this->UpdateTouchPadPosition(pHMD,
							static_cast<vr::TrackedDeviceIndex_t>(this->PointerIndexLookup[i]));
						vr::TrackedDevicePose_t &tdPose =
							renWin->GetTrackedDevicePose(
								static_cast<vr::TrackedDeviceIndex_t>(this->PointerIndexLookup[i]));
						double pos[3];
						double ppos[3];
						double wxyz[4];
						this->ConvertPoseToWorldCoordinates(ren, tdPose, pos, wxyz, ppos);

						// so even though we have world coordinates we have to convert them to
						// screen coordinates because all of VTKs picking code is currently
						// based on screen coordinates
						ren->SetWorldPoint(pos[0], pos[1], pos[2], 1.0);
						ren->WorldToDisplay();
						double *displayCoords = ren->GetDisplayPoint();
						this->SetEventPosition(displayCoords[0], displayCoords[1], i);
						this->SetWorldEventPosition(pos[0], pos[1], pos[2], i);
						this->SetWorldEventOrientation(wxyz[0], wxyz[1], wxyz[2], wxyz[3], i);
						this->SetPhysicalEventPosition(ppos[0], ppos[1], ppos[2], i);
					}
				}
				this->MouseMoveEvent();
			}
		}
		renWin->Render();
	}
}
