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
#include <stdlib.h>
#include "UeyeCamera.h"
#include "UeyeVideoCtrlObj.h"

using namespace lima;
using namespace lima::Ueye;

class Camera::_AcqThread : public Thread
{
  DEB_CLASS_NAMESPC(DebModCamera,"Camera","_AcqThread");
public:
  _AcqThread(Camera &aCam);
  virtual ~_AcqThread();
  
protected:
  virtual void threadFunction();
private:
  Camera&	m_cam;
};

Camera::Camera(int addresse) :
  m_quit(false),
  m_acq_thread(NULL),
  m_acq_started(false),
  m_video(NULL),
  m_status(HwInterface::Status::Ready)
{
  DEB_CONSTRUCTOR();

  m_cam_id = addresse;
  INT nRet = is_InitCamera(&m_cam_id,NULL);

  if(nRet != IS_SUCCESS)
    THROW_HW_ERROR(Error) << "Camera with addresse : " 
			  << addresse << " is not found";
  
  if(IS_SUCCESS != is_GetCameraInfo(m_cam_id,&m_camera_info))
    THROW_HW_ERROR(Error) << "Couldn't get camera information";

  DEB_ALWAYS() << "Manufacturer : " << m_camera_info.ID;
  DEB_TRACE() << DEB_VAR4(m_camera_info.Version,
			  m_camera_info.Date,
			  m_camera_info.SerNo,
			  m_camera_info.Select);

  if(IS_SUCCESS != is_GetSensorInfo(m_cam_id,&m_sensor_info))
    THROW_HW_ERROR(Error) << "Couldn't get sensor information";

  DEB_ALWAYS() << "Sensor name : " << m_sensor_info.strSensorName;
  if(isMonochrome())
    {
      DEB_ALWAYS() << "Color camera";
      m_video_mode = BAYER_RG16;
      m_image_type = Bpp16;
      if(IS_SUCCESS != is_SetColorMode(m_cam_id,IS_CM_BAYER_RG16))
	{
	  DEB_TRACE() << "Failed to set Bayer 16";
	  m_video_mode = BAYER_RG8;
	  m_image_type = Bpp8;
	  if(IS_SUCCESS != is_SetColorMode(m_cam_id,IS_CM_BAYER_RG8))
	    DEB_TRACE() << "Failed to set Bayer 8";
	}
    }
  else
    {
      DEB_ALWAYS() << "Monochrome camera";
      m_video_mode = Y16;
      m_image_type = Bpp16;
      if(IS_SUCCESS != is_SetColorMode(m_cam_id,IS_CM_MONO12))
	{
	  DEB_TRACE() << "Failed to set Mono 16";
	  m_video_mode = Y8;
	  m_image_type = Bpp8;
	  if(IS_SUCCESS != is_SetColorMode(m_cam_id,IS_CM_MONO8))
	    DEB_TRACE() << "Failed to set Mono 8";
	}
    }

  double pval1, pval2;
  int errno;
  char *errstr;
  if (IS_SUCCESS != is_SetAutoParameter (m_cam_id, IS_GET_ENABLE_AUTO_GAIN, &pval1, &pval2))
	  DEB_ALWAYS() << "Failed to get auto gain control setting";
  else
	  DEB_ALWAYS() << "auto gain: " << pval1;

  pval1 = 1;
  if (IS_SUCCESS != is_SetAutoParameter (m_cam_id, IS_SET_ENABLE_AUTO_GAIN, &pval1, &pval2))
	  DEB_ALWAYS() << "Failed to set auto gain control";

  if (IS_SUCCESS != is_SetAutoParameter (m_cam_id, IS_GET_ENABLE_AUTO_FRAMERATE, &pval1, &pval2))
	  DEB_ALWAYS() << "Failed to get auto framerate control setting";
  else
	  DEB_ALWAYS() << "auto framerate: " << pval1;

  pval1 = 1;
  if (IS_SUCCESS != is_SetAutoParameter (m_cam_id, IS_SET_ENABLE_AUTO_FRAMERATE, &pval1, &pval2))
  {
	  is_GetError (m_cam_id, &errno, &errstr);
	  DEB_ALWAYS() << "Failed to set auto frame rate control: " << errstr;
  }
  DEB_ALWAYS() << DEB_VAR4(m_sensor_info.nMaxWidth,
			  m_sensor_info.nMaxHeight,
			  m_sensor_info.bGlobShutter,
			  m_sensor_info.wPixelSize);

  DEB_ALWAYS() << DEB_VAR4(m_sensor_info.bMasterGain,
			  m_sensor_info.bRGain,
			  m_sensor_info.bGGain,
			  m_sensor_info.bBGain);

  m_buffer[0] = (char*)malloc(m_sensor_info.nMaxWidth * 
			      m_sensor_info.nMaxHeight *
			      sizeof(int));
  m_buffer[1] = (char*)malloc(m_sensor_info.nMaxWidth * 
			      m_sensor_info.nMaxHeight *
			      sizeof(int));
  _allocBuffer();

  if(IS_SUCCESS != is_EnableEvent(m_cam_id,IS_SET_EVENT_FRAME))
    {
      free(m_buffer[0]);
      free(m_buffer[1]);
      
      THROW_HW_ERROR(Error) << "Couldn't enable Event";
    }

  //Acquisition  Thread
  m_acq_thread = new _AcqThread(*this);
  m_acq_thread->start();
}

