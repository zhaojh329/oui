/*
 * rpcd - UBUS RPC server
 *
 * Copyright (C) 2017 Jianhui Zhao <jianhuizhao329@gmail.com>
 * Copyright (C) 2013-2015 Jo-Philipp Wich <jow@openwrt.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define _GNU_SOURCE /* crypt() */

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <signal.h>
#include <glob.h>
#include <libubox/blobmsg_json.h>
#include <libubox/avl-cmp.h>
#include <libubus.h>
#include <uci.h>

#include <rpcd/plugin.h>

/* limit of log size buffer */
#define RPC_VUCI_MAX_LOGSIZE		(128 * 1024)
#define RPC_VUCI_DEF_LOGSIZE       (16 * 1024)

/* location of menu definitions */
#define RPC_VUCI_MENU_FILES        "/usr/share/vuci/menu.d/*.json"

static const struct rpc_daemon_ops *ops;

static struct blob_buf buf;
static struct uci_context *cursor;

enum {
	RPC_I_NAME,
	RPC_I_ACTION,
	__RPC_I_MAX,
};

static const struct blobmsg_policy rpc_init_policy[__RPC_I_MAX] = {
	[RPC_I_NAME]   = { .name = "name",   .type = BLOBMSG_TYPE_STRING },
	[RPC_I_ACTION] = { .name = "action", .type = BLOBMSG_TYPE_STRING },
};

enum {
	RPC_D_DATA,
	__RPC_D_MAX
};

static const struct blobmsg_policy rpc_data_policy[__RPC_D_MAX] = {
	[RPC_D_DATA]   = { .name = "data",  .type = BLOBMSG_TYPE_STRING },
};

enum {
	RPC_K_KEYS,
	__RPC_K_MAX
};

static const struct blobmsg_policy rpc_sshkey_policy[__RPC_K_MAX] = {
	[RPC_K_KEYS]   = { .name = "keys",   .type = BLOBMSG_TYPE_ARRAY },
};

enum {
	RPC_P_USER,
	RPC_P_PASSWORD,
	__RPC_P_MAX
};

static const struct blobmsg_policy rpc_password_policy[__RPC_P_MAX] = {
	[RPC_P_USER]     = { .name = "user",     .type = BLOBMSG_TYPE_STRING },
	[RPC_P_PASSWORD] = { .name = "password", .type = BLOBMSG_TYPE_STRING },
};

enum {
	RPC_OM_LIMIT,
	RPC_OM_OFFSET,
	RPC_OM_PATTERN,
	__RPC_OM_MAX
};

static const struct blobmsg_policy rpc_opkg_match_policy[__RPC_OM_MAX] = {
	[RPC_OM_LIMIT]    = { .name = "limit",    .type = BLOBMSG_TYPE_INT32  },
	[RPC_OM_OFFSET]   = { .name = "offset",   .type = BLOBMSG_TYPE_INT32  },
	[RPC_OM_PATTERN]  = { .name = "pattern",  .type = BLOBMSG_TYPE_STRING },
};

enum {
	RPC_OP_PACKAGE,
	__RPC_OP_MAX
};

static const struct blobmsg_policy rpc_opkg_package_policy[__RPC_OP_MAX] = {
	[RPC_OP_PACKAGE]  = { .name = "package",  .type = BLOBMSG_TYPE_STRING },
};

enum {
	RPC_UPGRADE_KEEP,
	__RPC_UPGRADE_MAX
};

static const struct blobmsg_policy rpc_upgrade_policy[__RPC_UPGRADE_MAX] = {
	[RPC_UPGRADE_KEEP] = { .name = "keep",    .type = BLOBMSG_TYPE_BOOL },
};

enum {
	RPC_MENU_SESSION,
	__RPC_MENU_MAX
};

static const struct blobmsg_policy rpc_menu_policy[__RPC_MENU_MAX] = {
	[RPC_MENU_SESSION] = { .name = "ubus_rpc_session",
	                                          .type = BLOBMSG_TYPE_STRING },
};

enum {
	RPC_SWITCH_NAME,
	__RPC_SWITCH_MAX
};

static const struct blobmsg_policy rpc_switch_policy[__RPC_SWITCH_MAX] = {
	[RPC_SWITCH_NAME]  = { .name = "switch",  .type = BLOBMSG_TYPE_STRING },
};


static int rpc_errno_status(void)
{
	switch (errno) {
	case EACCES:
		return UBUS_STATUS_PERMISSION_DENIED;

	case ENOTDIR:
		return UBUS_STATUS_INVALID_ARGUMENT;

	case ENOENT:
		return UBUS_STATUS_NOT_FOUND;

	case EINVAL:
		return UBUS_STATUS_INVALID_ARGUMENT;

	default:
		return UBUS_STATUS_UNKNOWN_ERROR;
	}
}

static void log_read(FILE *log, int logsize)
{
	int len;
	char *logbuf;

	if (logsize == 0)
		logsize = RPC_VUCI_DEF_LOGSIZE;

	len = (logsize > RPC_VUCI_MAX_LOGSIZE) ? RPC_VUCI_MAX_LOGSIZE : logsize;
	logbuf = blobmsg_alloc_string_buffer(&buf, "log", len + 1);

	if (!logbuf)
		return;

	while (logsize > RPC_VUCI_MAX_LOGSIZE) {
		len = logsize % RPC_VUCI_MAX_LOGSIZE;

		if (len == 0)
			len = RPC_VUCI_MAX_LOGSIZE;

		fread(logbuf, 1, len, log);
		logsize -= len;
	}

	len = fread(logbuf, 1, logsize, log);
	*(logbuf + len) = 0;

	blobmsg_add_string_buffer(&buf);
}

static int rpc_vuci_system_log(struct ubus_context *ctx, struct ubus_object *obj,
                     struct ubus_request_data *req, const char *method,
                     struct blob_attr *msg)
{
	FILE *log;
	int logsize = 0;
	const char *logfile = NULL;
	struct stat st;
	struct uci_package *p;
	struct uci_element *e;
	struct uci_section *s;
	struct uci_ptr ptr = { .package = "system" };

	uci_load(cursor, ptr.package, &p);

	if (!p)
		return UBUS_STATUS_NOT_FOUND;

	uci_foreach_element(&p->sections, e) {
		s = uci_to_section(e);

		if (strcmp(s->type, "system"))
			continue;

		ptr.o = NULL;
		ptr.option = "log_type";
		ptr.section = e->name;
		uci_lookup_ptr(cursor, &ptr, NULL, true);
		break;
	}

	if (ptr.o && ptr.o->type == UCI_TYPE_STRING &&
	    !strcmp(ptr.o->v.string, "file")) {
		ptr.o = NULL;
		ptr.option = "log_file";
		uci_lookup_ptr(cursor, &ptr, NULL, true);

		if (ptr.o && ptr.o->type == UCI_TYPE_STRING)
			logfile = ptr.o->v.string;
		else
			logfile = "/var/log/messages";

		if (stat(logfile, &st) || !(log = fopen(logfile, "r")))
			goto fail;

		logsize = st.st_size;
	} else {
		ptr.o = NULL;
		ptr.option = "log_size";
		uci_lookup_ptr(cursor, &ptr, NULL, true);

		if (ptr.o && ptr.o->type == UCI_TYPE_STRING)
			logsize = atoi(ptr.o->v.string) * 1024;

		if (!(log = popen("logread", "r")))
			goto fail;
	}

	blob_buf_init(&buf, 0);

	log_read(log, logsize);
	fclose(log);

	uci_unload(cursor, p);
	ubus_send_reply(ctx, req, buf.head);
	return 0;

fail:
	uci_unload(cursor, p);
	return rpc_errno_status();
}

