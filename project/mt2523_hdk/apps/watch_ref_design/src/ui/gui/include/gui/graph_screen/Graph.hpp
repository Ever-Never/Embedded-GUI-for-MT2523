/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <gui/graph_screen/GraphLine.hpp>
#include <gui/graph_screen/GraphBelow.hpp>
#include <gui/graph_screen/GraphDots.hpp>
#include <gui/graph_screen/PainterVerticalAlpha.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>
#if !defined(USE_BPP) || USE_BPP==16
#include <touchgfx/widgets/canvas/PainterRGB565.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565Bitmap.hpp>
#elif USE_BPP==24
#include <touchgfx/widgets/canvas/PainterRGB888.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888Bitmap.hpp>
#endif
#include <BitmapDatabase.hpp>

#include <touchgfx/widgets/Box.hpp>

using namespace touchgfx;

/**
 * @class Graph Graph.hpp gui/graph_screen/Graph.hpp
 *
 * @brief A graph with line, area below and dots.
 *
 *        A graph with line, area below and dots. To save space, the 4 graph components are
 *        linked to share the same memory for graph points.
 *
 * @sa Container
 */
class Graph : public Container
{
public:
    /**
     * @fn Graph::Graph();
     *
     * @brief Default constructor.
     */
    Graph();

    /**
     * @fn virtual Graph::~Graph()
     *
     * @brief Destructor.
     */
    virtual ~Graph();

    /**
     * @fn void Graph::setup(int newWidth, int newHeight, colortype lineColor, colortype backgroundColor);
     *
     * @brief Set characteristics of the graph.
     *
     *        Set characteristics of the graph.
     *
     * @param newWidth        Width of the new.
     * @param newHeight       Height of the new.
     * @param lineColor       The line color.
     * @param backgroundColor The background color.
     */
    void setup(int newWidth, int newHeight, colortype lineColor, colortype backgroundColor);

    /**
     * @fn bool Graph::addValue(int x, int y)
     *
     * @brief Adds a point to the graph.
     *
     *        Adds a point to the graph.
     *
     * @param x The x coordinate.
     * @param y The y coordinate.
     *
     * @return true if it succeeds, false if it fails.
     */
    bool addValue(int x, int y)
    {
        return graphLine.addValue(x, y);
    }

    /**
     * @fn bool Graph::deleteValue(int x)
     *
     * @brief Deletes the value for the given x.
     *
     * @param x The x coordinate.
     *
     * @return true if it succeeds, false if it fails.
     */
    bool deleteValue(int x)
    {
        return graphLine.deleteValue(x);
    }

    /**
     * @fn void Graph::clear()
     *
     * @brief Clears the graph.
     *
     *        Clears the graph.
     */
    void clear()
    {
        graphLine.clearGraph();
    }

    /**
     * @fn void Graph::setRange(int left, int right, int top, int bottom);
     *
     * @brief Sets the range for the graph.
     *
     Sets the range for the graph.
     *
     * @param left   The left.
     * @param right  The right.
     * @param top    The top.
     * @param bottom The bottom.
     */
    void setRange(int left, int right, int top, int bottom) ;

    /**
     * @fn void Graph::setLineVisible(bool lineVisible);
     *
     * @brief Sets line visible.
     *
     *        Sets line visible.
     *
     * @param lineVisible true to show, false to hide.
     */
    void setLineVisible(bool lineVisible);

    /**
     * @fn void Graph::setDotsVisible(bool dotsVisible);
     *
     * @brief Sets dots visible.
     *
     *        Sets dots visible.
     *
     * @param dotsVisible true to show, false to hide.
     */
    void setDotsVisible(bool dotsVisible);

    /**
     * @fn void Graph::setDotsBackgroundVisible(bool dotsBackgroundVisible);
     *
     * @brief Sets dots background visible.
     *
     *        Sets dots background visible.
     *
     * @param dotsBackgroundVisible true to show, false to hide.
     */
    void setDotsBackgroundVisible(bool dotsBackgroundVisible);

    /**
     * @fn void Graph::setAreaVisible(bool areaVisible);
     *
     * @brief Sets area visible.
     *
     *        Sets area visible.
     *
     * @param areaVisible true to show, false to hide.
     */
    void setAreaVisible(bool areaVisible);

    /**
     * @fn void Graph::setDotShape(int startAngle, int angleStep, int lineWidth);
     *
     * @brief Sets dot shape.
     *
     *        Sets dot shape.
     *
     * @param startAngle The start angle.
     * @param angleStep  The angle step.
     * @param lineWidth  Width of the line.
     */
    void setDotShape(int startAngle, int angleStep, int lineWidth);

    /**
     * @fn void Graph::setDotBackgroundShape(int startAngle, int angleStep, int lineWidth);
     *
     * @brief Sets dot background shape.
     *
     *        Sets dot background shape.
     *
     * @param startAngle The start angle.
     * @param angleStep  The angle step.
     * @param lineWidth  Width of the line.
     */
    void setDotBackgroundShape(int startAngle, int angleStep, int lineWidth);

    /**
     * @fn void Graph::setAlpha(uint8_t alpha);
     *
     * @brief Sets the alpha.
     *
     *        Sets the alpha.
     *
     * @param alpha The alpha.
     */
    void setAlpha(uint8_t alpha);

    /**
     * @fn uint8_t Graph::getAlpha();
     *
     * @brief Gets the alpha.
     *
     *        Gets the alpha.
     *
     * @return The alpha.
     */
    uint8_t getAlpha();

protected:
    static const int NUMBER_OF_POINTS = 70; ///< Max number of points in the graph

    GraphLine graphLine;           ///< The line of the graph
    GraphBelow graphArea;          ///< The area below the graph
    GraphDots graphDots;           ///< The dots of the graph
    GraphDots graphDotsBackground; ///< The background of the dots of the graph

    GraphLine::GraphPoint graphBuffer[NUMBER_OF_POINTS]; ///< Memory for the graph points

#if !defined(USE_BPP) || USE_BPP==16
    PainterRGB565 graphLinePainter;           ///< Painter for the line
    PainterRGB565 graphDotsPainter;           ///< Painter for the dots (usually the same as graphLinePainter)
    PainterRGB565 graphDotsBackgroundPainter; ///< Painter for the background of the dots
#elif USE_BPP==24
    PainterRGB888 graphLinePainter;           ///< Painter for the line
    PainterRGB888 graphDotsPainter;           ///< Painter for the dots (usually the same as graphLinePainter)
    PainterRGB888 graphDotsBackgroundPainter; ///< Painter for the background of the dots
#endif
    PainterVerticalAlpha graphAreaPainter;    ///< Painter for area below graph

    uint8_t myAlpha; ///< Alpha value for entire graph
};


#endif /* GRAPH_HPP */
