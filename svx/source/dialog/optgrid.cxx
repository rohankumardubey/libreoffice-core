/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <svl/intitem.hxx>
#include <svtools/unitconv.hxx>
#include <officecfg/Office/Writer.hxx>

#include <svx/svxids.hrc>
#include <svx/optgrid.hxx>
#include <svx/dlgutil.hxx>

// local functions
static void lcl_GetMinMax(weld::MetricSpinButton const& rField, sal_Int64& nMin, sal_Int64& nMax)
{
    rField.get_range(nMin, nMax, FieldUnit::TWIP);
    nMin = rField.denormalize(nMin);
    nMax = rField.denormalize(nMax);
}

static void lcl_SetMinMax(weld::MetricSpinButton& rField, sal_Int64 nMin, sal_Int64 nMax)
{
    rField.set_range(rField.normalize(nMin), rField.normalize(nMax), FieldUnit::TWIP);
}

SvxOptionsGrid::SvxOptionsGrid() :
    nFldDrawX       ( 100 ),
    nFldDivisionX   ( 0 ),
    nFldDrawY       ( 100 ),
    nFldDivisionY   ( 0 ),
    nFldSnapX       ( 100 ),
    nFldSnapY       ( 100 ),
    bUseGridsnap    ( false ),
    bSynchronize    ( true ),
    bGridVisible    ( false ),
    bEqualGrid      ( true )
{
}

SvxGridItem* SvxGridItem::Clone( SfxItemPool* ) const
{
    return new SvxGridItem( *this );
}

bool SvxGridItem::operator==( const SfxPoolItem& rAttr ) const
{
    assert(SfxPoolItem::operator==(rAttr));

    const SvxGridItem& rItem = static_cast<const SvxGridItem&>(rAttr);

    return (    bUseGridsnap == rItem.bUseGridsnap &&
                bSynchronize == rItem.bSynchronize &&
                bGridVisible == rItem.bGridVisible &&
                bEqualGrid   == rItem.bEqualGrid   &&
                nFldDrawX    == rItem.nFldDrawX    &&
                nFldDivisionX== rItem.nFldDivisionX&&
                nFldDrawY    == rItem.nFldDrawY    &&
                nFldDivisionY== rItem.nFldDivisionY&&
                nFldSnapX    == rItem.nFldSnapX    &&
                nFldSnapY    == rItem.nFldSnapY     );
}

bool  SvxGridItem::GetPresentation
(
    SfxItemPresentation /*ePres*/,
    MapUnit             /*eCoreUnit*/,
    MapUnit             /*ePresUnit*/,
    OUString&           rText, const IntlWrapper&
)   const
{
    rText = "SvxGridItem";
    return true;
}

