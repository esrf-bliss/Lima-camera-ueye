.. _camera-ueye:

Ueye
-------
.. image:: ids.jpg
.. image:: ueye.jpg

Intoduction
```````````
Industrial Cameras for digital imaging and visualization (USB,GigE).

home site: http://www.ids-imaging.com/

Installation & Module configuration
````````````````````````````````````

-  follow first the steps for the linux installation :ref:`linux_installation`

Previously to this you have to install the Ueye SDK.
See the sdk README provide in the ueye module

Ueye python module need at least the lima core module.

The minimum configuration file is *config.inc* :

.. code-block:: sh

  COMPILE_CORE=1
  COMPILE_SIMULATOR=0
  COMPILE_SPS_IMAGE=1
  COMPILE_ESPIA=0
  COMPILE_FRELON=0
  COMPILE_MAXIPIX=0
  COMPILE_PILATUS=0
  COMPILE_BASLER=0
  COMPILE_PROSILICA=0
  COMPILE_ROPERSCIENTIFIC=0
  COMPILE_MYTHEN=0
  COMPILE_ADSC=0
  COMPILE_UEYE=1
  COMPILE_CBF_SAVING=0
  COMPILE_NXS_SAVING=0
  export COMPILE_CORE COMPILE_SPS_IMAGE COMPILE_SIMULATOR \
         COMPILE_ESPIA COMPILE_FRELON COMPILE_MAXIPIX COMPILE_PILATUS \
       COMPILE_BASLER COMPILE_PROSILICA COMPILE_ROPERSCIENTIFIC COMPILE_ADSC \
       COMPILE_MYTHEN COMPILE_UEYE COMPILE_CBF_SAVING COMPILE_NXS_SAVING

-  start the compilation :ref:`linux_compilation`

-  finally for the Tango server installation :ref:`tango_installation`

Initialisation and Capabilities
````````````````````````````````

Camera initialisation
......................

The camera will be initialized   by creating a Ueye::Camera object.  The Camera contructor
sets the camera with default parameters, only the video address (e.g. 0) of the camera is mandatory.

Std capabilites
................

This plugin has been implement in respect of the mandatory capabilites but with some limitations which
are due to the camera and SDK features. Only restriction on capabilites are documented here.

* HwDetInfo
  
  getCurrImageType/getDefImageType(): it can change if the video mode change (see HwVideo capability).

  setCurrImageType(): It only supports Bpp8 and Bpp16.

* HwSync

  get/setTrigMode(): the only supported mode are IntTrig, IntTrigMult ExtTrigSingle and ExtTrigMult.
  
  

Optional capabilites
........................
In addition to the standard capabilities, we make the choice to implement some optional capabilities which
are supported by the SDK. **Video** and Binning are available.

* HwVideo

  The prosilica cameras are pure video device, so video format for image are supported:
  
  **For color cameras ONLY**
   - BAYER_RG8
   - BAYER_RG16
   - BAYER_BG8
   - BAYER_BG16
   - RGB24
   - YUV422
   
  **Color and Monochrome cameras**
   - Y8   
   - Y16   

  Use get/setMode() methods of the *video* object (i.e. CtControl::video()) to read or set the format.

* HwBin 

  There is no restriction for the binning up to the maximum size.

Configuration
``````````````
See the sdk README in camera/ueye/sdk/ directory.

How to use
````````````
A python code example  for testing your camera:

.. code-block:: python

  from Lima import Ueye
  from lima import Core

  #-----------------+
  #                 |
  #                 v the video address
  cam = Ueye.Camera(0)

  hwint = Ueye.Interface(cam)
  ct = Core.CtControl(hwint)

  acq = ct.acquisition()


  # set video  and test video, supposing we have a color camera !!
  # 

  video=ct.video()
  video.setMode(Core.YUV422)
  video.setExposure(0.1)
  video.startLive()
  video.stopLive()
  video_img = video.getLastImage()


  # set and test acquisition 
  #

  # setting new file parameters and autosaving mode
  saving=ct.saving()

  pars=saving.getParameters()
  pars.directory='/buffer/lcb18012/opisg/test_lima'
  pars.prefix='test1_'
  pars.suffix='.edf'
  pars.fileFormat=Core.CtSaving.TIFF
  pars.savingMode=Core.CtSaving.AutoFrame
  saving.setParameters(pars)

  acq.setAcqExpoTime(0.1)
  acq.setNbImages(10) 
  ct.prepareAcq()
  ct.startAcq()

  # wait for last image (#9) ready
  lastimg = ct.getStatus().ImageCounters.LastImageReady
  while lastimg !=9:
    time.sleep(0.1)
    lastimg = ct.getStatus().ImageCounters.LastImageReady
 
  # read the first image
  im0 = ct.ReadImage(0)

