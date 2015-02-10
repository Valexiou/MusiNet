/**
 * @file core.h  Internal API
 *
 * Copyright (C) 2010 Creytiv.com
 */

// "My_variables"
#include <pthread.h>	// <-------------- Edited

/**
 * RFC 3551:
 *
 *    0 -  95  Static payload types
 *   96 - 127  Dynamic payload types
 */
enum {
	PT_CN       = 13,
	PT_STAT_MIN = 0,
	PT_STAT_MAX = 95,
	PT_DYN_MIN  = 96,
	PT_DYN_MAX  = 127
};


/** Media constants */
enum {
	AUDIO_BANDWIDTH = 128000 /**< Bandwidth for audio in bits/s       */
};


/*
 * Account
 */


/** Defines the answermodes */
enum answermode {
	ANSWERMODE_MANUAL = 0,
	ANSWERMODE_EARLY,
	ANSWERMODE_AUTO
};

struct account {
	char *buf;                   /**< Buffer for the SIP address         */
	struct sip_addr laddr;       /**< Decoded SIP address                */
	struct uri luri;             /**< Decoded AOR uri                    */
	char *dispname;              /**< Display name                       */
	char *aor;                   /**< Local SIP uri                      */

	/* parameters: */
	enum answermode answermode;  /**< Answermode for incoming calls      */
	struct le acv[8];            /**< List elements for aucodecl         */
	struct list aucodecl;        /**< List of preferred audio-codecs     */
	char *auth_user;             /**< Authentication username            */
	char *auth_pass;             /**< Authentication password            */
	char *mnatid;                /**< Media NAT handling                 */
	char *mencid;                /**< Media encryption type              */
	const struct mnat *mnat;     /**< MNAT module                        */
	const struct menc *menc;     /**< MENC module                        */
	char *outbound[2];           /**< Optional SIP outbound proxies      */
	uint32_t ptime;              /**< Configured packet time in [ms]     */
	uint32_t regint;             /**< Registration interval in [seconds] */
	uint32_t pubint;             /**< Publication interval in [seconds]  */
	char *regq;                  /**< Registration Q-value               */
	char *rtpkeep;               /**< RTP Keepalive mechanism            */
	char *sipnat;                /**< SIP Nat mechanism                  */
	char *stun_user;             /**< STUN Username                      */
	char *stun_pass;             /**< STUN Password                      */
	char *stun_host;             /**< STUN Hostname                      */
	uint16_t stun_port;          /**< STUN Port number                   */
	struct le vcv[4];            /**< List elements for vidcodecl        */
	struct list vidcodecl;       /**< List of preferred video-codecs     */
};


/*
 * Audio Player
 */

struct auplay_st {
	struct auplay *ap;
};

struct auplay {
	struct le        le;
	const char      *name;
	auplay_alloc_h  *alloch;
};


/*
 * Audio Source
 */

struct ausrc_st {
	struct ausrc *as;
};

struct ausrc {
	struct le        le;
	const char      *name;
	ausrc_alloc_h   *alloch;
};


/*
 * Audio Stream
 */

struct audio;

typedef void (audio_event_h)(int key, bool end, void *arg);
typedef void (audio_err_h)(int err, const char *str, void *arg);

int audio_alloc(struct audio **ap, const struct config *cfg,
		struct call *call, struct sdp_session *sdp_sess, int label,
		const struct mnat *mnat, struct mnat_sess *mnat_sess,
		const struct menc *menc, struct menc_sess *menc_sess,
		uint32_t ptime, const struct list *aucodecl,
		audio_event_h *eventh, audio_err_h *errh, void *arg);
int  audio_start(struct audio *a);
void audio_stop(struct audio *a);
int  audio_encoder_set(struct audio *a, const struct aucodec *ac,
		       int pt_tx, const char *params);
int  audio_decoder_set(struct audio *a, const struct aucodec *ac,
		       int pt_rx, const char *params);
struct stream *audio_strm(const struct audio *a);
int  audio_send_digit(struct audio *a, char key);
void audio_sdp_attr_decode(struct audio *a);
int  audio_print_rtpstat(struct re_printf *pf, const struct audio *au);