static int rpc_vuci_system_dmesg(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg)
{
	FILE *log;

	if (!(log = popen("dmesg", "r")))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);

	log_read(log, RPC_VUCI_MAX_LOGSIZE);
	fclose(log);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_system_diskfree(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
	int i;
	void *c;
	struct statvfs s;
	const char *fslist[] = {
		"/",    "root",
		"/tmp", "tmp",
	};

	blob_buf_init(&buf, 0);

	for (i = 0; i < sizeof(fslist) / sizeof(fslist[0]); i += 2) {
		if (statvfs(fslist[i], &s))
			continue;

		c = blobmsg_open_table(&buf, fslist[i+1]);

		blobmsg_add_u32(&buf, "total", s.f_blocks * s.f_frsize);
		blobmsg_add_u32(&buf, "free",  s.f_bfree  * s.f_frsize);
		blobmsg_add_u32(&buf, "used", (s.f_blocks - s.f_bfree) * s.f_frsize);

		blobmsg_close_table(&buf, c);
	}

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_process_list(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg)
{
	FILE *top;
	void *c, *d;
	char line[1024];
	char *pid, *ppid, *user, *stat, *vsz, *pvsz, *pcpu, *cmd;

	if (!(top = popen("/bin/busybox top -bn1", "r")))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "processes");

	while (fgets(line, sizeof(line) - 1, top)) {
		pid  = strtok(line, " ");

		if (*pid < '0' || *pid > '9')
			continue;

		ppid = strtok(NULL, " ");
		user = strtok(NULL, " ");
		stat = strtok(NULL, " ");

		if (!stat)
			continue;

		if (!*(stat + 1))
			*(stat + 1) = ' ';

		if (!*(stat + 2))
			*(stat + 2) = ' ';

		*(stat + 3) = 0;

		vsz  = strtok(stat + 4, " ");
		pvsz = strtok(NULL, " ");
		pcpu = strtok(NULL, " ");
		cmd  = strtok(NULL, "\n");

		if (!cmd)
			continue;

		d = blobmsg_open_table(&buf, NULL);

		blobmsg_add_u32(&buf, "pid", atoi(pid));
		blobmsg_add_u32(&buf, "ppid", atoi(ppid));
		blobmsg_add_string(&buf, "user", user);
		blobmsg_add_string(&buf, "stat", stat);
		blobmsg_add_u32(&buf, "vsize", atoi(vsz) * 1024);
		blobmsg_add_u32(&buf, "vsize_percent", atoi(pvsz));
		blobmsg_add_u32(&buf, "cpu_percent", atoi(pcpu));
		blobmsg_add_string(&buf, "command", cmd);

		blobmsg_close_table(&buf, d);
	}

	fclose(top);
	blobmsg_close_array(&buf, c);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_init_list(struct ubus_context *ctx, struct ubus_object *obj,
                    struct ubus_request_data *req, const char *method,
                    struct blob_attr *msg)
{
	int n;
	void *c, *t;
	char *p, path[PATH_MAX];
	struct stat s;
	struct dirent *e;
	FILE *f;
	DIR *d;

	if (!(d = opendir("/etc/init.d")))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "initscripts");

	while ((e = readdir(d)) != NULL) {
		snprintf(path, sizeof(path) - 1, "/etc/init.d/%s", e->d_name);

		if (stat(path, &s) || !S_ISREG(s.st_mode) || !(s.st_mode & S_IXUSR))
			continue;

		if ((f = fopen(path, "r")) != NULL) {
			n = -1;
			p = fgets(path, sizeof(path) - 1, f);

			if (!p || !strstr(p, "/etc/rc.common"))
				goto skip;

			t = blobmsg_open_table(&buf, NULL);

			blobmsg_add_string(&buf, "name", e->d_name);

			while (fgets(path, sizeof(path) - 1, f)) {
				p = strtok(path, "= \t");

				if (!strcmp(p, "START") && !!(p = strtok(NULL, "= \t\n")))
				{
					n = atoi(p);
					blobmsg_add_u32(&buf, "start", n);
				}
				else if (!strcmp(p, "STOP") && !!(p = strtok(NULL, "= \t\n")))
				{
					blobmsg_add_u32(&buf, "stop", atoi(p));
					break;
				}
			}

			if (n > -1) {
				snprintf(path, sizeof(path) - 1, "/etc/rc.d/S%02d%s",
				         n, e->d_name);

				blobmsg_add_u8(&buf, "enabled",
				               (!stat(path, &s) && (s.st_mode & S_IXUSR)));
			} else {
				blobmsg_add_u8(&buf, "enabled", 0);
			}

			blobmsg_close_table(&buf, t);

skip:
			fclose(f);
		}
	}

	closedir(d);
	blobmsg_close_array(&buf, c);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_init_action(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	int fd;
	pid_t pid;
	struct stat s;
	char path[PATH_MAX];
	const char *action;
	struct blob_attr *tb[__RPC_I_MAX];

	blobmsg_parse(rpc_init_policy, __RPC_I_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_I_NAME] || !tb[RPC_I_ACTION])
		return UBUS_STATUS_INVALID_ARGUMENT;

	action = blobmsg_data(tb[RPC_I_ACTION]);

	if (strcmp(action, "start") && strcmp(action, "stop") &&
	    strcmp(action, "reload") && strcmp(action, "restart") &&
	    strcmp(action, "enable") && strcmp(action, "disable"))
		return UBUS_STATUS_INVALID_ARGUMENT;

	snprintf(path, sizeof(path) - 1, "/etc/init.d/%s",
	         (char *)blobmsg_data(tb[RPC_I_NAME]));

	if (stat(path, &s))
		return rpc_errno_status();

	if (!(s.st_mode & S_IXUSR))
		return UBUS_STATUS_PERMISSION_DENIED;

	switch ((pid = fork())) {
	case -1:
		return rpc_errno_status();

	case 0:
		uloop_done();

		if ((fd = open("/dev/null", O_RDWR)) > -1) {
			dup2(fd, 0);
			dup2(fd, 1);
			dup2(fd, 2);

			close(fd);
		}

		chdir("/");

		if (execl(path, path, action, NULL))
			return rpc_errno_status();

	default:
		return 0;
	}
}

static int rpc_vuci_rclocal_get(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	FILE *f;
	char data[4096] = { 0 };

	if (!(f = fopen("/etc/rc.local", "r")))
		return rpc_errno_status();

	fread(data, sizeof(data) - 1, 1, f);
	fclose(f);

	blob_buf_init(&buf, 0);
	blobmsg_add_string(&buf, "data", data);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_rclocal_set(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	FILE *f;
	struct blob_attr *tb[__RPC_D_MAX];

	blobmsg_parse(rpc_data_policy, __RPC_D_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_D_DATA] || blobmsg_data_len(tb[RPC_D_DATA]) >= 4096)
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!(f = fopen("/etc/rc.local", "w")))
		return rpc_errno_status();

	fwrite(blobmsg_data(tb[RPC_D_DATA]),
	       blobmsg_data_len(tb[RPC_D_DATA]) - 1, 1, f);

	fclose(f);
	return 0;
}

static int rpc_vuci_crontab_get(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	FILE *f;
	char data[4096] = { 0 };

	if (!(f = fopen("/etc/crontabs/root", "r")))
		return rpc_errno_status();

	fread(data, sizeof(data) - 1, 1, f);
	fclose(f);

	blob_buf_init(&buf, 0);
	blobmsg_add_string(&buf, "data", data);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_crontab_set(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	FILE *f;
	struct stat s;
	struct blob_attr *tb[__RPC_D_MAX];

	blobmsg_parse(rpc_data_policy, __RPC_D_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_D_DATA] || blobmsg_data_len(tb[RPC_D_DATA]) >= 4096)
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (stat("/etc/crontabs", &s) && mkdir("/etc/crontabs", 0755))
		return rpc_errno_status();

	if (!(f = fopen("/etc/crontabs/root", "w")))
		return rpc_errno_status();

	fwrite(blobmsg_data(tb[RPC_D_DATA]),
	       blobmsg_data_len(tb[RPC_D_DATA]) - 1, 1, f);

	fclose(f);
	return 0;
}

static int rpc_vuci_sshkeys_get(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	FILE *f;
	void *c;
	char *p, line[4096];

	if (!(f = fopen("/etc/dropbear/authorized_keys", "r")))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "keys");

	while (fgets(line, sizeof(line) - 1, f)) {
		for (p = line + strlen(line) - 1; (p > line) && isspace(*p); p--)
			*p = 0;

		for (p = line; isspace(*p); p++)
			*p = 0;

		if (*p)
			blobmsg_add_string(&buf, NULL, p);
	}

	blobmsg_close_array(&buf, c);
	fclose(f);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_sshkeys_set(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	FILE *f;
	int rem;
	struct blob_attr *cur, *tb[__RPC_K_MAX];

	blobmsg_parse(rpc_sshkey_policy, __RPC_K_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_K_KEYS])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!(f = fopen("/etc/dropbear/authorized_keys", "w")))
		return rpc_errno_status();

	blobmsg_for_each_attr(cur, tb[RPC_K_KEYS], rem) {
		if (blobmsg_type(cur) != BLOBMSG_TYPE_STRING)
			continue;

		fwrite(blobmsg_data(cur), blobmsg_data_len(cur) - 1, 1, f);
		fwrite("\n", 1, 1, f);
	}

	fclose(f);
	return 0;
}

static int rpc_vuci_password_set(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg)
{
	pid_t pid;
	int fd, fds[2];
	struct stat s;
	struct blob_attr *tb[__RPC_P_MAX];

	blobmsg_parse(rpc_password_policy, __RPC_P_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_P_USER] || !tb[RPC_P_PASSWORD])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (stat("/usr/bin/passwd", &s))
		return UBUS_STATUS_NOT_FOUND;

	if (!(s.st_mode & S_IXUSR))
		return UBUS_STATUS_PERMISSION_DENIED;

	if (pipe(fds))
		return rpc_errno_status();

	switch ((pid = fork())) {
	case -1:
		close(fds[0]);
		close(fds[1]);
		return rpc_errno_status();

	case 0:
		uloop_done();

		dup2(fds[0], 0);
		close(fds[0]);
		close(fds[1]);

		if ((fd = open("/dev/null", O_RDWR)) > -1) {
			dup2(fd, 1);
			dup2(fd, 2);
			close(fd);
		}

		chdir("/");

		if (execl("/usr/bin/passwd", "/usr/bin/passwd",
		          blobmsg_data(tb[RPC_P_USER]), NULL))
			return rpc_errno_status();

	default:
		close(fds[0]);

		write(fds[1], blobmsg_data(tb[RPC_P_PASSWORD]),
		              blobmsg_data_len(tb[RPC_P_PASSWORD]) - 1);
		write(fds[1], "\n", 1);

		usleep(100 * 1000);

		write(fds[1], blobmsg_data(tb[RPC_P_PASSWORD]),
		              blobmsg_data_len(tb[RPC_P_PASSWORD]) - 1);
		write(fds[1], "\n", 1);

		close(fds[1]);

		waitpid(pid, NULL, 0);

		return 0;
	}
}

static int rpc_vuci_led_list(struct ubus_context *ctx, struct ubus_object *obj,
                   struct ubus_request_data *req, const char *method,
                   struct blob_attr *msg)
{
	DIR *d;
	FILE *f;
	void *list, *led, *trigger;
	char *p, *active_trigger, line[512];
	struct dirent *e;

