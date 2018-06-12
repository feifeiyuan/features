#define SIZE_CLOM		6
#define BUFSIZE  		256
#define CHANNEL_PATH  	"/sys/class/misc/sprd_ptm/chn"

#define DEBUG			0

typedef unsigned int u32;
typedef struct data_chn * chn_node;

struct bm_per_info {
	u32 count;
	u32 t_start;
	u32 t_stop;
	u32 tmp1;
	u32 tmp2;
	u32 per_data[11][SIZE_CLOM];
};

struct data_chn{
	char *name;
	chn_node next;
};

static chn_node CreateChnLink(chn_node tail, char *buffer, int buffer_len);
static chn_node find_chn_link(void);
static u32 show_title(u32 dis_all, chn_node point);