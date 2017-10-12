#pragma once
/* Contains all standard default and shared includes to speedup compilation
 */

/* Project */

/* Standard includes */
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <cstdio>

/********** vtk includes *********/
/* Generic (data) */
#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkDataSet.h>
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