Camera::~Camera()
{
  delete m_acq_thread;

  is_ClearSequence(m_cam_id);

  is_FreeImageMem(m_cam_id,m_buffer[0],m_buffer_id[0]);
  is_FreeImageMem(m_cam_id,m_buffer[1],m_buffer_id[1]);

  free(m_buffer[0]);
  free(m_buffer[1]);

  is_ExitCamera(m_cam_id);
}

void Camera::reset()
{
  stopAcq();
}

void Camera::startAcq(int nbFrames)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(nbFrames);

  m_last_frames_id = -1;
  m_asked_frames_number = nbFrames;
  if(IS_SUCCESS != is_CaptureVideo(m_cam_id,IS_DONT_WAIT))
    THROW_HW_ERROR(Error) << "Can't start acquisition";
  m_acq_started = true;
}

void Camera::stopAcq()
{
  DEB_MEMBER_FUNCT();

  if(IS_SUCCESS != is_StopLiveVideo(m_cam_id,IS_FORCE_VIDEO_STOP))
    THROW_HW_ERROR(Error) << "Can't stop acquisition";
  m_acq_started = false;
}

int Camera::getNbAcquiredFrames() const
{
  return m_last_frames_id + 1;
}

void Camera::getStatus(HwInterface::StatusType &status)
{
  if (m_acq_started)
    {
      status.acq = AcqRunning;
      status.det = DetExposure;
    }
  else
    {
      status.acq = AcqReady;
      status.det = DetIdle;
    }
}

bool Camera::isMonochrome() const
{
  DEB_MEMBER_FUNCT();

  return m_sensor_info.nColorMode == IS_COLORMODE_BAYER;
}

VideoMode Camera::getVideoMode() const
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(m_video_mode);

  return m_video_mode;
}

void Camera::setVideoMode(VideoMode aMode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aMode);
  INT aColorMode;
  ImageType anImageType;

  if (aMode == m_video_mode)
	  return;

  switch(aMode)
    {
    case Y8:
      aColorMode = IS_CM_MONO8;
      anImageType = Bpp8;
      break;
    case Y16:
      aColorMode = IS_CM_MONO12;
      anImageType = Bpp16;
      break;
    case BAYER_RG8:
      aColorMode = IS_CM_BAYER_RG8;
      anImageType = Bpp8;
      break;
    case BAYER_RG16:
      aColorMode = IS_CM_BAYER_RG16;
      anImageType = Bpp16;
      break;
    default:
      throw LIMA_HW_EXC(InvalidValue,"This video mode is not managed!");
    }

  if (m_acq_started)
    {
      DEB_TRACE() << "Acquisition in progress, stopping";
      if (IS_SUCCESS != is_StopLiveVideo(m_cam_id, IS_WAIT))
	THROW_HW_ERROR(Error) << "Can't stop acquisition";
    }

  if(IS_SUCCESS != is_SetColorMode(m_cam_id, aColorMode))
	  THROW_HW_ERROR(Error) << "Can't change video mode";

  if (anImageType != m_image_type)
    {
      m_image_type = anImageType;
      _allocBuffer();
    }

  if (m_acq_started)
    {
      DEB_TRACE() << "Restarting acquisition";
      if (IS_SUCCESS != is_CaptureVideo(m_cam_id,IS_DONT_WAIT))
	THROW_HW_ERROR(Error) << "Can't start acquisition";
    }

  m_video_mode = aMode;
}

double Camera::getGain() const
{
	DEB_MEMBER_FUNCT();
	double aGain;
	int masterGain = is_SetHardwareGain (m_cam_id,
				IS_GET_MASTER_GAIN,
				IS_IGNORE_PARAMETER,
				IS_IGNORE_PARAMETER,
				IS_IGNORE_PARAMETER);

	aGain = double(masterGain) / 100;

	DEB_RETURN() << DEB_VAR1(aGain);
	return aGain;
}

void Camera::setGain(double aGain)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(aGain);

	int masterGain;
	if (aGain < 0)
		masterGain = IS_SET_ENABLE_AUTO_GAIN;
	else
		masterGain = int(100 * aGain);

	if (IS_SUCCESS != is_SetHardwareGain (m_cam_id,
			masterGain,
			IS_IGNORE_PARAMETER,
			IS_IGNORE_PARAMETER,
			IS_IGNORE_PARAMETER))
		THROW_HW_ERROR(Error) << "Can't start acquisition";
}

