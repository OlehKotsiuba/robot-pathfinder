#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
	struct shortData {
		int l;
		int h;
	};
	
	union Payload{
		byte bytes[4];
		shortData words;
		long dWord;
	};
	
	enum Type: byte {
		MOVE_ANALOG,
		MOVE_FORWARD,
		MOVE_BACKWARD,
		TURN_LEFT,
		TURN_RIGHT,
		STOP,
		PRINT,
		SPEED,
		ENCODER_DATA,
		LOCATOR_DATA,
		SET_MODE,
		SET_HEADING
	};
	
	Type type;
	Payload payload;
};

#endif
