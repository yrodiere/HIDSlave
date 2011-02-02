#ifndef __L2CAP_H__
#define __L2CAP_H__

int l2cap_listen(const bdaddr_t *bdaddr, unsigned short psm, int lm, int backlog);
int l2cap_connect(bdaddr_t *src, bdaddr_t *dst, unsigned short psm);
int l2cap_accept(int sk, bdaddr_t *bdaddr);

#endif __L2CAP_H__
