![AutoPutty.png](/AutoPutty.png "AutoPutty") - PuTTY manager
=========
AutoPutty is a portable, well-organized, and easy to use PuTTY connection manager.  It was originally a very small focused project to auto login into your putty sessions, but quickly grew into a very elegant way to manage putty logins/passwords and sessions.  You can move the windows around, dock and un-dock them, stack them, tab them, etc...  AutoPutty is licensed under GPLv3 and will always continue to be free and open source.
***
Features
--------
- Auto login to putty session
- Docking/Un-docking putty sessions
- Stack putty sessions
- Design your putty session layout the way you like
- Tabbed interface with multiple docking putty sessions per tab
- Easy to use user/password setup screen
- Comes with PuTTY executable packaged with AutoPutty binary
- Right-Click context menus
- Change putty window titles and tab titles for organization
- Stores passwords obfuscated with your own passphrase key

Requirements
---------
- Windows XP or later
- AutoPutty

AutoPutty is statically linked currently to Qt 5+'s libs, packaged with putty executable, and statically linked to Visual C's libs.  The only thing you need to run a released version of AutoPutty is AutoPutty.

That being said AutoPutty will build from Qt's dynamic libraries, it will just need packaged with Qt's libs or installed on any machine running AutoPutty.  AutoPutty should theoretically build with gcc or VC Express also, although it is not currently tested with most recent releases.

Build Prerequisites
---------
- QT 4.8+ (Static or Dynamic)
- Compiler (Visual C, or GCC)
- nmake (Visual C), make, gmake, or mingw32-make
- Qt Creator 2.8+ (Optional)
- Make sure environment has Qt's Bin Path and that your C/C++ build, header, and library environment is set up
- AutoPutty src

Get Source
---------
`git clone https://github.com/jwhitmore/AutoPutty.git AutoPutty`  
`cd AutoPutty`  
`git checkout master`  

Build From CMD.exe
---------
`cd AutoPutty`  
`qmake`  
`make` or `nmake` etc...   

Build From Qt Creator
---------
- Open AutoPutty.pro in Qt Creator
- Set up project with appropriate compiler/kit
- Right Click AutoPutty Project->Run qmake
- Right Click AutoPutty Project->Build










