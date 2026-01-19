/*!
	@brief	MIDI parsing library

	@file	midi.c
	@author	Satoshi "Chuck" Takagi
	@date	2009/04/03
 */


#include <inttypes.h>
#include "midi.h"

static uint8_t _midiSystemMessage(void);
static void _midiChannelMessage(void);

static uint8_t rb;				//!< read byte from UART
static uint8_t midi_ch = 0;		//!< MIDI channel
static uint8_t midi_stat = 255;	//!< MIDI parsing status initial value
static uint8_t midi_recv_ch;	//!< MIDI receive channel
static uint8_t midi_byte[3];	//!< MIDI parsing work array


//	@brief	MIDI シリアルポートの初期化を行う
void MidiInit(void)
{
	midiUartInit();
}


//	@brief	MIDI channel 番号を記録する
void MidiSetChannel(uint8_t ch)
{
	if (ch < 16)
		midi_ch = ch;
}

//	@brief	MIDI channel 番号を取り出す
uint8_t MidiGetChannel(void)
{
	return (midi_ch);
}


//	@brief	MIDI Parser
/*!
	@brief	MIDI Parser

	@param	なし

	@note	MIDIシステムメッセージを解析し、system exclusiveは除外してそれ以外の必要なメッセージを構造体に記録する。main loop で定期的に呼び出すこと。
*/

void MidiParse(void)
{
	int16_t c;

    rb = c = midiUartGetByte();
    if(c != -1){
		if( _midiSystemMessage() ){
			_midiChannelMessage();
		}
	}
}



/*!
	@brief	MIDI System Message parser

	@param	なし
	@return	引き続くMIDI Channel Messageがあれば1

	@note	MIDIシステムメッセージを解析し、system exclusiveは除外してそれ以外の必要なメッセージをMidiSystemMessage構造体に記録する。
*/
static uint8_t _midiSystemMessage(void)
{
	static uint8_t sysex;

	if (sysex) {					// in sysex process
		if (rb == 0xF7) sysex=0;	// end of sysex process
	} else {
		if (rb < 0xF8) { 
			if (rb >= 0xF0) {		// common msg
				if (rb == 0xF0)	sysex = 1;	// start of sysex process
				midi_stat = 100;
			} else if (rb > 0x7F) {	// voice msg
				midi_recv_ch = rb & 0x0F;
				if (midi_recv_ch == midi_ch) {
					midi_stat = 0;
				}
			} else {				// store midi data byte 00-7F
				midi_byte[midi_stat & 0x01] = rb;
 			}
 			return 1;
 		} else {					// realtime msg >= 0xF8
			switch (rb) {
			case 0xF8:	MidiSystemMessage.Status |= fMidiTimingClock; break;		// timing clock
			case 0xF9:	break;
			case 0xFA:	MidiSystemMessage.Status |= fMidiStart; break;				// start
			case 0xFB:	MidiSystemMessage.Status |= fMidiContinue; break;			// continue
			case 0xFC:	MidiSystemMessage.Status |= fMidiStop; break;				// stop
			case 0xFD:	break;
			case 0xFE:	MidiSystemMessage.Status |= fMidiActiveSensing; break;		// active sensing
			case 0xFF:	MidiSystemMessage.Status |= fMidiSystemReset;   break;		// system reset
			}
		}
	}

	return 0;
}

