#ifndef UEYEVIDEOCTRLOBJ_H
#define UEYEVIDEOCTRLOBJ_H
#include "HwVideoCtrlObj.h"

namespace lima
{
  namespace Ueye
  {
    class Camera;
    class VideoCtrlObj : public HwVideoCtrlObj
    {
      DEB_CLASS_NAMESPC(DebModCamera,"VideoCtrlObj","Ueye");
    public:
      VideoCtrlObj(Camera *cam);
      virtual ~VideoCtrlObj();

      virtual void getSupportedVideoMode(std::list<VideoMode> &aList) const;
      virtual void setVideoMode(VideoMode);
      virtual void getVideoMode(VideoMode&) const;

      virtual void setLive(bool);
      virtual void getLive(bool&) const;

      virtual void getGain(double&) const {};
      virtual void setGain(double) {};

      virtual void checkBin(Bin& bin);
      virtual void checkRoi(const Roi& set_roi, Roi& hw_roi);

      virtual void setBin(const Bin&){};
      virtual void setRoi(const Roi&){};

    private:
      Camera*	m_cam;
      bool	m_live_flag;
    };
  }
}
#endif
