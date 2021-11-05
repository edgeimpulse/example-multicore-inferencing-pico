#include <pico/util/queue.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include "ei_run_classifier.h"
#include "core1_thread.h"

queue_t results_queue;
queue_t data_queue;
const uint LED_PIN = 25;
float signal_buf[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr)
{
  memcpy(out_ptr, signal_buf + offset, length * sizeof(float));
  return 0;
}

void core1_run(void)
{
  queue_init(&results_queue, sizeof(ei_impulse_result_t), 2);
  // add space for 4 additional samples to avoid blocking main thread
  queue_init(&data_queue, sizeof(float), EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE + (4 * sizeof(float)));

  multicore_launch_core1(core1_entry);
}

void core1_entry()
{
  ei_impulse_result_t result = {nullptr};
  signal_t features_signal;

  // init LED
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  features_signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
  features_signal.get_data = &raw_feature_get_data;

  while (1) {
    // get data from sensor
    for(int i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i++) {
      queue_remove_blocking(&data_queue, &signal_buf[i]);
    }
    
    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
    if (res != 0)
      continue;

    // return results to core0
    queue_add_blocking(&results_queue, &result);

    // toggle LED
    gpio_put(LED_PIN, !gpio_get(LED_PIN));
  }
}