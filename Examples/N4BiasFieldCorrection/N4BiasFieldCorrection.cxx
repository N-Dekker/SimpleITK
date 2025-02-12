/*=========================================================================
*
*  Copyright NumFOCUS
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/

#include <SimpleITK.h>
#include <iostream>
#include <stdlib.h>


namespace sitk = itk::simple;

int main ( int argc, char* argv[] ) {

  if ( argc < 2 ) {
    std::cerr << "Usage: N4BiasFieldCorrection inputImage outputImage";
    std::cerr << " [shrinkFactor] [maskImage] [numberOfIterations]";
    std::cerr << " [numberOfFittingLevels]\n";
    return 1;
  }

  sitk::Image inputImage = sitk::ReadImage( argv[1], sitk::sitkFloat32 );
  sitk::Image image = inputImage;

  sitk::Image maskImage;
  if ( argc > 4 ) {
    maskImage = sitk::ReadImage( argv[4], sitk::sitkUInt8 );
  } else {
    maskImage = sitk::OtsuThreshold( image, 0, 1, 200 );
  }

  if ( argc > 3 ) {
    unsigned int s = atoi( argv[3] );
    std::vector<unsigned int> shrink( inputImage.GetDimension(), s );
    image = sitk::Shrink( inputImage, shrink );
    maskImage = sitk::Shrink( maskImage, shrink );
  }

  sitk::N4BiasFieldCorrectionImageFilter *corrector
    = new sitk::N4BiasFieldCorrectionImageFilter();

  unsigned int numFittingLevels = 4;

  if ( argc > 6) {
    numFittingLevels = atoi(argv[6]);
  }

  if ( argc > 5 ) {
    unsigned int it = atoi( argv[5] );
    std::vector<unsigned int> iterations( numFittingLevels, it );
    corrector->SetMaximumNumberOfIterations( iterations );
  }

  sitk::Image corrected_image = corrector->Execute( image, maskImage );

  sitk::Image log_bias_field = corrector->GetLogBiasFieldAsImage( inputImage );

  sitk::Image bias_field = sitk::Divide( inputImage, sitk::Exp( log_bias_field ) );

  sitk::WriteImage( corrected_image, argv[2] );

  if (getenv("SITK_NOSHOW") == NULL)
    sitk::Show(corrected_image, "N4 Corrected");

  return 0;
}
