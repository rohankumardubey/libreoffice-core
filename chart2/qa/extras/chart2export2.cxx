/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "charttest.hxx"

#include <com/sun/star/chart2/XDataPointCustomLabelField.hpp>
#include <com/sun/star/chart2/DataPointCustomLabelFieldType.hpp>
#include <com/sun/star/chart2/RelativePosition.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/chart2/DataPointLabel.hpp>
#include <com/sun/star/chart/DataLabelPlacement.hpp>
#include <com/sun/star/graphic/XGraphic.hpp>

using uno::Reference;
using beans::XPropertySet;

class Chart2ExportTest2 : public ChartTest
{
public:
    Chart2ExportTest2()
        : ChartTest("/chart2/qa/extras/data/")
    {
    }
};

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testSetSeriesToSecondaryAxisXLSX)
{
    loadFromURL(u"xlsx/add_series_secondary_axis.xlsx");
    Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
    // Second series
    Reference<chart2::XDataSeries> xSeries = getDataSeriesFromDoc(xChartDoc, 1);
    CPPUNIT_ASSERT(xSeries.is());

    Reference<beans::XPropertySet> xPropSet(xSeries, uno::UNO_QUERY_THROW);
    sal_Int32 AxisIndex = 1;
    // Attach the second series to the secondary axis. (The third series is already attached.)
    xPropSet->setPropertyValue("AttachedAxisIndex", uno::Any(AxisIndex));

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    // Check there are only two <lineChart> tag in the XML, one for the primary and one for the secondary axis.
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:lineChart"_ostr, 2);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCombinedChartSecondaryAxisXLSX)
{
    // Original file was created with MS Office
    loadFromURL(u"xlsx/combined_chart_secondary_axis.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    // Collect barchart axID on secondary Axis
    OUString XValueIdOfBarchart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:axId[1]"_ostr, "val"_ostr);
    OUString YValueIdOfBarchart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:axId[2]"_ostr, "val"_ostr);
    // Collect linechart axID on primary Axis
    OUString XValueIdOfLinechart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:axId[1]"_ostr, "val"_ostr);
    OUString YValueIdOfLinechart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:axId[2]"_ostr, "val"_ostr);
    // Check which c:catAx and c:valAx contain the AxisId of charttypes
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx[1]/c:axId"_ostr, "val"_ostr,
                XValueIdOfLinechart);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx[1]/c:axId"_ostr, "val"_ostr,
                YValueIdOfLinechart);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx[2]/c:axId"_ostr, "val"_ostr,
                XValueIdOfBarchart);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx[2]/c:axId"_ostr, "val"_ostr,
                YValueIdOfBarchart);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCombinedChartSecondaryAxisODS)
{
    // Original file was created with LibreOffice
    loadFromURL(u"ods/combined_chart_secondary_axis.ods");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    // Collect barchart axID on secondary Axis
    OUString XValueIdOfBarchart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:axId[1]"_ostr, "val"_ostr);
    OUString YValueIdOfBarchart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:axId[2]"_ostr, "val"_ostr);
    // Collect linechart axID on primary Axis
    OUString XValueIdOfLinechart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:axId[1]"_ostr, "val"_ostr);
    OUString YValueIdOfLinechart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:axId[2]"_ostr, "val"_ostr);
    // Check which c:catAx and c:valAx contain the AxisId of charttypes
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx[1]/c:axId"_ostr, "val"_ostr,
                XValueIdOfLinechart);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx[1]/c:axId"_ostr, "val"_ostr,
                YValueIdOfLinechart);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx[2]/c:axId"_ostr, "val"_ostr,
                XValueIdOfBarchart);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx[2]/c:axId"_ostr, "val"_ostr,
                YValueIdOfBarchart);
    // do not need CT_crosses tag if the actual axis is deleted, so we need to make sure it is not saved
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx[2]/c:crosses"_ostr, 0);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCrossBetweenXLSX)
{
    // FIXME: validation error in OOXML export: Errors: 1
    skipValidation();

    // Original files were created with MS Office
    {
        loadFromURL(u"xlsx/tdf127777.xlsx");
        save("Calc Office Open XML");
        xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
        CPPUNIT_ASSERT(pXmlDoc);
        assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx/c:crossBetween"_ostr,
                    "val"_ostr, "between");
    }
    {
        loadFromURL(u"xlsx/tdf132076.xlsx");
        save("Calc Office Open XML");
        xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
        CPPUNIT_ASSERT(pXmlDoc);
        assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx/c:crossBetween"_ostr,
                    "val"_ostr, "between");
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCrossBetweenWithDeletedAxis)
{
    // Original file was created with MS Office (the category axis is deleted in the file)
    loadFromURL(u"xlsx/tdf128633.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx/c:crossBetween"_ostr, "val"_ostr,
                "between");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCrossBetweenODS)
{
    // Original file was created with LibreOffice
    loadFromURL(u"ods/test_CrossBetween.ods");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx/c:crossBetween"_ostr, "val"_ostr,
                "between");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testAxisTitleRotationXLSX)
{
    loadFromURL(u"xlsx/axis_title_rotation.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:valAx/c:title/c:tx/c:rich/a:bodyPr"_ostr,
                "rot"_ostr, "0");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testAxisTitlePositionDOCX)
{
    loadFromURL(u"docx/testAxisTitlePosition.docx");
    save("Office Open XML Text");
    xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // test X Axis title position
    OUString aXVal = getXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:catAx/c:title/c:layout/c:manualLayout/c:x"_ostr,
        "val"_ostr);
    double nX = aXVal.toDouble();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.698208543867708, nX, 1e-3);
    OUString aYVal = getXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:catAx/c:title/c:layout/c:manualLayout/c:y"_ostr,
        "val"_ostr);
    double nY = aYVal.toDouble();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.805152435594555, nY, 1e-3);

    // test Y Axis title position
    aXVal = getXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:valAx/c:title/c:layout/c:manualLayout/c:x"_ostr,
        "val"_ostr);
    nX = aXVal.toDouble();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0253953671500755, nX, 1e-3);
    aYVal = getXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:valAx/c:title/c:layout/c:manualLayout/c:y"_ostr,
        "val"_ostr);
    nY = aYVal.toDouble();
    // just test the first two decimal digits because it is not perfect in docx yet.
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.384070199122511, nY, 1e-2);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testAxisCrossBetweenDOCX)
{
    loadFromURL(u"odt/axis-position.odt");

    // FIXME: validation error in OOXML export: Errors: 3
    skipValidation();

    save("Office Open XML Text");
    xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
    assertXPath(pXmlDoc, "(//c:crossBetween)[1]"_ostr, "val"_ostr, "midCat");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testPieChartDataPointExplosionXLSX)
{
    loadFromURL(u"xlsx/pie_chart_datapoint_explosion.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dPt/c:explosion"_ostr,
                "val"_ostr, "28");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCustomDataLabel)
{
    loadFromURL(u"pptx/tdf115107.pptx");

    Reference<chart2::XChartDocument> xChartDoc(getChartDocFromDrawImpress(0, 0), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc.is());

    uno::Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
    CPPUNIT_ASSERT(xDataSeries.is());
    float nFontSize;
    sal_Int64 nFontColor;
    sal_Int32 nCharUnderline;
    uno::Reference<beans::XPropertySet> xPropertySet;
    uno::Sequence<uno::Reference<chart2::XDataPointCustomLabelField>> aFields;

    // 1
    xPropertySet.set(xDataSeries->getDataPointByIndex(0), uno::UNO_SET_THROW);
    xPropertySet->getPropertyValue("CustomLabelFields") >>= aFields;
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(2), aFields.getLength());

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_TEXT,
                         aFields[0]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("90.0 = "), aFields[0]->getString());
    aFields[0]->getPropertyValue("CharHeight") >>= nFontSize;
    aFields[0]->getPropertyValue("CharColor") >>= nFontColor;
    CPPUNIT_ASSERT_EQUAL(static_cast<float>(18), nFontSize);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int64>(0xed7d31), nFontColor);

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_VALUE,
                         aFields[1]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("90"), aFields[1]->getString());
    CPPUNIT_ASSERT_EQUAL(OUString("{0C576297-5A9F-4B4E-A675-B6BA406B7D87}"), aFields[1]->getGuid());

    // 2
    xPropertySet.set(xDataSeries->getDataPointByIndex(1), uno::UNO_SET_THROW);
    xPropertySet->getPropertyValue("CustomLabelFields") >>= aFields;
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(8), aFields.getLength());

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_TEXT,
                         aFields[0]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("Text"), aFields[0]->getString());

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_TEXT,
                         aFields[1]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString(" : "), aFields[1]->getString());

    CPPUNIT_ASSERT_EQUAL(
        chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_CATEGORYNAME,
        aFields[2]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("B"), aFields[2]->getString());
    CPPUNIT_ASSERT_EQUAL(OUString("{0CCAAACD-B393-42CE-8DBD-82F9F9ADC852}"), aFields[2]->getGuid());
    aFields[2]->getPropertyValue("CharHeight") >>= nFontSize;
    aFields[2]->getPropertyValue("CharColor") >>= nFontColor;
    CPPUNIT_ASSERT_EQUAL(static_cast<float>(16), nFontSize);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int64>(0xed7d31), nFontColor);

    CPPUNIT_ASSERT_EQUAL(
        chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_NEWLINE,
        aFields[3]->getFieldType());

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_TEXT,
                         aFields[4]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("Multi"), aFields[4]->getString());

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_TEXT,
                         aFields[5]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("line"), aFields[5]->getString());
    aFields[5]->getPropertyValue("CharHeight") >>= nFontSize;
    aFields[5]->getPropertyValue("CharColor") >>= nFontColor;
    CPPUNIT_ASSERT_EQUAL(static_cast<float>(11.97), nFontSize);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int64>(0xbf9000), nFontColor);

    CPPUNIT_ASSERT_EQUAL(
        chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_NEWLINE,
        aFields[6]->getFieldType());

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_TEXT,
                         aFields[7]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("Abc"), aFields[7]->getString());
    aFields[7]->getPropertyValue("CharHeight") >>= nFontSize;
    aFields[7]->getPropertyValue("CharColor") >>= nFontColor;
    aFields[7]->getPropertyValue("CharUnderline") >>= nCharUnderline;
    CPPUNIT_ASSERT_EQUAL(static_cast<float>(12), nFontSize);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int64>(0xa9d18e), nFontColor);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(1), nCharUnderline);

    // 3
    xPropertySet.set(xDataSeries->getDataPointByIndex(2), uno::UNO_SET_THROW);
    xPropertySet->getPropertyValue("CustomLabelFields") >>= aFields;
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(1), aFields.getLength());

    CPPUNIT_ASSERT_EQUAL(
        chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_SERIESNAME,
        aFields[0]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("DATA"), aFields[0]->getString());
    CPPUNIT_ASSERT_EQUAL(OUString("{C8F3EB90-8960-4F9A-A3AD-B4FAC4FE4566}"), aFields[0]->getGuid());

    // 4
    xPropertySet.set(xDataSeries->getDataPointByIndex(3), uno::UNO_SET_THROW);
    xPropertySet->getPropertyValue("CustomLabelFields") >>= aFields;
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(2), aFields.getLength());

    CPPUNIT_ASSERT_EQUAL(
        chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_CELLREF,
        aFields[0]->getFieldType());
    //CPPUNIT_ASSERT_EQUAL(OUString("70"), aFields[0]->getString()); TODO: Not implemented yet

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_TEXT,
                         aFields[1]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString(" <CELLREF"), aFields[1]->getString());

    save("Impress MS PowerPoint 2007 XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    // Check the data labels font color for the complete data series
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:txPr/a:p/a:pPr/"
                "a:defRPr/a:solidFill/a:srgbClr"_ostr,
                "val"_ostr, "404040");
}

