#include "UeyeVideoCtrlObj.h"
#include "UeyeCamera.h"

using namespace lima;
using namespace lima::Ueye;

VideoCtrlObj::VideoCtrlObj(Camera *cam) : m_cam(cam),m_live_flag(false)
{
}

VideoCtrlObj::~VideoCtrlObj()
{
}

void VideoCtrlObj::getSupportedVideoMode(std::list<VideoMode> &aList) const
{
  aList.push_back(Y8);
  aList.push_back(Y16);
  if(m_cam->m_sensor_info.nColorMode == IS_COLORMODE_BAYER)
    {
      aList.push_back(BAYER_RG8);
      aList.push_back(BAYER_RG16);
      aList.push_back(RGB24);
      aList.push_back(YUV422);
    }
}

void VideoCtrlObj::getVideoMode(VideoMode &aMode) const
{
  DEB_MEMBER_FUNCT();

  INT aColorMode = is_SetColorMode(m_cam->m_cam_id,IS_GET_COLOR_MODE);
  switch(aColorMode)
    {
    case IS_CM_BAYER_RG16: 	aMode = BAYER_RG16;break;
    case IS_CM_BAYER_RG8: 	aMode = BAYER_RG8;break;
    case IS_CM_MONO16: 		aMode = Y16;break;
    case IS_CM_MONO8: 		aMode = Y8;break;
    case IS_SET_CM_RGB24: 	aMode = RGB24;break;
    case IS_CM_UYVY_PACKED: 	aMode = YUV422;break;
    default:
      THROW_HW_ERROR(Error) << "Unknowed video mode";
    }
  DEB_RETURN() << DEB_VAR1(aMode);
}

void VideoCtrlObj::setVideoMode(VideoMode aMode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aMode);

  INT aColorMode;
   switch(aMode)
    {
    case BAYER_RG16: 	aColorMode = IS_CM_BAYER_RG16;break;
    case BAYER_RG8: 	aColorMode = IS_CM_BAYER_RG8;break;
    case Y16: 		aColorMode = IS_CM_MONO16;break;
    case Y8: 		aColorMode = IS_CM_MONO8;break;
    case RGB24: 	aColorMode = IS_CM_RGB8_PACKED;break;
    case YUV422: 	aColorMode = IS_CM_UYVY_PACKED;break;
    default:
      THROW_HW_ERROR(Error) << "Video mode not managed";
    }
   if(IS_SUCCESS != is_SetColorMode(m_cam->m_cam_id,aColorMode))
     THROW_HW_ERROR(Error) << "Can't change video mode";
}

void VideoCtrlObj::setLive(bool flag)
{
  m_live_flag = flag;
  if(flag)
    m_cam->startAcq(0);
  else
    m_cam->stopAcq();
}

void VideoCtrlObj::getLive(bool &flag) const
{
  flag = m_live_flag;
}

void VideoCtrlObj::checkBin(Bin& bin)
{
  bin = Bin(1,1);		// Do not manage Hw Bin
}

void VideoCtrlObj::checkRoi(const Roi&, Roi& hw_roi)
{
  hw_roi = Roi(0,0,m_cam->m_sensor_info.nMaxWidth,m_cam->m_sensor_info.nMaxHeight);
}
