# Práctica: Conexión Segura mTLS de ESP32 a AWS IoT Core

Práctica de la asignatura **Internet de las Cosas** (ITIC).

En esta práctica se implementa una **conexión cifrada de extremo a extremo** entre una placa ESP32 y la plataforma en la nube **AWS IoT Core**. Mediante el uso de autenticación mutua (mTLS) y el protocolo MQTT sobre el puerto 8883, el microcontrolador valida la identidad del servidor en la nube y presenta sus propias credenciales criptográficas, garantizando una transmisión de datos segura y protegida contra intercepciones.

## Estructura del Repositorio

| Archivo | Descripción |
| --- | --- |
| `ESP32_AWS_mTLS.ino` | Sketch de Arduino: manejo de Wi-Fi, sincronización NTP, cliente seguro y publicación MQTT. |
| `documentacion.pdf` | Reporte técnico completo con marco teórico, diagramas y evidencias de laboratorio. |

## Materiales Requeridos

- Placa de desarrollo ESP32 (Dev Module).
- Cable USB de datos.
- Arduino IDE configurado con las librerías:
  - **WiFi** y **WiFiClientSecure** (Nativas del core de ESP32)
  - **PubSubClient** (de Nick O'Leary)
- Cuenta activa en **AWS IoT Core** con un Objeto (*Thing*) registrado y credenciales X.509 descargadas.

## Infraestructura en la Nube (AWS IoT Core)

Para que el dispositivo pueda autenticarse, la infraestructura requiere los siguientes elementos previos:
1. **Política de IoT:** Permisos JSON adjuntos al certificado para permitir acciones de conexión, publicación y suscripción.
2. **Certificado del Dispositivo:** Credencial de identidad única emitida para la ESP32.
3. **Clave Privada:** Llave criptográfica secreta asociada al certificado.
4. **Certificado Raíz (Amazon Root CA 1):** Permite a la ESP32 verificar la autenticidad del servidor de AWS.

## Topología de Comunicación

```text
  ESP32 (Node)
      |
      |--- 1. Conexión Wi-Fi & Sincronización NTP (Hora exacta)
      |--- 2. Negociación mTLS (Certificados + Clave Privada)
      v
  AWS IoT Core (Puerto 8883 - MQTT / TLS)
      |
      +---> MQTT Test Client (Consola de AWS para visualización en tiempo real)