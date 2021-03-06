#ifndef GRStaff_H
#define GRStaff_H

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

// #include "GRDefine.h"
#include "GRCompositeNotationElement.h"
#include "GuidoDefs.h"

class ARBar;
class ARBarFormat;
class ARClef;
class ARChord;
class ARDoubleBar;
class ARKey;
class ARInstrument;
class ARIntens;
class AROctava;
class ARRepeatBegin;
class ARRepeatEnd;
class ARRepeatEndRangeEnd;
class ARFinishBar;
class ARText;
class ARMeter;
class ARMusicalVoiceState;
class ARStaffFormat;

class GRVoice;
class GRClef;
class GRNote;
class GRMeter;
// class GRNoteFactory;
class GRKey;
class GRStaffManager;
class GRVoiceManager;
class GRSystem;
class GRSystemSlice;
class GRGlue;
class GRSpring;
class GRRod;
class GRFinishBar;
class GRInstrument;

class GRRepeatEnd;
class GRRepeatBegin;
class GRDoubleBar;
class GRFermata;
class GRIntens;
class GRText;
class GRBar;
class GRPossibleBreakState;


template <class T> class KF_List;

typedef KF_List<float> AccList;
// typedef KF_IPointerList<GRTag> GRTagPointerList;
// typedef KF_Vector<float> UnitVect;

/** \brief Keeps information about accidentals at measure level.
*/
// DF - class introduced on nov. 4 2011 to cover octave changes
// and to comply to the practice described in "Behind Bars" E. Gould p. 79
class MeasureAccidentals
{
	enum	{ kPitchClasses = 12, kOctRange=10 };	// 12 pitch classes to differentiate 'is' notes 
	float	fAccidentals[kPitchClasses][kOctRange];	// keeps current accidentals at measure level
	float	fPClass[kPitchClasses];					// keeps current accidentals by pîtch class
	public:
				 MeasureAccidentals()	{ reset(); }
		virtual ~MeasureAccidentals() {}
		
		void	reset();
		void	setPitchClassAccidental(int pclass, float accidental);
		void	setAccidental(int pitch, int oct, float accidental);
		float	accidental (int pitch, int oct) const;
		float	accidental (int pitch) const;

		MeasureAccidentals & operator=(const MeasureAccidentals &ma);
};

/** \brief Keeps information about a staff.
*/
class GRStaffState
{
	friend class GRStaff;
	friend class GRStaffManager;
	friend class GRVoiceManager;
	friend class GRSystem;
	friend class GRSystemSlice;

	public:
		enum clefstate { CLEFAUTO, CLEFEXPLICIT, CLEFINTERN, CLEFOFF };

						 GRStaffState();
		virtual 		~GRStaffState();

		GRStaffState & operator=(const GRStaffState &tmp);

		GRStaffState &	getState()								{ return *this; }
		int				getInstrKeyNumber() const				{ return instrNumKeys; }
		void			reset2key();

	protected:
		// Meter-Parameters
		bool	meterset;		// flag if meter-sig is given . TRUE, FALSE
		int		mnum;			// MeasureNumber

		ARMeter * curmeter;
		// Noteparameter
		bool	keyset;							// flag for if key-sig is given
		int		numkeys;						// number of accidentals, neede for GRKey.
		float	KeyArray[NUMNOTES];
		MeasureAccidentals	fMeasureAccidentals;

		ARKey * curkey;
		// clef-Parameter
		clefstate	clefset;			// CLEFINTERN, CLEFEXPLICIT, CLEFAUTO, [ CLEFOFF ]
		std::string	clefname;			// clef name (same as for ARClef ...)
		int			basepit;
		int			basepitoffs;		// these two parameters are for
		int			instrNumKeys;		// instrument-tag:
				// basepitoffs: the offset for the given instrument, example: clarinet in A
				// has an offset of -2, it is two pitchclasses away from c-major
				// instrnumkeys: the number of keys for the transposed instrument ,,,
		float instrKeyArray[NUMNOTES];
				// the key-array for the current instrument (if transposed).

		int baseoct;
		int octava;
		int baseline;

		ARClef *		curclef;
		ARBarFormat *	curbarfrmt;
		ARStaffFormat * curstaffrmt;

		float	staffLSPACE;	// distance between two lines
		int		numlines;		// Zahl der Notenlinien.
		float	distance;
		bool	distanceset;
		// this is VOICE-Stuff!
		// Stemdirection
		// stemstate stemset;  // STEMUP, STEMDOWN, STEMAUTO

		// This is voice-stuff!
		// Tuplets
		// int tupletset;
		// GRTupletState tplstate;

 };

/** \brief A GRStaff represents a single line of music.
*/

class GRStaff : public GRCompositeNotationElement
{
	friend class GRPossibleBreakState;
	friend class GRStaffState;
	friend class GRStaffManager;
	friend class GRVoiceManager;

	public:
						GRStaff(GRSystemSlice * systemslice);
		virtual 		~GRStaff();

		float           getDistance() const;
		GRSystem *      getGRSystem() const;
		GRSystemSlice * getGRSystemSlice() const;
		GRStaff *       getPreviousStaff() const;
		int             getStaffNumber() const;
		GRGlue *        getEndGlue() const;
		GRGlue *        getStartGlue() const;
		GRGlue *        getSecondGlue() const;
		int             getFontSize() const;
		float           getStaffLSPACE() const      { return mStaffState.staffLSPACE; }
		float           getSizeRatio() const        { return mStaffState.staffLSPACE / LSPACE; }
		const GRStaffState *    getStaffState();
		GRStaffState &          getGRStaffState() { return mStaffState; }
		const GRStaffState &    getGRStaffState() const { return mStaffState; }

