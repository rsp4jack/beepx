What is it
----------
This little VDD driver fixes a problem with the Windows 7
NTVDM where the PC Speaker output is not wrapped correctly.
This may be due to the fact that the functionality of the
BEEP.SYS System driver was moved to the Usermode Sound Agent.

For further information, see:
http://blogs.msdn.com/larryosterman/archive/2010/01/04/what-s-up-with-the-beep-driver-in-windows-7.aspx

This driver tries to fix the problem by hooking the NTVDM-
Function resposible for  the Beep (LazyBeep) and replace
it with out own implementation that has  various options
to fix the Problem (see below)

System Requirements
-------------------
It is only needed on Windows 7 and 8.

What Options do I have?
-----------------------
There are multiple ways that you can reenable the Support for the
PC Speaker on your system. To understand them, I'll try to explain
the problem:

Nearly every PC has a timer chip (8254) on the Mainboard (today it's mostly
emulated by the SuperIO Chips, so it's not a seperate chip anymore 
like it was back in the old days) which is connected to a little Speaker
inside your chassis. It is used to make Beep noises.
The PC Speaker can be used by the windows HAL (Hardware Abstraction Layer).
It has a function to drive the Timer connected to the speaker and make
Noise with it. The HAL Beeper Functionality was exposed to Applications
via the beep.sys driver in Windows XP and below.
Now if I get this right, there was a Problem to get it to work with
64bit windows or maybe there were other reasons, however Microsoft
redesigned the Audio Stack to operate mostly in Usermode and decided to
modify the Beep.sys driver to not use the PC Speaker in the PC, but instead
generate a sine wave and feed the input to the Soundcard driver to replace
the need for the classic PC Speaker. The HAL function for the beeper is 
still there in Windows 7, but the BEEP.SYS Driver which exposes the
Beeper to the applications was rewritten like mentioned.
This change however makes it a bit slow to react. Beeping now isn't just
done by enabling the 8254 chip via IO Port 61h, but it has to tell the
Usermode Sound Agent to generate a sine wave and feed it to the Soundcard
driver. Normally you can pass a duration to the Beep-Driver, however
it is also possible to just turn the Beeper on (by issuing -1 as duration
when talking directly to the BEEP.SYS driver. Doesn't apply for usermode Beep()
function tough, as the device is opened and closed by this function) and off.
Now from the NTVDM (DOS-Emulation) viewing point, it is not possible to find out
for how long the DOS Application wants to generate a signal. All the DOS-
Emulation gets to know is that the Application wants to turn on or off
the PC Speaker via port 61h. This wasn't a problem with the old BEEP.SYS driver
as it was in fact still directly talking to the PC Speaker.
As DOS Applications assume that they are talking to the real hardware,
which is reacting extremely fast, there are very fast switches to turn the
PC Speaker on and Off. But with the new BEEP.SYS driver the switches are too
fast for the Usermode Sound agent to react, so when it would finally be able
to generate the Beep, the DOS Application has switches off the Speaker again
already leading to the big Problem: You don't hear anything!

The following Pseudocode demonstrates the Problem with the new BEEP.SYS:
Say you want to generate a Beep for 50ms with a Frequency of 300:

// Works, even with new BEEP.SYS, we know the duration:
Do_Beep (300, 50);	

// Doesn't work, too fast!
Do_Beep (300, -1); // Turn on beeper. 
Sleep(50);	   // Wait 50 ms
Do_Beep (0,  0);   // Turn off beeper


Now that you know about the problem, here are the 4 possible Solutions to
this:

1) Restore the old Windows XP BEEP.SYS driver. Using this Option, the old
   PC Speaker gets used again to generate Beeps and you are basically 
   back to the old behaviour.
   However I noticed that even in this case Tone generation in NTVDM had
   problems (turned on and off the PC Speaker too fast, I don't know why),
   so you still need the NTVDM-fix too.
   However you have to have a Copy of the Windows XP BEEP.SYS driver, so
   technically you need a Windows XP license.

2) I implemented my own simple Beep-Driver which has an Interface compatible
   with BEEP.SYS that drives the PC Speaker. So it's basically my own re-
   implementation of the Windows XP BEEP.SYS. I named it BEEPXP.SYS to 
   avoid confusion and to not interfere with the Microsoft BEEP.SYS
   It is listening to the Device BeepXP (wheras BEEP.SYS from Microsoft is
   the device Beep).
   You can tell the NTVDM-Fix to use my driver and it should work fine.
   However this also means that PC Speaker Beep-Functionality is only 
   available in the NTVDM, all other Beep-Requests are still handled by the
   Windows 7 driver.

3) On some Mainboards there is a PC Speaker connector but the Speaker is
   not connected. So you can't hear any beeps. You wouldn't have heard any
   with Windows XP either.
   This may be due to the fact that the PC Builder wanted to save a few bucks
   for the PC Speaker or there was no space for it in the chassis.
   When thinking of Notebooks, you may also think that this applies to them,
   but their BIOS is intelligent enough to remap the PC Speaker to the
   internal Speakers of the integrated Audio card, so you normally don't have
   this Problem on Notebooks.
   Whatever reason it may be that the PC Speaker is not working on your machine,
   you are stuck with the new Windows 7 BEEP.SYS
   Reading the problem description above, you now know that it's basically a
   lacking delay that is causing the problem that you can't hear anything.
   So with this solution, the NTVDM-fix must Sleep a certain amount of time
   after switching on the Beeper. We could also measure the time between 
   switching on and off the Speaker and issue a Beep() afterwards, however we
   cannot predict if an application will turn off the Speaker fast or not,
   so our only solution is to wait far a ceratain amount of time before toggling 
   the Beeper device so we can ensure that the user heard the last signal 
   generated. 200ms turned our to be an acceptable delay.

