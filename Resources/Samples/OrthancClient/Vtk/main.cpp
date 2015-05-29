/**
 * Orthanc - A Lightweight, RESTful DICOM Store
 * Copyright (C) 2012-2015 Sebastien Jodogne, Medical Physics
 * Department, University Hospital of Liege, Belgium
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/


#include <iostream>

#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkOpenGLRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include <orthanc/OrthancCppClient.h>


void Display(OrthancClient::Series& series)
{
  /**
   * Load the 3D image from Orthanc into VTK.
   **/

  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
  image->SetDimensions(series.GetWidth(), series.GetHeight(), series.GetInstanceCount());
  image->SetScalarType(VTK_SHORT);
  image->AllocateScalars();

  if (series.GetWidth() != 0 &&
      series.GetHeight() != 0 && 
      series.GetInstanceCount() != 0)
  {
    series.Load3DImage(image->GetScalarPointer(0, 0, 0), Orthanc::PixelFormat_SignedGrayscale16,
                       2 * series.GetWidth(), 2 * series.GetHeight() * series.GetWidth());
  }

  image->SetSpacing(series.GetVoxelSizeX(), 
                    series.GetVoxelSizeY(), 
                    series.GetVoxelSizeZ());


  /**
   * The following code is based on the VTK sample for MIP
   * http://www.vtk.org/Wiki/VTK/Examples/Cxx/VolumeRendering/MinIntensityRendering
   **/

  // Create a transfer function mapping scalar value to opacity
  double range[2];
  image->GetScalarRange(range);

  vtkSmartPointer<vtkPiecewiseFunction> opacityTransfer = 
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  opacityTransfer->AddSegment(range[0], 0.0, range[1], 1.0);
 
  vtkSmartPointer<vtkColorTransferFunction> colorTransfer = 
    vtkSmartPointer<vtkColorTransferFunction>::New();
  colorTransfer->AddRGBPoint(0, 1.0, 1.0, 1.0);
  colorTransfer->AddRGBPoint(range[1], 1.0, 1.0, 1.0);
 
  vtkSmartPointer<vtkVolumeProperty> property = 
    vtkSmartPointer<vtkVolumeProperty>::New();
  property->SetScalarOpacity(opacityTransfer);
  property->SetColor(colorTransfer);
  property->SetInterpolationTypeToLinear();

  // Create a Maximum Intensity Projection rendering
  vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> mapper = 
    vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
  mapper->SetBlendModeToMaximumIntensity();
  mapper->SetInput(image);

  vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(mapper);
  volume->SetProperty(property);
  
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkOpenGLRenderer>::New();
  renderer->AddViewProp(volume);
  renderer->SetBackground(0.1, 0.2, 0.3); // Background color dark blue

  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = 
    vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
 
  vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
  window->AddRenderer(renderer); 

  vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(window);
  interactor->SetInteractorStyle(style);
  interactor->Start();
}


int main()
{
  try
  {
    // The following explicit initialization is not required, except
    // if you wish to specify the full path to the shared library
    OrthancClient::Initialize();

    // Use the commented code below if you know the identifier of a
    // series that corresponds to a 3D image.

    /*
      {
      OrthancClient::OrthancConnection orthanc("http://localhost:8042");
      OrthancClient::Series series(orthanc, "dc5ec3d9-6e1a7b2c-73a829f0-64c609f6-ef976a97");
      Display(series);
      return 0;
      }
    */


    // Try and find a 3D image inside the local store
    OrthancClient::OrthancConnection orthanc("http://localhost:8042");

    for (unsigned int i = 0; i < orthanc.GetPatientCount(); i++)
    {
      OrthancClient::Patient patient(orthanc.GetPatient(i));
      std::cout << "Patient: " << patient.GetId() << std::endl;

      for (unsigned int j = 0; j < patient.GetStudyCount(); j++)
      {
        OrthancClient::Study study(patient.GetStudy(j));
        std::cout << "  Study: " << study.GetId() << std::endl;

        for (unsigned int k = 0; k < study.GetSeriesCount(); k++)
        {
          OrthancClient::Series series(study.GetSeries(k));
          std::cout << "    Series: " << series.GetId() << std::endl;

          if (series.Is3DImage())
          {
            Display(series);
            return 0;
          }
          else
          {
            std::cout << "      => Not a 3D image..." << std::endl;
          }
        }
      }
    }

    std::cout << "Unable to find a 3D image in the local Orthanc store" << std::endl;

    return 0;
  }
  catch (OrthancClient::OrthancClientException& e)
  {
    std::cerr << "EXCEPTION: [" << e.What() << "]" << std::endl;
    return -1;
  }
}