// TabPage Screen Settings
SvxGridTabPage::SvxGridTabPage(weld::Container* pPage, weld::DialogController* pController, const SfxItemSet& rCoreSet)
    : SfxTabPage(pPage, pController, "svx/ui/optgridpage.ui", "OptGridPage", &rCoreSet)
    , bAttrModified(false)
    , m_xCbxUseGridsnap(m_xBuilder->weld_check_button("usegridsnap"))
    , m_xCbxUseGridsnapImg(m_xBuilder->weld_widget("lockusegridsnap"))
    , m_xCbxGridVisible(m_xBuilder->weld_check_button("gridvisible"))
    , m_xCbxGridVisibleImg(m_xBuilder->weld_widget("lockgridvisible"))
    , m_xMtrFldDrawX(m_xBuilder->weld_metric_spin_button("mtrflddrawx", FieldUnit::CM))
    , m_xMtrFldDrawXImg(m_xBuilder->weld_widget("lockmtrflddrawx"))
    , m_xMtrFldDrawY(m_xBuilder->weld_metric_spin_button("mtrflddrawy", FieldUnit::CM))
    , m_xMtrFldDrawYImg(m_xBuilder->weld_widget("lockmtrflddrawy"))
    , m_xNumFldDivisionX(m_xBuilder->weld_spin_button("numflddivisionx"))
    , m_xNumFldDivisionXImg(m_xBuilder->weld_widget("locknumflddivisionx"))
    , m_xNumFldDivisionY(m_xBuilder->weld_spin_button("numflddivisiony"))
    , m_xNumFldDivisionYImg(m_xBuilder->weld_widget("locknumflddivisiony"))
    , m_xCbxSynchronize(m_xBuilder->weld_check_button("synchronize"))
    , m_xCbxSynchronizeImg(m_xBuilder->weld_widget("locksynchronize"))
    , m_xSnapFrames(m_xBuilder->weld_widget("snapframes"))
    , m_xCbxSnapHelplines(m_xBuilder->weld_check_button("snaphelplines"))
    , m_xCbxSnapBorder(m_xBuilder->weld_check_button("snapborder"))
    , m_xCbxSnapFrame(m_xBuilder->weld_check_button("snapframe"))
    , m_xCbxSnapPoints(m_xBuilder->weld_check_button("snappoints"))
    , m_xMtrFldSnapArea(m_xBuilder->weld_metric_spin_button("mtrfldsnaparea", FieldUnit::PIXEL))
    , m_xCbxOrtho(m_xBuilder->weld_check_button("ortho"))
    , m_xCbxBigOrtho(m_xBuilder->weld_check_button("bigortho"))
    , m_xCbxRotate(m_xBuilder->weld_check_button("rotate"))
    , m_xMtrFldAngle(m_xBuilder->weld_metric_spin_button("mtrfldangle", FieldUnit::DEGREE))
    , m_xMtrFldBezAngle(m_xBuilder->weld_metric_spin_button("mtrfldbezangle", FieldUnit::DEGREE))
{
    // This page requires exchange Support
    SetExchangeSupport();

    // Set Metrics
    FieldUnit eFUnit = GetModuleFieldUnit( rCoreSet );
    sal_Int64 nMin, nMax;

    lcl_GetMinMax(*m_xMtrFldDrawX, nMin, nMax);
    SetFieldUnit( *m_xMtrFldDrawX, eFUnit, true );
    lcl_SetMinMax(*m_xMtrFldDrawX, nMin, nMax);

    lcl_GetMinMax(*m_xMtrFldDrawY, nMin, nMax);
    SetFieldUnit( *m_xMtrFldDrawY, eFUnit, true );
    lcl_SetMinMax(*m_xMtrFldDrawY, nMin, nMax);


    m_xCbxRotate->connect_toggled(LINK(this, SvxGridTabPage, ClickRotateHdl_Impl));
    Link<weld::Toggleable&,void> aLink = LINK(this, SvxGridTabPage, ChangeGridsnapHdl_Impl);
    m_xCbxUseGridsnap->connect_toggled(aLink);
    m_xCbxSynchronize->connect_toggled(aLink);
    m_xCbxGridVisible->connect_toggled(aLink);
    m_xMtrFldDrawX->connect_value_changed(
        LINK( this, SvxGridTabPage, ChangeDrawHdl_Impl ) );
    m_xMtrFldDrawY->connect_value_changed(
        LINK( this, SvxGridTabPage, ChangeDrawHdl_Impl ) );
    m_xNumFldDivisionX->connect_value_changed(
        LINK( this, SvxGridTabPage, ChangeDivisionHdl_Impl ) );
    m_xNumFldDivisionY->connect_value_changed(
        LINK( this, SvxGridTabPage, ChangeDivisionHdl_Impl ) );
}

SvxGridTabPage::~SvxGridTabPage()
{
}

std::unique_ptr<SfxTabPage> SvxGridTabPage::Create(weld::Container* pPage, weld::DialogController* pController, const SfxItemSet& rAttrSet)
{
    return std::make_unique<SvxGridTabPage>(pPage, pController, rAttrSet);
}

OUString SvxGridTabPage::GetAllStrings()
{
    OUString sAllStrings;
    OUString labels[]
        = { "label1",    "label2", "flddrawx",  "flddrawy", "label6", "label7", "label3",
            "divisionx", "label4", "divisiony", "label5",   "label8", "label9" };

    for (const auto& label : labels)
    {
        if (const auto& pString = m_xBuilder->weld_label(label))
            sAllStrings += pString->get_label() + " ";
    }

    OUString checkButton[]
        = { "usegridsnap", "gridvisible", "synchronize", "snaphelplines", "snapborder",
            "snapframe",   "snappoints",  "ortho",       "bigortho",      "rotate" };

    for (const auto& check : checkButton)
    {
        if (const auto& pString = m_xBuilder->weld_check_button(check))
            sAllStrings += pString->get_label() + " ";
    }

    return sAllStrings.replaceAll("_", "");
}

