#ifndef UEYEDETINFOCTRLOBJ_H
#define UEYEDETINFOCTRLOBJ_H

#include "HwDetInfoCtrlObj.h"
#include "HwMaxImageSizeCallback.h"
#include "Debug.h"

namespace lima
{
  namespace Ueye
  {
    class Camera;
    class DetInfoCtrlObj : public HwMaxImageSizeCallbackGen,public HwDetInfoCtrlObj
    {
      DEB_CLASS_NAMESPC(DebModCamera, "DetInfoCtrlObj","Ueye");

    public:
      DetInfoCtrlObj(Camera*);
      virtual ~DetInfoCtrlObj();

      virtual void getMaxImageSize(Size& max_image_size);
      virtual void getDetectorImageSize(Size& det_image_size);

      virtual void getDefImageType(ImageType& def_image_type);
      virtual void getCurrImageType(ImageType& curr_image_type);
      virtual void setCurrImageType(ImageType  curr_image_type);

      virtual void getPixelSize(double& pixel_size);
      virtual void getDetectorType(std::string& det_type);
      virtual void getDetectorModel(std::string& det_model);
      
      virtual void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
      {
        m_cbk_gen.registerMaxImageSizeCallback(cb);
      }

      virtual void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
      {
        m_cbk_gen.unregisterMaxImageSizeCallback(cb);
      }

    private:
      Camera* m_cam;
      HwMaxImageSizeCallbackGen m_cbk_gen;
    };

  } // namespace Ueye
} // namespace lima

#endif
