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
public class L2capSocket implements Closeable {
	private NativeSocket nativeSocket = new NativeSocket();
	private BluetoothDevice remoteDevice = null;
	private InputStream inputStream = new L2capInputStream(nativeSocket);
	private OutputStream outputStream = new L2capOutputStream(nativeSocket);

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
	public void connect(BluetoothDevice remoteDevice, int timeout)
			throws IOException {
		try {
			nativeSocket.lock();
			if (nativeSocket.get() > 0)
				nativeClose();
			getNativeSocket();
			nativeConnect(remoteDevice.getAddress(), timeout);
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
			throw new IOException(
					"No input stream available (This socket is not connected)");
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
			throw new IOException(
					"No output stream available (This socket is not connected)");
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
	 * Immediately close this socket, and release all associated resources.
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
		} catch (InterruptedException e) {
			throw new IOException(e.getMessage());
		} finally {
			nativeSocket.release();
		}
	}

	private native void getNativeSocket() throws IOException;

	private native void nativeConnect(String address, int timeout)
			throws IOException;

	private native void nativeClose() throws IOException;

	static {
		System.loadLibrary("hidroid");
	}
}
