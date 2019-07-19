Ueye Tango device
==================

This is the reference documentation of the Ueye Tango device.

you can also find some useful information about the camera models/prerequisite/installation/configuration/compilation in the :ref:`Ueye camera plugin <camera-ueye>` section.

Properties
----------
================= =============== =============== =========================================================================
Property name	  Mandatory	  Default value	  Description
================= =============== =============== =========================================================================
address		  No		  0		  The video address
================= =============== =============== =========================================================================


Attributes
----------
This device has no attribute.

Commands
--------

=======================	=============== =======================	===========================================
Command name		Arg. in		Arg. out		Description
=======================	=============== =======================	===========================================
Init			DevVoid 	DevVoid			Do not use
State			DevVoid		DevLong			Return the device state
Status			DevVoid		DevString		Return the device state as a string
getAttrStringValueList	DevString:	DevVarStringArray:	Return the authorized string value list for
			Attribute name	String value list	a given attribute name
=======================	=============== =======================	===========================================






