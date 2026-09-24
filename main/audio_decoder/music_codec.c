#include "audio_decoder/music_codec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "esp_log.h"

#include "audio_decoder/third_party/miniflac.h"

#define STB_VORBIS_HEADER_ONLY
#include "audio_decoder/third_party/stb_vorbis.c"

static const char *TAG = "MUSIC_CODEC";

static const char *vorbis_error_to_str(int error)
{
    switch (error) {
    case VORBIS__no_error: return "no_error";
    case VORBIS_need_more_data: return "need_more_data";
    case VORBIS_invalid_api_mixing: return "invalid_api_mixing";
    case VORBIS_outofmem: return "outofmem";
    case VORBIS_feature_not_supported: return "feature_not_supported";
    case VORBIS_too_many_channels: return "too_many_channels";
    case VORBIS_file_open_failure: return "file_open_failure";
    case VORBIS_seek_without_length: return "seek_without_length";
    case VORBIS_unexpected_eof: return "unexpected_eof";
    case VORBIS_seek_invalid: return "seek_invalid";
    case VORBIS_invalid_setup: return "invalid_setup";
    case VORBIS_invalid_stream: return "invalid_stream";
    case VORBIS_missing_capture_pattern: return "missing_capture_pattern";
    case VORBIS_invalid_stream_structure_version: return "invalid_stream_structure_version";
    case VORBIS_continued_packet_flag_invalid: return "continued_packet_flag_invalid";
    case VORBIS_incorrect_stream_serial_number: return "incorrect_stream_serial_number";
    case VORBIS_invalid_first_page: return "invalid_first_page";
    case VORBIS_bad_packet_type: return "bad_packet_type";
    case VORBIS_cant_find_last_page: return "cant_find_last_page";
    case VORBIS_seek_failed: return "seek_failed";
    case VORBIS_ogg_skeleton_not_supported: return "ogg_skeleton_not_supported";
    default: return "unknown";
    }
}

static long file_get_size(FILE *fp)
{
    if (!fp) return -1;
    long cur = ftell(fp);
    if (cur < 0) return -1;
    if (fseek(fp, 0, SEEK_END) != 0) return -1;
    long size = ftell(fp);
    (void)fseek(fp, cur, SEEK_SET);
    return size;
}

static long find_oggs_offset(FILE *fp)
{
    if (!fp) return -1;

    if (fseek(fp, 0, SEEK_SET) != 0) return -1;

    const uint8_t signature[4] = {'O', 'g', 'g', 'S'};
    uint8_t buf[1024];
    uint8_t tail[3] = {0, 0, 0};
    size_t tail_len = 0;
    long scanned = 0;
    const long max_scan = 256 * 1024;

    while (scanned < max_scan) {
        size_t n = fread(buf, 1, sizeof(buf), fp);
        if (n == 0) break;

        uint8_t combined[1027];
        memcpy(combined, tail, tail_len);
        memcpy(combined + tail_len, buf, n);
        size_t total = tail_len + n;

        for (size_t i = 0; i + 4 <= total; i++) {
            if (memcmp(combined + i, signature, 4) == 0) {
                long pos = scanned - (long)tail_len + (long)i;
                if (pos >= 0) return pos;
            }
        }

        if (total >= 3) {
            tail[0] = combined[total - 3];
            tail[1] = combined[total - 2];
            tail[2] = combined[total - 1];
            tail_len = 3;
        } else {
            memcpy(tail, combined, total);
            tail_len = total;
        }

        scanned += (long)n;
    }

    return -1;
}

/**
 * @brief Scan for 'fLaC' magic marker in file. MFLAC files may have a private header.
 */
