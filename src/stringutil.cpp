/***
 * -*- C++ -*-
 * -*- UTF-8 -*-
 *
 * Copyright (C),
 * @file
 * @brief
 * @author:
 * 		<name>		<email>
 * 		kins ren	kins.ren@me.com	
 * @version
 * @date
 * @warning
 * @History:
 * 		<author>	<time>	<desc>
*/

#include "stringutil.h"

string StringUtil::trim(const string &str)
{	
	string dst;
	int len = str.size();
	
	int spos = 0;
	int epos = len - 1;

	for( int i=spos; i<len; i++ )
	{
		if( !(str[i] == ' ' || str[i] == '\t' || str[i] == '\r' || str[i] == '\n') )
		{
			spos = i;
			break;
		}
	}
	
	if( spos < epos )
	{
		for( int i=epos; i>=0; i-- )
		{
			if( !(str[i] == ' ' || str[i] == '\t' || str[i] == '\r' || str[i] == '\n') )
			{
				epos = i;
				break;
			}
		}
		dst = str.substr(spos, epos-spos+1);	
	}

	return dst;
}

string StringUtil::lower(const string &str)
{
	string dst = str;

	for( int i=0; i< dst.size(); i++ )
	{
		if( dst[i] >= 0x41 && dst[i] <= 0x5A )
		{
			dst[i] = dst[i] + 0x20;
		}
	}

	return dst;
}

string StringUtil::upper(const string &str)
{
	string dst = str;

	for( int i=0; i< dst.size() ; i++ )
	{
		if( dst[i] >= 0x61 && dst[i] <= 0x7a )
		{
			dst[i] = dst[i] - 0x20;
		}
	}

	return dst;
}

bool StringUtil::startWith(const string &str, const string &prefix)
{
	if( prefix.empty() )
	{
		return false;
	}

	return (strncmp(str.c_str(), prefix.c_str(), prefix.size()) == 0);
}

bool StringUtil::endWith(const string &str, const string &suffix)
{
	if( suffix.empty() )
	{
		return false;
	}

	return (strncmp(str.c_str()+(str.size()-suffix.size()), suffix.c_str(), suffix.size()) == 0);
}

int StringUtil::getHexInt(const string &str, int pos, int len)
{
	const char *s = str.c_str();
	int slen = str.size();

	if( !s || slen <= 0 || pos >= slen || (pos+len)> slen || len <= 0 )
	{
		return 0;
	}

	int hexInt = 0;
	int unit = 1;
	for( int i=pos+len-1; i>=pos; i-- )
	{
		char hex = s[i];
		if( 0x30 <= hex && hex <= 0x39 )
		{
			hexInt += unit * (hex -0x30);
		}
		else if( 0x61 <= hex && hex <= 0x66 )
		{
			hexInt += unit * (hex - 0x61 + 10);
		}
		else
		{
			return 0;
		}

		unit *= 16;
	}

	return hexInt;
}

void StringUtil::split(const string& line, const string& segmentor, vector<string>& segs)
{
	bool bStart = false;
	int iStart = 0;
	size_t i=0;
	for( i=0; i<line.length(); i++ )
	{
		bool bSegment = (segmentor.find(line[i]) != string::npos);
		if( !bStart && !bSegment )
		{
			// 开始
			bStart = true;
			iStart = i;
		}
		else if( bStart && bSegment )
		{
			// 结束
			bStart = false;
			segs.push_back(line.substr(iStart, i-iStart));
		}
	}

	// 最后一个
	if( bStart )
	{
		segs.push_back(line.substr(iStart, i-iStart));
	}
}

void StringUtil::join(vector<string>& segs, const string &segmentor, string &line)
{
    line.clear();

    for( int i=0; i<segs.size(); i++ ) {
        if( i > 0 ) {
            line.append(segmentor);
        }
        line.append(segs[i]);
    }
}

bool StringUtil::convertHTMLColor(const string &style, unsigned int &color)
{
	const char *normalColors[] = {
		"#000000", "black",	"#c0c0c0", "silver",
		"#ff0000", "red",	"#0000ff", "blue",
		"#ffffff", "white",	"#ffff00", "yellow",
		"#00ff00", "green",	"#00ffff", "aqua",
	};

	int normalCorlorCount = sizeof(normalColors)/sizeof(char *);
	
	string colorStyle = lower(trim(style));
	if( colorStyle.empty() )
	{
		return false;
	}
	
	if( !startWith(colorStyle, "#") && !startWith(colorStyle, "rgb") )
	{
		// 非#,rgb开头
		
		bool bNormalColor = false;
		for( int i=0; i<normalCorlorCount/2; i++ )
		{
			if( colorStyle == normalColors[i*2+1] )
			{
				colorStyle = normalColors[i*2];
				bNormalColor = true;
				break;
			}
		}

		if( !bNormalColor )
		{
			return false;
		}
	}
	
	unsigned int r,g,b,a;
	r = g = b = a = 255;

	if( startWith(colorStyle, "#") )
	{
		if( colorStyle.size() == 7 )
		{
			// #ffff00
			r = getHexInt(colorStyle, 1, 2);
			g = getHexInt(colorStyle, 3, 2);
			b = getHexInt(colorStyle, 5, 2);
		}
		else if( colorStyle.size() == 4 )
		{
			// #ff0
			r = getHexInt(colorStyle, 1, 1);
			r = r*16+r;
			g = getHexInt(colorStyle, 2, 1);
			g = g*16 + g;
			b = getHexInt(colorStyle, 3, 1);
			b = b*16 + b;
		}
		else
		{
			return false;
		}
	}
	else if( startWith(colorStyle, "rgba") )
	{
		float alpha;
		if( sscanf(colorStyle.c_str(), "rgba(%d,%d,%d,%f)", &r, &g, &b, &alpha) != 4 )
		{
			return false;
		}
		a = alpha * 255;
	}
	else if( startWith(colorStyle, "rgb") )
	{
		if( sscanf(colorStyle.c_str(), "rgb(%d,%d,%d)", &r, &g, &b) != 3 )
		{
			return false;
		}
	}

	if( r > 255 || g > 255 || b > 255 || a > 255 )
	{
		return false;
	}

	// ARGB
	color = a<<24 | r<<16 | g << 8 | b;
	return true;
}

int StringUtil::loadContentsOfURL(const string &url, string &content)
{
    return 0;
}
