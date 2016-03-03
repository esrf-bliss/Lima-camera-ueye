//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#include <cstdlib>
#include "UeyeDetInfoCtrlObj.h"
#include "UeyeCamera.h"

using namespace lima;
using namespace lima::Ueye;

DetInfoCtrlObj::DetInfoCtrlObj(Camera *cam):
  m_cam(cam)
{
}

DetInfoCtrlObj::~DetInfoCtrlObj()
{
}

void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
  max_image_size = Size(int(m_cam->m_sensor_info.nMaxWidth),
			int(m_cam->m_sensor_info.nMaxHeight));
}

void DetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
  getMaxImageSize(det_image_size);
}

void DetInfoCtrlObj::getDefImageType(ImageType& def_image_type)
{
  def_image_type = Bpp16;
}

void DetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type)
{
  DEB_MEMBER_FUNCT();
  
  INT aColorMode = is_SetColorMode(m_cam->m_cam_id,IS_GET_COLOR_MODE);
  DEB_PARAM() << DEB_VAR1(aColorMode);
  
  switch(aColorMode)
    {
    case IS_CM_SENSOR_RAW16:
    case IS_CM_SENSOR_RAW12:
    case IS_CM_SENSOR_RAW10:
    case IS_CM_MONO12:
    case IS_CM_MONO16:
      curr_image_type = Bpp16;
      break;
    default:
      curr_image_type = Bpp8;
      break;
    }
  DEB_RETURN() << DEB_VAR1(curr_image_type);
}

void DetInfoCtrlObj::setCurrImageType(ImageType curr_image_type)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(curr_image_type);

  INT aMode = is_SetColorMode(m_cam->m_cam_id,IS_GET_COLOR_MODE);
  INT aNextMode;
  switch(curr_image_type)
    {
    case Bpp16:
      if(aMode == IS_CM_MONO8 || aMode == IS_CM_MONO16)
	aNextMode = IS_CM_MONO16;
      else
	aNextMode = IS_CM_SENSOR_RAW16;
      break;
    case Bpp8:
      if(aMode == IS_CM_MONO8 || aMode == IS_CM_MONO16)
	aNextMode = IS_CM_MONO8;
      else if(aMode == IS_CM_SENSOR_RAW16)
	aNextMode = IS_CM_SENSOR_RAW8;
      break;
    default:
      throw LIMA_HW_EXC(InvalidValue,"This image type is not Managed");
    }

  if(IS_SUCCESS != is_SetColorMode(m_cam->m_cam_id,aNextMode))
    THROW_HW_ERROR(Error) << "Couldn't set color mode to :" << DEB_VAR2(aMode,aNextMode);
}

void DetInfoCtrlObj::getPixelSize(double& x_size,double& y_size)
{  
  x_size = y_size = -1.;		// @todo don't know
}

void DetInfoCtrlObj::getDetectorType(std::string& det_type)
{
  det_type = "Ueye";
}

void DetInfoCtrlObj::getDetectorModel(std::string& det_model)
{
  det_model = m_cam->m_sensor_info.strSensorName;
}
void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
  m_cbk_gen.registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
  m_cbk_gen.unregisterMaxImageSizeCallback(cb);
}
