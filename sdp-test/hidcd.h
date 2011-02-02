/*
 *  xkbd-bthid
 *
 *  Collin R. Mulliner <collin@betaversion.net>
 *
 *  http://www.mulliner.org/bluetooth/
 *
 *  License: GPL
 *
 */

int bthid(int ip);
int bthid_open();
int bthid_close();
int bthid_send(int key, int updown);
int bthid_process_keypress(button *active_but, int updown);