static long find_flac_offset(FILE *fp)
{
    if (!fp) return -1;

    if (fseek(fp, 0, SEEK_SET) != 0) return -1;

    const uint8_t signature[4] = {'f', 'L', 'a', 'C'};
    uint8_t buf[1024];
    uint8_t tail[3] = {0, 0, 0};
    size_t tail_len = 0;
    long scanned = 0;
    const long max_scan = 256 * 1024;

    while (scanned < max_scan) {
        size_t n = fread(buf, 1, sizeof(buf), fp);
        if (n == 0) break;

        uint8_t combined[1027];
        memcpy(combined, tail, tail_len);
        memcpy(combined + tail_len, buf, n);
        size_t total = tail_len + n;

        for (size_t i = 0; i + 4 <= total; i++) {
            if (memcmp(combined + i, signature, 4) == 0) {
                long pos = scanned - (long)tail_len + (long)i;
                if (pos >= 0) return pos;
            }
        }

        if (total >= 3) {
            tail[0] = combined[total - 3];
            tail[1] = combined[total - 2];
            tail[2] = combined[total - 1];
            tail_len = 3;
        } else {
            memcpy(tail, combined, total);
            tail_len = total;
        }

        scanned += (long)n;
    }

    return -1;
}

static stb_vorbis *open_vorbis_from_path(const char *path, int *error)
{
    if (error) *error = 0;
    if (!path) {
        if (error) *error = VORBIS_file_open_failure;
        return NULL;
    }

    FILE *fp = fopen(path, "rb");
    if (!fp) {
        if (error) *error = VORBIS_file_open_failure;
        return NULL;
    }

    long file_size = file_get_size(fp);
    if (file_size <= 0) {
        if (error) *error = VORBIS_unexpected_eof;
        fclose(fp);
        return NULL;
    }

    long oggs_offset = find_oggs_offset(fp);
    if (oggs_offset < 0 || oggs_offset >= file_size) {
        /* Dump first 32 bytes for diagnostics */
        uint8_t head[32] = {0};
        fseek(fp, 0, SEEK_SET);
        size_t rd = fread(head, 1, sizeof(head), fp);
        ESP_LOGE(TAG, "OggS未找到, 文件头(%d字节): %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X",
                 (int)rd,
                 head[0], head[1], head[2], head[3], head[4], head[5], head[6], head[7],
                 head[8], head[9], head[10], head[11], head[12], head[13], head[14], head[15]);
        ESP_LOGE(TAG, "  文本: %.16s", (char *)head);
        if (error) *error = VORBIS_missing_capture_pattern;
        fclose(fp);
        return NULL;
    }

    if (fseek(fp, oggs_offset, SEEK_SET) != 0) {
        if (error) *error = VORBIS_seek_failed;
        fclose(fp);
        return NULL;
    }

    unsigned int section_len = (unsigned int)(file_size - oggs_offset);
    stb_vorbis *vorbis = stb_vorbis_open_file_section(fp, 1, error, NULL, section_len);
    if (!vorbis) {
        return NULL;
    }

    return vorbis;
}

static int str_ends_with_ignore_case(const char *text, const char *suffix)
{
    if (!text || !suffix) return 0;
    size_t tlen = strlen(text);
    size_t slen = strlen(suffix);
    if (slen > tlen) return 0;

    const char *p = text + tlen - slen;
    for (size_t i = 0; i < slen; i++) {
        if (tolower((unsigned char)p[i]) != tolower((unsigned char)suffix[i])) {
            return 0;
        }
    }
    return 1;
}

music_codec_type_t music_codec_detect(const char *path)
{
    if (!path) return MUSIC_CODEC_UNKNOWN;
    if (str_ends_with_ignore_case(path, ".mflac") || str_ends_with_ignore_case(path, ".mfl") ||
        str_ends_with_ignore_case(path, ".flac")) {
        return MUSIC_CODEC_MFLAC;
    }
    if (str_ends_with_ignore_case(path, ".mgg") || str_ends_with_ignore_case(path, ".ogg")) {
        return MUSIC_CODEC_MGG;
    }
    return MUSIC_CODEC_UNKNOWN;
}

typedef struct {
    FILE *fp;
} mflac_reader_t;

static size_t mflac_file_read_cb(uint8_t *buffer, size_t bytes, void *userdata)
{
    mflac_reader_t *ctx = (mflac_reader_t *)userdata;
    if (!ctx || !ctx->fp || !buffer || bytes == 0) return 0;
    return fread(buffer, 1, bytes, ctx->fp);
}

static int16_t pcm32_to_pcm16(int32_t sample, int bits_per_sample)
{
    int32_t value = sample;
    if (bits_per_sample > 16) {
        value >>= (bits_per_sample - 16);
    } else if (bits_per_sample < 16) {
        value <<= (16 - bits_per_sample);
    }

    if (value > 32767) value = 32767;
    if (value < -32768) value = -32768;
    return (int16_t)value;
}

