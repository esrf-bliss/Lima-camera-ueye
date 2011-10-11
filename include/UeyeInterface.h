#ifndef UEYEINTERFACE_H
#define UEYEINTERFACE_H

#include "HwInterface.h"

namespace lima
{
  namespace Ueye
  {
    class Camera;
    class DetInfoCtrlObj;
    class VideoCtrlObj;
    class SyncCtrlObj;

    class Interface : public HwInterface
    {
      DEB_CLASS_NAMESPC(DebModCamera, "Interface", "Ueye");

    public:
      Interface(Camera*);
      virtual ~Interface();

      virtual void getCapList(CapList &) const;

      virtual void reset(ResetLevel reset_level);
      virtual void prepareAcq();
      virtual void startAcq();
      virtual void stopAcq();
      virtual void getStatus(StatusType& status);

      virtual int getNbAcquiredFrames();
      virtual int getNbHwAcquiredFrames();
    private:
      Camera* 		m_cam;
      DetInfoCtrlObj* 	m_det_info;
      VideoCtrlObj* 	m_video;
      SyncCtrlObj* 	m_sync;
    };

  } // namespace Ueye

} // namespace lima

#endif // UEYEINTERFACE_H
