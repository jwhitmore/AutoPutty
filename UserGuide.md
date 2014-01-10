- [Session Setup](#session-setup)
    - [New Session](#new-session)
    - [Passphrase](#passphrase)
    - [Forgot Passphrase](#forgot-passphrase)
- [User And Password Setup][#user\/password-setup]
- [Opening Sessions](#opening-sessions)
- [Docking](#docking)
- [Putty Menu](#putty-menu)
- [Session Control Window](#session-control-window)
- [Changing Tab Title](#changing-tab-title)
- [Changing Putty Title](#changing-putty-title)

AutoPutty is free and open source software licensed under GPLv3.  AutoPutty does not alter PuTTY or use PuTTY's source in any way.  It manipulates a PuTTY window and organizes it into dockable windows within tabs to be more productive.

AutoPutty will always continue to be free and open, but if you would like to support the project with paypal, you can find a link by going to **Help->About AutoPutty**. 

![AutoPutty-About-1.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-About-1.png?raw=true "AutoPutty-About-1.png")

There you will find links to Paypal support, AutoPutty releases, and link to email support for issues or questions.  The about screen will also show what current version of AutoPutty you are on.

![AutoPutty-About-2.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-About-2.png?raw=true "AutoPutty-About-2.png")

Session Setup
---------

AutoPutty is packaged with the PuTTY executable for convenience.  The sessions are loaded from PuTTY's settings and can be changed still in PuTTY.

### New Session

To create a new session you do what you normally would do in PuTTY by changing it's settings.  From AutoPutty you can go to **Options->Change Putty Settings**.  That will open PuTTY in a separate window and allow you to add, remove, or update sessions.

![AutoPutty-ChangePuttySettings.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-ChangePuttySettings.png?raw=true "AutoPutty-ChangePuttySettings.png")

After changing putty's settings, if you have created or removed any sessions you can have AutoPutty refresh putty's settings by going to **Options->Refresh** Sessions.

![AutoPutty-RefreshSessions.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-RefreshSessions.png?raw=true "AutoPutty-RefreshSessions.png")
### Passphrase

AutoPutty requires a paraphrase to work correctly.  This is a passphrase created by you and is your key for encoding and decoding user passwords you have for your sessions.  Once you are set up, this passphrase will be the only thing you will need to login to your sessions.

### Forgot passphrase

Easy just pick a new one then follow the guide for "**User and password setup**" with the new passphrase.

User/Password Setup
---------

Go to **Options->User Setup**

![AutoPutty-UserSetup-1.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-UserSetup-1.png?raw=true "AutoPutty-UserSetup-1.png")

Enter your passphrase and hit the Enter key if your passphrase was filled in previously on the main screen it will auto-populate this field with that passphrase.

![AutoPutty-UserSetup-2.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-UserSetup-2.png?raw=true "AutoPutty-UserSetup-2.png")

Here you can select your sessions. You can select all of them if they all use the same username and password or multi-select them by CTRL+click and pick the ones with the same usernames and passwords.

![AutoPutty-UserSetup-3.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-UserSetup-3.png?raw=true "AutoPutty-UserSetup-3.png")

After they are selected you can enter your username and password above and click **Update**.

![AutoPutty-UserSetup-4.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-UserSetup-4.png?raw=true "AutoPutty-UserSetup-4.png")

You will see your username and password was updated in the table.  This is a preview nothing has saved yet.  You can view your decoded password by clicking the combo box **Show Passwords** to make sure they are right.

![AutoPutty-UserSetup-5.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-UserSetup-5.png?raw=true "AutoPutty-UserSetup-5.png")

Once you have set up your usernames and passwords for your sessions you can click **Save** and your done!

![AutoPutty-UserSetup-6.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-UserSetup-6.png?raw=true "AutoPutty-UserSetup-6.png")

![AutoPutty-UserSetup-7.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-UserSetup-7.png?raw=true "AutoPutty-UserSetup-7.png")


Opening Sessions
---------

Once the usernames/passwords are setup you can start using AutoPutty.  Enter your unique passphrase in the **Session Control** box.  You can double-click the sessions, multi-select and click login, right click to open in new/current tabs or standalone, or use the Sessions menu item.  

![AutoPutty-Main-3.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-Main-3.png?raw=true "AutoPutty-Main-3.png")

![AutoPutty-SessionMenu.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-SessionMenu.png?raw=true "AutoPutty-SessionMenu.png")

If you double-click a session or use the Login button, AutoPutty uses a "**Default Tab Action**" to open in new/current tab.  This can be changed by going to **Options->Tab Control->Default Tab Action**.  If it is checked the default action is to open in the current focused tab, if not it will open your sessions in a new tab by default.

![AutoPutty-DefaultTabAction.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-DefaultTabAction.png?raw=true "AutoPutty-DefaultTabAction.png")

Docking
---------

Now you can tab, dock, un-dock, and stack your sessions how you see fit.  Just drag the windows around.

![AutoPutty-Undock.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-Undock.png?raw=true "AutoPutty-Undock.png")

![AutoPutty-Docking.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-Docking.png?raw=true "AutoPutty-Docking.png")

Putty Menu
---------

To access putty's settings or menu from an open window, just focus the putty window and hit **Left Alt** on your keyboard.

![AutoPutty-LALTPuttySettings.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-LALTPuttySettings.png?raw=true "AutoPutty-LALTPuttySettings.png")


Session Control Window
---------

You can show or hide the session control window a couple of different ways...

The menu by going to **View->Session Control**

![AutoPutty-ViewSessionControl-1.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-ViewSessionControl-1.png?raw=true "AutoPutty-ViewSessionControl-1.png")

You can just close it by clicking the "X" at the top right corner

![AutoPutty-ViewSessionControl-1-1.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-ViewSessionControl-1-1.png?raw=true "AutoPutty-ViewSessionControl-1-1.png")


And the right-click context menu has a **View->Session Control**

![AutoPutty-ViewSessionControl-2.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-ViewSessionControl-2.png?raw=true "AutoPutty-ViewSessionControl-2.png")


Changing Tab Title
---------

Right click on AutoPutty to open the context menu. Go **Tab Control->Change Tab Title**.  **NOTE: this will change only the focused tab's title.  Remember to focus the tab you want to change the title to first then change the title.**

![AutoPutty-ChangeTabTitle-1.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-ChangeTabTitle-1.png?raw=true "AutoPutty-ChangeTabTitle-1.png")  

Changing Putty Title
---------

Right Click the top of the putty session and choose **Change Title**

![AutoPutty-ChangeTitle-1.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-ChangeTitle-1.png?raw=true "AutoPutty-ChangeTitle-1.png")  

![AutoPutty-ChangeTitle-2.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-ChangeTitle-2.png?raw=true "AutoPutty-ChangeTitle-2.png")  

![AutoPutty-ChangeTitle-3.png](https://github.com/jwhitmore/AutoPutty/blob/gh-pages/images/AutoPuttyGuide/AutoPutty-ChangeTitle-3.png?raw=true "AutoPutty-ChangeTitle-3.png")  