// Additional "my_function" for our scope:
int rx_audio_alloc(struct audio **ap, const struct config *cfg,uint32_t ptime);
int tx_audio_alloc(struct audio **ap, const struct config *cfg,
		struct call *call, struct sdp_session *sdp_sess, int label,
		const struct mnat *mnat, struct mnat_sess *mnat_sess,
		const struct menc *menc, struct menc_sess *menc_sess,
		uint32_t ptime, const struct list *aucodecl,
		audio_event_h *eventh, audio_err_h *errh, void *arg);
		
void atx_sock_cpy(struct audio *a1, struct audio *a2);


/*
 * BFCP
 */

struct bfcp;
int bfcp_alloc(struct bfcp **bfcpp, struct sdp_session *sdp_sess,
	       const char *proto, bool offerer,
	       const struct mnat *mnat, struct mnat_sess *mnat_sess);
int bfcp_start(struct bfcp *bfcp);


/*
 * Call Control
 */

struct call;

/** Call parameters */
struct call_prm {
	enum vidmode vidmode;
	int af;
};

int  call_alloc(struct call **callp, const struct config *cfg,
		struct list *lst,
		const char *local_name, const char *local_uri,
		struct account *acc, struct ua *ua, const struct call_prm *prm,
		const struct sip_msg *msg, struct call *xcall,
		call_event_h *eh, void *arg);
int  call_connect(struct call *call, const struct pl *paddr);
int  call_accept(struct call *call, struct sipsess_sock *sess_sock,
		 const struct sip_msg *msg);
int  call_hangup(struct call *call, uint16_t scode, const char *reason);
int  call_progress(struct call *call);
int  call_answer(struct call *call, uint16_t scode);
int  call_sdp_get(const struct call *call, struct mbuf **descp, bool offer);
int  call_jbuf_stat(struct re_printf *pf, const struct call *call);
int  call_info(struct re_printf *pf, const struct call *call);
int  call_reset_transp(struct call *call);
int  call_notify_sipfrag(struct call *call, uint16_t scode,
			 const char *reason, ...);
int  call_af(const struct call *call);
void call_set_xrtpstat(struct call *call);


/*
 * Conf
 */

int conf_get_range(const struct conf *conf, const char *name,
		   struct range *rng);
int conf_get_csv(const struct conf *conf, const char *name,
		 char *str1, size_t sz1, char *str2, size_t sz2);


/*
 * Media control
 */

int mctrl_handle_media_control(struct pl *body, bool *pfu);


/*
 * Media NAT traversal
 */

struct mnat {
	struct le le;
	const char *id;
	const char *ftag;
	mnat_sess_h *sessh;
	mnat_media_h *mediah;
	mnat_update_h *updateh;
};

const struct mnat *mnat_find(const char *id);


/*
 * Metric
 */

struct metric {
	/* internal stuff: */
	struct tmr tmr;
	uint64_t ts_start;
	bool started;

	/* counters: */
	uint32_t n_packets;
	uint32_t n_bytes;
	uint32_t n_err;

	/* bitrate calculation */
	uint32_t cur_bitrate;
	uint64_t ts_last;
	uint32_t n_bytes_last;
};

void     metric_init(struct metric *metric);
void     metric_reset(struct metric *metric);
void     metric_add_packet(struct metric *metric, size_t packetsize);
uint32_t metric_avg_bitrate(const struct metric *metric);


/*
 * Module
 */

int module_init(const struct conf *conf);
void module_app_unload(void);


/*
 * Network
 */

int net_reset(void);


/*
 * Register client
 */

struct reg;

int  reg_add(struct list *lst, struct ua *ua, int regid);
int  reg_register(struct reg *reg, const char *reg_uri,
		    const char *params, uint32_t regint, const char *outbound);
void reg_unregister(struct reg *reg);
bool reg_isok(const struct reg *reg);
int  reg_sipfd(const struct reg *reg);
int  reg_debug(struct re_printf *pf, const struct reg *reg);
int  reg_status(struct re_printf *pf, const struct reg *reg);


/*
 * RTP keepalive
 */

struct rtpkeep;

int  rtpkeep_alloc(struct rtpkeep **rkp, const char *method, int proto,
		   struct rtp_sock *rtp, struct sdp_media *sdp);
void rtpkeep_refresh(struct rtpkeep *rk, uint32_t ts);


/*
 * SIP Request
 */

int sip_req_send(struct ua *ua, const char *method, const char *uri,
		 sip_resp_h *resph, void *arg, const char *fmt, ...);


