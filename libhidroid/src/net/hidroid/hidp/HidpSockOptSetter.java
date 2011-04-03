package net.hidroid.hidp;

import net.hidroid.l2cap.L2capSocket;

public class HidpSockOptSetter implements L2capSocket.SockOptSetter {
	@Override
	public native void setSockOpt(int CSocket);
	
	static {
		System.loadLibrary("hidroid");
	}
}
