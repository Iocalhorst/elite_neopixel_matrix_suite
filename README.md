# elite_neopixel_matrix_suite
esp-idf(5.1+) based gfx engine for fast led-matrix game/animation development + example applications 

very early stage. display size is 10x30 pixels - hardcoded. if you want to give it a try or customize feel free  : 

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