	if (!(d = opendir("/sys/class/leds")))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);
	list = blobmsg_open_array(&buf, "leds");

	while ((e = readdir(d)) != NULL) {
		snprintf(line, sizeof(line) - 1, "/sys/class/leds/%s/trigger",
		         e->d_name);

		if (!(f = fopen(line, "r")))
			continue;

		led = blobmsg_open_table(&buf, NULL);

		blobmsg_add_string(&buf, "name", e->d_name);

		if (fgets(line, sizeof(line) - 1, f)) {
			trigger = blobmsg_open_array(&buf, "triggers");

			for (p = strtok(line, " \n"), active_trigger = NULL;
			     p != NULL;
			     p = strtok(NULL, " \n")) {
				if (*p == '[') {
					*(p + strlen(p) - 1) = 0;
					*p++ = 0;
					active_trigger = p;
				}

				blobmsg_add_string(&buf, NULL, p);
			}

			blobmsg_close_array(&buf, trigger);

			if (active_trigger)
				blobmsg_add_string(&buf, "active_trigger", active_trigger);
		}

		fclose(f);

		snprintf(line, sizeof(line) - 1, "/sys/class/leds/%s/brightness",
		         e->d_name);

		if ((f = fopen(line, "r")) != NULL) {
			if (fgets(line, sizeof(line) - 1, f))
				blobmsg_add_u32(&buf, "brightness", atoi(line));

			fclose(f);
		}

		snprintf(line, sizeof(line) - 1, "/sys/class/leds/%s/max_brightness",
		         e->d_name);

		if ((f = fopen(line, "r")) != NULL) {
			if (fgets(line, sizeof(line) - 1, f))
				blobmsg_add_u32(&buf, "max_brightness", atoi(line));

			fclose(f);
		}

		blobmsg_close_table(&buf, led);
	}

	closedir(d);

	blobmsg_close_array(&buf, list);
	ubus_send_reply(ctx, req, buf.head);

	return 0;
}

static int rpc_vuci_usb_list(struct ubus_context *ctx, struct ubus_object *obj,
                   struct ubus_request_data *req, const char *method,
                   struct blob_attr *msg)
{
	DIR *d;
	FILE *f;
	int i;
	void *list, *device;
	char *p, line[512];
	struct stat s;
	struct dirent *e;

	const char *attributes[] = {
		"manufacturer", "vendor_name",  "s",
		"product",      "product_name", "s",
		"idVendor",     "vendor_id",    "x",
		"idProduct",    "product_id",   "x",
		"serial",       "serial",       "s",
		"speed",        "speed",        "d",
	};

	if (!(d = opendir("/sys/bus/usb/devices")))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);
	list = blobmsg_open_array(&buf, "devices");

	while ((e = readdir(d)) != NULL) {
		if (e->d_name[0] < '0' || e->d_name[0] > '9')
			continue;

		snprintf(line, sizeof(line) - 1,
		         "/sys/bus/usb/devices/%s/%s", e->d_name, attributes[0]);

		if (stat(line, &s))
			continue;

		device = blobmsg_open_table(&buf, NULL);

		blobmsg_add_string(&buf, "name", e->d_name);

		for (i = 0; i < sizeof(attributes) / sizeof(attributes[0]); i += 3) {
			snprintf(line, sizeof(line) - 1,
					 "/sys/bus/usb/devices/%s/%s", e->d_name, attributes[i]);

			if (!(f = fopen(line, "r")))
				continue;

			if (fgets(line, sizeof(line) - 1, f)) {
				switch (*attributes[i+2]) {
				case 'x':
					blobmsg_add_u32(&buf, attributes[i+1],
					                strtoul(line, NULL, 16));
					break;

				case 'd':
					blobmsg_add_u32(&buf, attributes[i+1],
					                strtoul(line, NULL, 10));
					break;

				default:
					if ((p = strchr(line, '\n')) != NULL)
						while (p > line && isspace(*p))
							*p-- = 0;

					blobmsg_add_string(&buf, attributes[i+1], line);
					break;
				}
			}

			fclose(f);
		}

		blobmsg_close_table(&buf, device);
	}

	closedir(d);

	blobmsg_close_array(&buf, list);
	ubus_send_reply(ctx, req, buf.head);

	return 0;
}

static int rpc_vuci_upgrade_test(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg)
{
	const char *cmd[4] = { "sysupgrade", "--test", "/tmp/firmware.bin", NULL };
	return ops->exec(cmd, NULL, NULL, NULL, NULL, NULL, ctx, req);
}

static int rpc_vuci_upgrade_start(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
	return 0;
}

static int rpc_vuci_upgrade_clean(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
	if (unlink("/tmp/firmware.bin"))
		return rpc_errno_status();

	return 0;
}

static int rpc_vuci_backup_restore(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
	const char *cmd[4] = { "sysupgrade", "--restore-backup",
	                       "/tmp/backup.tar.gz", NULL };

	return ops->exec(cmd, NULL, NULL, NULL, NULL, NULL, ctx, req);
}

static int rpc_vuci_backup_clean(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg)
{
	if (unlink("/tmp/backup.tar.gz"))
		return rpc_errno_status();

	return 0;
}

static int rpc_vuci_backup_config_get(struct ubus_context *ctx, struct ubus_object *obj,
                            struct ubus_request_data *req, const char *method,
                            struct blob_attr *msg)
{
	FILE *f;
	char conf[2048] = { 0 };

	if (!(f = fopen("/etc/sysupgrade.conf", "r")))
		return rpc_errno_status();

	fread(conf, sizeof(conf) - 1, 1, f);
	fclose(f);

	blob_buf_init(&buf, 0);
	blobmsg_add_string(&buf, "config", conf);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_backup_config_set(struct ubus_context *ctx, struct ubus_object *obj,
                            struct ubus_request_data *req, const char *method,
                            struct blob_attr *msg)
{
	FILE *f;
	struct blob_attr *tb[__RPC_D_MAX];

	blobmsg_parse(rpc_data_policy, __RPC_D_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_D_DATA])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (blobmsg_data_len(tb[RPC_D_DATA]) >= 2048)
		return UBUS_STATUS_NOT_SUPPORTED;

	if (!(f = fopen("/etc/sysupgrade.conf", "w")))
		return rpc_errno_status();

	fwrite(blobmsg_data(tb[RPC_D_DATA]),
	       blobmsg_data_len(tb[RPC_D_DATA]) - 1, 1, f);

	fclose(f);
	return 0;
}

struct backup_state {
	bool open;
	void *array;
};

static int backup_parse_list(struct blob_buf *blob, char *buf, int len, void *priv)
{
	struct backup_state *s = priv;
	char *nl = strchr(buf, '\n');

	if (!nl)
		return 0;

	if (!s->open)
	{
		s->open  = true;
		s->array = blobmsg_open_array(blob, "files");
	}

	*nl = 0;
	blobmsg_add_string(blob, NULL, buf);

	return (nl - buf + 1);
}

static int backup_finish_list(struct blob_buf *blob, int status, void *priv)
{
	struct backup_state *s = priv;

	if (!s->open)
		return UBUS_STATUS_NO_DATA;

	blobmsg_close_array(blob, s->array);

	return UBUS_STATUS_OK;
}

static int rpc_vuci_backup_list(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	struct backup_state *state = NULL;
	const char *cmd[3] = { "sysupgrade", "--list-backup", NULL };

	state = malloc(sizeof(*state));

	if (!state)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(state, 0, sizeof(*state));

	return ops->exec(cmd, NULL, backup_parse_list, NULL, backup_finish_list,
	                 state, ctx, req);
}

static int rpc_vuci_reset_test(struct ubus_context *ctx, struct ubus_object *obj,
                     struct ubus_request_data *req, const char *method,
                     struct blob_attr *msg)
{
	FILE *mtd;
	struct stat s;
	char line[64] = { 0 };
	bool supported = false;

	if (!stat("/sbin/mtd", &s) && (s.st_mode & S_IXUSR)) {
		if ((mtd = fopen("/proc/mtd", "r")) != NULL) {
			while (fgets(line, sizeof(line) - 1, mtd)) {
				if (strstr(line, "\"rootfs_data\"")) {
					supported = true;
					break;
				}
			}

			fclose(mtd);
		}
	}

	blob_buf_init(&buf, 0);
	blobmsg_add_u8(&buf, "supported", supported);

	ubus_send_reply(ctx, req, buf.head);

	return 0;
}

static int rpc_vuci_reset_start(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	switch (fork()) {
	case -1:
		return rpc_errno_status();

	case 0:
		uloop_done();

		chdir("/");

		close(0);
		close(1);
		close(2);

		sleep(1);

		execl("/sbin/jffs2reset", "/sbin/jffs2reset", "-r", "-y", NULL);

		return rpc_errno_status();

	default:
		return 0;
	}
}

static int rpc_vuci_reboot(struct ubus_context *ctx, struct ubus_object *obj,
                 struct ubus_request_data *req, const char *method,
                 struct blob_attr *msg)
{
	switch (fork()) {
	case -1:
		return rpc_errno_status();

	case 0:
		chdir("/");

		close(0);
		close(1);
		close(2);

		sleep(1);

		execl("/sbin/reboot", "/sbin/reboot", NULL);

		return rpc_errno_status();

	default:
		return 0;
	}
}


static FILE *dnsmasq_leasefile(void)
{
	FILE *leases = NULL;
	struct uci_package *p;
	struct uci_element *e;
	struct uci_section *s;
	struct uci_ptr ptr = {
		.package = "dhcp",
		.section = NULL,
		.option  = "leasefile"
	};

	uci_load(cursor, ptr.package, &p);

	if (!p)
		return NULL;

	uci_foreach_element(&p->sections, e) {
		s = uci_to_section(e);

		if (strcmp(s->type, "dnsmasq"))
			continue;

		ptr.section = e->name;
		uci_lookup_ptr(cursor, &ptr, NULL, true);
		break;
	}

	if (ptr.o && ptr.o->type == UCI_TYPE_STRING)
		leases = fopen(ptr.o->v.string, "r");

	uci_unload(cursor, p);

	return leases;
}

