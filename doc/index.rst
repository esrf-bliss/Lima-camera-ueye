Ueye
-------
.. image:: ids.jpg
.. image:: ueye.jpg

Intoduction
```````````
Industrial Cameras for digital imaging and visualization (USB,GigE).

home site: http://www.ids-imaging.com/

Module configuration
````````````````````
Previously to this you have to install the Ueye SDK

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

See :ref:`Compilation`

Installation
`````````````

- After installing ueye modules :ref:`installation`

- And probably Tango server :ref:`tango_installation`


Configuration
``````````````

See the sdk README provide in the ueye module