/// Test for tdf#94235
CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testDataSeriesName)
{
    // ODF
    {
        loadFromURL(u"ods/ser_labels.ods");
        saveAndReload("calc8");
        uno::Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
        uno::Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
        CPPUNIT_ASSERT(xDataSeries.is());
        uno::Reference<beans::XPropertySet> xPropertySet;
        chart2::DataPointLabel aDataPointLabel;
        xPropertySet.set(xDataSeries->getDataPointByIndex(0), uno::UNO_SET_THROW);
        xPropertySet->getPropertyValue("Label") >>= aDataPointLabel;
        CPPUNIT_ASSERT_EQUAL(sal_True, aDataPointLabel.ShowSeriesName);
    }

    // OOXML
    {
        loadFromURL(u"xlsx/ser_labels.xlsx");
        saveAndReload("Calc Office Open XML");
        uno::Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
        uno::Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
        CPPUNIT_ASSERT(xDataSeries.is());
        uno::Reference<beans::XPropertySet> xPropertySet;
        chart2::DataPointLabel aDataPointLabel;
        xPropertySet.set(xDataSeries->getDataPointByIndex(0), uno::UNO_SET_THROW);
        xPropertySet->getPropertyValue("Label") >>= aDataPointLabel;
        CPPUNIT_ASSERT_EQUAL(sal_True, aDataPointLabel.ShowSeriesName);
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCustomPositionofDataLabel)
{
    // FIXME: validation error in OOXML export: Errors: 1
    skipValidation();

    loadFromURL(u"xlsx/testCustomPosDataLabels.xlsx");
    {
        save("Calc Office Open XML");
        xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
        CPPUNIT_ASSERT(pXmlDoc);

        // test custom position of data label (xlsx)
        assertXPath(
            pXmlDoc,
            "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser/c:dLbls/c:dLbl[1]/c:idx"_ostr,
            "val"_ostr, "2");
        OUString aXVal = getXPath(pXmlDoc,
                                  "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser/c:dLbls/"
                                  "c:dLbl[1]/c:layout/c:manualLayout/c:x"_ostr,
                                  "val"_ostr);
        double nX = aXVal.toDouble();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.11027682973075476, nX, 1e-7);

        OUString aYVal = getXPath(pXmlDoc,
                                  "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser/c:dLbls/"
                                  "c:dLbl[1]/c:layout/c:manualLayout/c:y"_ostr,
                                  "val"_ostr);
        double nY = aYVal.toDouble();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0742140311063737, nY, 1e-7);
    }

    loadFromURL(u"docx/testTdf108110.docx");
    {
        save("Office Open XML Text");
        xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
        CPPUNIT_ASSERT(pXmlDoc);

        // test custom position of data label (docx)
        assertXPath(
            pXmlDoc,
            "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser/c:dLbls/c:dLbl[2]/c:idx"_ostr,
            "val"_ostr, "2");
        OUString aXVal = getXPath(pXmlDoc,
                                  "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser/c:dLbls/"
                                  "c:dLbl[2]/c:layout/c:manualLayout/c:x"_ostr,
                                  "val"_ostr);
        double nX = aXVal.toDouble();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0227256488772236, nX, 1e-7);

        OUString aYVal = getXPath(pXmlDoc,
                                  "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser/c:dLbls/"
                                  "c:dLbl[2]/c:layout/c:manualLayout/c:y"_ostr,
                                  "val"_ostr);
        double nY = aYVal.toDouble();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.172648731408574, nY, 1e-7);
    }

    loadFromURL(u"ods/tdf136024.ods");
    {
        saveAndReload("calc8");
        // tdf#136024: test custom position of pie chart data label after an ods export
        Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
        CPPUNIT_ASSERT(xChartDoc.is());

        Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
        CPPUNIT_ASSERT(xDataSeries.is());
        uno::Reference<beans::XPropertySet> xPropertySet(xDataSeries->getDataPointByIndex(0),
                                                         uno::UNO_SET_THROW);

        chart2::RelativePosition aCustomLabelPosition;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("CustomLabelPosition")
                       >>= aCustomLabelPosition);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.0961935120945059, aCustomLabelPosition.Primary, 1e-5);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.209578842093566, aCustomLabelPosition.Secondary, 1e-5);
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCustomDataLabelMultipleSeries)
{
    loadFromURL(u"pptx/tdf115107-2.pptx");

    Reference<chart2::XChartDocument> xChartDoc(getChartDocFromDrawImpress(0, 0), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc.is());

    uno::Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
    CPPUNIT_ASSERT(xDataSeries.is());
    float nFontSize;
    sal_Int64 nFontColor;
    uno::Reference<beans::XPropertySet> xPropertySet;
    uno::Sequence<uno::Reference<chart2::XDataPointCustomLabelField>> aFields;

    // First series
    xPropertySet.set(xDataSeries->getDataPointByIndex(0), uno::UNO_SET_THROW);
    xPropertySet->getPropertyValue("CustomLabelFields") >>= aFields;
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(3), aFields.getLength());

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_VALUE,
                         aFields[0]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("4.3"), aFields[0]->getString());
    aFields[0]->getPropertyValue("CharHeight") >>= nFontSize;
    aFields[0]->getPropertyValue("CharColor") >>= nFontColor;
    CPPUNIT_ASSERT_EQUAL(static_cast<float>(18), nFontSize);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int64>(0xc00000), nFontColor);

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_TEXT,
                         aFields[1]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString(" "), aFields[1]->getString());

    CPPUNIT_ASSERT_EQUAL(
        chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_SERIESNAME,
        aFields[2]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("Bars"), aFields[2]->getString());

    // Second series
    xDataSeries = getDataSeriesFromDoc(xChartDoc, 0, 1);
    CPPUNIT_ASSERT(xDataSeries.is());

    xPropertySet.set(xDataSeries->getDataPointByIndex(0), uno::UNO_SET_THROW);
    xPropertySet->getPropertyValue("CustomLabelFields") >>= aFields;
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(3), aFields.getLength());

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_VALUE,
                         aFields[0]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("2"), aFields[0]->getString());
    aFields[0]->getPropertyValue("CharHeight") >>= nFontSize;
    aFields[0]->getPropertyValue("CharColor") >>= nFontColor;
    CPPUNIT_ASSERT_EQUAL(static_cast<float>(18), nFontSize);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int64>(0xffd966), nFontColor);

    CPPUNIT_ASSERT_EQUAL(chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_TEXT,
                         aFields[1]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString(" "), aFields[1]->getString());

    CPPUNIT_ASSERT_EQUAL(
        chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_SERIESNAME,
        aFields[2]->getFieldType());
    CPPUNIT_ASSERT_EQUAL(OUString("Line"), aFields[2]->getString());
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testLeaderLines)
{
    // FIXME: validation error in OOXML export: Errors: 2
    skipValidation();

    loadFromURL(u"xlsx/testTdf90749.xlsx");
    {
        save("Calc Office Open XML");
        xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
        CPPUNIT_ASSERT(pXmlDoc);
        assertXPath(pXmlDoc,
                    "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser[1]/c:dLbls/c:extLst/c:ext/"
                    "c15:showLeaderLines"_ostr,
                    "val"_ostr, "1");
        assertXPath(pXmlDoc,
                    "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser[2]/c:dLbls/c:extLst/c:ext/"
                    "c15:showLeaderLines"_ostr,
                    "val"_ostr, "0");
    }
    loadFromURL(u"docx/MSO_Custom_Leader_Line.docx");
    {
        save("Office Open XML Text");
        xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
        CPPUNIT_ASSERT(pXmlDoc);
        // tdf#134571: Check the leader line is switch off.
        assertXPath(pXmlDoc,
                    "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:extLst/c:ext/"
                    "c15:showLeaderLines"_ostr,
                    "val"_ostr, "0");
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testNumberFormatExportPPTX)
{
    loadFromURL(u"pptx/tdf115859.pptx");
    save("Impress MS PowerPoint 2007 XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:numFmt"_ostr,
                "formatCode"_ostr, "#,##0.00,\\K");
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:numFmt"_ostr,
                "sourceLinked"_ostr, "0");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testLabelSeparatorExportDOCX)
{
    loadFromURL(u"docx/testLabelSeparator.docx");

    Reference<chart2::XChartDocument> xChartDoc(getChartDocFromWriter(0), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc.is());

    save("Office Open XML Text");
    xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // The text separator should be a new line
    assertXPathContent(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:dLbls/c:separator"_ostr,
        "\n");
    // The text separator should be a comma
    assertXPathContent(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[2]/c:dLbls/c:separator"_ostr,
        ", ");
    // The text separator should be a semicolon
    assertXPathContent(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[3]/c:dLbls/c:separator"_ostr,
        "; ");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testChartTitlePropertiesColorFillPPTX)
{
    loadFromURL(u"pptx/testChartTitlePropertiesColorFill.pptx");
    save("Impress MS PowerPoint 2007 XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:title/c:spPr/a:solidFill/a:srgbClr"_ostr,
                "val"_ostr, "ff0000");
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:title/c:spPr/a:ln/a:noFill"_ostr, 1);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testChartTitlePropertiesGradientFillPPTX)
{
    loadFromURL(u"pptx/testChartTitlePropertiesGradientFill.pptx");
    save("Impress MS PowerPoint 2007 XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:title/c:spPr/a:gradFill/a:gsLst/a:gs[1]/a:srgbClr"_ostr,
                "val"_ostr, "f6f8fc");
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:title/c:spPr/a:gradFill/a:gsLst/a:gs[2]/a:srgbClr"_ostr,
                "val"_ostr, "c7d5ed");
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:title/c:spPr/a:ln/a:noFill"_ostr, 1);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testChartTitlePropertiesBitmapFillPPTX)
{
    loadFromURL(u"pptx/testChartTitlePropertiesBitmapFill.pptx");
    save("Impress MS PowerPoint 2007 XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:title/c:spPr/a:blipFill/a:blip"_ostr,
                "embed"_ostr, "rId1");
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:title/c:spPr/a:ln/a:noFill"_ostr, 1);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testxAxisLabelsRotation)
{
    loadFromURL(u"xlsx/xAxisLabelsRotation.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc1 = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc1);

    // Chart1 xAxis labels should be 45 degree
    assertXPath(pXmlDoc1, "/c:chartSpace/c:chart/c:plotArea/c:catAx/c:txPr/a:bodyPr"_ostr,
                "rot"_ostr, "2700000");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testMultipleCategoryAxisLablesXLSX)
{
    loadFromURL(u"ods/multilevelcat.ods");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    // check category axis labels number of first level
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/c:multiLvlStrRef/"
                "c:multiLvlStrCache/c:ptCount"_ostr,
                "val"_ostr, "6");
    // check category axis labels text of first level
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/"
                       "c:multiLvlStrRef/c:multiLvlStrCache/c:lvl[1]/c:pt[1]/c:v"_ostr,
                       "Categoria 1");
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/"
                       "c:multiLvlStrRef/c:multiLvlStrCache/c:lvl[1]/c:pt[6]/c:v"_ostr,
                       "Categoria 6");
    // check category axis labels text of second level
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/"
                       "c:multiLvlStrRef/c:multiLvlStrCache/c:lvl[2]/c:pt[1]/c:v"_ostr,
                       "2011");
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/"
                       "c:multiLvlStrRef/c:multiLvlStrCache/c:lvl[2]/c:pt[3]/c:v"_ostr,
                       "2013");
    // check the 'noMultiLvlLbl' tag - ChartExport.cxx:2950 FIXME: seems not support, so check the default noMultiLvlLbl value.
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx/c:noMultiLvlLbl"_ostr,
                "val"_ostr, "0");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testMultipleCategoryAxisLablesDOCX)
{
    loadFromURL(u"odt/multilevelcat.odt");
    save("Office Open XML Text");
    xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    // check category axis labels number of first level
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/c:multiLvlStrRef/"
                "c:multiLvlStrCache/c:ptCount"_ostr,
                "val"_ostr, "4");
    // check category axis labels text of first level
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/"
                       "c:multiLvlStrRef/c:multiLvlStrCache/c:lvl[1]/c:pt[1]/c:v"_ostr,
                       "Categoria 1");
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/"
                       "c:multiLvlStrRef/c:multiLvlStrCache/c:lvl[1]/c:pt[4]/c:v"_ostr,
                       "Categoria 4");
    // check category axis labels text of second level
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/"
                       "c:multiLvlStrRef/c:multiLvlStrCache/c:lvl[2]/c:pt[1]/c:v"_ostr,
                       "2011");
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/"
                       "c:multiLvlStrRef/c:multiLvlStrCache/c:lvl[2]/c:pt[2]/c:v"_ostr,
                       "2012");
    // check the 'noMultiLvlLbl' tag - ChartExport.cxx:2950 FIXME: seems not support, so check the default noMultiLvlLbl value.
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx/c:noMultiLvlLbl"_ostr,
                "val"_ostr, "0");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf116163)
{
    loadFromURL(u"pptx/tdf116163.pptx");
    save("Impress MS PowerPoint 2007 XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx/c:txPr/a:bodyPr"_ostr,
                "rot"_ostr, "-5400000");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf111824)
{
    loadFromURL(u"xlsx/tdf111824.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // Collect 3D barchart Z axID
    OUString zAxisIdOf3DBarchart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:bar3DChart/c:axId[3]"_ostr, "val"_ostr);
    // 3D barchart Z axis properties should be in a serAx OOXML tag instead of catAx
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:serAx/c:axId"_ostr, "val"_ostr,
                zAxisIdOf3DBarchart);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, test3DAreaChartZAxis)
{
    loadFromURL(u"xlsx/test3DAreaChartZAxis.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // Collect 3D area chart Z axID
    OUString zAxisIdOf3DAreachart = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:area3DChart/c:axId[3]"_ostr, "val"_ostr);
    // 3D area chart z-axis properties should be in a serAx OOXML element instead of catAx
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:serAx/c:axId"_ostr, "val"_ostr,
                zAxisIdOf3DAreachart);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf119029)
{
    loadFromURL(u"odp/tdf119029.odp");
    // Only use "chart", without number, because the number depends on the previous tests
    save("Impress MS PowerPoint 2007 XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:txPr/a:bodyPr"_ostr,
                "rot"_ostr, "-5400000");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf108022)
{
    loadFromURL(u"odt/tdf108022.odt");
    saveAndReload("Office Open XML Text");

    // assert we really have two charts
    Reference<chart2::XChartDocument> xChartDoc1(getChartDocFromWriter(0), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc1.is());
    Reference<chart2::XChartDocument> xChartDoc2(getChartDocFromWriter(1), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc2.is());
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf121744)
{
    loadFromURL(u"docx/tdf121744.docx");
    save("Office Open XML Text");
    xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    OUString XValueId = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:axId[1]"_ostr, "val"_ostr);
    OUString YValueId = getXPath(
        pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:axId[2]"_ostr, "val"_ostr);

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:axId[1]"_ostr, "val"_ostr,
                XValueId);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:axId[2]"_ostr, "val"_ostr,
                YValueId);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf121189)
{
    loadFromURL(u"odp/tdf121189.odp");
    saveAndReload("Impress Office Open XML");

    uno::Reference<drawing::XDrawPagesSupplier> xDoc(mxComponent, uno::UNO_QUERY_THROW);
    uno::Reference<drawing::XDrawPage> xPage(xDoc->getDrawPages()->getByIndex(0),
                                             uno::UNO_QUERY_THROW);
    CPPUNIT_ASSERT_EQUAL(sal_Int32(1), xPage->getCount());
    uno::Reference<drawing::XShape> xShape(xPage->getByIndex(0), uno::UNO_QUERY_THROW);
    CPPUNIT_ASSERT_EQUAL(OUString("com.sun.star.drawing.OLE2Shape"), xShape->getShapeType());
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf122031)
{
    //Checks pie chart data label format.
    loadFromURL(u"xlsx/tdf122031.xlsx");

    // FIXME: validation error in OOXML export: Errors: 1
    skipValidation();

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dLbls/c:numFmt"_ostr,
                "formatCode"_ostr, "0.000%");
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dLbls/c:dLbl[1]/c:numFmt"_ostr,
                "formatCode"_ostr, "0.000%");
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dLbls/c:dLbl[2]/c:numFmt"_ostr,
                "formatCode"_ostr, "0.000%");
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dLbls/c:dLbl[3]/c:numFmt"_ostr,
                "formatCode"_ostr, "0.000%");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf115012)
{
    loadFromURL(u"xlsx/tdf115012.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    // workaround: use-zero instead of leave-gap to show the original line chart
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:dispBlanksAs"_ostr, "val"_ostr, "zero");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf134118)
{
    loadFromURL(u"xlsx/tdf134118.xlsx");

    // FIXME: validation error in OOXML export: Errors: 1
    skipValidation();

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    // workaround: use leave-gap instead of zero to show the original line chart
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:dispBlanksAs"_ostr, "val"_ostr, "gap");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf123206_customLabelText)
{
    loadFromURL(u"docx/tdf123206.docx");

    Reference<chart2::XChartDocument> xChartDoc(getChartDocFromWriter(0), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc.is());

    // FIXME: validation error in OOXML export: Errors: 2
    skipValidation();

    save("Office Open XML Text");
    xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dLbls/c:dLbl[2]/c:tx/"
                       "c:rich/a:p/a:r/a:t"_ostr,
                       "kiscica");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCustomLabelText)
{
    loadFromURL(u"docx/testCustomlabeltext.docx");

    Reference<chart2::XChartDocument> xChartDoc(getChartDocFromWriter(0), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc.is());

    // FIXME: validation error in OOXML export: Errors: 3
    skipValidation();

    save("Office Open XML Text");
    xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:dLbl[1]/c:idx"_ostr,
                "val"_ostr, "2");
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:dLbl[1]/c:tx/"
                       "c:rich/a:p/a:r[1]/a:t"_ostr,
                       "3.5");
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:dLbl[1]/c:tx/"
                       "c:rich/a:p/a:r[3]/a:t"_ostr,
                       "CustomLabel 1");

    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:dLbl[2]/c:idx"_ostr,
                "val"_ostr, "3");
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:dLbl[2]/c:tx/"
                       "c:rich/a:p/a:r[1]/a:t"_ostr,
                       "4.5");
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:dLbl[2]/c:tx/"
                       "c:rich/a:p/a:r[3]/a:t"_ostr,
                       "CustomLabel 2");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testDeletedLegendEntries)
{
    loadFromURL(u"xlsx/deleted_legend_entry.xlsx");
    {
        saveAndReload("Calc Office Open XML");
        Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
        CPPUNIT_ASSERT(xChartDoc.is());
        Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 1));
        CPPUNIT_ASSERT(xDataSeries.is());
        Reference<beans::XPropertySet> xPropertySet(xDataSeries, uno::UNO_QUERY_THROW);
        bool bShowLegendEntry = true;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("ShowLegendEntry") >>= bShowLegendEntry);
        CPPUNIT_ASSERT(!bShowLegendEntry);
    }

    loadFromURL(u"xlsx/deleted_legend_entry2.xlsx");
    {
        saveAndReload("Calc Office Open XML");
        Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
        CPPUNIT_ASSERT(xChartDoc.is());
        Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
        CPPUNIT_ASSERT(xDataSeries.is());
        Reference<beans::XPropertySet> xPropertySet(xDataSeries, uno::UNO_QUERY_THROW);
        bool bShowLegendEntry = true;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("ShowLegendEntry") >>= bShowLegendEntry);
        CPPUNIT_ASSERT(!bShowLegendEntry);

        Reference<chart2::XChartDocument> xChartDoc2 = getChartDocFromSheet(1, mxComponent);
        CPPUNIT_ASSERT(xChartDoc.is());
        Reference<chart2::XDataSeries> xDataSeries2(getDataSeriesFromDoc(xChartDoc2, 0));
        CPPUNIT_ASSERT(xDataSeries2.is());
        Reference<beans::XPropertySet> xPropertySet2(xDataSeries2, uno::UNO_QUERY_THROW);
        Sequence<sal_Int32> deletedLegendEntriesSeq;
        CPPUNIT_ASSERT(xPropertySet2->getPropertyValue("DeletedLegendEntries")
                       >>= deletedLegendEntriesSeq);
        CPPUNIT_ASSERT_EQUAL(sal_Int32(1), deletedLegendEntriesSeq[0]);
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf60316)
{
    loadFromURL(u"pptx/tdf60316.pptx");
    save("Impress MS PowerPoint 2007 XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // Without the fix in place, the shape would have had a solidFill background
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:spPr/a:noFill"_ostr, 1);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:spPr/a:solidFill"_ostr, 0);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf130225)
{
    loadFromURL(u"docx/piechart_deleted_legend_entry.docx");
    saveAndReload("Office Open XML Text");
    Reference<chart2::XChartDocument> xChartDoc(getChartDocFromWriter(0), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc.is());
    Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
    CPPUNIT_ASSERT(xDataSeries.is());
    Reference<beans::XPropertySet> xPropertySet(xDataSeries, uno::UNO_QUERY_THROW);
    Sequence<sal_Int32> deletedLegendEntriesSeq;
    CPPUNIT_ASSERT(xPropertySet->getPropertyValue("DeletedLegendEntries")
                   >>= deletedLegendEntriesSeq);
    CPPUNIT_ASSERT_EQUAL(sal_Int32(1), deletedLegendEntriesSeq[0]);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf59857)
{
    loadFromURL(u"ods/tdf59857.ods");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:floor/c:spPr/a:ln/a:noFill"_ostr, 1);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:floor/c:spPr/a:solidFill/a:srgbClr"_ostr,
                "val"_ostr, "cccccc");
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:backWall/c:spPr/a:ln/a:noFill"_ostr, 0);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:backWall/c:spPr/a:ln/a:solidFill/a:srgbClr"_ostr,
                "val"_ostr, "b3b3b3");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf126076)
{
    loadFromURL(u"xlsx/auto_marker_excel10.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // This was 12: all series exported with square markers
    assertXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser/c:marker/c:symbol[@val='square']"_ostr,
        0);
    // instead of skipping markers
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:lineChart/c:ser/c:marker"_ostr, 0);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf75330)
{
    loadFromURL(u"ods/legend_overlay.ods");
    saveAndReload("calc8");
    {
        uno::Reference<chart2::XChartDocument> xChart2Doc = getChartDocFromSheet(0, mxComponent);
        uno::Reference<chart::XChartDocument> xChartDoc(xChart2Doc, uno::UNO_QUERY);
        uno::Reference<drawing::XShape> xLegend = xChartDoc->getLegend();
        Reference<beans::XPropertySet> xPropertySet(xLegend, uno::UNO_QUERY_THROW);
        bool bOverlay = false;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("Overlay") >>= bOverlay);
        CPPUNIT_ASSERT(bOverlay);
    }
    saveAndReload("Calc Office Open XML");
    {
        uno::Reference<chart2::XChartDocument> xChart2Doc = getChartDocFromSheet(0, mxComponent);
        uno::Reference<chart::XChartDocument> xChartDoc(xChart2Doc, uno::UNO_QUERY);
        uno::Reference<drawing::XShape> xLegend = xChartDoc->getLegend();
        Reference<beans::XPropertySet> xPropertySet(xLegend, uno::UNO_QUERY_THROW);
        bool bOverlay = false;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("Overlay") >>= bOverlay);
        CPPUNIT_ASSERT(bOverlay);
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf127792)
{
    loadFromURL(u"docx/MSO_axis_position.docx");
    save("Office Open XML Text");
    xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx/c:crossBetween"_ostr, "val"_ostr,
                "between");

    pXmlDoc = parseExport("word/charts/chart2.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:valAx/c:crossBetween"_ostr, "val"_ostr,
                "midCat");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf131979)
{
    loadFromURL(u"ods/tdf131115.ods");
    {
        saveAndReload("calc8");
        Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
        CPPUNIT_ASSERT(xChartDoc.is());
        Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
        CPPUNIT_ASSERT(xDataSeries.is());
        Reference<beans::XPropertySet> xPropertySet;
        xPropertySet.set(xDataSeries->getDataPointByIndex(2), uno::UNO_SET_THROW);
        bool blinknumberformattosource = true;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue(CHART_UNONAME_LINK_TO_SRC_NUMFMT)
                       >>= blinknumberformattosource);
        CPPUNIT_ASSERT_MESSAGE("\"LinkNumberFormatToSource\" should be set to false.",
                               !blinknumberformattosource);
    }

    loadFromURL(u"ods/tdf131979.ods");
    {
        saveAndReload("calc8");
        Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
        CPPUNIT_ASSERT(xChartDoc.is());
        Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
        CPPUNIT_ASSERT(xDataSeries.is());
        Reference<beans::XPropertySet> xPropertySet;
        xPropertySet.set(xDataSeries->getDataPointByIndex(2), uno::UNO_SET_THROW);
        bool blinknumberformattosource = true;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue(CHART_UNONAME_LINK_TO_SRC_NUMFMT)
                       >>= blinknumberformattosource);
        CPPUNIT_ASSERT_MESSAGE("\"LinkNumberFormatToSource\" should be set to true.",
                               blinknumberformattosource);
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf132076)
{
    // FIXME: validation error in OOXML export: Errors: 1
    skipValidation();

    {
        loadFromURL(u"ods/tdf132076.ods");
        save("Calc Office Open XML");
        xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
        CPPUNIT_ASSERT(pXmlDoc);
        assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx/c:numFmt"_ostr,
                    "formatCode"_ostr, "dd");
        assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:catAx/c:numFmt"_ostr,
                    "sourceLinked"_ostr, "0");
    }
    {
        loadFromURL(u"xlsx/tdf132076.xlsx");
        save("Calc Office Open XML");
        xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
        CPPUNIT_ASSERT(pXmlDoc);
        assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:dateAx/c:numFmt"_ostr,
                    "formatCode"_ostr, "dd");
        assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:dateAx/c:numFmt"_ostr,
                    "sourceLinked"_ostr, "0");
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf125812)
{
    loadFromURL(u"odp/ellipticalGradientFill.odp");
    save("Impress MS PowerPoint 2007 XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:spPr/a:gradFill/a:path"_ostr,
                "path"_ostr, "circle");
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:spPr/a:gradFill/a:path/a:fillToRect"_ostr,
                "l"_ostr, "50000");
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:spPr/a:gradFill/a:path/a:fillToRect"_ostr,
                "t"_ostr, "49000");
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:spPr/a:gradFill/a:path/a:fillToRect"_ostr,
                "r"_ostr, "50000");
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:spPr/a:gradFill/a:path/a:fillToRect"_ostr,
                "b"_ostr, "51000");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf133190)
{
    loadFromURL(u"xlsx/tdf133190_tdf133191.xlsx");

    // FIXME: validation error in OOXML export: Errors: 1
    skipValidation();

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // Test word wrap of data point label
    assertXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dLbls/c:dLbl[1]/c:txPr/a:bodyPr"_ostr,
        "wrap"_ostr, "none");
    assertXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dLbls/c:dLbl[2]/c:txPr/a:bodyPr"_ostr,
        "wrap"_ostr, "square");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf133191)
{
    loadFromURL(u"xlsx/tdf133190_tdf133191.xlsx");

    // FIXME: validation error in OOXML export: Errors: 1
    skipValidation();

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // Test rotation of data point label
    assertXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dLbls/c:dLbl[3]/c:txPr/a:bodyPr"_ostr,
        "rot"_ostr, "-4500000");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf132594)
{
    loadFromURL(u"xlsx/chart_pie2007.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:cat"_ostr, 1);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf134255)
{
    loadFromURL(u"docx/tdf134255.docx");
    Reference<chart2::XChartDocument> xChartDoc(getChartDocFromWriter(0), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc.is());

    // import test
    Reference<chart2::XDataSeries> xDataSeries = getDataSeriesFromDoc(xChartDoc, 0);
    CPPUNIT_ASSERT(xDataSeries.is());
    Reference<beans::XPropertySet> xPropSet(xDataSeries, UNO_QUERY_THROW);
    bool bWrap = false;
    CPPUNIT_ASSERT((xPropSet->getPropertyValue("TextWordWrap") >>= bWrap));
    CPPUNIT_ASSERT(bWrap);

    // FIXME: validation error in OOXML export: Errors: 11
    skipValidation();

    // export test
    save("Office Open XML Text");
    xmlDocUniquePtr pXmlDoc = parseExport("word/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:pieChart/c:ser/c:dLbls/c:txPr/a:bodyPr"_ostr,
                "wrap"_ostr, "square");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf134977)
{
    loadFromURL(u"xlsx/custom_data_label.xlsx");

    //import test
    uno::Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
    CPPUNIT_ASSERT(xChartDoc.is());
    Reference<chart2::XDataSeries> xDataSeries = getDataSeriesFromDoc(xChartDoc, 0);
    CPPUNIT_ASSERT(xDataSeries.is());
    uno::Reference<beans::XPropertySet> xPropertySet(xDataSeries->getDataPointByIndex(0),
                                                     uno::UNO_SET_THROW);
    uno::Sequence<uno::Reference<chart2::XDataPointCustomLabelField>> aFields;
    float nFontSize;
    xPropertySet->getPropertyValue("CustomLabelFields") >>= aFields;
    aFields[0]->getPropertyValue("CharHeight") >>= nFontSize;
    CPPUNIT_ASSERT_EQUAL(static_cast<float>(9), nFontSize);

    // FIXME: validation error in OOXML export: Errors: 2
    skipValidation();

    //export test
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:dLbls/c:dLbl/c:tx/c:rich/a:p/"
                "a:r/a:rPr"_ostr,
                "sz"_ostr, "900");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf123647)
{
    loadFromURL(u"xlsx/empty_chart.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart"_ostr, 1);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf136267)
{
    loadFromURL(u"xlsx/tdf136267.xlsx");

    // FIXME: validation error in OOXML export: Errors: 2
    skipValidation();

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPathContent(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:cat/c:strRef/c:strCache/c:pt/c:v"_ostr,
        "John");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testDataLabelPlacementPieChart)
{
    loadFromURL(u"xlsx/tdf134978.xlsx");
    saveAndReload("calc8");
    uno::Reference<chart::XChartDocument> xChartDoc(getChartCompFromSheet(0, 0, mxComponent),
                                                    UNO_QUERY_THROW);
    // test the placement of the manually positioned label
    Reference<beans::XPropertySet> xDataPointPropSet(
        xChartDoc->getDiagram()->getDataPointProperties(2, 0), uno::UNO_SET_THROW);
    uno::Any aAny = xDataPointPropSet->getPropertyValue("LabelPlacement");
    CPPUNIT_ASSERT(aAny.hasValue());
    sal_Int32 nLabelPlacement = 0;
    CPPUNIT_ASSERT(aAny >>= nLabelPlacement);
    CPPUNIT_ASSERT_EQUAL(chart::DataLabelPlacement::OUTSIDE, nLabelPlacement);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf137917)
{
    loadFromURL(u"xlsx/tdf137917.xlsx");

    // FIXME: validation error in OOXML export: Errors: 1
    skipValidation();

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:dateAx/c:baseTimeUnit"_ostr,
                "val"_ostr, "days");
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:dateAx/c:majorUnit"_ostr, "val"_ostr,
                "1");
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:dateAx/c:majorTimeUnit"_ostr,
                "val"_ostr, "months");
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:dateAx/c:minorUnit"_ostr, "val"_ostr,
                "7");
    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:dateAx/c:minorTimeUnit"_ostr,
                "val"_ostr, "days");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf138204)
{
    loadFromURL(u"xlsx/tdf138204.xlsx");

    Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
    CPPUNIT_ASSERT(xChartDoc.is());

    struct CustomLabelsTestData
    {
        sal_Int32 nSeriesIdx;
        sal_Int32 nNumFields;
        // First field attributes.
        chart2::DataPointCustomLabelFieldType eFieldType;
        OUString aCellRange;
        OUString aString;
    };

    const CustomLabelsTestData aTestEntries[2] = {
        {
            // series id of c:ser[1] is 0.
            0, // nSeriesIdx
            1, // nNumFields
            chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_CELLRANGE,
            "Munka1!$F$9", // aCellRange
            "67,5%", // aString
        },
        {

            // series id of c:ser[2] is 1.
            1, // nSeriesIdx
            1, // nNumFields
            chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_CELLRANGE,
            "Munka1!$G$9", // aCellRange
            "32,3%", // aString
        },
    };

    for (const auto& aTestEntry : aTestEntries)
    {
        uno::Reference<chart2::XDataSeries> xDataSeries(
            getDataSeriesFromDoc(xChartDoc, aTestEntry.nSeriesIdx));
        CPPUNIT_ASSERT(xDataSeries.is());

        uno::Reference<beans::XPropertySet> xPropertySet;
        uno::Sequence<uno::Reference<chart2::XDataPointCustomLabelField>> aFields;
        xPropertySet.set(xDataSeries->getDataPointByIndex(0), uno::UNO_SET_THROW);
        xPropertySet->getPropertyValue("CustomLabelFields") >>= aFields;
        CPPUNIT_ASSERT_EQUAL(aTestEntry.nNumFields, aFields.getLength());

        CPPUNIT_ASSERT_EQUAL(aTestEntry.eFieldType, aFields[0]->getFieldType());
        CPPUNIT_ASSERT_EQUAL(aTestEntry.aCellRange, aFields[0]->getCellRange());
        CPPUNIT_ASSERT_EQUAL(aTestEntry.aString, aFields[0]->getString());
    }

    // FIXME: validation error in OOXML export: Errors: 2
    skipValidation();

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // Check the first data label field type
    assertXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:dLbls/c:dLbl/c:tx/c:rich/a:p/a:fld"_ostr,
        "type"_ostr, "CELLRANGE");

    assertXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[2]/c:dLbls/c:dLbl/c:tx/c:rich/a:p/a:fld"_ostr,
        "type"_ostr, "CELLRANGE");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf138181)
{
    loadFromURL(u"xlsx/piechart_deleted_legendentry.xlsx");
    Reference<chart::XChartDocument> xChartDoc(getChartDocFromSheet(0, mxComponent),
                                               UNO_QUERY_THROW);
    Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xChartDoc, UNO_QUERY_THROW);
    Reference<drawing::XDrawPage> xDrawPage(xDrawPageSupplier->getDrawPage(), UNO_SET_THROW);
    Reference<drawing::XShapes> xShapes(xDrawPage->getByIndex(0), UNO_QUERY_THROW);
    Reference<drawing::XShape> xLegendEntry1, xLegendEntry2, xLegendEntry3;

    // first legend entry is visible
    xLegendEntry1
        = getShapeByName(xShapes, "CID/MultiClick/D=0:CS=0:CT=0:Series=0:Point=0:LegendEntry=0");
    CPPUNIT_ASSERT(xLegendEntry1.is());

    // second legend entry is not visible
    xLegendEntry2
        = getShapeByName(xShapes, "CID/MultiClick/D=0:CS=0:CT=0:Series=0:Point=1:LegendEntry=0");
    CPPUNIT_ASSERT(!xLegendEntry2.is());

    // third legend entry is visible
    xLegendEntry3
        = getShapeByName(xShapes, "CID/MultiClick/D=0:CS=0:CT=0:Series=0:Point=2:LegendEntry=0");
    CPPUNIT_ASSERT(xLegendEntry3.is());
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testCustomShapeText)
{
    loadFromURL(u"ods/tdf72776.ods");
    saveAndReload("calc8");
    Reference<chart::XChartDocument> xChartDoc(getChartDocFromSheet(0, mxComponent),
                                               UNO_QUERY_THROW);
    // test that the text of custom shape exists inside the chart
    Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xChartDoc, UNO_QUERY_THROW);
    Reference<drawing::XDrawPage> xDrawPage(xDrawPageSupplier->getDrawPage(), UNO_SET_THROW);
    Reference<drawing::XShape> xCustomShape(xDrawPage->getByIndex(1), UNO_QUERY_THROW);
    CPPUNIT_ASSERT(xCustomShape.is());

    Reference<text::XText> xRange(xCustomShape, uno::UNO_QUERY_THROW);
    CPPUNIT_ASSERT(!xRange->getString().isEmpty());
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testuserShapesXLSX)
{
    loadFromURL(u"xlsx/tdf128621.xlsx");
    saveAndReload("Calc Office Open XML");

    Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
    CPPUNIT_ASSERT(xChartDoc.is());

    // test that the custom shape exists
    Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xChartDoc, UNO_QUERY_THROW);
    Reference<drawing::XDrawPage> xDrawPage(xDrawPageSupplier->getDrawPage(), UNO_SET_THROW);
    Reference<drawing::XShape> xCustomShape(xDrawPage->getByIndex(1), UNO_QUERY_THROW);
    CPPUNIT_ASSERT(xCustomShape.is());
    // test type of shape
    CPPUNIT_ASSERT(xCustomShape->getShapeType().endsWith("CustomShape"));
    // test custom shape position
    awt::Point aPosition = xCustomShape->getPosition();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1356, aPosition.X, 300);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9107, aPosition.Y, 300);
    // test custom shape size
    awt::Size aSize = xCustomShape->getSize();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9520, aSize.Width, 300);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1805, aSize.Height, 300);
    // test custom shape text
    Reference<text::XText> xRange(xCustomShape, uno::UNO_QUERY_THROW);
    CPPUNIT_ASSERT(!xRange->getString().isEmpty());
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testuserShapesDOCX)
{
    loadFromURL(u"docx/tdf143130.docx");
    saveAndReload("Office Open XML Text");

    Reference<chart2::XChartDocument> xChartDoc(getChartDocFromWriter(0), uno::UNO_QUERY);
    CPPUNIT_ASSERT(xChartDoc.is());

    // test that the custom shape exists
    Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xChartDoc, UNO_QUERY_THROW);
    Reference<drawing::XDrawPage> xDrawPage(xDrawPageSupplier->getDrawPage(), UNO_SET_THROW);
    Reference<drawing::XShape> xCustomShape(xDrawPage->getByIndex(0), UNO_QUERY_THROW);
    CPPUNIT_ASSERT(xCustomShape.is());
    // test type of shape
    CPPUNIT_ASSERT(xCustomShape->getShapeType().endsWith("CustomShape"));
    // test custom shape position
    awt::Point aPosition = xCustomShape->getPosition();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9824, aPosition.X, 300);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(547, aPosition.Y, 300);
    // test custom shape size
    awt::Size aSize = xCustomShape->getSize();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1848, aSize.Width, 300);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1003, aSize.Height, 300);
    // test custom shape text
    Reference<text::XText> xRange(xCustomShape, uno::UNO_QUERY_THROW);
    CPPUNIT_ASSERT(!xRange->getString().isEmpty());
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testGraphicBlipXLSX)
{
    loadFromURL(u"xlsx/tdf143127.xlsx");
    saveAndReload("Calc Office Open XML");

    Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
    CPPUNIT_ASSERT(xChartDoc.is());

    // test that the Graphic shape exists
    Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xChartDoc, UNO_QUERY_THROW);
    Reference<drawing::XDrawPage> xDrawPage(xDrawPageSupplier->getDrawPage(), UNO_SET_THROW);
    Reference<drawing::XShape> xCustomShape(xDrawPage->getByIndex(1), UNO_QUERY_THROW);
    CPPUNIT_ASSERT(xCustomShape.is());
    // test type of shape
    CPPUNIT_ASSERT(xCustomShape->getShapeType().endsWith("GraphicObjectShape"));
    Reference<XPropertySet> xShapeProps(xCustomShape, UNO_QUERY);

    uno::Reference<graphic::XGraphic> xGraphic;
    CPPUNIT_ASSERT(xShapeProps->getPropertyValue("Graphic") >>= xGraphic);

    Graphic aGraphic(xGraphic);
    GfxLink aLink = aGraphic.GetGfxLink();
    std::size_t nDataSize = aLink.GetDataSize();

    // test the image size is bigger then 0.
    CPPUNIT_ASSERT_GREATER(size_t(0), nDataSize);
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testNameRangeXLSX)
{
    loadFromURL(u"xlsx/chart_with_name_range.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    // test the syntax of local range name on the local sheet.
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:cat/c:strRef/c:f"_ostr,
                       "Sheet1!local_name_range");
    // test the syntax of a global range name.
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser/c:val/c:numRef/c:f"_ostr,
                       "[0]!series1");
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testTdf143942)
{
    loadFromURL(u"xlsx/tdf143942.xlsx");

    Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
    CPPUNIT_ASSERT(xChartDoc.is());

    uno::Reference<chart2::XDataSeries> xDataSeries(getDataSeriesFromDoc(xChartDoc, 0));
    CPPUNIT_ASSERT(xDataSeries.is());

    constexpr size_t nLabels = 4;
    OUString aCellRange = "Sheet1!$A$2:$A$5";
    OUString aLabels[nLabels] = {
        "Test1",
        "Test2",
        "Tes3",
        "Test4",
    };

    uno::Reference<beans::XPropertySet> xPropertySet;
    uno::Sequence<uno::Reference<chart2::XDataPointCustomLabelField>> aFields;
    for (size_t i = 0; i < nLabels; ++i)
    {
        xPropertySet.set(xDataSeries->getDataPointByIndex(i), uno::UNO_SET_THROW);
        xPropertySet->getPropertyValue("CustomLabelFields") >>= aFields;
        CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(1), aFields.getLength());
        CPPUNIT_ASSERT_EQUAL(
            chart2::DataPointCustomLabelFieldType::DataPointCustomLabelFieldType_CELLRANGE,
            aFields[0]->getFieldType());
        CPPUNIT_ASSERT_EQUAL(aCellRange, aFields[0]->getCellRange());
        CPPUNIT_ASSERT_EQUAL(aLabels[i], aFields[0]->getString());
    }

    // FIXME: validation error in OOXML export: Errors: 4
    skipValidation();

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("xl/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:scatterChart/c:ser[1]/c:extLst/c:ext"_ostr,
                "uri"_ostr, "{02D57815-91ED-43cb-92C2-25804820EDAC}");
    assertXPath(pXmlDoc,
                "/c:chartSpace/c:chart/c:plotArea/c:scatterChart/c:ser[1]/c:extLst/c:ext/"
                "c15:datalabelsRange/c15:dlblRangeCache/c:ptCount"_ostr,
                "val"_ostr, "4");
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:scatterChart/c:ser[1]/c:extLst/c:ext/"
                       "c15:datalabelsRange/c15:f"_ostr,
                       aCellRange);
    for (size_t i = 0; i < nLabels; ++i)
    {
        assertXPath(pXmlDoc,
                    "/c:chartSpace/c:chart/c:plotArea/c:scatterChart/c:ser[1]/c:dLbls/c:dLbl["
                        + OString::number(i + 1) + "]/c:tx/c:rich/a:p/a:fld",
                    "type"_ostr, "CELLRANGE");
        assertXPath(pXmlDoc,
                    "/c:chartSpace/c:chart/c:plotArea/c:scatterChart/c:ser[1]/c:dLbls/c:dLbl["
                        + OString::number(i + 1) + "]/c:extLst/c:ext/c15:showDataLabelsRange",
                    "val"_ostr, "1");
        // Check if the actual label is stored under c15:datalabelsRange
        assertXPathContent(pXmlDoc,
                           "/c:chartSpace/c:chart/c:plotArea/c:scatterChart/c:ser[1]/c:extLst/"
                           "c:ext/c15:datalabelsRange/c15:dlblRangeCache/c:pt["
                               + OString::number(i + 1) + "]/c:v",
                           aLabels[i]);
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testDateCategoriesPPTX)
{
    loadFromURL(u"pptx/bnc889755.pptx");

    // FIXME: validation error in OOXML export: Errors: 1
    skipValidation();

    save("Impress Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("ppt/charts/chart1.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    constexpr size_t nCats = 16;
    double aDates[nCats] = {
        41183, 41214, 41244, 41275, 41306, 41334, 41365, 41395,
        41426, 41456, 41487, 41518, 41548, 41579, 41609, 41640,
    };

    assertXPath(pXmlDoc, "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat"_ostr);
    assertXPathContent(pXmlDoc,
                       "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/c:numRef/"
                       "c:numCache/c:formatCode"_ostr,
                       "mmm\\-yy");
    assertXPath(
        pXmlDoc,
        "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/c:numRef/c:numCache/c:ptCount"_ostr,
        "val"_ostr, OUString::number(nCats));

    for (size_t i = 0; i < nCats; ++i)
    {
        assertXPath(
            pXmlDoc,
            "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/c:numRef/c:numCache/c:pt["
                + OString::number(i + 1) + "]",
            "idx"_ostr, OUString::number(i));
        assertXPathContent(
            pXmlDoc,
            "/c:chartSpace/c:chart/c:plotArea/c:barChart/c:ser[1]/c:cat/c:numRef/c:numCache/c:pt["
                + OString::number(i + 1) + "]/c:v",
            OUString::number(aDates[i]));
    }
}

CPPUNIT_TEST_FIXTURE(Chart2ExportTest2, testDataTableImportExport)
{
    loadFromURL(u"xlsx/ChartDataTable.xlsx");
    {
        uno::Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
        CPPUNIT_ASSERT(xChartDoc.is());
        auto xDiagram = xChartDoc->getFirstDiagram();
        CPPUNIT_ASSERT(xDiagram.is());
        auto xDataTable = xDiagram->getDataTable();
        CPPUNIT_ASSERT(xDataTable.is());
        uno::Reference<beans::XPropertySet> xPropertySet(xDataTable, uno::UNO_QUERY);
        CPPUNIT_ASSERT(xPropertySet.is());
        bool bHBorder;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("HBorder") >>= bHBorder);
        CPPUNIT_ASSERT_EQUAL(true, bHBorder);
        bool bVBorder;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("VBorder") >>= bVBorder);
        CPPUNIT_ASSERT_EQUAL(true, bVBorder);
        bool bOutline;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("Outline") >>= bOutline);
        CPPUNIT_ASSERT_EQUAL(false, bOutline);
        bool bKeys;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("Keys") >>= bKeys);
        CPPUNIT_ASSERT_EQUAL(false, bKeys);
    }
    saveAndReload("calc8");
    {
        uno::Reference<chart2::XChartDocument> xChartDoc = getChartDocFromSheet(0, mxComponent);
        CPPUNIT_ASSERT(xChartDoc.is());
        auto xDiagram = xChartDoc->getFirstDiagram();
        CPPUNIT_ASSERT(xDiagram.is());
        auto xDataTable = xDiagram->getDataTable();
        CPPUNIT_ASSERT(xDataTable.is());
        uno::Reference<beans::XPropertySet> xPropertySet(xDataTable, uno::UNO_QUERY);
        CPPUNIT_ASSERT(xPropertySet.is());
        bool bHBorder;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("HBorder") >>= bHBorder);
        CPPUNIT_ASSERT_EQUAL(true, bHBorder);
        bool bVBorder;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("VBorder") >>= bVBorder);
        CPPUNIT_ASSERT_EQUAL(true, bVBorder);
        bool bOutline;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("Outline") >>= bOutline);
        CPPUNIT_ASSERT_EQUAL(false, bOutline);
        bool bKeys;
        CPPUNIT_ASSERT(xPropertySet->getPropertyValue("Keys") >>= bKeys);
        CPPUNIT_ASSERT_EQUAL(false, bKeys);
    }
}

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
