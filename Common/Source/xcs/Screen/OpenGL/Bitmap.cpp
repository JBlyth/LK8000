/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2015 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include <algorithm>

#include "Screen/Bitmap.hpp"
#include "Screen/Debug.hpp"
#include "Screen/Custom/UncompressedImage.hpp"
#include "UncompressedImage.hpp"
#include "Texture.hpp"
#include "Debug.hpp"

Bitmap::Bitmap(Bitmap &&src) :
   texture(std::exchange(src.texture, nullptr)),
   size(src.size),
   interpolation(src.interpolation),
   flipped(src.flipped)
{
}

Bitmap& Bitmap::operator=(Bitmap &&src)
{
  std::swap(texture, src.texture);
  std::swap(size, src.size);
  std::swap(interpolation, src.interpolation);
  std::swap(flipped, src.flipped);
  
  return *this;
}

void
Bitmap::EnableInterpolation()
{
  interpolation = true;
  if (texture != nullptr) {
    texture->Bind();
    texture->EnableInterpolation();
  }
}

#ifndef ANDROID

bool
Bitmap::Load(const UncompressedImage &uncompressed, gcc_unused Type type)
{
  delete texture;
  texture = type == Type::MONO
    ? ImportAlphaTexture(uncompressed)
    : ImportTexture(uncompressed);
  if (texture == nullptr)
    return false;

  if (interpolation)
    texture->EnableInterpolation();

  size = { uncompressed.GetWidth(), uncompressed.GetHeight() };
  return true;
}
#endif /* !ANDROID */

void
Bitmap::Reset()
{
  assert(!IsDefined() || IsScreenInitialized());
  assert(!IsDefined() || pthread_equal(pthread_self(), OpenGL::thread));

  delete texture;
  texture = nullptr;
}

const PixelSize
Bitmap::GetSize() const
{
  assert(IsDefined());

  return size;
}