static int rpc_vuci_network_leases(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
	FILE *leases;
	void *c, *d;
	char line[128];
	char *ts, *mac, *addr, *name;
	time_t now = time(NULL);

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "leases");

	leases = dnsmasq_leasefile();

	if (!leases)
		goto out;

	while (fgets(line, sizeof(line) - 1, leases)) {
		ts   = strtok(line, " \t");
		mac  = strtok(NULL, " \t");
		addr = strtok(NULL, " \t");
		name = strtok(NULL, " \t");

		if (!ts || !mac || !addr || !name)
			continue;

		if (strchr(addr, ':'))
			continue;

		d = blobmsg_open_table(&buf, NULL);

		blobmsg_add_u32(&buf, "expires", atoi(ts) - now);
		blobmsg_add_string(&buf, "macaddr", mac);
		blobmsg_add_string(&buf, "ipaddr", addr);

		if (strcmp(name, "*"))
			blobmsg_add_string(&buf, "hostname", name);

		blobmsg_close_table(&buf, d);
	}

	fclose(leases);

out:
	blobmsg_close_array(&buf, c);
	ubus_send_reply(ctx, req, buf.head);

	return 0;
}

static int rpc_vuci_network_leases6(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
	FILE *leases;
	void *c, *d;
	char line[128];
	char *ts, *mac, *addr, *name, *duid;
	time_t now = time(NULL);

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "leases");

	leases = fopen("/tmp/hosts/6relayd", "r");

	if (leases) {
		while (fgets(line, sizeof(line) - 1, leases)) {
			if (strncmp(line, "# ", 2))
				continue;

			strtok(line + 2, " \t"); /* iface */

			duid = strtok(NULL, " \t");

			strtok(NULL, " \t"); /* iaid */

			name = strtok(NULL, " \t");
			ts   = strtok(NULL, " \t");

			strtok(NULL, " \t"); /* id */
			strtok(NULL, " \t"); /* length */

			addr = strtok(NULL, " \t\n");

			if (!addr)
				continue;

			d = blobmsg_open_table(&buf, NULL);

			blobmsg_add_u32(&buf, "expires", atoi(ts) - now);
			blobmsg_add_string(&buf, "duid", duid);
			blobmsg_add_string(&buf, "ip6addr", addr);

			if (strcmp(name, "-"))
				blobmsg_add_string(&buf, "hostname", name);

			blobmsg_close_array(&buf, d);
		}

		fclose(leases);
	} else {
		leases = dnsmasq_leasefile();

		if (!leases)
			goto out;

		while (fgets(line, sizeof(line) - 1, leases)) {
			ts   = strtok(line, " \t");
			mac  = strtok(NULL, " \t");
			addr = strtok(NULL, " \t");
			name = strtok(NULL, " \t");
			duid = strtok(NULL, " \t\n");

			if (!ts || !mac || !addr || !duid)
				continue;

			if (!strchr(addr, ':'))
				continue;

			d = blobmsg_open_table(&buf, NULL);

			blobmsg_add_u32(&buf, "expires", atoi(ts) - now);
			blobmsg_add_string(&buf, "macaddr", mac);
			blobmsg_add_string(&buf, "ip6addr", addr);

			if (strcmp(name, "*"))
				blobmsg_add_string(&buf, "hostname", name);

			if (strcmp(duid, "*"))
				blobmsg_add_string(&buf, "duid", name);

			blobmsg_close_table(&buf, d);
		}

		fclose(leases);
	}

out:
	blobmsg_close_array(&buf, c);
	ubus_send_reply(ctx, req, buf.head);

	return 0;
}

static int rpc_vuci_network_ct_count(struct ubus_context *ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg)
{
	FILE *f;
	char line[128];

	blob_buf_init(&buf, 0);

	if ((f = fopen("/proc/sys/net/netfilter/nf_conntrack_count", "r")) != NULL) {
		if (fgets(line, sizeof(line) - 1, f))
			blobmsg_add_u32(&buf, "count", atoi(line));

		fclose(f);
	}

	if ((f = fopen("/proc/sys/net/netfilter/nf_conntrack_max", "r")) != NULL) {
		if (fgets(line, sizeof(line) - 1, f))
			blobmsg_add_u32(&buf, "limit", atoi(line));

		fclose(f);
	}

	ubus_send_reply(ctx, req, buf.head);

	return 0;
}

static int rpc_vuci_network_ct_table(struct ubus_context *ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg)
{
	FILE *f;
	int i;
	void *c, *d;
	char *p, line[512];
	bool seen[6];

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "entries");

	if ((f = fopen("/proc/net/nf_conntrack", "r")) != NULL) {
		while (fgets(line, sizeof(line) - 1, f)) {
			d = blobmsg_open_table(&buf, NULL);
			memset(seen, 0, sizeof(seen));

			for (i = 0, p = strtok(line, " "); p; i++, p = strtok(NULL, " ")) {
				if (i == 0)
					blobmsg_add_u8(&buf, "ipv6", !strcmp(p, "ipv6"));
				else if (i == 3)
					blobmsg_add_u32(&buf, "protocol", atoi(p));
				else if (i == 4)
					blobmsg_add_u32(&buf, "expires", atoi(p));
				else if (i >= 5) {
					if (*p == '[')
						continue;

					if (!seen[0] && !strncmp(p, "src=", 4)) {
						blobmsg_add_string(&buf, "src", p + 4);
						seen[0] = true;
					} else if (!seen[1] && !strncmp(p, "dst=", 4)) {
						blobmsg_add_string(&buf, "dest", p + 4);
						seen[1] = true;
					} else if (!seen[2] && !strncmp(p, "sport=", 6)) {
						blobmsg_add_u32(&buf, "sport", atoi(p + 6));
						seen[2] = true;
					} else if (!seen[3] && !strncmp(p, "dport=", 6)) {
						blobmsg_add_u32(&buf, "dport", atoi(p + 6));
						seen[3] = true;
					} else if (!strncmp(p, "packets=", 8)) {
						blobmsg_add_u32(&buf,
						                seen[4] ? "tx_packets" : "rx_packets",
						                atoi(p + 8));
						seen[4] = true;
					} else if (!strncmp(p, "bytes=", 6)) {
						blobmsg_add_u32(&buf,
										seen[5] ? "tx_bytes" : "rx_bytes",
						                atoi(p + 6));
						seen[5] = true;
					}
				}
			}

			blobmsg_close_table(&buf, d);
		}

		fclose(f);
	}

	blobmsg_close_array(&buf, c);
	ubus_send_reply(ctx, req, buf.head);

	return 0;
}

static int rpc_vuci_network_arp_table(struct ubus_context *ctx, struct ubus_object *obj,
                            struct ubus_request_data *req, const char *method,
                            struct blob_attr *msg)
{
	FILE *f;
	void *c, *d;
	char *addr, *mac, *dev, line[128];

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "entries");

	if ((f = fopen("/proc/net/arp", "r")) != NULL) {
		/* skip header line */
		fgets(line, sizeof(line) - 1, f);

		while (fgets(line, sizeof(line) - 1, f)) {
			addr = strtok(line, " \t");

			strtok(NULL, " \t"); /* HW type */
			strtok(NULL, " \t"); /* Flags */

			mac = strtok(NULL, " \t");

			strtok(NULL, " \t"); /* Mask */

			dev = strtok(NULL, " \t\n");

			if (!dev)
				continue;

			d = blobmsg_open_table(&buf, NULL);
			blobmsg_add_string(&buf, "ipaddr", addr);
			blobmsg_add_string(&buf, "macaddr", mac);
			blobmsg_add_string(&buf, "device", dev);
			blobmsg_close_table(&buf, d);
		}

		fclose(f);
	}

	blobmsg_close_array(&buf, c);
	ubus_send_reply(ctx, req, buf.head);

	return 0;
}

static void put_hexaddr(const char *name, const char *s, const char *m)
{
	int bits;
	struct in_addr a;
	char as[sizeof("255.255.255.255/32\0")];

	a.s_addr = strtoul(s, NULL, 16);
	inet_ntop(AF_INET, &a, as, sizeof(as));

	if (m) {
		for (a.s_addr = ntohl(strtoul(m, NULL, 16)), bits = 0;
		     a.s_addr & 0x80000000;
		     a.s_addr <<= 1)
			bits++;

		sprintf(as + strlen(as), "/%u", bits);
	}

	blobmsg_add_string(&buf, name, as);
}

static int rpc_vuci_network_routes(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
	FILE *routes;
	void *c, *d;
	char *dst, *dmask, *next, *metric, *device;
	char line[256];
	unsigned int n;

	if (!(routes = fopen("/proc/net/route", "r")))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "routes");

	/* skip header line */
	fgets(line, sizeof(line) - 1, routes);

	while (fgets(line, sizeof(line) - 1, routes)) {
		device = strtok(line, "\t ");
		dst    = strtok(NULL, "\t ");
		next   = strtok(NULL, "\t ");

		strtok(NULL, "\t "); /* flags */
		strtok(NULL, "\t "); /* refcount */
		strtok(NULL, "\t "); /* usecount */

		metric = strtok(NULL, "\t ");
		dmask  = strtok(NULL, "\t ");

		if (!dmask)
			continue;

		d = blobmsg_open_table(&buf, NULL);

		put_hexaddr("target", dst, dmask);
		put_hexaddr("nexthop", next, NULL);

		n = strtoul(metric, NULL, 10);
		blobmsg_add_u32(&buf, "metric", n);

		blobmsg_add_string(&buf, "device", device);

		blobmsg_close_table(&buf, d);
	}

	blobmsg_close_array(&buf, c);
	fclose(routes);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static void put_hex6addr(const char *name, const char *s, const char *m)
{
	int i;
	struct in6_addr a;
	char as[INET6_ADDRSTRLEN + sizeof("/128")];

#define hex(x) \
	(((x) <= '9') ? ((x) - '0') : \
		(((x) <= 'F') ? ((x) - 'A' + 10) : \
			((x) - 'a' + 10)))

	for (i = 0; i < 16; i++, s += 2)
		a.s6_addr[i] = (16 * hex(*s)) + hex(*(s+1));

	inet_ntop(AF_INET6, &a, as, sizeof(as));

	if (m)
		sprintf(as + strlen(as), "/%lu", strtoul(m, NULL, 16));

	blobmsg_add_string(&buf, name, as);
}

