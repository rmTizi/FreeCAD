/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef PARTGUI_SOBREPPOINTSET_H
#define PARTGUI_SOBREPPOINTSET_H

#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSubField.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoReplacedElement.h>
#include <vector>
#include <memory>
#include <Gui/SoFCSelectionContext.h>
#include "BoundBoxRayPick.h"

class SoCoordinateElement;
class SoGLCoordinateElement;
class SoTextureCoordinateBundle;

namespace PartGui {

class PartGuiExport SoBrepPointSet : public SoPointSet {
    typedef SoPointSet inherited;

    SO_NODE_HEADER(SoBrepPointSet);

public:
    SoMFInt32 highlightIndices;
    SoSFColor highlightColor;
    SoSFBool  elementSelectable;

    static void initClass();
    SoBrepPointSet();

    void setSiblings(std::vector<SoNode*> &&);

protected:
    virtual ~SoBrepPointSet() {};
    virtual void GLRender(SoGLRenderAction *action);
    virtual void GLRenderInPath(SoGLRenderAction *action);
    virtual void GLRenderBelowPath(SoGLRenderAction * action);
    virtual void doAction(SoAction* action); 
    virtual void rayPick(SoRayPickAction *action);

    virtual void getBoundingBox(SoGetBoundingBoxAction * action);

private:
    typedef Gui::SoFCSelectionContextEx SelContext;
    typedef Gui::SoFCSelectionContextExPtr SelContextPtr;
    void glRender(SoGLRenderAction *action, bool inpath);
    void renderHighlight(SoGLRenderAction *action, SelContextPtr);
    void renderSelection(SoGLRenderAction *action, SelContextPtr, bool push=true);
    void _renderSelection(SoGLRenderAction *action, bool checkColor, SbColor color, bool push);

    bool isSelected(SelContextPtr ctx);
    void initBoundingBoxes(const SbVec3f *coords, int numverts);

private:
    SelContextPtr selContext;
    SelContextPtr selContext2;
    Gui::SoFCSelectionCounter selCounter;
    std::vector<SoNode*> siblings;

    uint64_t coordNodeId = 0;
    BoundBoxRayPick bboxPicker;
    std::vector<int> bboxMap;
};

} // namespace PartGui


#endif // PARTGUI_SOBREPPOINTSET_H

