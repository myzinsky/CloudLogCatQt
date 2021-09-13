# CloudLogCatQt
Qt app that can be compiled on  Linux, Windows or Mac for providing CAT support for Cloudlog (http://www.cloudlog.co.uk/).
The app uses a connection to FlRig (http://www.w1hkj.com)

# Build (Linux)

```bash
apt install qt5-default
git clone --recursive https://github.com/myzinsky/CloudLogCatQt.git
cd CloudLogCatQt
mkdir build
cd build
qmake ../CloudLogCatQt.pro
make -j4
```

# Settings

The settings are almost self explaining. According to https://github.com/magicbug/Cloudlog/wiki/API create an API key and fill this together with your CloudLog URL e.g. ```https://<CloudLogServer>/index.php/api/radio```into the settings pane. Furthermore, add the conneciton to FlRig. The default settings are ```localhost``` and the port ```12345```

## MacOS:
![Screenshot](https://raw.githubusercontent.com/myzinsky/CloudLogCatQt/master/doc/CloudLogCATQt.png "Screenshot" )

## Windows
![Screenshot](https://raw.githubusercontent.com/myzinsky/CloudLogCatQt/master/doc/Windows.png "Screenshot" )

The app is still in development and therefore it can not be considered as fully stable.
