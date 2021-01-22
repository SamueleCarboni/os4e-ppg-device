<h1>os4e-ppg-device</h1>

<h2>Project structure</h2>

This project implements a Yocto layer (version 3.1 "_dunfell_") in order to simulate a Photopletismography (PPG) sensor and a test application to be deployed on a Raspberry Pi 3B+.

In this layer, two parts are deployed:
1. A Linux character-based driver, which code can be found into recipes-modules directory. The driver reads pre-defined values from a file.
2. The application (in C language) which code is into recipes-application folder. The application reads data from the device every 20ms (50Hz frequency) and performs a FFT on 2048 points. Then it computes the Power Spectral Density and finally the heart rate in bpm (beats per minute) that is displayed on screen.

<h2>Build system setup</h2>

The whole build and the following process has been tested under Ubuntu 18.04 distro.

<h4>Apt Packages</h4>
After installing the Linux distribution on the build machine, it's necessary to install some apt packages in order to setup the build environment:

<pre>
sudo apt-get update
sudo apt-get install gawk wget git diffstat unzip texinfo gcc-multilib build-essential chrpath socat libsdl1.2-dev xterm ncurses-dev lzop minicom u-boot-tools curl net-tools python2.7 python -y
</pre>

<h4>Prerequired layers</h4>
After the apt packages are installed, it's time to download all the necessary layers:

<pre>
git clone -b dunfell http://git.yoctoproject.org/git/poky
cd poky
git clone -b dunfell git://git.openembedded.org/meta-openembedded
git clone -b dunfell https://github.com/meta-qt5/meta-qt5
git clone -b dunfell git://git.yoctoproject.org/meta-security
git clone -b dunfell git://git.yoctoproject.org/meta-raspberrypi
</pre>

<h4>Clone this repository</h4>
While still in poky directory, download also this repository in this way:

<pre>
git clone git@gitlab.com:Carboni/os4e-ppg-device.git
</pre>

The last cloning operation brings down this layer and in particular the folder named **_meta-custom_** is present inside.

For better clarity, the tree of poky directory is shown here:

<pre>
samuele@samuele:~/poky$ ls -l
total 96
-rw-r--r--  1 samuele samuele   834 Dec 24 08:43 LICENSE
-rw-r--r--  1 samuele samuele 15394 Dec 24 08:43 LICENSE.GPL-2.0-only
-rw-r--r--  1 samuele samuele  1286 Dec 24 08:43 LICENSE.MIT
-rw-r--r--  1 samuele samuele   244 Dec 24 08:43 MEMORIAM
-rw-r--r--  1 samuele samuele   809 Dec 24 08:43 README.OE-Core
lrwxrwxrwx  1 samuele samuele    30 Dec 24 08:43 README.hardware -> meta-yocto-bsp/README.hardware
lrwxrwxrwx  1 samuele samuele    21 Dec 24 08:43 README.poky -> meta-poky/README.poky
-rw-r--r--  1 samuele samuele   529 Dec 24 08:43 README.qemu
drwxr-xr-x  6 samuele samuele  4096 Dec 24 08:43 bitbake
drwxr-xr-x  3 samuele samuele  4096 Dec 24 08:43 contrib
drwxr-xr-x 15 samuele samuele  4096 Dec 24 08:43 documentation
drwxr-xr-x 19 samuele samuele  4096 Dec 24 08:43 meta
drwxr-xr-x 14 samuele samuele  4096 Dec 24 08:44 meta-openembedded
drwxr-xr-x  5 samuele samuele  4096 Dec 24 08:43 meta-poky
drwxr-xr-x 12 samuele samuele  4096 Dec 24 08:44 meta-qt5
drwxr-xr-x 18 samuele samuele  4096 Dec 24 08:45 meta-raspberrypi
drwxr-xr-x 20 samuele samuele  4096 Dec 24 08:44 meta-security
drwxr-xr-x  9 samuele samuele  4096 Dec 24 08:43 meta-selftest
drwxr-xr-x  8 samuele samuele  4096 Dec 24 08:43 meta-skeleton
drwxr-xr-x  8 samuele samuele  4096 Dec 24 08:43 meta-yocto-bsp
-rwxr-xr-x  1 samuele samuele  1293 Dec 24 08:43 oe-init-build-env
drwxr-xr-x  4 samuele samuele  4096 Dec 24 08:45 os4e-ppg-device
drwxr-xr-x  8 samuele samuele  4096 Dec 24 08:43 scripts
</pre>

<h4>Create build directory</h4>
Then, it's necessary to create the build directory by sourcing oe-init-build-env:

<pre>
source oe-init-build-env build
</pre>

This operation creates a folder named _build_ where the image of the system will be created.

<h4>Copy conf files</h4>
Next, the right configuration file has to be copied. 
The right conf file is in meta-custom/conf folder named local.conf. 
From the build directory, copy the file and overwrite current one:

<pre>
cp ../os4e-ppg-device/meta-custom/conf/local.conf conf/local.conf
</pre>

Next, overwrite also conf/bblayer.conf with this content:

<pre>
# POKY_BBLAYERS_CONF_VERSION is increased each time build/conf/bblayers.conf
# changes incompatibly
POKY_BBLAYERS_CONF_VERSION = "2"
BBPATH = "${TOPDIR}"
BBFILES ?= ""
BBLAYERS ?= " \
    path_to_poky/meta \
    path_to_poky/meta-poky \
    path_to_poky/meta-yocto-bsp \
    path_to_poky/meta-openembedded/meta-oe \
    path_to_poky/meta-openembedded/meta-multimedia \
    path_to_poky/meta-openembedded/meta-networking \
    path_to_poky/meta-openembedded/meta-python \
    path_to_poky/meta-raspberrypi \
    path_to_poky/os4e-ppg-device/meta-custom \
"
</pre>

Remember to overwrite _path_to_poky_ with the correct path.
As an example, if you cloned your poky into /home/root/, path_to_poky is equal to /home/root/poky.

<h2>Build execution</h2>

After having done all the previous operations, start the build by typing:

<pre>
bitbake core-image-full-cmdline
</pre>

When the build finishes, flash a micro SD with the file **_tmp/deploy/images/raspberrypi3/core-image-full-cmdline-raspberrypi3.rpi-sdimg_** inside build directory.

Plug the micro SD into the Raspberry and power on the device.

<h2>Software testing</h2>

After booting and logging into the system (username: root and password none), it's possible to see the presence of the module with the following methods:

<pre>
# To see the major id
cat /proc/devices | grep ppg

# To see kernel printk log
dmesg | grep ppg

# To see loaded module
lsmod
</pre>

Next, it's time to create the device. 
In order to do so, launch the script to create the device and verify operation:

<pre>
# Create
create_device.sh

# Verify
ls -l /dev/ppg
</pre>

After the device has been created, it's possible to test it by using the heartrate-app binary:

<pre>
heartrate-app
</pre>

Every about 40/41 seconds, the app prints the sampled value in BPM.