static int rpc_vuci_network_routes6(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
	FILE *routes;
	void *c, *d;
	char *src, *smask, *dst, *dmask, *next, *metric, *flags, *device;
	char line[256];
	unsigned int n;

	if (!(routes = fopen("/proc/net/ipv6_route", "r")))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "routes");

	while (fgets(line, sizeof(line) - 1, routes)) {
		dst    = strtok(line, " ");
		dmask  = strtok(NULL, " ");
		src    = strtok(NULL, " ");
		smask  = strtok(NULL, " ");
		next   = strtok(NULL, " ");
		metric = strtok(NULL, " ");

		strtok(NULL, " "); /* refcount */
		strtok(NULL, " "); /* usecount */

		flags  = strtok(NULL, " ");
		device = strtok(NULL, " \n");

		if (!device)
			continue;

		n = strtoul(flags, NULL, 16);

		if (!(n & 1))
			continue;

		d = blobmsg_open_table(&buf, NULL);

		put_hex6addr("target", dst, dmask);
		put_hex6addr("source", src, smask);
		put_hex6addr("nexthop", next, NULL);

		n = strtoul(metric, NULL, 16);
		blobmsg_add_u32(&buf, "metric", n);

		blobmsg_add_string(&buf, "device", device);

		blobmsg_close_table(&buf, d);
	}

	blobmsg_close_array(&buf, c);
	fclose(routes);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}


struct swconfig_state {
	bool open;
	void *array;
	bool open2;
	void *array2;
	int port;
};

static int swconfig_parse_list(struct blob_buf *blob, char *buf, int len, void *priv)
{
	char *p;
	char *nl = strchr(buf, '\n');
	struct swconfig_state *s = priv;

	if (!nl)
		return 0;

	if (!s->open) {
		s->open = true;
		s->array = blobmsg_open_array(blob, "switches");
	}

	strtok(buf, "-");
	p = strtok(NULL, " \n");

	if (p)
		blobmsg_add_string(blob, NULL, p);

	return (nl - buf + 1);
}

static int swconfig_finish_list(struct blob_buf *blob, int status, void *priv)
{
	struct swconfig_state *s = priv;

	if (!s->open)
		return UBUS_STATUS_NO_DATA;

	blobmsg_close_array(blob, s->array);

	return UBUS_STATUS_OK;
}

static int rpc_vuci_network_sw_list(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
	struct swconfig_state *state = NULL;
	const char *cmd[3] = { "swconfig", "list", NULL };

	state = malloc(sizeof(*state));

	if (!state)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(state, 0, sizeof(*state));

	return ops->exec(cmd, NULL, swconfig_parse_list, NULL, swconfig_finish_list,
	                 state, ctx, req);
}


static int swconfig_parse_help(struct blob_buf *blob, char *buf, int len, void *priv)
{
	void *c;
	char *p;
	char *nl = strchr(buf, '\n');
	struct swconfig_state *s = priv;

	if (!nl)
		return 0;

	if (!s->open) {
		s->open = true;
		s->array = blobmsg_open_table(blob, "info");
	}

	switch (*buf) {
	case ' ':
		strtok(buf, "-");
		p = strtok(NULL, "-\n");

		if (p) {
			if (s->open2)
				blobmsg_close_array(blob, s->array2);

			s->array2 = blobmsg_open_array(blob, p);
			s->open2 = true;
		}

		break;

	case '\t':
		c = blobmsg_open_table(blob, NULL);

		strtok(buf, "(");
		p = strtok(NULL, ")");

		if (p)
			blobmsg_add_string(blob, "type", p);

		p = strtok(NULL, ":( ");

		if (p)
			blobmsg_add_string(blob, "name", p);

		p = strtok(NULL, "\n");
		*(nl - 1) = 0;

		if (p)
			blobmsg_add_string(blob, "description", p + 1);

		blobmsg_close_table(blob, c);
		break;

	default:
		strtok(buf, "(");
		p = strtok(NULL, ")");

		if (p)
			blobmsg_add_string(blob, "model", p);

		strtok(NULL, ":");
		p = strtok(NULL, "(");

		if (p)
			blobmsg_add_u32(blob, "num_ports", atoi(p));

		strtok(NULL, "@");
		p = strtok(NULL, ")");

		if (p)
			blobmsg_add_u32(blob, "cpu_port", atoi(p));

		strtok(NULL, ":");
		p = strtok(NULL, "\n");

		if (p)
			blobmsg_add_u32(blob, "num_vlans", atoi(p));

		break;
	}

	return (nl - buf + 1);
}

static int swconfig_finish_help(struct blob_buf *blob, int status, void *priv)
{
	struct swconfig_state *s = priv;

	if (!s->open)
		return UBUS_STATUS_NO_DATA;

	if (s->open2)
		blobmsg_close_array(blob, s->array2);

	blobmsg_close_table(blob, s->array);

	return UBUS_STATUS_OK;
}

static int rpc_vuci_network_sw_info(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
	struct swconfig_state *state = NULL;
	struct blob_attr *tb[__RPC_SWITCH_MAX];
	const char *cmd[5] = { "swconfig", "dev", NULL, "help", NULL };

	blobmsg_parse(rpc_switch_policy, __RPC_SWITCH_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_SWITCH_NAME])
		return UBUS_STATUS_INVALID_ARGUMENT;

	state = malloc(sizeof(*state));

	if (!state)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(state, 0, sizeof(*state));

	cmd[2] = blobmsg_get_string(tb[RPC_SWITCH_NAME]);

	return ops->exec(cmd, NULL, swconfig_parse_help, NULL, swconfig_finish_help,
	                 state, ctx, req);
}


static void swconfig_parse_link(struct blob_buf *blob, char *val)
{
	char *p;

	int speed = 0;

	bool rxflow = false;
	bool txflow = false;
	bool duplex = false;
	bool aneg = false;
	bool up = false;

	for (p = strtok(val, " "); p; p = strtok(NULL, " ")) {
		if (!strncmp(p, "speed:", 6))
			speed = atoi(p + 6);
		else if (!strcmp(p, "link:up"))
			up = true;
		else if (!strcmp(p, "txflow"))
			txflow = true;
		else if (!strcmp(p, "rxflow"))
			rxflow = true;
		else if (!strcmp(p, "full-duplex"))
			duplex = true;
		else if (!strcmp(p, "auto"))
			aneg = true;
	}

	blobmsg_add_u8(blob, "link",             up);
	blobmsg_add_u8(blob, "rx_flow_control",  rxflow);
	blobmsg_add_u8(blob, "tx_flow_control",  txflow);
	blobmsg_add_u8(blob, "full_duplex",      duplex);
	blobmsg_add_u8(blob, "auto_negotiation", aneg);
	blobmsg_add_u32(blob, "speed",           speed);
}

static int swconfig_parse_stat(struct blob_buf *blob, char *buf, int len, void *priv)
{
	char *p, *v;
	char *nl = strchr(buf, '\n');
	struct swconfig_state *s = priv;

	if (!nl)
		return 0;

	if (nl == buf)
		return 1;

	if (!s->open) {
		s->open = true;
		s->array = blobmsg_open_array(blob, "ports");
	}

	p = strtok(buf, " :\t");
	if (p) {
		if (!strcmp(p, "Port")) {
			if (s->open2)
				blobmsg_close_table(blob, s->array2);

			s->array2 = blobmsg_open_table(blob, NULL);
			s->open2 = true;
		} else if (s->open2) {
			v = strtok(NULL, "\n");
			if (v) {
				if (!strcmp(p, "link"))
					swconfig_parse_link(blob, v);
			}
		}
	}

	return (nl - buf + 1);
}

static int swconfig_finish_stat(struct blob_buf *blob, int status, void *priv)
{
	struct swconfig_state *s = priv;

	if (!s->open)
		return UBUS_STATUS_NO_DATA;

	if (s->open2)
		blobmsg_close_table(blob, s->array2);

	blobmsg_close_array(blob, s->array);

	return UBUS_STATUS_OK;
}

static int rpc_vuci_network_sw_status(struct ubus_context *ctx, struct ubus_object *obj,
                            struct ubus_request_data *req, const char *method,
                            struct blob_attr *msg)
{
	struct swconfig_state *state = NULL;
	struct blob_attr *tb[__RPC_SWITCH_MAX];
	const char *cmd[5] = { "swconfig", "dev", NULL, "show", NULL };

	blobmsg_parse(rpc_switch_policy, __RPC_SWITCH_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_SWITCH_NAME])
		return UBUS_STATUS_INVALID_ARGUMENT;

	state = malloc(sizeof(*state));

	if (!state)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(state, 0, sizeof(*state));

	cmd[2] = blobmsg_get_string(tb[RPC_SWITCH_NAME]);

	return ops->exec(cmd, NULL, swconfig_parse_stat, NULL, swconfig_finish_stat,
	                 state, ctx, req);
}

enum {
	NETWORK_CMD_PING,
	NETWORK_CMD_PING6,
	NETWORK_CMD_TRACEROUTE,
	NETWORK_CMD_TRACEROUTE6,
	NETWORK_CMD_NSLOOKUP
};

static int network_cmd(struct ubus_context *ctx, struct ubus_request_data *req,
            struct blob_attr *msg, int which)
{
	char *arg;
	struct blob_attr *tb[__RPC_D_MAX];

