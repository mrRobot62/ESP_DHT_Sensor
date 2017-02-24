//#######################################################################################################
//######################## Plugin 196: Temperature and Humidity sensor DHT 11
//(Adafruit DHT Unified) #####################
//#######################################################################################################

#define PLUGIN_196
#define PLUGIN_ID_196 196
#define PLUGIN_NAME_196 "Temperature & Humidity - DHT_Unified (WeMos)"
#define PLUGIN_VALUENAME1_196 "Temperature"
#define PLUGIN_VALUENAME2_196 "Humidity"

#define DUMP_TIME (1000 * 60)

boolean Plugin_196_init = false;

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// DHT dht(DHT_PIN,DHT_TYPE);
DHT_Unified *dht;
sensor_t sensor;
long sensor_delay_ms;
// unsigned long last_millis;

boolean Plugin_196_DHT_init(struct EventStruct *event) {
  String log = "Plugin_196_DHT_init: ";
  uint8_t pin = Settings.TaskDevicePin1[event->TaskIndex];
  uint8_t dev = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
  pin = 2;
  // dev = 2;
  switch (dev) {
  case 2:
    log = "DHT21 on pin ";
    log += String(pin);
    dht = new DHT_Unified(DHT21, pin);
    break;
  case 3:
    log = "init DHT22 on pin ";
    log += String(pin);
    dht = new DHT_Unified(DHT22, pin);
    break;
  case 1:
  default:
    log = "init default DHT11(blue) on pin ";
    log += String(pin);
    dht = new DHT_Unified(DHT11, pin);
    break;
  }
  log += " - PIN(";
  log += String(Settings.TaskDevicePin1[event->TaskIndex]) + ")";
  log += " - DEV(";
  log += String(Settings.TaskDevicePluginConfig[event->TaskIndex][0]) + ")";

  addLog(LOG_LEVEL_INFO, log);
  //
  if (dht != NULL) {
    dht->begin();
    dht->temperature().getSensor(&sensor);
    Dump_DHT_Sensor(event);
  } else {
    addLog(LOG_LEVEL_ERROR, "no DHT sensor object found");
  }
}

void Dump_DHT_Sensor(struct EventStruct *event) {
  String log = "DHT_Unified:";
  sensor_t sensor;
  if (dht != NULL) {
    dht->temperature().getSensor(&sensor);
    log += "\n----------------------------------";
    log += "\nTemperature";
    log += "\nName:       " + String(sensor.name);
    log += "\nDriver Ver. " + String(sensor.version);
    log += "\nUnique ID:  " + String(sensor.sensor_id);
    log += "\nMax Value:  " + String(sensor.max_value) + " *C";
    log += "\nMin Value:  " + String(sensor.min_value) + " *C";
    log += "\nResolution: " + String(sensor.resolution) + " *C";
    log += "\n----------------------------------";
    dht->humidity().getSensor(&sensor);
    log += "\n----------------------------------";
    log += "\nHumidity";
    log += "\nName:       " + String(sensor.name);
    log += "\nDriver Ver. " + String(sensor.version);
    log += "\nUnique ID:  " + String(sensor.sensor_id);
    log += "\nMax Value:  " + String(sensor.max_value) + " *C";
    log += "\nMin Value:  " + String(sensor.min_value) + " *C";
    log += "\nResolution: " + String(sensor.resolution) + " *C";
    log += "\n----------------------------------";
    addLog(LOG_LEVEL_INFO, log);
  } else {
    addLog(LOG_LEVEL_ERROR, "can't DUMP sensor - no object found");
  }
  sensor_delay_ms = sensor.min_delay / 1000;
}

