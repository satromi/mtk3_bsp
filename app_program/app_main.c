/*
 *	app_main.c — Application main
 *
 *	USB HID ドライバは knl_start_device() で初期化済み
 *	キーボードフレームワークが有効ならスキャナータスクを起動する
 */

#include <sys/machine.h>
#include <tk/tkernel.h>
#include <tm/tmonitor.h>

extern void kb_start(void);

EXPORT INT usermain(void)
{
	tm_printf((UB *)"Starting keyboard framework\n");
	kb_start();

	tk_slp_tsk(TMO_FEVR);
	return 0;
}