esp_err_t music_codec_decode_mflac(const char *path, music_pcm_cb_t callback, void *user_data)
{
    if (!path || !callback) return ESP_ERR_INVALID_ARG;

    esp_err_t ret = ESP_OK;
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        ESP_LOGE(TAG, "打开 mflac 文件失败: %s", path);
        return ESP_FAIL;
    }

    /* MFLAC files may have a private header before the actual FLAC stream.
     * Scan for 'fLaC' marker and seek to it. */
    long flac_offset = find_flac_offset(fp);
    if (flac_offset < 0) {
        ESP_LOGE(TAG, "MFLAC 文件中未找到 fLaC 标记: %s", path);
        fclose(fp);
        return ESP_FAIL;
    }
    if (flac_offset > 0) {
        ESP_LOGI(TAG, "fLaC 标记位于偏移 %ld", flac_offset);
    }
    if (fseek(fp, flac_offset, SEEK_SET) != 0) {
        ESP_LOGE(TAG, "fseek 到 fLaC 偏移失败");
        fclose(fp);
        return ESP_FAIL;
    }

    mflac_t *decoder = (mflac_t *)malloc(mflac_size());
    if (!decoder) {
        fclose(fp);
        return ESP_ERR_NO_MEM;
    }

    int32_t **samples = (int32_t **)calloc(8, sizeof(int32_t *));
    if (!samples) {
        free(decoder);
        fclose(fp);
        return ESP_ERR_NO_MEM;
    }

    int16_t *interleaved = NULL;
    size_t sample_capacity[8] = {0};
    size_t interleaved_capacity = 0;

    mflac_reader_t reader = {.fp = fp};
    mflac_init(decoder, MINIFLAC_CONTAINER_UNKNOWN, mflac_file_read_cb, &reader);

    MFLAC_RESULT res = mflac_sync(decoder);
    if (res != MFLAC_OK) {
        ESP_LOGE(TAG, "mflac_sync 初始失败: %d", (int)res);
        ret = ESP_FAIL;
        goto cleanup;
    }

    while (mflac_is_metadata(decoder)) {
        res = mflac_sync(decoder);
        if (res != MFLAC_OK) {
            if (res == MFLAC_EOF) break;
            ESP_LOGE(TAG, "mflac metadata sync 失败: %d", (int)res);
            ret = ESP_FAIL;
            goto cleanup;
        }
    }

    while (1) {
        int channels = (int)decoder->flac.frame.header.channels;
        int sample_rate = (int)decoder->flac.frame.header.sample_rate;
        int bps = (int)decoder->flac.frame.header.bps;
        int block_size = (int)decoder->flac.frame.header.block_size;

        if (channels <= 0 || channels > 8 || block_size <= 0 || block_size > 65535) {
            ESP_LOGE(TAG, "mflac 帧参数非法 ch=%d block=%d", channels, block_size);
            ret = ESP_FAIL;
            goto cleanup;
        }

        for (int ch = 0; ch < channels; ch++) {
            if ((size_t)block_size > sample_capacity[ch]) {
                int32_t *new_samples = (int32_t *)realloc(samples[ch], sizeof(int32_t) * (size_t)block_size);
                if (!new_samples) {
                    ESP_LOGE(TAG, "mflac 样本缓冲区扩容失败 ch=%d block=%d", ch, block_size);
                    ret = ESP_ERR_NO_MEM;
                    goto cleanup;
                }
                samples[ch] = new_samples;
                sample_capacity[ch] = (size_t)block_size;
            }
        }

        size_t needed_interleaved = (size_t)block_size * (size_t)channels;
        if (needed_interleaved > interleaved_capacity) {
            int16_t *new_interleaved = (int16_t *)realloc(interleaved, sizeof(int16_t) * needed_interleaved);
            if (!new_interleaved) {
                ESP_LOGE(TAG, "mflac 输出缓冲区扩容失败 samples=%u", (unsigned)needed_interleaved);
                ret = ESP_ERR_NO_MEM;
                goto cleanup;
            }
            interleaved = new_interleaved;
            interleaved_capacity = needed_interleaved;
        }

        res = mflac_decode(decoder, samples);
        if (res == MFLAC_EOF) {
            break;
        }
        if (res != MFLAC_OK) {
            ESP_LOGE(TAG, "mflac_decode 失败: %d", (int)res);
            ret = ESP_FAIL;
            goto cleanup;
        }

        for (int i = 0; i < block_size; i++) {
            for (int ch = 0; ch < channels; ch++) {
                interleaved[i * channels + ch] = pcm32_to_pcm16(samples[ch][i], bps);
            }
        }

        esp_err_t cb_ret = callback(interleaved, (size_t)block_size, channels, sample_rate, user_data);
        if (cb_ret != ESP_OK) {
            ret = cb_ret;
            goto cleanup;
        }

        res = mflac_sync(decoder);
        if (res == MFLAC_EOF) {
            break;
        }
        if (res != MFLAC_OK) {
            ESP_LOGE(TAG, "mflac_sync 帧后失败: %d", (int)res);
            ret = ESP_FAIL;
            goto cleanup;
        }
    }