boolean Plugin_196(byte function, struct EventStruct *event, String &string) {
  boolean success = false;

  switch (function) {
  case PLUGIN_DEVICE_ADD: {
    Device[++deviceCount].Number = PLUGIN_ID_196;
    Device[deviceCount].Type = DEVICE_TYPE_DUAL;
    Device[deviceCount].VType = SENSOR_TYPE_TEMP_HUM;
    Device[deviceCount].Ports = 0;
    Device[deviceCount].PullUpOption = false;
    Device[deviceCount].InverseLogicOption = false;
    Device[deviceCount].FormulaOption = true;
    Device[deviceCount].ValueCount = 2;
    Device[deviceCount].SendDataOption = true;
    Device[deviceCount].TimerOption = true;
    Device[deviceCount].GlobalSyncOption = true;
    addLog(LOG_LEVEL_INFO, "PLUGIN_DEVICE_ADD");
    break;
  }

  case PLUGIN_GET_DEVICENAME: {
    string = F(PLUGIN_NAME_196);
    break;
  }

  case PLUGIN_GET_DEVICEVALUENAMES: {
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0],
             PSTR(PLUGIN_VALUENAME1_196));
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1],
             PSTR(PLUGIN_VALUENAME2_196));
    break;
  }

  case PLUGIN_INIT: {
    String log = "";
    if (dht != NULL) {
      dht->begin();
      log = "initialize P196_DHT_WEMOS plugin... ";
      log += "open DHT device";
      setPinState(PLUGIN_ID_196, Settings.TaskDevicePin1[event->TaskIndex],
                  PIN_MODE_INPUT, 0);
    } else {
    }
    addLog(LOG_LEVEL_INFO, log);
    success = true;
    break;
  }
  case PLUGIN_WEBFORM_LOAD: {
    byte choice = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
    String options[3];
    options[0] = F("DHT11");
    options[1] = F("DHT21 (AM2301)");
    options[2] = F("DHT22 (AM2302)");
    int optionValues[3];
    optionValues[0] = 1;
    optionValues[1] = 2;
    optionValues[2] = 3;
    string += F("<TR><TD>DHT Type:<TD><select name='plugin_196_type'>");
    for (byte x = 0; x < 3; x++) {
      string += F("<option value='");
      string += optionValues[x];
      string += "'";
      if (choice == optionValues[x])
        string += F(" selected");
      string += ">";
      string += options[x];
      string += F("</option>");
    }
    string += F("</select>");
    addLog(LOG_LEVEL_INFO, "PLUGIN_WEBFORM_LOAD");
    Plugin_196_DHT_init(event);
    success = true;
    break;
  }

  case PLUGIN_WEBFORM_SAVE: {
    String log = "";
    uint8_t pin;
    String plug1 = WebServer.arg("plugin_196_type");
    Settings.TaskDevicePluginConfig[event->TaskIndex][0] = plug1.toInt();
    addLog(LOG_LEVEL_INFO, "PLUGIN_WEBFORM_SAVE");
    Settings.TaskDevicePin1[event->TaskIndex] = Plugin_196_DHT_init(event);
    success = true;
    break;
  }

  case PLUGIN_ONCE_A_SECOND: {
    if ((millis() - last_millis) >= DUMP_TIME) {
      addLog(LOG_LEVEL_INFO, "Dump-Sensor every " + String(DUMP_TIME) + "ms");
      Dump_DHT_Sensor(event);
      last_millis = millis();
    }
    success = true;
    break;
  }

  case PLUGIN_READ: {
    String log = "PLUGIN_READ";
    float dht_dat[3];
    sensors_event_t sensor_event;

    // --------- Humidity -----------------------
    dht->humidity().getEvent(&sensor_event);
    if (isnan(sensor_event.relative_humidity)) {
      log = "Error reading DHT sensor.humidity";
      success = false;
    } else {
      dht_dat[0] = sensor_event.relative_humidity;
      log += "\nHumidity:     " + String(dht_dat[0]) + " %";
    }
    //
    delay(sensor_delay_ms);
    //---------- Tempearture ----------------------
    dht->temperature().getEvent(&sensor_event);
    if (isnan(sensor_event.temperature)) {
      log = "Error reading DHT sensor.temperature";
      success = false;
    } else {
      dht_dat[1] = sensor_event.temperature;
      log += "\nTemperature:   " + String(dht_dat[1]) + " *C";
    }
    addLog(LOG_LEVEL_INFO, log);

    UserVar[event->BaseVarIndex] = dht_dat[0];
    UserVar[event->BaseVarIndex + 1] = dht_dat[1];
    UserVar[event->BaseVarIndex + 2] = -1;

    success = true;

    if (!success) {
      addLog(LOG_LEVEL_INFO, log);
      UserVar[event->BaseVarIndex] = NAN;
      UserVar[event->BaseVarIndex + 1] = NAN;
    }
    break;
  }
  }
  return success;
}