	blobmsg_parse(rpc_data_policy, __RPC_D_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_D_DATA])
		return UBUS_STATUS_INVALID_ARGUMENT;

	arg = blobmsg_get_string(tb[RPC_D_DATA]);

	const char *cmds[][8] = {
		[NETWORK_CMD_PING] = {
			"ping", "-c", "5", "-W", "1", arg
		},
		[NETWORK_CMD_PING6] = {
			"ping6", "-c", "5", "-W", "1", arg
		},
		[NETWORK_CMD_TRACEROUTE] = {
			"traceroute", "-q", "1", "-w", "1", "-n", arg
		},
		[NETWORK_CMD_TRACEROUTE6] = {
			"traceroute6", "-q", "1", "-w", "2", "-n", arg
		},
		[NETWORK_CMD_NSLOOKUP] = {
			"nslookup", arg
		}
	};

	return ops->exec(cmds[which], NULL, NULL, NULL, NULL, NULL, ctx, req);
}

static int rpc_vuci_network_ping(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg)
{
	return network_cmd(ctx, req, msg, NETWORK_CMD_PING);
}

static int rpc_vuci_network_ping6(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
	return network_cmd(ctx, req, msg, NETWORK_CMD_PING6);
}

static int rpc_vuci_network_traceroute(struct ubus_context *ctx, struct ubus_object *obj,
                             struct ubus_request_data *req, const char *method,
                             struct blob_attr *msg)
{
	return network_cmd(ctx, req, msg, NETWORK_CMD_TRACEROUTE);
}

static int rpc_vuci_network_traceroute6(struct ubus_context *ctx, struct ubus_object *obj,
                              struct ubus_request_data *req, const char *method,
                              struct blob_attr *msg)
{
	return network_cmd(ctx, req, msg, NETWORK_CMD_TRACEROUTE6);
}

static int rpc_vuci_network_nslookup(struct ubus_context *ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg)
{
	return network_cmd(ctx, req, msg, NETWORK_CMD_NSLOOKUP);
}


static int network_ifupdown(struct ubus_context *ctx, struct ubus_request_data *req,
                 struct blob_attr *msg, bool up)
{
	const char *cmd[3] = { NULL };
	struct blob_attr *tb[__RPC_D_MAX];

	blobmsg_parse(rpc_data_policy, __RPC_D_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_D_DATA])
		return UBUS_STATUS_INVALID_ARGUMENT;

	cmd[0] = up ? "/sbin/ifup" : "/sbin/ifdown";
	cmd[1] = blobmsg_get_string(tb[RPC_D_DATA]);

	return ops->exec(cmd, NULL, NULL, NULL, NULL, NULL, ctx, req);
}

static int rpc_vuci_network_ifup(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg)
{
	return network_ifupdown(ctx, req, msg, true);
}

static int rpc_vuci_network_ifdown(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
	return network_ifupdown(ctx, req, msg, false);
}

static int rpc_vuci_network_dev_list(struct ubus_context *ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg)
{
	char path[PATH_MAX];
	struct dirent *e;
	struct stat s;
	void *c, *t;
	bool wireless, bridge, tuntap;
	int type, flags;
	DIR *d;
	FILE *f;

	if (!(d = opendir("/sys/class/net")))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "devices");

	while ((e = readdir(d)) != NULL) {
		snprintf(path, sizeof(path) - 1, "/sys/class/net/%s/type", e->d_name);

		if (stat(path, &s) || !S_ISREG(s.st_mode) || !(f = fopen(path, "r")))
			continue;

		type = 1;
		memset(path, 0, sizeof(path));

		if (fread(path, 1, sizeof(path) - 1, f) > 0)
			type = atoi(path);

		fclose(f);

		snprintf(path, sizeof(path) - 1, "/sys/class/net/%s/flags", e->d_name);

		if (stat(path, &s) || !S_ISREG(s.st_mode) || !(f = fopen(path, "r")))
			continue;

		flags = 0;
		memset(path, 0, sizeof(path));

		if (fread(path, 1, sizeof(path) - 1, f) > 0)
			flags = strtoul(path, NULL, 16);

		fclose(f);

		snprintf(path, sizeof(path) - 1,
		         "/sys/class/net/%s/wireless", e->d_name);

		wireless = (!stat(path, &s) && S_ISDIR(s.st_mode));

		snprintf(path, sizeof(path) - 1,
		         "/sys/class/net/%s/phy80211", e->d_name);

		wireless = (wireless || (!stat(path, &s) && S_ISLNK(s.st_mode)));

		snprintf(path, sizeof(path) - 1,
		         "/sys/class/net/%s/bridge", e->d_name);

		bridge = (!stat(path, &s) && S_ISDIR(s.st_mode));

		snprintf(path, sizeof(path) - 1,
		         "/sys/class/net/%s/tun_flags", e->d_name);

		tuntap = (!stat(path, &s) && S_ISREG(s.st_mode));

		t = blobmsg_open_table(&buf, NULL);

		blobmsg_add_string(&buf, "device", e->d_name);
		blobmsg_add_u32(&buf, "type", type);
		blobmsg_add_u8(&buf, "is_up", flags & 1);
		blobmsg_add_u8(&buf, "is_bridge", bridge);
		blobmsg_add_u8(&buf, "is_tuntap", tuntap);
		blobmsg_add_u8(&buf, "is_wireless", wireless);

		blobmsg_close_table(&buf, t);
	}

	blobmsg_close_array(&buf, c);

	closedir(d);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_network_eap_support(struct ubus_context *ctx, struct ubus_object *obj,
                              struct ubus_request_data *req, const char *method,
                              struct blob_attr *msg)
{
	blob_buf_init(&buf, 0);
	blobmsg_add_u8(&buf, "master", !system("/usr/sbin/hostapd -veap"));
	blobmsg_add_u8(&buf, "client", !system("/usr/sbin/wpa_supplicant -veap"));
	ubus_send_reply(ctx, req, buf.head);
	return 0;
}


struct opkg_state {
	int cur_offset;
	int cur_count;
	int req_offset;
	int req_count;
	int total;
	int missing;
	bool open[2];
	void *array[2];
	char prev_name[64];
};

static int opkg_parse_list(struct blob_buf *blob, char *buf, int len, void *priv)
{
	struct opkg_state *s = priv;

	char *ptr, *last;
	char *nl = strchr(buf, '\n');
	char *name = NULL, *vers = NULL, *desc = NULL;
	int size = 0;

	if (!nl)
		return 0;

	s->total++;

	if (s->cur_offset++ < s->req_offset)
		goto skip;

	if (s->cur_count >= s->req_count)
		goto skip;

	if (!s->open[0]) {
		s->open[0]  = true;
		s->array[0] = blobmsg_open_array(blob, "packages");
	}

	for (ptr = buf, last = buf, *nl = 0; ptr <= nl; ptr++) {
		if (!*ptr || (*ptr == ' ' && *(ptr+1) == '-' && *(ptr+2) == ' ')) {
			if (!name) {
				name = last;
				last = ptr + 3;
				*ptr = 0;
				ptr += 2;
			} else if (!vers) {
				vers = last;
				last = ptr + 3;
				*ptr = 0;
				ptr += 2;
			} else if (!size) {
				size = strtoul(last, NULL, 10);
				desc = *ptr ? (ptr + 3) : NULL;
				*ptr = 0;
				break;
			}
		}
	}

	if (name && vers) {
		/* opkg may spit out duplicate package lines, one for the installed pkg
		 * and the other for the repo entry. The repo info is usually more
		 * complete (contains size + description, therfore merge it with the
		 * previous entry)
		 */
		if (!strcmp(s->prev_name, name)) {
			if (s->missing > 1 && size > 0) {
				blobmsg_add_u32(blob, NULL, size);

				if (s->missing > 0 && desc && *desc)
					blobmsg_add_string(blob, NULL, desc);
			}

			s->missing = 0;
		} else {
			if (s->open[1])
				blobmsg_close_array(blob, s->array[1]);

			s->open[1] = true;
			s->array[1] = blobmsg_open_array(blob, NULL);
			s->missing = 2;

			blobmsg_add_string(blob, NULL, name);
			blobmsg_add_string(blob, NULL, vers);

			if (size > 0) {
				s->missing--;
				blobmsg_add_u32(blob, NULL, size);

				if (desc && *desc) {
					s->missing--;
					blobmsg_add_string(blob, NULL, desc);
				}
			}

			strncpy(s->prev_name, name, sizeof(s->prev_name) - 1);
			s->cur_count++;
		}
	}

skip:
	return (nl - buf + 1);
}

static int opkg_finish_list(struct blob_buf *blob, int status, void *priv)
{
	struct opkg_state *s = priv;

	if (!s->open[0])
		return UBUS_STATUS_NO_DATA;

	if (s->open[1])
		blobmsg_close_array(blob, s->array[1]);

	blobmsg_close_array(blob, s->array[0]);
	blobmsg_add_u32(blob, "total", s->total);

	return UBUS_STATUS_OK;
}

static int opkg_exec_list(const char *action, struct blob_attr *msg,
               struct ubus_context *ctx, struct ubus_request_data *req)
{
	struct opkg_state *state = NULL;
	struct blob_attr *tb[__RPC_OM_MAX];
	const char *cmd[6] = { "opkg", action, "-size", "-nocase", NULL, NULL };

	blobmsg_parse(rpc_opkg_match_policy, __RPC_OM_MAX, tb,
	              blob_data(msg), blob_len(msg));

	state = malloc(sizeof(*state));

	if (!state)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(state, 0, sizeof(*state));

	if (tb[RPC_OM_PATTERN])
		cmd[4] = blobmsg_data(tb[RPC_OM_PATTERN]);

	if (tb[RPC_OM_LIMIT])
		state->req_count = blobmsg_get_u32(tb[RPC_OM_LIMIT]);

	if (tb[RPC_OM_OFFSET])
		state->req_offset = blobmsg_get_u32(tb[RPC_OM_OFFSET]);