cleanup:
    free(interleaved);
    for (int i = 0; i < 8; i++) free(samples[i]);
    free(samples);
    free(decoder);
    fclose(fp);
    return ret;
}

esp_err_t music_codec_decode_mgg(const char *path, music_pcm_cb_t callback, void *user_data)
{
    if (!path || !callback) return ESP_ERR_INVALID_ARG;

    int error = 0;
    stb_vorbis *vorbis = open_vorbis_from_path(path, &error);
    if (!vorbis) {
        ESP_LOGE(TAG, "打开 mgg/ogg 失败: %s, err=%d(%s)", path, error, vorbis_error_to_str(error));
        return ESP_FAIL;
    }

    stb_vorbis_info info = stb_vorbis_get_info(vorbis);
    int channels = info.channels;
    int sample_rate = (int)info.sample_rate;

    if (channels <= 0 || channels > 8) {
        stb_vorbis_close(vorbis);
        return ESP_FAIL;
    }

    const int frames_per_chunk = 1024;
    int16_t *buffer = (int16_t *)malloc(sizeof(int16_t) * frames_per_chunk * channels);
    if (!buffer) {
        stb_vorbis_close(vorbis);
        return ESP_ERR_NO_MEM;
    }

    while (1) {
        int got_frames = stb_vorbis_get_samples_short_interleaved(
            vorbis, channels, buffer, frames_per_chunk * channels);
        if (got_frames <= 0) break;

        esp_err_t cb_ret = callback(buffer, (size_t)got_frames, channels, sample_rate, user_data);
        if (cb_ret != ESP_OK) {
            free(buffer);
            stb_vorbis_close(vorbis);
            return cb_ret;
        }
    }

    free(buffer);
    stb_vorbis_close(vorbis);
    return ESP_OK;
}

esp_err_t music_codec_decode_auto(const char *path, music_pcm_cb_t callback, void *user_data)
{
    switch (music_codec_detect(path)) {
    case MUSIC_CODEC_MFLAC:
        return music_codec_decode_mflac(path, callback, user_data);
    case MUSIC_CODEC_MGG:
        return music_codec_decode_mgg(path, callback, user_data);
    default:
        ESP_LOGE(TAG, "不支持的音乐格式: %s", path ? path : "(null)");
        return ESP_ERR_NOT_SUPPORTED;
    }
}

