int OpenH265File(string filename)
{
    int ret;
    unsigned int i;
    m_codecType=H265;
    CLog::AddLogDebug("OpenH265File");

    av_init_packet(&m_pkt);
    if ((ret = avformat_open_input(&m_fmctx, filename.c_str(), NULL, NULL)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
            return ret;
        }
        if ((ret = avformat_find_stream_info(m_fmctx, NULL)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
            return ret;
        }

    //	stream_ctx = av_mallocz_array(ifmt_ctx->nb_streams, sizeof(*stream_ctx));
    //	if (!stream_ctx)
    //		return AVERROR(ENOMEM);

        for (i = 0; i < m_fmctx->nb_streams; i++) {
            AVStream *stream = m_fmctx->streams[i];
            AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
            AVCodecContext *codec_ctx;
            if (!dec) {
                av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
                return AVERROR_DECODER_NOT_FOUND;
            }
            codec_ctx = avcodec_alloc_context3(dec);
            if (!codec_ctx) {
                av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
                return AVERROR(ENOMEM);
            }
            ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
                       "for stream #%u\n", i);
                return ret;
            }
            /* Reencode video & audio and remux subtitles etc. */
            if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
                    || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
                if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
                    codec_ctx->framerate = av_guess_frame_rate(m_fmctx, stream, NULL);
                /* Open decoder */
                ret = avcodec_open2(codec_ctx, dec, NULL);
                if (ret < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
                    return ret;
                }
            }
        }
        av_dump_format(m_fmctx, 0, filename.c_str(), 0);
            return 0;
}
ReadStatus NextH265Frame(unsigned long &sampletime, unsigned long &duration, byte **outframe,size_t& size,bool &isKeyFrame,ReadDirection direction,int &nalltype,byte &extradatapacket, size_t& extradatasize )
{
    ReadStatus re= EndOfFile;
    string szFuncName="NextH265Sample";
    try
    {
        unsigned int stream_index;
        int ret;
        enum AVMediaType type;
        boost::mutex::scoped_lock lock(m_fileMutex);
        while (1)
        {
            if ((ret = av_read_frame(m_fmctx, &m_pkt)) < 0)
                return re;

        if(av_read_frame(m_fmctx, &m_pkt) >= 0)
        {
			naltype=m_pkt.data[4];
            nalltype=naltype;
			stream_index = m_pkt.stream_index;
			if(stream_index==AVMEDIA_TYPE_VIDEO && nalltype==38)
			{
                    memcpy(&extradatapacket,in_stream->codecpar->extradata,in_stream->codecpar->extradata_size);
                    extradatasize=in_stream->codecpar->extradata_size;
                    av_packet_unref(&m_pkt);
			}
             *outframe=reinterpret_cast<byte*>(m_pkt.data);
              size=m_pkt.size;
              duration=m_pkt.duration;
              sampletime=m_pkt.pts;
              isKeyFrame=m_pkt.flags?AV_PKT_FLAG_KEY:false;
              isKeyFrame=m_pkt.flags?AV_PKT_FLAG_KEY:false;
              av_packet_unref(&m_pkt);

            re=Success;
        }
        else
            re=Failure;

    }
    MYCATCH_ALL(szFuncName)
            return Success;
}