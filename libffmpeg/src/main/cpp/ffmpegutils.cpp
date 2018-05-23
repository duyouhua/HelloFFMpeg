#include "ffmpegutils.h"

void render(AVPacket *packet) {
    int ret;
    ret = avcodec_send_packet(m_pCodecCtx, packet);
    if (ret == 0) {
        ret = avcodec_receive_frame(m_pCodecCtx, m_pYUVFrame);
        if (ret == 0) {

            // Determine required buffer size and allocate buffer
            int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_pCodecCtx->width,
                                                    m_pCodecCtx->height, 1);
            buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
            av_image_fill_arrays(m_pRGBAFrame->data, m_pRGBAFrame->linesize, buffer,
                                 AV_PIX_FMT_RGBA,
                                 m_pCodecCtx->width, m_pCodecCtx->height, 1);



            // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
            sws_ctx = sws_getContext(m_pCodecCtx->width,
                                     m_pCodecCtx->height,
                                     m_pCodecCtx->pix_fmt,
                                     m_pCodecCtx->width,
                                     m_pCodecCtx->height,
                                     AV_PIX_FMT_RGBA,
                                     SWS_BILINEAR,
                                     NULL,
                                     NULL,
                                     NULL);

            ANativeWindow_lock(mNativeWindow, &mWindwoBuffer, 0);

            // 格式转换
            sws_scale(sws_ctx, (uint8_t const *const *) m_pYUVFrame->data,
                      m_pYUVFrame->linesize, 0, m_pCodecCtx->height,
                      m_pRGBAFrame->data, m_pRGBAFrame->linesize);

            // 获取stride
            uint8_t *dst = static_cast<uint8_t *>(mWindwoBuffer.bits);
            int dstStride = mWindwoBuffer.stride * 4;
            uint8_t *src = m_pRGBAFrame->data[0];
            int srcStride = m_pRGBAFrame->linesize[0];

            // 由于window的stride和帧的stride不同,因此需要逐行复制
            int height = m_pCodecCtx->height;
            int h;
            for (h = 0; h < height; h++) {
                memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
            }
            delete(buffer);
            ANativeWindow_unlockAndPost(mNativeWindow);
        } else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else {
            exit(1);
        }
    } else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return;
    } else {
        exit(1);
    }
}

int parse(JNIEnv *env, jobject obj, uint8_t *pBuff, int videosize, uint64_t pts) {
    int paserLength_In = videosize;
    int paserLen;
    int decode_data_length;
    int got_picture = 0;
    uint8_t *pFrameBuff = (uint8_t *) pBuff;
    while (paserLength_In > 0) {
        AVPacket packet;
        av_init_packet(&packet);
        if (m_pCodecPaser == NULL) {
            Java_com_chenxi1991_libffmpeg_FfmpegUtils_init(env, obj);
        }
        if (m_pCodecCtx == NULL) {
            Java_com_chenxi1991_libffmpeg_FfmpegUtils_init(env, obj);
        }
        paserLen = av_parser_parse2(m_pCodecPaser, m_pCodecCtx, &packet.data, &packet.size,
                                    pFrameBuff,
                                    paserLength_In, AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
        paserLength_In -= paserLen;
        pFrameBuff += paserLen;
        if (packet.size > 0) {
            render(&packet);
        }
        av_packet_unref(&packet);
    }
    return 0;
}


jboolean Java_com_chenxi1991_libffmpeg_FfmpegUtils_init(JNIEnv *env, jobject instance) {
    if (isFFmpegInitialized == 0) {
        avcodec_register_all();
        av_register_all();
        isFFmpegInitialized = 1;
    }
    m_pAVCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    m_pCodecCtx = avcodec_alloc_context3(m_pAVCodec);
    m_pCodecPaser = av_parser_init(AV_CODEC_ID_H264);
    if (m_pAVCodec == NULL || m_pCodecCtx == NULL) {
        return 0;
    }

    if (m_pAVCodec->capabilities & CODEC_CAP_TRUNCATED)
        m_pCodecCtx->flags |= CODEC_FLAG_TRUNCATED;

    m_pCodecCtx->thread_count = 4;
    m_pCodecCtx->thread_type = FF_THREAD_FRAME;

    if (avcodec_open2(m_pCodecCtx, m_pAVCodec, NULL) < 0) {
        m_pAVCodec = NULL;
        return 0;
    }

    m_pYUVFrame = av_frame_alloc();
    m_pRGBAFrame = av_frame_alloc();
    if (m_pYUVFrame == NULL || m_pRGBAFrame == NULL) {
        return 0;
    }
    return 1;
}


jboolean
Java_com_chenxi1991_libffmpeg_FfmpegUtils_parse(JNIEnv *env, jobject instance, jbyteArray pBuff,
                                            int size) {
    jbyte *jBuff = (env->GetByteArrayElements(pBuff, 0));
    uint8_t *buff = (uint8_t *) jBuff;
    uint64_t pts = 0;
    jbyte *jBuff2;

    if (size >= fillersize2 && memcmp(fillerbuffer2, buff + size - fillersize2, fillersize2) == 0) {
        parse(env, instance, buff, size - fillersize2, pts);
    } else if (size >= audaudsize2 &&
               memcmp(audaudbuffer2, buff + size - audaudsize2, audaudsize2) == 0) {
        parse(env, instance, buff, size - audaudsize2, pts);
    } else if (size >= audsize2 && memcmp(audbuffer2, buff + size - audsize2, audsize2) == 0) {
        parse(env, instance, buff, size - audsize2, pts);
    } else {
        parse(env, instance, buff, size, pts);
    }
    env->ReleaseByteArrayElements(pBuff, jBuff, 0);

    return 1;
}

jboolean Java_com_chenxi1991_libffmpeg_FfmpegUtils_release(JNIEnv *env, jobject instance) {
    if (m_pCodecCtx) {
        avcodec_close(m_pCodecCtx);
        m_pCodecCtx = NULL;
    }
    if (mNativeWindow) {
        mNativeWindow = NULL;
    }
    av_free(m_pYUVFrame);
    av_free(m_pRGBAFrame);
    av_free(m_pCodecCtx);
    av_parser_close(m_pCodecPaser);
    return 1;
}


void
Java_com_chenxi1991_libffmpeg_FfmpegUtils_setSurface(JNIEnv *env, jobject instance, jobject jsurface) {
    mNativeWindow = ANativeWindow_fromSurface(env, jsurface);
}

