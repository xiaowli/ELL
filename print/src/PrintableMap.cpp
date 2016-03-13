////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     PrintableMap.cpp (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableMap.h"
#include "PrintableInput.h"
#include "PrintableCoordinatewise.h"
#include "PrintableSum.h"
#include "SvgHelpers.h"

// utilities
#include "Format.h"

// stl
#include <string>
#include <memory>
#include <functional>
#include <vector>

namespace
{
    const char* styleDefinitionFormat =
R"aw(
    <style>

        text
        {
            font-family:    sans-serif;
        }

        rect
        {
            stroke:         #222222;
            stroke-width:   2;
        }
    
        rect.Scale
        {
            fill:           #06aed5;
        }

        rect.Shift
        {
            fill:           #f15156;
        }

        rect.Sum
        {
            fill:           #cf4eff;
        }

        rect.Input
        {
            fill:           #bbbbbb;
        }

        rect.Element
        {
            fill:           white;
        }

        ellipse.Connector
        {
            fill:           #222222;
        }

        path.Edge
        {
            stroke:             #110011;
            stroke-width:       2;
            fill:               none;
            stroke-dasharray:   %;
        }

        text.Layer
        {
            fill:           white;
            font:           bold 15 sans-serif;
        }

        text.Element
        {
            fill:           black;
            font:           15 sans-serif;
        }

        text.ElementIndex
        {
            fill:           #666666;
            font:           9 sans-serif;
        }

    </style>

)aw";
}

void PrintElementDefinition(std::ostream& os, const std::string& id, double width, double height, double connectorRadius, double cornerRadius, bool hasInputConnector=true)
{
    os << "            <g id=\"" << id << "\">\n";
    if (hasInputConnector)
    {
        SvgCircle(os, 4, "Connector", 0, -height / 2, connectorRadius);
    }
    SvgCircle(os, 4, "Connector", 0, height/2, connectorRadius);
    SvgRect(os, 4, "Element", -width/2, -height/2, width, height, cornerRadius);
    os << "            </g>\n";
}

void PrintableMap::Print(std::ostream & os, const PrintArguments& Arguments)
{
    os << "<html>\n<body>\n";
    utilities::PrintFormat(os, styleDefinitionFormat, Arguments.edgeStyle.dashStyle);

    os << "    <Svg>\n\n        <defs>\n";
    PrintElementDefinition(os,
        "ValueElement",
        Arguments.valueElementLayout.width, 
        Arguments.valueElementLayout.height, 
        Arguments.valueElementStyle.connectorRadius, 
        Arguments.valueElementStyle.cornerRadius);
    
    PrintElementDefinition(os,
        "EmptyElement",
        Arguments.emptyElementLayout.width,
        Arguments.emptyElementLayout.height,
        Arguments.emptyElementStyle.connectorRadius,
        Arguments.emptyElementStyle.cornerRadius);

    PrintElementDefinition(os,
        "InputElement",
        Arguments.emptyElementLayout.width,
        Arguments.emptyElementLayout.height,
        Arguments.emptyElementStyle.connectorRadius,
        Arguments.emptyElementStyle.cornerRadius,
        false);

    os << "        </defs>\n\n";

    // print layer by layer
    double layerTop = Arguments.mapLayout.verticalMargin;
    std::vector<LayerLayout> layouts;

    for (uint64 layerIndex = 0; layerIndex < _layers.size(); ++layerIndex)
    {
        auto printableLayer = GetLayer<PrintableLayer>(layerIndex);
        auto layout = printableLayer->Print(os, Arguments.mapLayout.horizontalMargin, layerTop, layerIndex, Arguments);
        layerTop += layout.GetHeight() + Arguments.mapLayout.verticalSpacing;
        os << std::endl;

        // print edges
        if (layerIndex > 0) // skip input layer
        {
            uint64 layerSize = _layers[layerIndex]->Size();
            for (uint64 column = 0; column<layerSize; ++column)
            {
                if (!layout.IsHidden(column)) // if output is hidden, hide edge
                {
                    auto inputCoordinates = _layers[layerIndex]->GetInputCoordinates(column);
                    while (inputCoordinates.IsValid()) // foreach incoming edge
                    {
                        auto coordinate = inputCoordinates.Get();
                        const auto& inputLayout = layouts[coordinate.GetLayerIndex()];
                        if (!inputLayout.IsHidden(coordinate.GetElementIndex())) // if input is hidden, hide edge
                        {
                            SvgEdge(os, 2, inputLayout.GetOutputPoint(coordinate.GetElementIndex()), layout.GetInputPoint(column), Arguments.edgeStyle.flattness);
                        }

                        // on to the next input
                        inputCoordinates.Next();
                    }
                }
            }
        }

        // add the current layer's layout to the list of layouts
        layouts.push_back(std::move(layout));

        os << std::endl;
    }

    os << "\n    </Svg>\n\n<html>\n<body>\n";
}

void PrintableMap::Deserialize(utilities::JsonSerializer & serializer)
{
    serializer.Read("layers", _layers, PrintableMap::DeserializeLayers);
}

void PrintableMap::DeserializeLayers(utilities::JsonSerializer & serializer, std::shared_ptr<layers::Layer>& up)
{
    //auto type = serializer.Read<std::string>("_type");
    //auto version = serializer.Read<int>("_version");

    //if (type == "Input")
    //{
    //    auto upZero = std::make_shared<PrintableInput>();
    //    upZero->Deserialize(serializer, version);
    //    up = upZero;
    //}
    //else if (type == "Scale")
    //{
    //    auto upScale = std::make_shared<PrintableCoordinatewise>(layers::Layer::Type::scale);
    //    upScale->Deserialize(serializer, version);
    //    up = upScale;
    //}
    //else if (type == "Shift")
    //{
    //    auto upCoordinatewise = std::make_shared<PrintableCoordinatewise>(layers::Layer::Type::shift);
    //    upCoordinatewise->Deserialize(serializer, version);
    //    up = upCoordinatewise;
    //}
    //else if (type == "Sum")
    //{
    //    auto upSum = std::make_shared<PrintableSum>();
    //    upSum->Deserialize(serializer, version);
    //    up = upSum;
    //}
    //else
    //{
    //    throw std::runtime_error("unidentified type in map file: " + type);
    //}
}

