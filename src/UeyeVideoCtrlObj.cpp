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
  if(m_cam->isMonochrome())
    {
      aList.push_back(BAYER_RG8);
      aList.push_back(BAYER_RG16);
      aList.push_back(RGB24);
      aList.push_back(YUV422);
    }
}

void VideoCtrlObj::getVideoMode(VideoMode &aMode) const
{
  aMode = m_cam->getVideoMode();
}

void VideoCtrlObj::setVideoMode(VideoMode aMode)
{
  m_cam->setVideoMode(aMode);
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
