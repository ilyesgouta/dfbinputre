// DFbInputRe, a tool for redirecting the local mouse and keyboard input
// to a remote target over the network using UDP datagrams.
//
// Copyright (C) 2012, Ilyes Gouta, ilyes.gouta@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DFBINPUTRE_H
#define DFBINPUTRE_H

#include <stdint.h>

#define MAGIC 0xc001feed

typedef enum {
    DFBINPUT_MOUSE_MOTION = 1,
    DFBINPUT_MOUSE_CLICK,
    DFBINPUT_MOUSE_DBLCLICK,
    DFBINPUT_KEYPRESS
} DFbInputType;

typedef struct DFbInputPacket {
    uint32_t magic;
    uint32_t type;
    uint32_t keyascii;
    uint32_t cursor[2];
} DFbInputPacket __attribute__((packed));

#endif // DFBINPUTRE_H
