package net.hidroid.test.core;

import java.util.HashMap;
import java.util.Map;

public class HIDASCIITranslator {

	Map<Byte, Byte> translateMap = new HashMap<Byte, Byte>();

	public HIDASCIITranslator() {
		super();
		translateMap.put(new Byte((byte) 'a'), new Byte((byte) 0x04));
		translateMap.put(new Byte((byte) 'b'), new Byte((byte) 0x05));
		translateMap.put(new Byte((byte) 'c'), new Byte((byte) 0x06));
		translateMap.put(new Byte((byte) 'd'), new Byte((byte) 0x07));
		translateMap.put(new Byte((byte) 'e'), new Byte((byte) 0x08));
		translateMap.put(new Byte((byte) 'f'), new Byte((byte) 0x09));
		translateMap.put(new Byte((byte) 'g'), new Byte((byte) 0x0A));
		translateMap.put(new Byte((byte) 'h'), new Byte((byte) 0x0B));

	}

	public byte Translate(Byte car) {
		Byte result = translateMap.get(car);
		if (result == null) {
			return 0;
		} else {
			return result.byteValue();
		}
		// TODO remplacer par une recherche pour renvoyer un code d'erreur.
	}

}
