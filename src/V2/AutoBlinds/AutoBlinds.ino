#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Servo.h>
Servo myservo;

int powerControl = 14;

#include "Wire.h"

const int MPU_ADDR = 0x68;

int16_t accelerometer_x, accelerometer_y, accelerometer_z;
int16_t gyro_x, gyro_y, gyro_z;
int16_t temperature;

char tmp_str[7];

char* convert_int16_to_str(int16_t i) {
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}


const char* ssid = "";
const char* password =  "";
const char* mqttServer = "";
const int mqttPort = 1883;
const char* mqttUser = "user1";
const char* mqttPassword = "";

long lastMsg = 0;
char msg[50];
int value = 0;
String actie = "stop";

boolean isgoed = false;


int mx , my, hx, hy, lx, ly ;

WiFiClient espClient;
PubSubClient client(espClient);

void c() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("Ruben1", mqttUser, mqttPassword )) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }

  }
  client.publish("/esp", "Hello from ESP8266");
  client.subscribe("/esp");
}

void setup() {
  Serial.begin(115200);
  myservo.attach(2);  // D4
  c();
  myservo.write(90);              // tell servo to go to position in variable 'pos'
  delay(15);                       // waits 15ms for the servo to reach the position
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  pinMode(powerControl, OUTPUT);

  digitalWrite(powerControl, HIGH);


}

void callback(char* topic, byte* payload, unsigned int length) {
  char x;
  String bericht = "";
  for (int i = 0; i < length; i++) {
    x = (char)payload[i];
    bericht = String(bericht + x);
  }

  String waarvoor = bericht.substring(0, 2);
  String waarde = bericht.substring(2);
  //Serial.print(waarvoor);
  //Serial.print(waarde);

  int waardeint = waarde.toInt();
  if (waarvoor.equals("of")) {
    Serial.println("OFF");
    digitalWrite(powerControl, LOW);
  } else if (waarvoor.equals("on")) {
    Serial.println("ON");
    digitalWrite(powerControl, HIGH);
  } else if (waarvoor.equals("oh")) {
    actie = "openH";
    isgoed = false;
  } else if (waarvoor.equals("ol")) {
    actie = "openL";
    isgoed = false;
  } else if (waarvoor.equals("mm")) {
    actie = "midden";
    isgoed = false;
  } else if (waarvoor.equals("ss")) {
    actie = "s";
  }  else if (waarvoor.equals("ll")) {
    actie = "l";
  } else if (waarvoor.equals("rr")) {
    actie = "r";
  } else if (waarvoor.equals("mx")) {
    if ( waardeint < 0 || waardeint > 0 ) {
      mx = waardeint;
    }
  } else if (waarvoor.equals("my")) {
    if ( waardeint < 0 || waardeint > 0 ) {
      my = waardeint;
    }
  } else if (waarvoor.equals("hx")) {
    if ( waardeint < 0 || waardeint > 0 ) {
      hx = waardeint;
    }
  } else if (waarvoor.equals("hy")) {
    if ( waardeint < 0 || waardeint > 0 ) {
      hy = waardeint;
    }
  } else if (waarvoor.equals("lx")) {
    if ( waardeint < 0 || waardeint > 0 ) {
      lx = waardeint;
    }
  } else if (waarvoor.equals("ly")) {
    if ( waardeint < 0 || waardeint > 0 ) {
      ly = waardeint;
    }
  }

}


void s() {
  myservo.write(90);              // tell servo to go to position in variable 'pos'
  delay(15);                       // waits 15ms for the servo to reach the position
}

void r() {
  myservo.write(0);              // tell servo to go to position in variable 'pos'
  delay(15);                       // waits 15ms for the servo to reach the position
}

void l() {
  myservo.write(180);              // tell servo to go to position in variable 'pos'
  delay(15);                       // waits 15ms for the servo to reach the position
}



