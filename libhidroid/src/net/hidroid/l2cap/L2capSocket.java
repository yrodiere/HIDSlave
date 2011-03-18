/**
 * TODO : license
 */
package net.hidroid.l2cap;

import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.bluetooth.BluetoothDevice;

/**
 * @author fenrhil
 * @todo Allow to choose between datagrams, streams, etc.
 */
public abstract class L2capSocket implements Closeable {
	protected enum Type {
		STREAM, DATAGRAM, SEQ_PAQUET
	};

	private BluetoothDevice remoteDevice = null;
	private int remotePort = -1;
	protected NativeSocket nativeSocket = new NativeSocket();
	protected InputStream inputStream = null;
	protected OutputStream outputStream = null;

	/**
	 * Attempt to connect to a remote device.
	 * 
	 * This method will block until a connection is made, the timeout is reached
	 * or the connection fails. If this method returns without an exception then
	 * this socket is now connected.
	 * 
	 * TODO Handle the 'timeout' parameter
	 * 
	 * @param remoteDevice
	 *            the remote device to connect to.
	 * @param timeout
	 *            the timeout value in milliseconds or 0 for an infinite
	 *            timeout. Not handled yet.
	 * @throws IOException
	 */
	public void connect(BluetoothDevice remoteDevice, int remotePort,
			int timeout) throws IOException {
		try {
			nativeSocket.lock();

			if (nativeSocket.get() > 0) {
				nativeClose();
				this.remoteDevice = null;
				this.remotePort = -1;
				inputStream = null;
				outputStream = null;
			}

			getNativeSocket();

			nativeConnect(remoteDevice.getAddress(), remotePort, timeout);
			this.remoteDevice = remoteDevice;
			this.remotePort = remotePort;
		} catch (InterruptedException e) {
			throw new IOException(e.getMessage());
		} finally {
			nativeSocket.release();
		}
	}

	/**
	 * Get the input stream associated with this socket.
	 * 
	 * The input stream will be returned even if the socket is not yet
	 * connected, but operations on that stream will throw IOException until the
	 * associated socket is connected.
	 * 
	 * @return InputStream
	 * @throws IOException
	 */
	public InputStream getInputStream() throws IOException {
		if (inputStream == null) {
			throw new IOException("No input stream available");
		} else {
			return inputStream;
		}
	}

	/**
	 * Get the output stream associated with this socket.
	 * 
	 * The output stream will be returned even if the socket is not yet
	 * connected, but operations on that stream will throw IOException until the
	 * associated socket is connected.
	 * 
	 * @return OutputStream
	 * @throws IOException
	 */
	public OutputStream getOutputStream() throws IOException {
		if (outputStream == null) {
			throw new IOException("No output stream available");
		} else {
			return outputStream;
		}
	}

	/**
	 * Get the remote device associated with this socket.
	 * 
	 * @return The remote device, or null if there's none.
	 */
	public BluetoothDevice getRemoteDevice() {
		return remoteDevice;
	}

	/**
	 * Get the remote port associated with this socket.
	 * 
	 * @return The remote port, or -1 if there's none.
	 */
	public int getRemotePort() {
		return remotePort;
	}

	/**
	 * Immediately close this socket, and release all associated resources,
	 * including input and output streams.
	 * 
	 * Causes blocked calls on this socket in other threads to immediately throw
	 * an IOException.
	 * 
	 * @throws IOException
	 */
	@Override
	public void close() throws IOException {
		try {
			nativeSocket.lock();
			nativeClose();
			remoteDevice = null;
			remotePort = -1;
			inputStream = null;
			outputStream = null;
		} catch (InterruptedException e) {
			throw new IOException(e.getMessage());
		} finally {
			nativeSocket.release();
		}
	}

	protected abstract Type getSocketType();

	private native void getNativeSocket() throws IOException;

	private native void nativeConnect(String remoteAddress, int remotePort,
			int timeout) throws IOException;

	private native void nativeClose() throws IOException;

	static {
		System.loadLibrary("hidroid");
	}
}