4) In fact the same as 3) - You don't have a working PC speaker.
   But as the new BEEP.SYS generates a sine wave, this doesn't sound like
   the real PC speaker you used to know. So to overcome this problem, I
   implemented my own DLL names dsbeep.dll which uses DirectSound to
   generate a square wave sound and let spkrfix use this instead of
   BEEP.SYS. 

How to Install
--------------
You can either use the supplied installer to install all the necessary
components, or you can do it by hand, I'll describe both ways here.

a) Using Installer

Depends on what option you choose from above:
Option 1)
   You need to have a copy of the Windows XP BEEP.SYS file around!
   -) Push "Install old BEEP.SYS" and point to your Windows XP BEEP.SYS
   -) Select the first option
   -) "Install VDM Speaker fix"
   -) OK

Option 2) 
   -) Push "Install BEEPXP.SYS"
   -) Select the second option
   -) "Install VDM Speaker fix"
   -) OK

Option 3)
   -) Select the third option
   -) "Install VDM Speaker fix"
   -) OK

Option 4)
   -) Select the fourth option
   -) "Install VDM Speaker fix"
   -) OK


b) Manually installing it

Open command shell as Administrator (rightclick on Command Prompt in 
Start/Programs/Accessories, Run as Administrator)

Depending on what option you choose from above:

Option 1)
   -) cd drivers
      takeown /f beep.sys
   -) You get a message that your user has taken ownership. 
      You must use this [username] in the following command:
      icacls beep.sys /grant [username]:F
   -) move beep.sys beep.sys.WIN7
   -) copy Path\to\xpdriver\beep.sys .
   -) Now continue to with the VDMfix-Installation below.

Option 2)
   -) RUNDLL32.EXE SETUPAPI.DLL,InstallHinfSection DefaultInstall 132 Path\to\beepxp\beepxp.inf
      net start BeepXP
   -) Edit spkrfix.ini in the spkrfix-Subdir:
      Driver=BeepXP
      Delay=0
   -) copy spkrfix.ini %WINDIR%\
   -) Now continue to with the VDMfix-Installation below.

Option 3)
   -) Edit spkrfix.ini in the spkrfix-Subdir:
      Driver=Beep
      Delay=200
   -) copy spkrfix.ini %WINDIR%\
   -) Now continue to with the VDMfix-Installation below.

Option 4)
   -) Edit spkrfix.ini in the spkrfix-Subdir:
      DSBeep=1
      Delay=20
   -) copy spkrfix.ini %WINDIR%\
   -) Now continue to with the VDMfix-Installation below.

Install VDMfix:
RUNDLL32.EXE SETUPAPI.DLL,InstallHinfSection DefaultInstall 132 Path\to\spkrfix\spkrfix.inf


How to Uninstall
----------------
a) Using Installer

Depends on what option you choose from above:
Option 1)
   -) Push "Unstall old BEEP.SYS"
   -) Push "Uninstall VDM Speaker fix"
   -) OK

Option 2)
   -) Push "Uninstall BEEPXP.SYS"
   -) Push "Uninstall VDM Speaker fix"
   -) OK

Option 3)
Option 4)
   -) Push "Uninstall VDM Speaker fix"
   -) OK

b) Manually uninstalling it

Open command shell as Administrator (rightclick on Command Prompt in 
Start/Programs/Accessories, Run as Administrator)

Depending on what option you choose from above:

Option 1)
   -) cd drivers
      del beep.sys
      move beep.sys.WIN7 beep.sys
   -) Now continue to with the VDMfix-Uninstallation below.

Option 2)
   -) net stop BeepXP
      RUNDLL32.EXE SETUPAPI.DLL,InstallHinfSection DefaultUninstall 132 Path\to\beepxp\beepxp.inf
   -) Now continue to with the VDMfix-Uninstallation below.

Option 3)
Option 4)
   -) Continue to with the VDMfix-Uninstallation below.

VDMfix uninstall:
RUNDLL32.EXE SETUPAPI.DLL,InstallHinfSection DefaultUninstall 132 Path\to\spkrfix\spkrfix.inf

Special features
----------------
There is a litte special feature in the VDM fix that tries to circumvent the following Problem:
NTVDM behaviour is a bit inconsistent when an application is running on fullscreen and a
DOS application is issuing a Beep (i.e. printing Charater 7 (BEL) to the terminal).
If the application is running windowed, a Standard MessageBeep ("Ding") is issued in this
case, however if you are running fullscreen, you get a normal PC Speaker BEEP.
Some people may want to have a DING sound even in fullscreen mode, so I have implemented
a dirty workaround: When a Beep is issued with a frequency of 877Hz, then a MessageBeep
is played instead of enabling the Beeper. Of course this is a very dirty workaround,
but it more or less does its job when running simple office applications.
Per defaut, this feature is disabled, but it can be enabled in the installer.
It sets Ding=1 in the spkrfix.ini file.

Compiling
---------
You need the NTDDK Headers to compile. In the Makefile, it
is assumed that NTDDK is installed in C:\NTDDK
The application was compiled using Microsoft Visual C 6.0
Just use nmake or use the VC 6 Project file.

Author
------
Dipl.-Ing. (FH) Ludwig Ertl
mailto: ludwig.ertl@dose.0wnz.at

You can get my other useful DOS-Utilities here:
http://www.waldbauer.com/tmp/reference.php

License
-------
This Application and Sourcecode may be distributed freely.
The Sourcecode is Licensed in Terms of the GNU GPL v3.
See: http://www.gnu.org/licenses/gpl.html

Thanks
------
Thanks to all the users who are still using DOS Applications ;)
