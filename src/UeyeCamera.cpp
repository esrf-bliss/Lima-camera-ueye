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
  m_video(NULL)
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
  if(m_sensor_info.nColorMode == IS_COLORMODE_BAYER)
    {
      DEB_ALWAYS() << "Color camera";
      if(IS_SUCCESS != is_SetColorMode(m_cam_id,IS_CM_BAYER_RG16))
	{
	  DEB_TRACE() << "Failed to set Bayer 16";
	  if(IS_SUCCESS != is_SetColorMode(m_cam_id,IS_CM_BAYER_RG8))
	    DEB_TRACE() << "Failed to set Bayer 8";
	}
    }
  else
    {
      DEB_ALWAYS() << "Monochrome camera";
      is_SetColorMode(m_cam_id,IS_CM_MONO16);
    }

  DEB_TRACE() << DEB_VAR3(m_sensor_info.nMaxWidth,
			  m_sensor_info.nMaxHeight,
			  m_sensor_info.bGlobShutter);
  //m_sensor_info.wPixelSize);
  DEB_TRACE() << DEB_VAR4(m_sensor_info.bMasterGain,
			  m_sensor_info.bRGain,
			  m_sensor_info.bGGain,
			  m_sensor_info.bBGain);

  m_buffer[0] = (char*)malloc(m_sensor_info.nMaxWidth * 
			      m_sensor_info.nMaxHeight *
			      sizeof(int));
  m_buffer[1] = (char*)malloc(m_sensor_info.nMaxWidth * 
			      m_sensor_info.nMaxHeight *
			      sizeof(int));

  if(IS_SUCCESS != is_SetAllocatedImageMem(m_cam_id,
					   m_sensor_info.nMaxWidth,
					   m_sensor_info.nMaxHeight,
					   32,m_buffer[0],&m_buffer_id[0]))
    {
      free(m_buffer[0]);
      free(m_buffer[1]);
      THROW_HW_ERROR(Error) << "Couldn't set memory to harware";
    }

  if(IS_SUCCESS != is_SetAllocatedImageMem(m_cam_id,
					   m_sensor_info.nMaxWidth,
					   m_sensor_info.nMaxHeight,
					   32,m_buffer[1],&m_buffer_id[1]))
    {
      is_FreeImageMem(m_cam_id,m_buffer[0],m_buffer_id[0]);
      free(m_buffer[0]);
      free(m_buffer[1]);

      THROW_HW_ERROR(Error) << "Couldn't set memory to harware";
    }
  
  if(IS_SUCCESS != is_AddToSequence(m_cam_id,m_buffer[0],m_buffer_id[0]))
    {
      is_FreeImageMem(m_cam_id,m_buffer[0],m_buffer_id[0]); 
      is_FreeImageMem(m_cam_id,m_buffer[1],m_buffer_id[1]);

      free(m_buffer[0]);
      free(m_buffer[1]);
      
      THROW_HW_ERROR(Error) << "Couldn't add memory to sequence";
    }

  if(IS_SUCCESS != is_AddToSequence(m_cam_id,m_buffer[1],m_buffer_id[1]))
    {
      is_ClearSequence(m_cam_id);

      is_FreeImageMem(m_cam_id,m_buffer[0],m_buffer_id[0]); 
      is_FreeImageMem(m_cam_id,m_buffer[1],m_buffer_id[1]);

      free(m_buffer[0]);
      free(m_buffer[1]);
      
      THROW_HW_ERROR(Error) << "Couldn't add memory to sequence";
    }
  if(IS_SUCCESS != is_EnableEvent(m_cam_id,IS_SET_EVENT_FRAME))
    {
      is_ClearSequence(m_cam_id);

      is_FreeImageMem(m_cam_id,m_buffer[0],m_buffer_id[0]); 
      is_FreeImageMem(m_cam_id,m_buffer[1],m_buffer_id[1]);

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