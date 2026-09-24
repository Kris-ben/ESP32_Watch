/**
 * @file MSM261D4030H1CPM_Driver_V5.cpp
 * @brief MSM261D4030H1CPM PDM MEMS Microphone Driver Implementation
 * 
 * @author Kilo Code
 * @date 2026-01-03
 */

#include "MSM261D4030H1CPM_Driver_V5.hpp"
#include "esp_log.h"
#include "esp_check.h"
#include <string.h>

static const char *TAG = "MSM261D4030H1CPM";

namespace MSM261D4030H1CPM {

// Clock frequency ranges based on datasheet specifications
#define CLOCK_FREQ_SLEEP_MAX        50000      // 50 kHz max for sleep mode
#define CLOCK_FREQ_LOW_POWER_MIN    150000     // 150 kHz min for low-power mode
#define CLOCK_FREQ_LOW_POWER_MAX    900000     // 900 kHz max for low-power mode
#define CLOCK_FREQ_STANDARD_MIN     1100000    // 1.1 MHz min for standard mode
#define CLOCK_FREQ_STANDARD_MAX     4000000    // 4.0 MHz max for standard mode

// Recommended clock frequencies for different modes
#define CLOCK_FREQ_LOW_POWER_TYPICAL    768000   // 768 kHz (datasheet typical)
#define CLOCK_FREQ_STANDARD_TYPICAL     2400000  // 2.4 MHz (datasheet typical)

// Timing specifications from datasheet
#define WAKEUP_TIME_MS              200        // Wake-up time: max 200 μs (use 1 ms for safety)
#define POWER_UP_TIME_MS            20         // Power-up time: max 20 ms
#define MODE_CHANGE_TIME_MS         10         // Mode change time: max 10 ms

MicDriver::MicDriver() 
    : rx_handle_(nullptr)
    , initialized_(false) {
    // Initialize config with default values
    config_ = get_default_config();
}

MicDriver::~MicDriver() {
    end();
}

esp_err_t MicDriver::config(const mic_config_t &cfg) {
    if (initialized_) {
        ESP_LOGW(TAG, "Microphone already initialized, call end() first");
        return ESP_ERR_INVALID_STATE;
    }
    
    // Validate configuration
    esp_err_t ret = validate_config(cfg);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // Store configuration
    config_ = cfg;
    
    ESP_LOGI(TAG, "Configuration set: CLK=GPIO%d, DATA=GPIO%d, SR=%lu Hz, Mode=%d",
             config_.pin_clk, config_.pin_data, config_.sample_rate, config_.mode);
    
    return ESP_OK;
}

esp_err_t MicDriver::begin() {
    if (initialized_) {
        ESP_LOGW(TAG, "Microphone already initialized");
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Initializing MSM261D4030H1CPM microphone...");
    
    // Configure I2S in PDM RX mode
    esp_err_t ret = configure_i2s_pdm();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure I2S PDM: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Enable I2S RX channel
    ret = i2s_channel_enable(rx_handle_);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable I2S channel: %s", esp_err_to_name(ret));
        i2s_del_channel(rx_handle_);
        rx_handle_ = nullptr;
        return ret;
    }
    
    // Wait for microphone power-up and wake-up time
    vTaskDelay(pdMS_TO_TICKS(POWER_UP_TIME_MS + WAKEUP_TIME_MS));
    
    // Test read to verify data is valid
    int16_t test_buffer[64];
    size_t bytes_read = 0;
    ret = i2s_channel_read(rx_handle_, test_buffer, sizeof(test_buffer), &bytes_read, pdMS_TO_TICKS(500));
    if (ret == ESP_OK && bytes_read > 0) {
        // Check if all samples are the same (indicates error)
        bool all_same = true;
        int16_t first_val = test_buffer[0];
        for (size_t i = 1; i < bytes_read / sizeof(int16_t); i++) {
            if (test_buffer[i] != first_val) {
                all_same = false;
                break;
            }
        }
        if (all_same) {
            ESP_LOGW(TAG, "Warning: Test read returned constant value %d - I2S may not be working properly", first_val);
        } else {
            ESP_LOGI(TAG, "Test read successful: data appears valid");
        }
    } else {
        ESP_LOGW(TAG, "Test read failed or no data: %s", esp_err_to_name(ret));
    }
    
    initialized_ = true;
    ESP_LOGI(TAG, "Microphone initialized successfully");
    
    return ESP_OK;
}

void MicDriver::end() {
    if (!initialized_) {
        return;
    }
    
    ESP_LOGI(TAG, "Stopping microphone...");
    
    if (rx_handle_) {
        // Disable and delete I2S channel
        i2s_channel_disable(rx_handle_);
        i2s_del_channel(rx_handle_);
        rx_handle_ = nullptr;
    }
    
    initialized_ = false;
    ESP_LOGI(TAG, "Microphone stopped");
}

esp_err_t MicDriver::read(int16_t* buffer, size_t bytes_to_read, size_t* bytes_read, uint32_t timeout_ms) {
    if (!initialized_) {
        ESP_LOGE(TAG, "Microphone not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (buffer == nullptr || bytes_read == nullptr) {
        ESP_LOGE(TAG, "Invalid buffer or bytes_read pointer");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Read data from I2S
    esp_err_t ret = i2s_channel_read(rx_handle_, buffer, bytes_to_read, bytes_read, 
                                     pdMS_TO_TICKS(timeout_ms));
    
    if (ret != ESP_OK && ret != ESP_ERR_TIMEOUT) {
        ESP_LOGE(TAG, "I2S read error: %s", esp_err_to_name(ret));
        return ret;
    }
    
    return ret;
}

esp_err_t MicDriver::set_mode(mic_mode_t mode) {
    if (mode == config_.mode) {
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Changing mode from %d to %d", config_.mode, mode);
    
    bool was_initialized = initialized_;
    
    // Stop if running
    if (was_initialized) {
        end();
    }
    
    // Update mode
    config_.mode = mode;
    
    // Restart if it was running
    if (was_initialized) {
        vTaskDelay(pdMS_TO_TICKS(MODE_CHANGE_TIME_MS));
        return begin();
    }
    
    return ESP_OK;
}

uint32_t MicDriver::calculate_clock_freq(mic_mode_t mode, uint32_t sample_rate) {
    uint32_t clock_freq;
    
    switch (mode) {
        case MIC_MODE_SLEEP:
            // Sleep mode: use minimum clock (not practical for audio)
            clock_freq = CLOCK_FREQ_SLEEP_MAX;
            ESP_LOGW(TAG, "Sleep mode selected - not suitable for audio recording");
            break;
            
        case MIC_MODE_LOW_POWER:
            // Low-power mode: use typical 768 kHz or calculate based on sample rate
            clock_freq = CLOCK_FREQ_LOW_POWER_TYPICAL;
            // Ensure it's within range
            if (clock_freq < CLOCK_FREQ_LOW_POWER_MIN) {
                clock_freq = CLOCK_FREQ_LOW_POWER_MIN;
            } else if (clock_freq > CLOCK_FREQ_LOW_POWER_MAX) {
                clock_freq = CLOCK_FREQ_LOW_POWER_MAX;
            }
            break;
            
        case MIC_MODE_STANDARD_PERFORMANCE:
        default:
            // Standard mode: use typical 2.4 MHz or calculate based on sample rate
            // PDM oversampling ratio is typically 64x or 128x
            clock_freq = CLOCK_FREQ_STANDARD_TYPICAL;
            // Ensure it's within range
            if (clock_freq < CLOCK_FREQ_STANDARD_MIN) {
                clock_freq = CLOCK_FREQ_STANDARD_MIN;
            } else if (clock_freq > CLOCK_FREQ_STANDARD_MAX) {
                clock_freq = CLOCK_FREQ_STANDARD_MAX;
            }
            break;
    }
    
    ESP_LOGI(TAG, "Calculated clock frequency: %lu Hz for mode %d", clock_freq, mode);
    return clock_freq;
}

esp_err_t MicDriver::validate_config(const mic_config_t &cfg) {
    // Validate GPIO pins
    if (!GPIO_IS_VALID_GPIO(cfg.pin_clk)) {
        ESP_LOGE(TAG, "Invalid CLK pin: %d", cfg.pin_clk);
        return ESP_ERR_INVALID_ARG;
    }
    
    if (!GPIO_IS_VALID_GPIO(cfg.pin_data)) {
        ESP_LOGE(TAG, "Invalid DATA pin: %d", cfg.pin_data);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Validate I2S port
    if (cfg.i2s_port != I2S_NUM_0 && cfg.i2s_port != I2S_NUM_1) {
        ESP_LOGE(TAG, "Invalid I2S port: %d", cfg.i2s_port);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Validate sample rate (common audio sample rates)
    if (cfg.sample_rate < 8000 || cfg.sample_rate > 48000) {
        ESP_LOGW(TAG, "Unusual sample rate: %lu Hz", cfg.sample_rate);
    }
    
    // Validate DMA buffer settings
    if (cfg.dma_buf_count < 2 || cfg.dma_buf_count > 128) {
        ESP_LOGE(TAG, "Invalid DMA buffer count: %d (must be 2-128)", cfg.dma_buf_count);
        return ESP_ERR_INVALID_ARG;
    }
    
    if (cfg.dma_buf_len < 8 || cfg.dma_buf_len > 1024) {
        ESP_LOGE(TAG, "Invalid DMA buffer length: %d (must be 8-1024)", cfg.dma_buf_len);
        return ESP_ERR_INVALID_ARG;
    }
    
    return ESP_OK;
}

esp_err_t MicDriver::configure_i2s_pdm() {
    esp_err_t ret;
    
    // Create I2S channel configuration
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(config_.i2s_port, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = config_.dma_buf_count;
    chan_cfg.dma_frame_num = config_.dma_buf_len;
    chan_cfg.auto_clear = true;  // Auto clear DMA buffer
    
    // Create new I2S RX channel
    ret = i2s_new_channel(&chan_cfg, NULL, &rx_handle_);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2S channel: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Configure PDM RX mode with proper clock and slot settings
    i2s_pdm_rx_config_t pdm_rx_cfg = {
        .clk_cfg = {
            .sample_rate_hz = config_.sample_rate,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .mclk_multiple = I2S_MCLK_MULTIPLE_256,
            .dn_sample_mode = I2S_PDM_DSR_8S,  // 改为8x下采样，提高信号质量
        },
        .slot_cfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
            .slot_mode = I2S_SLOT_MODE_MONO,
            .slot_mask = I2S_PDM_SLOT_LEFT,  // Will be updated below
            .data_fmt = I2S_PDM_DATA_FMT_PCM,  // PCM format (PDM2PCM conversion)
#if SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER
            .hp_en = true,  // Enable high-pass filter
            .hp_cut_off_freq_hz = 35.5,  // 35.5 Hz cutoff frequency
            // 注意：amplify_num 是"乘数"（IDF 默认值为 1）。
            // 原来写 15 会把 PDM 转 PCM 后的直流残余一起放大（实测约 1640），
            // 导致 VAD 永远判定为"有声音"。现在直流已在 voice_dialog.cpp 里被
            // remove_dc_offset() 去掉，这里取 8：兼顾灵敏度与不削顶。
            .amplify_num = 8,
#endif
        },
        .gpio_cfg = {
            .clk = config_.pin_clk,
            .din = config_.pin_data,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    
    // Set slot mask based on channel configuration
    if (config_.channel == MIC_CHANNEL_LEFT) {
        pdm_rx_cfg.slot_cfg.slot_mask = I2S_PDM_SLOT_LEFT;
    } else {
        pdm_rx_cfg.slot_cfg.slot_mask = I2S_PDM_SLOT_RIGHT;
    }
    
    // Initialize I2S channel with PDM RX configuration
    ret = i2s_channel_init_pdm_rx_mode(rx_handle_, &pdm_rx_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize PDM RX mode: %s", esp_err_to_name(ret));
        i2s_del_channel(rx_handle_);
        rx_handle_ = nullptr;
        return ret;
    }
    
    ESP_LOGI(TAG, "I2S PDM configured: SR=%lu Hz, CLK=GPIO%d, DATA=GPIO%d, Channel=%s",
             config_.sample_rate, config_.pin_clk, config_.pin_data,
             config_.channel == MIC_CHANNEL_LEFT ? "LEFT" : "RIGHT");
    
    return ESP_OK;
}

} // namespace MSM261D4030H1CPM
