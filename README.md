# elite_neopixel_matrix_suite
esp-idf(5.1+) based gfx engine for fast led-matrix game/animation development + example applications. batteries not included

Theres a playlist with demo footage on my YT-Channel.

https://www.youtube.com/playlist?list=PLHSZrglLne2v1WkVeYQztqZad_cIJlpPZ


The purpose of this repository is to 

(a-1)) reveal how badly i suck at using git. omfg

a) migrate my neopixel-related arduino code base to esp-idf. its a lot of stuff. and i'd rather choose not to. but its inevitable, mr anderson.

b) Implement all the fun stuff and pretty things. My inner child does not negotiate.

c) Do it "properly". I know how not to do things. Knowing better does very little when its about doing so. my wifi creds are in defines.h. fwiw

Its at a very early stage. And I'm learning this stuff as i go. please take that into account.

Right now Display size is 10x30 pixels - hardcoded. if you want to give it a try or customize feel free  : 

Of course it can play snake, tetris and run a pixelart slideshow. I hacked a shadowcasting demo as well. And im working on some sort of paint application.... 
You will need an esp32 board. If you dont have enough blinking lights ... you should do something about it. In the meantime you can work around it by utilizing the streaming capability. 
It throws the raw rgb888 pixels at you via udp if you "startudp" in the cli. Ill put the client/viewer-code up here soon. 

clone the repo

replace wifi ssid and password in defines.h

replace host ip4v adress in defines.h and elite_logger.py

idf.py set-target esp32 build flash
python3 elite_logger.py //to see logging output

to copy the assets for the spriteshow example application :

$nc <esp32ip4adress> 9002 to access the cli

to see available cli-commands :

user@esp32matrix:/$ help 

you can try the "rain" "tetris" "template" applications without further steps.
you must use "kill" command before starting a different animation.
"reboot" if things go wrong.

before using the "spriteshow" command to start the animation, the littleFS partition needs to be formatted
this should happen automatically. then you need to upload the files from the asset folder. 
to upload the spriteshow asset-files onto the littleFS partition you can do: 
cd elite_neopixel_matrix_suite/elite_components/elite_pixel_applications/elite_spriteshow/assets

python3 -m http.server

then use the cli command :

user@esp32matrix:/$ getsprites

or use "server" command in the cli to start the (webbased) fileserver running on port 80 and do it manually via browser from your host machine- which is tedious.

i'll go to sleep now. cya soon

