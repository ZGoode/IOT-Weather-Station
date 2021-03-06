String parseHomePage();
String parseConfigurePage();

String footer = "<!-- Footer -->"
                "<footer class='w3-bottom w3-center w3-black w3-padding-small w3-opacity w3-hover-opacity-off'>"
                "<div class='w3-xlarge'>"
                "<a href='https://github.com/ZGoode/IOT-Weather-Station'><i class='fa fa-github w3-hover-opacity'></i></a>"
                "<a href='https://twitter.com/FlamingBandaid'><i class='fa fa-twitter w3-hover-opacity'></i></a>"
                "<a href='http://linkedin.com/in/zachary-goode-724441160'><i class='fa fa-linkedin w3-hover-opacity'></i></a>"
                "</div>"
                "</footer>"
                ""
                "</body>"
                "</html>";

String header = "<!DOCTYPE html>"
                "<html>"
                "<title>IOT Cloud</title>"
                "<meta charset='UTF-8'>"
                "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                "<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>"
                "<link rel='stylesheet' href='https://fonts.googleapis.com/css?family=Lato'>"
                "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>"
                "<style>"
                "body,h1,h2,h3,h4,h5,h6 {font-family: 'Lato', sans-serif;}"
                "body, html {"
                "height: 100%;"
                "color: #777;"
                "line-height: 1.8;"
                "}"
                "/* Create a Parallax Effect */"
                ".bgimg{"
                "background-attachment: fixed;"
                "background-position: center;"
                "background-repeat: no-repeat;"
                "background-size: cover;"
                "}"
                "/* Background Picture */"
                ".bgimg{"
                "background-image: url('%BACKGROUND_IMAGE%');"
                "min-height: 100%;"
                "}"
                ".w3-wide {letter-spacing: 10px;}"
                ".w3-hover-opacity {cursor: pointer;}"
                "</style>"
                "<body>"
                "<!-- Navbar (sit on top) -->"
                "<div class='w3-top'>"
                "<div class='w3-bar' id='myNavbar'>"
                "<a href='Home' class='w3-bar-item w3-button'>HOME</a>"
                "<a href='Configure' class='w3-bar-item w3-button w3-hide-small'><i class='fa fa-cogs'></i> CONFIGURE</a>"
                "<a href='https://github.com/ZGoode/IOT-Weather-Station' class='w3-bar-item w3-button w3-hide-small'><i class='fa fa-th'></i> ABOUT</a>"
                "<a href='/WifiReset' class='w3-bar-item w3-button w3-hide-small w3-right w3-hover-red'>WIFI RESET</a>"
                "<a href='/FactoryReset' class='w3-bar-item w3-button w3-hide-small w3-right w3-hover-red'>FACTORY RESET</a>"
                "</div>"
                "</div>";

String homePage = "<!-- First Parallax Image with Logo Text -->"
                  "<div class='bgimg w3-display-container w3-opacity-min' id='home'>"
                  "<div class='w3-display-middle' style='white-space:nowrap;'>"
                  "<p><span class='w3-center w3-padding-large w3-black w3-xlarge w3-wide w3-animate-opacity'>IOT<span class='w3-hide-small'> WEATHER</span> STATION</span></p>"
                  "</div>"
                  "</div>";

String configurePage = "<div class='bgimg w3-display-container w3-opacity-min' id='home'>"
                       "<div class='w3-display-middle' style='white-space:nowrap;'>"
                       "<form class='w3-container' action='/updateConfig' method='get'><h2>Cloud Config:</h2>"
                       "<p><label>User ID (for this interface)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='userid' value='%USERID%' maxlength='20'></p>"
                       "<p><label>Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='stationpassword' value='%STATIONPASSWORD%'></p>"
                       "<p><label>OTA Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='otapassword' value='%OTAPASSWORD%'></p>"
                       "<p><label>MQTT IP Address </label><input class='w3-input w3-border w3-margin-bottom' type='text' name='mqttipaddress' value='%IPADDRESS%'></p>"
                       "<p><label>MQTT Username </label><input class='w3-input w3-border w3-margin-bottom' type='text' name='mqttusername' value='%USERNAME%'></p>"
                       "<p><label>MQTT Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='mqttpassword' value='%PASSWORD%'></p>"
                       "<p><label>MQTT Client Name </label><input class='w3-input w3-border w3-margin-bottom' type='text' name='mqttclientname' value='%CLIENTNAME%'></p>"
                       "<p><label>MQTT Port </label><input class='w3-input w3-border w3-margin-bottom' type='text' name='mqttport' value='%PORT%' onkeypress='return isNumberKey(event)'></p>"
                       "<button class='w3-button w3-block w3-grey w3-section w3-padding' type='submit'>Save</button>"
                       "</form>"
                       "</div>"
                       "</div>";

String parseHomePage() {
  return header + homePage + footer;
}

String parseConfigurePage() {
  return header + configurePage + footer;
}
