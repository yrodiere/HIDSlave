/**
 * 
 */
package net.hidroid.l2cap;

import java.io.IOException;
import java.io.OutputStream;

/**
 * @author fenrhil
 * 
 */
class L2capOutputStream extends OutputStream {
	private NativeSocket nativeSocket;

	public L2capOutputStream(NativeSocket nativeSocket) {
		super();
		this.nativeSocket = nativeSocket;
	}

	@Override
	public void write(byte[] buffer, int offset, int count) throws IOException {
		try {
			nativeSocket.lock();
			nativeWrite(buffer, offset, count);
		} catch (InterruptedException e) {
			throw new IOException(e.getMessage());
		} finally {
			nativeSocket.release();
		}
	}

	@Override
	public void write(int oneByte) throws IOException {
		byte buffer[] = new byte[1];
		buffer[0] = Integer.valueOf(oneByte).byteValue();
		write(buffer, 0, 1);
	}

	@Override
	public void write(byte[] buffer) throws IOException {
		write(buffer, 0, buffer.length);
	}

	private native int nativeWrite(byte[] buffer, int offset, int count)
			throws IOException;

	static {
		System.loadLibrary("hidroid");
	}

}
