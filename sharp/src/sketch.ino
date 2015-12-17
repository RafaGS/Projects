///
/// Utilización del sensor de distancia Sharp 2Y0A21
/// by RafaG
///
/// http://minibots.wordpress.com/2013/11/30/utilizacion-del-sensor-de-distancia-sharp-2y0a21/
///

// Pines de lectura
int ir_sensor0 = A0;
int ir_sensor1 = A1;

void setup() 
{
    // inicia comunicaciones serie a 9600 bps
    Serial.begin(9600);
}

void loop() 
{
    int lectura, cm;

    lectura = analogRead(ir_sensor0); // lectura del sensor 0
    cm = pow(3027.4 / lectura, 1.2134); // conversión a centímetros
    Serial.print("Sensor 0: ");
    Serial.print(cm); // lectura del sensor 0
    delay(500); // tiempo de espera

    lectura = analogRead(ir_sensor1); // lectura del sensor 1
    cm = pow(3027.4 / lectura, 1.2134); // conversión a centímetros
    Serial.print("Sensor 1: ");
    Serial.println(cm); // lectura del sensor 1
    delay(1000); // tiempo de espera para realizar otro par de lecturas
}
