package net.hidroid.l2cap;

import java.io.IOException;
import java.io.InputStream;

public class L2capInputStream extends InputStream {

	private NativeSocket nativeSocket;

	public L2capInputStream(NativeSocket nativeSocket) {
		super();
		this.nativeSocket = nativeSocket;
	}

	@Override
	public int read(byte[] buffer, int offset, int count) throws IOException {
		int nRead = 0;
		
		try {
			nativeSocket.lock();
			nRead = nativeRead(buffer, offset, count);
		} catch (InterruptedException e) {
			throw new IOException(e.getMessage());
		} finally {
			nativeSocket.release();
		}
		
		return nRead; 
	}

	@Override
	public int read() throws IOException {
		byte buffer[] = new byte[1];
		read(buffer,0,1);
		return buffer[0];
	}

	@Override
	public int read(byte[] buffer) throws IOException {
		return read(buffer, 0, buffer.length);
	}

	private native int nativeRead(byte[] buffer, int offset, int count)
			throws IOException;

	static {
		System.loadLibrary("hidroid");
	}
}
