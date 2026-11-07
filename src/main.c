
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <pico/stdlib.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "tkjhat/sdk.h"

// Exercise 4. Include the libraries necessaries to use the usb-serial-debug,
// and tinyusb Tehtävä 4 . Lisää usb-serial-debugin ja tinyusbin käyttämiseen
// tarvittavat kirjastot.

#define DEFAULT_STACK_SIZE 2048
#define CDC_ITF_TX 1

// Tehtävä 3: Tilakoneen esittely Add missing states.
// Exercise 3: Definition of the state machine. Add missing states.
enum state { WAITING = 1 };
enum state programState = WAITING;

// Tehtävä 3: Valoisuuden globaali muuttuja
// Exercise 3: Global variable for ambient light
uint32_t ambientLight;

static void btn_fxn(uint gpio, uint32_t eventMask) {
  // Tehtävä 1: Vaihda LEDin tila käyttämällä TKJHAT SDK:n sopivaa funktiota.
  // Exercise 1: Toggle the LED using the appropriate function from the TKJHAT
  // SDK.
}

typedef enum { ORIENT_UNKNOWN, ORIENT_DOT, ORIENT_DASH } Orientation;

static void sensor_task(void *arg) {
  (void)arg;

  float ax, ay, az;
  float gx, gy, gz, t;

  char message[10] = "";
  size_t message_len = 0;

  // Setting up the sensor.
  if (init_ICM42670() == 0) {
    printf("ICM-42670P initialized successfully!\n");
    if (ICM42670_start_with_default_values() != 0) {
      printf("ICM-42670P could not initialize accelerometer or gyroscope\n");
      vTaskDelete(NULL);
    }
  } else {
    printf("Failed to initialize ICM-42670P.\n");
    vTaskDelete(NULL);
  }

  bool btn1_was_pressed = false;
  bool btn2_was_pressed = false;
  bool message_confirmed = false;
  TickType_t btn2_pressed_at = 0;

  // Start collection data here. Infinite loop.
  while (1) {
    Orientation orientation = ORIENT_UNKNOWN;
    bool btn1_pressed = gpio_get(BUTTON1);
    bool btn2_pressed = gpio_get(BUTTON2);

    if (ICM42670_read_sensor_data(&ax, &ay, &az, &gx, &gy, &gz, &t) == 0) {
      // Calculate total acceleration magnitude
      float magnitude = sqrtf(ax * ax + ay * ay + az * az);

      // In a stable state g is close to 1
      if (magnitude >= 0.75f && magnitude <= 1.25f) {
        float z_share = fabsf(az) / magnitude;

        if (z_share >= 0.75f) {
          orientation = ORIENT_DOT;
        } else if (z_share <= 0.65f) {
          orientation = ORIENT_DASH;
        }

      } else {
        printf("Orientation unknown: acceleration outside accepted range\n");
      }

    } else {
      printf("Failed to read imu data\n");
    }

    // Symbol handling here
    char symbol = orientation == ORIENT_DOT    ? '.'
                  : orientation == ORIENT_DASH ? '-'
                                               : '\0';

    if (btn1_pressed && !btn1_was_pressed && symbol) {

      if (message_len >= sizeof message - 1) // check we don't exceed the cap
      {
        printf("Message: %s | Max length reached!\n", message);

      } else {
        message[message_len] = symbol;
        message_len++;
        message[message_len] = '\0';
        printf("Message: %s\n", message);
      }
    }

    if (btn2_pressed) {
      // On a new press record time and reset confirmed flag
      if (!btn2_was_pressed) {
        btn2_pressed_at = xTaskGetTickCount();
        message_confirmed = false;
      }
      // After 200ms hold confirm and clear draft
      if (!message_confirmed &&
          (xTaskGetTickCount() - btn2_pressed_at >= pdMS_TO_TICKS(200))) {
        printf("%s  \n", message);
        printf("Message sent \n");
        message_confirmed = true;
        message[0] = '\0';
        message_len = 0;
      }
    } // On release, add a space if hold didn't fire
    else if (btn2_was_pressed && !message_confirmed) {
      if (message_len >= sizeof message - 1) // check we don't exceed the cap
      {
        printf("Message: %s | Max length reached!\n", message);
      } else {
        message[message_len] = ' ';
        message_len++;
        message[message_len] = '\0';
        printf("Message: %s\n", message);
      }
    }

    btn1_was_pressed = btn1_pressed;
    btn2_was_pressed = btn2_pressed;

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void print_task(void *arg) {
  (void)arg;

  while (1) {

    // Tehtävä 3: Kun tila on oikea, tulosta sensoridata merkkijonossa
    // serial-monitor-ikkunaan
    //            Muista tilamuutos
    //            Älä unohda kommentoida seuraavaa koodiriviä.
    // Exercise 3: Print out sensor data as string to serial monitor if the
    // state is correct
    //             Remember to modify state
    //             Do not forget to comment next line of code.
    tight_loop_contents();

    // Exercise 4. Use usb_serial_print() instead of printf() for debug output
    //             produced after the USB serial library has been initialized.
    //             Do not replace error messages that can execute before
    //             usb_serial_init(). Use the TinyUSB library to send data
    //             through the other serial port (CDC 1). You can use the
    //             functions at
    //             https://github.com/hathach/tinyusb/blob/master/src/class/cdc/cdc_device.h
    //             You can find an example at hello_dual_cdc
    //             The data written using this should be provided using csv
    //             timestamp, illuminance
    // Tehtävä 4. Käytä usb_serial_print()-funktiota printf()-funktion sijaan
    //            debug-tulostukseen sen jälkeen, kun USB-sarjaliikennekirjasto
    //            on alustettu. Älä korvaa virheilmoituksia, jotka voivat
    //            suorittua ennen usb_serial_init()-kutsua. Käytä
    //            TinyUSB-kirjastoa datan lähettämiseen toisen sarjaportin (CDC
    //            1) kautta. Voit käyttää funktioita:
    //            https://github.com/hathach/tinyusb/blob/master/src/class/cdc/cdc_device.h
    //            Esimerkki löytyy hello_dual_cdc-projektista.
    //            Tällä menetelmällä kirjoitettu data tulee antaa CSV-muodossa:
    //            timestamp, illuminance

    // Exercise 3. Just for sanity check. Please, comment this out
    // Tehtävä 3: Just for sanity check. Please, comment this out
    printf("printTask\n");

    // Do not remove this
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// Exercise 4: Uncomment the following line to activate the TinyUSB library.
// Tehtävä 4:  Poista seuraavan rivin kommentointi aktivoidaksesi
// TinyUSB-kirjaston.

/*
static void usbTask(void *arg) {
    (void)arg;
    while (1) {
        tud_task();              // With FreeRTOS wait for events
                                 // Do not add vTaskDelay.
    }
}*/

int main() {

  // Exercise 4: Comment the statement stdio_init_all();
  //             Instead, add AT THE END OF MAIN (before vTaskStartScheduler();)
  //             adequate statements to enable the TinyUSB library and the
  //             usb-serial-debug. You can see hello_dual_cdc for help In
  //             CMakeLists.txt add usb_serial_debug to target_link_libraries.
  //             In CMakeLists.txt deactivate pico_enable_stdio_usb
  // Tehtävä 4:  Kommentoi lause stdio_init_all();
  //             Sen sijaan lisää MAIN LOPPUUN (ennen vTaskStartScheduler();)
  //             tarvittavat komennot aktivoidaksesi TinyUSB-kirjaston ja
  //             usb-serial-debugin. Voit katsoa apua esimerkistä
  //             hello_dual_cdc. Lisää CMakeLists.txt-tiedoston
  //             target_link_libraries-kohtaan usb_serial_debug. Poista
  //             CMakeLists.txt-tiedostosta käytöstä pico_enable_stdio_usb

  stdio_init_all();

  // Uncomment this lines if you want to wait till the serial monitor is
  // connected
  /*while (!stdio_usb_connected()){
      sleep_ms(10);
  }*/

  init_hat_sdk();
  sleep_ms(300); // Wait some time so initialization of USB and hat is done.

  init_button1();
  init_button2();

  TaskHandle_t hSensorTask, hPrintTask, hUSB = NULL;

  // Exercise 4: Uncomment this xTaskCreate to create the task that enables dual
  // USB communication. Tehtävä 4: Poista tämän xTaskCreate-rivin kommentointi
  // luodaksesi tehtävän, joka mahdollistaa kaksikanavaisen USB-viestinnän.

  /*
  xTaskCreate(usbTask, "usb", 2048, NULL, 3, &hUSB);
  #if (configNUMBER_OF_CORES > 1)
      vTaskCoreAffinitySet(hUSB, 1u << 0);
  #endif
  */

  // Create the tasks with xTaskCreate
  BaseType_t result = xTaskCreate(
      sensor_task,        // (en) Task function
      "sensor",           // (en) Name of the task
      DEFAULT_STACK_SIZE, // (en) Size of the stack for this task (in words).
                          // Generally 1024 or 2048
      NULL,               // (en) Arguments of the task
      2,                  // (en) Priority of this task
      &hSensorTask);      // (en) A handle to control the execution of this task

  if (result != pdPASS) {
    printf("Sensor task creation failed\n");
    return 0;
  }
  result = xTaskCreate(
      print_task,         // (en) Task function
      "print",            // (en) Name of the task
      DEFAULT_STACK_SIZE, // (en) Size of the stack for this task (in words).
                          // Generally 1024 or 2048
      NULL,               // (en) Arguments of the task
      2,                  // (en) Priority of this task
      &hPrintTask);       // (en) A handle to control the execution of this task

  if (result != pdPASS) {
    printf("Print Task creation failed\n");
    return 0;
  }

  // Start the scheduler (never returns)
  vTaskStartScheduler();

  // Never reach this line.
  return 0;
}
