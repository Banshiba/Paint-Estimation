#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
void notfound(AsyncWebServerRequest *request)
{
  request->send(404,"text/plain","Page not found");
}

#define outputA 5
 #define outputB 4
 #define sensor 36
 long counterwin1=0,counterwin2=0,counterdoor1=0,counterdoor2=0,counterwall1=0,counterwall2=0;
 //int aState;
 //int aLastState;
 int aStatewin1,aLastStatewin1,aStatewin2,aLastStatewin2,aStatedoor1,aLastStatedoor1,aStatedoor2,aLastStatedoor2,aStatewall1,aLastStatewall1,aStatewall2,aLastStatewall2;
 float windowlength=0;  
 float windowheight=0;
 float doorheight=0;
 float doorlength=0;
 float walllength=0;
 float wallheight=0;
 int lengthpin=14;
 int heightpin=34;
 //int reset =14;
 float paint=0;
 float area=0;
 float putty=0;
 float primer=0;
 int x=0;
 float arr[1000]={0};
static float copy;
int j=0;
float moisturecontent=0;
int value,val;


char webpage[] PROGMEM =R"=====(
<!DOCTYPE HTML>
<html>
<head>
<title>Paint estimator</title>
</head>
<body>
<div id="title">
<h1 >Wall moisture and paint estimation tool</h1>
</div>
<div id="formdiv">
<form id="form">
<label>No of rooms :</label>
<input type="number" id="room" value=1><br><br>
<label>No of windows :</label>
<input type="number" id="win" value=1><br><br>
<label>No of doors :</label>
<input type="number" id="door"value=1><br><br>
<label>Ceiling </label>
<input type="checkbox" id="ceiling"><br><br>
<label>Half Height  Approximation</label>
<input type="checkbox" id="half">
<br> <br>
<label>Painting type :</label>
<select id="paintingtype" >
<option value="re-painting">Re-Painting</option>
<option value="fresh-painting">Fresh-Painting</option>
</select>
<br><br>
<label>Paint type:</label>
<select id="painttype">
<option value="Tractor Emulsion">Tractor Emulsion</option>
<option value="Premium Emulsion">Premium Emulsion</option>
<option value="Royal Luxury">Royal Luxury</option>
<option value="Royal Matt">Royal Matt</option>
</select>
<br> <br>
<input type="submit" id="submit" value=Submit>
</form>
</div>
<div id="buttons">
<button id="moisture" onclick="moisture_button()">Moisture</button>
<button id="windowbutton" onclick="window_button()">Window</button>
<button id="doorbutton" onclick="door_button()">Door</button>
<button id="wallbutton" onclick="wall_button()">Wall </button>
</div>
<div id="secbutns">
<button id="reset" onclick="reset_button()">Reset</button>
<button id="result" onclick="result_button()">Result</button>
</div>

<div id="parameters">
<h3 id ="m"class="param">Moisture Level :</h3>
<div class="param">
<h3 id="wl">Window Length :</h3>
<h3 id="wh">Window Height :</h3>
</div>
<div class="param">
<h3 id="dl">Door Length :</h3>
<h3 id="dh">Door Height :</h3>
</div>
<div class="param">
<h3 id="wal">Wall Length :</h3>
<h3 id="wah">Wall Height :</h3>
</div>
</div>

 <div id="params"style="text-align:center; display:none">
 <h3 id="resultarea">Area :</h3>
 <h3 id="resultpaint">Paint :</h3>
 <h3 id="resultputty">Putty :</h3>
 <h3 id="resultprimer">Primer :</h3>
 </div>

<style>
body{
font-family: "Lucida Console", "Courier New", monospace;}
#title{text-align:center;
font style}
#parameters{
data-inline="true";}
.param{

margin :30px;}
#formdiv{margin-top: 30px;}
#moisture{
border-radius: 100%;
background-color:#008CBA;
margin :30px;
border:none;
width:70px;
height:70px;
data-inline="true"}
#windowbutton{

border-radius: 100%;
background-color:#4CAF50;
margin: 30px;
border:none;
width:70px;
height:70px;
data-inline="true"
}
#doorbutton{
border-radius: 100%;
background-color:#f44336;;
margin :30px;
border:none;
width:70px;
height:70px;
data-inline="true"
}
#wallbutton{
border-radius: 100%;
background-color:#A52A2A;
margin :30px;
border:none;
width:70px;
height:70px;
data-inline="true"}


#buttons{
margin-top:30px;
}
#reset{

