// Author: Deci | Project: OmniCardputer | Name: Util.h
// Date: 2025/05/14 @ 21:05:47

#pragma once

inline int char_to_int(const char c)
{
	return (int) c - 48;
}

inline void clear(String& sz, char v)
{
	for (int i = 0; i < sz.length(); i++) {
		// sz[i] = v;
		sz.setCharAt(i, v);
	}
}