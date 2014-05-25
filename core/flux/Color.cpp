/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ColorNames.h"
#include "Format.h"
#include "Color.h"

namespace flux
{

Color Color::parse(const char *s, bool *ok)
{
	bool localOk;
	if (!ok) ok = &localOk;
	if (!s[0]) {
		*ok = false;
		return Color();
	}
	if (s[0] == '#') {
		struct H {
			static inline int dehex(char ch, bool *ok) {
				if ('0' <= ch && ch <= '9') return ch - '0';
				else if ('a' <= ch && ch <= 'f') return ch - 'a';
				else if ('A' <= ch && ch <= 'F') return ch - 'A';
				*ok = false;
				return -1;
			}
		};
		Color c;
		int n = 1;
		while (s[n] && n < 11) ++n;
		if (n == 7) { // #RRGGBB
			*ok = true;
			int rh = H::dehex(s[1], ok);
			int rl = H::dehex(s[2], ok);
			int gh = H::dehex(s[3], ok);
			int gl = H::dehex(s[4], ok);
			int bh = H::dehex(s[5], ok);
			int bl = H::dehex(s[6], ok);
			if (!*ok) return Color();
			return Color(
				(rh << 8) | rl,
				(gh << 8) | gl,
				(bh << 8) | bl
			);
		}
		else if (n == 9) { // #RRGGBBAA
			*ok = true;
			int rh = H::dehex(s[1], ok);
			int rl = H::dehex(s[2], ok);
			int gh = H::dehex(s[3], ok);
			int gl = H::dehex(s[4], ok);
			int bh = H::dehex(s[5], ok);
			int bl = H::dehex(s[6], ok);
			int ah = H::dehex(s[7], ok);
			int al = H::dehex(s[8], ok);
			if (!*ok) return Color();
			return Color(
				(rh << 8) | rl,
				(gh << 8) | gl,
				(bh << 8) | bl,
				(ah << 8) | al
			);
		}
		else if (n == 4) { // #RGB
			*ok = true;
			int rh = H::dehex(s[1], ok);
			int gh = H::dehex(s[2], ok);
			int bh = H::dehex(s[3], ok);
			if (!*ok) return Color();
			return Color(
				(rh << 8) | rh,
				(gh << 8) | gh,
				(bh << 8) | bh
			);
		}
		else if (n == 5) { // #RGBA
			*ok = true;
			int rh = H::dehex(s[1], ok);
			int gh = H::dehex(s[2], ok);
			int bh = H::dehex(s[3], ok);
			int ah = H::dehex(s[4], ok);
			if (!*ok) return Color();
			return Color(
				(rh << 8) | rh,
				(gh << 8) | gh,
				(bh << 8) | bh,
				(ah << 8) | ah
			);
		}
		*ok = false;
		return Color();
	}
	Color c;
	if (colorNames()->lookup(s, &c)) {
		*ok = true;
		return c;
	}
	*ok = false;
	return Color();
}

String str(Color c)
{
	Format f;
	f << "#" << hex(c.r(), 2) << hex(c.g(), 2) << hex(c.b(), 2);
	if (c.a() != 0xff) f << hex(c.a(), 2);
	return f;
}

} // namespace flux
