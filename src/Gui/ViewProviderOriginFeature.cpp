/***************************************************************************
 *   Copyright (c) 2015 Alexander Golubev (Fat-Zer) <fatzer2@gmail.com>    *
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

#include "PreCompiled.h"

#ifndef _PreComp_
# include <Inventor/nodes/SoAsciiText.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoAnnotation.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoIndexedLineSet.h>
# include <Inventor/nodes/SoTranslation.h>
# include <Inventor/nodes/SoFontStyle.h>
# include <Inventor/nodes/SoScale.h>
# include <Inventor/details/SoLineDetail.h>
#endif

#include <App/Document.h>
#include <App/OriginFeature.h>

#include "SoFCUnifiedSelection.h"
#include "SoFCSelection.h"
#include "Window.h"
#include "ViewProviderOrigin.h"

#include "ViewProviderOriginFeature.h"

using namespace Gui;

PROPERTY_SOURCE(Gui::ViewProviderOriginFeature, Gui::ViewProviderGeometryObject)

ViewProviderOriginFeature::ViewProviderOriginFeature () {
    ADD_PROPERTY_TYPE ( Size, (ViewProviderOrigin::defaultSize()), 0, App::Prop_ReadOnly,
    QT_TRANSLATE_NOOP("App::Property", "Visual size of the feature"));

    ShapeColor.setValue ( 50.f/255, 150.f/255, 250.f/255 ); // Set default color for origin (light-blue)
    BoundingBox.setStatus(App::Property::Hidden, true); // Hide Boundingbox from the user due to it doesn't make sense

    // Create node for scaling the origin
    pScale = new SoScale ();
    pScale->ref ();

    // Create the separator filled by inherited classes
    pOriginFeatureRoot = new SoSeparator();
    pOriginFeatureRoot->ref ();
    pOriginFeatureRoot->renderCaching = SoSeparator::OFF;

    // Create the Label node
    pLabel = new SoAsciiText();
    pLabel->ref();
    pLabel->width.setValue(-1);

    ShadowStyle.setValue(3);
}


ViewProviderOriginFeature::~ViewProviderOriginFeature () {
    pScale->unref ();
    pOriginFeatureRoot->unref ();
    pLabel->unref ();
}

// Separator node that alters OpenGL depth function. Coin3d's SoDepthBuffer
// does not account for user code change of depth function.
class DepthSeparator : public SoSeparator {
    typedef SoSeparator inherited;

public:
    DepthSeparator(int32_t func)
        :func(func)
    {}

    virtual void GLRenderBelowPath(SoGLRenderAction * action) {
        render(action, false);
    }

    virtual void GLRenderInPath(SoGLRenderAction * action) {
        render(action, true);
    }

    void render(SoGLRenderAction *action, bool inpath) {
        Gui::FCDepthFunc guard(func);
        SoState *state = action->getState();
        float t = SoLazyElement::getTransparency(state, 0);
        if (t != 0.0f) {
            state->push();
            float trans = 0.0f;
            SoLazyElement::setTransparency(state, this, 1, &trans, &packer);
        }
        if (inpath)
            inherited::GLRenderInPath(action);
        else
            inherited::GLRenderBelowPath(action);
        if (t != 0.0f)
            state->pop();
    }


private:
    int32_t func;
    SoColorPacker packer;
};

void ViewProviderOriginFeature::attach(App::DocumentObject* pcObject)
{
    ViewProviderGeometryObject::attach(pcObject);

    float defaultSz = ViewProviderOrigin::baseSize();
    float sz = Size.getValue () / defaultSz;

    // Create an external separator
    SoSeparator  *sep = new DepthSeparator(GL_LEQUAL);
    sep->renderCaching = SoSeparator::OFF;

    // Add material from the base class
    sep->addChild(pcShapeMaterial);

    // Bind same material to all part
    SoMaterialBinding* matBinding = new SoMaterialBinding;
    matBinding->value = SoMaterialBinding::OVERALL;
    sep->addChild(matBinding);

    // Bind same material to all part
    SoLightModel * lightmodel = new SoLightModel;
    lightmodel->model = SoLightModel::BASE_COLOR;
    sep->addChild(lightmodel);

    // Scale feature to the given size
    pScale->scaleFactor = SbVec3f (sz, sz, sz);
    // sep->addChild (pScale);
    pOriginFeatureRoot->addChild (pScale);

    // Setup font size
    SoFont *font = new SoFont ();
    font->size.setValue ( defaultSz/10.);

    // Adding font node under SoFCSelection node is crucial for its bounding
    // box rendering to work. Because SoGetBoundingBoxAction is applied on the
    // node itself, instead of a path. Without the font, SoAsciiText will
    // report incorrect bounds.
    //
    // sep->addChild ( font );
    pOriginFeatureRoot->addChild (font);

    // Create the selection node
    SoFCSelection *highlight = new SoFCSelection ();
    highlight->applySettings ();
    if ( !Selectable.getValue() ) {
        highlight->selectionMode = Gui::SoFCSelection::SEL_OFF;
    }
    highlight->objectName    = getObject()->getNameInDocument();
    highlight->documentName  = getObject()->getDocument()->getName();
    highlight->style = SoFCSelection::EMISSIVE_DIFFUSE;

    // Style for normal (visible) lines
    SoDrawStyle* style = new SoDrawStyle ();
    style->lineWidth = 2.0f;
    highlight->addChild ( style );

    // Visible lines
    highlight->addChild ( pOriginFeatureRoot );

    // Hidden features
    auto *hidden = new SoFCPathAnnotation;
    hidden->priority = -1;
    hidden->renderCaching = SoSeparator::OFF;

    // Style for hidden lines
    style = new SoDrawStyle ();
    style->lineWidth = 2.0f;
    style->linePattern.setValue ( 0xF000 ); // (dash-skip-skip-skip)
    hidden->addChild ( style );

    // Hidden lines
    hidden->addChild ( pOriginFeatureRoot );

    highlight->addChild ( hidden );

    sep->addChild ( highlight );

    // Setup the object label as it's text
    pLabel->string.setValue ( SbString ( pcObject->Label.getValue () ) );

    addDisplayMaskMode ( sep, "Base" );
}

void ViewProviderOriginFeature::updateData ( const App::Property* prop ) {
    if (prop == &getObject()->Label) {
        pLabel->string.setValue ( SbString ( getObject()->Label.getValue () ) );
    }
    ViewProviderGeometryObject::updateData(prop);
}

void ViewProviderOriginFeature::onChanged ( const App::Property* prop ) {
    if (prop == &Size) {
        float sz = Size.getValue () / ViewProviderOrigin::baseSize();
        pScale->scaleFactor = SbVec3f (sz, sz, sz);
    }
    ViewProviderGeometryObject::onChanged(prop);
}

std::vector<std::string> ViewProviderOriginFeature::getDisplayModes () const
{
    // add modes
    std::vector<std::string> StrList;
    StrList.push_back("Base");
    return StrList;
}

void ViewProviderOriginFeature::setDisplayMode (const char* ModeName)
{
    if (strcmp(ModeName, "Base") == 0)
        setDisplayMaskMode("Base");
    ViewProviderGeometryObject::setDisplayMode(ModeName);
}

bool ViewProviderOriginFeature::onDelete(const std::vector<std::string> &) {
    App::OriginFeature *feat = static_cast <App::OriginFeature *> ( getObject() );
    // Forbid deletion if there is an origin this feature belongs to

    if ( feat->getOrigin () ) {
        return false;
    } else {
        return true;
    }
}
