#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

// --- CONFIGURACIÓN DE RED ---
const char* WIFI_SSID = "sebas";
const char* WIFI_PASSWORD = "apocositilin";

// --- CONFIGURACIÓN DE AWS IOT CORE ---
const char* AWS_IOT_ENDPOINT = "a3lavp493f93pa-ats.iot.us-east-2.amazonaws.com";
const char* CLIENT_ID = "Prueba2"; 
const char* AWS_PUB_TOPIC = "esp32/telemetria";

// --- CERTIFICADOS Y CLAVES ---

// Amazon Root CA 1
const char* AWS_CERT_CA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Certificado del dispositivo (xxx-certificate.pem.crt)
const char* AWS_CERT_CRT = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUT5ZXA5cnqhOs0KTDdT9c2UmKSJswDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MTExNzAwMzEy
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANnIhUTLnTZk1bj1Kdlo
jROsvF1e/lbXoNNx5l4pFFQaSTJ+4MTHTdyztit0mpLuSTg56BJ6MiTw7Qwl7WU6
Czys+s1ct6iqy9iju2RjsBpc1rNCu0acMiMznzDawWIMMSkr0f65Z8OmGn7sBBfW
H3orhTdEYjlOpT7VVpMPyo6okdIkTlya8GKrcUcSzYT6XDqz2QsXIFFpewYeEsgl
iMqFCAxIEmQrcXHlzIl3yP2x6VurCEsBzrd2HVv1N7hGaJD6klt/85++PB9wC3tl
XrbbGoZfMz7GI8J7WLsIQi8rBx1ExYlx+NKY8cwv7vio5skkYLPY5E8XaiAt9zp3
7JMCAwEAAaNgMF4wHwYDVR0jBBgwFoAU0MQ4T9YLujDiGEvfotiLHk3smYgwHQYD
VR0OBBYEFDvsgvbZl1JaH/99Mwv4Oc/N7eyBMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQARFFLCbiFspPohxBqZemotmJJQ
Z2UuTlMw/CMuryF4bJBWWKkl5hz1o7gX0sVlQniXHfD5ksyUASQXjRhiMAivc/CH
NSMtWhY3NhOpgSaQ0tKz00IPgltGkv2azjHXR6EKXMbFy/gJlT+n90XJu9qhdOCJ
YkCMZIXaMAwrtkQij13iBWTiLRUAM7l5g7099f1hoebcj0RqVOv4B5QnXkTR6OKV
QpqX3FDrfsC0GRNDBBjMtSPjApRGWe7Nvcjl/PlMJH3c9gpv746lr1VUe8zt/awl
9CsdrU+c+Zqc98Tuso4iOne27mQYTSwsU1i3ybQy3ObfVuIfnnC4AibHuIQJ
-----END CERTIFICATE-----
)EOF";