		virtual float       getNotePosition(TYPE_PITCH pit, TYPE_REGISTER oct) const;
		virtual GDirection  getDefaultThroatDirection(TYPE_PITCH pit, TYPE_REGISTER oct) const;
		virtual int         getNumHelplines(TYPE_PITCH pit, TYPE_REGISTER oct) const;
		virtual VGColor     getNoteColor(TYPE_PITCH pit) const;
		virtual int         getNumlines() const { return mStaffState.numlines; }
		virtual	float       getDredgeSize() const;
		virtual float       getKeyPosition(TYPE_PITCH pitch, int numkeys = 1) const;
		virtual GRNote *    getLastNote() const;

		virtual void setStaffFormat(ARStaffFormat * staffrmt);
		void    setStaffState(GRStaffState * state);
		void    setInstrumentFormat(const GRStaffState & state);
		void    setBarFormat(ARBarFormat * barfrmt);
		void    setNoteParameters(GRNote * inNote );
		void    setKeyParameters(GRKey * inKey);
		void    setDistance(float distance)         { mStaffState.distanceset = true; mStaffState.distance = distance; }
		void    setEndPosition( float newendpos)    { mLength = newendpos - mPosition.x; }
		void    setLength( float newlength)         { mLength = newlength; }

		GRNotationElement * AddOctava(AROctava * aroct);
		void                AddSecondGlue(GRGlue * myglue);
		void                AddElementAt(GuidoPos pos,GRNotationElement * el);
		GRRepeatBegin *     AddRepeatBegin(ARRepeatBegin *);
		GRRepeatEnd *       AddRepeatEnd(ARRepeatEnd *);
		GRDoubleBar *       AddDoubleBar(ARDoubleBar * ardbar,const TYPE_TIMEPOSITION & tp);
		GRFinishBar *       AddFinishBar(ARFinishBar * arfbar,const TYPE_TIMEPOSITION & tp);
		GRIntens *          AddIntens(ARIntens * aintens);
		GRText *            AddText(ARText * atext);
		GRKey *             AddKey(ARKey * arkey);
		GRBar *             AddBar(ARBar * abar, const TYPE_TIMEPOSITION & von);
		GRMeter *           AddMeter(ARMeter * armeter);
		void                AddTag(GRNotationElement * grtag);
		GRClef *            AddClef(ARClef *); // adds a clef to the staff at the current position.
		GRInstrument *      AddInstrument(ARInstrument *);

		virtual AccList * askAccidentals(TYPE_PITCH p_pit, TYPE_REGISTER p_oct, int p_acc, float detune=0.f);

		virtual void	boundingBoxPreview();
		virtual void	updateBoundingBox();
		virtual void	FinishStaff();
		virtual float	FirstNoteORRestXPos() const;

		void        UpdateStaffState(GuidoPos pos);
		void        TellNewStaff(GuidoPos pos);
		void        createNewRods(GRStaffManager * staffmgr, int & startspr, int & endspr);
		void        CreateBeginElements(GRStaffManager * staffmgr,GRStaffState & state, int staffnum);
		GuidoPos    EndStaff(const TYPE_TIMEPOSITION & tp, GRStaffManager * staffmgr,
							GuidoPos endpos = 0, int lastline = 0);

		void        EndStaff(const TYPE_TIMEPOSITION & tp,  GRSpring * spr);
		void        BeginStaff(GRStaffManager * staffmgr);
		void        addElementsToSprings();

		virtual void GGSOutput() const;
		virtual void OnDraw( VGDevice & hdc ) const;
		virtual void GetMap( GuidoeElementSelector sel, MapCollector& f, MapInfos& infos ) const;
		virtual void print() const;

		// this function was defined as private previously. Because GRNoteFactory needs to add elements 
		// to the staff (Tuplets), it must be defined public.

		// Dies function will be very powerfull  ...
		// It must do the part of createStaffElements and call createCutStaffElement aufrufen ...
		// the old functionality will be lost
		void addNotationElement(GRNotationElement * notationElement);

		void checkSystemBar(const TYPE_TIMEPOSITION & tp);

  protected:

		void	DebugPrintState(const char * info) const;

		void	DrawStaffUsingSymbolScale( VGDevice & hdc ) const;
		void	DrawStaffUsingSymbolRepeat( VGDevice & hdc ) const;
		void	DrawStaffUsingLines( VGDevice & hdc ) const;
		void	DrawNotationElements( VGDevice & hdc ) const;

		void	setClefParameters(	GRClef * grclef, GRStaffState::clefstate cstate = GRStaffState::CLEFAUTO );
		void	setMeterParameters( GRMeter * grmeter );

		GuidoPos 		lastrodpos;
		GRRod * 		lastrod;
	  	GRRod * 		firstrod;
	  	GRGlue * 		endglue;
	  	GRGlue * 		startglue;
	  	GRGlue * 		secglue;
		GRSystem * 		mGrSystem;
		GRSystemSlice * mGrSystemSlice;

	  	float     		mLength;
		GRStaffState 	mStaffState;

		// for the STEM-Auto calculation
		float 			avg_position;
		int 			mNoteCount;

 	 private:
		TYPE_TIMEPOSITION	fLastSystemBarChecked;
		void newMeasure(const TYPE_TIMEPOSITION & tp);
};

#endif