bool SvxGridTabPage::FillItemSet( SfxItemSet* rCoreSet )
{
    if ( bAttrModified )
    {
        SvxGridItem aGridItem( SID_ATTR_GRID_OPTIONS );

        aGridItem.bUseGridsnap  = m_xCbxUseGridsnap->get_active();
        aGridItem.bSynchronize  = m_xCbxSynchronize->get_active();
        aGridItem.bGridVisible  = m_xCbxGridVisible->get_active();

        MapUnit eUnit = rCoreSet->GetPool()->GetMetric( SID_ATTR_GRID_OPTIONS );
        tools::Long nX = GetCoreValue(  *m_xMtrFldDrawX, eUnit );
        tools::Long nY = GetCoreValue( *m_xMtrFldDrawY, eUnit );

        aGridItem.nFldDrawX    = static_cast<sal_uInt32>(nX);
        aGridItem.nFldDrawY    = static_cast<sal_uInt32>(nY);
        aGridItem.nFldDivisionX = static_cast<tools::Long>(m_xNumFldDivisionX->get_value() - 1);
        aGridItem.nFldDivisionY = static_cast<tools::Long>(m_xNumFldDivisionY->get_value() - 1);

        rCoreSet->Put( aGridItem );
    }
    return bAttrModified;
}

void SvxGridTabPage::Reset( const SfxItemSet* rSet )
{
    const SvxGridItem* pGridAttr = nullptr;

    if( (pGridAttr = rSet->GetItemIfSet( SID_ATTR_GRID_OPTIONS , false )) )
    {
        m_xCbxUseGridsnap->set_active(pGridAttr->bUseGridsnap);
        m_xCbxUseGridsnap->set_sensitive(!officecfg::Office::Writer::Grid::Option::SnapToGrid::isReadOnly());
        m_xCbxUseGridsnapImg->set_visible(officecfg::Office::Writer::Grid::Option::SnapToGrid::isReadOnly());

        m_xCbxSynchronize->set_active(pGridAttr->bSynchronize);
        m_xCbxSynchronize->set_sensitive(!officecfg::Office::Writer::Grid::Option::Synchronize::isReadOnly());
        m_xCbxSynchronizeImg->set_visible(officecfg::Office::Writer::Grid::Option::Synchronize::isReadOnly());

        m_xCbxGridVisible->set_active(pGridAttr->bGridVisible);
        m_xCbxGridVisible->set_sensitive(!officecfg::Office::Writer::Grid::Option::VisibleGrid::isReadOnly());
        m_xCbxGridVisibleImg->set_visible(officecfg::Office::Writer::Grid::Option::VisibleGrid::isReadOnly());

        MapUnit eUnit = rSet->GetPool()->GetMetric( SID_ATTR_GRID_OPTIONS );
        SetMetricValue( *m_xMtrFldDrawX , pGridAttr->nFldDrawX, eUnit );
        SetMetricValue( *m_xMtrFldDrawY , pGridAttr->nFldDrawY, eUnit );

        m_xMtrFldDrawX->set_sensitive(!officecfg::Office::Writer::Grid::Resolution::XAxis::isReadOnly());
        m_xMtrFldDrawXImg->set_visible(officecfg::Office::Writer::Grid::Resolution::XAxis::isReadOnly());

        m_xMtrFldDrawY->set_sensitive(!officecfg::Office::Writer::Grid::Resolution::YAxis::isReadOnly());
        m_xMtrFldDrawYImg->set_visible(officecfg::Office::Writer::Grid::Resolution::YAxis::isReadOnly());

        m_xNumFldDivisionX->set_value(pGridAttr->nFldDivisionX + 1);
        m_xNumFldDivisionY->set_value(pGridAttr->nFldDivisionY + 1);

        m_xNumFldDivisionX->set_sensitive(!officecfg::Office::Writer::Grid::Subdivision::XAxis::isReadOnly());
        m_xNumFldDivisionXImg->set_visible(officecfg::Office::Writer::Grid::Subdivision::XAxis::isReadOnly());

        m_xNumFldDivisionY->set_sensitive(!officecfg::Office::Writer::Grid::Subdivision::YAxis::isReadOnly());
        m_xNumFldDivisionYImg->set_visible(officecfg::Office::Writer::Grid::Subdivision::YAxis::isReadOnly());
    }

    ChangeGridsnapHdl_Impl(*m_xCbxUseGridsnap);
    bAttrModified = false;
}