// Clave privada (xxx-private.pem.key)
const char* AWS_CERT_PRIVATE = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA2ciFRMudNmTVuPUp2WiNE6y8XV7+Vteg03HmXikUVBpJMn7g
xMdN3LO2K3Saku5JODnoEnoyJPDtDCXtZToLPKz6zVy3qKrL2KO7ZGOwGlzWs0K7
RpwyIzOfMNrBYgwxKSvR/rlnw6YafuwEF9YfeiuFN0RiOU6lPtVWkw/KjqiR0iRO
XJrwYqtxRxLNhPpcOrPZCxcgUWl7Bh4SyCWIyoUIDEgSZCtxceXMiXfI/bHpW6sI
SwHOt3YdW/U3uEZokPqSW3/zn748H3ALe2Vettsahl8zPsYjwntYuwhCLysHHUTF
iXH40pjxzC/u+KjmySRgs9jkTxdqIC33OnfskwIDAQABAoIBAQDPMHrBOWkmz4WY
wfz/bf0RY7uMpQwZeGjz4J+Go1RIsv1CQfz6mJYVyw8bhoJsGLCmAkksEv7TsDYE
4axZKoB4SCUO7Q95MoxeXV6xIjnuyrYjsu/SPFVisgDYDLX/3pOClMLisGbsdoEs
kMNQHhCyPOcu7ir4VD7XaUVFAbf2QN27GNamF4/w5qn+LCwyPavOWprRxitQdOIS
+g+ILAC+OVRnGq+WQjUULfag4wn8f25TwUcarRdrYLsNBZsvcpAK89J2u8v1j0U0
3fllR+CnSzT9aL5zc/AoFSyxDL+f+Nj/5itNGh+H4IX1daORxfdvknOVHg0PWbct
QyZ154XJAoGBAOzYJAVxJjNdg5o4yPbM1Oo/MpmuH2nSlkK1LVsH0N9neHJVHcbu
HoOpFcpJUucEZ5dnta8AMM3gZYCB8OhnAWfX0emkm78H9Eg5nKRiE9n6s4IOsAAb
XbnZOUgFg4Z7RNxJZOkjl8alBoCFaqyA6lsqY/6/dh8fDGMYY13MCrvNAoGBAOtl
uMCnG6XhogEBtQwCI7yrRPfBHXudK86gUtfTZofuUliPZRjPjE88odSOb+cJzXfR
JmgrDmtTQH6CiI1tb+uEqC/sl3ieS7rAQYGJLt991NwxL9OUlw3xTNo6B/jZZ5xT
Ez3YvhBSEs1r7Czo9GO6mSqWi+ktsxOVEyPHtKnfAoGACjZ/xyRL62ca56mPMOVA
QSGcgvf5Lsz/4byCSiyaGygRD40QDaXy9uZxpBlpU8Di3qqAMR2m/RmyBsqTotHe
Q6YQzSxl8uTa6cpiMMxx4rJmzzLm7/o+Hmd5ituCAmiKXZ1klpEMWGqBT0VxhiLA
+FFponCLHWhvVk6U6lSEGfkCgYBDpCdCtMItGHkE8QilOrgqLszSpJtkOfWsmkUz
33tG9CSVbh8O0hTvb+zQt11QdnCkTWVL6ZNrI7EUqU2z1g6jDquFmeassk/wie5I
2rKtZesUms2zbq85Qm1jB+niiBZNxVFyJCX5apwcizaCcS5IugXpRvlNN+SNa/jj
/y7a4QKBgQC8HEzSYT0KXqyUhwMRYhM+ipMizby3Pv/V8o65D2z1vczzVs1PegLQ
4E2i/+kY2hFacO1WWHbGXhPg1UM/+NxIiZ0A0UBMK9UrZ0btJTfoN+clwvIc18Cw
KVgrz+1Vg8yLFHxV7R1/zqU/fjn47FFzZbsVTZG0tlj7oiYgewx5oA==
-----END RSA PRIVATE KEY-----
)EOF";

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void syncTime() {
  // 1. Usar el servidor NTP de Google junto con pool.ntp.org (responde más rápido)
  configTime(-6 * 3600, 0, "time.google.com", "pool.ntp.org"); 
  
  Serial.print("Sincronizando hora NTP");
  
  time_t now = time(nullptr);
  int intentos = 0;
  
  // 2. Esperar máximo 10 segundos para no bloquear el programa indefinidamente
  while (now < 8 * 3600 * 2 && intentos < 20) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    intentos++;
  }

  if (intentos >= 20) {
    Serial.println("\n[ADVERTENCIA] Tiempo de espera NTP agotado. Reintentando...");
  } else {
    Serial.println("\n¡Hora sincronizada correctamente!");
  }
}

void connectAWS() {
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.setBufferSize(512); // Extiende el búfer para handshake mTLS
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  Serial.print("Conectando a AWS IoT Core");
  while (!client.connected()) {
    if (client.connect(CLIENT_ID)) {
      Serial.println("\n¡Conexión segura mTLS establecida exitosamente con AWS!");
    } else {
      Serial.print(".");
      Serial.print(" Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado.");

  // Sincronizar reloj antes de iniciar mTLS
  syncTime();

  // Iniciar conexión con AWS
  connectAWS();
}

void loop() {
  if (!client.connected()) {
    connectAWS();
  }
  client.loop();

  // Enviar telemetría cada 10 segundos
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 10000) {
    lastMsg = millis();
    
    String payload = "{\"status\":\"ok\", \"conexion\":\"cifrada mTLS\"}";
    
    if (client.publish(AWS_PUB_TOPIC, payload.c_str())) {
      Serial.println("Mensaje enviado a AWS: " + payload);
    } else {
      Serial.println("Error al enviar mensaje.");
    }
  }
}