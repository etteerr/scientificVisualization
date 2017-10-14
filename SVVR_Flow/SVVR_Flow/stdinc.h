#pragma once
/* Contains all standard default and shared includes to speedup compilation
 */

/* Project */
#include "defines.h"

/* Standard includes */
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <cstdio>

/********** vtk includes *********/
/* Generic (application) */
#include <vtkErrorCode.h>
/* Generic (data) */
#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkDataSet.h>
/* Legacy data readers */
#include <vtkGenericDataObjectReader.h>
#include <vtkStructuredGridReader.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
/* renderer */
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
/* XML */
#include <vtkXMLReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkXMLHyperOctreeReader.h>
#include <vtkXMLCompositeDataReader.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkXMLImageDataReader.h>
/* data types */
#include <vtkPolyDataMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkHyperOctree.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkStructuredGrid.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkFieldData.h>
#include <vtkCellTypes.h>
#include <vtksys/SystemTools.hxx>