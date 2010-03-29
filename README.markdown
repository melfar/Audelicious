# Audelicious — sound appreciation app for Nokia N900

![screenshot](http://github.com/melfar/audelicious/tree/master/images/screenshot.jpg?raw=true)

## Installation

* Install and setup [scratchbox](http://wiki.forum.nokia.com/index.php/Maemo_5_SDK_installation_for_beginners)

* Download [fftw3](http://www.fftw.org/download.html) and install to /usr (build-time dependency):

    wget http://www.fftw.org/fftw-3.2.2.tar.gz
    tar xzf fftw-3.2.2.tar.gz && cd fftw-3.2.2
    ./configure --prefix=/usr
    make && make install

* You need [Qt 4.6](http://qt.nokia.com/products/platform/maemo) installed in scratchbox to proceed.
 
    qmake && make
   
* Copy to device, make sure you have Qt 4.6 installed on the device.  If you have both 4.5 and 4.6, move 4.5 from the linker path and set env `LD_LIBRARY_PATH` to qt4.6 directory.  *NOTE*: you won't need to worry about it on PR 1.2 :)
 
* run `./AudeliciousQT` from the terminal or install `.desktop` file to application manager.  Enjoy!
