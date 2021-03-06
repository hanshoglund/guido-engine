#ifndef GRNotationElement_H
#define GRNotationElement_H

/*
	GUIDO Library
	Copyright (C) 2002  Holger Hoos, Juergen Kilian, Kai Renz
	Copyright (C) 2003	Grame

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

#include <cassert>
#include <iostream>

#include "GObject.h"
#include "defines.h"	// for TYPE_DURATION

class NEPointerList;
class GRStaff;
class GRSystemSlice;
class ARMusicalObject;
class GRAccidental;


/** \brief parent class for all notation elements.
*/
class GRNotationElement : public GObject  
{
public:
					GRNotationElement();
	virtual		 	~GRNotationElement();

	static int		ComparePos( const GRNotationElement * g1, const GRNotationElement * g2 );

			GRStaff * getGRStaff() const				{ return mGrStaff; }
	virtual void	setGRStaff( GRStaff * in )			{ mGrStaff = in; }

			GRSystemSlice * getGRSystemSlice() const;
	virtual int		getStaffNumber() const;

	virtual void	tellPosition( GObject * caller, const NVPoint & point);
		
	virtual float	getLeftSpace() const 				{ return mLeftSpace; }
	virtual float	getRightSpace() const 				{ return mRightSpace; }

	virtual void	setLeftSpace( float lspace ) 		{ mLeftSpace = lspace; }
	virtual void	setRightSpace( float rspace ) 		{ mRightSpace = rspace; }

	virtual void	setHPosition( float nx );
	virtual void	setPosition( const NVPoint & newPosition);

	virtual void	setNeedsSpring( int inFlag )		{ mNeedsSpring = inFlag; }
	virtual int		getNeedsSpring() const				{ return mNeedsSpring; }
	virtual int		getSpringID() const 				{ return  mSpringID; }
	virtual void	tellSpringID( int inSpringID ) 		{ mSpringID = inSpringID; }

			NEPointerList * getAssociations()			{ return mAssociated; }
			const NEPointerList * getAssociations()	const { return mAssociated; }

	virtual void removeAssociation( const NEPointerList & nl );
	virtual void removeAssociation( GRNotationElement * el );
			void FreeAssociatedList();

	virtual NVPoint getTieEnd() const					{ return mPosition; } // do not rely on that
	virtual NVPoint getTieStart() const					{ return mPosition; }
	
			void addToBoundingBox( GRNotationElement * in );


	
	// - Drawing	
	virtual void OnDraw( VGDevice & hdc ) const;
			void OnDrawSymbol(VGDevice & hdc, unsigned int inSymbol, float inOffsetX = 0,
								float inOffsetY = 0, float inFontSize = 0) const; // , float inScaleX = 0) const;
			void OnDrawText( VGDevice & hdc, const char * cp, int inCharCount = -1 ) const;
			void DrawSymbol( VGDevice & hdc, unsigned int inSymbol, float inOffsetX = 0, float inOffsetY = 0,
								   float inFontSize = 0) const;

	virtual void SendMap (const NVRect& r, MapCollector& f, TYPE_TIMEPOSITION date, TYPE_DURATION dur, GuidoElementType type, MapInfos& infos) const;
	virtual void SendMap (MapCollector& f, TYPE_TIMEPOSITION date, TYPE_DURATION dur, GuidoElementType type, MapInfos& infos) const;
	virtual unsigned int getTextAlign() const;
	
	// 
	virtual ARMusicalObject * getAbstractRepresentation() const		{ return 0; }	
	virtual const TYPE_DURATION & getDuration() const				{ return mDurationOfGR; }
	
	virtual void setDuration(const TYPE_TIMEPOSITION & dur )					{ mDurationOfGR = dur; }
	virtual void setRelativeTimePosition(const TYPE_TIMEPOSITION & tp )			{ mRelativeTimePositionOfGR = tp; }
	virtual void setRelativeEndTimePosition(const TYPE_TIMEPOSITION & tp )		{ mDurationOfGR = tp - mRelativeTimePositionOfGR; }
	virtual const TYPE_TIMEPOSITION & getRelativeTimePosition() const			{ return mRelativeTimePositionOfGR; }
	
	
	virtual TYPE_TIMEPOSITION  getRelativeEndTimePosition() const;
	
	virtual void recalcVerticalPosition();
	virtual void addAssociation( GRNotationElement * grnot );

	// - Text, streams
	virtual void print() const 	{}
	virtual void print(std::ostream& os) const;
	virtual void GGSOutput() const;
	virtual void GGSOutputAt(unsigned int tmptype, long offsx = 0, long offsy = 0, 
															float mysize = 0.0f ) const;
	inline const NEPointerList* associated() const { return mAssociated; }

protected:
	
	void		DrawExtents( VGDevice & hdc, const VGColor & inColor ) const; // Debug

	unsigned int mSymbol;
	GRStaff * 	mGrStaff;
	int  		mNeedsSpring;	// Can be -1, 0 or 1
	int  		mSpringID;
	float 		mLeftSpace;		// Can't we deal only with bounding boxes ?
	float 		mRightSpace;

	TYPE_DURATION mDurationOfGR;
	TYPE_TIMEPOSITION mRelativeTimePositionOfGR;

	NEPointerList * mAssociated;

};

std::ostream& operator<< (std::ostream& os, const GRNotationElement& e);


/** \brief This comparison routine compares to notationelements
	 by y-position. This way the association-list of chords is sorted.
*/
/*
 previous implementation made use of template declared as below
 template <class T> int compnotposy( const T * el1, const T * el2 );
 since gcc 4.0, template has been removed and replaced by the 2 specialized
 functions declared below and implemented in cpp file.
*/
int compnotposy( const GRNotationElement * el1, const GRNotationElement * el2 );
int compaccposy( const GRAccidental * el1, const GRAccidental * el2 );

#endif