border-radius: 100%;
background-color:#808080;
margin: 30px;
border:none;
width:70px;
height:70px;
data-inline="true";
color:"#FFFFF";
}
#result{

border-radius: 100%;
background-color:#808080;
margin: 30px;
border:none;
width:70px;
height:70px;
data-inline="true";
color:"#FFFFFF";
}
</style>
<script>
var connection= new WebSocket('ws://'+location.hostname+':81');
let win;
let door;
let room;
let  ceiling;
let half;
let paintingtype;
let painttype;
var form=document.getElementById('form');
form.addEventListener('submit',function(event){
  event.preventDefault();
  win=document.getElementById("win").value;
  console.log(win);
  door=document.getElementById("door").value;
  console.log(door);
  room=document.getElementById("room").value;
  console.log(room);
  ceiling=document.getElementById("ceiling");
  console.log(ceiling);
  half=document.getElementById("half");
  console.log(half);
  paintingtype=document.getElementById("paintingtype").value;
  console.log(paintingtype);
  painttype=document.getElementById("painttype").value ;
  console.log(painttype);
  document.getElementById("formdiv").style.display = "none";
});
var moisture_level;
 var windowLRec=0;
var windowHRec=0;
var doorLRec=0;
var doorHRec=0;
var wallLRec=0;
var wallHRec=0;
 var area=0;
 var paint=0;
 var primer=0;
 var putty=0;
 var resetbutton=0;
 connection.onmessage=function(event){
  var full_data=event.data;
  console.log(full_data);
  var data=JSON.parse(full_data);

  area=data.area;
  paint=data.paint;
  primer=data.primer;
  putty=data.putty;
  windowLRec=data.windowLSent;
  windowHRec=data.windowHSent;
  doorLRec=data.doorLSent;
  doorHRec=data.doorHSent;
  wallLRec=data.wallLSent;
  wallHRec=data.wallHSent;
  moisture_level=data.moistureSent;
  var Area=0;
  if(half.checked=='true')
  wallHRec*=2;
  Area=(wallLRec*wallHRec*2)-(windowLRec*windowHRec*win)-(doorLRec*doorHRec*door);
  if(ceiling.checked=='true')
  {Area+=(wallLRec*wallHRec);}
  Area=Area*10.7639;
  Area=Area*room;
  var Paint=Area/100;
  var Primer=Area/100;
  var Putty=Area/40;
  if(moisture_level<=40)
   {document.getElementById("m").innerHTML = "Moisture level : "+moisture_level+" % Can continue with Area Estimation.";}
  else
    {document.getElementById("m").innerHTML = "Moisture level : "+moisture_level+" % :  Take necessary steps to remove damp.";}
  
  document.getElementById("wl").innerHTML = "Window Length : "+windowLRec+ " meters";
document.getElementById("wh").innerHTML = "Window Height : "+windowHRec+ " meters";
document.getElementById("dl").innerHTML = "Door Length : "+doorLRec+ " meters";
document.getElementById("dh").innerHTML = "Door Height : "+doorHRec+ " meters";
document.getElementById("wal").innerHTML = "Wall Length : "+wallLRec+ " meters";
document.getElementById("wah").innerHTML = "Wall Height : "+wallHRec+ " meters";
  document.getElementById("resultarea").innerHTML = "Area: "+Area.toFixed(2) +" square feet";
  document.getElementById("resultpaint").innerHTML ="Paint: "+Paint.toFixed(2) +" Litres";
  document.getElementById("resultprimer").innerHTML ="Primer: " +Primer.toFixed(2)+" Litres";
  document.getElementById("resultputty").innerHTML ="Putty: " +Putty.toFixed(2)+" Kilograms";
 }
var windowbutton=0;
var moisturebutton=0;
var wallbutton=0;
var doorbutton=0;
var resetbutton=0;
function send_data()
{
var full_data='{"moisture":'+moisturebutton+',"window":'+windowbutton+',"door":'+doorbutton+',"wall":'+wallbutton+',"reset":'+resetbutton+'}';
connection.send(full_data);
}





function window_button(){
windowbutton=!(windowbutton);
if(windowbutton)
{document.getElementById("windowbutton").style.backgroundColor="#93E9BE";}
else
document.getElementById("windowbutton").style.backgroundColor="#4CAF50";
send_data();
}
function moisture_button(){
moisturebutton=!(moisturebutton);                      
if(moisturebutton)
document.getElementById("moisture").style.backgroundColor="#ADD8E6";
else
document.getElementById("moisture").style.backgroundColor="#008CBA";
send_data();}
function door_button(){
doorbutton=!(doorbutton);
if(doorbutton)
{document.getElementById("doorbutton").style.backgroundColor="#D7A4A3";}
else
document.getElementById("doorbutton").style.backgroundColor="#f44336";
send_data();
}

