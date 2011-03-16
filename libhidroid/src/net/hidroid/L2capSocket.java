/**
 * 
 */
package net.hidroid;

/**
 * @author fenrhil
 *
 */
public class L2capSocket {

	public native String test();

	static {
		System.loadLibrary("hidroid");
	}
}
