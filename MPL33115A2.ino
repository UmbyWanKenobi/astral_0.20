

String ReadMPL(byte flag) {
  String value;

  switch (flag) {
    case 0:
      value = "Pressione: ";
      value += MPL.readPressure() / 100;

      value += " ettopascal";
      return (value);
      break;
    case 1:
      value = "Temperatura: ";
      value += MPL.readTemp();
      value += " Celsius";
      return (value);

      break;
    case 2:
      value = "Quota: ";
      value += MPL.readAltitude() / 131.23;
      value += " m s.l.m.";
      return (value);
      break;

  }

}