/*
 * SDP
 */

int sdp_decode_multipart(const struct pl *ctype_prm, struct mbuf *mb);
const struct sdp_format *sdp_media_format_cycle(struct sdp_media *m);


/*
 * Stream
 */

struct rtp_header;

enum {STREAM_PRESZ = 4+12}; /* same as RTP_HEADER_SIZE */

typedef void (stream_rtp_h)(const struct rtp_header *hdr, struct mbuf *mb,
			    void *arg);
typedef void (stream_rtcp_h)(struct rtcp_msg *msg, void *arg);

/** Defines a generic media stream */
struct stream {
	struct le le;            /**< Linked list element                   */
	struct config_avt cfg;   /**< Stream configuration                  */
	struct call *call;       /**< Ref. to call object                   */
	struct sdp_media *sdp;   /**< SDP Media line                        */
	struct rtp_sock *rtp;    /**< RTP Socket                            */
	struct rtpkeep *rtpkeep; /**< RTP Keepalive                         */
	struct rtcp_stats rtcp_stats;/**< RTCP statistics                   */
	struct jbuf *jbuf;       /**< Jitter Buffer for incoming RTP        */
	struct mnat_media *mns;  /**< Media NAT traversal state             */
	const struct menc *menc; /**< Media encryption module               */
	struct menc_sess *mencs; /**< Media encryption session state        */
	struct menc_media *mes;  /**< Media Encryption media state          */
	struct metric metric_tx; /**< Metrics for transmit                  */
	struct metric metric_rx; /**< Metrics for receiving                 */
	char *cname;             /**< RTCP Canonical end-point identifier   */
	uint32_t ssrc_rx;        /**< Incoming syncronizing source          */
	uint32_t pseq;           /**< Sequence number for incoming RTP      */
	int pt_enc;              /**< Payload type for encoding             */
	bool rtcp;               /**< Enable RTCP                           */
	bool rtcp_mux;           /**< RTP/RTCP multiplex supported by peer  */
	bool jbuf_started;       /**< True if jitter-buffer was started     */
	stream_rtp_h *rtph;      /**< Stream RTP handler                    */
	stream_rtcp_h *rtcph;    /**< Stream RTCP handler                   */
	void *arg;               /**< Handler argument                      */
};

int stream_alloc(struct stream **sp, const struct config_avt *cfg,
		  struct call *call, struct sdp_session *sdp_sess,
		  const char *name, int label,
		  const struct mnat *mnat, struct mnat_sess *mnat_sess,
		  const struct menc *menc, struct menc_sess *menc_sess,
		  const char *cname,
		  stream_rtp_h *rtph, stream_rtcp_h *rtcph, void *arg);
struct sdp_media *stream_sdpmedia(const struct stream *s);
int  stream_send(struct stream *s, bool marker, int pt, uint32_t ts,
		 struct mbuf *mb);
void stream_update(struct stream *s);
void stream_update_encoder(struct stream *s, int pt_enc);
int  stream_jbuf_stat(struct re_printf *pf, const struct stream *s);
void stream_hold(struct stream *s, bool hold);
void stream_set_srate(struct stream *s, uint32_t srate_tx, uint32_t srate_rx);
void stream_send_fir(struct stream *s, bool pli);
void stream_reset(struct stream *s);
void stream_set_bw(struct stream *s, uint32_t bps);
int  stream_debug(struct re_printf *pf, const struct stream *s);
int  stream_print(struct re_printf *pf, const struct stream *s);

// Additional "my_function" for our scope:
int rx_stream_alloc(struct stream **sp, const struct config_avt *cfg, stream_rtp_h *rtph, stream_rtcp_h *rtcph, void *arg);	// <-------------- Edited
int tx_stream_alloc(struct stream **sp, const struct config_avt *cfg,
		  struct call *call, struct sdp_session *sdp_sess,
		  const char *name, int label,
		  const struct mnat *mnat, struct mnat_sess *mnat_sess,
		  const struct menc *menc, struct menc_sess *menc_sess,
		  const char *cname);

void stream_sock_cpy(struct stream *s1, struct stream *s2);

/*
 * User-Agent
 */

struct ua;

void         ua_event(struct ua *ua, enum ua_event ev, struct call *call,
		      const char *fmt, ...);
void         ua_printf(const struct ua *ua, const char *fmt, ...);

