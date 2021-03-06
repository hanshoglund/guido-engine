/*
	GUIDO Library
	Copyright (C) 2011	Grame

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

#include <stdio.h>
#include <string.h>

#include "SVGDevice.h"
#include "VGFont.h"
#include "GUIDOEngine.h"

using namespace std;

//______________________________________________________________________________
ostream& operator<< (ostream& os, const svgendl& endl)
{
    endl.print(os);
    return os;
}

//______________________________________________________________________________
void svgendl::print(std::ostream& os) const { 
	int i = fIndent;
    os << std::endl;
    while (i-- > 0)  os << "    ";
}

//______________________________________________________________________________
// SVGDevice
//______________________________________________________________________________
SVGDevice::SVGDevice(std::ostream& outstream, SVGSystem* system, const char* guidofontfile) : 
	fSystem (system), fGuidoFontFile(guidofontfile),
	fStream(outstream), 
	fWidth(1000), fHeight(1000),
	fMusicFont(0), fTextFont(0), fOpMode(kUnknown),
	fXScale(1), fYScale(1), fXOrigin(0), fYOrigin(0), fXPos(0), fYPos(0),
	fFontAlign(kAlignBase), fDPI(0),
	fPushedPen(false), fPushedPenColor(false), fPushedPenWidth(false), fPushedFill(false), fScaled(false), fOffset(false),
	fCurrFont (kNoFont),
	fPendingStrokeColor(0),
	fBeginDone(false)
{
}
SVGDevice::~SVGDevice() 
{
	delete fPendingStrokeColor;
}

//______________________________________________________________________________
// Guido font output
//______________________________________________________________________________
void SVGDevice::getsvgfont (const char* ptr, string& str) const
{
	const char* fontstart = "<font";
	const char* fontend = "</font>";
	int len = strlen (fontstart);
	bool infont = false;
	while (*ptr) {
		if (!infont) {
			if (*ptr == '<') {
				if (strncmp(ptr, fontstart, len) == 0) {
					while (len-- && *ptr) ptr++;
					if ((*ptr == ' ') || (*ptr ==  '	')) {
						infont = true;
						str += fontstart;
						str += ' ';
						len = strlen(fontend);
					}
				}
			}
			ptr++;
		}
		else {
			if (*ptr == '<') {
				if (strncmp(ptr, fontend, len) == 0) {
					str += fontend;
					break;
				}
			}
			str += *ptr++;
		}
	}
}

void SVGDevice::printFont(std::ostream& out, const char* file) const
{
	ifstream is (file);
	if (is.is_open()) {
		is.seekg (0, ios::end);
		int length = is.tellg();
		is.seekg (0, ios::beg);
		char * buffer = new char [++length];
		// read data as a block:
		is.read (buffer,length);
		buffer[length-1] = 0;
		string str;
		getsvgfont (buffer, str);
		delete [] buffer;
		if (str.size())
			out << "<defs>\n" << str << "\n</defs>" << endl;
	}
	else cerr << "SVGDevice: can't open svg guido font " << file << endl;
}

//______________________________________________________________________________
// - Drawing services
//______________________________________________________________________________
bool SVGDevice::BeginDraw()
{
	fStream << "<?xml version=\"1.0\"?>" << fEndl;
	fStream << "<svg viewBox=\"0 0 " << fWidth << " " << fHeight << "\" xmlns=\"http://www.w3.org/2000/svg\"  version=\"1.1\">";
	fEndl++;
	fStream << fEndl << "<desc> SVG file generated using the GuidoEngine version " << GuidoGetVersionStr() << "</desc>";
	if (fGuidoFontFile) printFont (fStream, fGuidoFontFile);
	fBeginDone = true;
	if (fPendingStrokeColor) {
		SelectPenColor (*fPendingStrokeColor);
		delete fPendingStrokeColor;
		fPendingStrokeColor = 0;
	}
	return true;
}

void SVGDevice::EndDraw()
{
	if (fPushedPen) closegroup();
	if (fPushedPenColor) closegroup();
	if (fPushedPenWidth) closegroup();
	if (fPushedFill) closegroup();
	if (fScaled) closegroup();
	if (fOffset) closegroup();
	if (fCurrFont) closegroup();
	fPushedPen = false;
	fPushedFill = false;
	fScaled = false;
	fOffset = false;
	fEndl--; fStream << fEndl << "</svg>" << fEndl;
}

void SVGDevice::InvalidateRect( float left, float top, float right, float bottom ) {}
void SVGDevice::checkfont()	
{
	if (fCurrFont) {
		closegroup();
		fCurrFont = 0;
	}
}

//______________________________________________________________________________
// - Standard graphic primitives
//______________________________________________________________________________
void SVGDevice::MoveTo( float x, float y )	{ fXPos = x; fYPos = y; }
void SVGDevice::LineTo( float x, float y )	{ Line(fXPos, fYPos, x, y); }
void SVGDevice::Line( float x1, float y1, float x2, float y2 )
{
	fStream << fEndl << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 <<	"\" y2=\"" << y2 << "\" />";
}

void SVGDevice::Frame( float left, float top, float right, float bottom )
{
	fStream << fEndl << "<rect x=\"" << left << "\" y=\"" << top 
			<< "\" width=\"" << (right - left) <<	"\" height=\"" << (bottom - top) 
			<< "\" fill=\"none\" />";
}

void SVGDevice::Arc( float left, float top, float right, float bottom, float startX, float startY, float endX, float endY )
{
}

//______________________________________________________________________________
// - Filled surfaces
//______________________________________________________________________________
void SVGDevice::Triangle( float x1, float y1, float x2, float y2, float x3, float y3 )
{
	float x[3], y[3];
	x[0] = x1;
	x[1] = x2;
	x[2] = x3;
	y[0] = y1;
	y[1] = y2;
	y[2] = y3;
	Polygon (x, y, 3);
}

void SVGDevice::Polygon( const float * xCoords, const float * yCoords, int count )
{
	fStream << fEndl << "<polygon points=\"";
	for (int i=0; i < count; i++) 
		fStream << xCoords[i] << " " << yCoords[i] << "  ";
	fStream << "\" />";
}

void SVGDevice::Rectangle( float left,  float top, float right, float bottom )
{
	fStream << fEndl << "<rect x=\"" << left << "\" y=\"" << top 
			<< "\" width=\"" << (right - left) <<	"\" height=\"" << (bottom - top) 
			<< "\" />";
}

//______________________________________________________________________________
void SVGDevice::print (std::ostream& out, const VGColor& color) const
{
	char buff[10];
	sprintf (buff, "#%02x%02x%02x",  int(color.mRed), int(color.mGreen), int(color.mBlue));
	out << buff;
}

float SVGDevice::alpha2float (const VGColor& color)	const	{ return (float)color.mAlpha / ALPHA_OPAQUE; }
void SVGDevice::closegroup()								{ fEndl--; fStream << fEndl << "</g>"; }

//______________________________________________________________________________
// - Font services
//______________________________________________________________________________
void			SVGDevice::SetMusicFont( const VGFont * font )	{ fMusicFont = font; }
const VGFont *	SVGDevice::GetMusicFont() const					{ return fMusicFont; }
void			SVGDevice::SetTextFont( const VGFont * font )	{ fTextFont = font; }
const VGFont *	SVGDevice::GetTextFont() const					{ return fTextFont; }

//______________________________________________________________________________
// - Pen & brush services
//______________________________________________________________________________
void SVGDevice::SelectPen( const VGColor & c, float w )
{
	if (fPushedPen) closegroup();
	PushPen (c, w);
	fPushedPen = true;
}
void SVGDevice::SelectFillColor( const VGColor & c )
{
	if (fPushedFill) closegroup();
	PushFillColor (c);
	fPushedFill = true;
}

void SVGDevice::PushPen( const VGColor & color, float width )
{
	fStream << fEndl << "<g style=\"stroke:";
	print(fStream, color);
	fStream << "; stroke-opacity:" << alpha2float(color) << "; stroke-width:" << width << "\">"; 
	fEndl++ ;
}
void SVGDevice::PushFillColor( const VGColor & color )
{
	fStream << fEndl << "<g style=\"fill:";
	print(fStream, color);
	fStream << "; fill-opacity:" << alpha2float(color) << ";\">"; 
	fEndl++ ;
}

void SVGDevice::PopPen()			{ closegroup(); }
void SVGDevice::PopFillColor()		{ closegroup(); }

void SVGDevice::SetRasterOpMode( VRasterOpMode mode)			{ fOpMode = mode; }
VGDevice::VRasterOpMode SVGDevice::GetRasterOpMode() const		{ return fOpMode; }


//______________________________________________________________________________
// - Bitmap services (bit-block copy methods)
// =======  unsupported =======
//______________________________________________________________________________
bool SVGDevice::CopyPixels( VGDevice* pSrcDC, float alpha)		{ return false; } 
bool SVGDevice::CopyPixels( int xDest, int yDest, VGDevice* pSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, float alpha) { return false; }
bool SVGDevice::CopyPixels( int xDest, int yDest, int dstWidth, int dstHeight, VGDevice* pSrcDC, float alpha) { return false; }
bool SVGDevice::CopyPixels( int xDest, int yDest, int dstWidth, int dstHeight, VGDevice* pSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, float alpha) { return false; }

//______________________________________________________________________________
// - Coordinate services
//______________________________________________________________________________
void SVGDevice::SetScale( float x, float y )
{ 
	fXScale = x; fYScale = y; 
	if (fScaled) closegroup();
	fStream << fEndl << "<g transform=\"scale(" << x << ", " << y << ")\">";
	fEndl++ ;
	fScaled = true;
}

void SVGDevice::SetOrigin( float x, float y )
{ 
	checkfont();
	if (fOffset) closegroup();
	fStream << fEndl << "<g transform=\"translate(" << x << ", " << y << ")\">";
	fXOrigin = x; fYOrigin = y;
	fEndl++ ;
	fOffset = true;
}
void SVGDevice::OffsetOrigin( float x, float y )	
{ 
	SetOrigin( fXOrigin + x, fYOrigin += y); 
}
void SVGDevice::LogicalToDevice( float * x, float * y ) const {}
void SVGDevice::DeviceToLogical( float * x, float * y ) const {}

float SVGDevice::GetXScale() const		{ return fXScale; }
float SVGDevice::GetYScale() const		{ return fYScale; }
float SVGDevice::GetXOrigin() const		{ return fXOrigin; }
float SVGDevice::GetYOrigin() const		{ return fYOrigin; }

void SVGDevice::NotifySize( int w, int h )	{ fWidth=w; fHeight=h;  }
int	SVGDevice::GetWidth() const				{ return fWidth; }
int SVGDevice::GetHeight() const			{ return fHeight; }


//______________________________________________________________________________
// - Text and music symbols services
//______________________________________________________________________________
const char* SVGDevice::align2str (int align) const
{
	if (align & kAlignRight)	return "text-anchor=\"end\"";
	if (align & kAlignCenter)	return "text-anchor=\"middle\"";
//	return "start"; // default value is "start"
	return "";
}
const char* SVGDevice::baseline2str (int align) const
{
	if (align & kAlignTop)	return " alignment-baseline=\"hanging\"";
//	return "baseline"; // default value is "baseline"
	return "";
}

//______________________________________________________________________________
void SVGDevice::selectfont (int fonttype)
{
	if (fCurrFont == fonttype) return;		// nothing to do
	if (fCurrFont) closegroup();

	const VGFont * font = 0;
	switch (fonttype) {
		case kMusicFont:	font = fMusicFont; break;
		case kTextFont:		font = fTextFont; break;
	}
	if (font) {
		fStream << fEndl << "<g font-family=\"" << font->GetName()  << "\">";  // << "\" font-size=\"" << font->GetSize() << "\">"; 
		fEndl++ ;
		fCurrFont = fonttype;
	}
}

void SVGDevice::DrawMusicSymbol(float x, float y, unsigned int inSymbolID )
{
	selectfont (kMusicFont);
	fStream << fEndl << "<text x=\"" << x << "\" y=\"" << y  << "\" " << align2str(fFontAlign) 
	<< baseline2str(fFontAlign) << " font-size=\"" << fMusicFont->GetSize() 
	<< "\" stroke=\""; 
	print(fStream, fFontColor);
	fStream << "\" fill=\""; 
	print(fStream, fFontColor);
	fStream << "\">&#" << inSymbolID << ";</text>"; 
}

void SVGDevice::DrawString( float x, float y, const char * s, int inCharCount )
{
	selectfont (kTextFont);
	fStream << fEndl << "<text x=\"" << x << "\" y=\"" << y << "\" " << align2str(fFontAlign) 
	<< baseline2str(fFontAlign) << " font-size=\"" << fTextFont->GetSize() 
	<< "\" stroke=\""; 
	print(fStream, fFontColor);
	fStream << "\" fill=\""; 
	print(fStream, fFontColor);
	fStream << "\">";
	fStream << fEndl++; 
	for (int i=0; i<inCharCount; i++) {
		if (s[i] > 0)
			fStream << s[i];
		else 
			fStream << "&#" << int((unsigned char)s[i]) << ";"; 
	}
	fEndl--;
	fStream << fEndl << "</text>"; 
}

void	SVGDevice::SetFontColor( const VGColor & color )	{ fFontColor = color; }
VGColor SVGDevice::GetFontColor() const						{ return fFontColor; }
void	SVGDevice::SetFontBackgroundColor( const VGColor & inColor )	{fFontBackgroundColor = inColor; }
VGColor SVGDevice::GetFontBackgroundColor() const						{ return fFontBackgroundColor; }
void	SVGDevice::SetFontAlign( unsigned int inAlign )	{ fFontAlign = inAlign; }
unsigned int SVGDevice::GetFontAlign() const			{ return fFontAlign; }

//______________________________________________________________________________
// - Printer informations services - useless in svg context
//______________________________________________________________________________
void SVGDevice::SetDPITag( float inDPI )		{ fDPI = inDPI; }
float SVGDevice::GetDPITag() const				{ return fDPI; }

//______________________________________________________________________________
// - VGDevice extension
//______________________________________________________________________________
void SVGDevice::SelectPenColor( const VGColor & color)
{
	if (fBeginDone) {
		if (fPushedPenColor) closegroup();
		PushPenColor (color);
		fPushedPenColor = true;
	}
	else fPendingStrokeColor = new VGColor(color);
}

void SVGDevice::SelectPenWidth( float witdh)
{
	if (fPushedPenWidth) closegroup();
	PushPenWidth (witdh);
	fPushedPenWidth = true;
}

void SVGDevice::PushPenColor( const VGColor & color)
{
	fStream << fEndl << "<g style=\"stroke:";
	print(fStream, color);
	fStream << "; stroke-opacity:" << alpha2float(color) << "\">"; 
	fEndl++ ;
}

void SVGDevice::PushPenWidth( float width)
{
	fStream << fEndl << "<g style=\"stroke-width:" << width << "\">"; 
	fEndl++ ;
}
void SVGDevice::PopPenColor()								{ closegroup(); }
void SVGDevice::PopPenWidth()								{ closegroup(); }

