package net.hidroid.test.core;

public class HIDASCIITranslator {

	public HIDASCIITranslator() {
		super();
	}

	public byte Translate(Byte car) {
		if ('a' <= car && car <= 'z') {
			return (byte) (car - 'a' + 0x04);
		} else if ('A' <= car && car <= 'Z') {
			return (byte) (car - 'A' + 0x04);
		} else {
			// TODO renvoyer un code d'erreur.
			return 0;
		}
	}
}
