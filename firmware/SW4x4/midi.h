/*!
	@brief	MIDI parsing library header

	@file	midi.h
	@author	Satoshi "Chuck" Takagi
	@date	2009/04/03
 */


#ifndef MIDI_H
#define MIDI_H

#include "config/midi_conf.h"

#define MIDI_BAUD_RATE 31250

/*! MIDI ChannelVoice message */
#define MIDI_NOTE_OFF			0x80
#define MIDI_NOTE_ON			0x90
#define MIDI_POLY_PRESSURE		0xA0
#define MIDI_CONTROL_CHANGE		0xB0
#define MIDI_CC					0xB0
#define MIDI_PROGRAM_CHANGE		0xC0
#define MIDI_PC					0xC0
#define MIDI_CHANNEL_PRESSURE	0xD0
#define MIDI_PITCH_BEND			0xE0
#define MIDI_PBEND				0xE0

/*! MIDI System message */

#define MIDI_ALL_NOTE_OFF             0x7B
#define MIDI_RESET_ALL_CONTROLLERS    0x79

#define MIDI_CC_BANK_SELECT           0x00
#define MIDI_CC_MODULATION_DEPTH      0x01
#define MIDI_CC_BREATH_CONTROL        0x02
#define MIDI_CC_FOOT_CONTROL          0x04
#define MIDI_CC_PORTAMENTO_TIME       0x05
#define MIDI_CC_DATA_ENTRY            0x06
#define MIDI_CC_VOLUME                0x07
#define MIDI_CC_BALANCE               0x08
#define MIDI_CC_PANPOT                0x0A
#define MIDI_CC_EXPRESSION            0x0B

#define MIDI_CC_BANK_SELECT_LSB       0x20
#define MIDI_CC_MODULATION_DEPTH_LSB  0x21
#define MIDI_CC_BREATH_CONTROL_LSB    0x22
#define MIDI_CC_FOOT_CONTROL_LSB      0x24
#define MIDI_CC_PORTAMENTO_TIME_LSB   0x25
#define MIDI_CC_DATA_ENTRY_LSB        0x26
#define MIDI_CC_VOLUME_LSB            0x27
#define MIDI_CC_BALANCE_LSB           0x28
#define MIDI_CC_PANPOT_LSB            0x2A
#define MIDI_CC_EXPRESSION_LSB        0x2B

#define MIDI_CC_HOLD1                 0x40
#define MIDI_CC_PORTAMENTO            0x41
#define MIDI_CC_SOSTENUTO             0x42
#define MIDI_CC_SOFT_PEDAL            0x43
#define MIDI_CC_LEGATO                0x44
#define MIDI_CC_HOLD2                 0x45
#define MIDI_CC_PATCH_SELECT          0x46



//! MIDI_SystemMessage構造体
struct MIDI_SystemMessage {
	//! Status
	/*! 記録されているシステムメッセージの種類を指し示すタグ */
	uint16_t Status;
	//! union
	/*! 引数を伴うシステムメッセージの内容を記録する共用体 */
	union {
		uint8_t Timecode;				//!< タイムコード
		uint16_t SongPositionPointer;	//!< ソングポジション
		uint8_t SongSelect;				//!< ソングセレクト
	};
};


//! MIDI_ChannelMessage構造体
/*! */
struct MIDI_ChannelMessage {
	//! Channel
	/*! このチャンネルメッセージのチャンネルを指し示すタグ */
	uint8_t Channel;
	//! Status
	/*! 記録されているチャンネルメッセージの種類を指し示すタグ */
	uint8_t Status;
	//! チャンネルメッセージの内容をあらわす共用体
	/*! */
	union {
		//! Note On
		struct {
			uint8_t Note;		//!< Note On Note
			uint8_t Velocity;	//!< Note On Velocity
		} NoteOn;
		//! Note Off
		struct {
			uint8_t Note;
			uint8_t Velocity;
		} NoteOff;
		//! Polyphonic Pressure
		struct {
			uint8_t Note;
			uint8_t Value;
		} PolyPressure;
		//! Control Change
		struct {
			uint8_t Control;
			uint8_t Value;
		} ControlChange;
		//! Program Change
		struct {
			uint8_t Value;
		} ProgramChange;
		//! Channel Pressure
		struct {
			uint8_t Value;
		} ChannelPressure;
		//! Pitch Bend
		struct {
			uint16_t Value;
		} PitchBend;
		//! Mode Change
		struct {
			uint8_t Mode;
			uint8_t Value;
		} ModeChange;
	};
};


//! MIDI_ChannelMessage構造体用 Statusフラグ
#define fMidiNoteOff			(0x01)
#define fMidiNoteOn				(0x02)
#define fMidiPolyPressure		(0x04)
#define fMidiControlChange		(0x08)
#define fMidiProgramChange		(0x10)
#define fMidiChannelPressure	(0x20)
#define fMidiPitchBend			(0x40)
#define fMidiModeChange			(0x80)

//! MIDI_SystemMessage構造体用 Statusフラグ
#define fMidiStartOfExclusive		(0x0001)
#define fMidiQuaterFrame			(0x0002)
#define fMidiSongPositionPointer	(0x0004)
#define fMidiSongSelect				(0x0008)
// undefined						(0x0010)
// undefined						(0x0020)
#define fMidiTuneRequest			(0x0040)
#define fMidiEndOfExclusive			(0x0080)
#define fMidiTimingClock			(0x0100)
// undefined						(0x0200)
#define fMidiStart					(0x0400)
#define fMidiContinue				(0x0800)
#define fMidiStop					(0x1000)
// undefined						(0x2000)
#define fMidiActiveSensing			(0x4000)
#define fMidiSystemReset			(0x8000)


/* variables */
extern struct MIDI_SystemMessage MidiSystemMessage;
extern struct MIDI_ChannelMessage MidiChannelMessage;

/* prototypes */
extern void MidiInit(void);
extern void MidiSetChannel(uint8_t chan);
extern uint8_t MidiGetChannel(void);
extern void MidiParse(void);



#endif 
