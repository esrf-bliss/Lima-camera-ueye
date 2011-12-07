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
#include "UeyeSyncCtrlObj.h"
#include "UeyeCamera.h"

using namespace lima;
using namespace lima::Ueye;

SyncCtrlObj::SyncCtrlObj(Camera *cam) :
  m_cam(cam),
  m_nb_frames(1)
{
  DEB_CONSTRUCTOR();
}

SyncCtrlObj::~SyncCtrlObj()
{
  DEB_DESTRUCTOR();
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(trig_mode);

  switch(trig_mode)
    {
    case IntTrig:
    case IntTrigMult:
      return true;
    default:
      return false;
    }
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(trig_mode);

  if(checkTrigMode(trig_mode))
    m_trig_mode = trig_mode;
  else
    throw LIMA_HW_EXC(NotSupported,"Trigger type not supported");
}

void SyncCtrlObj::getTrigMode(TrigMode &trig_mode)
{
  trig_mode = m_trig_mode;
}

void SyncCtrlObj::setExpTime(double exp_time)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(exp_time);
  double newFrameRate;
  if(IS_SUCCESS != is_SetFrameRate (m_cam->m_cam_id,1/exp_time,&newFrameRate))
    THROW_HW_ERROR(Error) << "Can't set frame rate";

  if(IS_SUCCESS != is_Exposure(m_cam->m_cam_id,IS_EXPOSURE_CMD_SET_EXPOSURE,
			       &exp_time,sizeof(double)))
    THROW_HW_ERROR(Error) << "Can't set exposure time";
}

void SyncCtrlObj::getExpTime(double &exp_time)
{
  DEB_MEMBER_FUNCT();
  if(IS_SUCCESS != is_Exposure(m_cam->m_cam_id,IS_EXPOSURE_CMD_GET_EXPOSURE,
			       &exp_time,sizeof(double)))
    THROW_HW_ERROR(Error) << "Can't get exposure time";

  DEB_RETURN() << DEB_VAR1(exp_time);
}

void SyncCtrlObj::setLatTime(double  lat_time)
{
  //No latency managed
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
  lat_time = 0.;		// Don't know
}

void SyncCtrlObj::setNbFrames(int  nb_frames)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(nb_frames);

  m_nb_frames = nb_frames;
}

void SyncCtrlObj::getNbFrames(int& nb_frames)
{
  nb_frames = m_nb_frames;
}

void SyncCtrlObj::setNbHwFrames(int  nb_frames)
{
  setNbFrames(nb_frames);
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
  getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
  DEB_MEMBER_FUNCT();
  
  double ranges[3];
  if(IS_SUCCESS != is_Exposure(m_cam->m_cam_id,IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE,
			       ranges,sizeof(ranges)))
    THROW_HW_ERROR(Error) << "Can't get exposure ranges";
  valid_ranges.min_exp_time = ranges[0];
  valid_ranges.max_exp_time = ranges[1];
  valid_ranges.min_lat_time = 0.; // Don't know
  valid_ranges.max_lat_time = 0.; // Don't know
  
  DEB_RETURN() << DEB_VAR1(valid_ranges);
}