function wall_button(){
wallbutton=!(wallbutton);
if(wallbutton)
{document.getElementById("wallbutton").style.backgroundColor="#C4A484";}
else
document.getElementById("wallbutton").style.backgroundColor="#A52A2A";
send_data();
}

function result_button()
{
   document.getElementById("params").style.display="block";
}
function reset_button()
{
  resetbutton=1;
  send_data();
  resetbutton=0;
}
</script>
</body>
</html>
)=====";
 AsyncWebServer server(80); // server port 80
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String message = String((char*)( payload));
      Serial.println(message);

      
     DynamicJsonDocument doc(200);
    // deserialize the data
    DeserializationError error = deserializeJson(doc, message);
    // parse the parameters we expect to receive (TO-DO: error handling)
      // Test if parsing succeeds.
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }


int sensor_status=doc["moisture"];
int window_status=doc["window"];
int door_status=doc["door"];
int wall_status=doc["wall"];
int reset_status=doc["reset"];

if(doc["moisture"]==true )
{
  Serial.println("Inside software moisture switch");
  while(digitalRead(heightpin)==LOW){
    Serial.println("Inside outer loop");
    int i=0;
  
  int newarr[1000]={0};
  while(digitalRead(lengthpin)==HIGH)
  {
    Serial.println("Inside innner loop");
     value=analogRead(sensor);
    Serial.print("Moisture value: ");
    Serial.println(value);
     val= map(value,4095,0,0,100);
     Serial.print("Data in percent: ");
     Serial.println(val);
    newarr[i]=val;
    i++;
    Serial.print("i= ");
        Serial.print(i);
    delay(1000);  }
    if(i>0)
    {
      i--;
      int xe=i;
      float avg=0;
      for(;i>0;i--)
        {avg+=newarr[i];
        }
        avg/=xe;
        arr[j]=avg;
        Serial.print("Final Value= ");
        Serial.println(arr[j]);
        Serial.print("i= ");
        Serial.print(i);
        j++;
    }
    }
  }

