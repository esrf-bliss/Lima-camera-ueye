#include "UeyeInterface.h"
#include "UeyeCamera.h"
#include "UeyeDetInfoCtrlObj.h"
#include "UeyeSyncCtrlObj.h"
#include "UeyeVideoCtrlObj.h"

using namespace lima;
using namespace lima::Ueye;

Interface::Interface(Camera *cam) :
  m_cam(cam)
{
  DEB_CONSTRUCTOR();

  m_det_info = new DetInfoCtrlObj(cam);
  m_video = new VideoCtrlObj(cam);
  m_sync = new SyncCtrlObj(cam);

  cam->m_video = m_video;
}

Interface::~Interface()
{
  DEB_DESTRUCTOR();
  delete m_sync;
  delete m_video;
  delete m_det_info;
}

void Interface::getCapList(CapList &cap_list) const
{
  cap_list.push_back(HwCap(m_sync));
  cap_list.push_back(HwCap(m_det_info));
  cap_list.push_back(HwCap(m_video));
  cap_list.push_back(HwCap(&(m_video->getHwBufferCtrlObj())));
}

void Interface::reset(ResetLevel reset_level)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(reset_level);

  m_cam->reset();
}

void Interface::prepareAcq()
{
}

void Interface::startAcq()
{
  DEB_MEMBER_FUNCT();

  int aNbFrames;
  m_sync->getNbFrames(aNbFrames);
  m_video->getBuffer().setStartTimestamp(Timestamp::now());
  m_cam->startAcq(aNbFrames);
}

void Interface::stopAcq()
{
  DEB_MEMBER_FUNCT();

  m_cam->stopAcq();
}

void Interface::getStatus(StatusType& status)
{
  m_cam->getStatus(status);
}

int Interface::getNbAcquiredFrames()
{
  return m_cam->getNbAcquiredFrames();
}

int Interface::getNbHwAcquiredFrames()
{
  return getNbHwAcquiredFrames();
}
