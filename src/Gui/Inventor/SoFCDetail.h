/****************************************************************************
 *   Copyright (c) 2020 Zheng, Lei (realthunder) <realthunder.dev@gmail.com>*
 *                                                                          *
 *   This file is part of the FreeCAD CAx development system.               *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Library General Public            *
 *   License as published by the Free Software Foundation; either           *
 *   version 2 of the License, or (at your option) any later version.       *
 *                                                                          *
 *   This library  is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Library General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU Library General Public      *
 *   License along with this library; see the file COPYING.LIB. If not,     *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,          *
 *   Suite 330, Boston, MA  02111-1307, USA                                 *
 *                                                                          *
 ****************************************************************************/

#ifndef FC_SOFCDETAIL_H
#define FC_SOFCDETAIL_H

#include <set>
#include <array>
#include <Inventor/details/SoSubDetail.h>
#include <Inventor/details/SoDetail.h>

class GuiExport SoFCDetail : public SoDetail
{
  SO_DETAIL_HEADER(SoFCDetail);

  public:
  SoFCDetail(void);
  virtual ~SoFCDetail();

  static void initClass(void);
  virtual SoDetail * copy(void) const;

  enum Type {
    Vertex,
    Edge,
    Face,
    TypeMax,
  };
  const std::set<int> &getIndices(Type type) const;
  void setIndices(Type type, std::set<int> &&indices);
  bool addIndex(Type type, int index);
  bool removeIndex(Type type, int index);

  private:
  std::array<std::set<int>, TypeMax> indexArray;
};

#endif // FC_SOFCDETAIL_H
// vim: noai:ts=2:sw=2
