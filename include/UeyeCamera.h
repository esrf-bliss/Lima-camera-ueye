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
#ifndef UEYECAMERA_H
#define UEYECAMERA_H
#include <ueye.h>
#include "HwMaxImageSizeCallback.h"
#include "HwInterface.h"

namespace lima
{
  namespace Ueye
  {
    class VideoCtrlObj;
    class Camera
    {
      DEB_CLASS_NAMESPC(DebModCamera,"Camera","Ueye");
      friend class Interface;
      friend class DetInfoCtrlObj;
      friend class VideoCtrlObj;
      friend class SyncCtrlObj;
    public:
      Camera(int addresse = 0);	// addresse == 0 first available
      ~Camera();
      void reset();
      void startAcq(int nbFrames);
      void stopAcq();
      int getNbAcquiredFrames() const;
      void getStatus(HwInterface::StatusType& status);
      bool isMonochrome() const;
    private:
      class _AcqThread;
      friend class _AcqThread;

      HIDS 		m_cam_id;
      CAMINFO 		m_camera_info;
      SENSORINFO 	m_sensor_info;
      char*		m_buffer[2];
      int		m_buffer_id[2];
      volatile bool	m_quit;
      _AcqThread*	m_acq_thread;
      int		m_last_frames_id;
      int		m_asked_frames_number;
      bool		m_acq_started;
      VideoCtrlObj*	m_video;
      HwInterface::Status::Basic m_status;
    };
  }
}
#endif
