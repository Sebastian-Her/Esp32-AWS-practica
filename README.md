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

## Configuración de la Infraestructura en AWS IoT Core

Para que la ESP32 pueda autenticarse y conectarse de forma segura, debes configurar los recursos en la nube siguiendo esta ruta exacta:

1. **Acceder a IoT Core:** Ingresa a la consola de AWS y busca el servicio **AWS IoT Core**.
2. **Navegar a Objetos (Things):** En el panel lateral izquierdo, despliega la sección **Manage** (Administrar) y haz clic en la subsección **All devices** (Todos los dispositivos). Selecciona **Things** (Objetos).
3. **Crear el Dispositivo:** Haz clic en el botón **Create things** (Crear objetos) a la derecha.
4. **Seleccionar Creación Individual:** Elige la opción **Create single thing** (Crear un único objeto) y presiona **Next** (Siguiente).
5. **Nombrar el Objeto:** Asignale un nombre a tu dispositivo (por ejemplo, `FuelGuard_Node_01`), el cual debe coincidir exactamente con el `CLIENT_ID` definido en tu código de la ESP32.
6. **Acceder a IoT Core:** Ingresa a la consola de AWS y busca el servicio **AWS IoT Core**.
7. **Navegar a Objetos (Things):** En el panel lateral izquierdo, despliega la sección **Manage** y haz clic en **All devices**. Selecciona **Things** y haz clic en el botón **Create things** a la derecha.
8. **Configurar el Objeto:**
   * Selecciona **Create single thing** y haz clic en **Next**.
   * Escribe el nombre de tu dispositivo en **Thing name** (Nota: **el nombre no debe tener espacios**, por ejemplo: `FuelGuard_Node_01`).
   * Desplázate hacia abajo hasta la sección de **Device shadow** y elige la opción **No shadow**.
   * Haz clic en **Next**.
9. **Configurar el Certificado:** En la sección **Configure device certificate**, selecciona la opción **Auto-generate a new certificate (recommended)** y haz clic en **Next**.
10. **Crear la Política de Seguridad:**
   * En la sección **Attach policies to certificate**, haz clic en el botón para **crear una nueva política** (*Create policy*).
   * Asignale un nombre a tu política (por ejemplo, `Politica_ESP32`).
   * Baja hasta el apartado **Policy document**, cambia la opción a formato **JSON** y pega el siguiente documento de permisos:
     ```json
     {
       "Version": "2012-10-17",
       "Statement": [
         {
           "Effect": "Allow",
           "Action": "iot:*",
           "Resource": "*"
         }
       ]
     }
     ```
   * Haz clic en el botón **Create policy** para guardarla. Regresa a la pestaña anterior, selecciona tu política recién creada para adjuntarla al certificado y haz clic en **Create thing**.

11. **Descargar y Activar Credenciales:**
   * En la ventana emergente que aparece, descarga uno por uno los **3 archivos clave** (el Certificado del dispositivo, la Clave privada y el *Amazon Root CA 1*). Recuerda no cerrar la ventana sin descargarlos.
   * Asegúrate de hacer clic en **Activate** si el certificado lo requiere, y luego presiona **Done**.

12. **Copiar el Endpoint:**
   * En el menú lateral izquierdo, ve a **Settings** y copia la dirección que aparece en **Device data endpoint** (ej. `a3lavp493f93pa-ats.iot.us-east-2.amazonaws.com`), ya que la necesitarás en el código de la ESP32.

   


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