	if (state->req_offset < 0)
		state->req_offset = 0;

	if (state->req_count <= 0 || state->req_count > 100)
		state->req_count = 100;

	return ops->exec(cmd, NULL, opkg_parse_list, NULL, opkg_finish_list,
	                 state, ctx, req);
}


static int rpc_vuci_opkg_list(struct ubus_context *ctx, struct ubus_object *obj,
                    struct ubus_request_data *req, const char *method,
                    struct blob_attr *msg)
{
	return opkg_exec_list("list", msg, ctx, req);
}

static int rpc_vuci_opkg_list_installed(struct ubus_context *ctx, struct ubus_object *obj,
                              struct ubus_request_data *req, const char *method,
                              struct blob_attr *msg)
{
	return opkg_exec_list("list-installed", msg, ctx, req);
}

static int rpc_vuci_opkg_find(struct ubus_context *ctx, struct ubus_object *obj,
                    struct ubus_request_data *req, const char *method,
                    struct blob_attr *msg)
{
	return opkg_exec_list("find", msg, ctx, req);
}

static int rpc_vuci_opkg_update(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	const char *cmd[3] = { "opkg", "update", NULL };
	return ops->exec(cmd, NULL, NULL, NULL, NULL, NULL, ctx, req);
}

static int rpc_vuci_opkg_install(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg)
{
	struct blob_attr *tb[__RPC_OP_MAX];
	const char *cmd[5] = { "opkg", "--force-overwrite",
	                       "install", NULL, NULL };

	blobmsg_parse(rpc_opkg_package_policy, __RPC_OP_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_OP_PACKAGE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	cmd[3] = blobmsg_data(tb[RPC_OP_PACKAGE]);

	return ops->exec(cmd, NULL, NULL, NULL, NULL, NULL, ctx, req);
}

static int rpc_vuci_opkg_remove(struct ubus_context *ctx, struct ubus_object *obj,
                      struct ubus_request_data *req, const char *method,
                      struct blob_attr *msg)
{
	struct blob_attr *tb[__RPC_OP_MAX];
	const char *cmd[5] = { "opkg", "--force-removal-of-dependent-packages",
	                       "remove", NULL, NULL };

	blobmsg_parse(rpc_opkg_package_policy, __RPC_OP_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_OP_PACKAGE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	cmd[3] = blobmsg_data(tb[RPC_OP_PACKAGE]);

	return ops->exec(cmd, NULL, NULL, NULL, NULL, NULL, ctx, req);
}


static int opkg_parse_info(struct blob_buf *blob, char *buf, int len, void *priv)
{
	struct blob_attr *cur;
	char *ptr, *key, *val;
	char *nl = strchr(buf, '\n');
	void *c;
	int n, rem;

	if (!nl)
		return 0;

	key = strtok(buf, ":\n");
	val = strtok(NULL, "\n");

	if (key && val) {
		while (isspace(*val))
			val++;

		/* convert field name to lowercase and replace dashes with underscore */
		for (ptr = key; *ptr; ptr++) {
			if (*ptr >= 65 && *ptr <= 90)
				*ptr += 32;
			else if (*ptr == '-')
				*ptr = '_';
		}

		/* opkg may repeat the info multiple times for different package sources
		 * therfore skip any fields we already have */
		blobmsg_for_each_attr(cur, blob->head, rem)
			if (!strcmp(blobmsg_name(cur), key))
				goto skip;

		if (!strcmp(key, "depends")) {
			c = blobmsg_open_array(blob, key);

			for (ptr = strtok(val, ","); ptr; ptr = strtok(NULL, ","))
			{
				while (isspace(*ptr))
					ptr++;

				blobmsg_add_string(blob, NULL, ptr);
			}

			blobmsg_close_array(blob, c);
		} else if (!strcmp(key, "status")) {
			c = blobmsg_open_array(blob, key);

			for (ptr = strtok(val, " "); ptr; ptr = strtok(NULL, " "))
				blobmsg_add_string(blob, NULL, ptr);

			blobmsg_close_array(blob, c);
		} else if (!strcmp(key, "size") || !strcmp(key, "installed_time")) {
			n = strtoul(val, &ptr, 10);

			if (ptr > val && *ptr == 0)
				blobmsg_add_u32(blob, key, n);
		} else {
			blobmsg_add_string(blob, key, val);
		}
	}

skip:
	return (nl - buf + 1);
}

static int opkg_finish_info(struct blob_buf *blob, int status, void *priv)
{
	return blob_len(blob->head) ? UBUS_STATUS_OK : UBUS_STATUS_NOT_FOUND;
}

static int rpc_vuci_opkg_info(struct ubus_context *ctx, struct ubus_object *obj,
                    struct ubus_request_data *req, const char *method,
                    struct blob_attr *msg)
{
	struct blob_attr *tb[__RPC_OP_MAX];
	const char *cmd[4] = { "opkg", "info", NULL, NULL };

	blobmsg_parse(rpc_opkg_package_policy, __RPC_OP_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_OP_PACKAGE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	cmd[2] = blobmsg_data(tb[RPC_OP_PACKAGE]);

	return ops->exec(cmd, NULL, opkg_parse_info, NULL, opkg_finish_info, NULL,
	                 ctx, req);
}

static int rpc_vuci_opkg_config_get(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
	FILE *f;
	char conf[2048] = { 0 };

	if (!(f = fopen("/etc/opkg.conf", "r")))
		return rpc_errno_status();

	fread(conf, sizeof(conf) - 1, 1, f);
	fclose(f);

	blob_buf_init(&buf, 0);
	blobmsg_add_string(&buf, "config", conf);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_opkg_config_set(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
	FILE *f;
	struct blob_attr *tb[__RPC_D_MAX];

	blobmsg_parse(rpc_data_policy, __RPC_D_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_D_DATA])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (blobmsg_data_len(tb[RPC_D_DATA]) >= 2048)
		return UBUS_STATUS_NOT_SUPPORTED;

	if (!(f = fopen("/etc/opkg.conf", "w")))
		return rpc_errno_status();

	fwrite(blobmsg_data(tb[RPC_D_DATA]),
	       blobmsg_data_len(tb[RPC_D_DATA]) - 1, 1, f);

	fclose(f);
	return 0;
}


static bool menu_access(struct blob_attr *sid, struct blob_attr *acls, struct blob_buf *e)
{
	int rem;
	struct blob_attr *acl;
	bool rv = true;
	void *c;

	c = blobmsg_open_table(e, "write");

	blobmsg_for_each_attr(acl, acls, rem)
	{
		if (!ops->session_access(blobmsg_data(sid), "access-group",
		                         blobmsg_data(acl), "read"))
		{
			rv = false;
			break;
		}

		blobmsg_add_u8(e, blobmsg_data(acl),
		               ops->session_access(blobmsg_data(sid), "access-group",
		                                   blobmsg_data(acl), "write"));
	}

	blobmsg_close_table(e, c);

	return rv;
}

static bool menu_files(struct blob_attr *files)
{
	int rem;
	bool empty = true;
	struct stat s;
	struct blob_attr *file;

	blobmsg_for_each_attr(file, files, rem)
	{
		empty = false;

		if (blobmsg_type(file) != BLOBMSG_TYPE_STRING)
			continue;

		if (stat(blobmsg_get_string(file), &s) || !S_ISREG(s.st_mode))
			continue;

		return true;
	}

	return empty;
}

static int rpc_vuci_ui_menu(struct ubus_context *ctx, struct ubus_object *obj,
                  struct ubus_request_data *req, const char *method,
                  struct blob_attr *msg)
{
	int i, rem, rem2;
	glob_t gl;
	struct blob_buf menu = { 0 };
	struct blob_buf item = { 0 };
	struct blob_attr *entry, *attr;
	struct blob_attr *tb[__RPC_MENU_MAX];
	bool access, files;
	void *c, *d;

	blobmsg_parse(rpc_menu_policy, __RPC_MENU_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_MENU_SESSION])
		return UBUS_STATUS_INVALID_ARGUMENT;


	blob_buf_init(&buf, 0);
	c = blobmsg_open_table(&buf, "menu");

	if (!glob(RPC_VUCI_MENU_FILES, 0, NULL, &gl)) {
		for (i = 0; i < gl.gl_pathc; i++) {
			blob_buf_init(&menu, 0);

			if (!blobmsg_add_json_from_file(&menu, gl.gl_pathv[i]))
				goto skip;

			blob_for_each_attr(entry, menu.head, rem) {
				access = files = true;

				blob_buf_init(&item, 0);
				d = blobmsg_open_table(&item, blobmsg_name(entry));

				blobmsg_for_each_attr(attr, entry, rem2) {
					if (blob_id(attr) == BLOBMSG_TYPE_ARRAY &&
					    !strcmp(blobmsg_name(attr), "acls"))
						access = menu_access(tb[RPC_MENU_SESSION], attr, &item);
					else if (blob_id(attr) == BLOBMSG_TYPE_ARRAY &&
					         !strcmp(blobmsg_name(attr), "files"))
						files = menu_files(attr);
					else
						blobmsg_add_blob(&item, attr);
				}

				blobmsg_close_table(&item, d);

				if (access && files)
					blob_for_each_attr(attr, item.head, rem2)
						blobmsg_add_blob(&buf, attr);

				blob_buf_free(&item);
			}

skip:
			blob_buf_free(&menu);
		}

		globfree(&gl);
	}

	blobmsg_close_table(&buf, c);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}


static void parse_acl_file(struct blob_buf *acls, const char *path)
{
	struct blob_buf acl = { 0 };
	struct blob_attr *cur;
	void *c;
	int rem;

	blob_buf_init(&acl, 0);

	if (blobmsg_add_json_from_file(&acl, path))
	{
		c = blobmsg_open_table(acls, NULL);

		blob_for_each_attr(cur, acl.head, rem)
			blobmsg_add_blob(acls, cur);

		blobmsg_close_table(acls, c);
	}

	blob_buf_free(&acl);
}

static int rpc_vuci_ui_acls(struct ubus_context *ctx, struct ubus_object *obj,
                  struct ubus_request_data *req, const char *method,
                  struct blob_attr *msg)
{
	int i;
	void *c;
	glob_t gl;

	if (glob(RPC_SESSION_ACL_DIR "/*.json", 0, NULL, &gl))
		return rpc_errno_status();

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "acls");

	for (i = 0; i < gl.gl_pathc; i++)
		parse_acl_file(&buf, gl.gl_pathv[i]);

	globfree(&gl);
	blobmsg_close_array(&buf, c);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}

static int rpc_vuci_ui_crypt(struct ubus_context *ctx, struct ubus_object *obj,
                   struct ubus_request_data *req, const char *method,
                   struct blob_attr *msg)
{
	char *hash;
	struct blob_attr *tb[__RPC_D_MAX];

	blobmsg_parse(rpc_data_policy, __RPC_D_MAX, tb,
	              blob_data(msg), blob_len(msg));

	if (!tb[RPC_D_DATA] || blobmsg_data_len(tb[RPC_D_DATA]) >= 128)
		return UBUS_STATUS_INVALID_ARGUMENT;

	hash = crypt(blobmsg_get_string(tb[RPC_D_DATA]), "$1$");

	blob_buf_init(&buf, 0);
	blobmsg_add_string(&buf, "crypt", hash);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}


static int rpc_vuci_api_init(const struct rpc_daemon_ops *o, struct ubus_context *ctx)
{
	int rv = 0;

	static const struct ubus_method vuci_system_methods[] = {
		UBUS_METHOD_NOARG("syslog",       rpc_vuci_system_log),
		UBUS_METHOD_NOARG("dmesg",        rpc_vuci_system_dmesg),
		UBUS_METHOD_NOARG("diskfree",     rpc_vuci_system_diskfree),
		UBUS_METHOD_NOARG("process_list", rpc_vuci_process_list),

		UBUS_METHOD_NOARG("init_list",    rpc_vuci_init_list),
		UBUS_METHOD("init_action",        rpc_vuci_init_action,
		                                  rpc_init_policy),
		UBUS_METHOD_NOARG("rclocal_get",  rpc_vuci_rclocal_get),
		UBUS_METHOD("rclocal_set",        rpc_vuci_rclocal_set,
		                                  rpc_data_policy),
		UBUS_METHOD_NOARG("crontab_get",  rpc_vuci_crontab_get),
		UBUS_METHOD("crontab_set",        rpc_vuci_crontab_set,
		                                  rpc_data_policy),
		UBUS_METHOD_NOARG("sshkeys_get",  rpc_vuci_sshkeys_get),
		UBUS_METHOD("sshkeys_set",        rpc_vuci_sshkeys_set,
		                                  rpc_sshkey_policy),
		UBUS_METHOD("password_set",       rpc_vuci_password_set,
		                                  rpc_password_policy),
		UBUS_METHOD_NOARG("led_list",     rpc_vuci_led_list),
		UBUS_METHOD_NOARG("usb_list",     rpc_vuci_usb_list),
		UBUS_METHOD_NOARG("upgrade_test", rpc_vuci_upgrade_test),
		UBUS_METHOD("upgrade_start",      rpc_vuci_upgrade_start,
		                                  rpc_upgrade_policy),
		UBUS_METHOD_NOARG("upgrade_clean", rpc_vuci_upgrade_clean),
		UBUS_METHOD_NOARG("backup_restore", rpc_vuci_backup_restore),
		UBUS_METHOD_NOARG("backup_clean", rpc_vuci_backup_clean),
		UBUS_METHOD_NOARG("backup_config_get", rpc_vuci_backup_config_get),
		UBUS_METHOD("backup_config_set",  rpc_vuci_backup_config_set,
		                                  rpc_data_policy),
		UBUS_METHOD_NOARG("backup_list",  rpc_vuci_backup_list),
		UBUS_METHOD_NOARG("reset_test",   rpc_vuci_reset_test),
		UBUS_METHOD_NOARG("reset_start",  rpc_vuci_reset_start),
		UBUS_METHOD_NOARG("reboot",       rpc_vuci_reboot)
	};

	static struct ubus_object_type vuci_system_type =
		UBUS_OBJECT_TYPE("luci-rpc-vuci-system", vuci_system_methods);

	static struct ubus_object system_obj = {
		.name = "vuci.system",
		.type = &vuci_system_type,
		.methods = vuci_system_methods,
		.n_methods = ARRAY_SIZE(vuci_system_methods),
	};


	static const struct ubus_method vuci_network_methods[] = {
		UBUS_METHOD_NOARG("conntrack_count", rpc_vuci_network_ct_count),
		UBUS_METHOD_NOARG("conntrack_table", rpc_vuci_network_ct_table),
		UBUS_METHOD_NOARG("arp_table",       rpc_vuci_network_arp_table),
		UBUS_METHOD_NOARG("dhcp_leases",     rpc_vuci_network_leases),
		UBUS_METHOD_NOARG("dhcp6_leases",    rpc_vuci_network_leases6),
		UBUS_METHOD_NOARG("routes",          rpc_vuci_network_routes),
		UBUS_METHOD_NOARG("routes6",         rpc_vuci_network_routes6),
		UBUS_METHOD_NOARG("switch_list",     rpc_vuci_network_sw_list),
		UBUS_METHOD("switch_info",           rpc_vuci_network_sw_info,
		                                     rpc_switch_policy),
		UBUS_METHOD("switch_status",         rpc_vuci_network_sw_status,
		                                     rpc_switch_policy),
		UBUS_METHOD("ping",                  rpc_vuci_network_ping,
		                                     rpc_data_policy),
		UBUS_METHOD("ping6",                 rpc_vuci_network_ping6,
		                                     rpc_data_policy),
		UBUS_METHOD("traceroute",            rpc_vuci_network_traceroute,
		                                     rpc_data_policy),
		UBUS_METHOD("traceroute6",           rpc_vuci_network_traceroute6,
		                                     rpc_data_policy),
		UBUS_METHOD("nslookup",              rpc_vuci_network_nslookup,
		                                     rpc_data_policy),
		UBUS_METHOD("ifup",                  rpc_vuci_network_ifup,
		                                     rpc_data_policy),
		UBUS_METHOD("ifdown",                rpc_vuci_network_ifdown,
		                                     rpc_data_policy),
		UBUS_METHOD_NOARG("device_list",     rpc_vuci_network_dev_list),
		UBUS_METHOD_NOARG("eap_support",     rpc_vuci_network_eap_support)
	};

	static struct ubus_object_type vuci_network_type =
		UBUS_OBJECT_TYPE("luci-rpc-vuci-network", vuci_network_methods);

	static struct ubus_object network_obj = {
		.name = "vuci.network",
		.type = &vuci_network_type,
		.methods = vuci_network_methods,
		.n_methods = ARRAY_SIZE(vuci_network_methods),
	};


	static const struct ubus_method vuci_opkg_methods[] = {
		UBUS_METHOD("list",                  rpc_vuci_opkg_list,
		                                     rpc_opkg_match_policy),
		UBUS_METHOD("list_installed",        rpc_vuci_opkg_list_installed,
		                                     rpc_opkg_match_policy),
		UBUS_METHOD("find",                  rpc_vuci_opkg_find,
		                                     rpc_opkg_match_policy),
		UBUS_METHOD("install",               rpc_vuci_opkg_install,
		                                     rpc_opkg_package_policy),
		UBUS_METHOD("remove",                rpc_vuci_opkg_remove,
		                                     rpc_opkg_package_policy),
		UBUS_METHOD("info",                  rpc_vuci_opkg_info,
		                                     rpc_opkg_package_policy),
		UBUS_METHOD_NOARG("update",          rpc_vuci_opkg_update),
		UBUS_METHOD_NOARG("config_get",      rpc_vuci_opkg_config_get),
		UBUS_METHOD("config_set",            rpc_vuci_opkg_config_set,
		                                     rpc_data_policy)
	};

	static struct ubus_object_type vuci_opkg_type =
		UBUS_OBJECT_TYPE("luci-rpc-vuci-network", vuci_opkg_methods);

	static struct ubus_object opkg_obj = {
		.name = "vuci.opkg",
		.type = &vuci_opkg_type,
		.methods = vuci_opkg_methods,
		.n_methods = ARRAY_SIZE(vuci_opkg_methods),
	};


	static const struct ubus_method vuci_ui_methods[] = {
		UBUS_METHOD_NOARG("menu",            rpc_vuci_ui_menu),
		UBUS_METHOD_NOARG("acls",            rpc_vuci_ui_acls),
		UBUS_METHOD("crypt",                 rpc_vuci_ui_crypt,
		                                     rpc_data_policy)
	};

	static struct ubus_object_type vuci_ui_type =
		UBUS_OBJECT_TYPE("luci-rpc-vuci-ui", vuci_ui_methods);

	static struct ubus_object ui_obj = {
		.name = "vuci.ui",
		.type = &vuci_ui_type,
		.methods = vuci_ui_methods,
		.n_methods = ARRAY_SIZE(vuci_ui_methods),
	};

	cursor = uci_alloc_context();

	if (!cursor)
		return UBUS_STATUS_UNKNOWN_ERROR;

	ops = o;

	rv |= ubus_add_object(ctx, &system_obj);
	rv |= ubus_add_object(ctx, &network_obj);
	rv |= ubus_add_object(ctx, &opkg_obj);
	rv |= ubus_add_object(ctx, &ui_obj);

	return rv;
}

struct rpc_plugin rpc_plugin = {
	.init = rpc_vuci_api_init
};
