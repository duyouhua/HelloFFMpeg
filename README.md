# HelloFFMpeg
>1、逐帧接收H.264视频帧，使用FFMPEG解码成为YUV420的AVPacket

>2、封装AVPacket为AVFrame，并使用sws_scale格式转化为RGBA

>3、使用ANativeWindow渲染图片进行视频播放
