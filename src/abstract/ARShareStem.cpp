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

#include <iostream>
#include "ARShareStem.h"
#include "TagParameterInt.h"
#include "TagParameterList.h"
#include "ListOfStrings.h"

ListOfTPLs ARShareStem::ltpls(1);

ARShareStem::ARShareStem(const ARShareStem * stm)
: ARMTParameter(-1,stm)
{

}
ARShareStem::~ARShareStem()
	{
	}

void ARShareStem::setTagParameterList(TagParameterList & tpl)
{
	if (ltpls.GetCount() == 0)
	{
		// create a list of string ...

		ListOfStrings lstrs; // (1); std::vector test impl
		lstrs.AddTail(
			(
			""));
		CreateListOfTPLs(ltpls,lstrs);
	}

	TagParameterList *rtpl = NULL;
	int ret = MatchListOfTPLsWithTPL(ltpls,tpl,&rtpl);

	if (ret>=0 && rtpl)
	{
		// we found a match!
		if (ret == 0)
		{
			// then, we now the match for
			// the first ParameterList
			// GuidoPos pos = rtpl->GetHeadPosition();

		}

		delete rtpl;
	}
		
	
	tpl.RemoveAll();
}

ARMusicalObject * ARShareStem::Copy() const
{
	return new ARShareStem(this);
}

void ARShareStem::PrintName(std::ostream & os) const
{
	os << "\\shareStem";
	if (!getRange())
		os << "Begin";
}

void ARShareStem::PrintParameters(std::ostream & ) const
{
}
