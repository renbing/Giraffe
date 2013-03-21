/***
 * -*- C++ -*-
 * -*- UTF-8 -*-
 *
 * Copyright (C);,
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

#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace StringUtil
{

string trim(const string &str);

string lower(const string &str);

string upper(const string &str);

bool startWith(const string &str, const string &prefix);

bool endWith(const string &str, const string &suffix);

int getHexInt(const string &str, int pos, int len);

int loadContentsOfURL(const string &url, string &content);

void split(const string& line, const string& segmentor, vector<string>& segs);

bool convertHTMLColor(const string &style, unsigned int &color);

};