void SvxGridTabPage::ActivatePage( const SfxItemSet& rSet )
{
    const SvxGridItem* pGridAttr = nullptr;
    if( (pGridAttr = rSet.GetItemIfSet( SID_ATTR_GRID_OPTIONS , false )) )
    {
        m_xCbxUseGridsnap->set_active(pGridAttr->bUseGridsnap);

        ChangeGridsnapHdl_Impl(*m_xCbxUseGridsnap);
    }

    // Metric Change if necessary (as TabPage is in the dialog, where the
    // metric can be set
    const SfxUInt16Item* pItem = rSet.GetItemIfSet( SID_ATTR_METRIC , false );
    if( !pItem )
        return;


    FieldUnit eFUnit = static_cast<FieldUnit>(static_cast<tools::Long>(pItem->GetValue()));

    if (eFUnit == m_xMtrFldDrawX->get_unit())
        return;

    // Set Metrics
    sal_Int64 nMin, nMax;
    int nVal = m_xMtrFldDrawX->denormalize(m_xMtrFldDrawX->get_value(FieldUnit::TWIP));

    lcl_GetMinMax(*m_xMtrFldDrawX, nMin, nMax);
    SetFieldUnit(*m_xMtrFldDrawX, eFUnit, true);
    lcl_SetMinMax(*m_xMtrFldDrawX, nMin, nMax);

    m_xMtrFldDrawX->set_value(m_xMtrFldDrawX->normalize(nVal), FieldUnit::TWIP);

    nVal = m_xMtrFldDrawY->denormalize(m_xMtrFldDrawY->get_value(FieldUnit::TWIP));
    lcl_GetMinMax(*m_xMtrFldDrawY, nMin, nMax);
    SetFieldUnit(*m_xMtrFldDrawY, eFUnit, true);
    lcl_SetMinMax(*m_xMtrFldDrawY, nMin, nMax);
    m_xMtrFldDrawY->set_value(m_xMtrFldDrawY->normalize(nVal), FieldUnit::TWIP);
}

DeactivateRC SvxGridTabPage::DeactivatePage( SfxItemSet* _pSet )
{
    if ( _pSet )
        FillItemSet( _pSet );
    return DeactivateRC::LeavePage;
}

IMPL_LINK(SvxGridTabPage, ChangeDrawHdl_Impl, weld::MetricSpinButton&, rField, void)
{
    bAttrModified = true;
    if (m_xCbxSynchronize->get_active())
    {
        if (&rField == m_xMtrFldDrawX.get())
            m_xMtrFldDrawY->set_value(m_xMtrFldDrawX->get_value(FieldUnit::NONE), FieldUnit::NONE);
        else
            m_xMtrFldDrawX->set_value(m_xMtrFldDrawY->get_value(FieldUnit::NONE), FieldUnit::NONE);
    }
}

IMPL_LINK_NOARG(SvxGridTabPage, ClickRotateHdl_Impl, weld::Toggleable&, void)
{
    if (m_xCbxRotate->get_active())
        m_xMtrFldAngle->set_sensitive(true);
    else
        m_xMtrFldAngle->set_sensitive(false);
}

IMPL_LINK(SvxGridTabPage, ChangeDivisionHdl_Impl, weld::SpinButton&, rField, void)
{
    bAttrModified = true;
    if (m_xCbxSynchronize->get_active())
    {
        if (m_xNumFldDivisionX.get() == &rField)
            m_xNumFldDivisionY->set_value(m_xNumFldDivisionX->get_value());
        else
            m_xNumFldDivisionX->set_value(m_xNumFldDivisionY->get_value());
    }
}

IMPL_LINK_NOARG(SvxGridTabPage, ChangeGridsnapHdl_Impl, weld::Toggleable&, void)
{
    bAttrModified = true;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