else if((arr[0]!=0 )&& (j)&&(digitalRead(lengthpin)==LOW)&&(digitalRead(heightpin)==HIGH)&& (doc["moisture"]==false))
{
  Serial.println("Inside software moisture off switch");
  Serial.println("Mositure off");
  float average=0;
  j--;
  Serial.print("Value of J ");
  Serial.println(j);
  for( int s=j;s>=0;s--)
  {Serial.print("Arr  Values = ");
  Serial.println(arr[s]);
  average+=arr[s];}
  moisturecontent=average/(j+1);
  Serial.println(moisturecontent);
  String JSON_Data="{\"windowLSent\":";
           JSON_Data+=windowlength/100;
           JSON_Data+=",\"moistureSent\":";
           JSON_Data+=moisturecontent;
           JSON_Data+=",\"windowHSent\":";
           JSON_Data+=windowheight/100;
           JSON_Data+=",\"doorHSent\":";
           JSON_Data+=doorheight/100;
           JSON_Data+=",\"doorLSent\":";
           JSON_Data+=doorlength/100;
           JSON_Data+=",\"wallLSent\":";
           JSON_Data+=walllength/100;
           JSON_Data+=",\"wallHSent\":";
           JSON_Data+=wallheight/100;
           JSON_Data+=",\"area\":";
            area=abs((walllength*wallheight)/10000-(windowlength*windowheight)/10000-(doorlength*doorheight)/10000);
           area=area*10.7639;
           JSON_Data+=area;
           JSON_Data+=",\"paint\":";
           paint=area/100;
           JSON_Data+=paint;
           JSON_Data+=",\"primer\":";
           primer=paint;
           JSON_Data+=primer;
           JSON_Data+=",\"putty\":";
           putty=area/40;
           JSON_Data+=putty;
           JSON_Data+="}";
           websockets.broadcastTXT(JSON_Data);
  
  
}


 else if(doc["window"]==true)
{
  Serial.println("Inside software window switch");
  while(digitalRead(lengthpin)==HIGH)
  {
     aStatewin1 = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aStatewin1 != aLastStatewin1){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aStatewin1) { 
       counterwin1 --;//++
       
     } else {
       counterwin1 ++;//--
     }
   windowlength = counterwin1*1.280;
   Serial.print("window length : ");
    Serial.println(windowlength);
    //Serial.clear();
   }
   aLastStatewin1 = aStatewin1;
  }
  while(digitalRead(heightpin)==HIGH)
  {
    aStatewin2 = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aStatewin2 != aLastStatewin2){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aStatewin2) { 
       counterwin2--;//++
       
     } else {
       counterwin2 ++;//--
     }
   windowheight = counterwin2*1.280;
   Serial.print("window hieght : ");
    Serial.println(windowheight);
    //Serial.clear();
   }
   aLastStatewin2 = aStatewin2;
  }
  Serial.println("Inside software switch");
  String JSON_Data="{\"windowLSent\":";
           JSON_Data+=windowlength/100;
           JSON_Data+=",\"moistureSent\":";
           JSON_Data+=moisturecontent;
           JSON_Data+=",\"windowHSent\":";
           JSON_Data+=windowheight/100;
           JSON_Data+=",\"doorHSent\":";
           JSON_Data+=doorheight/100;
           JSON_Data+=",\"doorLSent\":";
           JSON_Data+=doorlength/100;
           JSON_Data+=",\"wallLSent\":";
           JSON_Data+=walllength/100;
           JSON_Data+=",\"wallHSent\":";
           JSON_Data+=wallheight/100;
           JSON_Data+=",\"area\":";
           area=abs((walllength*wallheight)/10000-(windowlength*windowheight)/10000-(doorlength*doorheight)/10000);
           area=area*10.7639;
           JSON_Data+=area;
           JSON_Data+=",\"paint\":";
           paint=area/100;
           JSON_Data+=paint;
           JSON_Data+=",\"primer\":";
           primer=paint;
           JSON_Data+=primer;
           JSON_Data+=",\"putty\":";
           putty=area/40;
           JSON_Data+=putty;
           JSON_Data+="}";
           websockets.broadcastTXT(JSON_Data);
  }
  else  if(doc["door"]==true)
  {
    Serial.println("Inside software door switch");
    while(digitalRead(lengthpin)==HIGH)
  {
     aStatedoor1 = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aStatedoor1 != aLastStatedoor1){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aStatedoor1) { 
       counterdoor1 --;//++
       
     } else {
       counterdoor1 ++;//--
     }
   doorlength = counterdoor1*1.280;
   Serial.print("door length : ");
    Serial.println(doorlength);
    //Serial.clear();
   }
   aLastStatedoor1 = aStatedoor1;
  }
  while(digitalRead(heightpin)==HIGH)
  {
    aStatedoor2 = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aStatedoor2 != aLastStatedoor2){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aStatedoor2) { 
       counterdoor2 --;//++
       
     } else {
       counterdoor2 ++;//--
     }
   doorheight = counterdoor2*1.280;
   Serial.print("door hieght : ");
    Serial.println(doorheight);
    //Serial.clear();
   }
   aLastStatedoor2 = aStatedoor2;
  }
  String JSON_Data="{\"windowLSent\":";
           JSON_Data+=windowlength/100;
           JSON_Data+=",\"moistureSent\":";
           JSON_Data+=moisturecontent;
           JSON_Data+=",\"windowHSent\":";
           JSON_Data+=windowheight/100;
           JSON_Data+=",\"doorHSent\":";
           JSON_Data+=doorheight/100;
           JSON_Data+=",\"doorLSent\":";
           JSON_Data+=doorlength/100;
           JSON_Data+=",\"wallLSent\":";
           JSON_Data+=walllength/100;
           JSON_Data+=",\"wallHSent\":";
           JSON_Data+=wallheight/100;
           JSON_Data+=",\"area\":";
           area=abs((walllength*wallheight)/10000-(windowlength*windowheight)/10000-(doorlength*doorheight)/10000);
           area=area*10.7639;
           JSON_Data+=area;
           JSON_Data+=",\"paint\":";
           paint=area/100;
           JSON_Data+=paint;
           JSON_Data+=",\"primer\":";
           primer=paint;
           JSON_Data+=primer;
           JSON_Data+=",\"putty\":";
           putty=area/40;
           JSON_Data+=putty;
           JSON_Data+="}";
           websockets.broadcastTXT(JSON_Data);
  }
  else if(doc["wall"]==true)
  {
    Serial.println("Inside software wall switch");
    while(digitalRead(lengthpin)==HIGH)
  {
     aStatewall1 = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aStatewall1 != aLastStatewall1){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aStatewall1) { 
       counterwall1 --;//++
       
     } else {
       counterwall1 ++;//--
     }
   walllength = counterwall1*1.280;
   Serial.print("Wall length : ");
    Serial.println(walllength);
    //Serial.clear();
   }
   aLastStatewall1 = aStatewall1;
  }
  while(digitalRead(heightpin)==HIGH)
  {
    aStatewall2 = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aStatewall2 != aLastStatewall2){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aStatewall2) { 
       counterwall2 --;//++
       
     } else {
       counterwall2 ++;//--
     }
   wallheight = counterwall2*1.280;
   Serial.print("wall hieght : ");
    Serial.println(wallheight);
    //Serial.clear();
   }
   aLastStatewall2 = aStatewall2;
  }
  String JSON_Data="{\"windowLSent\":";
           JSON_Data+=windowlength/100;
           JSON_Data+=",\"moistureSent\":";
           JSON_Data+=moisturecontent;
           JSON_Data+=",\"windowHSent\":";
           JSON_Data+=windowheight/100;
           JSON_Data+=",\"doorHSent\":";
           JSON_Data+=doorheight/100;
           JSON_Data+=",\"doorLSent\":";
           JSON_Data+=doorlength/100;
           JSON_Data+=",\"wallLSent\":";
           JSON_Data+=walllength/100;
           JSON_Data+=",\"wallHSent\":";
           JSON_Data+=wallheight/100;
           JSON_Data+=",\"area\":";
            area=abs((walllength*wallheight)/10000-(windowlength*windowheight)/10000-(doorlength*doorheight)/10000);
           area=area*10.7639;
           JSON_Data+=area;
           JSON_Data+=",\"paint\":";
           paint=area/100;
           JSON_Data+=paint;
           JSON_Data+=",\"primer\":";
           primer=paint;
           JSON_Data+=primer;
           JSON_Data+=",\"putty\":";
           putty=area/40;
           JSON_Data+=putty;
           JSON_Data+="}";
           websockets.broadcastTXT(JSON_Data);
  }
  else if(doc["reset"]==true)
  {
    Serial.println("Inside software reset switch");
      counterwall1=0;
      counterwall2=0;
      counterwin1=0;
      counterwin2=0;
      counterdoor1=0;
      counterdoor2=0;
      windowlength=0;  
      windowheight=0;
     doorheight=0;
     doorlength=0;
     walllength=0;
     wallheight=0;
     String JSON_Data="{\"windowLSent\":";
           JSON_Data+=windowlength/100;
           JSON_Data+=",\"moistureSent\":";
           JSON_Data+=moisturecontent;
           JSON_Data+=",\"windowHSent\":";
           JSON_Data+=windowheight/100;
           JSON_Data+=",\"doorHSent\":";
           JSON_Data+=doorheight/100;
           JSON_Data+=",\"doorLSent\":";
           JSON_Data+=doorlength/100;
           JSON_Data+=",\"wallLSent\":";
           JSON_Data+=walllength/100;
           JSON_Data+=",\"wallHSent\":";
           JSON_Data+=wallheight/100;
           JSON_Data+=",\"area\":";
            area=abs((walllength*wallheight)/10000-(windowlength*windowheight)/10000-(doorlength*doorheight)/10000);
           area=area*10.7639;
           JSON_Data+=area;
           JSON_Data+=",\"paint\":";
           paint=area/100;
           JSON_Data+=paint;
           JSON_Data+=",\"primer\":";
           primer=paint;
           JSON_Data+=primer;
           JSON_Data+=",\"putty\":";
           putty=area/40;
           JSON_Data+=putty;
           JSON_Data+="}";
           websockets.broadcastTXT(JSON_Data);
  }
  }
}

void setup(void)
{
  Serial.begin(9600);
  WiFi.softAP("ESP","321");
   pinMode(lengthpin,INPUT);
    pinMode(heightpin,INPUT);
//    pinMode(sensorswitch,INPUT);
    //pinMode(sensor,INPUT);
Serial.println(WiFi.softAPIP());

if(MDNS.begin("ESP")){
  Serial.print("MDNS Responder started");}
  server.on("/",[](AsyncWebServerRequest * request)
  {
    request->send_P(200,"text/html",webpage);
  });
  server.onNotFound(notfound);
  server.begin();
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  
}
void loop(void)
{
  websockets.loop();
}

 