void Camera::_allocBuffer()
{
  DEB_MEMBER_FUNCT();
  INT bitsPerPixel;

  switch(m_image_type)
    {
  case Bpp8:
  case Bpp8S:
	  bitsPerPixel = 8; break;
  case Bpp10:
  case Bpp10S:
	  bitsPerPixel = 10; break;
  case Bpp12:
  case Bpp12S:
	  bitsPerPixel = 12; break;
  case Bpp14:
  case Bpp14S:
	  bitsPerPixel = 14; break;
  case Bpp16:
  case Bpp16S:
	  bitsPerPixel = 16; break;
  case Bpp32:
  case Bpp32S:
	  bitsPerPixel = 32; break;
  default:
	  THROW_HW_ERROR(InvalidValue) << "Unknown image type";
    }

  is_ClearSequence(m_cam_id);
  is_FreeImageMem(m_cam_id,m_buffer[0],m_buffer_id[0]);
  is_FreeImageMem(m_cam_id,m_buffer[1],m_buffer_id[1]);

  if(IS_SUCCESS != is_SetAllocatedImageMem(m_cam_id,
					   m_sensor_info.nMaxWidth,
					   m_sensor_info.nMaxHeight,
					   bitsPerPixel,m_buffer[0],&m_buffer_id[0]))
    {
      THROW_HW_ERROR(Error) << "Couldn't set memory to harware";
    }

  if(IS_SUCCESS != is_SetAllocatedImageMem(m_cam_id,
					   m_sensor_info.nMaxWidth,
					   m_sensor_info.nMaxHeight,
					   bitsPerPixel,m_buffer[1],&m_buffer_id[1]))
    {
      is_FreeImageMem(m_cam_id,m_buffer[0],m_buffer_id[0]);
      THROW_HW_ERROR(Error) << "Couldn't set memory to harware";
    }

  if(IS_SUCCESS != is_AddToSequence(m_cam_id,m_buffer[0],m_buffer_id[0]))
    {
      is_FreeImageMem(m_cam_id,m_buffer[0],m_buffer_id[0]);
      is_FreeImageMem(m_cam_id,m_buffer[1],m_buffer_id[1]);

      THROW_HW_ERROR(Error) << "Couldn't add memory to sequence";
    }

  if(IS_SUCCESS != is_AddToSequence(m_cam_id,m_buffer[1],m_buffer_id[1]))
    {
      is_ClearSequence(m_cam_id);

      is_FreeImageMem(m_cam_id,m_buffer[0],m_buffer_id[0]);
      is_FreeImageMem(m_cam_id,m_buffer[1],m_buffer_id[1]);

      THROW_HW_ERROR(Error) << "Couldn't add memory to sequence";
    }

}

/*============================================================================
			   Thread function
============================================================================*/
Camera::_AcqThread::_AcqThread(Camera &cam) :
  m_cam(cam)
{
  pthread_attr_setscope(&m_thread_attr,PTHREAD_SCOPE_PROCESS);
}

Camera::_AcqThread::~_AcqThread()
{
  m_cam.m_quit = true;
  join();
}

void Camera::_AcqThread::threadFunction()
{
  DEB_MEMBER_FUNCT();
  sched_param param;
  param.sched_priority = sched_get_priority_max(SCHED_FIFO);
  if(pthread_setschedparam(pthread_self(),SCHED_FIFO,
			   &param))
    {
      DEB_TRACE() << "Could not set Fifo scheduling for Acquisition thread";
    }

  while(!m_cam.m_quit)
    {
      if(IS_SUCCESS == is_WaitEvent(m_cam.m_cam_id,IS_SET_EVENT_FRAME,1000))
	{
	  ++m_cam.m_last_frames_id;

	  bool m_continue_acq = false;
	  if(m_cam.m_video)
	    {
	      void *imageBufferPt;
	      if(IS_SUCCESS == is_GetImageMem(m_cam.m_cam_id,&imageBufferPt))
		{
		   DEB_TRACE() << "Acq frame : " << m_cam.m_last_frames_id;
		   DEB_TRACE() << DEB_VAR1(imageBufferPt);
		   VideoMode aMode;
		   m_cam.m_video->getVideoMode(aMode);
		   m_continue_acq = m_cam.m_video->callNewImage((char*)imageBufferPt,
								m_cam.m_sensor_info.nMaxWidth,
								m_cam.m_sensor_info.nMaxHeight,
								aMode);
		}
	    }

	  if(!m_continue_acq ||
	     (m_cam.m_asked_frames_number &&
	      m_cam.m_last_frames_id == m_cam.m_asked_frames_number - 1))
	    m_cam.stopAcq();
	}
    }
}
