#include <stdio.h>
#include <string.h>
#include <hardware/gpio.h>
#include <pico/stdio_usb.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/util/queue.h>
#include "ei_classifier_types.h"
#include "porting/ei_classifier_porting.h"
#include "core1_thread.h"
#include "accelerometer.h"

int main()
{
  ei_impulse_result_t res = {nullptr};
  float acceleration[EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME];
  uint64_t last_sample_time = 0;

  stdio_init_all();
  accel_init();
  core1_run();

  while (true)
  {
    ei_printf("Edge Impulse standalone inferencing on multicore (Raspberry Pi Pico)\n");

    while (1)
    {
      if(queue_try_remove(&results_queue, &res)) {
        ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                  res.timing.dsp, res.timing.classification, res.timing.anomaly);
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
        {
          ei_printf("\t%s: %.5f\n", res.classification[ix].label,
                                    res.classification[ix].value);
        }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("\tanomaly score: %.3f\n", res.anomaly);
#endif
        ei_sleep(2000);
      }

      // read sample every 10000 us = 10ms (= 100 Hz)
      if(ei_read_timer_us() - last_sample_time >= 10000) {
        accel_get_data(acceleration);
        for(int i=0; i<EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME; i++) {
          if(queue_try_add(&data_queue, &acceleration[i]) == false) {
            // ei_printf("Data queue full!\n");
            break;
          }
        }
        last_sample_time = ei_read_timer_us();
      }
    }
  }
}