void loop() {

  client.loop();

  if (!client.connected()) {
    c();
  }

  long now = millis();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2, true); // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read() << 8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  /*
    accelerometer_z = Wire.read() << 8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    temperature = Wire.read() << 8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
    gyro_x = Wire.read() << 8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
    gyro_y = Wire.read() << 8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
    gyro_z = Wire.read() << 8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  */
  //int mx , my, hx, hy, lx, ly ;

  float f;
  float ff;

  if (actie == "s") {
    s();
  } else if (actie == "l") {
    l();
  } else if (actie == "r") {
    r();
  } else if (isgoed) {

    //String t = "klaar" ;
    //client.publish("/esp/f", (char*) t.c_str());

  } else if (actie == "openH" && !isgoed) {

    if (hx <= accelerometer_x) {
      r();
    } else if (hx >= accelerometer_x ) {
      l();
    }

    f = (float)(abs(hx) - abs(accelerometer_x));

    // String fs = String(f);
    // client.publish("/esp/f", (char*) fs.c_str());

    ff = f / hx;

    //String ffs = String(ff);
    //client.publish("/esp/ff", (char*) ffs.c_str());

    if (ff >= -0.05 && ff <= 0.05) {
      isgoed = true;
      s();
    }
  } else if (actie == "openL" && !isgoed) {
    if (lx <= accelerometer_x) {
      r();
    } else if (lx >= accelerometer_x ) {
      l();
    }

    f = (float)(abs(lx) - abs(accelerometer_x));

    //String fs = String(f);
    //client.publish("/esp/f", (char*) fs.c_str());

    ff = f / lx;

    // String ffs = String(ff);
    //client.publish("/esp/ff", (char*) ffs.c_str());

    if (ff >= -0.05 && ff <= 0.05) {
      isgoed = true;
      s();
    }
  } else if (actie == "midden" && !isgoed) {
    if (mx <= accelerometer_x) {
      r();
    } else if (mx >= accelerometer_x ) {
      l();
    }

    if ( mx > 0) {

      if (accelerometer_x > 0) {
        f = (float)(mx - accelerometer_x);
      } else {
        f = (float)(mx + accelerometer_x);
      }
      ff = f / mx;
      //String fs = String(f);
      // client.publish("/esp/f", (char*) fs.c_str());
      //String ffs = String(ff);
      //client.publish("/esp/ff", (char*) ffs.c_str());



    } else {
      if (accelerometer_x > 0) {
        f = (float)(mx + accelerometer_x  );
      } else {
        f = (float)(mx - accelerometer_x  );
      }
      ff = f / mx;
      //String fs = String(f);
      // client.publish("/esp/f", (char*) fs.c_str());
      //String ffs = String(ff);
      //client.publish("/esp/ff", (char*) ffs.c_str());
    }


    if (mx > 0) {
      if (accelerometer_x>0) {
        if (ff >= -0.05 && ff <= 0.05) {
          isgoed = true;
          s();
        }
      }
    } else {
      if (accelerometer_x<0) {
        if (ff >= -0.05 && ff <= 0.05) {
          isgoed = true;
          s();
        }
      }
    }



  }


  if (now - lastMsg > 2000) {

    //
    // print out data
    /*
      Serial.print("aX = "); Serial.print(convert_int16_to_str(accelerometer_x));
      Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accelerometer_y));
      Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(accelerometer_z));
      // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
      Serial.print(" | tmp = "); Serial.print(temperature / 340.00 + 36.53);
      Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gyro_x));
      Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gyro_y));
      Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gyro_z));
      Serial.println();
    */

    lastMsg = now;

    String x =
      /* + "| ax =| " + String(convert_int16_to_str(*/String(accelerometer_y)/*))/* + " "
              "| ay = | " + String(convert_int16_to_str(accelerometer_x)) + " "
              + " | az =| " + String(convert_int16_to_str(accelerometer_z))
              + " | gx =| " + String(convert_int16_to_str( gyro_x ))
              + " | gy =| " + String(convert_int16_to_str( gyro_y ))
              + " | gz =| " + String(convert_int16_to_str( gyro_z ))
      */
      ;


    String y =
      /* "| ay = | "*+ String(convert_int16_to_str(*/String(accelerometer_x)/*)) /*+ " "
        + "| ax =| " + String(convert_int16_to_str(accelerometer_y)) + " "
         + " | az =| " + String(convert_int16_to_str(accelerometer_z))
         + " | gx =| " + String(convert_int16_to_str( gyro_x ))
         + " | gy =| " + String(convert_int16_to_str( gyro_y ))
         + " | gz =| " + String(convert_int16_to_str( gyro_z ))
      */
      ;


    Serial.print("Publish message: ");
    Serial.println(msg);

    client.publish("/esp/x", (char*) x.c_str());
    client.publish("/esp/y", (char*) y.c_str());




  }

}
