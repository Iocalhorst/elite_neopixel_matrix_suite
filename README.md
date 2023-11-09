# elite_neopixel_matrix_suite
esp-idf(5.1+) based gfx engine for fast led-matrix game/animation development + example applications 

very early stage. but if you want to give it a try : 

clone the repo

replace wifi ssid and password in defines.h

replace host ip4v adress in defines.h and elite_logger.py

idf.py set-target esp32 build flash
python3 elite_logger.py //to see logging output

to copy the assets for the spriteshow example application :

$nc <esp32ip4adress> 9002 to access the cli

to see available cli-commands :

user@esp32matrix:/$ help 

to upload the spriteshow asset-files onto the littleFS partition you can do: 

cd elite_neopixel_matrix_suite/elite_components/elite_pixel_applications/elite_spriteshow/assets

python3 -m http.server

then use the cli command :

user@esp32matrix:/$ getsprites

i'll go to sleep now. cya soon

