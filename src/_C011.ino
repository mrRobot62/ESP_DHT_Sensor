//#######################################################################################################
//########################### Controller Plugin 011: OpenHAB MQTT
//#######################################
//#######################################################################################################

#define CPLUGIN_011
#define CPLUGIN_ID_011 11
#define CPLUGIN_NAME_011 "THPLightSensor MQTT"
#define INFO_LINES 3

unsigned long last_millis = 0;

boolean CPlugin_011(byte function, struct EventStruct *event, String &string) {
  boolean success = false;

  switch (function) {
  case CPLUGIN_PROTOCOL_ADD: {
    Protocol[++protocolCount].Number = CPLUGIN_ID_011;
    Protocol[protocolCount].usesMQTT = true;
    Protocol[protocolCount].usesTemplate = true;
    Protocol[protocolCount].usesAccount = false;
    Protocol[protocolCount].usesPassword = false;
    Protocol[protocolCount].defaultPort = 1883;
    break;
  }

  case CPLUGIN_GET_DEVICENAME: {
    string = F(CPLUGIN_NAME_011);
    break;
  }

  case CPLUGIN_PROTOCOL_TEMPLATE: {
    strcpy_P(Settings.MQTTsubscribe, PSTR("/%sysname%/#"));
    strcpy_P(Settings.MQTTpublish, PSTR("/%sysname%/%tskname%/%valname%"));
    break;
  }

  case CPLUGIN_PROTOCOL_RECV: {
    // Split topic into array
    String tmpTopic = event->String1.substring(1);
    String topicSplit[10];
    int SlashIndex = tmpTopic.indexOf('/');
    byte count = 0;
    while (SlashIndex > 0 && count < 10 - 1) {
      topicSplit[count] = tmpTopic.substring(0, SlashIndex);
      tmpTopic = tmpTopic.substring(SlashIndex + 1);
      SlashIndex = tmpTopic.indexOf('/');
      count++;
    }
    topicSplit[count] = tmpTopic;

    String cmd = "";
    struct EventStruct TempEvent;

    if (topicSplit[count] == "cmd") {
      cmd = event->String2;
      parseCommandString(&TempEvent, cmd);
      TempEvent.Source = VALUE_SOURCE_MQTT;
    } else {
      cmd = topicSplit[count - 1];
      TempEvent.Par1 = topicSplit[count].toInt();
      TempEvent.Par2 = event->String2.toFloat();
      TempEvent.Par3 = 0;
    }
    // in case of event, store to buffer and return...
    String command = parseString(cmd, 1);
    if (command == F("event"))
      eventBuffer = cmd.substring(6);
    else
      PluginCall(PLUGIN_WRITE, &TempEvent, cmd);
    break;
  }

  case CPLUGIN_PROTOCOL_SEND: {
    statusLED(true);

    if (ExtraTaskSettings.TaskDeviceValueNames[0][0] == 0)
      PluginCall(PLUGIN_GET_DEVICEVALUENAMES, event, dummyString);

    String pubname = Settings.MQTTpublish;
    pubname.replace("%sysname%", Settings.Name);
    pubname.replace("%tskname%", ExtraTaskSettings.TaskDeviceName);
    pubname.replace("%id%", String(event->idx));

    String value = "";
    byte DeviceIndex =
        getDeviceIndex(Settings.TaskDeviceNumber[event->TaskIndex]);
    byte valueCount = getValueCountFromSensorType(event->sensorType);
    /*
    for (byte x = 0; x < valueCount; x++) {
      String tmppubname = pubname;
      tmppubname.replace("%valname%",
                         ExtraTaskSettings.TaskDeviceValueNames[x]);
      if (event->sensorType == SENSOR_TYPE_LONG) {
        value = (unsigned long)UserVar[event->BaseVarIndex] +
                ((unsigned long)UserVar[event->BaseVarIndex + 1] << 16);
      } else {
        value = toString(UserVar[event->BaseVarIndex + x],
                         ExtraTaskSettings.TaskDeviceValueDecimals[x]);
      }
      MQTTclient.publish(tmppubname.c_str(), value.c_str(),
                         Settings.MQTTRetainFlag);
    }
    */
    char infoText[INFO_LINES][40];
    LoadCustomTaskSettings(event->TaskIndex, (byte *)&infoText,
                           sizeof(infoText));

    String tmppubname = pubname;
    tmppubname.replace("%valname%", ExtraTaskSettings.TaskDeviceValueNames[0]);
    value = toString(UserVar[event->BaseVarIndex + 0],
                     ExtraTaskSettings.TaskDeviceValueDecimals[0]);
    value += " ";
    value += ExtraTaskSettings.TaskDeviceValueNames[1];
    value += ": ";
    value += toString(UserVar[event->BaseVarIndex + 1],
                      ExtraTaskSettings.TaskDeviceValueDecimals[0]);
    value += " ";
    value += ExtraTaskSettings.TaskDeviceValueNames[2];
    value += ": ";
    value += toString(UserVar[event->BaseVarIndex + 2],
                      ExtraTaskSettings.TaskDeviceValueDecimals[0]);
    value += " InfoText:'";
    for (byte x = 0; x < INFO_LINES; x++) {
      String tmpString = infoText[x];
      if (tmpString.length()) {
        value += tmpString;
        value += "/";
      }
    }
    value += "'";
    MQTTclient.publish(tmppubname.c_str(), value.c_str(),
                       Settings.MQTTRetainFlag);
    /*
    char buf[80];
    sprintf("Broker message TO: %s VALUE: %s", tmppubname.c_str(),
            value.c_str());
    */
    addLog(LOG_LEVEL_INFO, "Broker message sent");
    break;
  }
    return success;
  }
}
