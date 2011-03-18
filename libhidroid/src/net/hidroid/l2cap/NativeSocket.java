package net.hidroid.l2cap;

import java.util.concurrent.Semaphore;

class NativeSocket {
	private Semaphore mutex = new Semaphore(1);
	private int socket = -1;

	void lock() throws InterruptedException {
		mutex.acquire();
	}

	int get() {
		return socket;
	}

	void set(int newValue) {
		socket = newValue;
	}

	void release() {
		mutex.release();
	}

}
