/*************************************************************
 *  Beeper Device FIX for Windows 7 NTVDM -- Beeper driver
 *
 *  (C)oded by Dipl.-Ing. (FH) Ludwig Ertl, 2010
 *
 * This is a simple driver that implements the BEEP.SYS 
 * interface and interfaces with the HAL Beeper.
 * Based on ReactOS BEEP.SYS driver.
 *
 *************************************************************
 * Module:   beep.c
 * Descr.:   Driver module
 * License:  GPL 3
 * Date  :   07.01.2010
 * Changelog:
 *************************************************************/

/* INCLUDES ****************************************************************/

#include <ntddk.h>
#include <ntddbeep.h>

#define STDCALL
#define RTL_CONSTANT_STRING(__SOURCE_STRING__)                  \
{                                                               \
    sizeof(__SOURCE_STRING__) - sizeof((__SOURCE_STRING__)[0]), \
    sizeof(__SOURCE_STRING__),                                  \
    (__SOURCE_STRING__)                                         \
}


NTSTATUS STDCALL 
DriverEntry(PDRIVER_OBJECT DriverObject,
            PUNICODE_STRING RegistryPath);

/* TYEPEDEFS ***************************************************************/

typedef struct _BEEP_DEVICE_EXTENSION
{
  KDPC Dpc;
  KTIMER Timer;
  BOOLEAN BeepOn;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


/* FUNCTIONS ***************************************************************/

static VOID STDCALL
BeepDPC(PKDPC Dpc,
	PVOID DeferredContext,
	PVOID SystemArgument1,
	PVOID SystemArgument2)
{
  PDEVICE_EXTENSION DeviceExtension = DeferredContext;

  HalMakeBeep(0);
  DeviceExtension->BeepOn = FALSE;
}


static NTSTATUS STDCALL
BeepCreate(
   PDEVICE_OBJECT DeviceObject,
	 PIRP Irp)
/*
 * FUNCTION: Handles user mode requests
 * ARGUMENTS:
 *                       DeviceObject = Device for request
 *                       Irp = I/O request packet describing request
 * RETURNS: Success or failure
 */
{
  Irp->IoStatus.Status = STATUS_SUCCESS;
  Irp->IoStatus.Information = 0;
  IoCompleteRequest(Irp,
		    IO_NO_INCREMENT);

  return(STATUS_SUCCESS);
}


static NTSTATUS STDCALL
BeepClose(PDEVICE_OBJECT DeviceObject,
	  PIRP Irp)
/*
 * FUNCTION: Handles user mode requests
 * ARGUMENTS:
 *                       DeviceObject = Device for request
 *                       Irp = I/O request packet describing request
 * RETURNS: Success or failure
 */
{
  PDEVICE_EXTENSION DeviceExtension;
  NTSTATUS Status;


  DeviceExtension = DeviceObject->DeviceExtension;
  HalMakeBeep(0);
  if (DeviceExtension->BeepOn == TRUE)
    {
      DeviceExtension->BeepOn = FALSE;
      KeCancelTimer(&DeviceExtension->Timer);
    }

  Status = STATUS_SUCCESS;

  Irp->IoStatus.Status = Status;
  Irp->IoStatus.Information = 0;
  IoCompleteRequest(Irp,
		    IO_NO_INCREMENT);

  return(Status);
}


static NTSTATUS STDCALL
BeepCleanup(PDEVICE_OBJECT DeviceObject,
	    PIRP Irp)
/*
 * FUNCTION: Handles user mode requests
 * ARGUMENTS:
 *                       DeviceObject = Device for request
 *                       Irp = I/O request packet describing request
 * RETURNS: Success or failure
 */
{
  Irp->IoStatus.Status = STATUS_SUCCESS;
  Irp->IoStatus.Information = 0;
  IoCompleteRequest(Irp,
		    IO_NO_INCREMENT);

  return(STATUS_SUCCESS);
}


static NTSTATUS STDCALL
BeepDeviceControl(PDEVICE_OBJECT DeviceObject,
		  PIRP Irp)
/*
 * FUNCTION: Handles user mode requests
 * ARGUMENTS:
 *                       DeviceObject = Device for request
 *                       Irp = I/O request packet describing request
 * RETURNS: Success or failure
 */
{
  PIO_STACK_LOCATION Stack;
  PDEVICE_EXTENSION DeviceExtension;
  PBEEP_SET_PARAMETERS BeepParam;
  LARGE_INTEGER DueTime;

  DeviceExtension = DeviceObject->DeviceExtension;
  Stack = IoGetCurrentIrpStackLocation(Irp);
  BeepParam = (PBEEP_SET_PARAMETERS)Irp->AssociatedIrp.SystemBuffer;

  Irp->IoStatus.Information = 0;

  if (Stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_BEEP_SET)
    {
      Irp->IoStatus.Status = STATUS_NOT_IMPLEMENTED;
      IoCompleteRequest(Irp,
			IO_NO_INCREMENT);
      return(STATUS_NOT_IMPLEMENTED);
    }

  if ((Stack->Parameters.DeviceIoControl.InputBufferLength != sizeof(BEEP_SET_PARAMETERS))
      || ((BeepParam->Frequency < BEEP_FREQUENCY_MINIMUM && BeepParam->Duration > 0))
      || (BeepParam->Frequency > BEEP_FREQUENCY_MAXIMUM))
    {
      Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
      IoCompleteRequest(Irp,
			IO_NO_INCREMENT);
      return(STATUS_INVALID_PARAMETER);
    }

  DueTime.QuadPart = 0;

  /* do the beep!! */
  if ((LONG)BeepParam->Duration > 0)
    {
      DueTime.QuadPart = (LONGLONG)BeepParam->Duration * -10000;

      KeSetTimer(&DeviceExtension->Timer,
		 DueTime,
		 &DeviceExtension->Dpc);
      DeviceExtension->BeepOn = (BeepParam->Frequency>0)?TRUE:FALSE;
    }

  HalMakeBeep(BeepParam->Frequency);


  Irp->IoStatus.Status = STATUS_SUCCESS;
  IoCompleteRequest(Irp,
		    IO_NO_INCREMENT);
  return(STATUS_SUCCESS);
}


VOID STDCALL
BeepUnload(PDRIVER_OBJECT DriverObject)
{
  IoDeleteDevice (DriverObject->DeviceObject);
  DbgPrint ("BEEPXP unloaded");
}


NTSTATUS STDCALL
DriverEntry(PDRIVER_OBJECT DriverObject,
	    PUNICODE_STRING RegistryPath)
/*
 * FUNCTION:  Called by the system to initalize the driver
 * ARGUMENTS:
 *            DriverObject = object describing this driver
 *            RegistryPath = path to our configuration entries
 * RETURNS:   Success or failure
 */
{
  PDEVICE_EXTENSION DeviceExtension;
  PDEVICE_OBJECT DeviceObject;
  UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\BeepXP");
  NTSTATUS Status;


  DriverObject->MajorFunction[IRP_MJ_CREATE] = BeepCreate;
  DriverObject->MajorFunction[IRP_MJ_CLOSE] = BeepClose;
  DriverObject->MajorFunction[IRP_MJ_CLEANUP] = BeepCleanup;
  DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = BeepDeviceControl;
  DriverObject->DriverUnload = BeepUnload;

  Status = IoCreateDevice(DriverObject,
			  sizeof(DEVICE_EXTENSION),
			  &DeviceName,
			  FILE_DEVICE_BEEP,
			  0,
			  FALSE,
			  &DeviceObject);
  if (!NT_SUCCESS(Status))
    return Status;

  /* set up device extension */
  DeviceExtension = DeviceObject->DeviceExtension;
  DeviceExtension->BeepOn = FALSE;

  KeInitializeDpc(&DeviceExtension->Dpc,
		  BeepDPC,
		  DeviceExtension);
  KeInitializeTimer(&DeviceExtension->Timer);

  DbgPrint ("BEEPXP loaded");
  return(STATUS_SUCCESS);
}

/* EOF */

