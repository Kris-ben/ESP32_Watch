/**
 * @file MSM261D4030H1CPM_Driver_V5.hpp
 * @brief MSM261D4030H1CPM PDM MEMS Microphone Driver for ESP32
 * 
 * This driver supports the MSM261D4030H1CPM digital PDM MEMS microphone
 * with multi-mode operation (Standard Performance, Low-Power, Sleep).
 * 
 * Key Features:
 * - PDM digital output via I2S interface
 * - Clock frequency: 1.1-4.0 MHz (Standard), 150-900 kHz (Low-Power)
 * - High SNR: 64 dB(A) @ 2.4 MHz
 * - Multiple power modes
 * 
 * @author Kilo Code
 * @date 2026-01-03
 */

#ifndef MSM261D4030H1CPM_DRIVER_V5_HPP
#define MSM261D4030H1CPM_DRIVER_V5_HPP

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/i2s_pdm.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace MSM261D4030H1CPM {

/**
 * @brief Microphone operating modes based on clock frequency
 */
typedef enum {
    MIC_MODE_SLEEP = 0,           ///< Sleep mode: f_CLK ≤ 50 kHz, I_SLEEP ~1 μA
    MIC_MODE_LOW_POWER,           ///< Low-Power mode: 150-900 kHz, SNR 62 dB(A)
    MIC_MODE_STANDARD_PERFORMANCE ///< Standard mode: 1.1-4.0 MHz, SNR 64 dB(A)
} mic_mode_t;

/**
 * @brief L/R channel selection
 */
typedef enum {
    MIC_CHANNEL_LEFT = 0,  ///< L/R pin = GND, data on CLK rising edge
    MIC_CHANNEL_RIGHT = 1  ///< L/R pin = VDD, data on CLK falling edge
} mic_channel_t;

/**
 * @brief Microphone configuration structure
 */
typedef struct {
    // GPIO pins
    gpio_num_t pin_clk;        ///< Clock pin (I2S CLK)
    gpio_num_t pin_data;       ///< Data pin (I2S DIN for PDM RX)
    
    // I2S configuration
    i2s_port_t i2s_port;       ///< I2S port number (I2S_NUM_0 or I2S_NUM_1)
    uint32_t sample_rate;      ///< Sample rate in Hz (8000, 16000, 44100, 48000, etc.)
    
    // Microphone settings
    mic_channel_t channel;     ///< Left or Right channel (L/R pin connection)
    mic_mode_t mode;           ///< Operating mode (affects clock frequency)
    
    // Buffer settings
    size_t dma_buf_count;      ///< Number of DMA buffers (default: 4)
    size_t dma_buf_len;        ///< Length of each DMA buffer in samples (default: 1024)
    
    // Advanced settings
    bool use_apll;             ///< Use APLL for better clock accuracy (default: true)
    uint32_t mclk_multiple;    ///< MCLK multiple of sample rate (default: 256)
} mic_config_t;

/**
 * @brief MSM261D4030H1CPM Microphone Driver Class
 */
class MicDriver {
public:
    /**
     * @brief Constructor
     */
    MicDriver();
    
    /**
     * @brief Destructor
     */
    ~MicDriver();
    
    /**
     * @brief Configure the microphone
     * @param cfg Configuration structure
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t config(const mic_config_t &cfg);
    
    /**
     * @brief Initialize and start the microphone
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t begin();
    
    /**
     * @brief Stop and deinitialize the microphone
     */
    void end();
    
    /**
     * @brief Read audio data from microphone
     * @param buffer Buffer to store audio data (16-bit PCM samples)
     * @param bytes_to_read Number of bytes to read
     * @param bytes_read Pointer to store actual bytes read
     * @param timeout_ms Timeout in milliseconds
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t read(int16_t* buffer, size_t bytes_to_read, size_t* bytes_read, uint32_t timeout_ms);
    
    /**
     * @brief Check if microphone is initialized
     * @return true if initialized, false otherwise
     */
    bool is_initialized() const { return initialized_; }
    
    /**
     * @brief Get current operating mode
     * @return Current microphone mode
     */
    mic_mode_t get_mode() const { return config_.mode; }
    
    /**
     * @brief Change operating mode (requires re-initialization)
     * @param mode New operating mode
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t set_mode(mic_mode_t mode);
    
    /**
     * @brief Get actual sample rate being used
     * @return Sample rate in Hz
     */
    uint32_t get_sample_rate() const { return config_.sample_rate; }
    
    /**
     * @brief Get I2S port number
     * @return I2S port number
     */
    i2s_port_t get_i2s_port() const { return config_.i2s_port; }

private:
    mic_config_t config_;           ///< Current configuration
    i2s_chan_handle_t rx_handle_;   ///< I2S RX channel handle
    bool initialized_;              ///< Initialization status
    
    /**
     * @brief Calculate appropriate clock frequency for mode
     * @param mode Operating mode
     * @param sample_rate Desired sample rate
     * @return Clock frequency in Hz
     */
    uint32_t calculate_clock_freq(mic_mode_t mode, uint32_t sample_rate);
    
    /**
     * @brief Validate configuration parameters
     * @param cfg Configuration to validate
     * @return ESP_OK if valid, error code otherwise
     */
    esp_err_t validate_config(const mic_config_t &cfg);
    
    /**
     * @brief Configure I2S in PDM RX mode
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t configure_i2s_pdm();
};

/**
 * @brief Get default microphone configuration
 * @return Default configuration structure
 */
inline mic_config_t get_default_config() {
    mic_config_t cfg = {
        .pin_clk = GPIO_NUM_4,              // Default CLK pin (IO4)
        .pin_data = GPIO_NUM_5,             // Default DATA pin (IO5)
        .i2s_port = I2S_NUM_0,              // I2S port 0
        .sample_rate = 16000,               // 16 kHz sample rate
        .channel = MIC_CHANNEL_LEFT,        // Left channel (L/R = GND)
        .mode = MIC_MODE_STANDARD_PERFORMANCE, // Standard performance mode
        .dma_buf_count = 4,                 // 4 DMA buffers
        .dma_buf_len = 1024,                // 1024 samples per buffer
        .use_apll = true,                   // Use APLL for accuracy
        .mclk_multiple = 256                // MCLK = 256 * sample_rate
    };
    return cfg;
}

} // namespace MSM261D4030H1CPM

#endif // MSM261D4030H1CPM_DRIVER_V5_HPP
