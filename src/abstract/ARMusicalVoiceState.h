#ifndef ARMusicalVoiceState_H
#define ARMusicalVoiceState_H

/*
	GUIDO Library
	Copyright (C) 2002  Holger Hoos, Juergen Kilian, Kai Renz

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include <typeinfo>

#include "TagList.h"		// is a typedef
#include "defines.h"		// for TYPE_TIMEPOSITION
#include "GUIDOTypes.h"		// for GuidoPos
/*
class ARMusic;
class ARVoiceManager;
class GRVoiceManager;
*/

class ARMusicalTag;
class ARPositionTag;
class ARMeter;
class ARDisplayDuration;
class ARBeamState;
class ARAuto;
class ARGrace;
class ARChordTag;

template <class T> class KF_IPointerList;
typedef KF_IPointerList<ARPositionTag> PositionTagList;

/** \brief This class manage the state of a voice. Here additional information for 
	voices will be stored (i.e..: Stemstate (Stemsup, down, auto).
 */
class ARMusicalVoiceState
{
	friend class ARMusic;
	friend class ARMusicalVoice;
	friend class ARVoiceManager;
	friend class GRVoiceManager;
	friend class GRStaffManager;

	public:
				ARMusicalVoiceState();
				ARMusicalVoiceState(const ARMusicalVoiceState & vst);
				ARMusicalVoiceState & operator=(const ARMusicalVoiceState & vst);

		virtual ~ARMusicalVoiceState();

        ARMusicalTag * getCurStateTag(const std::type_info & ti);
		ARMusicalTag * RemoveCurStateTag(const std::type_info & ti);

		void DeleteAddedAndRemovedPTags();
		
		const TagList * getCurTags() const					{ return curtags; }
		const TagList * getCurStateTags() const				{ return curstatetags; }
		const PositionTagList * getCurPositionTags() const	{ return curpositiontags; }

		void AddStateTag(ARMusicalTag * ntag);
		void AddTag(ARMusicalTag * ntag);
		void RemoveTag(ARMusicalTag * ntag);
		void AddPositionTag(ARPositionTag * ntag, int addtoaddedlist = 1);
		void RemovePositionTag(ARPositionTag * ntag, int addtoremovedlist = 1);

		void DeleteAll();

	protected:

		// The timeposition of the last barline.
		TYPE_TIMEPOSITION curlastbartp;

		GuidoPos curlastbarpos;

		// ptagpos indicates the current position within the ptaglist.
		GuidoPos ptagpos;


		TYPE_TIMEPOSITION curtp;
		// Distinguish between Tags, that allow a range and those, who don't 

		TagList * curstatetags; // A list of Tags, that determine the State of
							// the voice (e.g. clef, key, meter, etc. )
							// All the tags, that have no range ...

		// the tags that are just tags in the musical voice, like e.g. ARText 
		// or ARFermata ... (the ones without ranges)
		TagList * curtags;

		// these describe those position tags that have been Added in the last
		// operation on the Voice with the given state.
		PositionTagList * addedpositiontags;

		// this list holds all the position tags that have been removed in the last
		// operation on the Voice with the state.
		PositionTagList * removedpositiontags;

		// this list holds all the position tags that are currently active.
		PositionTagList * curpositiontags;

		GuidoPos vpos; // Position within the voice

		// now we save the current meter
		// which is actually also a state-tag -> this is handled so that autobeaming can be done elegantly
		ARMeter * curmeter;

		// the current displayduration (if set) ...
		ARDisplayDuration * curdispdur;

		// we also save the beamstate ... makes it more elegant for autobeaming ...
		ARBeamState * curbeamstate;

		ARAuto * curautostate;

		ARGrace * curgracetag;
		ARChordTag * curchordtag;

		ARMusicalVoiceState * chordState;
		ARMusicalVoiceState * prevchordState;
};

#endif


