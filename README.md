# QT_Vending_Machine_VCOM

This a QT project about vending machine scenario

the code is based on QT cross-compile enviroment:

###https://github.com/steven-wj-wu/qt5_cross_compilation_pi4b_survey

IF need to use GPIO function 

the destination device need to install WiringPi for Debian OS:bulleye follow by: https://github.com/WiringPi/WiringPi

  ###can install simply as bellow:
  
  git clone https://github.com/WiringPi/WiringPi
  
  cd WiringPi
  
  ./build
  
the virtual comport function is used to communicate with NXP MCU(K82)


touch thread is use to get touch point response by device event

##Enviroment:

  Touch Device: Focaltech-FT7251 (Driver needed)
  
  Destination Device: Raspberry Pi 4 (BullEye 32bit)
  