esp_err_t music_codec_get_duration_ms(const char *path, uint32_t *out_duration_ms)
{
    if (!path || !out_duration_ms) return ESP_ERR_INVALID_ARG;
    *out_duration_ms = 0;

    music_codec_type_t codec = music_codec_detect(path);
    if (codec == MUSIC_CODEC_MGG) {
        int error = 0;
        stb_vorbis *vorbis = open_vorbis_from_path(path, &error);
        if (!vorbis) {
            ESP_LOGE(TAG, "读取 mgg/ogg 时长失败: %s, err=%d(%s)", path, error, vorbis_error_to_str(error));
            return ESP_FAIL;
        }
        float sec = stb_vorbis_stream_length_in_seconds(vorbis);
        stb_vorbis_close(vorbis);
        if (sec <= 0.0f) return ESP_FAIL;
        *out_duration_ms = (uint32_t)(sec * 1000.0f);
        return ESP_OK;
    }

    if (codec == MUSIC_CODEC_MFLAC) {
        FILE *fp = fopen(path, "rb");
        if (!fp) return ESP_FAIL;

        /* Scan for 'fLaC' marker, skip private header */
        long flac_offset = find_flac_offset(fp);
        if (flac_offset < 0) {
            fclose(fp);
            return ESP_FAIL;
        }
        if (fseek(fp, flac_offset, SEEK_SET) != 0) {
            fclose(fp);
            return ESP_FAIL;
        }

        mflac_t *decoder = (mflac_t *)malloc(mflac_size());
        if (!decoder) {
            fclose(fp);
            return ESP_ERR_NO_MEM;
        }

        mflac_reader_t reader = {.fp = fp};
        mflac_init(decoder, MINIFLAC_CONTAINER_UNKNOWN, mflac_file_read_cb, &reader);

        MFLAC_RESULT res = mflac_sync(decoder);
        if (res != MFLAC_OK) {
            free(decoder);
            fclose(fp);
            return ESP_FAIL;
        }

        uint32_t sample_rate = 0;
        uint64_t total_samples = 0;
        int found_streaminfo = 0;

        while (res == MFLAC_OK && mflac_is_metadata(decoder)) {
            if (mflac_metadata_is_streaminfo(decoder)) {
                if (mflac_streaminfo_sample_rate(decoder, &sample_rate) == MFLAC_OK &&
                    mflac_streaminfo_total_samples(decoder, &total_samples) == MFLAC_OK &&
                    sample_rate > 0) {
                    *out_duration_ms = (uint32_t)((total_samples * 1000ULL) / sample_rate);
                    found_streaminfo = 1;
                    break;
                }
            }
            res = mflac_sync(decoder);
        }

        free(decoder);
        fclose(fp);
        return found_streaminfo ? ESP_OK : ESP_FAIL;
    }

    return ESP_ERR_NOT_SUPPORTED;
}

typedef struct {
    max98357a_handle_t *speaker;
    uint32_t sample_rate;
    int16_t *stereo_buf;
    size_t stereo_cap_frames;
} playback_ctx_t;

static esp_err_t speaker_pcm_callback(const int16_t *pcm_interleaved,
                                      size_t frames,
                                      int channels,
                                      int sample_rate,
                                      void *user_data)
{
    playback_ctx_t *ctx = (playback_ctx_t *)user_data;
    if (!ctx || !ctx->speaker || !pcm_interleaved) return ESP_ERR_INVALID_ARG;

    if (ctx->sample_rate != (uint32_t)sample_rate) {
        esp_err_t ret = max98357a_set_sample_rate(ctx->speaker, (uint32_t)sample_rate);
        if (ret != ESP_OK) return ret;
        ctx->sample_rate = (uint32_t)sample_rate;
    }

    const int16_t *out_ptr = pcm_interleaved;
    size_t out_samples = frames * (size_t)channels;

    if (channels == 1) {
        if (ctx->stereo_cap_frames < frames) return ESP_ERR_NO_MEM;
        for (size_t i = 0; i < frames; i++) {
            int16_t v = pcm_interleaved[i];
            ctx->stereo_buf[i * 2] = v;
            ctx->stereo_buf[i * 2 + 1] = v;
        }
        out_ptr = ctx->stereo_buf;
        out_samples = frames * 2;
    }

    size_t written = 0;
    return max98357a_write(ctx->speaker, out_ptr, out_samples * sizeof(int16_t), &written, 1000);
}

esp_err_t music_codec_play_file(const char *path, max98357a_handle_t *speaker)
{
    if (!path || !speaker) return ESP_ERR_INVALID_ARG;

    playback_ctx_t ctx = {
        .speaker = speaker,
        .sample_rate = 0,
        .stereo_buf = NULL,
        .stereo_cap_frames = 65535,
    };

    ctx.stereo_buf = (int16_t *)malloc(sizeof(int16_t) * ctx.stereo_cap_frames * 2);
    if (!ctx.stereo_buf) return ESP_ERR_NO_MEM;

    esp_err_t ret = music_codec_decode_auto(path, speaker_pcm_callback, &ctx);
    free(ctx.stereo_buf);
    return ret;
}
