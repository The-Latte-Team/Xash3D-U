/*
utflib.c - small unicode conversion library
Copyright (C) 2024 Alibek Omarov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#include "utflib.h"
#include "xash3d_types.h"

uint32_t Q_DecodeUTF8( utfstate_t *s, uint32_t in )
{
	// get character length
	if( s->len == 0 )
	{
		// init state
		s->uc = 0;

		// expect ASCII symbols by default
		if( likely( in <= 0x7fu ))
			return in;

		// invalid sequence
		if( unlikely( in >= 0xf8u ))
			return 0;

		s->k = 0;

		if( in >= 0xf0u )
		{
			s->uc = in & 0x07u;
			s->len = 3;
		}
		else if( in >= 0xe0u )
		{
			s->uc = in & 0x0fu;
			s->len = 2;
		}
		else if( in >= 0xc0u )
		{
			s->uc = in & 0x1fu;
			s->len = 1;
		}

		return 0;
	}

	// invalid sequence, reset
	if( unlikely( in > 0xbfu ))
	{
		s->len = 0;
		return 0;
	}

	s->uc <<= 6;
	s->uc += in & 0x3fu;
	s->k++;

	// sequence complete, reset and return code point
	if( likely( s->k == s->len ))
	{
		s->len = 0;
		return s->uc;
	}

	// feed more characters
	return 0;
}

uint32_t Q_DecodeUTF16( utfstate_t *s, uint32_t in )
{
	// get character length
	if( s->len == 0 )
	{
		// init state
		s->uc = 0;

		// expect simple case, after all decoding UTF-16 must be easy
		if( likely( in < 0xd800u || in > 0xdfffu ))
			return in;

		s->uc = (( in - 0xd800u ) << 10 ) + 0x10000u;
		s->len = 1;
		s->k = 0;

		return 0;
	}

	// invalid sequence, reset
	if( unlikely( in < 0xdc00u || in > 0xdfffu ))
	{
		s->len = 0;
		return 0;
	}

	s->uc |= in - 0xdc00u;
	s->k++;

	// sequence complete, reset and return code point
	if( likely( s->k == s->len ))
	{
		s->len = 0;
		return s->uc;
	}

	// feed more characters (should never happen with UTF-16)
	return 0;
}

size_t Q_EncodeUTF8( char dst[4], uint32_t ch )
{
	if( ch <= 0x7fu )
	{
		dst[0] = ch;
		return 1;
	}
	else if( ch <= 0x7ffu )
	{
		dst[0] = 0xc0u | (( ch >> 6 ) & 0x1fu );
		dst[1] = 0x80u | (( ch ) & 0x3fu );
		return 2;
	}
	else if( ch <= 0xffffu )
	{
		dst[0] = 0xe0u | (( ch >> 12 ) & 0x0fu );
		dst[1] = 0x80u | (( ch >> 6 ) & 0x3fu );
		dst[2] = 0x80u | (( ch ) & 0x3fu );
		return 3;
	}

	dst[0] = 0xf0u | (( ch >> 18 ) & 0x07u );
	dst[1] = 0x80u | (( ch >> 12 ) & 0x3fu );
	dst[2] = 0x80u | (( ch >> 6 ) & 0x3fu );
	dst[3] = 0x80u | (( ch ) & 0x3fu );
	return 4;
}

size_t Q_UTF8Length( const char *s )
{
	size_t len = 0;
	utfstate_t state = { 0 };

	if( !s )
		return 0;

	for( ; *s; s++ )
	{
		uint32_t ch = Q_DecodeUTF8( &state, (uint32_t)*s );

		if( ch == 0 )
			continue;

		len++;
	}

	return len;
}

static size_t Q_CodepointLength( uint32_t ch )
{
	if( ch <= 0x7fu )
		return 1;
	else if( ch <= 0x7ffu )
		return 2;
	else if( ch <= 0xffffu )
		return 3;

	return 4;
}

size_t Q_UTF16ToUTF8( char *dst, size_t dstsize, const uint16_t *src, size_t srcsize )
{
	utfstate_t state = { 0 };
	size_t dsti = 0, srci;

	if( !dst || !src || !dstsize || !srcsize )
		return 0;

	for( srci = 0; srci < srcsize && src[srci]; srci++ )
	{
		uint32_t ch;
		size_t len;

		ch = Q_DecodeUTF16( &state, src[srci] );

		if( ch == 0 )
			continue;

		len = Q_CodepointLength( ch );

		if( dsti + len + 1 > dstsize )
			break;

		dsti += Q_EncodeUTF8( &dst[dsti], ch );
	}

	dst[dsti] = 0;

	return dsti;
}