struct tls  *uag_tls(void);
const char  *uag_allowed_methods(void);


/*
 * Video Display
 */

struct vidisp {
	struct le        le;
	const char      *name;
	vidisp_alloc_h  *alloch;
	vidisp_update_h *updateh;
	vidisp_disp_h   *disph;
	vidisp_hide_h   *hideh;
};

struct vidisp *vidisp_get(struct vidisp_st *st);


/*
 * Video Source
 */

struct vidsrc {
	struct le         le;
	const char       *name;
	vidsrc_alloc_h   *alloch;
	vidsrc_update_h  *updateh;
};

struct vidsrc *vidsrc_get(struct vidsrc_st *st);


/*
 * Video Stream
 */

struct video;

int  video_alloc(struct video **vp, const struct config *cfg,
		 struct call *call, struct sdp_session *sdp_sess, int label,
		 const struct mnat *mnat, struct mnat_sess *mnat_sess,
		 const struct menc *menc, struct menc_sess *menc_sess,
		 const char *content, const struct list *vidcodecl);
int  video_start(struct video *v, const char *peer);
void video_stop(struct video *v);
int  video_encoder_set(struct video *v, struct vidcodec *vc,
		       int pt_tx, const char *params);
int  video_decoder_set(struct video *v, struct vidcodec *vc, int pt_rx,
		       const char *fmtp);
struct stream *video_strm(const struct video *v);
void video_update_picture(struct video *v);
void video_sdp_attr_decode(struct video *v);
int  video_print(struct re_printf *pf, const struct video *v);

// Additional "my_function" for our scope:
int rx_video_alloc(struct video **vp, const struct config *cfg);
int tx_video_alloc(struct video **vp, const struct config *cfg,
		 struct call *call, struct sdp_session *sdp_sess, int label,
		 const struct mnat *mnat, struct mnat_sess *mnat_sess,
		 const struct menc *menc, struct menc_sess *menc_sess,
		 const char *content, const struct list *vidcodecl);
		 
void vtx_sock_cpy(struct video* v1, struct video* v2);

// // Additional "my_function" for our scope:
// void vtx_obtain(struct call *call, struct video *v);	
// void tx_obtain(struct call *call, struct audio *a);
// void tx_cpy(struct audio *a1, struct audio *a2);

// Additional "my_variables" for our scope:
struct node
{
	struct node *prev;
	struct node *next;
	
	char* cname;
	uint32_t a_ssrc;
	uint32_t v_ssrc;
	struct audio *a;
	struct video *v;
	
	int vbuf_busy;
	int abuf_busy;

	pthread_mutex_t abufmtx;
	pthread_mutex_t vbufmtx;
		
	pthread_cond_t abufprocessing;
	pthread_cond_t vbufprocessing;	
};


// Additional "my_variables" for our scope:
extern struct node *first;
extern struct node *last;
extern uint32_t max_assrc;
extern uint32_t max_vssrc;

// Additional "my_function" for our scope:
int check_av_ssrc(uint32_t ssrc);
uint32_t return_max_ssrc(int packet_type);

// Additional "my_function" for our scope:
int init_list( struct node **f, struct node **l, uint32_t ssrc, int packet_type);
int create_node(struct node **l, uint32_t ssrc, int packet_type);
void *find_user(uint32_t ssrc, int packet_type);
void *insert_node(uint32_t ssrc, int packet_type);

// Additional "my_function" for our scope:
void video_jbuf_update(void *arg, const struct rtp_header *hdr, struct mbuf *mb);
void audio_jbuf_update(void *arg, const struct rtp_header *hdr, struct mbuf *mb);

// Additional "my_function" for our scope:
void stream_jbuf_put(struct stream **s, const struct rtp_header *hdr, struct mbuf *mb);

// // Additional "my_function" for our scope:
// void audio_stream_recv_handler(const struct rtp_header *hdr, struct mbuf *mb, void *arg);
// void video_stream_recv_handler(const struct rtp_header *hdr, struct mbuf *mb, void *arg);
// struct stream *get_video_stream(void *arg);
// struct stream *get_audio_stream(void *arg);

// Additional "my_function" for our scope:
void vrx_update(struct call *call, struct video **v);
void arx_update(struct call *call, struct audio **a);

// Additional "my_function" for our scope:
void rx_set_vidisp(struct video *v);

