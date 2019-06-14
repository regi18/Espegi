 /***************************************************
 Espegi - Sonoff (esp8266 based) control with Blynk and web interface
 Created by: regi18
 Version: Version: 3.5.0
 Github: 

 regi18.ml   |   regi18.github.io
 **************************************************/

const char* INDEX_START = "<!DOCTYPE html><html><head> <meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/> <meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate'> <meta http-equiv='Pragma' content='no-cache'> <meta http-equiv='Expires' content='0'> <style>body{text-align: center; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;}div{padding: 5px; margin: auto; width: 30%;}@media only screen and (max-width: 600px){div{width: 100%; padding: 0;}}table{margin: auto; padding: 1.5em 0 2.5em 0; width: 99.7%;}button{border: 0; border-radius: 0.6rem; background-color: #1fb3ec; color: rgb(252, 252, 252); line-height: 2.4rem; font-size: 1.2rem; background-color: #008CBA; width: 100%; margin-bottom: 1em;}</style> <title>ESPEGI</title></head><body> <div> <h1 id='state' style='font-size: 3.5em;'>";
// state
const char* INDEX_PART1 = "</h1>";
#ifdef DHT_SENSOR
    const char* INDEX_DHT_1 = " <table> <tr> <td style='text-align: left'><b>Temperature</b></td><td style='text-align: right'>";
    //temp
    const char* INDEX_DHT_2 = "&#xB0;C</td></tr><tr> <td style='text-align: left;'><b>Humidity</b></td><td style='text-align: right'>";
    //hum
    const char* INDEX_DHT_END = "%</td></tr></table> ";
#endif
const char* INDEX_PART2 = "<form action='/toggle' method='get'><button>Toggle</button></form> <form action='/update' method='get'><button>Firmware Update</button></form> <p style='position: absolute; bottom: 1.5em; width: inherit;'>Hostname: ";
//hostname
const char* INDEX_END = "</p><p style='position: absolute; bottom: 0; width: inherit;'> <a style='color: black;' href='/forcereload'>force reload</a>&emsp;|&emsp; <a style='color: black;' href='/restart'>restart</a> </p></div></body></html>";


#define UPDATE_PAGE_PART1 "<!DOCTYPE html><html><head> <style>body{text-align: center; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;}div{padding: 5px; margin: auto; width: 30%;}@media only screen and (max-width: 600px){div{width: 100%; padding: 0;}}#b{border: 0; border-radius: 0.6rem; background-color: #1fb3ec; color: rgb(252, 252, 252); line-height: 2.4rem; font-size: 1.2rem; background-color: #008CBA; width: 100%;}</style> <meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/> <title>ESPEGI</title></head><body> <div> <h1 style='font-size: 3.5em;'>Update</h1> <form method='POST' action='/startupdate' enctype='multipart/form-data' style='padding-top: 2em;'> <input type='file' name='update'><br/><br/> <input type='submit' value='Update' id='b'> </form> <p style='position: absolute; bottom: 0; width: inherit;'>Hostname: "
// hosname
#define UPDATE_PAGE_PART2 "</p></div></body></html>"

#define UPDATE_MESSAGE "<p style='font-size: 2.5em; text-align: center; font-family: \"Segoe UI\", Tahoma, Geneva, Verdana, sans-serif;'>"
#define END_UPDATE_MESSAGE "</p><br><br><br><br><p style='text-align: center; font-family: \"Segoe UI\", Tahoma, Geneva, Verdana, sans-serif;'><a href='/'>Homepage</a></p>"
