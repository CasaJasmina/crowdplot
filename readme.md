#CROWDPLOT

Crowdplot is a connected vertical plotter machine that sketch images drawn by user trough a webapp. 


![alt tag](https://raw.github.com/CasaJasmina/crowdplot/master/crowdplot.gif)



The web app is in charge of letting people draw and generate a gcode;
A processing sketch fetch the Gcode, does some conversions and send it to the Arduino;
The Arduino does the rest! it parse the gcode and activates the motors:

##TODO
- Use an Arduino Yun to fetch the Gcode and Avoid the use of processing
- Fix the Arduino firmware and add a way to calculate the changes in the spool diameter
- Create a better domunentation!

##CREDITS
- The project was done by Lorenzo Romagnoli, assisted by Gianluca Owen and Federico Vanzati.
- The mechanics is based on [OpenWall (https://github.com/crassociati/openWall)] by [Pietro Leoni (pietroleoni.com)]
- The Arduino Firmware is a modified version of [MakeLangelo (https://github.com/MarginallyClever/Makelangelo)]
