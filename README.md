# CloudLogCatQt
Qt app that can be compiled on  Linux, Windows or Mac for providing CAT support for Cloudlog (http://www.cloudlog.co.uk/).
The app uses a connection to FlRig (http://www.w1hkj.com)

# Table of Contents

- [CloudLogCatQt](#cloudlogcatqt)
- [Table of Contents](#table-of-contents)
- [Build](#build)
  - [Linux](#linux)
  - [Windows](#windows)
- [Settings](#settings)
  - [MacOS:](#macos)
  - [Windows](#windows-1)

# Build

## Linux

```bash
apt install qt5-default
git clone --recursive https://github.com/myzinsky/CloudLogCatQt.git
cd CloudLogCatQt
mkdir build
cd build
qmake ../CloudLogCatQt.pro
make -j4
```

## Windows

For building CloudLogCatQt on Windows 10 follow the steps below.

1. Install [Chocolatey] (The Package Manager For Windows)
2. Open an elevated PowerShell and run  
    `choco install -y qt5-default cmake qtcreator`
3. Start _QT Creator_
4. Open project file [CloudLogCatQt.pro]()
5. Change the projects build settings for _Release_ and add a _Custom Process Step_
     - Command: `%{Qt:QT_INSTALL_BINS}\windeployqt.exe`
    - Arguments: `%{buildDir}\release`
    - Working directory: `%{buildDir}\release`
6. Click on _Build Project_ [`CTRL+B`]
7. The release and all it's dependencies should now be located in the _Build directory_ as defined in the Projects Build Settings (e.g. in _build-CloudLogCatQt-Desktop-Release/release_ next to your project)

_Read more on the deployment process: [Qt for Windows - Deployment]_


# Settings

The settings are almost self explaining. According to https://github.com/magicbug/Cloudlog/wiki/API create an API key and fill this together with your CloudLog URL e.g. ```https://<CloudLogServer>/index.php/api/radio```into the settings pane. Furthermore, add the conneciton to FlRig. The default settings are ```localhost``` and the port ```12345```

## MacOS:
![Screenshot](https://raw.githubusercontent.com/myzinsky/CloudLogCatQt/master/doc/CloudLogCATQt.png "Screenshot" )

## Windows
![Screenshot](https://raw.githubusercontent.com/myzinsky/CloudLogCatQt/master/doc/Windows.png "Screenshot" )

The app is still in development and therefore it can not be considered as fully stable.



[Chocolatey]: https://chocolatey.org/install
[Qt for Windows - Deployment]: https://doc.qt.io/qt-5/windows-deployment.html