/*!
	@brief	MIDI Channel Message parser

	@param	なし
	@return	なし

	@note	MIDIチャネルメッセージを解析し、必要なメッセージをMidiChannelMessage構造体に記録する。1回のコールにつき1バイトずつ処理し、進捗状態等をmidi_byteにて管理している。
*/
static void _midiChannelMessage(void)
{
	switch (midi_stat) {
	case 0:
		switch ( rb & 0xF0 ) {
#ifdef USE_MIDI_NOTE
		case MIDI_NOTE_OFF			: midi_byte[2] = rb; midi_stat=10;  break;
		case MIDI_NOTE_ON			: midi_byte[2] = rb; midi_stat=20;  break;
#endif // USE_MIDI_NOTE

#ifdef USE_MIDI_PBEND
		case MIDI_PBEND				: midi_byte[2] = rb; midi_stat=30; break;
#endif // USE_MIDI_PBEND

#ifdef USE_MIDI_CC
		case MIDI_CC				: midi_byte[2] = rb; midi_stat=40; break;
#endif // USE_MIDI_CC

#ifdef USE_MIDI_PC
		case MIDI_PC				: midi_byte[2] = rb; midi_stat=50;  break;
#endif // USE_MIDI_PC

#ifdef NO_USE
		case MIDI_POLY_PRESSURE		: midi_byte[2] = rb; midi_stat=60;  break;
		case MIDI_CHANNEL_PRESSURE	: midi_byte[2] = rb; midi_stat=70; break;
#endif
		default:
			midi_stat=255;
		}
		break;

#ifdef USE_MIDI_NOTE
	/* Note Off */
	case 10: midi_stat=11; break;
	case 11: midi_stat=10;
			if (midi_recv_ch == midi_ch) {
				MidiChannelMessage.NoteOff.Note = midi_byte[0];
				MidiChannelMessage.NoteOff.Velocity = midi_byte[1];
				MidiChannelMessage.Status |= fMidiNoteOff;
			}
			break;

	/* Note On */
	case 20: midi_stat=21; break;
	case 21: midi_stat=20;
			if (midi_recv_ch == midi_ch) {
				if (midi_byte[1] == 0) {
					// note off
					MidiChannelMessage.NoteOff.Note = midi_byte[0];
					MidiChannelMessage.NoteOff.Velocity = 64;
					MidiChannelMessage.Status |= fMidiNoteOff;
				} else {
					MidiChannelMessage.NoteOn.Note = midi_byte[0];
					MidiChannelMessage.NoteOn.Velocity = midi_byte[1];
					MidiChannelMessage.Status |= fMidiNoteOn;
				}
			}
			break;
#endif // USE_MIDI_NOTE

#ifdef USE_MIDI_PBEND
	/* PitchBend */
	case 30: midi_stat=31; break;
	case 31: midi_stat=30;
			if (midi_recv_ch == midi_ch) {
				MidiChannelMessage.PitchBend.Value = midi_byte[1]<<7 + midi_byte[0];
				MidiChannelMessage.Status |= fMidiPitchBend;
			}
			break;
#endif // USE_MIDI_PBEND

#ifdef USE_MIDI_CC
	/* Control Change */
	case 40: midi_stat=41; break;
	case 41: midi_stat=40;			// data and for running status
			if (midi_recv_ch == midi_ch) {
				MidiChannelMessage.ControlChange.Control = midi_byte[0];
				MidiChannelMessage.ControlChange.Value = midi_byte[1];
				if (midi_byte[0] < 120)
					MidiChannelMessage.Status |= fMidiControlChange;
				else
					MidiChannelMessage.Status |= fMidiModeChange;
			}
			break;
#endif // USE_MIDI_CC

#ifdef USE_MIDI_PC
	/* Program change */
	case 50:
			if (midi_recv_ch == midi_ch) {
				MidiChannelMessage.ProgramChange.Value = midi_byte[0];
				MidiChannelMessage.Status |= fMidiProgramChange;
			}
			break;

#endif // USE_MIDI_PC

#ifdef NO_USE
	/* Polyphonic Key Pressure */ 
	case 60: midi_stat=61; break;
	case 61: midi_stat=60;
			if (midi_recv_ch == midi_ch) {
				MidiChannelMessage.PolyPressure.Note = midi_byte[0];
				MidiChannelMessage.PolyPressure.Value = midi_byte[1];
				MidiChannelMessage.Status |= fMidiPolyPressure;
			}
			break;

	/* ChannelPressure */
	case 70:
			if (midi_recv_ch == midi_ch) {
				MidiChannelMessage.ChannelPressure.Value = midi_byte[0];
				MidiChannelMessage.Status |= fMidiChannelPressure;
			}
			break;
#endif // NO_USE

#ifdef NOT_IMPLEMENTED
	case 100:
		switch ( rb ) {
		case MIDI_MIDITC   : midi_stat = 110; break;
		case MIDI_SONG_PTR : midi_stat = 120; break;
		case MIDI_SONG_SEL : midi_stat = 130; break;
		case MIDI_TUNE_REQ : midi_stat = 255; break;
		case 0xF4          : midi_stat = 255; break;
		case 0xF5          : midi_stat = 255; break;
		}
		break;

	/* MIDI timecode */
	case 110: break;

	/* Song Position Pointer */
	case 120: midi_stat=121; break;
	case 121: break;

	/* Song Select */
	case 130: break;
#endif // NOT_IMPLEMENTED
	
	default: break;
	}

}
